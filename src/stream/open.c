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
#define checknot(CASIO__COND, CASIO__ERR) \
	{ if (CASIO__COND) {err = CASIO__ERR; goto fail;}}

/**
 *	casio_open:
 *	Open a libcasio stream.
 *
 *	@arg	pstream		the stream to open.
 *	@arg	mode		the open mode.
 *	@arg	type		the stream type.
 *	@arg	cookie		the stream cookie.
 *	@arg	callbacks	the stream callbacks.
 *	@return				the error code (0 if ok).
 */

int casio_open(casio_stream_t **pstream, casio_openmode_t mode,
	casio_streamtype_t type, void *cookie,
	const casio_streamfuncs_t *callbacks)
{
	int err; casio_stream_t *stream = NULL;
	casio_streamfuncs_t *c;

	/* allocate the stream */
	*pstream = malloc(sizeof(casio_stream_t));
	stream = *pstream;
	checknot(stream == NULL, casio_error_alloc)

	/* initialize the stream callbacks */
	c = &stream->casio_stream_callbacks;
	memset(c, 0, sizeof(casio_streamfuncs_t));
	c->casio_streamfuncs_close = callbacks->casio_streamfuncs_close;
	c->casio_streamfuncs_settm = callbacks->casio_streamfuncs_settm;
	if (!type
	|| (type & (casio_streamtype_usb | casio_streamtype_serial)
	&& ~type &  casio_streamtype_scsi)) {
		c->casio_streamfuncs_read =     callbacks->casio_streamfuncs_read;
		c->casio_streamfuncs_write =    callbacks->casio_streamfuncs_write;
		c->casio_streamfuncs_seek =     callbacks->casio_streamfuncs_seek; }
	if (type & casio_streamtype_serial)
		c->casio_streamfuncs_setattrs = callbacks->casio_streamfuncs_setattrs;
	if (type & casio_streamtype_scsi)
		c->casio_streamfuncs_scsi = callbacks->casio_streamfuncs_scsi;

	/* initialize the stream properties */
	stream->casio_stream_type = type;
	stream->casio_stream_mode = mode;
	stream->casio_stream_cookie = cookie;
	stream->casio_stream_offset = 0;
	stream->casio_stream_lasterr = 0;
	casio_init_attrs(stream);
	casio_init_timeouts(stream);

	/* no error! */
	err = 0;
fail:
	if (err) {
		free(stream);
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

int casio_close(casio_stream_t *stream)
{
	casio_stream_close_t *c;

	if (!stream) return (0);
	c = getcb(stream, close);
	if (c) (*c)(stream->casio_stream_cookie);
	free(stream);
	return (0);
}
