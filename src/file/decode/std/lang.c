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
#include "dict.h"

/**
 *	decode_lang_entry:
 *	Decode a language file entry.
 *
 *	@arg	file		the file.
 *	@arg	id			the entry ID.
 *	@arg	mem			the memory.
 *	@arg	size		the memory size.
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL int decode_lang_entry(casio_file_t *handle,
	unsigned int id, void *mem, size_t size)
{
	size_t len; void *chr;
	char *str;

	/* Find the length. */
	chr = memchr(mem, '\0', size);
	len = chr ? (size_t)((char*)chr - (char*)mem) : size;

	/* Allocate. */
	handle->casio_file_messages[id] = casio_alloc(len + 1, 1);
	str = handle->casio_file_messages[id];
	if (!str) return (casio_error_alloc);

	/* Copy the data. */
	memcpy(str, mem, len);
	str[len] = 0;

	/* Log it and return. */
	msg((ll_info, "[#%d] '%s'", id, str));
	return (0);
}
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
	int err; casio_file_t *handle;
	casio_lang_subheader_t hd;
	unsigned int num;
	unsigned long dict_size;

	(void)std; *h = NULL;
	/* read the subheader */
	GDREAD(hd)
	num = be16toh(hd.casio_lang_subheader_count) + 1;

	/* beat the best, read the rest! */
	dict_size = std->casio_standard_header_filesize
		- sizeof(casio_standard_header_t)
		- sizeof(casio_lang_subheader_t);

	/* make the handle */
	err = casio_make_lang(h, casio_filefor_fx, num);
	if (err) return (err);
	handle = *h;

	/* read using the dictionary decoding function. */
	err = casio_decode_std_dict(buffer,
		(casio_decode_dict_entry_t*)&decode_lang_entry, handle,
		num, dict_size, 0);
	if (err) goto fail;

	/* no error */
	return (0);

	/* omg fail! */
fail:
	casio_free_file(*h); *h = NULL;
	return (err);
}

/**
 *	casio_decode_std_cg_lang:
 *	Decode fx-CG language file.
 *
 *	@arg	h			the handle to make.
 *	@arg	buffer		the buffer to read from.
 *	@arg	std			the standard header.
 *	@arg	sub			the standard subheader.
 *	@arg	prizm		the prizm-specific subheader.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_decode_std_cg_lang(casio_file_t **h,
	casio_stream_t *buffer, casio_standard_header_t *std,
	casio_standard_subheader_t *sub, casio_standard_prizm_subheader_t *prizm)
{
	int err; casio_file_t *handle;
	casio_prizm_lang_subheader_t lhd;
	unsigned int num;
	unsigned long dict_size;

	(void)std; (void)prizm;
	/* read the subheader */
	DREAD(lhd)
	num = be32toh(lhd.casio_prizm_lang_subheader_count);
	dict_size = be32toh(sub->casio_standard_subheader_filesize)
		- sizeof(casio_standard_header_t)
		- sizeof(casio_standard_subheader_t)
		- sizeof(casio_standard_prizm_subheader_t)
		- sizeof(casio_lang_subheader_t)
		- sizeof(casio_uint32_t);

	/* make the handle */
	err = casio_make_lang(h, casio_filefor_cg, num);
	if (err) return (err);
	handle = *h;

	/* read using the dictionary decoding function. */
	err = casio_decode_std_dict(buffer,
		(casio_decode_dict_entry_t*)&decode_lang_entry, handle,
		num, dict_size, 1);
	if (err) goto fail;

	/* done */
	return (0);

fail:
	casio_free_file(*h); *h = NULL;
	return (err);
}
