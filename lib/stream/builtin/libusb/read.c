/* ****************************************************************************
 * stream/builtin/libusb/read.c -- read from a libusb stream.
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
 *	casio_libusb_read:
 *	Read using libusb cookie.
 *
 *	@arg	vcookie		the cookie (voided)
 *	@arg	data		the data pointer.
 *	@arg	size		the data size.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_libusb_read(cookie_libusb_t *cookie,
	unsigned char *dest, size_t size)
{
	int libusberr;
	size_t tocopy;

	/* Transmit what's already in the buffer. */

	if (cookie->_start <= cookie->_end) {
		tocopy = cookie->_end - cookie->_start + 1;
		if (tocopy > size) tocopy = size;

		memcpy(dest, &cookie->_buffer[cookie->_start], tocopy);
		cookie->_start += tocopy;
		dest += tocopy;
		size -= tocopy;
	}

	/* Main receiving loop. */

	while (size) {
		int recv;

		/* Make the transfer. */

		libusberr = libusb_bulk_transfer(cookie->_handle, ENDPOINT_IN,
			cookie->_buffer, BUFSIZE, &recv, cookie->tmread);
		switch (libusberr) {
		case 0:
			break;

		case LIBUSB_ERROR_PIPE:
		case LIBUSB_ERROR_NO_DEVICE:
		case LIBUSB_ERROR_IO:
			msg((ll_error, "The calculator is not here anymore :("));
			return (casio_error_nocalc);

		case LIBUSB_ERROR_TIMEOUT:
			return (casio_error_timeout);

		default:
			msg((ll_fatal, "libusb error was %d: %s", libusberr,
				libusb_strerror(libusberr)));
			return (casio_error_unknown);
		}

		/* Get the current size to copy. */

		tocopy = (size_t)recv;
		if (tocopy > size)
			tocopy = size;

		/* Copy to destination. */

		memcpy(dest, cookie->_buffer, tocopy);
		dest += tocopy;
		size -= tocopy;

		/* Correct start and end points. */

		cookie->_start = tocopy;
		cookie->_end = (size_t)recv - 1;
	}

	return (0);
}

#endif
