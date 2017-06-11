/* ****************************************************************************
 * link/usage/setlink.c -- link utilities.
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
#include "usage.h"
#define MPARSTOP (CASIO_PARMASK | CASIO_STOPBITSMASK)

/**
 *	casio_setlink:
 *	Set link properties.
 *
 *	@arg	handle		the link handle.
 *	@arg	uattrs		the attributes to set.
 *	@return				the error code (0 if ok).
 */

int casio_setlink(casio_link_t *handle, const casio_streamattrs_t *uattrs)
{
	int err; casio_streamattrs_t attrs;
	unsigned int speed; int stopbits, parity;

	/* make checks */
	chk_handle(handle);
	chk_seven(handle);
	chk_active(handle);

	/* check settings. */
	if (casio_get_attrs(handle->casio_link_stream, &attrs))
		return (casio_error_op);
	attrs.casio_streamattrs_speed = uattrs->casio_streamattrs_speed;
	attrs.casio_streamattrs_flags =
		(  attrs.casio_streamattrs_flags & ~MPARSTOP) |
		(uattrs->casio_streamattrs_flags &  MPARSTOP);

	/* get raw information for the command. */
	speed = attrs.casio_streamattrs_speed;
	stopbits = attrs.casio_streamattrs_flags & CASIO_TWOSTOPBITS ? 2 : 1;
	parity = (~attrs.casio_streamattrs_flags & CASIO_PARENB) ? 0
		: (attrs.casio_streamattrs_flags & CASIO_PARODD) ? 1 : 2;

	/* send command. */
	msg((ll_info, "sending the command."));
	if ((err = casio_seven_send_cmdsys_setlink(handle, speed, parity,
	  stopbits))) {
		msg((ll_fatal, "could not set command/receive answer"));
		return (err);
	} else if (response.casio_seven_packet_type != casio_seven_type_ack) {
		err = casio_error_unknown;
		goto end;
	}

	/* set communication properties. */
	casio_set_attrs(handle->casio_link_stream, &attrs);

	err = 0;
end:
	/* wait for the calculator to set its serial interface, and we're done. */
	casio_sleep(100);
	return (err);
}
