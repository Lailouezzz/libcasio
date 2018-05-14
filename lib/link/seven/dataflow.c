/* ****************************************************************************
 * link/seven/dataflow.c -- data exchanging flows.
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
 * These functions are about sending and receiving data buffers.
 * ************************************************************************* */
#include "data.h"
#define ONEBUF CASIO_SEVEN_MAX_RAWDATA_SIZE
#define BUFNUM 64
#define BUFSIZE (ONEBUF * BUFNUM)

/* ---
 * Buffer version.
 * --- */

/**
 *	casio_seven_send_buffer:
 *	Part of the packet flows where data is sent.
 *
 *	@arg	handle		the link handle
 *	@arg	buffer		the buffer to read from
 *	@arg	size		the buffer size
 *	@arg	shift		should shift?
 *	@arg	disp		the display callback.
 *	@arg	dcookie		the display callback cookie.
 *	@return				the error (0 if ok)
 */

int CASIO_EXPORT casio_seven_send_buffer(casio_link_t *handle,
	casio_stream_t *buffer, casio_off_t size, int shift,
	casio_link_progress_t *disp, void *dcookie)
{
	int err, stream_err, resp; unsigned char buf[8 + BUFSIZE];
	unsigned int id, total, lastsize, datasize;

	/* Get the vars set up. */
	lastsize = size % ONEBUF;
	total = (unsigned int)(size / ONEBUF) + !!lastsize;
	if (!lastsize) lastsize = ONEBUF;

	/* Initialize the progress displayer. */
	if (disp) (*disp)(dcookie, 1, 0);

	/* Data sending loop. */
	datasize = total == 1 ? lastsize : ONEBUF;
	resp = !shift;
	for (id = 1; id <= total;) {
		unsigned char *p = buf;
		size_t toread;

		/* Read the big block. */
		toread = min(BUFSIZE, size);
		err = casio_read(buffer, buf + 8, toread);
		if (err) return (casio_error_noread);
		size -= toread;

		/* Send each block. */
		for (; toread && id <= total; id++) {
			if (id == total)
				datasize = lastsize;

			/* Send the packet. */
			msg((ll_info, "Sending packet %u/%u (%uo)", id, total, datasize));
			stream_err = casio_seven_send_quick_data_packet(handle, total, id,
				p, datasize, resp);
			if (stream_err) { err = casio_error_noread; goto fail; }
			p += datasize; toread -= datasize; resp = 1;

			/* Display the progress. */
			if (disp) (*disp)(dcookie, id, total);

			/* Check the response. */
			if (response.casio_seven_packet_type != casio_seven_type_ack) {
				msg((ll_error, "Calculator didn't send ACK, wtf?"));
				err = casio_error_unknown;
				goto fail;
			}
		}
	}

	/* Unshift. */
	if (handle->casio_link_flags & casio_linkflag_shifted)
		err = casio_seven_unshift(handle);

fail:
	return (err);
}

/**
 *	casio_seven_get_buffer:
 *	Part of the packet flows where data is received.
 *
 *	Do not send an ack before going in this function, it will do it.
 *
 *	@arg	handle		the link handle
 *	@arg	buffer		the buffer to write to.
 *	@arg	shift		should shift?
 *	@arg	disp		the display callback.
 *	@arg	dcookie		the display callback cookie.
 *	@return				the error (0 if ok)
 */

int CASIO_EXPORT casio_seven_get_buffer(casio_link_t *handle,
	casio_stream_t *buffer, casio_off_t size, int shift,
	casio_link_progress_t *disp, void *dcookie)
{
	int err, buf_err; unsigned char buf[BUFSIZE];
	unsigned char *p = buf; size_t ps = 0;

	/* Initialize the progress displayer. */
	if (disp) (*disp)(dcookie, 1, 0);

	/* If shifting is enabled, send an ACK to double the first one.
	 * This has proven to be very unstable. You really shouldn't use this
	 * in a non-experimental program. */
	if (shift) casio_seven_send_ack(handle, 0);

	/* Main receiving loop. */
	while (size) {
		/* Send the ACK. */
		err = casio_seven_send_ack(handle, 1);
		if (err) {
			msg((ll_error, "Couldn't send the ACK."));
			goto fail;
		}

		/* If the response is not data, wtf? */
		if (response.casio_seven_packet_type != casio_seven_type_data) {
			msg((ll_error, "Packet type wasn't data, wtf?"));
			err = casio_error_unknown;
			goto fail;
		}

		/* Display the progress. */
		if (disp) (*disp)(dcookie, response.casio_seven_packet_id,
			response.casio_seven_packet_total);

		/* Check if there is an overflow. */
		if ((casio_off_t)response.casio_seven_packet_data_size > size)
			response.casio_seven_packet_data_size = (unsigned int)size;

		/* Copy the data. */
		memcpy(p, response.casio_seven_packet_data,
			response.casio_seven_packet_data_size);
		p    += response.casio_seven_packet_data_size;
		ps   += response.casio_seven_packet_data_size;
		size -= response.casio_seven_packet_data_size;

		/* Check if we should empty the buffer. */
		if (ps >= BUFSIZE - CASIO_SEVEN_MAX_RAWDATA_SIZE || !size) {
			msg((ll_info, "buffer too full, should be emptied"));

			/* Empty the buffer in the stream. */
			buf_err = casio_write(buffer, buf, ps);
			if (buf_err) { err = casio_error_nowrite; goto fail; }

			/* Reset the pointer. */
			p = buf; ps = 0;
		}
	}

	/* Send the last ACK. */
	err = casio_seven_send_ack(handle, 1);
	if (err) {
		msg((ll_error, "Unable to send the last ACK."));
		goto fail;
	}

	/* Unshift if needed -- FIXME? */
	if (shift) {
		err = casio_seven_unshift(handle);
		if (err) goto fail;
	}

	/* end */
	err = 0;
fail:
	return (err);
}

/* ---
 * Data version.
 * --- */

/**
 *	casio_seven_send_data:
 *	Part of the packet flow where data is sent - buffer version.
 *
 *	@arg	handle		the link handle.
 *	@arg	vbuf		the buffer.
 *	@arg	size		the number of bytes of the buffer to send.
 *	@arg	shift		should shift?
 *	@arg	disp		the display function.
 *	@arg	dcookie		the display function cookie.
 *	@return				the error code (0 if ok)
 */

int CASIO_EXPORT casio_seven_send_data(casio_link_t *handle, const void *vbuf,
	casio_off_t size, int shift, casio_link_progress_t *disp,
	void *dcookie)
{
	int err = 0;
	casio_stream_t *stream;

	err = casio_open_memory(&stream, vbuf, size);
	if (err) return (err);
	err = casio_seven_send_buffer(handle, stream, size, shift, disp, dcookie);
	if (err) { casio_close(stream); return (err); }
	return (casio_close(stream));
}

/**
 *	casio_seven_get_data:
 *	Part of the packet flow where data is received - buffer version.
 *
 *	@arg	handle		the link handle.
 *	@arg	vbuf		the final buffer (uncasted)
 *	@arg	size		the buffer size (and size to receive)
 *	@arg	shift		should shift?
 *	@arg	disp		the display function.
 *	@arg	dcookie		the display function callback.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_seven_get_data(casio_link_t *handle, void *vbuf,
	casio_off_t size, int shift, casio_link_progress_t disp, void *dcookie)
{
	int err = 0;
	casio_stream_t *stream;

	err = casio_open_memory(&stream, vbuf, size);
	if (err) return (err);
	err = casio_seven_get_buffer(handle, stream, size, shift, disp, dcookie);
	if (err) { casio_close(stream); return (err); }
	return (casio_close(stream));
}
