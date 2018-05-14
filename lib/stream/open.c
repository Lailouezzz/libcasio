/* ****************************************************************************
 * stream/open.c -- open a stream.
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
 *	casio_open_stream:
 *	Open a libcasio stream.
 *
 *	@arg	pstream		the stream to open.
 *	@arg	mode		the open mode.
 *	@arg	cookie		the stream cookie.
 *	@arg	callbacks	the stream callbacks.
 *	@arg	ioff		the initial offset.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_open_stream(casio_stream_t **pstream,
	casio_openmode_t mode, void *cookie,
	const casio_streamfuncs_t *callbacks,
	casio_off_t ioff)
{
	int err; casio_stream_t *stream = NULL;
	casio_streamfuncs_t *c;

	/* Allocate the stream. */

	*pstream = casio_alloc(1, sizeof(casio_stream_t));
	if (!(stream = *pstream)) {
		err = casio_error_alloc;
		goto fail;
	}

	/* Initialize the stream callbacks. */

	stream->casio_stream_mode = 0;
	c = &stream->casio_stream_callbacks;
	memset(c, 0, sizeof(casio_streamfuncs_t));
	c->casio_streamfuncs_close = callbacks->casio_streamfuncs_close;
	c->casio_streamfuncs_settm = callbacks->casio_streamfuncs_settm;
	if ((mode & CASIO_OPENMODE_READ) && callbacks->casio_streamfuncs_read) {
		stream->casio_stream_mode |= CASIO_OPENMODE_READ;
		c->casio_streamfuncs_read  = callbacks->casio_streamfuncs_read;
	}
	if ((mode & CASIO_OPENMODE_WRITE) && callbacks->casio_streamfuncs_write) {
		stream->casio_stream_mode |= CASIO_OPENMODE_WRITE;
		c->casio_streamfuncs_write = callbacks->casio_streamfuncs_write;
	}
	if (mode & (CASIO_OPENMODE_READ | CASIO_OPENMODE_WRITE))
		c->casio_streamfuncs_seek  = callbacks->casio_streamfuncs_seek;

	if ((mode & CASIO_OPENMODE_SERIAL)
	 && callbacks->casio_streamfuncs_setattrs) {
		stream->casio_stream_mode |= CASIO_OPENMODE_SERIAL;
		c->casio_streamfuncs_setattrs = callbacks->casio_streamfuncs_setattrs;
	}
	if ((mode & CASIO_OPENMODE_SCSI) && callbacks->casio_streamfuncs_scsi) {
		stream->casio_stream_mode |= CASIO_OPENMODE_SCSI;
		c->casio_streamfuncs_scsi = callbacks->casio_streamfuncs_scsi;
	}

	/* Initialize the stream properties. */

	stream->casio_stream_cookie = cookie;
	stream->casio_stream_offset = ioff;
	stream->casio_stream_lasterr = 0;
	casio_init_attrs(stream);
	casio_init_timeouts(stream);

	err = 0;
fail:
	if (err) {
		casio_free(stream);
		(*callbacks->casio_streamfuncs_close)(cookie);
	}
	return (err);
}

/**
 *	casio_close:
 *	Close a libcasio stream.
 *
 *	@arg	stream		the stream to close.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_close(casio_stream_t *stream)
{
	casio_stream_close_t *c;

	if (!stream) return (0);
	c = getcb(stream, close);
	if (c) (*c)(stream->casio_stream_cookie);
	casio_free(stream);
	return (0);
}
