/* ****************************************************************************
 * libcasio/mutex.h -- libcasio mutexes.
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
#ifndef  LIBCASIO_MUTEX_H
# define LIBCASIO_MUTEX_H 1
# include <libcasio/cdefs.h>

typedef int casio_mutex_t;

CASIO_EXTERN void CASIO_EXPORT casio_init_mutex
	OF((casio_mutex_t *casio__mutex));

CASIO_EXTERN int  CASIO_EXPORT casio_lock_mutex
	OF((casio_mutex_t *casio__mutex));
CASIO_EXTERN int  CASIO_EXPORT casio_trylock_mutex
	OF((casio_mutex_t *casio__mutex));

CASIO_EXTERN void CASIO_EXPORT casio_unlock_mutex
	OF((casio_mutex_t *casio__mutex));

#endif /* LIBCASIO_MUTEX_H */
