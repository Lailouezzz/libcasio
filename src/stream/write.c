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
 *	@return				the error code (0 if ok).
 */

int casio_write(casio_stream_t *stream, const void *data, size_t size)
{
	int err;

	/* check if we can write */
	failure(~stream->casio_stream_mode & CASIO_OPENMODE_WRITE,
		casio_error_write)

	/* write */
	if (!size) return (0);
	err = (*getcb(stream, write))(stream->casio_stream_cookie, data, size);
	failure(err, err)

	/* move the cursor and return */
	stream->casio_stream_offset += size;
	err = 0;
fail:
	stream->casio_stream_lasterr = err;
	return (err);
}

/**
 *	casio_write_char:
 *	Write a character to a libcasio stream.
 *
 *	@arg	stream		the stream to write to.
 *	@arg	car			the character to write.
 *	@return				the error code (0 if ok).
 */

int casio_write_char(casio_stream_t *stream, int car)
{
	char ccar = car;

	return (casio_write(stream, &ccar, 1));
}
