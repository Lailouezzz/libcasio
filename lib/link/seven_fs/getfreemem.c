/* ****************************************************************************
 * link/seven_fs/getfreemem.c -- get the available free space on a 7.00 fs.
 * Copyright (C) 2020 Alan "Lailouezzz" Le Bouder <alanlebouder@gmail.com>
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

/**
 *	casio_sevenfs_getfreemem:
 *	Get the available free space on a Protocol 7.00 filesystem.
 *
 *	@arg	cookie		the cookie.
 *	@arg	path		the path.
 *  @arg    capacity    the pointer to capacity
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_sevenfs_getfreemem(sevenfs_cookie_t *cookie,
                                          sevenfs_path_t *path,
		                                  size_t *capacity)
{
    int err;
    casio_link_t *handle = cookie;

    return (0);
}