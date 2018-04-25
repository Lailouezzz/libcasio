/* ****************************************************************************
 * mcsfile/decode/mcs/ssheet.c -- decode an MCS spreadsheet.
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
#include "../decode.h"

/**
 *	casio_decode_mcs_spreadsheet:
 *	Decode a spreadsheet.
 *
 *	@arg	h			the handle to make.
 *	@arg	buffer		the buffer to read from.
 *	@arg	head		the pre-filled head to complete and use.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_decode_mcs_spreadsheet(casio_mcsfile_t **h,
	casio_stream_t *buffer, casio_mcshead_t *head)
{
	int err;
	casio_mcsfile_t *handle;
	casio_mcs_spreadsheet_header_t hd;
	casio_mcs_spreadsheet_subheader_t shd;
	casio_mcscell_t *cells = NULL, **tab;
	unsigned long colcount; int cells_count = 0;
	unsigned long rows = 0, cols = 0;
	unsigned long x, y;

	/* read header */
	GDREAD(hd)
	if (hd.casio_mcs_spreadsheet_header_has_subheader != 0x01)
		return (0);

	/* read subheader */
	GDREAD(shd)
	colcount = hd.casio_mcs_spreadsheet_header_column_count;
	colcount = be32toh(colcount << 8);
	shd.casio_mcs_spreadsheet_subheader_defs_size =
		be32toh(shd.casio_mcs_spreadsheet_subheader_defs_size);

	/* prepare */
	cells = casio_alloc(1000 * colcount, sizeof(casio_mcscell_t));
	memset(cells, 0, sizeof(casio_mcscell_t) * 1000 * colcount);

	/* log some info */
	msg((ll_info, "%lu columns to read!", colcount));

	if (colcount) {
		casio_uint32_t column_directory[25]; /* FIXME */
		unsigned long c;

		/* get the column directory */
		GREAD(&column_directory, sizeof(casio_uint32_t) * colcount)

		/* browse columns */
		for (c = 0; c < colcount; c++) {
			unsigned char row_directory[0x80];
			unsigned char *rd = row_directory;
			int bit = 1 << 7;

			/* check if column is empty */
			if (!column_directory[c])
				continue;

			/* get the row directory */
			GREAD(&row_directory, (size_t)0x80)

			/* explore each cell */
			for (y = 0; y < 1000; y++) {
				/* check if used */
				if (*rd & bit) {
					casio_mcscell_t *cell;
					casio_mcsbcd_t rawcell;

					/* get cell */
					GDREAD(rawcell)

					/* read cell */
					cell = &cells[c * 1000 + 1];
					casio_bcd_frommcs(&cell->casio_mcscell_real, &rawcell);
					cell->casio_mcscell_flags = casio_mcscellflag_used;

					/* check things (max row, max col, cells count) */
					rows = max(rows, y);
					cols = c;
					cells_count++;
				}

				/* iterate bit and rd */
				rd += (bit & 1);
				bit = (bit >> 1) | ((bit & 1) << 7);
			}
		}
	}

	/* we have max rows and columns, increment to have sizes */
	rows++, cols++;

	/* create final tab */
	head->casio_mcshead_width = 0; head->casio_mcshead_height = 0;
	if (cells_count) {
		head->casio_mcshead_width = cols;
		head->casio_mcshead_height = rows;
	}
	err = casio_make_mcsfile(h, head);
	if (err) return (err);
	handle = *h;

	/* main copying loop */
	tab = handle->casio_mcsfile_cells;
	for (y = 0; y < head->casio_mcshead_height; y++)
	  for (x = 0; x < head->casio_mcshead_width; x++)
		tab[y][x] = cells[x * 1000 + y];

	/* end */
	err = 0;
fail:
	casio_free(cells);
	return (err);
}
