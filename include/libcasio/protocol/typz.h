/* ****************************************************************************
 * libcasio/protocol/typz.h -- TYPZ capture detail.
 * Copyright (C) 2016-2017 Thomas "Cakeisalie5" Touhey <thomas@touhey.fr>
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
#ifndef  LIBCASIO_PROTOCOL_TYPZ_H
# define LIBCASIO_PROTOCOL_TYPZ_H
# include "../cdefs.h"
CASIO_BEGIN_NAMESPACE

/* In both Protocol 7.00 and extended SCSI protocols, recent CASIO
 * calculators (Prizm) use a TYPZ subheader to give details about the
 * capture they're sending. It's name is this because the five-letter ID
 * is "TYPZ1" or "TYPZ2". The subheader is the following: */

typedef struct casio_typz_s {
	/* ASCII-hex size */
	unsigned char casio_typz_size[6];

	/* dimensions */
	unsigned char casio_typz_height[4];
	unsigned char casio_typz_width[4];

	/* one: we are number one but it is in ascii (always "1") */
	unsigned char casio_typz_one;

	/* encoding algorithm:
	 * - RC2: 16-bit mode;
	 * - RC3: 3-bit mode (1 nib./pxl, red-green-blue-trash)
	 * - RM2: 2-bit mode? */
	unsigned char casio_typz_enc[3];
} casio_typz_t;

CASIO_END_NAMESPACE
#endif /* LIBCASIO_PROTOCOL_TYPZ_H */
