/* ****************************************************************************
 * stream/read.c -- read from a stream.
 * Copyright (C) 2017 Thomas "Cakeisalie5" Touhey <thomas@touhey.fr>
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
#include "stream.h"

/**
 *	casio_read:
 *	Read from a libcasio stream.
 *
 *	@arg	stream		the stream to read from.
 *	@arg	dest		the destination buffer.
 *	@arg	size		the amount of bytes to read.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_read(casio_stream_t *stream, void *dest, size_t *psize)
{
	int err;

	/* check if we can read */
	failure(~stream->casio_stream_mode & CASIO_OPENMODE_READ, casio_error_read)

	/* read */
	if (!(*psize)) return (0);
	if (!psize) {
		size_t size = -1;
		err = (*getcb(stream, read))(stream->casio_stream_cookie, dest, &size);
	} else {
		err = (*getcb(stream, read))(stream->casio_stream_cookie, dest, psize);
	}
	if (err && err != casio_error_ieof) {
		msg((ll_error, "Stream reading failure: %s", casio_strerror(err)));
		goto fail;
	}

	/* move the cursor and return */
	stream->casio_stream_offset += *psize;
fail:
	stream->casio_stream_lasterr = err;
	return (err);
}
