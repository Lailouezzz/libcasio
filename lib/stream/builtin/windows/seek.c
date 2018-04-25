/* ****************************************************************************
 * stream/builtin/windows/seek.c -- seek within a Windows API stream.
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
 *	casio_windows_seek:
 *	Seek within an MS-Windows stream.
 *
 *	@arg	cookie		the cookie.
 *	@arg	offset		the offset to seek to, and the final offset.
 *	@arg	whence		the whence.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_windows_seek(win_cookie_t *cookie,
	casio_off_t *offset, casio_whence_t whence)
{
	DWORD ret, MoveMethod;

	/* Get the move method. */

	switch (whence) {
	case CASIO_SEEK_CUR:
		MoveMethod = FILE_CURRENT;
		break;
	case CASIO_SEEK_END:
		MoveMethod = FILE_END;
		break;
	default /* CASIO_SEEK_SET */:
		MoveMethod = FILE_BEGIN;
		break;
	}

	ret = SetFilePointer(cookie->_handle, (LONG)*offset, NULL, MoveMethod);
	if (ret == INVALID_SET_FILE_POINTER)
		return (casio_error_op); /* TODO: check error with `GetLastError`? */

	*offset = (casio_off_t)ret;
	return (0);
}

#endif
