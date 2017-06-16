/* ****************************************************************************
 * file/decode/cas.c -- decode a CASIOLINK file.
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
 *
 * Based on the Casetta project documentation:
 * https://casetta.tuxfamily.org/formats/cas
 * ************************************************************************* */
#include "decode.h"

#define READGSIZE() if (grc) { \
	casio_uint16_t size; READ(&size, 2) \
	msg((ll_info, "GraphCard next buffer size was: 0x%04X", be16toh(size))); }
#define READCOLON() { \
	char colon; \
	READGSIZE() \
	GREAD(&colon, 1) \
	err = casio_error_magic; \
	if (colon != ':') { \
		msg((ll_error, "Expected ':', got '%c' (0x%02X)", colon, colon)); \
		goto fail; \
	}}

/**
 *	decode_cas:
 *	Decode a CAS file.
 *
 *	Is also sort of a guide for using the CAS MCS files.
 *	The colon ':' (0x3A) is already read at this point (CAS file id.).
 *
 *	@arg	h		the handle to create.
 *	@arg	buffer	the buffer to read from.
 *	@arg	grc		is a graphcard file.
 *	@return			the error.
 */

CASIO_LOCAL int decode_cas(casio_file_t **h, casio_stream_t *buffer, int grc)
{
	int err; casio_file_t *handle;

	/* make the handle */
	err = casio_make_mcs(h, 0);
	if (err) return (err);
	handle = *h;

	/* read each */
	for (handle->casio_file_count = 0;;) {
		int j; casio_mcsfile_t *mcsfile; casio_mcshead_t head;

		/* read the head */
		msg((ll_info, "Reading the next head."));
		if ((err = casio_decode_casfile_head(&head, buffer))) goto fail;
		if (head.casio_mcshead_type == casio_mcstype_end) break;

		/* prepare the file */
		msg((ll_info, "Preparing the file."));
		err = casio_make_mcsfile(&mcsfile, &head);
		if (err) goto fail;

		/* read each part */
		for (j = 1; mcsfile->casio_mcsfile_head.casio_mcshead_flags
		   & casio_mcsflag_unfinished; j++) {
			/* initialize */
			msg((ll_info, "Reading part #%d", j));
			READCOLON()

			/* read the part */
			err = casio_decode_casfile_part(mcsfile, buffer);
			if (err) goto fail;
		}

		/* put the file into the MCS */
		err = casio_put_mcsfile(handle->casio_file_mcs, mcsfile, 1);
		if (err) goto fail;

		/* read first colon of the next part */
		READCOLON()
	}

	/* read last size (check if zero?) */
	READGSIZE()

	/* everything went well :) */
	return (0);

fail:
	/* END OF DA WORLD */
	casio_free_file(handle);
	return (err);
}

/* ************************************************************************* */
/*  Public decoding functions                                                */
/* ************************************************************************* */
/**
 *	casio_decode_cas:
 *	Decode a CAS file.
 *
 *	@arg	handle	the handle to make.
 *	@arg	buffer	the buffer to read from.
 *	@return			the error.
 */

int CASIO_EXPORT casio_decode_cas(casio_file_t **handle,
	casio_stream_t *buffer)
{
	return (decode_cas(handle, buffer, 0));
}

/**
 *	casio_decode_grc:
 *	Decode Graph Card file.
 *
 *	@arg	handle		the handle to make.
 *	@arg	buffer		the buffer to read from.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_decode_grc(casio_file_t **handle,
	casio_stream_t *buffer)
{
	return (decode_cas(handle, buffer, 1));
}
