/* ****************************************************************************
 * fs/delete.c -- delete an element from a libcasio filesystem.
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
 *	casio_delete:
 *	Delete from a filesystem, using a native path.
 *
 *	@arg	fs		the filesystem.
 *	@arg	nat		the native path.
 *	@return			the error code (0 if ok).
 */

int CASIO_EXPORT casio_delete(casio_fs_t *fs, void *path)
{
	int err; casio_fs_del_t *delete;

	/* Get the callback. */
	delete = fs->casio_fs_funcs.casio_fsfuncs_del;
	if (!delete) return (casio_error_op);

	/* Make the operation. */
	err = (*delete)(fs->casio_fs_cookie, path);
	return (err);
}

/**
 *	casio_delete_path:
 *	Delete from a filesystem, using an abstract path.
 *
 *	@arg	fs		the filesystem.
 *	@arg	path	the abstract path.
 *	@return			the error code (0 if ok).
 */

int CASIO_EXPORT casio_delete_path(casio_fs_t *fs, casio_path_t *path)
{
	int err; void *nat;
	casio_fs_del_t *delete;

	/* Get the function. */
	delete = fs->casio_fs_funcs.casio_fsfuncs_del;
	if (!delete) return (casio_error_op);

	/* Make the native path. */
	err = casio_make_native_path(fs, &nat, path);
	if (err) return (err);

	/* Make the operation. */
	err = (*delete)(fs->casio_fs_cookie, nat);
	casio_free_native_path(fs, nat);
	return (err);
}
