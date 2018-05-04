/* ****************************************************************************
 * libcasio/format/std/addin.h -- the standard add-in format description.
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
#ifndef  LIBCASIO_FORMAT_STD_ADDIN_H
# define LIBCASIO_FORMAT_STD_ADDIN_H
# include "../../cdefs.h"
# pragma pack(1)
CASIO_BEGIN_NAMESPACE

/* Add-ins are compiled programs. They only have one standard part,
 * These add-ins usually have the `g1a`, `g3a` or `c1a` extension. */

/* ---
 * Legacy fx systems add-ins (G1A).
 * --- */

/* G1A subheader is the following: */

# define G1A_ICON_WIDTH 30
# define G1A_ICON_HEIGHT 17

typedef struct casio_addin_subheader_s {
	/* the internal name, of format "@APPNAME".
	 *	useful for add-ins calling themselves... I guess? */
	casio_uint8_t  casio_addin_subheader_internal_name[8];

	/* spacing */
	casio_uint8_t  casio_addin_subheader__spacing0[3];

	/* the number of estrips (I don't know yet) */
	casio_uint8_t  casio_addin_subheader_estrips_count;

	/* spacing */
	casio_uint8_t  casio_addin_subheader__spacing1[4];

	/* the add-in version, of format "01.23.4567"
	 *	the "01.23" will be displayed in SYSTEM > VERSION */
	casio_uint8_t  casio_addin_subheader_version[10];

	/* spacing */
	casio_uint8_t  casio_addin_subheader__spacing2[2];

	/* the add-in creation type, of format "YYYY.MMDD.HHMM" */
	casio_uint8_t  casio_addin_subheader_creation_date[14];

	/* spacing */
	casio_uint8_t  casio_addin_subheader__spacing3[2];

	/* 30x17 pixel menu icon bitmap */
	casio_uint8_t  casio_addin_subheader_icon[68];

	/* spacing */
	casio_uint8_t  casio_addin_subheader__spacing4[0x144];

	/* program title */
	casio_uint8_t  casio_addin_subheader_title[8];

	/* spacing */
	casio_uint8_t  casio_addin_subheader__spacing5[0x14];

	/* and the filesize of the part! */
	casio_uint32_t casio_addin_subheader_filesize;

	/* spacing */
	casio_uint8_t  casio_addin_subheader__spacing6[0xc];
} casio_addin_subheader_t;

/* Then the G1A file will just contain the add-in code and stop. */

/* ---
 * Classpad and Prizm add-ins (C1A, G3A).
 * --- */

/* The two formats both have the Standard Subheader, then, for the C1A,
 * the Classpad-specific subheader, and for the G3A, the Prizm-specific
 * subheader.
 *
 * After this, the G3A subheader looks like this: */

# define G3A_ICON_WIDTH 92
# define G3A_ICON_HEIGHT 64

typedef struct casio_prizm_addin_subheader_s {
	/* Selected and unselected icon image. */

	casio_uint8_t casio_prizm_addin_subheader_selected_icon_image[0x3000];
	casio_uint8_t casio_prizm_addin_subheader_unselected_icon_image[0x3000];
} casio_prizm_addin_subheader_t;

/* And the C1A subheader goes like this: */

# define C1A_ICON_WIDTH 46
# define C1A_ICON_HEIGHT 30

typedef struct casio_classpad_addin_subheader_s {
	/* Unknown bytes. */

	casio_uint8_t  casio_classpad_addin_subheader__unknown0[0x2C];

	/* This is an approximation (46x30 pixels, packed 1-bit). */

	casio_uint8_t  casio_classpad_addin_subheader_icon[172];

	/* Unknown. */

	casio_uint8_t  casio_classpad_addin_subheader__unknown1[0xC54];
} casio_classpad_addin_subheader_t;

CASIO_END_NAMESPACE
# pragma pack()
#endif /* LIBCASIO_FORMAT_STD_ADDIN_H */
