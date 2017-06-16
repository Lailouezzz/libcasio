/* ****************************************************************************
 * link/open_com.c -- open a serial communication.
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
#include "link.h"

/**
 *	casio_open_com:
 *	Open a serial communication.
 *
 *	@arg	handle		the link handle to make.
 *	@arg	flags		the link flags.
 *	@arg	path		the serial device path.
 *	@arg	attrs		the serial attributes.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_open_com(casio_link_t **handle, unsigned long flags,
	const char *path, const casio_streamattrs_t *attrs)
{
	int err, failed = 0, tries = 3; casio_stream_t *stream;

	*handle = NULL;
	msg((ll_info, "Looking for serial device with the following path:"));
	msg((ll_info, "| %s", path));
	while (1) {
		if (failed) {
			msg((ll_info, "Trying again in one second."));
			casio_sleep(1000);
		}

		err = casio_open_com_stream(&stream, path);
		if (err == casio_error_op) return (casio_error_nocalc);
		if (!err) break;
		if (err != casio_error_nocalc)
			return (err);

		msg((ll_error, "No device found!"));
		if (!tries--)
			return (casio_error_nocalc);
		failed = 1;
	}

	err = casio_open_link(handle, flags, stream, attrs);
	if (err) return (err);
	return (0);
}
