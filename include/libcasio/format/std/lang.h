/* ****************************************************************************
 * libcasio/format/std/lang.h -- the standard language file format description.
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
#ifndef  LIBCASIO_FORMAT_STD_LANG_H
# define LIBCASIO_FORMAT_STD_LANG_H
# include <libcasio/cdefs.h>
# include <stdint.h>
# pragma pack(1)
CASIO_BEGIN_NAMESPACE

/* ************************************************************************* */
/*  G1L - Language files for fx calculators                                  */
/* ************************************************************************* */
/* It all starts with a header: */

typedef struct casio_lang_subheader_s {
	/* identifier: is "PowerUSB" on original things */
	casio_uint8_t  casio_lang_subheader_identifier[8];

	/* OS information (raw binary format), e.g. 0x02,0x09 for 2.09. */
	casio_uint8_t  casio_lang_subheader_os_major;
	casio_uint8_t  casio_lang_subheader_os_minor;

	/* unknown bytes */
	casio_uint8_t  casio_lang_subheader__unknown[2];

	/* message count */
	casio_uint16_t casio_lang_subheader_count;
} casio_lang_subheader_t;

/* Then comes a list of offsets, and the messages.
 *
 * Each element of the offsets list is 16-bytes long.
 * It is relative to the first element (which starts right after the
 * offsets list - no alignment).
 *
 * The messages are null-terminated - once you get the offsets, you get
 * them completely. */
/* ************************************************************************* */
/*  G3L - Language files for Prizm                                           */
/* ************************************************************************* */
/* G3L and G3N start with the StandardHeader and the Standard Subheader,
 * then the Prizm-specific subheader. After this, both the G3L and G3N
 * have this language header: */

typedef struct casio_prizm_lang_subheader_s {
	/* sequence: '4C 59 37 35 35 00 00 00' (LY755   ) */
	casio_uint8_t  casio_prizm_lang_subheader_sequence[8];

	/* unknown: 0x02 */
	casio_uint8_t  casio_prizm_lang_subheader__unknown;

	/* unused byte. */
	casio_uint8_t  casio_prizm_lang_subheader__unused0;

	/* number of messages ("possibly 0 base indexed") */
	casio_uint32_t casio_prizm_lang_subheader_count;

	/* unused bytes */
	casio_uint8_t  casio_prizm_lang_subheader__unused1[2];
} casio_prizm_lang_subheader_t;

/* Then we have offsets of all messages (4 bytes each),
 * then messages themselves, zero-terminated.
 *
 * And don't forget the footer (see `libcasio/format/std.h`) */

CASIO_END_NAMESPACE
# pragma pack()
#endif /* LIBCASIO_FORMAT_STD_LANG_H */
