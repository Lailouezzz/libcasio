/* ****************************************************************************
 * stream/builtin/windows/write.c -- write to a Windows API stream.
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
 *	casio_windows_write:
 *	Write to an MS-Windows stream.
 *
 *	@arg	cookie		the cookie.
 *	@arg	data		the source
 *	@arg	size		the source size
 *	@return				the size written if > 0, or if < 0 the error code is -[returned value].
 */

ssize_t CASIO_EXPORT casio_windows_write(win_cookie_t *cookie,
	const unsigned char *data, size_t size)
{
	BOOL wsuccess, werr;
	size_t writtensize = 0;

	/* Make the I/O request. */

	wsuccess = TRUE;
	do {
		DWORD wrt;

		/* Write. */

		wsuccess = WriteFile(cookie->_handle, data, size, &wrt, NULL);
		if (!wsuccess) break;

		/* Go forward. */

		data += wrt;
		size -= wrt;
		writtensize += wrt;
	} while (size);

	/* Check the error. */

	if (!wsuccess) switch ((werr = GetLastError())) {
		case ERROR_DEV_NOT_EXIST:
			msg((ll_error, "Device has been disconnected!"));
			return -(casio_error_nocalc);

		default:
			msg((ll_fatal, "Encountered error 0x%08lx", werr));
			return -(casio_error_unknown);
	}

	return (writtensize);
}

#endif
