/* ****************************************************************************
 * libcasio/format/casemul.h -- the Casemul file format description.
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
#ifndef  LIBCASIO_FORMAT_CASEMUL_H
# define LIBCASIO_FORMAT_CASEMUL_H
# include "../cdefs.h"
# include "../number.h"
# pragma pack(1)

CASIO_BEGIN_NAMESPACE

/* Casemul files are made of an overall header, a source part and an
 * optional compiled part. One thing to know is that every specific header has
 * an internal header, which always has this structure: */

typedef struct casio_casemul_intheader_s {
	/* This signature, a set of 4 characters identifying the header type. */

	casio_uint32_t casio_casemul_intheader_signature;

	/* Version (more or less 0xMMmm, where MM is the major and
	 * mm the minor). */

	casio_uint32_t casio_casemul_intheader_version;

	/* Header size. */

	casio_uint32_t casio_casemul_intheader_size;
} casio_casemul_intheader_t;

/* The signature is made using `MAKELONG`, a Microsoft Windows specific macro
 * which concatenates two words (16-bit integers) into a single long
 * (32-bit integer). One usage in CasEmul is the following:
 *
 *	MAKELONG('CA', 'FS') -- for the overall header's internal header.
 *
 * The multi-byte values do not have a defined endianness! The Casemul files
 * are generated from C++ classes, so when one is generated, the host's
 * endianness is used. There is no setting anywhere related to endianness,
 * but the endianness can be guessed using the magic word: if it is
 * "CASF", then the platform was big endian, otherwise ("ACFS"), the platform
 * was little endian (very probable, as Microsoft Windows is mostly used on
 * the x86 architecture, which is little endian). */

/* ---
 * Overall, source and compiled headers.
 * --- */

/* The overall header contains information about the other sections, and the
 * current state of the file (whether it contains the compiled part or not).
 *
 * Its internal header ID is MAKELONG('CA', 'FS') (abbreviated "CAFS" for
 * simplicity, see the 'problem' with endianness above).
 * The expected version is 1.00. */

# define casemul_compiled 0x80 /* If the compiled program is there. */

typedef struct casio_casemul_header_s {
	/* The flags. */

	casio_uint8_t  casio_casemul_header_flags;

	/* Offset of the sources. */

	casio_uint32_t casio_casemul_header_source_offset;

	/* Compiled program offset. */

	casio_uint32_t casio_casemul_header_compiled_offset;

	/* Some alignment? */

	casio_uint8_t  casio_casemul_header__align[3];
} casio_casemul_header_t;

/* At the beginning of the source part, we find the source header.
 * The source part contains the programs, pictures, matrixes and lists as four
 * blocks, uncompiled.
 *
 * Its internal header ID is "SRCE" (RSEC).
 * The expected version is 1.00. */

typedef struct casio_casemul_source_header_s {
	/* Number of program records in the program block. */

	casio_uint8_t  casio_casemul_source_header_programs;

	/* Number of picture records in the program block. */

	casio_uint8_t  casio_casemul_source_header_pictures;

	/* Number of matrix records in the program block. */

	casio_uint8_t  casio_casemul_source_header_matrixes;

	/* Number of list records in the list block. */

	casio_uint8_t  casio_casemul_source_header_lists;

	/* Program block offset. */

	casio_uint32_t casio_casemul_source_header_programs_offset;

	/* Picture block offset (length of the file before). */

	casio_uint32_t casio_casemul_source_header_pictures_offset;

	/* Matrix block offset. */

	casio_uint32_t casio_casemul_source_header_matrixes_offset;

	/* List block offset. */

	casio_uint32_t casio_casemul_source_header_list_offset;

	/* Main program ID. */

	casio_uint8_t  casio_casemul_source_header_program_id;

	/* Alignment. */

	casio_uint8_t  casio_casemul_source_header__align[3];
} casio_casemul_source_header_t;

/* At the beginning of the compiled part, we find the compiled header.
 * The compilation process is unknown (yet to read the sources to find out
 * what it is).
 *
 * Its internal header ID is "COMP" (OCPM).
 * The expected version is 1.00. */

typedef struct casio_casemul_compiled_header_s {
	/* The number of instructions (size of the part). */

	casio_uint32_t casio_casemul_compiled_header_size;
} casio_casemul_compiled_header_t;

/* ---
 * Record.
 * --- */

/* For each element, there is record, with a header and a subheader. The
 * record header cannot easily be expressed as a structure, so here it is, in
 * the form of a comment:
 *
 *	uint32_t name_length;
 *		The name length.
 *	uint8_t name[name_length];
 *		The name.
 *	uint32_t length;
 *		The subheader+data length.
 *
 *	Actually, it looks like the name length is always 12 and finishes with
 *	"W:g\xA7"; I don't know why, I'll have to investiguate on this. XXX
 *
 * Then expect an internal header, part of the subheader, that expresses the
 * type of it (expected version for all of them is 1.00). */

/* ---
 * Program.
 * --- */

/* A program has type "PROG" (RPGO). Its subheader is the following: */

typedef struct casio_casemul_prog_header_s {
	/* Program length after Casemul encoding. */

	casio_uint32_t casio_casemul_prog_header_length;
} casio_casemul_prog_header_t;

/* Casemul makes use of tokens instead of FONTCHARACTERs or Unicode - this
 * should be documented in the FONTCHARACTER reference. */

/* ---
 * Picture.
 * --- */

/* A picture has type "PICT" (IPTC), and has the following subheader: */

typedef struct casio_casemul_pict_header_s {
	/* Size. */

	casio_uint8_t  casio_casemul_pict_header_width;
	casio_uint8_t  casio_casemul_pict_header_height;

	/* ... aaaaand alignment. (you know, unchecked theory?) */

	casio_uint8_t  casio_casemul_pict_header__align[2];
} casio_casemul_pict_header_t;

/* Also, pixels are organized per _column_; this comes from the original code:
 *
 *	BYTE m_TabPixels[DATAPICTURE_CX][DATAPICTURE_CY];
 *
 * Notice that DATAPICTURE_CX and DATAPICTURE_CY are actually macros,
 * which means there are always 128x64 pixels (or at least that Casemul
 * will only be able to open 128x64 pictures)... */

/* ---
 * Matrix.
 * --- */

/* A matrix has type "MTRX" (TMXR) and its its subheader has the following
 * structure: */

typedef struct casio_casemul_mtrx_header_s {
	/* Number of lines and columns,
	 * actually int-s in the original code... */

	casio_uint32_t casio_casemul_mtrx_header_lines;
	casio_uint32_t casio_casemul_mtrx_header_columns;
} casio_casemul_mtrx_header_t;

/* Then it's simply a tab of `lines*columns` `double` numbers,
 * ordered by lines (y). They are the real parts, as the numbers in those
 * matrixes have got no imaginary parts. */

/* ---
 * List.
 * --- */

/* A list has type "LIST" (ILTS) and has the following subheader structure: */

typedef struct casio_casemul_list_header_s {
	/* Number of lines,
	 * actually int-s in the original code... */

	casio_uint32_t casio_casemul_list_header_lines;
} casio_casemul_list_header_t;

/* Then it's simply a tab of `lines` `double` numbers.
 * The numbers in these have no imaginary parts. */

CASIO_END_NAMESPACE
# pragma pack()
#endif /* LIBCASIO_FORMAT_CASEMUL_H */
