/* ****************************************************************************
 * link/seven/send.c -- prepare and send a packet.
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
 * These functions are the one used behind the others to send a packet.
 * ************************************************************************* */
#include "../link.h"
#include <stdio.h>
#include <string.h>
#define checksub8(BUF, SIZE) \
	casio_checksum_sub(&((char*)(BUF))[1], (SIZE) - 3, 0)

/* Internal macros. */
#define buffer \
	handle->casio_link_send_buffers[handle->casio_link_flags \
		& casio_linkflag_sendalt]
#define buffer_size \
	handle->casio_link_send_buffers_size[handle->casio_link_flags \
		& casio_linkflag_sendalt]
#define switch_buffer() \
	if (handle->casio_link_flags & casio_linkflag_shifted) \
		handle->casio_link_flags = \
			( handle->casio_link_flags & ~casio_linkflag_sendalt) | \
			(~handle->casio_link_flags &  casio_linkflag_sendalt);

/* ---
 * Main functions.
 * --- */

/**
 *	casio_seven_send_buf:
 *	Send a buffer.
 *
 *	@arg	handle		the link handle
 *	@arg	buf			the buffer to send (NULL if normal buffer)
 *	@arg	bufsize		the buffer size
 *	@arg	resp		check response (is yes for 99.9% of the cases)
 *	@return				if worked (0 if none)
 */

CASIO_LOCAL int casio_seven_send_buf(casio_link_t *handle,
	const unsigned char *buf, size_t bufsize, int resp)
{
	int err; int wasresend = 0, resp_err = 0, retries = 3;

	/* check if handler is initialized */
	if (!handle) return (casio_error_init);

	/* check if user wants to shift
	 *	with hack: if `buf` is non-NULL, we simply want to send
	 *  a custom packet */
	if (!buf && !resp) {
		if (handle->casio_link_flags & casio_linkflag_shifted)
			return (casio_error_shift);
		else handle->casio_link_flags |= casio_linkflag_shifted;
	}

	/* get buffer */
	if (!buf) {
		buf = buffer;
		bufsize = buffer_size;
	}

	/* sending loop */
	while (1) {
		/* log resend request */
		if (wasresend) {
			if (--retries < 0) {
				msg((ll_error,
					"Three retries in a row? Something is wrong."));
				return (casio_error_damned);
			}

			switch_buffer();
			buf = buffer; bufsize = buffer_size;
			msg((ll_warn, "resend request was received, resend it goes"));
		}

		/* send prepared packet */
		ssize_t ssize = casio_write(handle->casio_link_stream, buf, bufsize);
		err = ssize < 0 ? -ssize : 0;
		if (err) return (err);

		/* set wasreset for logging */
		wasresend = 1;

		/* check if we want an answer */
		if (!resp) break;
		if ((resp_err = casio_seven_receive(handle, 1)))
			break;
		if (response.casio_seven_packet_type == casio_seven_type_nak
		 && response.casio_seven_packet_code == casio_seven_err_resend)
			continue;
		break;
	}

	/* packet sending is finished */
	if (!resp_err && handle->casio_link_curr_type == casio_seven_type_swp)
		handle->casio_link_flags &= ~casio_linkflag_active;
	return (resp_err);
}

/**
 *	casio_seven_send_basic:
 *	Send a non-extended packet.
 *
 *	@arg	handle		the link handle
 *	@arg	type		the packet type
 *	@arg	subtype		the packet subtype
 *	@arg	resp		check response (is yes for 99.9% of the cases)
 *	@return				if worked (0 if none)
 */

int CASIO_EXPORT casio_seven_send_basic(casio_link_t *handle,
	casio_seven_type_t type, unsigned int subtype, int resp)
{
	/* change buffer and prepare packet */
	switch_buffer();
	buffer[0] = (unsigned char)type;
	casio_putascii(&buffer[1], subtype, 2);
	buffer[3] = '0';
	casio_putascii(&buffer[4], checksub8(buffer, 6), 2);
	buffer_size = 6;

	/* log packet */
	msg((ll_info, "sending the following basic packet :"));
	mem((ll_info, buffer, buffer_size));

	/* send it */
	handle->casio_link_curr_type = type;
	return (casio_seven_send_buf(handle, NULL, 0, resp));
}

/**
 *	casio_seven_send_ext:
 *	Send an extended packet.
 *
 *	@arg	handle		the link handle
 *	@arg	type		the packet type
 *	@arg	subtype		the packet subtype
 *	@arg	data		the packet data
 *	@arg	size		the packet data size
 *	@arg	resp		check response (is yes for 99.9% of the cases)
 *	@return				if it worked
 */

int CASIO_EXPORT casio_seven_send_ext(casio_link_t *handle,
	casio_seven_type_t type, unsigned int subtype,
	const void *data, unsigned int size, int resp)
{
	/* check if should be a binary zero at end of packet */
	int binary_zero = (type == casio_seven_type_cmd
		&& subtype == casio_seven_cmdosu_upandrun);

	/* change buffer and prepare packet */
	switch_buffer();
	/* - first infos - */
	buffer[0] = (unsigned char)type;
	casio_putascii(&buffer[1], subtype, 2);
	buffer[3] = '1';
	/* - data - */
	size = casio_seven_encoderaw(&buffer[8], data, size);
	casio_putascii(&buffer[4], size, 4);
	/* - checksum - */
	casio_putascii(&buffer[8 + size], checksub8(buffer, 8 + size + 2), 2);
	buffer_size = 8 + size + 2;
	if (binary_zero) { buffer[buffer_size] = 0; buffer_size++; }

	/* log packet */
	msg((ll_info, "sending the following extended packet :"));
	mem((ll_info, buffer, buffer_size));

	/* send it */
	handle->casio_link_curr_type = type;
	return (casio_seven_send_buf(handle, NULL, 0, resp));
}

/**
 *	casio_seven_send_again:
 *	Send the last packet again.
 *
 *	Useful for when resending wasn't managed in one of the previous functions.
 *	Only used for unshifting right now.
 *
 *	@arg	handle		the link handle
 *	@return				if it worked
 */

int CASIO_EXPORT casio_seven_send_again(casio_link_t *handle)
{
	/* log packet */
	msg((ll_info, "sending again the following packet :"));
	mem((ll_info, buffer, buffer_size));

	/* send it */
	return (casio_seven_send_buf(handle, NULL, 0, 1));
}

/* ---
 * Special packets.
 * --- */

/**
 *	casio_seven_send_err_resend:
 *	Resend the resend error.
 *
 *	@arg	handle		the handle to send the resend error for.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_seven_send_err_resend(casio_link_t *handle)
{
	static const unsigned char resend_buf[] =
		{casio_seven_type_nak, '0', '1', '0', '6', 'F', 0};

	/* send packet */
	msg((ll_info, "sending resend packet:"));
	mem((ll_info, resend_buf, 6));
	return (casio_seven_send_buf(handle, resend_buf, 6, 0));
}

/**
 *	casio_seven_send_timeout_check:
 *	Send a timeout check.
 *
 *	@arg	handle		the handle to send the timeout check for.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_seven_send_timeout_check(casio_link_t *handle)
{
	static const unsigned char check_buf[] =
		{casio_seven_type_chk, '0', '1', '0', '6', 'F', 0};

	/* send packet */
	msg((ll_info, "sending timeout check packet:"));
	mem((ll_info, check_buf, 6));
	return (casio_seven_send_buf(handle, check_buf, 6, 0));
}
