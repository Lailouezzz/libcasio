/* ****************************************************************************
 * log/conv.c -- convert between strings and number log levels.
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
#include "log.h"
#if !defined(LIBCASIO_DISABLED_LOG)

/**
 *	casio_loglevel_tostring:
 *	Make a string out of a log level.
 *
 *	@arg	level	the log level.
 *	@return			the string.
 */

const char* CASIO_EXPORT casio_loglevel_tostring(casio_loglevel_t level)
{
	if (level >= 0  && level < 10)
		return ("info");
	if (level >= 10 && level < 20)
		return ("warn");
	if (level >= 20 && level < 30)
		return ("error");
	if (level >= 30 && level < 40)
		return ("fatal");
	return ("none");
}

/**
 *	casio_loglevel_fromstring:
 *	Make a log level out of a string.
 *
 *	@arg	string	the string.
 *	@return			the log level.
 */

casio_loglevel_t CASIO_EXPORT casio_loglevel_fromstring(const char *string)
{
	if (!strcmp(string, "info"))
		return (casio_loglevel_info);
	if (!strcmp(string, "warn"))
		return (casio_loglevel_warn);
	if (!strcmp(string, "error"))
		return (casio_loglevel_error);
	if (!strcmp(string, "fatal"))
		return (casio_loglevel_fatal);
	return (casio_loglevel_none);
}
#endif

/**
 *	casio_listlog:
 *	List log levels.
 *
 *	@arg	callback	the callback.
 *	@arg	cookie		the callback cookie.
 */

void CASIO_EXPORT casio_listlog(casio_log_list_t *callback, void *cookie)
{
	(*callback)(cookie, "none");
#if !defined(LIBCASIO_DISABLED_LOG)
	(*callback)(cookie, "info");
	(*callback)(cookie, "warn");
	(*callback)(cookie, "error");
	(*callback)(cookie, "fatal");
#endif
}
