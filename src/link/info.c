/* ****************************************************************************
 * link/info.c -- link information utilities.
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
 *	casio_get_link_info:
 *	Get link information.
 *
 *	@arg	handle		the link handle.
 *	@return				the link information pointer.
 */

const casio_link_info_t *casio_get_link_info(casio_link_t *handle)
{
	if (!handle) return (NULL);
	return (&handle->casio_link_info);
}
