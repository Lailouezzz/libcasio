/* ****************************************************************************
 * libcasio/format/mcs/cells.h -- description of the MCS cells format.
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
#ifndef  LIBCASIO_FORMAT_MCS_CELLS_H
# define LIBCASIO_FORMAT_MCS_CELLS_H
# include <libcasio/cdefs.h>
# include <stdint.h>
# include <libcasio/number.h>
# pragma pack(1)
CASIO_BEGIN_NAMESPACE

/* Lists, matrixes and vectors have exactly the same format.
 * Lists only use the `height` (width is zero).
 * Vectors are normal matrixes (the OS just won't use other rows).
 *
 * They have this header: */

typedef struct casio_mcs_cellsheader_s {
	/* undocumented: probably the title? */
	casio_uint8_t  casio_mcs_cellsheader_title[8];

	/* height */
	casio_uint16_t casio_mcs_cellsheader_height;

	/* width */
	casio_uint16_t casio_mcs_cellsheader_width;

	/* re-undocumented */
	casio_uint8_t  casio_mcs_cellsheader__undocumented[3];

	/* undocumented byte (either 0x00 or 0x4F) */
	casio_uint8_t  casio_mcs_cellsheader__unknown;
} casio_mcs_cellsheader_t;

/* Then we have width*height BCD cells, which corresponds to the real parts,
 * grouped by height.
 *
 * If at least one of the cells has an imaginary part (highest bit of the
 * first nibble set), it is followed by a list of the imaginary parts, that
 * has the same size that the real parts list, that is grouped the same way
 * and that contains actual things (and not crap) only if the complex bit
 * is set on the real part.
 *
 * Notice that imaginary parts in cells files require at least
 * CASIOWIN 2.00. */

CASIO_END_NAMESPACE
# pragma pack()
#endif /* LIBCASIO_FORMAT_MCS_CELLS_H */
