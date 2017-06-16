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
#define BUFSIZE CASIO_SEVEN_MAX_RAWDATA_SIZE

/* Cookie structure. */
typedef struct {
	int _faulty;

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
 *	casio_seven_data_write:
 *	Send data to the calculator.
 *
 *	@arg	vcookie		the cookie (uncasted).
 *	@arg	data		the data to write.
 *	@arg	size		the size of the data to write.
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL int casio_seven_data_write(void *vcookie,
	const unsigned char *data, size_t size)
{
	seven_data_cookie_t *cookie = (void*)vcookie;
	int err; size_t tocopy, lastlimit;

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
		err = casio_seven_send_quick_data_packet(cookie->_link, cookie->_total,
			cookie->_id, &cookie->_reserved, cookie->_pos, 1);
		if (err) goto fail;
		if (cookie->_disp) (*cookie->_disp)(cookie->_disp_cookie,
			cookie->_id, cookie->_total);
		cookie->_id++; cookie->_pos = 0;
	}

	/* Intermediate packets. */
	while (cookie->_id < cookie->_total && size >= BUFSIZE) {
		memcpy(cookie->_current, data, BUFSIZE);
		err = casio_seven_send_quick_data_packet(cookie->_link,
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
		err = casio_seven_send_quick_data_packet(cookie->_link,
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
 *	@arg	vcookie		the cookie (uncasted).
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL int casio_seven_data_close(void *vcookie)
{
	seven_data_cookie_t *cookie = (void*)vcookie;

	/* Check if there is some data left to send. */
	if (cookie->_id <= cookie->_total) {
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
			if (casio_seven_data_write(vcookie, zeroes, BUFSIZE))
				goto end;
		}
		casio_seven_data_write(vcookie, zeroes, left);
	}

end:
	free(vcookie);
	return (0);
}
/* ************************************************************************* */
/*  Opening functions                                                        */
/* ************************************************************************* */
CASIO_LOCAL const casio_streamfuncs_t seven_data_callbacks =
casio_stream_callbacks_for_virtual(casio_seven_data_close,
	NULL, casio_seven_data_write, NULL);

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

	/* XXX: args checks */

	/* make the cookie */
	cookie = malloc(sizeof(cookie));
	if (!cookie) return (casio_error_alloc);
	cookie->_faulty = 0;
	cookie->_link = link;
	cookie->_disp = disp;
	cookie->_disp_cookie = dcookie;
	cookie->_id = 1;
	cookie->_lastsize = (unsigned int)(size % BUFSIZE);
	cookie->_total = (unsigned int)(size / BUFSIZE) + !!cookie->_lastsize;
	if (!cookie->_lastsize) cookie->_lastsize = BUFSIZE;
	cookie->_pos = 0;

	/* initialize the stream */
	return (casio_open(stream, CASIO_OPENMODE_WRITE, 0, cookie,
		&seven_data_callbacks));
}
