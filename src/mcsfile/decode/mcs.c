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

/* ************************************************************************* */
/*  Type correspondance list                                                 */
/* ************************************************************************* */
/* MCS file parsing function type */
typedef int mcs_decode_func_t OF((casio_mcsfile_t**, casio_stream_t*,
	casio_mcshead_t*));

/* Correspondance type */
struct mcs_corresp {
	unsigned int       type;
	mcs_decode_func_t *decode;
};

/* All correspondances */
#define TTERM {0, NULL}
static struct mcs_corresp mcs_types[] = {
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

static mcs_decode_func_t *lookup_mcsfile_decode(casio_mcstype_t type)
{
	struct mcs_corresp *c;

	/* lookup for the type */
	for (c = mcs_types; c->decode && type != c->type; c++);

	/* return the function */
	return (c->decode);
}
/* ************************************************************************* */
/*  Head decoding function                                                   */
/* ************************************************************************* */
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

int casio_decode_mcsfile_head(casio_mcshead_t *head,
	int raw_type, const unsigned char *groupname,
	const unsigned char *dirname, const unsigned char *filename,
	uint_fast32_t filesize)
{
	/* check that we have a head, lol */
	if (!head) return (-1);

	/* look for the raw type */
	casio_maketype_mcs(head, (char*)groupname, (char*)dirname,
		(char*)filename, raw_type);
	head->casio_mcshead_size = filesize;
	msg((ll_info, "libcasio file type is 0x%08lX", head->casio_mcshead_type));

	ifmsg(casio_mcshead_uses_id(head), (ll_info,
		"libcasio file id is (%d, %d)",
		casio_get_id_major(head->casio_mcshead_id),
		casio_get_id_minor(head->casio_mcshead_id)));

	/* everything went well! */
	return (0);
}
/* ************************************************************************* */
/*  File decoding functions                                                  */
/* ************************************************************************* */
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

int casio_decode_mcsfile(casio_mcsfile_t **handle, const casio_mcshead_t *head,
	casio_stream_t *buffer)
{
	int err = 0;
	mcs_decode_func_t *decode;
	casio_mcshead_t h;

	/* check that the head is there */
	if (!head) return (casio_error_op);
	h = *head;

	/* look for the decoding function */
	decode = lookup_mcsfile_decode(head->casio_mcshead_type);
	if (!decode) {
		msg((ll_error,
			"No dedicated decoding function for this type was found!"));
		goto notparsing;
	}

	/* decode */
	if (!head->casio_mcshead_size) err = (*decode)(handle, buffer, &h);
	else {
		casio_stream_t *lbuf;
		int alterr;

		err = casio_open_limited(&lbuf, buffer, head->casio_mcshead_size);
		if (err) goto fail;
		err = (*decode)(handle, lbuf, &h);
		alterr = casio_empty_limited(lbuf);
		casio_close(lbuf);
		if (!err && alterr) err = alterr;
		if (err) goto fail;
	}

	/* oh yeah, and go away. */
	if (err) goto fail;
	return (0);

notparsing:
	/* allocate enough space */
	if ((err = casio_make_mcsfile(handle, &h)))
		return (err);

	/* read the content */
	GREAD((*handle)->casio_mcsfile_content, h.casio_mcshead_size)

	/* log */
	msg((ll_info, "File content:"));
	mem((ll_info, (*handle)->casio_mcsfile_content, h.casio_mcshead_size));

	/* saved normally */
	return (0);

fail:
	casio_free_mcsfile(*handle);
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

int casio_decode_mcsfile_data(casio_mcsfile_t **handle,
	const casio_mcshead_t *head, const void *data, size_t size)
{
	int err; casio_stream_t *membuf;

	err = casio_open_memory(&membuf, data, size);
	if (err) return (err);
	err = casio_decode_mcsfile(handle, head, membuf);
	casio_close(membuf);
	return (err);
}
