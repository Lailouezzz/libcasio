/* ****************************************************************************
 * version/decode.c -- Version encoding and decoding utilities.
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
#include "../internals.h"

/**
 *	casio_decode_version:
 *	Decode a version from a 'MM.mm.ffff' formatted version string.
 *
 *	@arg	raw		the string to decode from.
 *	@arg	version	the structure where to store the information.
 *	@return			the error code (if any).
 */

int CASIO_EXPORT casio_decode_version(casio_version_t *version,
	const char *raw)
{
	/* helper values */
	const int two = '0' + '0' * 10;

	/* get the basic version data */
	version->casio_version_major = raw[0] * 10 + raw[1] - two;
	version->casio_version_minor = raw[3] * 10 + raw[4] - two;

	/* get normal values */
	version->casio_version_zone =     raw[6] - '0';
	version->casio_version_math =     raw[7] - '0';
	version->casio_version_status =   raw[8] - '0';
	version->casio_version_platform = raw[9] - '0';

	/* no error */
	return (0);
}
