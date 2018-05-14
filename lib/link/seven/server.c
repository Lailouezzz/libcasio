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
#include "../link.h"

/* ---
 * New-style server requests and responses.
 * --- */

/**
 *	casio_seven_start_server:
 *	Become passive when active.
 *
 *	@arg	handle		the link handle.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_seven_start_server(casio_link_t *handle)
{
	int err;

	/* Make checks. */

	if (!handle)
		return (casio_error_init);
	if (handle->casio_link_flags & casio_linkflag_ended)
		return (casio_error_init);

	/* If active, we just want to send a swap and become passive. */

	if (handle->casio_link_flags & casio_linkflag_active) {
		if ((err = casio_seven_send_swp(handle)))
			return (err);
		return (0);
	}

	/* We are passive, so we want to wait for the initial check from
	 * the counterpart and ACK it. If the received packet is not an
	 * initial check, we don't respond, just like the calculator.
	 *
	 * TODO: and if the start packet is in the middle of a packet? */

	do {
		if ((err = casio_seven_receive(handle, 0)))
			return (err);
	} while (response.casio_seven_packet_type != casio_seven_type_chk
	 || !response.casio_seven_packet_initial);

	err = casio_seven_send_ack(handle, 1);
	if (err) return (err);

	return (0);
}

/**
 *	casio_seven_get_next_request:
 *	Get the next request if server.
 *
 *	@arg	handle		the link handle.
 *	@return				the error.
 */

int CASIO_EXPORT casio_seven_get_next_request(casio_link_t *handle)
{
	int err;

	/* TODO: check if the last action was receive, and if it is not
	 * the case, receive? */

	if (response.casio_seven_packet_type != casio_seven_type_cmd) {
		if (response.casio_seven_packet_type == casio_seven_type_end)
			return (casio_seven_send_ack(handle, 0));
		if (response.casio_seven_packet_type == casio_seven_type_swp)
			return (casio_error_iter);
		if ((err = casio_seven_send_err(handle, casio_seven_err_other)))
			return (err);
	}

	return (0);
}

/* ---
 * Old-style server using the new-style utilities.
 * --- */

/**
 *	casio_seven_serve:
 *	Take control of the execution thread to make a passive server.
 *
 *	@arg	handle		the link handle.
 *	@arg	callbacks	the callbacks.
 *	@arg	cookie		i suppose that's just the way that cookie crumbles.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_seven_serve(casio_link_t *handle,
	casio_seven_server_func_t * const *callbacks, void *cookie)
{
	int err;

	if ((err = casio_seven_start_server(handle)) && err != casio_error_active)
		return (err);
	while (1) {
		/* Check if the next element is a command. */

		switch (casio_seven_get_next_request(handle)) {
		case 0:
			break;
		case casio_error_iter:
			return (0);
		default:
			return (err);
		}

		/* Check if the callback exists. */

		if (!callbacks[response.casio_seven_packet_code]) {
			err = casio_seven_send_err(handle, casio_seven_err_other);
			if (err)
				return (err);
			continue;
		}

		/* Call the callback. */

		err = (*callbacks[response.casio_seven_packet_code])(cookie, handle);
		if (err) {
			if (err != casio_error_unknown)
				return (err);

			err = casio_seven_send_err(handle, casio_seven_err_other);
			if (err)
				return (err);
			continue;
		}
	}

	return (0);
}
