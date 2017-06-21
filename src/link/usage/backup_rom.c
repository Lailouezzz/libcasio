/* ****************************************************************************
 * link/usage/backup_rom.c -- backup the ROM.
 * Copyright (C) 2017 Thomas "Cakeisalie5" Touhey <thomas@touhey.fr>
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
 * ************************************************************************* */
#include "usage.h"

struct thecookie {
	int _called;
	casio_stream_t *_buffer;
	casio_link_progress_t *_disp;
	void *_disp_cookie;
};

/**
 *	get_rom:
 *	Get the ROM.
 *
 *	@arg	vcookie		the cookie (uncasted).
 *	@arg	handle		the link handle.
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL int get_rom(void *vcookie, casio_link_t *handle)
{
	struct thecookie *cookie = (void*)vcookie;

	/* Check if the callback is already called. */
	if (cookie->_called)
		return (casio_error_unknown);
	cookie->_called = 1;

	/* Receive the data. */
	return (casio_seven_get_buffer(handle, cookie->_buffer,
		handle->casio_link_info.casio_link_info_rom_capacity,
		0, cookie->_disp, cookie->_disp_cookie));
}

/**
 *	casio_backup_rom:
 *	Backup the ROM.
 *
 *	@arg	handle		the link handle.
 *	@arg	buffer		the buffer.
 *	@arg	disp		the progress displayer.
 *	@arg	dcookie		the progress displayer cookie.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_backup_rom(casio_link_t *handle, casio_stream_t *buffer,
	casio_link_progress_t *disp, void *dcookie)
{
	int err; struct thecookie cookie;

	/* Make the appropriate checks. */
	chk_handle(handle);
	chk_seven(handle);
	chk_active(handle);
	chk_bufwrite(buffer);

	/* Send command packets. */
	msg((ll_info, "Sending the command."));
	if ((err = casio_seven_send_cmdbak_reqrom(handle)))
		return (err);
	else if (response.casio_seven_packet_type == casio_seven_type_nak
	 && response.casio_seven_packet_code == casio_seven_err_other) {
		msg((ll_fatal, "Backup seems unsupported."));
		return (casio_error_command);
	} else if (response.casio_seven_packet_type != casio_seven_type_ack) {
		msg((ll_fatal, "Didn't receive an ACK or a NAK...?"));
		return (casio_error_unknown);
	}

	/* Prepare the cookie. */
	cookie._called = 0;
	cookie._buffer = buffer;
	cookie._disp = disp;
	cookie._disp_cookie = dcookie;

	msg((ll_info, "Preparing the callbacks and running the server."));
	memset(handle->casio_link_seven_callbacks, 0,
		256 * sizeof(casio_seven_server_func_t*));
	handle->casio_link_seven_callbacks[casio_seven_cmdbak_putrom] = get_rom;
	err = casio_seven_serve(handle, handle->casio_link_seven_callbacks,
		&cookie);
	if (err) return (err);

	/* Check if the function was called. */
	return (cookie._called ? 0 : casio_error_unknown);
}

#ifndef LIBCASIO_DISABLED_FILE
/**
 *	casio_backup_rom_file:
 *	Backup the ROM as a FILE.
 *
 *	@arg	handle		the link handle.
 *	@arg	file		the FILE.
 *	@arg	disp		the progress displayer.
 *	@arg	dcookie		the progress displayer callback.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_backup_rom_file(casio_link_t *handle, FILE *file,
	casio_link_progress_t *disp, void *dcookie)
{
	int err, buf_err;
	casio_stream_t *stream = NULL;

	/* Open the stream. */
	err = casio_open_stream_file(&stream, file, file, 0, 0);
	if (err) return (err);

	/* Send the file. */
	err = casio_backup_rom(handle, stream, disp, dcookie);
	buf_err = casio_close(stream);
	return (err ? err : buf_err);
}
#endif
