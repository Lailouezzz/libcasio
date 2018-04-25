/* ****************************************************************************
 * stream/builtin/libusb/write.c -- write to a libusb stream.
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
 *	casio_libusb_write:
 *	Write using libusb cookie.
 *
 *	@arg	cookie		the cookie.
 *	@arg	data		the source.
 *	@arg	size		the source size.
 *	@return				the error code (0 if ok).
 */

# define ENDPOINT_OUT (LIBUSB_ENDPOINT_OUT | LIBUSB_TRANSFER_TYPE_ISOCHRONOUS)

int CASIO_EXPORT casio_libusb_write(cookie_libusb_t *cookie,
	const unsigned char *data, size_t size)
{
	int sent, libusberr;

	/* Make the transfer. */

	libusberr = libusb_bulk_transfer(cookie->_handle, ENDPOINT_OUT,
		(unsigned char*)data, size, &sent, cookie->tmwrite);
	switch (libusberr) {
		case 0: break;

		case LIBUSB_ERROR_PIPE:
		case LIBUSB_ERROR_NO_DEVICE:
			msg((ll_error, "The calculator is not here anymore :("));
			return (casio_error_nocalc);

		default:
			msg((ll_fatal, "libusb error was %d: %s", libusberr,
				libusb_strerror(libusberr)));
			return (casio_error_unknown);
	}

	return (0);
}

#endif
