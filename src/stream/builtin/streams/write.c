/* ****************************************************************************
 * stream/builtin/streams/write.c -- write to a STREAMS stream.
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
 *	casio_streams_write:
 *	Write to a terminal.
 *
 *	@arg	cookie		the cookie.
 *	@arg	data		the source.
 *	@arg	size		the source size.
 *	@return				the error (0 if ok).
 */

int CASIO_EXPORT casio_streams_write(streams_cookie_t *cookie,
	const unsigned char *data, size_t size)
{
	int fd = cookie->_writefd;

	/* send */
	while (size) {
		ssize_t wr = write(fd, data, size);
		if (wr < 0) break;
		size -= (size_t)wr;
	}

	/* be sure it's written, or check the error */
	if (size) switch (errno) {
		case ENODEV:
			return (casio_error_nocalc);
		default:
			msg((ll_fatal, "errno was %d: %s", errno, strerror(errno)));
			return (casio_error_unknown);
	}

	/* no error! */
	return (0);
}

#endif
