/* ****************************************************************************
 * stream/builtin/memory.c -- built-in memory stream.
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
typedef struct {
	unsigned char *_memory;
	casio_off_t _size, _offset;
} memory_cookie_t;

/* ************************************************************************* */
/*  Callbacks                                                                */
/* ************************************************************************* */
/**
 *	casio_memory_read:
 *	Read from a memory area.
 *
 *	@arg	vcookie		the cookie (uncasted)
 *	@arg	data		the data pointer.
 *	@arg	size		the data size.
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL int casio_memory_read(void *vcookie, unsigned char *dest, size_t size)
{
	memory_cookie_t *cookie = (void*)vcookie;

	if (((size_t)-1 - cookie->_offset) < size) /* overflow */
		return (casio_error_read);
	if (cookie->_offset + (casio_off_t)size > cookie->_size) {
		cookie->_offset = cookie->_size - 1;
		return (casio_error_eof);
	}

	memcpy(dest, &cookie->_memory[cookie->_offset], size);
	cookie->_offset += size;

	return (0);
}

/**
 *	casio_memory_write:
 *	Write on a memory area.
 *
 *	@arg	vcookie		the cookie (uncasted).
 *	@arg	data		the data pointer.
 *	@arg	size		the data size.
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL int casio_memory_write(void *vcookie, const unsigned char *data,
	size_t size)
{
	memory_cookie_t *cookie = (void*)vcookie;

	if (((size_t)-1 - cookie->_offset) < size) /* overflow */
		return (casio_error_write);
	if (cookie->_offset + (casio_off_t)size > cookie->_size) {
		cookie->_offset = cookie->_size - 1;
		return (casio_error_eof);
	}

	memcpy(&cookie->_memory[cookie->_offset], data, size);
	cookie->_offset += size;

	return (0);
}

/**
 *	casio_memory_seek:
 *	Move within a memory area.
 *
 *	@arg	vcookie		the cookie (uncasted).
 *	@arg	offset		the offset.
 *	@arg	whence		the whence.
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL int casio_memory_seek(void *vcookie, casio_off_t *offset,
	casio_whence_t whence)
{
	memory_cookie_t *cookie = (void*)vcookie;
	casio_off_t off;

	/* Get the offset. */

	switch (whence) {
	case CASIO_SEEK_CUR:
		off = cookie->_offset + *offset;
		break;
	case CASIO_SEEK_END:
		off = cookie->_size - *offset;
		break;
	default /* CASIO_SEEK_SET */:
		off = *offset;
		break;
	}

	/* Check the bounds. */

	if (off < 0)
		off = 0;
	else if (off >= cookie->_size)
		off = cookie->_size - 1;

	/* Set the offset, return. */

	*offset = off;
	cookie->_offset = off;
	return (0);
}

/**
 *	casio_memory_close:
 *	Close a FILE cookie.
 *
 *	@arg	vcookie		the cookie (uncasted)
 *	@return				the error code (0 if ok)
 */

CASIO_LOCAL int casio_memory_close(void *vcookie)
{
	casio_free(vcookie);
	return (0);
}

/* Callbacks. */
CASIO_LOCAL const casio_streamfuncs_t casio_memory_callbacks =
casio_stream_callbacks_for_virtual(casio_memory_close,
	casio_memory_read, casio_memory_write, casio_memory_seek);

/* ************************************************************************* */
/*  Opening functions                                                        */
/* ************************************************************************* */
/**
 *	casio_open_memory:
 *	Open a FILE stream.
 *
 *	@arg	stream		the stream to make.
 *	@arg	rstream		the FILE stream to read from.
 *	@arg	wstream		the FILe stream to write to.
 *	@return				the error (0 if ok).
 */

int CASIO_EXPORT casio_open_memory(casio_stream_t **stream,
	const void *memory, size_t size)
{
	memory_cookie_t *cookie = NULL;

	/* Check things. */

	if (!memory || !size)
		return (casio_error_nostream);

	/* Allocate the cookie. */

	cookie = casio_alloc(1, sizeof(memory_cookie_t));
	if (!cookie) return (casio_error_alloc);

	/* Fill the cookie. */

	cookie->_memory = (void*)memory;
	cookie->_size = size;
	cookie->_offset = 0;

	/* Initialize and return da stream. */

	return (casio_open_stream(stream, CASIO_OPENMODE_READ, cookie,
		&casio_memory_callbacks, 0));
}
