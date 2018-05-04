/* ****************************************************************************
 * libcasio/format/cas/picture.h -- description of the CAS picture format.
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
#ifndef  LIBCASIO_FORMAT_CAS_PICTURE_H
# define LIBCASIO_FORMAT_CAS_PICTURE_H
# include "../../cdefs.h"
# pragma pack(1)

CASIO_BEGIN_NAMESPACE

/* ---
 * CAS40 specific header bytes.
 * --- */

/* In old CAS headers, the five specific bytes are the following: */

typedef struct casio_cas_spe_screenshot_s {
	casio_uint8_t casio_cas_spe_screenshot_height;
	casio_uint8_t casio_cas_spe_screenshot_width;
	casio_uint8_t casio_cas_spe_screenshot__unused[3];
} casio_cas_spe_screenshot_t;

/* The specific bytes for a number start with either "RA" or "CA", 'R' or 'C'
 * meaning the number is complex or not. */

/* ---
 * Content.
 * --- */

/* The width and height are given in the CAS50 header/CAS40 specific bytes.
 * The picture format is either `casio_pictureformat_4bit_mono` or
 * `casio_pictureformat_4bit_color` -- see `libcasio/picture.h`. */

CASIO_END_NAMESPACE

# pragma pack()
#endif /* LIBCASIO_FORMAT_CAS_PICTURE_H */
