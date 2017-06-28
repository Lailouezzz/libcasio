/* ****************************************************************************
 * log/msg.c -- libcasio logging utilities.
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
#include <stdarg.h>
#if !defined(LIBCASIO_DISABLED_LOG)

/**
 *	casio_log_prefix:
 *	Put the prefix.
 *
 *	@arg	loglevel	the log level.
 *	@arg	func		the function name.
 */

void CASIO_EXPORT casio_log_prefix(casio_loglevel_t loglevel, const char *func)
{
	if (func && !strncmp(func, "casio_", 6))
		func = &func[6];
	if (func) fprintf(stderr, "[libcasio %5s] %s: ",
		casio_loglevel_tostring(loglevel), func);
	else fprintf(stderr, "[libcasio %5s] ",
		casio_loglevel_tostring(loglevel));
}

/**
 *	casio_log_msg:
 *	Log a simple message.
 *
 *	@arg	loglevel	the log level.
 *	@arg	func		the function name.
 *	@arg	format		the format.
 *	@arg	...			the arguments.
 */

void CASIO_EXPORT casio_log_msg(casio_loglevel_t loglevel,
	const char *func, const char *format, ...)
{
	va_list args;
	int shouldlog = casio_islog(loglevel, NULL);

	/* put the prefix */
	if (shouldlog)
		casio_log_prefix(loglevel, func);

	/* put the main part */
	va_start(args, format);
	if (shouldlog) {
		vfprintf(stderr, format, args);
		fputc('\n', stderr);
	}
	va_end(args);
}

#endif /* LIBCASIO_DISABLED_LOG */
