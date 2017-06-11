/* ****************************************************************************
 * libcasio/format/cas/cell.h -- description of the CAS cell format.
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
#ifndef  LIBCASIO_FORMAT_CAS_CELL_H
# define LIBCASIO_FORMAT_CAS_CELL_H

/* Lists, matrixes and variables are sent/stored the same way.
 * Variables are either 1*1 matrixes, or 0*0 matrixes if unused.
 * Lists are 1*size matrixes.
 * TODO: check the specific subheader in CAS40 headers.
 *
 * There aren't any imaginary parts in matrixes and lists (this wasn't made
 * possible until CASIOWIN 2.00, so a long time after).
 *
 * Each cell is sent as an independent content part, which has the following
 * format:
 *
 * casio_uint16_t y;
 *	The ordinate of the cell, starting from 1.
 * casio_uint16_t x;
 *	The abscissa of the cell, starting from 1.
 * cas_bcd_t real;
 *	The real part of the variable.
 * cas_bcd_t imgn; [ONLY IF REAL PART HAS SPECIAL FLAG SET]
 *	The imaginary part of the variable.
 * casio_uint8_t checksum;
 *	The checksum (calculated as for the header).
 *
 * The order is unknown to me yet. (XXX: todo) */

#endif /* LIBCASIO_FORMAT_CAS_CELL_H */
