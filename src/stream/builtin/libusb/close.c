/* ****************************************************************************
 * stream/builtin/libusb/close.c -- close a libusb stream.
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
#include "libusb.h"
#ifndef LIBCASIO_DISABLED_LIBUSB

/**
 *	casio_libusb_close:
 *	Close libusb cookie.
 *
 *	@arg	cookie		the cookie.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_libusb_close(cookie_libusb_t *cookie)
{
	if (cookie->_handle)  libusb_close(cookie->_handle);
	if (cookie->_context) libusb_exit(cookie->_context);
	casio_free(cookie);
	return (0);
}

#endif
