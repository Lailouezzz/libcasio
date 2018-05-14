/* ****************************************************************************
 * file/decode/decode.c -- decode a "standard" CASIO file.
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
#include "decode.h"

/**
 *	casio_decode_storage:
 *	Decode a fx-9860G storage memory.
 *
 *	@arg	handlep		the handle to create.
 *	@arg	buffer		the buffer to read from.
 */

int CASIO_EXPORT casio_decode_storage(casio_file_t **handlep,
	casio_stream_t *buffer)
{
	/* TODO: code this function. */

	(void)handlep;
	(void)buffer;

	msg((ll_info, "storage memory decoding is not available yet."));
	return (casio_error_op);
}
