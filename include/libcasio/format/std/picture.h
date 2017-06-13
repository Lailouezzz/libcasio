/* ****************************************************************************
 * libcasio/format/std/picture.h -- the picture formats description.
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
#ifndef  LIBCASIO_FORMAT_STD_PICTURE_H
# define LIBCASIO_FORMAT_STD_PICTURE_H
# include "../../cdefs.h"
# pragma pack(1)
CASIO_BEGIN_NAMESPACE

/* ************************************************************************* */
/*  G3P: pictures for Prizm                                                  */
/* ************************************************************************* */
/* G3P are pictures for fx-CG. They start with a Standard Header, and
 * a Standard Picture Header.
 *
 * This mysteries over this format have been unraveled by many, but I'm using
 * Simon Lothar's documentation and Kerm Martian's articles.
 *
 * Prizm pictures (G3P) support several image depths (encodings): */

# define g3p_color_4bit  0x03
# define g3p_color_16bit 0x10

/* After the standard headers, comes the specific header: */

typedef struct casio_prizm_picture_subheader_s {
	/* magic sequence? is: 0x00, 0x01, 0x00, 0x00 */
	casio_uint8_t  casio_prizm_picture_subheader_magic[4];

	/* size of the image data + footer */
	casio_uint32_t casio_prizm_picture_subheader_df_size;

	/* w0t */
	casio_uint8_t  casio_prizm_picture_subheader__unknown0[2];

	/* width */
	casio_uint16_t casio_prizm_picture_subheader_width;

	/* height */
	casio_uint16_t casio_prizm_picture_subheader_height;

	/* color depth - see `g3p_colorsize` */
	casio_uint16_t casio_prizm_picture_subheader_color_depth;

	/* undocumented, again */
	casio_uint8_t  casio_prizm_picture_subheader__unknown1[4];

	/* length of the image data + 6
	 * the 6 bytes are:
	 * - the 16-bit generator ID at the beginning;
	 * - the 32-bit checksum at the end of the data. */
	casio_uint32_t casio_prizm_picture_subheader_data_size;

	/* Generator ID */
	casio_uint16_t casio_prizm_picture_subheader_generator_id;
} casio_prizm_picture_subheader_t;

/* The generator ID is a 16-bit ID giving information about the picture
 * generator. The known IDs are the following: */

# define g3p_id_casio0    0x3C1B /* one footer */
# define g3p_id_casio1    0x3E93 /* three footers */
# define g3p_id_capture   0x388D /* no footer */
# define g3p_id_converter 0x789C /* one footer */

/* Then we have the deflated image data, which is `data_size - 6` sized
 * (see the `data_size` comment above for more explanation).
 *
 * Before inflating, you should check if the image is obfuscated (Simon Lothar
 * says 'encrypted', I suppose that's more polite). It is if:
 *
 *		std.obfuscated0 != std.subtype[0]
 *			+ ((std.filesize & 0xFF00) >> 8) + (std.filesize & 0xFF)
 *
 * Which is checked in `casio_decode_std`.
 * If it is, then in order to de-obfuscate, you should do the following
 * operations:
 *
 *		byte = ~byte;
 *		byte = (byte << 5) | (byte >> 3);
 *
 * For completeness' sake, here are the operations you'll need to do to
 * obfuscate while making a Prizm picture:
 *
 *		byte = (byte >> 5) | (byte << 3);
 *		byte = ~byte;
 *
 * Then you can inflate.
 * Once this is done, you can get the next four bytes, also considered as
 * part of the data: the 32-bit adler32 checksum. It is a checksum over the
 * raw, uncompressed data (and is appended to it before deflating/obfuscating).
 *
 * TODO: footers */
/* ************************************************************************* */
/*  C2P: Images for Classpads                                                */
/* ************************************************************************* */
/* C2P are pictures for CASIO Classpad calculators (fx-CP*). They start with
 * a Standard Header, followed by a Standard Picture Header.
 *
 * Then comes the specific subheader: */

typedef struct casio_classpad_picture_subheader_s {
	/* "0100" */
	casio_uint8_t  casio_classpad_picture_subheader_magic[4];

	/* footer offset? `filesize - 0x254` */
	casio_uint32_t casio_classpad_picture_subheader_footer_offset;

	/* unknown */
	casio_uint16_t casio_classpad_picture_subheader__unknown0;

	/* width (max for screen: 0x136 */
	casio_uint16_t casio_classpad_picture_subheader_width;

	/* height (max for screen: 0x191) */
	casio_uint16_t casio_classpad_picture_subheader_height;

	/* undocumented, again */
	casio_uint8_t  casio_classpad_picture_subheader__undocumented[18];

	/* filesize - 0x258 */
	casio_uint32_t casio_classpad_picture_subheader__size3;
} casio_classpad_picture_subheader_t;

/* Then there is the zlib header with default compression.
 * The pixels format is R5G6B5.
 *
 * After the image, there is a footer, which is mainly undocumented, but
 * here is what we've got: */

typedef struct casio_classpad_picture_footer_s {
	/* "0100", again */
	casio_uint8_t casio_classpad_picture_footer_magic[4];

	/* and some undocumented stuff */
	casio_uint8_t casio_classpad_picture_footer__undocumented[0xE4];
} casio_classpad_picture_footer_s;

CASIO_END_NAMESPACE
# pragma pack()
#endif /* LIBCASIO_FORMAT_STD_PICTURE_H */
