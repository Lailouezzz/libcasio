/* ****************************************************************************
 * stream/scsi.c -- make a SCSI request.
 * Copyright (C) 2018 Thomas "Cakeisalie5" Touhey <thomas@touhey.fr>
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
 *	casio_scsi_request:
 *	Make a SCSI request.
 *
 *	@arg	stream		the stream to use.
 *	@arg	request		the request to execute.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_scsi_request(casio_stream_t *stream,
	casio_scsi_t *request)
{
	int err;
	casio_stream_scsi_t *s;

	s = getcb(stream, scsi);
	if (!s)
		return (casio_error_op);

	err = (*s)(stream->casio_stream_cookie, request);
	failure(err, err)

	err = 0;
fail:
	stream->casio_stream_lasterr = err;
	return (err);
}
