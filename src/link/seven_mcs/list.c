/* ****************************************************************************
 * link/seven_mcs/list.c -- list files on a protocol seven main memory.
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
	casio_mcslist_t *_mcslist;
	void            *_mcookie;
};

/**
 *	file_info:
 *	The file information callback.
 *
 *	@arg	cookie		the cookie.
 *	@arg	handle		the link handle.
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL int file_info(struct thecookie *cookie, casio_link_t *handle)
{
	casio_mcshead_t head;

	/* Copy the raw information. */
	head.casio_mcshead_flags = casio_mcsfor_mcs;
	head.casio_mcshead_size = response.casio_seven_packet_filesize;
	head.casio_mcshead_rawtype = response.casio_seven_packet_mcstype;
	strncpy(head.casio_mcshead_dirname,
		response.casio_seven_packet_args[0], 8);
	head.casio_mcshead_dirname[8] = 0;
	strncpy(head.casio_mcshead_name,
		response.casio_seven_packet_args[1], 12);
	head.casio_mcshead_name[12] = 0;
	strncpy(head.casio_mcshead_group,
		response.casio_seven_packet_args[2], 16);
	head.casio_mcshead_group[16] = 0;

	/* Try to make abstract information out of the raw information. */
	casio_correct_mcshead(&head, 0);

	/* Return this head to the user, and end the command response. */
	(*cookie->_mcslist)(cookie->_mcookie, &head);
	return (0);
}

/**
 *	casio_sevenmcs_list:
 *	List a Protocol 7.00 MCS.
 *
 *	@arg	cookie		the cookie.
 *	@arg	mcslist		the listing callback.
 *	@arg	mcookie		the listing callback cookie.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_sevenmcs_list(sevenmcs_t *cookie,
	casio_mcslist_t *mcslist, void *mcookie)
{
	int err; casio_link_t *handle = cookie->sevenmcs_link;
	struct thecookie thecookie;

	msg((ll_info, "Sending the file info transfer all request."));
	err = casio_seven_send_cmdmcs_reqallinfo(handle);
	if (err) return (err);
	if (response.casio_seven_packet_type != casio_seven_type_ack)
		return (casio_error_unknown);

	/* Prepare the cookie. */
	thecookie._mcslist = mcslist;
	thecookie._mcookie = mcookie;

	msg((ll_info, "Preparing the callbacks and running the server."));
	memset(handle->casio_link_seven_callbacks, 0,
		256 * sizeof(casio_seven_server_func_t*));
	handle->casio_link_seven_callbacks[casio_seven_cmdmcs_fileinfo] =
		(casio_seven_server_func_t*)&file_info;
	err = casio_seven_serve(handle, handle->casio_link_seven_callbacks,
		&thecookie);
	return (err);
}
