/* ****************************************************************************
 * link/lock.c -- manage the link lock.
 * Copyright (C) 2016-2017 Thomas "Cakeisalie5" Touhey <thomas@touhey.fr>
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
#include "link.h"

/**
 *	casio_lock_link:
 *	Lock a link.
 *
 *	@arg	handle		the link handle to lock.
 *	@return				the error code (0 if ok).
 */

int  CASIO_EXPORT casio_lock_link(casio_link_t *handle)
{
	return (casio_lock(&handle->casio_link_lock));
}

/**
 *	casio_trylock_link:
 *	Try to lock a link.
 *
 *	@arg	handle		the link handle to lock.
 *	@return				the error code (0 if ok).
 */

int  CASIO_EXPORT casio_trylock_link(casio_link_t *handle)
{
	return (casio_trylock(&handle->casio_link_lock));
}

/**
 *	casio_unlock_link:
 *	Unlock a link.
 *
 *	@arg	handle		the link handle.
 */

void CASIO_EXPORT casio_unlock_link(casio_link_t *handle)
{
	casio_unlock(&handle->casio_link_lock);
}
