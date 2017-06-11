/* ****************************************************************************
 * file/open.c -- open and decode a file.
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
#include "file.h"

/**
 *	casio_open_file:
 *	Open and decode a file.
 *
 *	FIXME: implement other interfaces than `FILE`, such as
 *	platform-specific things like STREAMS or Windows files.
 *
 *	@arg	handle		the handle to make.
 *	@arg	path		the path to open.
 *	@arg	types		the expected types (0 if none).
 *	@return				the error code (0 if ok).
 */

int casio_open_file(casio_file_t **handle, const char *path,
	casio_filetype_t expected_types)
{
	int err; FILE *file = NULL;
	casio_stream_t *stream = NULL;

	/* Open the file using `stdio.h`. */
	file = fopen(path, "r");
	if (!file) return (casio_error_unknown);

	/* Open the stream. */
	err = casio_open_stream_file(&stream, file, file, 1, 1);
	if (err) { err = casio_error_alloc; goto fail; }

	/* Decode using this file. */
	err = casio_decode(handle, path, stream, expected_types);
	if (err) goto fail;

	err = 0;
fail:
	if (stream) casio_close(stream);
	return (err);
}
