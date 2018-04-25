/* ****************************************************************************
 * utils/mutex.c -- mutex internals.
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
#include "../internals.h"

/**
 *	casio_init_lock:
 *	Initialize a mutex.
 *
 *	@arg	mutex		the mutex to initialize.
 */

void CASIO_EXPORT casio_init_lock(casio_mutex_t *mutex)
{
	*mutex = 0;
}

/**
 *	casio_lock:
 *	Lock a mutex.
 *
 *	@arg	mutex		the mutex to lock.
 *	@return				the error code (0 if ok).
 */

int  CASIO_EXPORT casio_lock(casio_mutex_t *mutex)
{
	int err;

	if (!*mutex) goto unlocked;
	if ((err = casio_sleep(0))) return (err);
	while (*(volatile casio_mutex_t*)mutex)
		casio_sleep(5);

unlocked:
	*mutex = 1;
	return (0);
}

/**
 *	casio_trylock:
 *	Try to lock a mutex.
 *
 *	@arg	mutex		the mutex to lock.
 *	@return				the error code (0 if ok).
 */

int  CASIO_EXPORT casio_trylock(casio_mutex_t *mutex)
{
	if (*mutex) return (casio_error_lock);
	*mutex = 1;
	return (0);
}

/**
 *	casio_unlock_mutex:
 *	Unlock a mutex.
 *
 *	@arg	mutex		the mutex to unlock.
 */

void CASIO_EXPORT casio_unlock(casio_mutex_t *mutex)
{
	*mutex = 0;
}
