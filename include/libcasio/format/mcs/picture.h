/* ****************************************************************************
 * libcasio/format/mcs/picture.h -- description of the MCS picture format.
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
#ifndef  LIBCASIO_FORMAT_MCS_PICTURE_H
# define LIBCASIO_FORMAT_MCS_PICTURE_H
# include "../../cdefs.h"
# pragma pack(1)

CASIO_BEGIN_NAMESPACE

/* Captures start with a simple header: */

typedef struct casio_mcs_captureheader_s {
	/* the width (0x80) */
	casio_uint16_t casio_mcs_captureheader_width;

	/* the height (0x40) */
	casio_uint16_t casio_mcs_captureheader_height;
} casio_mcs_captureheader_t;

/* Then the image follows (0x400 for a 0x80*0x40px image).
 * A picture is the same that an capture, but without the header and
 * containing two 128x64 images (0x800 bytes).
 *
 * Pictures don't have any header: it's just two 128x64 images (packed 1-bit)
 * and that's all. */

CASIO_END_NAMESPACE

# pragma pack()
#endif /* LIBCASIO_FORMAT_MCS_PICTURE_H */
