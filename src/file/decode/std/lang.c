/* ****************************************************************************
 * file/decode/std/lang.c -- decode a language file.
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
/*  fx language files parsing                                                */
/* ************************************************************************* */
/**
 *	casio_decode_std_lang:
 *	Decode fx language files.
 *
 *	@arg	h			the file handle to create.
 *	@arg	buffer		the buffer to read from.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_decode_std_lang(casio_file_t **h,
	casio_stream_t *buffer, casio_standard_header_t *std)
{
	int err = casio_error_alloc;
	casio_lang_subheader_t hd; unsigned int num;
	char *data; size_t data_size;
	casio_uint16_t *offsets; char *messages;
	casio_file_t *handle;

	(void)std;
	/* read the subheader */
	DREAD(hd) num = be16toh(hd.casio_lang_subheader_count) + 1;

	/* log */
	msg((ll_info, "%u messages to read", num));

	/* beat the best, read the rest! */
	data_size = std->casio_standard_header_filesize
		- sizeof(casio_standard_header_t)
		- sizeof(casio_lang_subheader_t);
	if (!(data = casio_alloc(data_size, 1))) goto fail;
	READ(data, data_size)

	/* make the handle */
	err = casio_make_lang(h, casio_filefor_fx, num);
	if (err) return (err);
	handle = *h;

	/* get the offset table */
	offsets = (casio_uint16_t*)data;
	messages = (char*)(offsets + num);

	/* read messages */
	for (handle->casio_file_count = 0; handle->casio_file_count < (int)num;
	  handle->casio_file_count++) {
		int i = handle->casio_file_count;
		if (offsets[i] == (casio_uint16_t)-1) {
			msg((ll_info, "[#%d] -", i));
			continue;
		}

		/* correct offset and log */
		offsets[i] = be16toh(offsets[i]);
		msg((ll_info, "[#%d] '%s' (0x%" CASIO_PRIu16 ")", i,
			&messages[offsets[i]], offsets[i]));

		/* store */
		handle->casio_file_messages[i] = strdup(&messages[offsets[i]]);
		if (!handle->casio_file_messages[i]) goto fail;
		handle->casio_file_count++;
	}

	/* no error */
	casio_free(data);
	return (0);

	/* omg fail! */
fail:
	casio_free(data);
	casio_free_file(*h); *h = NULL;
	return (err);
}

/* ************************************************************************* */
/*  Decoding language files for fx-CG/Prizm (G3L)                            */
/* ************************************************************************* */
/**
 *	casio_decode_std_cg_lang:
 *	Decode fx-CG language file.
 *
 *	@arg	h			the handle to make.
 *	@arg	buffer		the buffer to read from.
 *	@arg	std			the standard header.
 *	@arg	sub			the standard subheader.
 *	@arg	prizm		the prizm-specific subheader.
 *	@arg	check		the checksum to feed.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_decode_std_cg_lang(casio_file_t **h,
	casio_stream_t *buffer, casio_standard_header_t *std,
	casio_standard_subheader_t *sub, casio_standard_prizm_subheader_t *prizm,
	casio_uint32_t *check)
{
	int err = casio_error_alloc;
	casio_prizm_lang_subheader_t lhd; unsigned int num;
	casio_uint8_t *data; size_t data_size;
	casio_uint32_t *offsets; char *messages;
	casio_file_t *handle;
	(void)std; (void)prizm;

	/* read the subheader */
	DREAD(lhd)
	*check = casio_checksum32(&lhd, sizeof(casio_prizm_lang_subheader_t),
		*check);

	/* read the data */
	data_size =
		be32toh(sub->casio_standard_subheader_filesize)
		- sizeof(casio_standard_header_t)
		- sizeof(casio_standard_subheader_t)
		- sizeof(casio_standard_prizm_subheader_t)
		- sizeof(casio_lang_subheader_t) - 4;
	if (!(data = casio_alloc(data_size, 1))) goto fail;
	READ(data, data_size)
	*check = casio_checksum32(data, data_size, *check);

	/* make the handle */
	num = be32toh(lhd.casio_prizm_lang_subheader_count);
	err = casio_make_lang(h, casio_filefor_cg, num);
	if (err) return (err);
	handle = *h;

	/* setup the pointers */
	offsets = (void*)data;
	messages = (char*)&offsets[num + 1];

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
		handle->casio_file_messages[i] = strdup((char*)&messages[offsets[i]]);
		if (!handle->casio_file_messages[i]) goto fail;
	}

	/* done */
	casio_free(data);
	return (0);

fail:
	casio_free(data);
	casio_free_file(*h); *h = NULL;
	return (err);
}
