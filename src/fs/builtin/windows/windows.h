/* ****************************************************************************
 * fs/builtin/windows/windows.h -- Windows filesystem internals.
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
#ifndef  LOCAL_FS_BUILTIN_WINDOWS_H
# define LOCAL_FS_BUILTIN_WINDOWS_H 1
# include "../../../internals.h"
# ifndef LIBCASIO_DISABLED_WINDOWS

/* Path conversions. */

CASIO_EXTERN int  CASIO_EXPORT casio_make_windows_path
	OF((void *casio__cookie, void **casio__native_path,
		casio_path_t *casio__array));
CASIO_EXTERN void CASIO_EXPORT casio_free_windows_path
	OF((void *casio__cookie, void  *casio__native_path));

# endif
#endif /* LOCAL_FS_BUILTIN_WINDOWS_H */
