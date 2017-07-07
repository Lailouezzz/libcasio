/* ****************************************************************************
 * fs/builtin/posix/make.c -- make a POSIX filesystem element.
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
#define PATHS_FTW
#include "posix.h"
#ifndef LIBCASIO_DISABLED_POSIX

/**
 *	casio_posix_make:
 *	Make a POSIX filesystem element.
 *
 *	@arg	cookie		the cookie.
 *	@arg	path		the file path.
 *	@arg	stat		the file information.
 *	@arg	...			other information depending on the file type.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_posix_make(void *cookie, const char *path,
	const casio_stat_t *info, ...)
{
	/* TODO */
	return (casio_error_op);
}

#endif
