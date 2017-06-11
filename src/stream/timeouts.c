/* ****************************************************************************
 * stream/timeouts.c -- set the timeouts.
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
 *	casio_init_timeouts:
 *	Initialize the timeouts of a libcasio stream.
 *
 *	@arg	stream		the stream to initialize the timeouts of.
 *	@arg	timeouts	the timeouts to set.
 *	@return				the error code (0 if ok).
 */

int casio_init_timeouts(casio_stream_t *stream)
{
	static const casio_timeouts_t timeouts = {0, 0, 0};

	return (casio_set_timeouts(stream, &timeouts));
}

/**
 *	casio_set_timeouts:
 *	Set the timeouts of a libcasio stream.
 *
 *	@arg	stream		the stream to set the timeouts of.
 *	@arg	timeouts	the timeouts to set.
 *	@return				the error code (0 if ok).
 */

int casio_set_timeouts(casio_stream_t *stream,
	const casio_timeouts_t *timeouts)
{
	int err; casio_stream_settm_t *s;

	/* check if the callback exists */
	if (!stream) return (casio_error_op);
	s = getcb(stream, settm);
	failure(!s, casio_error_op)

	/* call it */
	err = (*s)(stream->casio_stream_cookie, timeouts);
	failure(err, err)

	/* copy timeouts */
	memcpy(&stream->casio_stream_timeouts, timeouts,
		sizeof(casio_timeouts_t)); /* ABI XXX */
	err = 0;
fail:
	stream->casio_stream_lasterr = err;
	return (err);
}
