/* ****************************************************************************
 * log/manage.c -- fiddle with libcasio log settings.
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
/* The log setting */
CASIO_LOCAL casio_loglevel_t log_setting = LOGLEVEL;
#endif

/**
 *	casio_setlog:
 *	Set the log level at runtime.
 *
 *	@arg	level	the level to set.
 */

void CASIO_EXPORT casio_setlog(const char *level)
{
#if !defined(LIBCASIO_DISABLED_LOG)
	log_setting = casio_loglevel_fromstring(level);
#else
	(void)level;
#endif
}

/**
 *	casio_getlog:
 *	Get the log level at runtime.
 *
 *	@return			the current level.
 */

const char* CASIO_EXPORT casio_getlog(void)
{
#if !defined(LIBCASIO_DISABLED_LOG)
	return (casio_loglevel_tostring(log_setting));
#else
	return ("none");
#endif
}

#if !defined(LIBCASIO_DISABLED_LOG)
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
	return (log_setting <= level);
}
#endif
