/* ****************************************************************************
 * libcasio/format/std/eact.h -- the standard e-activity format description.
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
#ifndef  LIBCASIO_FORMAT_STD_EACT_H
# define LIBCASIO_FORMAT_STD_EACT_H
# include "../../cdefs.h"
# pragma pack(1)
CASIO_BEGIN_NAMESPACE

/* E-Activities are the format CASIO uses for in-calc documents.
 * It is the funniest subformat the the libcasio can decode.
 *
 * There is only one part for e-activities.
 * The overall header has two zones:
 * - the special header, that keeps the global info;
 * - the setup area, that keeps one part of the calculator setup.
 *
 * There are several types for E-Activies, identified by the extensions
 * CASIO give them.
 * The E-Act version in the header is 0x10100 for g1e, 0x10200 for g2e,
 * and 0x10400 for g3e. */

# define EACT_G1E 0x10100
# define EACT_G2E 0x10200
# define EACT_G3E 0x10400

/* The differences between these formats are the following:
 * - On g1e, the OS version is always 0x5061636B, where on g2e/g3e,
 *   the OS version is the version of the OS on which the file was written.
 * - On g1e/g2e, the setup directory is 0x10-bytes long, where on g3e,
 *   the setup directory is 0x2C-bytes long.
 *
 * So the main header is: */

typedef struct casio_eact_header_s {
	/* the filesize */
	casio_uint32_t casio_eact_header_filesize;

	/* the size of the setup area */
	casio_uint32_t casio_eact_header_setup_area_size;

	/* the E-Act version */
	casio_uint32_t casio_eact_header_eact_version;

	/* The OS version - only with G2E/G3E */
	casio_uint32_t casio_eact_header_os_version;
} casio_eact_header_t;

/* And now, the funniest part: the content.
 * So a content has this header: */

typedef struct casio_eact_content_header_s {
	/* content type: @EACT, @RUNMAT, ... */
	casio_uint8_t  casio_eact_content_header_type[8];

	/* align for element name */
	casio_uint8_t  casio_eact_content_header__align0[8];

	/* element name: "EACT1", "TEXT1", ... */
	casio_uint8_t  casio_eact_content_header_name[16];

	/* align for subheader */
	casio_uint8_t  casio_eact_content_header__align1[4];
} casio_eact_content_header_t;

/* There are several types of contents.
 * The most basic one is also called "@EACT".
 * It has this content subheader: */

typedef struct casio_eact_content_eact_header_s {
	/* line count */
	casio_uint32_t casio_eact_content_eact_header_line_count;
} casio_eact_content_eact_header_t;

/* And after, lines come. First of all, there is a line descriptor table,
 * that contain the line type and the line offset from the subheader begin.
 * A line type can have these types: */

typedef int casio_eact_ltype_t;
# define casio_eact_ltype_calc        0x03 /* calculation */
# define casio_eact_ltype_calc_result 0x04 /* calculation result
                                            * usually right after a calc. */
# define casio_eact_ltype_content     0x06 /* subcontent */
# define casio_eact_ltype_stdheading  0x07 /* standard heading
                                            * title is surrounded by '=' */
# define casio_eact_ltype_picture     0x08 /* path to a picture
                                            * e.g. '\\fls0\Pict\Pict01.g3p'
                                            * (fixed-width FONTCHARACTER)*/
# define casio_eact_ltype_text        0x81 /* pure text */
# define casio_eact_ltype_code        0x82 /* text mixed up with functions */

/* And here's the line descriptor structure: */

typedef struct casio_eact_line_descriptor_s {
	/* the entry type */
	casio_uint8_t  casio_eact_line_descriptor_type;

	/* the entry offset */
	casio_uint32_t casio_eact_line_descriptor_offset :24;
} casio_eact_line_descriptor_t;

/* And there is only one content after the main header, and it's a content
 * of EACT type. But hey, why the heck have we defined several types
 * and stuff?
 * Well, that's the funniest thing of E-Activities:
 *
 *                      they are RECURSIVE. [drama alert]
 *
 * Which means in each node, there can be a content to parse. */

CASIO_END_NAMESPACE
# pragma pack()
#endif /* LIBCASIO_FORMAT_STD_EACT_H */
