/* ****************************************************************************
 * link/usage/upload_and_run.c -- upload and run an executable.
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

/**
 *	casio_upload_and_run:
 *	Upload and run and executable.
 *
 *	@arg	handle		the link handle.
 *	@arg	buffer		the buffer.
 *	@arg	size		the update.exe size.
 *	@arg	loadaddr	the load address.
 *	@arg	straddr		the start address.
 *	@arg	disp		the progress displaying function.
 *	@arg	dcookie		the display callback cookie.
 *	@return				the error code (0 if ok).
 */

int casio_upload_and_run(casio_link_t *handle, casio_stream_t *buffer,
	casio_off_t size, unsigned long loadaddr, unsigned long straddr,
	casio_link_progress_t *disp, void *dcookie)
{
	int err, buf_err;

	/* Make checks. */
	chk_handle(handle);
	chk_seven(handle);
	chk_active(handle);

	/* Check the size. */
	if (size < 0) {
		buf_err = casio_seek(buffer, 0, CASIO_SEEK_END);
		if (buf_err) return (casio_error_noseek);

		size = casio_tell(buffer);
		buf_err = casio_seek(buffer, 0, CASIO_SEEK_SET);
		if (buf_err) return (casio_error_noseek);

		msg((ll_info, "Calculated file size is %lu.", size));
	}

	/* Send the command. */
	msg((ll_info, "Sending the up&run command."));
	err = casio_seven_send_cmdosu_upandrun(handle, size, loadaddr, straddr);
	if (err) return (err);
	else if (response.casio_seven_packet_type == casio_seven_type_nak
	 && response.casio_seven_packet_code == casio_seven_err_other) {
		msg((ll_fatal, "0x56 is not implemented... Oo"));
		return (casio_error_command);
	} else if (response.casio_seven_packet_type != casio_seven_type_ack) {
		msg((ll_fatal, "Response wasn't ack or nak...?!"));
		return (casio_error_unknown);
	}

	/* Upload data. */
	return (casio_seven_send_buffer(handle, buffer, size, 0, disp, dcookie));
}

#ifndef LIBCASIO_DISABLED_FILE
/**
 *	casio_upload_and_run_file:
 *	Upload and run, FILE version.
 *
 *	@arg	handle		the link handle.
 *	@arg	file		the program (FILE*).
 *	@arg	loadaddr	the load address.
 *	@arg	straddr		the start address.
 *	@arg	disp		the display callback.
 *	@arg	dcookie		the display callback cookie.
 *	@return				the error code (0 if ok).
 */

int casio_upload_and_run_file(casio_link_t *handle, FILE *file,
	unsigned long loadaddr, unsigned long straddr,
	casio_link_progress_t *disp, void *dcookie)
{
	int err, buf_err; casio_stream_t *buffer = NULL;

	/* Prepare the buffer. */
	err = casio_open_stream_file(&buffer, file, file, 0, 0);
	if (err) return (err);

	/* Send the file. */
	err = casio_upload_and_run(handle, buffer, -1,
		loadaddr, straddr, disp, dcookie);
	buf_err = casio_close(buffer);
	return (err ? err : buf_err);
}
#endif
