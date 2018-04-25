/* ****************************************************************************
 * stream/size.c -- make out the size of a stream.
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
 *	casio_getsize:
 *	Make out the size of a stream.
 *
 *	@arg	stream		the stream.
 *	@arg	size		the size to make out.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_getsize(casio_stream_t *stream, casio_off_t *size)
{
	int err; casio_off_t initial_offset;

	initial_offset = casio_tell(stream);
	if ((err = casio_seek(stream, 0, CASIO_SEEK_END)))
		return (err);
	*size = casio_tell(stream);
	if ((err = casio_seek(stream, initial_offset, CASIO_SEEK_SET)))
		return (err);

	return (0);
}
