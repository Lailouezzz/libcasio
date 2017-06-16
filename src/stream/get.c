/* ****************************************************************************
 * stream/get.c -- stream getters.
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
#undef casio_isreadable
#undef casio_iswritable

/**
 *	casio_get_type:
 *	Get the stream type.
 *
 *	@arg	stream		the stream.
 *	@return				the type.
 */

casio_streamtype_t CASIO_EXPORT casio_get_type(casio_stream_t *stream)
{
	if (!stream) return (0);
	return (stream->casio_stream_type);
}

/**
 *	casio_get_openmode:
 *	Get the open mode.
 *
 *	@arg	stream		the stream.
 *	@return				the type.
 */

casio_openmode_t CASIO_EXPORT casio_get_openmode(casio_stream_t *stream)
{
	if (!stream) return (0);
	return (stream->casio_stream_mode);
}

/**
 *	casio_isreadable:
 *	Check if the stream is readable.
 *
 *	@arg	stream		the stream.
 *	@return				the type.
 */

int CASIO_EXPORT casio_isreadable(casio_stream_t *stream)
{
	return (casio_get_openmode(stream) & CASIO_OPENMODE_READ);
}

/**
 *	casio_iswritable:
 *	Check if the stream is readable.
 *
 *	@arg	stream		the stream.
 *	@return				the type.
 */

int CASIO_EXPORT casio_iswritable(casio_stream_t *stream)
{
	return (casio_get_openmode(stream) & CASIO_OPENMODE_WRITE);
}

/**
 *	casio_get_cookie:
 *	Get the stream cookie.
 *
 *	@arg	stream		the stream.
 *	@return				the cookie.
 */

void* CASIO_EXPORT casio_get_cookie(casio_stream_t *stream)
{
	if (!stream) return (NULL);
	return (stream->casio_stream_cookie);
}

/**
 *	casio_get_lasterr:
 *	Get the last error.
 *
 *	@arg	stream		the stream.
 *	@return				the last error.
 */

int CASIO_EXPORT casio_get_lasterr(casio_stream_t *stream)
{
	if (!stream) return (0);
	return (stream->casio_stream_lasterr);
}

/**
 *	casio_get_attrs:
 *	Get the attributes of a libcasio stream.
 *
 *	@arg	stream		the stream to get the attributes of.
 *	@arg	attrs		the attrs to set.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_get_attrs(casio_stream_t *stream,
	casio_streamattrs_t *attrs)
{
	if (!getcb(stream, setattrs)) return (casio_error_op);
	memcpy(attrs, &stream->casio_stream_attrs,
		sizeof(casio_streamattrs_t)); /* DANGEROUS: ABI COMPAT XXX */
	return (0);
}

/**
 *	casio_get_timeouts:
 *	Get the timeouts of a libcasio stream.
 *
 *	@arg	stream		the stream to get the timeouts of.
 *	@arg	timeouts	the timeouts to get.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_get_timeouts(casio_stream_t *stream,
	casio_timeouts_t *timeouts)
{
	if (!getcb(stream, settm)) return (casio_error_op);
	memcpy(timeouts, &stream->casio_stream_timeouts,
		sizeof(casio_timeouts_t)); /* DANGEROUS: ABI COMPAT XXX */
	return (0);
}
