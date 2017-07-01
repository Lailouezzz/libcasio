/* ****************************************************************************
 * fs/path.c -- make a directory on a libcasio filesystem.
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
#include "fs.h"

/**
 *	casio_makedir:
 *	Make a directory.
 *
 *	@arg	fs		the filesystem.
 *	@arg	path	the path of the directory to make.
 *	@return			the error code (0 if ok).
 */

int CASIO_EXPORT casio_makedir(casio_filesystem_t *fs, casio_path_t *path)
{
	casio_fs_makedir_t *makedir;
	int err; void *nat = NULL;

	/* Get the callback. */
	makedir = fs->casio_filesystem_functions.casio_fsfuncs_makedir;
	if (!makedir) return (casio_error_op);

	/* Get the native path. */
	err = casio_make_native_path(fs, &nat, path);
	if (err) return (err);

	/* Make the directory, free the native path, return. */
	err = (*makedir)(fs->casio_filesystem_cookie, nat);
	casio_free_native_path(fs, nat);
	return (err);
}
