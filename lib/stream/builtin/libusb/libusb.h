/* ****************************************************************************
 * stream/builtin/libusb/libusb.h -- libusb stream internals.
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
#ifndef LOCAL_STREAM_BUILTIN_LIBUSB_H
# include "../../../internals.h"

# ifndef LIBCASIO_DISABLED_LIBUSB
#  include <libusb.h>

# define BUFSIZE 2048

typedef struct {
	libusb_context *_context;
	libusb_device_handle *_handle;

	/* Timeouts. */

	unsigned int tmread, tmwrite;

	/* Buffer control. */

	ssize_t _start, _end;
	unsigned char _buffer[BUFSIZE];
} cookie_libusb_t;

/* General callbacks. */

CASIO_EXTERN int CASIO_EXPORT casio_libusb_close
	OF((cookie_libusb_t *casio__cookie));
CASIO_EXTERN int CASIO_EXPORT casio_libusb_settm
	OF((cookie_libusb_t *casio__cookie,
		const casio_timeouts_t *casio__timeouts));

/* Character device callbacks. */

CASIO_EXTERN int CASIO_EXPORT casio_libusb_read
	OF((cookie_libusb_t *casio__cookie,
		unsigned char *casio__dest, size_t casio__size));
CASIO_EXTERN int CASIO_EXPORT casio_libusb_write
	OF((cookie_libusb_t *casio__cookie,
		const unsigned char *casio__data, size_t casio__size));

# endif
#endif /* LOCAL_STREAM_BUILTIN_LIBUSB_H */
