/* ****************************************************************************
 * file/decode/std.c -- decode a "standard" CASIO file.
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
#include "../corresp/standard.h"
#define FUNC(NAME) casio_decode_std_##NAME

/* ************************************************************************* */
/*  Getting the decoding function                                            */
/* ************************************************************************* */
/* Correspondance type */
typedef int decode_func ();
struct decode_corresp {
	unsigned int platform;
	unsigned int type;

	/* result */
	decode_func *decode;
};

/* The correspondances */
CASIO_LOCAL struct decode_corresp decode_functions[] = {
	/* add-ins */
	{casio_filefor_fx, casio_filetype_addin,     FUNC(addin)},
	{casio_filefor_cp, casio_filetype_addin,     FUNC(cp_addin)},
	{casio_filefor_cg, casio_filetype_addin,     FUNC(cg_addin)},
	/* mcs */
	{casio_filefor_fx, casio_filetype_mcs,       FUNC(mcs)},
	{casio_filefor_cg, casio_filetype_mcs,       FUNC(mcs)},
	/* language files */
	{casio_filefor_fx, casio_filetype_lang,      FUNC(lang)},
	{casio_filefor_cg, casio_filetype_lang,      FUNC(cg_lang)},
	/* function keys file */
	{casio_filefor_fx, casio_filetype_fkey,      FUNC(fkey)},
	{casio_filefor_cg, casio_filetype_fkey,      FUNC(cg_fkey)},
	/* e-activities */
/*	{casio_filefor_fx, casio_filetype_eact,      FUNC(eact)}, */
	/* pictures */
	{casio_filefor_cg, casio_filetype_pict,      FUNC(g3p)},
	{casio_filefor_cp, casio_filetype_pict,      FUNC(c2p)},

	{0, 0, NULL}
};

/**
 *	find_decode_function:
 *	Find the parsing function.
 *
 *	@arg	platform	the platform.
 *	@arg	type		the type.
 *	@arg	rd			pointer to the decode function.
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL int find_decode_function(casio_filefor_t platform,
	casio_filetype_t type, decode_func **rd)
{
	struct decode_corresp *c;

	/* get the function */
	for (c = decode_functions; c->decode; c++) {
		if (c->type != type)
			continue;
		if (c->platform != platform)
			continue;

		break;
	}
	if (!c->decode) {
		msg((ll_fatal, "No parsing function was found for this type."));
		return (casio_error_magic);
	}

	/* set the vars */
	*rd = c->decode;
	return (0);
}
/* ************************************************************************* */
/*  Main standard header decoding function                                   */
/* ************************************************************************* */
/**
 *	casio_decode_std:
 *	Decode a file with standard header.
 *
 *	@arg	h				the handle to create.
 *	@arg	path			the path.
 *	@arg	buffer			the buffer to read from.
 *	@arg	std				the standard header.
 *	@arg	expected_types	the expected types.
 *	@return					the error code (0 if ok).
 */

int CASIO_EXPORT casio_decode_std(casio_file_t **h, const char *path,
	casio_stream_t *buffer, casio_standard_header_t *std,
	casio_filetype_t expected_types)
{
	int err;
	casio_filetype_t type; casio_filefor_t platform;
	decode_func *rd; unsigned int mflags;
	casio_uint32_t check = casio_checksum32(std,
		sizeof(casio_standard_header_t), 0);

	/* reverse the standard header */ {
		unsigned char *u = (unsigned char*)std;
		size_t i;

		for (i = 0; i < sizeof(casio_standard_header_t); i++)
			u[i] = ~u[i];
	}

	/* print header */
	msg((ll_info, "Raw inverted standard header is:"));
	mem((ll_info, std, sizeof(casio_standard_header_t)));
	std->casio_standard_header_filesize =
		be32toh(std->casio_standard_header_filesize);
	std->casio_standard_header_number =
		be16toh(std->casio_standard_header_number);

	/* get the type */
	if (casio_maketype_std(path, std, &mflags, &platform, &type))
		return (casio_error_magic);

	/* check control bytes */
	if (mflags & casio_stdflag_check1
	 && std->casio_standard_header_control
	  != ((std->casio_standard_header_filesize + 0x41) & 0xff)) {
		msg((ll_info, "First control byte isn't right."));
		return (casio_error_magic);
	} else if (mflags & casio_stdflag_check2
	 && std->casio_standard_header_control2
	  != ((std->casio_standard_header_filesize + 0xb8) & 0xff)) {
		msg((ll_info, "Second control byte isn't right."));
		return (casio_error_magic);
	}

	/* check if there is a standard subheader */
	if (mflags & casio_stdflag_sub) {
		casio_standard_subheader_t hd;

		msg((ll_info, "Has a Standard Subheader!"));
		DREAD(hd)
		if (casio_maketype_sub(&hd, &mflags, &type, &platform))
			return (casio_error_magic);

		/* TODO: controls */
		/* find the decode function */
		if (find_decode_function(platform, type, &rd))
			return (casio_error_magic);
		if (expected_types && !(type & expected_types))
			return (casio_error_wrong);

		/* read and decode for specific platforms */
		check = casio_checksum32(&hd.casio_standard_subheader_filetype,
			sizeof(casio_standard_header_t) - 4, check);
		if (platform == casio_filefor_cp) {
			casio_standard_classpad_subheader_t shd;

			DREAD(shd)
			check = casio_checksum32(&shd,
				sizeof(casio_standard_classpad_subheader_t), check);

			/* decode the file content */
			err = (*rd)(h, buffer, std, &hd, &shd, &check);
			if (err) return (err);
		} else if (platform == casio_filefor_cg) {
			casio_standard_prizm_subheader_t shd;
			casio_uint32_t endcheck;

			DREAD(shd)
			check = casio_checksum32(&shd,
				sizeof(casio_standard_prizm_subheader_t), check);

			/* decode the file content */
			err = (*rd)(h, buffer, std, &hd, &shd, &check);
			if (err) return (err);

			/* read the footer */
			GDREAD(endcheck)
			err = casio_error_csum;
			if (be32toh(endcheck)
			 != be32toh(hd.casio_standard_subheader_checksum))
				goto fail;
		}

		/* check the sum */
		err = casio_error_csum;
		if (check != be32toh(hd.casio_standard_subheader_checksum))
			goto fail;

		/* no error */
		return (0);
	}

	/* check if there is a standard picture header */
	if (mflags & casio_stdflag_pic) {
		casio_standard_picheader_t hd;

		msg((ll_info, "Has a Standard Picture Header!"));
		DREAD(hd)

		/* correct the type; TODO: hardcode less */
		type = casio_filetype_picture;
		if (!memcmp(hd.casio_standard_picheader_cp, "CC", 2))
			platform = casio_filefor_cp;
		else if (!memcmp(hd.casio_standard_picheader_cp, "CP", 2))
			platform = casio_filefor_cg;
		else {
			msg((ll_error, "Unknown magic sequence: '%.2s'",
				hd.casio_standard_picheader_cp));
			return (casio_error_magic);
		}

		/* find the decode function */
		if (find_decode_function(platform, type, &rd))
			return (casio_error_magic);

		/* check the obfuscation bytes; imitates syscall 0x0C12
		 * http://bible.planet-casio.com/simlo/chm/v20/fxCG20_Bfile.htm */
		if (std->casio_standard_header_obfuscated0 ==
		  std->casio_standard_header_subtype[0]
		+ ((std->casio_standard_header_filesize & 0xFF00) >> 8)
		+  (std->casio_standard_header_filesize & 0xFF)) {
			std->casio_standard_header_obfuscated0 = 0;
			std->casio_standard_header_obfuscated1 = 0;
		} else {
			std->casio_standard_header_obfuscated0 = 1;
			std->casio_standard_header_obfuscated1 = 1;
		}

		/* call it */
		err = (*rd)(h, buffer, std, &hd);
		if (err) return (err);

		/* no error! */
		return (0);
	}

	/* find the decode function */
	if (find_decode_function(platform, type, &rd))
		return (casio_error_magic);
	if (expected_types && !(type & expected_types))
		return (casio_error_wrong);

	/* log some data */
	msg((ll_info, "Standard Header filesize is %" CASIO_PRIu32 "o.",
		std->casio_standard_header_filesize));
	msg((ll_info, "Standard Header num is %" CASIO_PRIu16 ".",
		std->casio_standard_header_number));

	/* subdecode. */
	return ((*rd)(h, buffer, std));
fail:
	casio_free_file(*h); *h = NULL;
	return (err);
}
