/* ****************************************************************************
 * mcsfile/decode/mcs.c -- decode an MCS file.
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
#include "decode.h"
#define FUNC(NAME) casio_decode_mcs_##NAME

/* ---
 * Type correspondance list.
 * --- */

/* MCS file parsing function type. */

typedef int mcs_decode_func_t OF((casio_mcsfile_t**, casio_stream_t*,
	casio_mcshead_t*));

/* Correspondance type */

struct mcs_corresp {
	casio_mcstype_t    type;
	mcs_decode_func_t *decode;
};

/* All correspondances */

#define TTERM {0, NULL}
CASIO_LOCAL struct mcs_corresp mcs_types[] = {
	{casio_mcstype_program,     FUNC(program)},
	{casio_mcstype_list,        FUNC(cells)},
	{casio_mcstype_mat,         FUNC(cells)},
	{casio_mcstype_vct,         FUNC(cells)},
	{casio_mcstype_spreadsheet, FUNC(spreadsheet)},
	{casio_mcstype_pict,        FUNC(picture)},
	{casio_mcstype_capt,        FUNC(capture)},
	{casio_mcstype_string,      FUNC(string)},
	{casio_mcstype_setup,       FUNC(setup)},
	{casio_mcstype_alphamem,    FUNC(var)},
	{casio_mcstype_variable,    FUNC(var)},
	TTERM
};

/**
 *	lookup_mcsfile_decode:
 *	Lookup for a parsing function for an MCS file type.
 *
 *	@arg	type		the MCS file type.
 *	@return				the function (NULL if not found).
 */

CASIO_LOCAL mcs_decode_func_t *lookup_mcsfile_decode(casio_mcstype_t type)
{
	struct mcs_corresp *c;

	/* lookup for the type */
	for (c = mcs_types; c->decode && type != c->type; c++);

	/* return the function */
	return (c->decode);
}

/* ---
 * Head decoding function.
 * --- */

/**
 *	casio_decode_mcsfile_head:
 *	Decode MCS file head.
 *
 *	@arg	head		the head to fill.
 *	@arg	raw_type	the raw file type.
 *	@arg	groupname	the groupname (up to 16 bytes).
 *	@arg	dirname		the directory name (up to 8 bytes).
 *	@arg	filename	the filename (up to 8 bytes).
 *	@arg	filesize	the data length.
 *	@return				0 if the head was filled with success, -1 otherwise.
 */

int CASIO_EXPORT casio_decode_mcsfile_head(casio_mcshead_t *head,
	int raw_type, const unsigned char *groupname,
	const unsigned char *dirname, const unsigned char *filename,
	unsigned long filesize)
{
	/* check that we have a head, lol */
	if (!head) return (-1);

	/* prepare the head */
	head->casio_mcshead_flags = casio_mcsfor_mcs;
	head->casio_mcshead_size = filesize;
	head->casio_mcshead_rawtype = raw_type;
	strncpy(head->casio_mcshead_name, (char*)filename, 12);
	head->casio_mcshead_name[12] = 0;
	strncpy(head->casio_mcshead_group, (char*)groupname, 16);
	head->casio_mcshead_group[16] = 0;
	strncpy(head->casio_mcshead_dirname, (char*)dirname, 8);
	head->casio_mcshead_dirname[8] = 0;

	/* make the lib abstract types out of this raw information. */
	casio_correct_mcshead(head, 0);
	msg((ll_info, "libcasio file type is 0x%08lX", head->casio_mcshead_type));

	ifmsg(casio_mcshead_uses_id(head), (ll_info,
		"libcasio file id is (%d, %d)",
		casio_get_id_major(head->casio_mcshead_id),
		casio_get_id_minor(head->casio_mcshead_id)));

	/* everything went well! */
	return (0);
}

/* ---
 * File decoding functions.
 * --- */

/**
 *	casio_decode_mcsfile:
 *	Decode MCS file content.
 *
 *	Part of the public API because of the Protocol 7, where file is sent
 *	without its header (only with specific subheaders).
 *
 *	@arg	handle		the handle to make.
 *	@arg	head		the head to use.
 *	@arg	buffer		the buffer to read from.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_decode_mcsfile(casio_mcsfile_t **handle,
	const casio_mcshead_t *mcshead, casio_stream_t *buffer)
{
	int err = 0;
	mcs_decode_func_t *decode;
	casio_mcshead_t head;

	/* Check that the head is there, correct it. */

	if (!mcshead)
		return (casio_error_op);
	memcpy(&head, mcshead, sizeof(casio_mcshead_t));
	casio_correct_mcshead(&head, 0);

	/* Look for the decoding function. */

	decode = lookup_mcsfile_decode(head.casio_mcshead_type);
	if (!decode) {
		msg((ll_error,
			"No dedicated decoding function for type 0x%08lX was found!",
			head.casio_mcshead_type));
		goto notparsing;
	}

	/* Decode. */

	if (!head.casio_mcshead_size)
		err = (*decode)(handle, buffer, &head);
	else {
		casio_stream_t *lbuf;
		int alterr;

		/* Open the limited buffer. */

		msg((ll_info, "Making a limited buffer of "
			"0x%" CASIO_PRIXSIZE " bytes", head.casio_mcshead_size));
		err = casio_open_limited(&lbuf, buffer, head.casio_mcshead_size);
		if (err) goto fail;

		/* Call the decode error. */

		msg((ll_info, "Decoding the file with the specific function."));
		err = (*decode)(handle, lbuf, &head);
		alterr = casio_empty_limited(lbuf);
		casio_close(lbuf);

		/* Check the error. */

		if (!err && alterr)
			err = alterr;
	}

	/* Oh yeah, and go away. */

	if (err) goto fail;
	return (0);

notparsing:
	/* allocate enough space */
	if ((err = casio_make_mcsfile(handle, &head)))
		return (err);

	/* read the content */
	GREAD((*handle)->casio_mcsfile_content, head.casio_mcshead_size)

	/* log */
	msg((ll_info, "File content:"));
	mem((ll_info, (*handle)->casio_mcsfile_content, head.casio_mcshead_size));

	/* saved normally */
	return (0);

fail:
	casio_free_mcsfile(*handle); *handle = NULL;
	return (err);
}

/**
 *	casio_decode_mcsfile_data:
 *	Decode a MCS file content from raw memory.
 *
 *	@arg	handle		the file handle.
 *	@arg	head		the file head.
 *	@arg	data		the buffer data.
 *	@arg	size		the buffer size.
 *	@return				the error.
 */

int CASIO_EXPORT casio_decode_mcsfile_data(casio_mcsfile_t **handle,
	const casio_mcshead_t *head, const void *data, size_t size)
{
	int err; casio_stream_t *membuf;

	err = casio_open_memory(&membuf, data, size);
	if (err) return (err);
	err = casio_decode_mcsfile(handle, head, membuf);
	casio_close(membuf);
	return (err);
}
