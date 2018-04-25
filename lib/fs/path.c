/* ****************************************************************************
 * fs/path.c -- filesystem native path management utilities.
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
 *	casio_make_native_path:
 *	Make a filesystem native path.
 *
 *	@arg	fs		the filesystem.
 *	@arg	nat		the native path to make.
 *	@arg	path	the path to convert.
 *	@return			the error code (0 if ok).
 */

int  CASIO_EXPORT casio_make_native_path(casio_fs_t *fs,
	void **nat, casio_path_t *path)
{
	casio_fs_makepath_t *makepath;

	/* Get the callback. */
	makepath = fs->casio_fs_funcs.casio_fsfuncs_makepath;
	if (!makepath) return (casio_error_op);

	/* Make the native path. */
	return ((*makepath)(fs->casio_fs_cookie, nat, path));
}

/**
 *	casio_free_native_path:
 *	Free a filesystem native path.
 *
 *	@arg	fs		the filesystem.
 *	@arg	nat		the native path to free.
 */

void CASIO_EXPORT casio_free_native_path(casio_fs_t *fs, void *nat)
{
	casio_fs_freepath_t *freepath;

	/* Get the callback. */
	freepath = fs->casio_fs_funcs.casio_fsfuncs_freepath;
	if (!freepath) return ;

	/* Free the native path. */
	(*freepath)(fs->casio_fs_cookie, nat);
}
