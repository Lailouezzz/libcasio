/* ****************************************************************************
 * mcsfile/decode/cas/program.c -- decode a CAS program.
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
 *	casio_decode_caspart_program:
 *	Decode a CAS program part.
 *
 *	@arg	handle		the handle to contribute to.
 *	@arg	buffer		the buffer to read from.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_decode_caspart_program(casio_mcsfile_t *handle,
	casio_stream_t *buffer)
{
	unsigned char checksum, csum;

	/* get content */
	READ(handle->casio_mcsfile_content,
		handle->casio_mcsfile_head.casio_mcshead_size)
	msg((ll_info, "Program content is:"));
	mem((ll_info, handle->casio_mcsfile_content,
		handle->casio_mcsfile_head.casio_mcshead_size));

	/* check the sum */
	DREAD(checksum)
	csum = casio_checksum_cas(handle->casio_mcsfile_content,
		handle->casio_mcsfile_head.casio_mcshead_size, 0);
	if (checksum != csum) {
		msg((ll_error, "Checksum mismatch: expected 0x%02X, got 0x%02X",
			csum, checksum));
		return (casio_error_csum);
	}

	/* everything went well :) */
	handle->casio_mcsfile_head.casio_mcshead_flags &=
		~casio_mcsflag_unfinished;
	return (0);
}
