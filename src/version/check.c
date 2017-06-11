/* ****************************************************************************
 * version/check.c -- Version encoding and decoding utilities.
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
#include <ctype.h>

/**
 *	casio_check_version:
 *	Check if a version string is well formatted.
 *
 *	@arg	raw		the string to check.
 *	@return			if there was an error.
 */

int casio_check_version(const char *raw)
{
	/* check length */
	if (memchr(raw, 0, 11) != &raw[10]) return (casio_error_op);

	/* check characters */
	if (!isdigit(raw[0]) || !isdigit(raw[1]) || raw[2] != '.'
	 || !isdigit(raw[3]) || !isdigit(raw[4]) || raw[5] != '.'
	 || !isdigit(raw[6]) || !isdigit(raw[7])
	 || !isdigit(raw[8]) || !isdigit(raw[9]))
		return (casio_error_op);

	/* no error! */
	return (0);
}
