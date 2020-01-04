/* ****************************************************************************
 * stream/write.c -- write to a stream.
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
 *	casio_write:
 *	Write to a libcasio stream.
 *
 *	@arg	stream		the stream to write to.
 *	@arg	data		the data to write.
 *	@arg	size		the amount of bytes to write.
 *	@return				the size written if > 0, or if < 0 the error code is -[returned value].
 */

ssize_t CASIO_EXPORT casio_write(casio_stream_t *stream,
	const void *data, size_t size)
{
	int err = casio_error_ok;
	ssize_t ssize = 0;

	/* check if we can write */
	failure(~stream->casio_stream_mode & CASIO_OPENMODE_WRITE, 
		casio_error_write);

	/* write */
	if (size == 0) {
		return (0);
	}
	ssize = (*getcb(stream, write))(stream->casio_stream_cookie, data, size);
	
	if (ssize < 0) {
		err = -ssize;
		msg((ll_error, "Stream reading failure: %s", casio_strerror(err)));
		goto fail;
	}

	/* move the cursor and return */
	if(ssize >= 0) {
		stream->casio_stream_offset += size;
	}
fail:
	stream->casio_stream_lasterr = err;
	return ssize;
}

/**
 *	casio_write_char:
 *	Write a character to a libcasio stream.
 *
 *	@arg	stream		the stream to write to.
 *	@arg	car			the character to write.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_write_char(casio_stream_t *stream, int car)
{
	unsigned char ccar = car;

	ssize_t ssize = casio_write(stream, &ccar, 1);
	return (ssize < 0 ? (int)-ssize : 0);
}
