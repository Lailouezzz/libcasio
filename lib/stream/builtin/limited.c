/* ****************************************************************************
 * stream/builtin/limited.c -- built-in limited stream.
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
#include "../../internals.h"

/* Cookie structure. */

typedef struct {
	char            _magic[8];
	casio_stream_t *_stream;
	size_t          _left;
} limited_cookie_t;

/* ---
 * Callbacks.
 * --- */

/**
 *	casio_limited_read:
 *	Read from a limited stream.
 *
 *	@arg	vcookie		the cookie (uncasted).
 *	@arg	data		the data pointer.
 *	@arg	size		the data size.
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL int casio_limited_read(void *vcookie, unsigned char *dest,
	size_t *psize)
{
	int err; limited_cookie_t *cookie = (void*)vcookie;
	size_t left; size_t size = *psize;

	if (size > cookie->_left) {
		/* First, skip the required bytes. */

		left = cookie->_left;
		cookie->_left = 0;
		if ((err = casio_skip(cookie->_stream, left)))
			return (err);

		/* Once the skip is done successfully, we return that we
		 * have an EOF. */

		return (casio_error_eof);
	}

	if ((err = casio_read(cookie->_stream, dest, &size))) {
		cookie->_left = 0; /* XXX: depends on the error? */
		return (err);
	}
	cookie->_left -= size;
	return (0);
}

/**
 *	casio_limited_close:
 *	Close a limited stream.
 *
 *	@arg	vcookie		the cookie (uncasted).
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL int casio_limited_close(void *vcookie)
{
	casio_free(vcookie);
	return (0);
}

/* Callbacks. */

CASIO_LOCAL const casio_streamfuncs_t casio_limited_callbacks =
casio_stream_callbacks_for_virtual(casio_limited_close,
	casio_limited_read, NULL, NULL);

/* ---
 * Main functions.
 * --- */

/**
 *	casio_open_limited:
 *	Open a limited stream.
 *
 *	@arg	stream		the stream to make.
 *	@arg	original	the original stream.
 *	@arg	size		the size the limited buffer is limited to.
 *	@return				the error (0 if ok).
 */

int CASIO_EXPORT casio_open_limited(casio_stream_t **stream,
	casio_stream_t *original, size_t size)
{
	limited_cookie_t *cookie = NULL;

	/* FIXME: check original stream */
	/* allocate the cookie */

	cookie = casio_alloc(1, sizeof(limited_cookie_t));
	if (!cookie)
		return (casio_error_alloc);

	/* Fill the cookie. */

	memcpy(cookie->_magic, "LIMITED\x7F", 8);
	cookie->_stream = original;
	cookie->_left = size;

	/* Initialize da stream. */

	return (casio_open_stream(stream,
		casio_get_openmode(original) & CASIO_OPENMODE_READ,
		cookie, &casio_limited_callbacks, 0));
}

/**
 *	casio_empty_limited:
 *	Empty a limited stream.
 *
 *	@arg	stream		the stream to empty.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_empty_limited(casio_stream_t *stream)
{
	int err; limited_cookie_t *cookie = (void*)casio_get_cookie(stream);

	if (!cookie || memcmp(cookie->_magic, "LIMITED\x7F", 8))
		return (casio_error_op);
	err = casio_skip(cookie->_stream, cookie->_left);
	cookie->_left = 0;
	return (err);
}
