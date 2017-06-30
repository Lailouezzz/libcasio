/* ****************************************************************************
 * stream/builtin/libusb/settm.c -- set timeouts of a libusb stream.
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
 *	casio_libusb_settm:
 *	Set timeouts.
 *
 *	@arg	vcookie		the cookie (uncasted).
 *	@arg	timeouts	the timeouts.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_libusb_settm(cookie_libusb_t *cookie,
	const casio_timeouts_t *timeouts)
{
	/* set the timeouts */
	cookie->tmread =  timeouts->casio_timeouts_read;
	cookie->tmwrite = timeouts->casio_timeouts_write;

	/* no error! */
	return (0);
}

#endif
