/* ****************************************************************************
 * link/seven_mcs/delete.c -- delete a file on a Protocol 7.00 main memory.
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

/**
 *	casio_sevenmcs_delete:
 *	Delete a file on the link main memory.
 *
 *	@arg	cookie		the link main memory cookie.
 *	@arg	mcshead		the head of the file to delete.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_sevenmcs_delete(sevenmcs_t *cookie,
	casio_mcshead_t *mcshead)
{
	int err; casio_link_t *handle = cookie->sevenmcs_link;
	casio_mcshead_t head;

	msg((ll_info, "Correcting the head."));
	memcpy(&head, mcshead, sizeof(head));
	if (casio_correct_mcshead(&head, casio_mcsfor_mcs))
		return (casio_error_notfound);

	msg((ll_info, "Sending the deletion command."));
	err = casio_seven_send_cmdmcs_delfile(handle, &head);
	if (err) return (err);
	if (response.casio_seven_packet_type == casio_seven_type_nak)
		return (casio_error_notfound);
	if (response.casio_seven_packet_type != casio_seven_type_ack)
		return (casio_error_unknown);

	return (0);
}
