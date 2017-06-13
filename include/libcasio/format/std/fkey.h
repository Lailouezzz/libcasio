/* ****************************************************************************
 * libcasio/format/std/fkey.h -- the std function keys file format description.
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
#ifndef  LIBCASIO_FORMAT_STD_FKEY_H
# define LIBCASIO_FORMAT_STD_FKEY_H
# include "../../cdefs.h"
# pragma pack(1)
CASIO_BEGIN_NAMESPACE

/* Function keys are the little boxes at the bottom with text in it, to tell
 * you what you're going to access if you press F1-F6. */
/* ************************************************************************* */
/*  G1N - Function-keys files for fx calculators                             */
/* ************************************************************************* */
/* In G1Ns, function keys are 19x8 1-bit with fill bits images.
 *
 * TODO: it is unknown yet how to identify G1N files, as no non-community-made
 * one of them was found and the calculator software just skips
 * StandardHeader. It has to be found for a correct integration in libcasio!
 *
 * The format looks a lot like G1L files. The header is the following: */

# define FKEY_WIDTH 24
# define FKEY_HEIGHT 8
typedef struct casio_fkey_subheader_s {
	/* usually "PowerUSB" */
	casio_uint8_t  casio_fkey_subheader_identifier[8];

	/* useless data */
	casio_uint8_t  casio_fkey_subheader__useless[4];

	/* number of icons */
	casio_uint16_t casio_fkey_subheader_count;
} casio_fkey_subheader_t;

/* Then there is a table of 16-bits offsets (iconXXX - icon0), then the icons.
 * The first "icon" is in fact the name of the language. */
/* ************************************************************************* */
/*  G3L-N - Function-keys files for CG calculators                           */
/* ************************************************************************* */
/* G3N (also named G3L-N) have exactly the same structure than G3Ls (see
 * `libcasio/format/std/lang.h`), but messages are 64x24 1-bit pictures. */

# define FKEY3_WIDTH 64
# define FKEY3_HEIGHT 24

/* TODO: it is unknown yet how to identify G3N/G3L files using the
 * StandardHeader, as no 'official' one of them was found and the calculator
 * software just skips StandardHeader. It should be found for a better
 * integration in libcasio! */

CASIO_END_NAMESPACE
# pragma pack()
#endif /* LIBCASIO_FORMAT_STD_FKEY_H */
