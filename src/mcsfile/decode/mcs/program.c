/* ****************************************************************************
 * mcsfile/decode/mcs/program.c -- decode an MCS program.
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
 *	casio_decode_mcs_program:
 *	Decode a program.
 *
 *	@arg	h			the handle to make.
 *	@arg	buffer		the buffer to read from.
 *	@arg	head		the pre-filled head to complete and use.
 *	@return				the error code (0 if ok).
 */

int casio_decode_mcs_program(casio_mcsfile_t **h, casio_stream_t *buffer,
	casio_mcshead_t *head)
{
	int err;
	casio_mcs_programheader_t hd;
	casio_mcsfile_t *handle;
	size_t content_size;

	/* read header */
	DREAD(hd)
	msg((ll_info, "Program password is '%.8s'.",
		hd.casio_mcs_programheader_password));

	/* make final head and file */
	strncpy(head->casio_mcshead_password,
		(char*)hd.casio_mcs_programheader_password, 8);
	head->casio_mcshead_password[8] = 0;
	head->casio_mcshead_size -= sizeof(casio_mcs_programheader_t);
	head->casio_mcshead_size += 1; /* terminating zero - XXX: keep it? */
	err = casio_make_mcsfile(h, head);
	if (err) return (err);

	/* get content */
	handle = *h;
	content_size = head->casio_mcshead_size - 1;
	msg((ll_info, "Getting program content (%" CASIO_PRIuSIZE "o)",
		content_size));
	GREAD(handle->casio_mcsfile_content, content_size);
	handle->casio_mcsfile_content[content_size] = 0;

	/* no error */
	err = 0;
fail:
	if (err) {
		casio_free_mcsfile(*h);
		*h = NULL;
	}
	return (err);
}
