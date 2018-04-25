/* ****************************************************************************
 * fs/builtin/posix/del.c -- delete a POSIX file/directory.
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
#ifndef LIBCASIO_DISABLED_POSIX_FS

/**
 *	casio_posix_delete:
 *	Delete a file or directory.
 *
 *	@arg	cookie		the cookie.
 *	@arg	path		the file path.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_posix_delete(void *cookie, const char *path)
{
	int err;

	/* Remove. */
	if (!remove(path))
		return (0);

	/* Get the libcasio error. */
	switch (errno) {
	/* TODO */
	default:
		err = casio_error_unknown;
	}

	return (err);
}

#endif
