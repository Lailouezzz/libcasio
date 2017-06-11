/* ****************************************************************************
 * file/decode/casemul.c -- decode a Casemul file.
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
 * https://casetta.tuxfamily.org/formats/casemul
 *
 * The support for the subfiles (program, picture, matrix, list) are not added
 * in the specific files in `decode/mcs/` as this is *not* the format they
 * have while in the MCS, but the format CasEmul uses internally.
 * ************************************************************************* */
#include "decode.h"
#define e32toh(N) ((big_endian) ? be32toh(N) : le32toh(N))
#define htoe32(N) ((big_endian) ? htobe32(N) : htole32(N))
#define VER htoe32(0x00000100) /* 1.00 */

/* ************************************************************************* */
/*  MAKELONG                                                                 */
/* ************************************************************************* */
/* This utility comes from Microsoft Windows.
 * It makes a 32-bit integer out of two 16-bit integers. */

static casio_uint32_t makelong_le(casio_uint16_t one, casio_uint16_t two)
{
	union {
		casio_uint16_t b8[4];
		casio_uint32_t b32;
	} tmp;

	tmp.b8[0] = one >> 8;
	tmp.b8[1] = one & 0xFF;
	tmp.b8[2] = two >> 8;
	tmp.b8[3] = two & 0xFF;
	return (tmp.b32);
}

static casio_uint32_t makelong_be(casio_uint16_t one, casio_uint16_t two)
{
	union {
		casio_uint16_t b8[4];
		casio_uint32_t b32;
	} tmp;

	tmp.b8[1] = one >> 8;
	tmp.b8[0] = one & 0xFF;
	tmp.b8[3] = two >> 8;
	tmp.b8[2] = two & 0xFF;
	return (tmp.b32);
}

#ifdef  MAKELONG
# undef MAKELONG
#endif
#define MAKELONG(A, B) ((big_endian) ? makelong_be(A, B) : makelong_le(A, B))
/* ************************************************************************* */
/*  Utilities                                                                */
/* ************************************************************************* */
/**
 *	read_internal:
 *	Read the internal headers, and check them.
 *
 *	@arg	buffer		the buffer to read from.
 *	@arg	type		the expected type.
 *	@arg	version		the expected version.
 *	@return				the error (if any).
 */

static int read_internal(casio_stream_t *buffer, casio_uint32_t signature,
	casio_uint32_t version)
{
	casio_casemul_intheader_t hd;
	DREAD(hd)

	/* check type */
	if (signature != hd.casio_casemul_intheader_signature) {
#if !defined(LIBCASIO_DISABLED_LOG)
		const char *a = (char*)&signature, *b =
			(char*)&hd.casio_casemul_intheader_signature;

		msg((ll_error, "signature mismatch!"));
		msg((ll_error, "expected '%c%c%c%c', got '%c%c%c%c'",
			a[0], a[1], a[2], a[3], b[0], b[1], b[2], b[3]));
#endif
		return (casio_error_magic);
	}

	/* check version */
	if (hd.casio_casemul_intheader_version != version) {
		msg((ll_error, "version mismatch!"));
		msg((ll_error, "epxected 0x%08" CASIO_PRIX32
			", got 0x%08" CASIO_PRIX32,
			version, hd.casio_casemul_intheader_version));
		return (casio_error_magic);
	}

	/* nothing wrong :) */
	return (0);
}

/**
 *	read_top:
 *	Read the top of a record.
 *
 *	@arg	buffer		the buffer to read from.
 *	@arg	name		the name buffer (13 bytes including '\0').
 *	@arg	length		the subheader+data length.
 *	@arg	big_endian	big endian?
 *	@return				the error code.
 */

static int read_top(casio_stream_t *buffer, char *name, casio_uint32_t *length,
	int big_endian)
{
	casio_uint32_t name_length;
	unsigned char rawname[8];

	/* read and check the name length */
	DREAD(name_length)
	name_length = e32toh(name_length);
	READ(rawname, min(8, name_length))
	if (name_length > 8) {
		SKIP(name_length - 8)
		name_length = 8;
	}

	/* copy to the real name */
	memcpy(name, rawname, name_length);
	name[name_length] = 0;

	/* read the length and correct */
	DREAD(*length)
	*length = e32toh(*length);

	/* no error! */
	return (0);
}
/* ************************************************************************* */
/*  Intermediate functions                                                   */
/* ************************************************************************* */
/**
 *	read_picture:
 *	Read a picture record and content.
 *
 *	@arg	pfile		the pointer to the mcsfile to make and fill.
 *	@arg	buffer		the buffer to read from.
 *	@arg	big_endian	whether the file is big endian or not.
 *	@return				the error (if occured).
 */

static int read_picture(casio_mcsfile_t **pfile, casio_stream_t *buffer,
	int big_endian)
{
	int err; char name[13]; casio_uint32_t record_length;
	casio_casemul_pict_header_t pct;
	unsigned char *rawpx = NULL;
	unsigned int width, height;
	size_t rawpxsize;
	casio_mcshead_t head;

	*pfile = NULL;
	/* general record things */
	if ((err = read_top(buffer, name, &record_length, big_endian))
	 || (err = read_internal(buffer, MAKELONG('PI', 'CT'), VER)))
		return (err);

	/* specific things */
	DREAD(pct)
	width  = pct.casio_casemul_pict_header_width;
	height = pct.casio_casemul_pict_header_height;
	msg((ll_info, "picture dimension is %d*%dpx", width, height));

	/* read all of the pixels */
	rawpxsize = casio_picturesize_casemul(width, height);
	rawpx = malloc(rawpxsize);
	if (!rawpx) return (casio_error_alloc);
	GREAD(rawpx, rawpxsize)

	/* make the head and allocate file */
	head.casio_mcshead_type = casio_mcstype_capture;
	head.casio_mcshead_width = pct.casio_casemul_pict_header_width;
	head.casio_mcshead_height = pct.casio_casemul_pict_header_height;
	memcpy(head.casio_mcshead_name, name, strlen(name) + 1);
	head.casio_mcshead_id = name[7] - '0';
	err = casio_make_mcsfile(pfile, &head);
	if (err) goto fail;

	/* set the pixels */
	casio_decode_picture((*pfile)->casio_mcsfile_pic, rawpx,
		casio_pictureformat_casemul, width, height);

	/* finish */
	err = 0;
fail:
	if (err) casio_free_mcsfile(*pfile);
	*pfile = NULL;
	free(rawpx);
	return (0);
}

/**
 *	read_matrix:
 *	Read a matrix record and content.
 *
 *	@arg	pfile		the pointer to the mcsfile to make and fill.
 *	@arg	buffer		the buffer to read from.
 *	@arg	big_endian	whether the file is big endian encoded or not.
 *	@return				the error that occured.
 */

static int read_matrix(casio_mcsfile_t **pfile, casio_stream_t *buffer,
	int big_endian)
{
	int err; char name[13];
	casio_uint32_t record_length;
	casio_casemul_mtrx_header_t mtx;
	unsigned int total, width, height, y, x;
	double *tab = NULL, *raw;
	casio_mcshead_t head; casio_mcscell_t **cells;

	*pfile = NULL;
	/* general record things */
	if ((err = read_top(buffer, name, &record_length, big_endian))
	 || (err = read_internal(buffer, MAKELONG('MT', 'RX'), VER)))
		return (err);

	/* read specific things */
	DREAD(mtx)
	width = e32toh(mtx.casio_casemul_mtrx_header_lines) & 0x7FFF;
	height = e32toh(mtx.casio_casemul_mtrx_header_columns) & 0x7FFF;

	/* read double tab */
	total = width * height;
	tab = malloc(total * sizeof(double));
	if (!tab) return (casio_error_alloc);
	GREAD(tab, total * sizeof(double))
	raw = tab;

	/* make the head and allocate file */
	head.casio_mcshead_type = casio_mcstype_matrix;
	head.casio_mcshead_width = width;
	head.casio_mcshead_height = height;
	memcpy(head.casio_mcshead_name, name, strlen(name) + 1);
	head.casio_mcshead_id = name[4] - 'A' + 1;
	err = casio_make_mcsfile(pfile, &head);
	if (err) { free(tab); return (err); }

	/* read the matrix */
	cells = (*pfile)->casio_mcsfile_cells;
	for (y = 0; y < height; y++) for (x = 0; x < width; x++) {
		casio_bcd_t bcd;

		/* read the bcd */
		casio_bcd_fromdouble(&bcd, *raw++);

#if !defined(LIBCASIO_DISABLED_LOG)
		if (islog(ll_info)) {
			char buf[CASIO_BCD_GOODBUFSIZE];

			/* log the bcd */
			casio_bcdtoa(buf, CASIO_BCD_GOODBUFSIZE, &bcd);
			msg((ll_info, "[%d][%d] %s", y, x, buf));
		}
#endif

		/* make the cell */
		cells[y][x].casio_mcscell_real = bcd;
		cells[y][x].casio_mcscell_flags = casio_mcscellflag_used;
	}

	/* no error */
	err = 0;
fail:
	if (err) casio_free_mcsfile(*pfile);
	*pfile = NULL;
	free(tab);
	return (0);
}

/**
 *	read_list:
 *	Read a list record and content.
 *
 *	@arg	pfile		the pointer to the mcsfile to make and fill.
 *	@arg	buffer		the buffer to read from.
 *	@arg	big_endian	whether the file is big endian encoded or not.
 *	@return				the error that occured.
 */

static int read_list(casio_mcsfile_t **pfile, casio_stream_t *buffer,
	int big_endian)
{
	int err, len, x; char name[13]; casio_uint32_t record_length;
	casio_casemul_list_header_t lst;
	casio_mcshead_t head; casio_mcscell_t **cells;
	double *tab, *raw;

	*pfile = NULL;
	/* general record things */
	if ((err = read_top(buffer, name, &record_length, big_endian))
	 || (err = read_internal(buffer, MAKELONG('LI', 'ST'), VER)))
		return (err);

	/* read specific things */
	DREAD(lst)
	len = le32toh(lst.casio_casemul_list_header_lines) & 0x7FFF;
	msg((ll_info, "%d elements in list", len));

	/* read double tab */
	tab = malloc(len); if (!tab) return (casio_error_alloc);
	GREAD(tab, len * sizeof(double))
	raw = tab;

	/* make head */
	head.casio_mcshead_type = casio_mcstype_list;
	head.casio_mcshead_width = 1;
	head.casio_mcshead_height = len;
	memcpy(head.casio_mcshead_name, name, strlen(name) + 1);
	head.casio_mcshead_id = name[5] - '0';
	err = casio_make_mcsfile(pfile, &head);
	if (err) { free(tab); return (err); }

	/* read the list */
	cells = (*pfile)->casio_mcsfile_cells;
	for (x = 0; x < len; x++) {
		casio_mcscell_t *cell = &cells[x][0];

		/* read bcd */
		casio_bcd_fromdouble(&cell->casio_mcscell_real, *raw++);

#if !defined(LIBCASIO_DISABLED_LOG)
		if (islog(ll_info)) {
			char buf[CASIO_BCD_GOODBUFSIZE];

			/* log information */
			casio_bcdtoa(buf, CASIO_BCD_GOODBUFSIZE,
				&cell->casio_mcscell_real);
			msg((ll_info, "[%d] %s", x, buf));
		}
#endif

		/* set the cell */
		cell->casio_mcscell_flags = casio_mcscellflag_used;
	}

	/* no error */
	err = 0;
fail:
	if (err) casio_free_mcsfile(*pfile);
	*pfile = NULL;
	free(tab);
	return (0);
}
/* ************************************************************************* */
/*  Main function                                                            */
/* ************************************************************************* */
/**
 *	casio_decode_casemul:
 *	Decode a CasEmul file.
 *
 *	@arg	h			the handle to make.
 *	@arg	buffer		the buffer to read from.
 *	@arg	big_endian	whether the file is big endian or not.
 *	@return				the error code (0 if ok).
 */

int casio_decode_casemul(casio_file_t **h, casio_stream_t *buffer,
	int big_endian)
{
	int err, i;
	casio_casemul_header_t glb;
	casio_casemul_source_header_t src;
	casio_file_t *handle;

	/* read the overall (global) header */
	DREAD(glb)
	msg((ll_info, "Header source offset is: 0x%08" CASIO_PRIX32,
		e32toh(glb.casio_casemul_header_source_offset)));
	ifmsg(glb.casio_casemul_header_flags & casemul_compiled, (ll_info,
		"Header compiled offset is: 0x%08" CASIO_PRIX32,
		e32toh(glb.casio_casemul_header_compiled_offset)));
	elsemsg((ll_info, "The file has got no compiled part."));

	/* read the source header */
	if ((err = read_internal(buffer, MAKELONG('SR', 'CE'), VER)))
		return (err);
	DREAD(src)

	/* make the handle */
	err = casio_make_mcs(h, /* src.programs */ 0
		+ src.casio_casemul_source_header_pictures
		+ src.casio_casemul_source_header_matrixes
		+ src.casio_casemul_source_header_lists);
	if (err) return (err);
	handle = *h;

	/* read each program; TODO: put this in a function when token parsing
	 * is managed. */
	for (i = 0; i < src.casio_casemul_source_header_programs; i++) {
		casio_casemul_prog_header_t prg;
		char name[13]; casio_uint32_t record_length;

		msg((ll_info, "Reading program #%d", i + 1));
		msg((ll_warn, "Program content will be skipped!"));
		/* general record things */
		if ((err = read_top(buffer, name, &record_length, big_endian))
		 || (err = read_internal(buffer, MAKELONG('PR', 'OG'), VER)))
			goto fail;

		/* specific things */
		GDREAD(prg)
		prg.casio_casemul_prog_header_length =
			e32toh(prg.casio_casemul_prog_header_length);

		/* TODO: decode using tokens (cf. refc and libfc) */
		SKIP(prg.casio_casemul_prog_header_length)
	}

	/* read each picture */
	for (i = 0; i < src.casio_casemul_source_header_pictures; i++) {
		msg((ll_info, "Reading picture #%d", i + 1));

		err = read_picture(
			&handle->casio_file_mcsfiles[handle->casio_file_count],
			buffer, big_endian);
		if (err) goto fail;
		handle->casio_file_count++;
	}

	/* read each matrix */
	for (i = 0; i < src.casio_casemul_source_header_matrixes; i++) {
		msg((ll_info, "Reading matrix #%d", i + 1));

		err = read_matrix(
			&handle->casio_file_mcsfiles[handle->casio_file_count],
			buffer, big_endian);
		if (err) goto fail;
		handle->casio_file_count++;
	}

	/* read each list */
	for (i = 0; i < src.casio_casemul_source_header_lists; i++) {
		msg((ll_info, "Reading list #%d", i + 1));

		err = read_list(
			&handle->casio_file_mcsfiles[handle->casio_file_count],
			buffer, big_endian);
		if (err) goto fail;
		handle->casio_file_count++;
	}

	/* TODO: skip compiled part? */
	msg((ll_warn, "Should read compiled part here!"));

	/* no error! */
	return (0);
fail:
	casio_free_file(*h); *h = NULL;
	return (err);
}
