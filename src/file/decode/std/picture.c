/* ****************************************************************************
 * file/decode/std/picture.c -- decode a picture file.
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
#include <zlib.h>

/* ************************************************************************* */
/*  Obfuscation-related functions                                            */
/* ************************************************************************* */
/**
 *	g3p_deobfuscate:
 *	De-obfuscate the image data.
 *
 *	@arg	buf			the buffer.
 *	@arg	n			the buffer size.
 */

static void g3p_deobfuscate(uint8_t *buf, size_t n)
{
	while (n--) {
		uint8_t byte = ~*buf;
		*buf++ = (byte << 5) | (byte >> 3);
	}
}
/* ************************************************************************* */
/*  Prizm Picture decoding function                                          */
/* ************************************************************************* */
/**
 *	casio_decode_std_g3p:
 *	Decode a G3P file.
 *
 *	@arg	handle		the handle.
 *	@arg	buffer		the buffer to read from.
 *	@arg	std			the standard header.
 *	@arg	pic			the standard picture header.
 *	@return				the error code (0 if ok).
 */

int casio_decode_std_g3p(casio_file_t **h, casio_stream_t *buffer,
	casio_standard_header_t *std, casio_standard_picheader_t *pic)
{
	int err = casio_error_alloc, z_err;
	casio_prizm_picture_subheader_t ihd;
	unsigned int width, height; casio_uint16_t gen;
	casio_uint16_t rawcol; const char *coldesc; size_t rawsize;
	uLong inflated_size, adl;
	size_t deflated_size; casio_uint32_t checksum;
	casio_pictureformat_t picfmt;
	casio_uint8_t *defbuf = NULL, *infbuf = NULL;
	casio_file_t *handle;

	*h = NULL; (void)pic;
	/* read the image header */
	DREAD(ihd)
	width = be16toh(ihd.casio_prizm_picture_subheader_width);
	height = be16toh(ihd.casio_prizm_picture_subheader_height);
	msg((ll_info, "Width: %upx, height: %upx", width, height));
	rawcol = be16toh(ihd.casio_prizm_picture_subheader_color_depth);
	switch (rawcol) {
		case g3p_color_4bit: picfmt = casio_pictureformat_4bit_code;
			coldesc = "4-bit"; break;
		case g3p_color_16bit: picfmt = casio_pictureformat_16bit;
			coldesc = "16-bit"; break;
		default:
			msg((ll_error, "Unknown picture format: 0x%04" PRIX16, rawcol));
			err = casio_error_magic; goto fail;
	}
	msg((ll_info, "Picture format: %s", coldesc));
	gen = be16toh(ihd.casio_prizm_picture_subheader_generator_id);
	msg((ll_info, "Generator ID: 0x%04X", gen));
	msg((ll_info, "-"));

	/* read deflated image */
	deflated_size = be32toh(ihd.casio_prizm_picture_subheader_data_size)
		- 6; /* FIXME: dangerous */
	msg((ll_info, "Reading %" CASIO_PRIuSIZE "B of deflated data",
		deflated_size));
	err = casio_error_alloc;
	if (!(defbuf = malloc(deflated_size))) goto fail;
	READ(defbuf, deflated_size)

	/* unobfuscate if required */
	if (std->casio_standard_header_obfuscated0) {
		msg((ll_info, "Is obfuscated, let's deobfuscate!"));
		g3p_deobfuscate(defbuf, deflated_size);
	}

	/* make the destination buffer */
	rawsize = casio_get_picture_size(NULL, picfmt, width, height);
	err = casio_error_alloc;
	if (!(infbuf = malloc(rawsize))) goto fail;

	/* uncompress */
	inflated_size = (uLong)rawsize;
	z_err = uncompress(infbuf, &inflated_size,
		defbuf, (uLong)deflated_size);
	if (z_err) {
		msg((ll_fatal, "Zlib error: error #%d", z_err));
		err = casio_error_magic;
		goto fail;
	}
	free(defbuf); defbuf = NULL;

	/* check the checksum */
	READ(&checksum, sizeof(casio_uint32_t))
	if (std->casio_standard_header_obfuscated0)
		g3p_deobfuscate((void*)&checksum, sizeof(casio_uint32_t));
	checksum = be32toh(checksum);
	adl = adler32(0, infbuf, inflated_size);
	if (adl != (uLong)checksum) {
		msg((ll_fatal, "Incorrect Adler32 checksum!"));
		msg((ll_fatal, "Expected 0x%08" CASIO_PRIX32 ", got 0x%08lX",
			checksum, adl));
		err = casio_error_csum;
		goto fail;
	}

	/* make the handle */
	err = casio_make_picture(h, ihd.casio_prizm_picture_subheader_width,
		ihd.casio_prizm_picture_subheader_height);
	if (err) goto fail;
	handle = *h;

	/* then store it */
	casio_decode_picture(handle->casio_file_pixels, infbuf,
		picfmt, handle->casio_file_width, handle->casio_file_height);
	free(infbuf); infbuf = NULL;

	/* TODO: footers? */

	/* no error */
	return (0);
fail:
	casio_free_file(*h); *h = NULL;
	free(infbuf); free(defbuf);
	return (err);
}
/* ************************************************************************* */
/*  Classpad Picture decoding function                                       */
/* ************************************************************************* */
/**
 *	casio_decode_std_c2p:
 *	Decode Classpad images.
 *
 *	@arg	handle		the handle.
 *	@arg	buffer		the buffer to read from.
 *	@arg	std			the standard header.
 *	@arg	pic			the standard picture header.
 *	@return				the error code (0 if ok).
 */

int casio_decode_std_c2p(casio_file_t **h, casio_stream_t *buffer,
	casio_standard_header_t *std, casio_standard_picheader_t *pic)
{
	(void)h; (void)buffer;
	(void)std; (void)pic;
	/* TODO */
	msg((ll_info, "C2P files are not managed yet."));

	/* no error */
	return (casio_error_magic);
}
