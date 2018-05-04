/* ****************************************************************************
 * stream/seek.c -- move in a stream.
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
 *	casio_seek:
 *	Move in a stream.
 *
 *	@arg	stream		the stream to move into.
 *	@arg	offset		the offset.
 *	@arg	whence		the whence.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_seek(casio_stream_t *stream, casio_off_t offset,
	casio_whence_t whence)
{
	int err; casio_stream_seek_t *s;

	failure(whence != CASIO_SEEK_SET && whence != CASIO_SEEK_CUR
	 && whence != CASIO_SEEK_END, casio_error_op)

	/* Check if the job isn't already done. */

	if ((whence == CASIO_SEEK_CUR && !offset)
	 || (whence == CASIO_SEEK_SET && offset == stream->casio_stream_offset))
		return (0);

	/* Try to seek using the dedicated function. */

	s = getcb(stream, seek);
	if (s) {
		/* The callback exists, let's use it! */

		err = (*s)(stream->casio_stream_cookie, &offset, whence);
		failure(err, err)
	} else if (whence == CASIO_SEEK_CUR && offset > 0
	 && stream->casio_stream_mode & CASIO_OPENMODE_READ) {
		/* Fallback mehod for skipping bytes: read until N bytes have
		 * been skipped. */

		{
			unsigned char buf[128];
			size_t to_skip = (size_t)offset;

			do {
				size_t rd = min(to_skip, 128);

				/* Read that much from the stream. */

				if ((err = casio_read(stream, buf, rd)))
					goto fail;

				to_skip -= rd;
			} while (to_skip);
		}
	} else {
		err = casio_error_op;
		goto fail;
	}

	stream->casio_stream_offset = offset;
	err = 0;
fail:
	stream->casio_stream_lasterr = err;
	return (err);
}

/**
 *	casio_tell:
 *	Tell the position in a stream.
 *
 *	@arg	stream		the stream to get the position of.
 *	@return				the position.
 */

casio_off_t CASIO_EXPORT casio_tell(casio_stream_t *stream)
{
	return (stream->casio_stream_offset);
}
