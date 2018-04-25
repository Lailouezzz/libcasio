/* ****************************************************************************
 * stream/builtin/windows/windows.h -- Windows API stream internals.
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
#ifndef  LOCAL_STREAM_BUILTIN_WINDOWS_H
# define LOCAL_STREAM_BUILTIN_WINDOWS_H 1
# include "../../../internals.h"
# ifndef LIBCASIO_DISABLED_WINDOWS
#  include <windows.h>
#  include <setupapi.h>
#  include <usbiodef.h>
#  include <winerror.h>
#  define BUFSIZE 2048

/* Here is the structure of a cookie, used by the stream callbacks.
 * PSP_COOKIE is just there to take the piss out of Microsoft, it isn't
 * actually used in the source code. */

typedef struct {
	HANDLE _handle;

	long _start, _end;
	unsigned char _buf[BUFSIZE];
} win_cookie_t, *PSP_COOKIE;

/* Find a USB device. */

CASIO_EXTERN int CASIO_EXPORT casio_windows_find_usb
	OF((char **path, unsigned int vid, unsigned int pid));

/* General callbacks. */

CASIO_EXTERN int CASIO_EXPORT casio_windows_close
	OF((win_cookie_t *cookie));
CASIO_EXTERN int CASIO_EXPORT casio_windows_settm
	OF((win_cookie_t *cookie, const casio_timeouts_t *timeouts));

/* Character device callbacks. */

CASIO_EXTERN int CASIO_EXPORT casio_windows_read
	OF((win_cookie_t *cookie, unsigned char *dest, size_t size));
CASIO_EXTERN int CASIO_EXPORT casio_windows_write
	OF((win_cookie_t *cookie, const unsigned char *data, size_t size));

/* Seek callback. */

CASIO_EXTERN int CASIO_EXPORT casio_windows_seek
	OF((win_cookie_t *cookie, casio_off_t *offset, casio_whence_t whence));

/* Serial callbacks. */

CASIO_EXTERN int CASIO_EXPORT casio_windows_setattrs
	OF((win_cookie_t *cookie, const casio_streamattrs_t *settings));

# endif
#endif /* LOCAL_STREAM_BUILTIN_WINDOWS_H */
