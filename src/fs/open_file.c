/* ****************************************************************************
 * fs/open_file.c -- delete an element from a libcasio filesystem.
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
 *	casio_open:
 *	Open a file.
 *
 *	@arg	fs		the filesystem.
 *	@arg	stream	the file stream to make.
 *	@arg	path	the abstract path.
 *	@arg	size	the file size.
 *	@arg	mode	the open mode.
 *	@return			the error code (0 if ok).
 */

int CASIO_EXPORT casio_open(casio_fs_t *fs, casio_stream_t **stream,
	casio_path_t *path, casio_off_t size, casio_openmode_t mode)
{
	int err; void *nat;
	casio_fs_open_t *open;

	/* Get the function. */
	open = fs->casio_fs_funcs.casio_fsfuncs_open;
	if (!open) return (casio_error_op);

	/* Make the native path. */
	err = casio_make_native_path(fs, &nat, path);
	if (err) return (err);

	/* Make the operation. */
	err = (*open)(fs->casio_fs_cookie, nat, size, mode, stream);
	casio_free_native_path(fs, nat);
	return (err);
}

/**
 *	casio_open_nat:
 *	Open a file.
 *
 *	@arg	fs		the filesystem.
 *	@arg	stream	the file stream to make.
 *	@arg	path	the native path.
 *	@arg	size	the file size.
 *	@arg	mode	the open mode.
 *	@return			the error code (0 if ok).
 */

int CASIO_EXPORT casio_open_nat(casio_fs_t *fs, casio_stream_t **stream,
	void *path, casio_off_t size, casio_openmode_t mode)
{
	int err; casio_fs_open_t *open;

	/* Get the callback. */
	open = fs->casio_fs_funcs.casio_fsfuncs_open;
	if (!open) return (casio_error_op);

	/* Make the operation. */
	err = (*open)(fs->casio_fs_cookie, path, size, mode, stream);
	return (err);
}
