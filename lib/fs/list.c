/* ****************************************************************************
 * fs/delete.c -- delete an element from a libcasio filesystem.
 * Copyright (C) 2019 Alan "Lailouezzz" Le Bouder <alanlebouder@gmail.com>
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
 *	casio_list:
 *	List all files/directories.
 *
 *	@arg	fs		    the filesystem.
 *	@arg	path	    the path.
 *  @arg    callback    the callback list function.
 *  @arg    cbcookie    the callback cookie.
 *	@return			    the error code (0 if ok).
 */

int CASIO_EXPORT casio_list(casio_fs_t *fs, casio_path_t *path,
		casio_fs_list_func_t *callback, void *cbcookie)
{
	int err; void *nat;
    casio_fs_list_t *list;

    /* Get the function */
    list = fs->casio_fs_funcs.casio_fsfuncs_list;
    if(!list) return (casio_error_op);

    /* Make the native path. */
	err = casio_make_native_path(fs, &nat, path);
	if (err) return (err);

	/* Make the operation. */
	err = (*list)(fs->casio_fs_cookie, nat, callback, cbcookie);
	casio_free_native_path(fs, nat);
    return (err);
}