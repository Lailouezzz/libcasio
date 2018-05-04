/* ****************************************************************************
 * stream/skip.c -- skip bytes from a stream.
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
 *	casio_skip:
 *	Skip data from a stream (read).
 *
 *	@arg	stream		the stream to skip data from.
 *	@arg	size		the size to skip.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_skip(casio_stream_t *stream, size_t size)
{
	/* Use what has been implemented in the `casio_seek()` function to
	 * skip N bytes. */

	return (casio_seek(stream, (casio_off_t)size, CASIO_SEEK_CUR));
}
