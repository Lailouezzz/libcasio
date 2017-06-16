/* ****************************************************************************
 * link/usage/server/seven.c -- serving a Protocol 7.00 server.
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
 * ************************************************************************* */
#include "../usage.h"

/**
 *	casio_seven_serve:
 *	Serve a Protocol 7.00 server.
 *
 *	@arg	handle		the link handle.
 *	@arg	callbacks	the server callbacks (256 entries).
 *	@arg	cookie		the server cookie.
 *	@return				the error.
 */

int CASIO_EXPORT casio_seven_serve(casio_link_t *handle,
	casio_seven_server_func_t **callbacks, void *cookie)
{
	int err;

	/* Make checks. */
	chk_handle(handle);
	chk_seven(handle);

	if (handle->casio_link_flags & casio_linkflag_active) {
		err = casio_seven_send_swp(handle);
		if (err) return (err);
	} else {
		/* Initial check. */
		do {
			if ((err = casio_seven_receive(handle, 0)))
				return (err);
		} while (response.casio_seven_packet_type != casio_seven_type_chk
		 || !response.casio_seven_packet_initial);

		/* Ack and start! */
		err = casio_seven_send_ack(handle, 1);
		if (err) return (err);
	}

	/* Main loop. */
	while (1) {
		/* Check command packet. */
		if (response.casio_seven_packet_type != casio_seven_type_cmd) {
			if (response.casio_seven_packet_type == casio_seven_type_end
			 || response.casio_seven_packet_type == casio_seven_type_swp)
				break;

			err = casio_seven_send_err(handle, casio_seven_err_other);
			if (err) return (err);
		}

		/* Check if the callback exists. */
		if (!callbacks[response.casio_seven_packet_code]) {
			err = casio_seven_send_err(handle, casio_seven_err_other);
			if (err) return (err);
			continue;
		}

		/* Call the callback. */
		err = (*callbacks[response.casio_seven_packet_code])(cookie, handle);
		if (err) {
			if (err != casio_error_unknown) return (err);
			err = casio_seven_send_err(handle, casio_seven_err_other);
			if (err) return (err);
			continue;
		}
	}

	/* Ack and disconnect. */
	return (casio_seven_send_ack(handle, 0));
}
