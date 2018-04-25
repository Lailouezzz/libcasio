/* ****************************************************************************
 * stream/builtin/csum32.c -- built-in stream for making a 32-bit checksum.
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

/* cookie structure */
struct thecookie {
	casio_stream_t *_stream;
	casio_uint32_t *_checksum;
};

/* ************************************************************************* */
/*  Callbacks                                                                */
/* ************************************************************************* */
/**
 *	csum32_read:
 *	Read from this stream.
 *
 *	@arg	cookie		the cookie.
 *	@arg	data		the data pointer.
 *	@arg	size		the data size.
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL int csum32_read(struct thecookie *cookie,
	unsigned char *dest, size_t size)
{
	int err;

	/* Make the call. */

	err = casio_read(cookie->_stream, dest, size);
	if (err) return (err);

	/* Make the checksum. */

	*cookie->_checksum = casio_checksum32(dest, size, *cookie->_checksum);
	return (0);
}

/**
 *	csum32_close:
 *	Close the stream.
 *
 *	@arg	cookie		the cookie.
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL int csum32_close(struct thecookie *cookie)
{
	casio_free(cookie);
	return (0);
}

/* Callbacks. */
CASIO_LOCAL const casio_streamfuncs_t csum32_callbacks =
casio_stream_callbacks_for_virtual(csum32_close,
	csum32_read, NULL, NULL);

/* ************************************************************************* */
/*  Main functions                                                           */
/* ************************************************************************* */
/**
 *	casio_open_csum32:
 *	Open a 32-bit checksum stream.
 *
 *	@arg	stream		the stream to make.
 *	@arg	original	the original stream.
 *	@arg	csum		the checksum pointer.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_open_csum32(casio_stream_t **stream,
	casio_stream_t *original, casio_uint32_t *csum)
{
	struct thecookie *cookie = NULL;
	casio_openmode_t openmode;

	/* FIXME: check original stream */
	/* Allocate the cookie. */

	cookie = casio_alloc(1, sizeof(struct thecookie));
	if (!cookie) return (casio_error_alloc);

	/* Fill the cookie. */

	cookie->_stream   = original;
	cookie->_checksum = csum;

	/* Initialize and return da stream. */

	openmode = casio_get_openmode(original) & CASIO_OPENMODE_READ;
	return (casio_open_stream(stream, openmode, cookie, &csum32_callbacks, 0));
}
