/* ****************************************************************************
 * utils/alloc.c -- dynamic memory allocation utilities.
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
#include "../internals.h"

/**
 *	casio_alloc:
 *	Allocate memory dynamically.
 *
 *	@arg	num		the number of elements.
 *	@arg	size	the size of an element.
 *	@return			the allocated memory region.
 */

void* CASIO_EXPORT casio_alloc(size_t num, size_t size)
{
	if (!num && !size)
		return (NULL);
	return (malloc(num * size));
}

/**
 *	casio_free:
 *	Free dynamically allocated memory.
 *
 *	@arg	ptr		the pointer to the allocated memory.
 */

void CASIO_EXPORT casio_free(void *ptr)
{
	if (!ptr)
		return ;
	free(ptr);
}
