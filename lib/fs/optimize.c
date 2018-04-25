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
 *	casio_optimize:
 *	Optimize a filesystem.
 *
 *	@arg	fs		the filesystem.
 *	@arg	device	the device name.
 *	@return			the error code (0 if ok).
 */

int CASIO_EXPORT casio_optimize(casio_fs_t *fs, const char *device)
{
	int err;
	casio_fs_optim_t *optim;

	/* Get the callback. */

	optim = fs->casio_fs_funcs.casio_fsfuncs_optim;
	if (!optim)
		return (casio_error_op);

	/* Just do it. */

	err = (*optim)(fs->casio_fs_cookie, device);
	return (err);
}
