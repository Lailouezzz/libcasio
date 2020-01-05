/* ****************************************************************************
 * link/seven_fs/open.c -- open a Protocol 7.00 filesystem.
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
#include "seven_fs.h"

/* Callbacks. */

CASIO_LOCAL casio_fsfuncs_t sevenfs_callbacks = {
	NULL,
	(casio_fs_makepath_t*)&casio_make_sevenfs_path,
	(casio_fs_freepath_t*)&casio_free_sevenfs_path,
	NULL,
	NULL,
	(casio_fs_del_t*)&casio_sevenfs_delete,
	NULL,
	(casio_fs_list_t*)&casio_sevenfs_list,
	(casio_fs_open_t*)&casio_sevenfs_open,
	(casio_fs_optim_t*)&casio_sevenfs_optimize
};

/**
 *	casio_open_seven_fs:
 *	Open a Protocol 7.00 filesystem.
 *
 *	@arg	fs		the filesystem to open.
 *	@arg	link	the link above which to access the file system.
 *	@return			the error code (0 if ok).
 */

int CASIO_EXPORT casio_open_seven_fs(casio_fs_t **fs,
	casio_link_t *link)
{
	/* Open the filesystem. (no need for cookie allocating for now) */
	return (casio_open_fs(fs, link, &sevenfs_callbacks));
}
