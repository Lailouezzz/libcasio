/* ****************************************************************************
 * libcasio/format/std.h -- the CASIO "standard" file format description.
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
#ifndef  LIBCASIO_FORMAT_STD_H
# define LIBCASIO_FORMAT_STD_H
# include "../cdefs.h"
# pragma pack(1)
CASIO_BEGIN_NAMESPACE

/* This format is used by CASIO with its calculator for storing things
 * like add-ins (compiled programs), MCS (main memory saves), and others.
 * In fact, there is no name for the general format, only names for the
 * "subformats" based on this one (which are mainly made of their extensions,
 * such as G1M or G3P). */
/* ************************************************************************* */
/*  Standard Header                                                          */
/* ************************************************************************* */
/* It all starts with a header, called Standard Header by Simon Lothar.
 * This Standard Header contains the total filesize, the standard type (add-in,
 * MCS, e-acts), some data that will be useful for the MCS type, and some
 * magic and control bytes.
 *
 * For some reason, this StandardHeader is INVERTED on every file it's on,
 * you will have to apply a NOT operation on its bytes for it to make sense.
 * Keep in mind that, everywhere in the header, multi-bytes integers
 * are BIG ENDIAN.
 *
 * The LSB is the Least Significant Byte: once adapted to the host endianness,
 * it can simply be obtained by bitwise-AND-ing with 0xff. */

typedef struct casio_standard_header_s {
	/* the file identifier */
	casio_uint8_t  casio_standard_header_main_id[8];

	/* our subtype! */
	casio_uint8_t  casio_standard_header_subtype[6];

	/* first control byte: filesize LSB + 0x41 */
	casio_uint8_t  casio_standard_header_control;

	/* said to be 0x01, but doesn't need to */
	casio_uint8_t  casio_standard_header_align_one;

	/* total filesize */
	casio_uint32_t casio_standard_header_filesize;

	/* second control byte: filesize LSB + 0xb8 */
	casio_uint8_t  casio_standard_header_control2;

	/* alignment */
	casio_uint8_t  casio_standard_header__align[7];

	/* is obfuscated - useful for G3P */
	casio_uint8_t  casio_standard_header_obfuscated0;
	casio_uint8_t  casio_standard_header_obfuscated1;

	/* number of objects contained (useful for MCS filetype) */
	casio_uint16_t casio_standard_header_number;
} casio_standard_header_t;

/* At the beginning, we thought "USBPower" was some magic sequence we would
 * systematically find in the "main_id" field. But counter examples came:
 * the G3L, whose main ID was "Ly755   ", and the C2P.
 *
 * We also thought the subtype was only one-byte long, but the C2P came along
 * with its "c2p\0\0\0" subtype.
 * All main ID/types correspondances are in the `src/utils/type.c` file.
 * From a user program, you can use the functions
 * in `include/libcasio/formatutils.h`.
 *
 * After the Standard Header is read and the type is read, we have parts,
 * each with their own subheaders and their own subtilities. */
/* ************************************************************************* */
/*  Standard Subheader                                                       */
/* ************************************************************************* */
/* Some Prizm/Classpad-related formats (language, fkeys, add-ins) use a
 * common subheader followed by a platform-specific subheader (which is the
 * same size of Prizm and Classpad, but doesn't seem to have the same
 * field organization). Here is the common subheader structure: */

typedef struct casio_standard_subheader_s {
	/* checksum */
	casio_uint32_t casio_standard_subheader_checksum;

	/* file type:
	 * - 0x00: picture (not used with this header);
	 * - 0x01: add-in;
	 * - 0x02: function keys;
	 * - 0x04: language files; */
	casio_uint8_t  casio_standard_subheader_filetype;

	/* platform:
	 * - 0x00: fx-CP;
	 * - 0x01: Prizm */
	casio_uint8_t  casio_standard_subheader_platform;

	/* unknown */
	casio_uint8_t  casio_standard_subheader__unknown0[4];

	/* size of the data + footer? */
	casio_uint32_t casio_standard_subheader_df_size;

	/* control:
	 * - for a G3A: filesize - 0x7000 - 4
	 * - for a C1A: filesize - 0x1000 - 4 */
	casio_uint32_t casio_standard_subheader_control;

	/* unknown */
	casio_uint8_t  casio_standard_subheader__unknown1[4];

	/* message zone size - unreliable (valid could have zero) */
	casio_uint32_t casio_standard_subheader_message_zone_size;

	/* model; for C1A: GY437 */
	casio_uint8_t  casio_standard_subheader_models[6];

	/* title/language name */
	casio_uint8_t  casio_standard_subheader_title[28];

	/* filesize */
	casio_uint32_t casio_standard_subheader_filesize;

	/* internal name */
	casio_uint8_t  casio_standard_subheader_internal_name[11];

	/* language labels */
	casio_uint8_t  casio_standard_subheader_labels[8][24];

	/* eAct strip flag (0x00: can't be used, 0x01: can be used) */
	casio_uint8_t  casio_standard_subheader_eact_strip_flag;

	/* unknown */
	casio_uint8_t  casio_standard_subheader__unknown2[4];

	/* version: MM.mm.ffff */
	casio_uint8_t  casio_standard_subheader_version[10];

	/* unknown */
	casio_uint8_t  casio_standard_subheader__unknown3[2];

	/* timestamp: YYYY.MMDD.HHmm */
	casio_uint8_t  casio_standard_subheader_timestamp[14];
} casio_standard_subheader_t;

/* Here is the Prizm-specific subheader: */

typedef struct casio_standard_prizm_subheader_s {
	/* unknown */
	casio_uint8_t  casio_standard_prizm_subheader__unknown0[38];

	/* eAct strip labels */
	casio_uint8_t  casio_standard_prizm_subheader_eact_strip_labels[8][36];

	/* eAct icon */
	casio_uint8_t  casio_standard_prizm_subheader_icon[0x300];

	/* unknown stuff */
	casio_uint8_t  casio_standard_prizm_subheader__unknown1[0x90C];

	/* language name (null terminated?) */
	casio_uint8_t  casio_standard_prizm_subheader_language_name[16];

	/* language salutation (null terminated?) */
	casio_uint8_t  casio_standard_prizm_subheader_language_salutation[16];

	/* filename (extension included) */
	casio_uint8_t  casio_standard_prizm_subheader_filename[0x144];
} casio_standard_prizm_subheader_t;

/* And here is the Classpad-specific subheader: */

typedef struct casio_standard_classpad_subheader_s {
	/* unknown */
	casio_uint8_t  casio_standard_classpad_subheader__unknown0[0x46];

	/* the C1A filename */
	casio_uint8_t  casio_standard_classpad_subheader_filename[0x144];

	/* unknown */
	casio_uint8_t  casio_standard_classpad_subheader__unknown1[0x2C];

	/* icon (maybe 46x30 pixels? packed 1-bit) */
	casio_uint8_t  casio_standard_classpad_subheader_icon[172];

	/* unknown */
	casio_uint8_t  casio_standard_classpad_subheader__unknown2[0xC54];
} casio_standard_classpad_subheader_t;

/* Also, if the platform is the Prizm, there is a footer at the end of the
 * file, which is only made of a 32-bit checksum that should be equal to
 * the subheader checksum. */
/* ************************************************************************* */
/*  Standard Picture Header                                                  */
/* ************************************************************************* */
/* Picture formats (C2P, G3P) have a common picture standard subheader, which
 * is the following: */

typedef struct casio_standard_picheader_s {
	/* magic sequence: "CC" for the Classpad, "CP" for the Prizm */
	casio_uint8_t  casio_standard_picheader_cp[2];

	/* "0100", either in raw (0x00, 0x01) for G3P
	 * or in ASCII (0x30, 0x31) for C2P */
	casio_uint8_t  casio_standard_picheader_magic[4];

	/* magic sequences:
	 * - "ColorCP\0" for the C2P;
	 * - "LY755\0\0\0" for the G3P. */
	casio_uint8_t  casio_standard_picheader_magic2[8];

	/* unknown byte: 0x02 on the G3P, 0x00 on the C2P */
	casio_uint8_t  casio_standard_picheader__unknown0;

	/* unused byte */
	casio_uint8_t  casio_standard_picheader__unused;

	/* filesize after the StandardHeader */
	casio_uint32_t casio_standard_picheader_filesize;

	/* "Always contains the 32-bit big-endian integer 0x00000001" */
	casio_uint32_t casio_standard_picheader__alwaysone;

	/* subsize (what's after the standard picture header) */
	casio_uint32_t casio_standard_picheader_subsize;

	/* unknown gap */
	casio_uint8_t  casio_standard_picheader__unknown2[0x7C];
} casio_standard_picheader_t;

/* Then we have the specific header -- see `libcasio/format/std/picture.h`. */
/* ************************************************************************* */
/*  Flavors                                                                  */
/* ************************************************************************* */
/* Where do you want to go next? Pick your poison. */

CASIO_END_NAMESPACE
# pragma pack()
# include "std/addin.h"
# include "std/eact.h"
# include "std/mcs.h"
# include "std/picture.h"
# include "std/lang.h"
# include "std/fkey.h"
#endif /* LIBCASIO_FORMAT_STD_H */
