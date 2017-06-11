/* ****************************************************************************
 * utils/date.c -- decode and encode a date.
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

int casio_decode_date(time_t *t, const char *c)
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

/**
 *	casio_encode_date:
 *	Encode a date from a string.
 *
 *	@arg	t		the source timestamp.
 *	@arg	c		the destination string.
 *	@return			the error code (0 if ok).
 */

int casio_encode_date(char *c, const time_t *t)
{
	/* helper values */
	struct tm *date = gmtime(t);
	char buf[15]; sprintf(buf, "%04u.%02u%02u.%02u%02u",
		min(date->tm_year + 1900, 9999), date->tm_mon + 1, date->tm_mday,
		date->tm_hour, date->tm_min);
	memcpy(c, buf, 14);
	return (0);
}
