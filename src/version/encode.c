/* ****************************************************************************
 * version/encode.c -- Version encoding and decoding utilities.
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
 *	casio_encode_version:
 *	Encode a version into an 'MM.mm.ffff' formatted version string.
 *
 *	@arg	version	the version to encode.
 *	@arg	raw		the string to encode into.
 *	@return			the error code (if any).
 */

int CASIO_EXPORT casio_encode_version(char *raw,
	const casio_version_t *version)
{
	/* make buffer */
	char buf[20]; sprintf(buf, "%02u.%02u.%c%c%c%c",
		version->casio_version_major,
		version->casio_version_minor,
		'0' + version->casio_version_zone,
		'0' + version->casio_version_math,
		'0' + version->casio_version_status,
		'0' + version->casio_version_platform);

	/* copy final */
	memcpy(raw, buf, 10);
	return (0);
}
