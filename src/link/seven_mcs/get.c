/* ****************************************************************************
 * link/seven_mcs/get.c -- get a file from a protocol seven main memory.
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
#include "seven_mcs.h"

struct thecookie {
	int                    _called;
	int                    _err;
	casio_mcshead_t       *_head;
	casio_mcsfile_t      **_mcsfile;
	casio_link_progress_t *_disp;
	void                  *_disp_cookie;
};

/**
 *	get_file:
 *	Get the main memory file.
 *
 *	@arg	cookie		the cookie (uncasted).
 *	@arg	handle		the link handle.
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL int get_file(struct thecookie *cookie, casio_link_t *handle)
{
	casio_stream_t  *data_stream = NULL;
	casio_mcshead_t *head        = cookie->_head;

	/* Check if the callback is already called. */
	if (cookie->_called)
		return (casio_error_unknown);
	cookie->_called = 1;

	/* Correct the head. */
	head->casio_mcshead_size = response.casio_seven_packet_filesize;

	/* Make the data stream. */
	cookie->_err = casio_seven_open_data_stream(&data_stream, handle, 0,
		cookie->_disp, cookie->_disp_cookie);
	if (cookie->_err) return (casio_error_unknown);

	/* Decode the MCS file. */
	cookie->_err = casio_decode_mcsfile(cookie->_mcsfile, head, data_stream);
	casio_close(data_stream);
	if (cookie->_err) return (casio_error_unknown);

	/* Send the final ack and return. */
	return (casio_seven_send_ack(handle, 1));
}

/**
 *	casio_sevenmcs_get:
 *	Get a file from a Protocol 7.00 main memory interface.
 *
 *	@arg	cookie		the interface cookie.
 *	@arg	mcsfile		the MCS file to get.
 *	@arg	mcshead		the MCS head.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_sevenmcs_get(sevenmcs_t *cookie,
	casio_mcsfile_t **mcsfile, casio_mcshead_t *mcshead)
{
	int err; casio_mcshead_t head;
	casio_link_t *handle = cookie->sevenmcs_link;
	struct thecookie ccookie;

	/* Correct the head. */
	msg((ll_info, "Correcting the head."));
	memcpy(&head, mcshead, sizeof(head));
	if (casio_correct_mcshead(&head, casio_mcsfor_mcs))
		return (casio_error_notfound);

	/* Send the command. */
	msg((ll_info, "Sending the file request."));
	err = casio_seven_send_cmd_data(handle,
		casio_seven_cmdmcs_reqfile, 0, head.casio_mcshead_rawtype, 0,
		head.casio_mcshead_dirname, head.casio_mcshead_name,
		head.casio_mcshead_group, NULL, NULL, NULL);
	if (err) return (err);

	/* Check the answer. */
	if (response.casio_seven_packet_type == casio_seven_type_nak
	 && response.casio_seven_packet_code == casio_seven_err_other)
		return (casio_error_notfound);
	else if (response.casio_seven_packet_type != casio_seven_type_ack)
		return (casio_error_unknown);

	/* Prepare the cookie. */
	ccookie._called      = 0;
	ccookie._err         = 0;
	ccookie._mcsfile     = mcsfile;
	ccookie._head        = &head;
	ccookie._disp        = NULL;
	ccookie._disp_cookie = NULL;

	msg((ll_info, "Preparing the callbacks and running the server."));
	memset(handle->casio_link_seven_callbacks, 0,
		256 * sizeof(casio_seven_server_func_t*));
	handle->casio_link_seven_callbacks[casio_seven_cmdmcs_sendfile] =
		(casio_seven_server_func_t*)&get_file;
	err = casio_seven_serve(handle, handle->casio_link_seven_callbacks,
		&ccookie);
	if (err) return (err);

	/* Check if the function was called. */
	return (ccookie._called ? ccookie._err : casio_error_unknown);
}
