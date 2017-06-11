/* ****************************************************************************
 * libcasio/format/mcs/spreadsheet.h -- description of the MCS spreadsheet fmt.
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
#ifndef  LIBCASIO_FORMAT_MCS_SPREADSHEET_H
# define LIBCASIO_FORMAT_MCS_SPREADSHEET_H
# include <libcasio/cdefs.h>
# include <stdint.h>
# include <libcasio/number.h>
# pragma pack(1)
CASIO_BEGIN_NAMESPACE

/* Spreadsheets are more complicated than that.
 *
 * For normal ones, there is a header: */

typedef struct casio_mcs_spreadsheet_header_s {
	/* has subheader: 0x01 if yes */
	casio_uint8_t  casio_mcs_spreadsheet_header_has_subheader;

	/* column count (max: 26), on 24 bits and non-aligned */
	casio_uint32_t casio_mcs_spreadsheet_header_column_count :24;
} casio_mcs_spreadsheet_header_t;

/* Then, if it is a normal spreadsheet, there is a subheader,
 * a column directory and a column definition table.
 *
 * Here's the subheader: */

typedef struct casio_mcs_spreadsheet_subheader_s {
	/* alignment or magic? {0, 0, 0, 0} */
	casio_uint8_t  casio_mcs_spreadsheet_subheader__align0[4];

	/* number of column definitions */
	casio_uint32_t casio_mcs_spreadsheet_subheader_defs_size;

	/* alignment or magic II? {0, 0, 0, 0} */
	casio_uint8_t  casio_mcs_spreadsheet_subheader__align1[4];
} casio_mcs_spreadsheet_subheader_t;

/* The column definition table is the main definition. It contains the row
 * directory, which is a 80-bytes long bitfield indicating if cells are empty
 * or not, and the list of non-empty cells.
 *
 * Each cell is a BCD structure (the real part of the number). Numbers in
 * spreadsheets can't have imaginary parts.
 *
 * The column directory is just a list of 4-bytes sizes of each column,
 * counting the row directory and the cells. It's easier to naviguate to
 * a column quicker. */

CASIO_END_NAMESPACE
# pragma pack()
#endif /* LIBCASIO_FORMAT_MCS_SPREADSHEET_H */
