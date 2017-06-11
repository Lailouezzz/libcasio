/* ****************************************************************************
 * file/decode/std/fkey.c -- decode a function keys file.
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

/* ************************************************************************* */
/*  Utilities                                                                */
/* ************************************************************************* */
/**
 *	fkeydup:
 *	"Duplicate" function key.
 *
 *	@arg	fkey		fkey to duplicate.
 *	@return				the "duplicated" function key.
 */

static casio_pixel_t **fkeydup(unsigned char *fkey)
{
	casio_pixel_t **pixels = alloc_pixels(FKEY_WIDTH, FKEY_HEIGHT);
	if (!pixels) return (NULL);
	prepare_pixels(pixels, FKEY_WIDTH, FKEY_HEIGHT)

	casio_decode_picture(pixels, fkey, casio_pictureformat_1bit_packed,
		FKEY_WIDTH, FKEY_HEIGHT);
	return (pixels);
}

/**
 *	fkeydup3:
 *	"Duplicate" function key for fx-CG.
 *
 *	@arg	fkey		fkey to duplicate.
 *	@return				the "duplicated" function key.
 */

static casio_pixel_t **fkeydup3(unsigned char *fkey)
{
	casio_pixel_t **pixels = alloc_pixels(FKEY3_WIDTH, FKEY3_HEIGHT);
	if (!pixels) return (NULL);
	prepare_pixels(pixels, FKEY3_WIDTH, FKEY3_HEIGHT)

	casio_decode_picture(pixels, fkey, casio_pictureformat_1bit_packed,
		FKEY3_WIDTH, FKEY3_HEIGHT);
	return (pixels);
}
/* ************************************************************************* */
/*  Decoding fx function keys file (G1N)                                     */
/* ************************************************************************* */
/**
 *	casio_decode_std_fkey:
 *	Decode fx function keys files.
 *
 *	@arg	h			the file handle to create.
 *	@arg	buffer		the buffer to read from.
 *	@arg	std			pointer to the standard header.
 *	@return				the error code (0 if ok).
 */

int casio_decode_std_fkey(casio_file_t **h, casio_stream_t *buffer,
	casio_standard_header_t *std)
{
	int err = casio_error_alloc;
	casio_fkey_subheader_t hd; unsigned int num;
	unsigned char *data = NULL; size_t data_size;
	casio_uint16_t *offsets; casio_uint8_t *fkeys;
	casio_file_t *handle;

	/* read the subheader */
	(void)std; DREAD(hd)
	num = be16toh(hd.casio_fkey_subheader_count) + 1;

	/* beat the best! read the rest! */
	data_size = be32toh(std->casio_standard_header_filesize)
		- sizeof(casio_standard_header_t)
		- sizeof(casio_fkey_subheader_t);
	if (!(data = malloc(data_size)))
		goto fail;
	READ(data, data_size)

	/* prepare the handle */
	err = casio_make_fkey(h, casio_filefor_fx, num);
	if (err) return (err);
	handle = *h;

	/* get the offset table */
	offsets = (casio_uint16_t*)data;
	fkeys = (casio_uint8_t*)(offsets + num);

	/* read the function keys name */
	handle->casio_file_title[16] = 0;
	strncpy(handle->casio_file_title, (char*)fkeys, 16);

	/* read all */
	for (handle->casio_file_count = 0; handle->casio_file_count < (int)num;
	  handle->casio_file_count++) {
		int i = handle->casio_file_count;
		if (offsets[i] == (uint16_t)-1)
			continue ;
		offsets[i] = be16toh(offsets[i]);

		/* store */
		handle->casio_file_fkeys[i] = fkeydup(&fkeys[i]);
		if (!handle->casio_file_fkeys[i]) goto fail;
	}

	/* no error */
	return (0);

	/* omg fail! */
fail:
	free(data);
	casio_free_file(*h); *h = NULL;
	return (err);
}
/* ************************************************************************* */
/*  Decoding fx-CG/Prizm function keys file (G3N)                            */
/* ************************************************************************* */
/**
 *	casio_decode_std_cg_fkey:
 *	Decode fx-CG key files.
 *
 *	@arg	handle		the file handle.
 *	@arg	buffer		the buffer to read from.
 *	@arg	std			the standard header.
 *	@arg	sub			the standard subheader.
 *	@arg	prizm		the prizm-specific subheader.
 *	@arg	check		the checksum to feed.
 *	@return				the error code (0 if ok).
 */

int casio_decode_std_cg_fkey(casio_file_t **h, casio_stream_t *buffer,
	casio_standard_header_t *std, casio_standard_subheader_t *sub,
	casio_standard_prizm_subheader_t *prizm, casio_uint32_t *check)
{
	int err = casio_error_alloc;
	casio_prizm_lang_subheader_t lhd; unsigned int num;
	unsigned char *data = NULL; size_t data_size;
	casio_uint32_t *offsets; casio_uint8_t *messages;
	casio_file_t *handle;
	(void)std; (void)prizm;

	/* read the subheader */
	DREAD(lhd)
	*check = casio_checksum32(&lhd,
		sizeof(casio_prizm_lang_subheader_t), *check);

	/* read the data */
	data_size = be32toh(sub->casio_standard_subheader_filesize)
		- sizeof(casio_standard_header_t)
		- sizeof(casio_standard_subheader_t)
		- sizeof(casio_standard_prizm_subheader_t)
		- sizeof(casio_prizm_lang_subheader_t) - 4;
	if (!(data = malloc(data_size))) goto fail;
	READ(data, data_size)
	*check = casio_checksum32(data, data_size, *check);

	/* make the handle */
	num = be32toh(lhd.casio_prizm_lang_subheader_count);
	err = casio_make_fkey(h, casio_filefor_cg, num);
	if (err) return (err);
	handle = *h;

	/* setup the pointers */
	offsets = (void*)data;
	messages = (uint8_t*)&offsets[num + 1];

	/* read messages */
	for (handle->casio_file_count = 0; handle->casio_file_count < (int)num;
	  handle->casio_file_count++) {
		int i = handle->casio_file_count;
		if (offsets[i] == (uint32_t)-1) {
			msg((ll_info, "[#%d] -", i));
			continue;
		}

		/* correct offset and log */
		offsets[i] = be32toh(offsets[i]);
		msg((ll_info, "[#%d] '%s' (0x%" PRIu32 ")", i,
			&messages[offsets[i]], offsets[i]));

		/* store */
		handle->casio_file_fkeys[i] = fkeydup3((void*)&messages[offsets[i]]);
		if (!handle->casio_file_fkeys[i]) goto fail;
	}

	/* done */
	free(data);
	return (0);

fail:
	if (data) free(data);
	casio_free_file(*h); *h = NULL;
	return (err);
}
