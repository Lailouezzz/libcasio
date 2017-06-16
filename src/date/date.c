/* ****************************************************************************
 * date/decode.c -- decode a date.
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
 *	casio_decode_date:
 *	Decode a date from a string.
 *
 *	@arg	c		the raw string.
 *	@arg	t		the destination timestamp.
 *	@return			the error code (0 if ok).
 */

int CASIO_EXPORT casio_decode_date(time_t *t, const char *c)
{
	const int two = '0' + '0' * 10, four = two + '0' * 100 + '0' * 1000;
	struct tm date;

	/* get the tm structure */
	memset(&date, 0, sizeof(date));
	date.tm_year = c[0] * 1000 + c[1] * 100 + c[2] * 10 + c[3] - four - 1900;
	date.tm_mon = c[5] * 10 + c[6] - two - 1;
	date.tm_mday = c[7] * 10 + c[8] - two;
	date.tm_hour = c[10] * 10 + c[11] - two;
	date.tm_min = c[12] * 10 + c[13] - two;

	/* set the timestamp, return */
	*t = mktime(&date);
	return (0);
}
