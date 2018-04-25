/* ****************************************************************************
 * fs/builtin/posix/open_fs.c -- open a POSIX filesystem.
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
#include "posix.h"
#ifndef LIBCASIO_DISABLED_POSIX_FS

/* Callbacks. */

CASIO_LOCAL casio_fsfuncs_t posix_fs_funcs = {
	NULL, &casio_make_posix_path, &casio_free_posix_path,
	&casio_posix_stat, NULL, NULL, NULL,
	NULL, NULL, NULL
};

/**
 *	casio_open_posix_fs:
 *	Open a POSIX filesystem interface.
 *
 *	@arg	fs		the filesystem interface to make.
 *	@return			the error code (0 if ok).
 */

int CASIO_EXPORT casio_open_posix_fs(casio_fs_t **fs)
{
	return (casio_open_fs(fs, NULL, &posix_fs_funcs));
}

#endif
