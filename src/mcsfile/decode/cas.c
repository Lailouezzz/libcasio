/* ****************************************************************************
 * mcsfile/decode/cas.c -- decode CASIOLINK (old MCS) files.
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
#include "../ref/ref.h"
#define FUNC(NAME)   &casio_decode_caspart_##NAME
#define HFUNC(NAME) &casio_decode_cashpart_##NAME

/* ************************************************************************* */
/*  Type correspondance list                                                 */
/* ************************************************************************* */
/* Part parsing function type */
typedef int decode_func ();
typedef int cas_decode_function OF((casio_mcsfile_t*, casio_stream_t*));
typedef int cas_heads_decode_function OF((casio_mcshead_t *head,
	casio_mcshead_t *heads, casio_stream_t *buffer));

/* Correspondance type */
struct cas_corresp {
	unsigned int type;
	cas_decode_function *decode;
	cas_heads_decode_function *hdecode;
};

/* All correspondances */
#define TTERM {0, NULL, NULL}
CASIO_LOCAL struct cas_corresp cas_types[] = {
	{casio_mcstype_var,     FUNC(var),     NULL},
	{casio_mcstype_program, FUNC(program), NULL},
	{casio_mcstype_matrix,  FUNC(matrix),  NULL},
	{casio_mcstype_list,    FUNC(matrix),  NULL},
	{casio_mcstype_capture, FUNC(capture), NULL},
	TTERM
};

/**
 *	lookup_cas_decode:
 *	Lookup for a parsing function for the CAS part.
 *
 *	@arg	type	the libcasio MCS file type.
 *	@return			the function (NULL if not found).
 */

CASIO_LOCAL decode_func *lookup_cas_decode(casio_mcstype_t type, int heads)
{
	/* lookup for the type */
	struct cas_corresp *c = cas_types;
	while (c->decode) {
		if (type == c->type)
			break;
		c++;
	}

	/* return the function */
	return (heads ? (decode_func*)c->hdecode : (decode_func*)c->decode);
}
/* ************************************************************************* */
/*  Head decoding functions                                                  */
/* ************************************************************************* */
/**
 *	decode_cas50:
 *	Decode a CASIOLINK Protocol header.
 *
 *	@arg	head	the head to fill.
 *	@arg	buffer	the buffer to read from.
 *	@arg	csum	the current checksum.
 *	@return			if there was an error, or not.
 */

CASIO_LOCAL int decode_cas50(casio_mcshead_t *head, casio_stream_t *buffer,
	unsigned char csum)
{
	casio_cas50_t hd;
	char *end; size_t len;

	/* read the raw header */
	DREAD(hd)
	msg((ll_info, "Raw CAS50 (CASPRO) header content (app: '%.3s'):",
		head->casio_mcshead_cas_app));
	mem((ll_info, &hd, sizeof(casio_cas50_t)));

	/* check the checksum */
	csum = casio_checksum_cas(&hd, sizeof(casio_cas50_t) - 1, csum);
	if (csum != hd.casio_cas50_checksum) {
		msg((ll_error, "Checksum mismatch: expected 0x%02X, got 0x%02X",
			hd.casio_cas50_checksum, csum));
		return (casio_error_csum);
	}

	/* copy the basic information */
	head->casio_mcshead_flags = casio_mcsfor_cas50;
	memcpy(head->casio_mcshead_datatype, hd.casio_cas50_data, 2);
	casio_correct_mcshead(head, 0);
	head->casio_mcshead_size = be16toh(hd.casio_cas50_height)
		- 2 /* checksum, colon */;
	end = memchr(hd.casio_cas50_name, 0xFF, 8);
	len = end ? (size_t)(end - (char*)hd.casio_cas50_name) : 8;
	memcpy(head->casio_mcshead_name, hd.casio_cas50_name, len);
	head->casio_mcshead_name[len] = 0;

	/* read specific data */
	switch (head->casio_mcshead_type) {
	case casio_mcstype_program:;
		/* copy password */
		head->casio_mcshead_flags |= casio_mcsflag_unfinished;
		end = memchr(hd.casio_cas50_aux, 0xFF, 8);
		len = end ? (size_t)(end - (char*)hd.casio_cas50_aux) : 8;
		memcpy(head->casio_mcshead_password, hd.casio_cas50_aux, len);
		head->casio_mcshead_password[len] = 0;
		msg((ll_info, "Is a program of %lu bytes",
			head->casio_mcshead_size));
		break;
	case casio_mcstype_variable:
	case casio_mcstype_matrix:
	case casio_mcstype_list:
		head->casio_mcshead_height = be16toh(hd.casio_cas50_height) & 0xFF;
		head->casio_mcshead_width = be16toh(hd.casio_cas50_width) & 0xFF;
		head->casio_mcshead_count = head->casio_mcshead_height;
		if (head->casio_mcshead_width && head->casio_mcshead_height)
			head->casio_mcshead_flags |= casio_mcsflag_unfinished;
		break;
	}

	/* TODO */
	return (0);
}

/**
 *	casio_decode_casfile_head:
 *	Decode the CAS file head.
 *
 *	@arg	head		the head to decode.
 *	@arg	buffer		the buffer to read the header from.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_decode_casfile_head(casio_mcshead_t *head,
	casio_stream_t *buffer)
{
	unsigned char buf[39], csum;
	casio_casdyn_t *dhd = (void*)buf;
	casio_cas40_t   *hd = (void*)buf;

	/* check that the head exists */
	if (!head) return (-1);
	memset(head, 0, sizeof(casio_mcshead_t));

	/* read beginning of the header, check if is an extended header */
	READ(buf, 4) csum = casio_checksum_cas(buf, 4, 0);
	if (!casio_check_cas_app(dhd->casio_casdyn_ext,
	  (char*)dhd->casio_casdyn_app)) {
		memcpy(head->casio_mcshead_cas_app, dhd->casio_casdyn_app, 3);
		head->casio_mcshead_cas_app[3] = 0;
		head->casio_mcshead_cas_ext = dhd->casio_casdyn_ext;

		switch (dhd->casio_casdyn_ext) {
		case casio_casdyn_9850: /* FALLTHRU */
		case casio_casdyn_end:
			return (decode_cas50(head, buffer, csum));

#if 0
		case casio_casdyn_g100: /* FALLTHRU */
		case casio_casdyn_g100b:
			return (decode_cas100(head, buffer));
#endif
		default:
			msg((ll_error, "Platform 0x%08X isn't implemented yet.",
				head->casio_mcshead_flags & casio_mcsfor_mask));
			return (casio_error_op);
		}
	}

	/* is a CAS40 head, read it. */
	READ(&buf[4], 35)
	csum = casio_checksum_cas(&buf[4], 34, csum);
	msg((ll_info, "Raw CAS40 (CAS) header:"));
	mem((ll_info, hd, sizeof(casio_cas40_t)));
	head->casio_mcshead_flags = casio_mcsfor_cas40;
	memcpy(head->casio_mcshead_datatype, hd->casio_cas40_data, 2);
	if (casio_correct_mcshead(head, 0))
		return (casio_error_magic);
	if (~csum + 1 != hd->casio_cas40_checksum)
		return (casio_error_csum);

	/* fill the handle */
	memset(head, 0, sizeof(casio_mcshead_t));
	head->casio_mcshead_flags |= casio_mcsfor_cas40;
	memcpy(head->casio_mcshead_name, hd->casio_cas40_filename, 12);
	head->casio_mcshead_name[12] = 0;

	/* type specific things */
	if (head->casio_mcshead_type == casio_mcstype_program) {
		casio_cas_spe_program_t *spe = (void*)hd->casio_cas40_misc;
		head->casio_mcshead_size = be16toh(spe->casio_cas_spe_program_length);
		/* TODO: store flags? */
	}

	/* no error! */
	return (0);
}
/* ************************************************************************* */
/*  Part decoding functions                                                  */
/* ************************************************************************* */
/**
 *	casio_decode_casfile_part:
 *	Decode a CASIOLINK Protocol content part.
 *
 *	@arg	file	the file to contribute to.
 *	@arg	buffer	the buffer to read from.
 *	@return			if there was an error, or not.
 */

int CASIO_EXPORT casio_decode_casfile_part(casio_mcsfile_t *file,
	casio_stream_t *buffer)
{
	cas_decode_function *decode;

	/* checks */
	if (!file) return (casio_error_op);

	/* look for the decoding function */
	decode = (cas_decode_function*)lookup_cas_decode(
		file->casio_mcsfile_head.casio_mcshead_type, 0);
	if (!decode) {
		msg((ll_error,
			"No dedicated decoding function was found for this type!"));
		return (casio_error_unknown);
	}

	/* decode the part */
	return ((*decode)(file, buffer));
}
