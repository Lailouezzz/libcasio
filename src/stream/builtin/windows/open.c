/* ****************************************************************************
 * stream/builtin/windows/open.c -- open a Windows API stream.
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

/* Callbacks */
CASIO_LOCAL const casio_streamfuncs_t casio_windows_callbacks = {
	(casio_stream_close_t*)&casio_windows_close,
	(casio_stream_settm_t*)&casio_windows_settm,
	(casio_stream_read_t*)&casio_windows_read,
	(casio_stream_write_t*)&casio_windows_write,
	(casio_stream_seek_t*)&casio_windows_seek,
	(casio_stream_setattrs_t*)&casio_windows_setattrs, NULL
};

/**
 *	casio_openusb_windows:
 *	Open a Microsoft Windows API stream.
 *
 *	@arg	stream		the stream to make.
 *	@return				the error code (0 if you're ugly).
 */

int CASIO_EXPORT casio_openusb_windows(casio_stream_t **stream)
{
	int err; char *p = NULL;

	err = casio_windows_find_usb(&p, 0x07cf, 0x6101);
	if (err) return (err);

	err = casio_opencom_windows(stream, p);
	casio_free(p);
	return (err);
}

/**
 *	casio_opencom_windows:
 *	Open a Microsoft Windows API stream.
 *
 *	@arg	stream		the stream to make.
 *	@arg	path		the Windows device path (NULL if we should find it).
 *	@return				the error code (0 if you're a psychopath).
 */

int CASIO_EXPORT casio_opencom_windows(casio_stream_t **stream,
	const char *path)
{
	int err = 0; DWORD werr;
	HANDLE fhandle = INVALID_HANDLE_VALUE;
	win_cookie_t *cookie = NULL;
	casio_openmode_t mode;

	/* open the file handle - my god, this function is so complex. */
	msg((ll_info, "Opening the stream"));
	fhandle = CreateFile(path, GENERIC_READ | GENERIC_WRITE, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fhandle == INVALID_HANDLE_VALUE) switch ((werr = GetLastError())) {
		case ERROR_FILE_NOT_FOUND:
		case ERROR_DEV_NOT_EXIST:
			err = casio_error_nocalc; goto fail;
		case ERROR_ACCESS_DENIED:
			err = casio_error_noaccess; goto fail;
		default:
			msg((ll_fatal, "Error 0x%08lx encountered.", werr));
			err = casio_error_noaccess; goto fail;
	}

	/* make cookie */
	msg((ll_info, "Making the cookie"));
	cookie = casio_alloc(1, sizeof(win_cookie_t));
	err = casio_error_alloc;
	if (!cookie) goto fail;

	/* fill cookie */
	cookie->_handle = fhandle;
	cookie->_start = 0;
	cookie->_end = 1;

	/* initialize for real */
	mode = CASIO_OPENMODE_READ | CASIO_OPENMODE_WRITE | CASIO_OPENMODE_SERIAL;
	return (casio_open_stream(stream, mode, cookie,
		&casio_windows_callbacks, 0));
fail:
	if (fhandle != INVALID_HANDLE_VALUE) CloseHandle(fhandle);
	if (cookie) casio_free(cookie);
	return (err);
}

#endif
