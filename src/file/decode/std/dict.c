/* ****************************************************************************
 * file/decode/std/dict.c -- dictionary decoding.
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
#include "dict.h"

/**
 *	casio_decode_std_dict:
 *	Decode a dictionary.
 *
 *	@arg	buffer		the stream from which to read from.
 *	@arg	callback	the callback.
 *	@arg	cookie		the callback cookie.
 *	@arg	num			the number of entries.
 *	@arg	size		the total size (index + content + alignment).
 *	@arg	is32		32-bit offsets instead of 16-bit ones.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_decode_std_dict(casio_stream_t *buffer,
	casio_decode_dict_entry_t *callback, void *cookie,
	unsigned int num, unsigned long size, int is32)
{
	int err, has_last_index = 0;
	unsigned int i, last_index;
	casio_uint16_t *index16 = NULL;
	casio_uint32_t *index32 = NULL;
	unsigned char *content = NULL;
	unsigned long last_offset = -1;
	unsigned long current_offset = 0;

	/* Read the index. */
	msg((ll_info, "Allocating and reading the index."));
	if (is32) {
		index32 = casio_alloc(num + 1, sizeof(casio_uint32_t));
		if (!index32) return (casio_error_alloc);
		GREAD(index32, (num + 1) * sizeof(casio_uint32_t))
		size -= sizeof(casio_uint32_t) * (num + 1);
	} else {
		index16 = casio_alloc(num, sizeof(casio_uint16_t));
		if (!index16) return (casio_error_alloc);
		GREAD(index16, num * sizeof(casio_uint16_t))
		size -= sizeof(casio_uint16_t) * num;
	}

	/* Read the entries. */
	msg((ll_info, "Reading %u entries.", num));
	for (i = 0; i < num; i++) {
		/* Get the offset, check if it is valid. */
		if ((!is32 && (current_offset = be16toh(index16[i])) == 0xFFFF)
		 || ( is32 && (current_offset = be32toh(index32[i])) == 0xFFFFFFFF)) {
			msg((ll_info, "Entry %u is invalid", i));
			continue;
		}

		/* If there is a last content, manage it. */
		if (has_last_index) {
			unsigned long content_size = current_offset - last_offset;

			/* Allocate and read the content. */
			msg((ll_info, "Entry %u is valid!", i));
			content = casio_alloc(content_size, 1);
			if (!content) { err = casio_error_alloc; goto fail; }
			GREAD(content, content_size)

			/* Send it to the user. */
			err = (*callback)(cookie, last_index, content, content_size);
			if (err) goto fail;

			/* Free the content. */
			free(content);
		}

		/* Update the last offset. */
		has_last_index = 1;
		last_index = i;
		last_offset = current_offset;
	}

	/* Read the last entry. */
	if (has_last_index) {
		unsigned long content_size = size - current_offset;

		/* Allocate and read the content. */
		content = casio_alloc(content_size, 1);
		if (!content) { err = casio_error_alloc; goto fail; }
		GREAD(content, content_size)

		/* Send it to the user. */
		err = (*callback)(cookie, last_index, content, content_size);
		if (err) goto fail;

		/* Free the content. */
		free(content);
	}

	free(index16);
	return (0);
fail:
	free(index16);
	free(content);
	return (err);
}
