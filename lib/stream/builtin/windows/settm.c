/* ****************************************************************************
 * stream/builtin/windows/settm.c -- set timeouts of a Windows API stream.
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
 *	casio_windows_settm:
 *	Set timeouts.
 *
 *	@arg	cookie		the cookie.
 *	@arg	timeouts	the timeouts to apply.
 *	@return				the error (0 if ok).
 */

int CASIO_EXPORT casio_windows_settm(win_cookie_t *cookie,
	const casio_timeouts_t *timeouts)
{
	COMMTIMEOUTS tm;

	/* Set the timeouts. */

	tm.ReadIntervalTimeout =       timeouts->casio_timeouts_read_bw;
	tm.ReadTotalTimeoutConstant =  timeouts->casio_timeouts_read;
	tm.WriteTotalTimeoutConstant = timeouts->casio_timeouts_write;
	SetCommTimeouts(cookie->_handle, &tm);

	return (0);
}

#endif
