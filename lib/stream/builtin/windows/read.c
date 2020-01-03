/* ****************************************************************************
 * stream/builtin/windows/read.c -- read from a Windows API stream.
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
#include "windows.h"
#ifndef LIBCASIO_DISABLED_WINDOWS

/**
 *	casio_windows_read: TODO
 *	Read from an MS-Windows stream.
 *
 *	@arg	cookie		the cookie.
 *	@return				the size if > 0, or if < 0 the error code is -[returned value].
 */

ssize_t CASIO_EXPORT casio_windows_read(win_cookie_t *cookie,
	unsigned char *dest, size_t size)
{
	DWORD werr, wsuccess, recv; size_t tocopy;

	/* Transmit what's already in the buffer. */

	if (cookie->_start <= cookie->_end) {
		tocopy = cookie->_end - cookie->_start + 1;
		if (tocopy > size) tocopy = size;

		memcpy(dest, &cookie->_buf[cookie->_start], tocopy);
		cookie->_start += tocopy;
		dest += tocopy;
		size -= tocopy;
	}

	/* Main receiving loop. */

	while (size) {
		/* Receive. */

		wsuccess = ReadFile(cookie->_handle,
			cookie->_buf, size, &recv, NULL);

		/* Check error. */

		if (!wsuccess) switch ((werr = GetLastError())) {
			case ERROR_DEV_NOT_EXIST:
				msg((ll_error, "Device has been disconnected!"));
				return (casio_error_nocalc);

			default:
				msg((ll_fatal, "Encountered error 0x%08lX", werr));
				return (casio_error_unknown);
		}

		/* Get the current size to copy. */

		tocopy = (size_t)recv;
		if (tocopy > size) tocopy = size;

		/* Copy to destination. */

		memcpy(dest, cookie->_buf, tocopy);
		dest += tocopy;
		size -= tocopy;

		/* Correct start and end points. */

		cookie->_start = tocopy;
		cookie->_end = (size_t)recv - 1;
	}

	return (0);
}

#endif
