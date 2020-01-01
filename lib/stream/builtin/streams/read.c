/* ****************************************************************************
 * stream/builtin/streams/read.c -- read from a STREAMS stream.
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
#include "streams.h"
#ifndef LIBCASIO_DISABLED_STREAMS

/**
 *	casio_streams_read:
 *	Read from a terminal.
 *
 *	@arg	cookie		the cookie.
 *	@arg	data		the data pointer.
 *	@arg	size		the data size.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_streams_read(streams_cookie_t *cookie,
	unsigned char *dest, size_t *psize)
{
	int fd = cookie->_readfd;
	size_t size = *psize;

	/* Transmit what's already in the buffer. */

	if (cookie->_start <= cookie->_end) {
		size_t tocopy = cookie->_end - cookie->_start + 1;
		if (tocopy > size) tocopy = size;

		memcpy(dest, &cookie->_buffer[cookie->_start], tocopy);
		cookie->_start += tocopy;
		dest += tocopy;
		size -= tocopy;
	}

	/* Main receiving loop. */

	while (size) {
		ssize_t recv;
		size_t tocopy;

		/* Receive. */

		recv = read(fd, cookie->_buffer, BUFSIZE);
		if (!recv)
			continue;

		/* Check error. */

		if (recv < 0) switch (errno) {
			case 0: continue;
			case ENODEV: case EIO:
				return (casio_error_nocalc);
			default:
				msg((ll_fatal, "error was %d: %s",
					errno, strerror(errno)));
				return (casio_error_unknown);
		}

		/* Get the current size to copy. */

		tocopy = (size_t)recv;
		if (tocopy > size) tocopy = size;

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
