/* ****************************************************************************
 * link/open/usb.c -- open a USB communication.
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
 *	casio_open_usb:
 *	Open a USB communication.
 *
 *	@arg	handle		the link handle to make.
 *	@arg	flags		the link flags.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_open_usb(casio_link_t **handle, unsigned long flags)
{
	int err, failed = 0, tries = 3;
	casio_stream_t *stream, *ns;

	/* Open the base USB stream. */

	*handle = NULL;
	msg((ll_info, "Looking for USB-connected calculators."));
	while (1) {
		if (failed) {
			msg((ll_info, "Trying again in one second."));
			casio_sleep(1000);
		}

		err = casio_open_usb_stream(&stream);
		if (err == casio_error_op)
			return (casio_error_nocalc);
		if (!err)
			break;
		if (err != casio_error_nocalc)
			return (err);

		msg((ll_error, "No device found!"));
		if (!tries--)
			return (casio_error_nocalc);
		failed = 1;
	}

	/* Open the SCSI device stream if required. */

	switch ((err = casio_open_seven_scsi(&ns, stream))) {
	case 0:
		stream = ns;
		break;

	case casio_error_op:
		/* Use the good ol' basic stream. */
		break;

	default:
		return (err);
	}

	/* Open the link based on the stream. */

	err = casio_open_link(handle, flags, stream, NULL);
	if (err)
		return (err);

	return (0);
}
