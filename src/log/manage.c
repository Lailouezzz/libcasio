/* ****************************************************************************
 * log/manage.c -- manage libcasio logging utilities.
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
#undef islog
#define islog(CASIO__LOGLEVEL) \
	(casio_getlog() <= (CASIO__LOGLEVEL))

/* ************************************************************************* */
/*  Interact with the log settings at runtime                                */
/* ************************************************************************* */
/* The log setting */
CASIO_LOCAL casio_loglevel_t log_setting = LOGLEVEL;

/**
 *	casio_setlog:
 *	Set the log level at runtime.
 *
 *	@arg	level	the level to set.
 */

void CASIO_EXPORT casio_setlog(casio_loglevel_t level)
{
	log_setting = level;
}

/**
 *	casio_getlog:
 *	Get the log level at runtime.
 *
 *	@return			the current level.
 */

casio_loglevel_t CASIO_EXPORT casio_getlog(void)
{
	return (log_setting);
}
/* ************************************************************************* */
/*  Make conversions between settings values and strings, list settings      */
/* ************************************************************************* */
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

/**
 *	casio_listlog:
 *	List log levels.
 *
 *	@arg	callback	the callback.
 *	@arg	cookie		the callback cookie.
 */

void CASIO_EXPORT casio_listlog(casio_log_list_t *callback, void *cookie)
{
	(*callback)(cookie, "info");
	(*callback)(cookie, "warn");
	(*callback)(cookie, "error");
	(*callback)(cookie, "fatal");
}

/**
 *	casio_islog:
 *	Get the log level out of the ll_* tuples.
 *
 *	@arg	level		the loglevel.
 *	@arg	func		the function.
 *	@return				the loglevel.
 */

int CASIO_EXPORT casio_islog(casio_loglevel_t level, const char *func)
{
	(void)func;
	return (islog(level));
}
