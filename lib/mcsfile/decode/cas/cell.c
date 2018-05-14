/* ****************************************************************************
 * mcsfile/decode/cas/cell.c -- decode a CAS matrix/list/variable.
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

/* ---
 * Internal function.
 * --- */

/**
 *	decode_cell:
 *	Read a cell.
 *
 *	@arg	buffer		the buffer to read from.
 *	@arg	cell		the cell to fill.
 *	@arg	x			the abscissa of the cell.
 *	@arg	y			the ordinate of the cell.
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL int decode_cell(casio_stream_t *buffer, casio_mcscell_t *cell,
	unsigned int *x, unsigned int *y)
{
	unsigned char csum = 0, checksum;
	casio_uint16_t fx, fy;
	casio_casbcd_t wkg;

	cell->casio_mcscell_flags = casio_mcscellflag_used;

	/* read position */
	DREAD(fx) *x = be16toh(fx) - 1;
	DREAD(fy) *y = be16toh(fy) - 1;
	csum = casio_checksum_cas(&fx, sizeof(casio_uint16_t), csum);
	csum = casio_checksum_cas(&fy, sizeof(casio_uint16_t), csum);

	/* read the parts */
	DREAD(wkg)
	csum = casio_checksum_cas(&wkg, sizeof(casio_casbcd_t), csum);
	if (casio_bcd_fromcas(&cell->casio_mcscell_real, &wkg)) {
		READ(&wkg, sizeof(casio_casbcd_t))
		csum = casio_checksum_cas(&wkg, sizeof(casio_casbcd_t), csum);
		casio_bcd_fromcas(&cell->casio_mcscell_imgn, &wkg);
	}

	/* read and check the checksum */
	DREAD(checksum)
	if (csum != checksum)
		return (casio_error_csum);

	/* no prob'! */
	return (0);
}

/* ---
 * Cell-reading CAS part decoding functions.
 * --- */

/**
 *	casio_decode_caspart_matrix:
 *	Decode a CAS matrix part.
 *
 *	@arg	handle		the handle to contribute to.
 *	@arg	buffer		the buffer to read from.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_decode_caspart_matrix(casio_mcsfile_t *handle,
	casio_stream_t *buffer)
{
	int err; casio_mcscell_t cell;
	unsigned int y, x;

	/* read the cell */
	err = decode_cell(buffer, &cell, &x, &y);
	if (err) return (err);

	/* save the cell (FIXME: secure) */
	handle->casio_mcsfile_cells[y][x] = cell;

	/* check if its the last cell */
	if (y == handle->casio_mcsfile_head.casio_mcshead_height - 1
	 && x == handle->casio_mcsfile_head.casio_mcshead_width - 1)
		handle->casio_mcsfile_head.casio_mcshead_flags &=
			~casio_mcsflag_unfinished;

	/* no error! */
	return (0);
}

/**
 *	casio_decode_caspart_var:
 *	Decode a CAS variable part.
 *
 *	@arg	handle		the handle to contribute to.
 *	@arg	buffer		the buffer to read from.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_decode_caspart_var(casio_mcsfile_t *handle,
	casio_stream_t *buffer)
{
	int err; unsigned int x, y;

	/* read and save the cell */
	err = decode_cell(buffer, &handle->casio_mcsfile_var, &x, &y);
	if (err) return (err);
	if (x != 1 || y != 1) return (casio_error_magic);

	/* no error! */
	handle->casio_mcsfile_head.casio_mcshead_flags &=
		~casio_mcsflag_unfinished;
	return (0);
}
