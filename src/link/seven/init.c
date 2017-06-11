/* ****************************************************************************
 * link/seven/init.c -- initialize a Protocol 7.00 link handle.
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

/**
 *	casio_seven_start:
 *	Start the communication.
 *
 *	@arg	handle		the handle.
 *	@return				the error, if any.
 */

int casio_seven_start(casio_link_t *handle)
{
	/* start */
	int err = 0;
	if (~handle->casio_link_flags & casio_linkflag_active)
		return (0);
	msg((ll_info, "Be active, live!"));

	/* send initial check */
	if (handle->casio_link_flags & casio_linkflag_check) {
		msg((ll_info, "Sending initial check packet."));
		if ((err = casio_seven_send_ini_check(handle))) {
			msg((ll_fatal, "couldn't send check/didn't receive answer"));
			return (err);
		} else if (response.casio_seven_packet_type != casio_seven_type_ack) {
			msg((ll_fatal, "received packet wasn't ack, ask your dentist"));
			return (casio_error_unknown);
		}
	} elsemsg((ll_info, "Initial check is for noobz."));

	/* discover environment */
	if (handle->casio_link_flags & casio_linkflag_disc) {
		msg((ll_info, "Checking the environment."));
		if ((err = casio_seven_send_cmdsys_getinfo(handle))) {
			msg((ll_fatal, "couldn't send env query/receive answer"));
			return (err);
		} else if (response.casio_seven_packet_type != casio_seven_type_ack
		 || !response.casio_seven_packet_extended) {
			msg((ll_fatal,
				"response to env query wasn't an extended ack packet"));
			return (casio_error_unknown);
		}
	}

	/* save server, get environment based on hardware id */
	handle->casio_link_info = response.casio_seven_packet_info;
	casio_seven_getenv(&handle->casio_link_env,
		handle->casio_link_info.casio_link_info_hwid);
	msg((ll_info, "Environment is '%s'",
		handle->casio_link_env.casio_seven_env_name));

	/* no error */
	return (0);
}

/**
 *	casio_seven_end:
 *	End the communication.
 *
 *	@arg	handle		the handle.
 *	@return				the error code (0 if none).
 */

int casio_seven_end(casio_link_t *handle)
{
	int err;

	if (~handle->casio_link_flags
	 & (casio_linkflag_active | casio_linkflag_term))
		return (0);
	if ( handle->casio_link_flags
	 & (casio_linkflag_ended))
		return (0);

	/* send the termination packet */
	msg((ll_info, "we were active, send sending terminate packet"));
	err = casio_seven_send_term(handle);
	if (err) return (err);

	/* check ack */
	if (response.casio_seven_packet_type != casio_seven_type_ack) {
		msg((ll_warn, "answer to terminate packet wasn't ack!"));
		return (casio_error_unknown);
	}

	/* no error! */
	return (0);
}
