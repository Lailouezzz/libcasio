/* ****************************************************************************
 * mcsfile/decode/mcs/string.c.c -- decode an MCS string.
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
#include "../decode.h"

/**
 *	casio_decode_mcs_string:
 *	Decode an MCS string.
 *
 *	@arg	h			the handle to make.
 *	@arg	buffer		the buffer to read from.
 *	@arg	head		the pre-filled head to complete and use.
 *	@return				the error code (0 if ok).
 */

int casio_decode_mcs_string(casio_mcsfile_t **h, casio_stream_t *buffer,
	casio_mcshead_t *head)
{
	int err; unsigned char *str = NULL;
	unsigned long length = head->casio_mcshead_size;

	/* print content */
	msg((ll_info, "String MCS file is not managed yet. Content:"));
	str = malloc(length);
	if (!str) { err = casio_error_alloc; goto fail; }
	GREAD(str, length)
	mem((ll_info, str, length));

	/* make the file */
	err = casio_make_mcsfile(h, head);
	if (err) goto fail;

	/* end */
	err = 0;
fail:
	free(str);
	return (err);
}
