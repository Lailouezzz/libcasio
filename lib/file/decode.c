/* ****************************************************************************
 * file/decode.c -- decode a file.
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
#include "file.h"

/* ---
 * Check using extensions.
 * --- */

/* Decode function. */

typedef int decode_func OF((casio_file_t**, casio_stream_t*));

/* Correspondance type. */

struct corresp {
	/* extension */
	const char *ext;

	/* expected types */
	casio_filetype_t type;

	/* function */
	decode_func *decode;
};

/* Correspondances */
CASIO_LOCAL struct corresp correspondances[] = {
	{"g1s", casio_filetype_storage, casio_decode_storage},

	/* terminating entry */
	{NULL,  0,                NULL}
};

/**
 *	lookup_extension:
 *	Get the function using the type.
 *
 *	@arg	path	the path.
 *	@arg	types	the expected types.
 *	@arg	decode	the decode function to set.
 *	@return			the error code (0 if ok).
 */

CASIO_LOCAL int lookup_extension(const char *path, casio_filetype_t types,
	decode_func **func)
{
	char ext[5];
	struct corresp *c;

	/* match the extension */
	if (!casio_getext(path, ext, 5)) return (casio_error_magic);
	for (c = correspondances - 1; c->ext; c++);

	/* check if correspondance is valid and expected */
	if (!c->ext) return (casio_error_magic);
	if (types && !(types & c->type)) return (casio_error_wrong);

	/* return the decode function */
	*func = c->decode;
	return (0);
}

/* ---
 * Main decoding function.
 * --- */

/**
 *	casio_decode:
 *	Decode a file.
 *
 *	Read the standard header, correct endianness, check magic numbers,
 *	then read subparts according to the file type.
 *
 *	@arg	handle			the handle.
 *	@arg	path			the file path.
 *	@arg	buffer			the buffer to read from.
 *	@arg	expected_types	the expected types.
 *	@return					the error code (0 if ok).
 */

int CASIO_EXPORT casio_decode(casio_file_t **handle, const char *path,
	casio_stream_t *buffer, casio_filetype_t expected_types)
{
	int err; decode_func *decode;
	unsigned char buf[0x20], altbuf[0x20];
	int casemul_be;

	/* Match using extension. */

	err = lookup_extension(path, expected_types, &decode);
	if (err == casio_error_wrong) return (err);
	else if (!err) return ((*decode)(handle, buffer));

	/* Identify a CAS file. */

	READ(buf, 1)
	if (buf[0] == ':') {
		if (expected_types && !(expected_types & casio_filetype_mcs))
			return (casio_error_wrong);
		return (casio_decode_cas(handle, buffer));
	}

	/* Identify a GraphCard file. */

	READ(&buf[1], 2)
	if (!buf[0] && buf[1] == 0x32 && buf[2] == ':') {
		if (expected_types && !(expected_types & casio_filetype_mcs))
			return (casio_error_wrong);
		return (casio_decode_grc(handle, buffer));
	}

	/* Identify a Casemul file. */

	READ(&buf[3], 9) casemul_be = !memcmp(buf, "CAFS", 4);
	if (casemul_be || !memcmp(buf, "ACFS", 4)) {
		casio_casemul_intheader_t *ch = (void*)buf;
		casio_uint32_t ver, siz;

		ver = casemul_be ? be32toh(ch->casio_casemul_intheader_version)
			: le32toh(ch->casio_casemul_intheader_version);
		siz = casemul_be ? le32toh(ch->casio_casemul_intheader_size)
			: le32toh(ch->casio_casemul_intheader_size);
		if (ver != 0x100 || siz != sizeof(casio_casemul_header_t))
			return (casio_error_magic);
		if (expected_types && !(expected_types & casio_filetype_mcs))
			return (casio_error_wrong);
		return (casio_decode_casemul(handle, buffer, casemul_be));
	}

	/* Identify a standard header (send a _copy_). */

	READ(&buf[12], 0x14) memcpy(altbuf, buf, 0x20);
	return (casio_decode_std(handle, path, buffer,
		(casio_standard_header_t*)altbuf, expected_types));
}
