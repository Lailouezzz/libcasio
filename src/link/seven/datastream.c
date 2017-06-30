/* ****************************************************************************
 * link/seven/datastream.c -- data exchanging stream.
 * Copyright (C) 2016-2017 Thomas "Cakeisalie5" Touhey <thomas@touhey.fr>
 *
 * This file is part of libcasio.
 * libcasio is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3.0 of the License,
 * or (at your option) any later version.
 *
 * libcasio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libcasio; if not, see <http://www.gnu.org/licenses/>.
 *
 * This stream is useful when you want to encode and send on the fly, without
 * having to use temporary files or god knows what else.
 * ************************************************************************* */
#include "data.h"
#include "../usage/usage.h"
#define BUFSIZE CASIO_SEVEN_MAX_RAWDATA_SIZE

/* Cookie structure. */
typedef struct {
	int _faulty, _read;

	casio_link_t *_link;
	casio_link_progress_t *_disp;
	void *_disp_cookie;
	unsigned int _id, _total;
	unsigned int _lastsize; /* last packet size */

	/* buffer management */
	size_t _pos; /* position in the buffer */
	unsigned char _reserved[8];
	unsigned char _current[BUFSIZE];
} seven_data_cookie_t;
/* ************************************************************************* */
/*  Callbacks                                                                */
/* ************************************************************************* */
/**
 *	casio_seven_data_read:
 *	Read data from the calculator, using Protocol 7.00 data flow.
 *
 *	@arg	cookie		the cookie.
 *	@arg	data		the data to read.
 *	@arg	size		the size of the data to read.
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL int casio_seven_data_read(seven_data_cookie_t *cookie,
	unsigned char *data, size_t size)
{
	int err; size_t tocopy;
	casio_link_t *handle = cookie->_link;
	unsigned int lastsize;

	/* Check if the stream is faulty. */
	if (cookie->_faulty)
		return (casio_error_noread);

	/* Empty the current buffer. */
	tocopy = cookie->_lastsize;
	if (size < tocopy) tocopy = size;
	if (tocopy) {
		memcpy(data, &cookie->_current[cookie->_pos], tocopy);
		cookie->_pos += tocopy;
		data += tocopy; size -= tocopy;

		if (!size) return (0);
	}

	/* Check if we have already finished. */
	if (cookie->_total && cookie->_id == cookie->_total)
		return (casio_error_eof);

	/* Receive packets. */
	while (size) {
		/* Send the ack and get the answer. */
		err = casio_seven_send_ack(handle, 1);
		if (err) goto fail;
		if (response.casio_seven_packet_type != casio_seven_type_data) {
			msg((ll_error, "Packet wasn't a data packet, wtf?"));
			err = casio_error_unknown;
			goto fail;
		}

		/* Check the total. */
		if (!cookie->_total)
			cookie->_total = response.casio_seven_packet_total;
		else if (cookie->_total != response.casio_seven_packet_total) {
			msg((ll_error,
				"Packet total did not correspond to the cached one...?"));
			err = casio_error_unknown;
			goto fail;
		}

		/* Check the ID. */
		cookie->_id++;
		if (cookie->_id != response.casio_seven_packet_id) {
			msg((ll_error, "Non-consecutive data packets."));
			err = casio_error_unknown;
			goto fail;
		}

		/* Increment and copy. */
		lastsize = response.casio_seven_packet_data_size;
		if (size >= lastsize) {
			memcpy(data, response.casio_seven_packet_data, lastsize);
			data += lastsize; size -= lastsize;
			continue;
		}

		/* Copy to the data, keep the rest! */
		cookie->_lastsize = lastsize;
		memcpy(data, response.casio_seven_packet_data, size);
		memcpy(&cookie->_current[size],
			&response.casio_seven_packet_data[size], lastsize - size);
		return (0);
	}

	return (0);
fail:
	/* XXX: tell the distant device we have a problem? */
	cookie->_faulty = 1;
	return (err);
}

/**
 *	casio_seven_data_write:
 *	Send data to the calculator, using Protocol 7.00 data flow.
 *
 *	@arg	cookie		the cookie.
 *	@arg	data		the data to write.
 *	@arg	size		the size of the data to write.
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL int casio_seven_data_write(seven_data_cookie_t *cookie,
	const unsigned char *data, size_t size)
{
	int err; size_t tocopy, lastlimit;
	casio_link_t *handle = cookie->_link;

	/* Check if the stream is faulty, or if we have already finished. */
	if (cookie->_faulty)
		return (casio_error_nowrite);
	if (cookie->_id > cookie->_total)
		return (casio_error_eof);

	/* Current packet management. */
	if (cookie->_pos || size < BUFSIZE) {
		/* Fill the current packet. */
		tocopy = size;
		if (cookie->_pos + tocopy < cookie->_pos /* overflow */
		 || cookie->_pos + tocopy > BUFSIZE)
			tocopy = BUFSIZE - 1 - cookie->_pos;
		memcpy(cookie->_current, data, tocopy);
		data += tocopy; size -= tocopy;
		cookie->_pos += tocopy;

		/* Check if the current packet is full. */
		if (cookie->_id == cookie->_total) {
			if (cookie->_pos < cookie->_lastsize)
				return (0);
		} else if (cookie->_pos < BUFSIZE - 1)
			return (0);

		/* Send the packet. */
		if (cookie->_id == 1 && cookie->_disp)
			(*cookie->_disp)(cookie->_disp_cookie, 1, 0);
		err = casio_seven_send_quick_data_packet(handle, cookie->_total,
			cookie->_id, &cookie->_reserved, cookie->_pos, 1);
		if (err) goto fail;
		if (cookie->_disp) (*cookie->_disp)(cookie->_disp_cookie,
			cookie->_id, cookie->_total);
		cookie->_id++; cookie->_pos = 0;
	}

	/* Intermediate packets. */
	while (cookie->_id < cookie->_total && size >= BUFSIZE) {
		memcpy(cookie->_current, data, BUFSIZE);
		err = casio_seven_send_quick_data_packet(handle,
			cookie->_total, cookie->_id, &cookie->_reserved, BUFSIZE, 1);
		if (err) goto fail;
		if (cookie->_disp) (*cookie->_disp)(cookie->_disp_cookie,
			cookie->_id, cookie->_total);
		data += BUFSIZE; size -= BUFSIZE; cookie->_id++;
	}

	/* Copy the last bytes of the call. */
	lastlimit = cookie->_id == cookie->_total ? cookie->_lastsize : BUFSIZE;
	tocopy = lastlimit; if (tocopy > size) tocopy = size;
	memcpy(cookie->_current, data, tocopy);
	cookie->_pos = tocopy; size -= tocopy;

	/* Send the last packet if required. */
	if (tocopy == lastlimit) {
		err = casio_seven_send_quick_data_packet(handle,
			cookie->_total, cookie->_id, &cookie->_reserved, cookie->_pos, 1);
		if (err) goto fail;
		if (cookie->_disp) (*cookie->_disp)(cookie->_disp_cookie,
			cookie->_id, cookie->_total);
		cookie->_id++; cookie->_pos = 0;
	}

	/* Check if there are still some bytes left
	 * (bytes after the last packet, means the announced size
	 *  was too small). */
	if (size) return (casio_error_nowrite);

	/* Everything went well! :) */
	return (0);
fail:
	/* XXX: tell the distant device we have a problem? */
	cookie->_faulty = 1;
	return (err);
}

/**
 *	casio_seven_data_close:
 *	Close the data stream.
 *
 *	@arg	cookie		the cookie.
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL int casio_seven_data_close(seven_data_cookie_t *cookie)
{
	int err = 0; casio_link_t *handle = cookie->_link;

	/* Check if there is some data left to receive. */
	if (cookie->_read) {
		if (!cookie->_total) {
			err = casio_seven_send_ack(handle, 1);
			if (err) goto fail;

			cookie->_id = response.casio_seven_packet_id;
			cookie->_total = response.casio_seven_packet_total;
			if (cookie->_id != 1) { err = casio_error_unknown; goto fail; }
		}

		while (cookie->_id < cookie->_total) {
			err = casio_seven_send_ack(handle, 1);
			if (err) goto fail;

			if (cookie->_id + 1 != response.casio_seven_packet_id
			 || cookie->_total != response.casio_seven_packet_total) {
				err = casio_error_unknown;
				goto fail;
			}
			cookie->_id++;
		}
	}

	/* Check if there is some data left to send. */
	if (!cookie->_read && cookie->_id <= cookie->_total) {
		size_t left; unsigned char zeroes[BUFSIZE];

		/* Intermediate packets. */
		left = (cookie->_total - cookie->_id - 1) * BUFSIZE;

		/* Current and last packet. */
		if (cookie->_id == cookie->_total)
			left += cookie->_lastsize - cookie->_pos; /* current & end */
		else {
			left += BUFSIZE - cookie->_pos; /* current */
			left += cookie->_lastsize; /* end */
		}

		/* Send the zeroes.
		 * If the stream gets faulty in the middle of this, fuck it and
		 * go directly to the end. */
		memset(zeroes, 0, BUFSIZE);
		for (; left >= BUFSIZE; left -= BUFSIZE) {
			if ((err = casio_seven_data_write(cookie, zeroes, BUFSIZE)))
				goto fail;
		}

		err = casio_seven_data_write(cookie, zeroes, left);
	}

	err = 0;
fail:
	casio_free(cookie);
	return (err);
}
/* ************************************************************************* */
/*  Opening functions                                                        */
/* ************************************************************************* */
CASIO_LOCAL const casio_streamfuncs_t seven_data_callbacks =
casio_stream_callbacks_for_virtual(casio_seven_data_close,
	casio_seven_data_read, casio_seven_data_write, NULL);

/**
 *	casio_seven_open_data_stream:
 *	Open a data stream.
 *
 *	@arg	stream		the stream to make.
 *	@arg	link		the link with which to send the data.
 *	@arg	size		the total size of the data to transmit.
 *	@arg	disp		the display callback.
 *	@arg	dcookie		the display callback cookie.
 *	@return				the error (0 if ok).
 */

int CASIO_EXPORT casio_seven_open_data_stream(casio_stream_t **stream,
	casio_link_t *link, casio_off_t size, casio_link_progress_t *disp,
	void *dcookie)
{
	seven_data_cookie_t *cookie = NULL;
	casio_openmode_t mode;

	/* make checks */
	chk_handle(link);
	chk_seven(link);

	/* make the cookie */
	cookie = casio_alloc(1, sizeof(seven_data_cookie_t));
	if (!cookie) return (casio_error_alloc);

	/* initialize the cookie and mode */
	cookie->_faulty = 0;
	cookie->_link = link;
	cookie->_pos = 0;
	cookie->_disp = disp;
	cookie->_disp_cookie = dcookie;
	if (link->casio_link_flags & casio_linkflag_active) {
		msg((ll_info, "The data stream is a write one."));
		mode = CASIO_OPENMODE_WRITE;

		cookie->_read = 0;
		cookie->_id = 1;
		cookie->_total = (unsigned int)(size / BUFSIZE) + !!cookie->_lastsize;
		cookie->_lastsize = (unsigned int)(size % BUFSIZE);
		if (!cookie->_lastsize) cookie->_lastsize = BUFSIZE;
	} else {
		msg((ll_info, "The data stream is a read one."));
		mode = CASIO_OPENMODE_READ;

		cookie->_read = 1;
		cookie->_id = 0;
		cookie->_total = 0;
		cookie->_lastsize = 0;
	}

	/* initialize the stream */
	return (casio_open(stream, mode, cookie, &seven_data_callbacks));
}
