/* ****************************************************************************
 * link/seven_fs/seven_fs.h -- Protocol 7.00 filesystem internals.
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
#ifndef  LOCAL_LINK_SEVEN_FS_H
# define LOCAL_LINK_SEVEN_FS_H 1
# include "../usage/usage.h"

/* The cookie is the `casio_link_t` pointer. */

typedef casio_link_t sevenfs_cookie_t;

/* Native "path" management.
 * This structure will probably not be allocated every time. */

typedef struct {
	unsigned char sevenfs_path_dir;
	unsigned char sevenfs_path_file;
	unsigned char sevenfs_path_dev;
	unsigned char sevenfs_path__align;
	char          sevenfs_path_data[24];
} sevenfs_path_t;

CASIO_EXTERN int  CASIO_EXPORT casio_make_sevenfs_path
	OF((sevenfs_cookie_t *casio__cookie, sevenfs_path_t **casio__native_path,
		casio_path_t *casio__path));
CASIO_EXTERN void CASIO_EXPORT casio_free_sevenfs_path
	OF((sevenfs_cookie_t *casio__cookie, sevenfs_path_t  *casio__native_path));

/* Delete a file. */

CASIO_EXTERN int CASIO_EXPORT casio_sevenfs_delete
	OF((sevenfs_cookie_t *casio__cookie, sevenfs_path_t *casio__path));

/* Optimize the filesystem. */

CASIO_EXTERN int CASIO_EXPORT casio_sevenfs_optimize
	OF((sevenfs_cookie_t *casio__cookie, const char *casio__device));

#endif /* LOCAL_LINK_SEVEN_FS_H */
