/* ****************************************************************************
 * mcsfile/decode/mcs/cells.c -- decode an MCS cells tab.
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
 *	casio_decode_mcs_cells:
 *	Decode a cells tab.
 *
 *	@arg	h			the handle to make.
 *	@arg	buffer		the buffer to read from.
 *	@arg	head		the pre-filled head to complete and use.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_decode_mcs_cells(casio_mcsfile_t **h,
	casio_stream_t *buffer, casio_mcshead_t *head)
{
	int err = casio_error_alloc;
	casio_mcs_cellsheader_t hd;
	casio_mcsfile_t *handle;
	casio_mcscell_t **tab;
	int one_imgn = 0;
	unsigned long cw, ch, cx, cy;

	/* read header */
	DREAD(hd)

	/* log info */
	cw = be16toh(hd.casio_mcs_cellsheader_width);
	ch = (head->casio_mcshead_type & casio_mcstype_list)
		? 1 : be16toh(hd.casio_mcs_cellsheader_height);
	msg((ll_info, "Matrix size is %lu*%lu", cw, ch));

	/* make final head */
	head->casio_mcshead_width  = cw;
	head->casio_mcshead_height = ch;
	if ((err = casio_make_mcsfile(h, head)))
		return (err);
	handle = *h;

	/* main copying loop */
	tab = handle->casio_mcsfile_cells;
	for (cy = 0; cy < ch; cy++) for (cx = 0; cx < cw; cx++) {
		casio_mcscell_t *cell = &tab[cy][cx];
		casio_mcsbcd_t   rawbcd;

		/* read the cell */
		GDREAD(rawbcd)

		/* store it. */
		one_imgn |= casio_bcd_frommcs(&cell->casio_mcscell_real, &rawbcd);
		cell->casio_mcscell_flags = casio_mcscellflag_used;
	}

	/* check imaginary parts */
	if (one_imgn) for (cy = 0; cy < ch; cy++) for (cx = 0; cx < cw; cx++) {
		casio_mcscell_t *cell = &tab[cy][cx];
		casio_mcsbcd_t   rawbcd;

		GDREAD(rawbcd)
		casio_bcd_frommcs(&cell->casio_mcscell_imgn, &rawbcd);
	}

#if !defined(LIBCASIO_DISABLED_LOG)
	/* logging loop */
	if (islog(ll_info)) {
		char rbuf[CASIO_BCD_GOODBUFSIZE], ibuf[CASIO_BCD_GOODBUFSIZE];
		for (cy = 0; cy < ch; cy++) for (cx = 0; cx < cw; cx++) {
			casio_bcdtoa(rbuf, CASIO_BCD_GOODBUFSIZE,
				&tab[cy][cx].casio_mcscell_real);

			if (casio_bcd_has_special(&tab[cy][cx].casio_mcscell_real)) {
				casio_bcd_t ibcd = tab[cy][cx].casio_mcscell_imgn;
				int sign = casio_bcd_is_negative(&ibcd) ? '-' : '+';
				ibcd.casio_bcd_flags &= ~casio_bcdflag_neg;

				casio_bcdtoa(ibuf, CASIO_BCD_GOODBUFSIZE, &ibcd);
				msg((ll_info, "[%lu][%lu] %s %c %si",
					cy, cx, rbuf, sign, ibuf));
			} else
				msg((ll_info, "[%lu][%lu] %s",
					cy, cx, rbuf));
		}
	}
#endif

	/* no error */
	return (0);

	/* in case of unexpected EOF */
fail:
	casio_free_mcsfile(*h);
	*h = NULL;
	return (err);
}
