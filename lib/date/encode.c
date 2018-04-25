/* ****************************************************************************
 * date/encode.c -- encode a date.
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
 *	casio_encode_date:
 *	Encode a date from a string.
 *
 *	@arg	t		the source timestamp.
 *	@arg	c		the destination string.
 *	@return			the error code (0 if ok).
 */

int CASIO_EXPORT casio_encode_date(char *c, const time_t *t)
{
	/* helper values */
	struct tm *date = gmtime(t);
	char buf[15]; sprintf(buf, "%04u.%02u%02u.%02u%02u",
		min(date->tm_year + 1900, 9999), date->tm_mon + 1, date->tm_mday,
		date->tm_hour, date->tm_min);
	memcpy(c, buf, 14);
	return (0);
}
