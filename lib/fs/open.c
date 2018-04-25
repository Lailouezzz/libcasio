/* ****************************************************************************
 * fs/open.c -- open a libcasio filesystem.
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
 *	casio_open_fs:
 *	Open a filesystem.
 *
 *	@arg	pfs		the filesystem to open.
 *	@arg	cookie	the filesystem cookie.
 *	@arg	funcs	the filesystem callbacks.
 *	@return			the error code (0 if ok).
 */

int CASIO_EXPORT casio_open_fs(casio_fs_t **pfs,
	void *cookie, const casio_fsfuncs_t *funcs)
{
	int err; casio_fs_t *fs;

	/* Allocate the filesystem. */

	*pfs = malloc(sizeof(casio_fs_t)); fs = *pfs;
	if (!fs) {
		err = casio_error_alloc;
		goto fail;
	}

	/* Copy the data into it. */

	fs->casio_fs_cookie = cookie;
	memcpy(&fs->casio_fs_funcs, funcs, sizeof(casio_fsfuncs_t));
	return (0);

fail:
	if (funcs->casio_fsfuncs_close)
		(*funcs->casio_fsfuncs_close)(cookie);
	return (err);
}

/**
 *	casio_close_fs:
 *	Close a filesystem.
 */

int CASIO_EXPORT casio_close_fs(casio_fs_t *fs)
{
	casio_fs_close_t *cl;
	int err = 0;

	if (!fs)
		return (0);

	/* Call the custom free function for the filesystem content. */

	cl = fs->casio_fs_funcs.casio_fsfuncs_close;
	if (cl)
		err = (*cl)(fs->casio_fs_cookie);

	/* Free the element and go! */

	free(fs);
	return (err);
}
