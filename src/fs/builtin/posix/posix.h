/* ****************************************************************************
 * fs/builtin/posix/posix.h -- POSIX filesystem internals.
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
#ifndef  LOCAL_FS_BUILTIN_POSIX_H
# define LOCAL_FS_BUILTIN_POSIX_H 1
# include "../../../internals.h"
# ifndef LIBCASIO_DISABLED_POSIX_FS
#  include <sys/stat.h>
#  include <unistd.h>

/* Path conversions. */

CASIO_EXTERN int  CASIO_EXPORT casio_make_posix_path
	OF((void *casio__cookie, void **casio__path, casio_path_t *casio__array));
CASIO_EXTERN void CASIO_EXPORT casio_free_posix_path
	OF((void *casio__cookie, void  *casio__path));

CASIO_EXTERN int CASIO_EXPORT casio_make_posix_path_array
	OF((casio_path_t **casio__path, const char *casio__rawpath));

/* File information gathering. */

CASIO_EXTERN int CASIO_EXPORT casio_posix_stat
	OF((void *casio__cookie, void *casio__path,
		casio_stat_t *casio__file_info));

# endif
#endif /* LOCAL_FS_BUILTIN_POSIX_H */
