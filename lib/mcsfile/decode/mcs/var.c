/* ****************************************************************************
 * mcsfile/decode/mcs/var.c -- decode an MCS variable/alpha memory file.
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
#define MAX_VAR_NUMBER 30 /* if more than 30 one day, augment this… */

/**
 *	casio_decode_mcs_var:
 *	Decode alpha memory (variables).
 *
 *	@arg	handle		the handle to make.
 *	@arg	buffer		the buffer to read from.
 *	@arg	head		the pre-filled head to complete and use.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_decode_mcs_var(casio_mcsfile_t **handle,
	casio_stream_t *buffer, casio_mcshead_t *head)
{
	int err, num;
	unsigned long length = head->casio_mcshead_size;
	casio_mcsbcd_t buf[MAX_VAR_NUMBER * 2];
	const casio_mcsbcd_t *b;
	casio_mcsfile_t *h;
	int i;

	/* Get the number of elements. */

	num = length / (2 * sizeof(casio_mcsbcd_t));
	if (num > MAX_VAR_NUMBER)
		num = MAX_VAR_NUMBER;
	length = num * 2 * sizeof(casio_mcsbcd_t);

	/* Read the data. */

	READ(buf, length)

	/* Complete header */

	head->casio_mcshead_count = num;
	err = casio_make_mcsfile(handle, head);
	if (err)
		return (err);
	h = *handle;

	/* Check the count. */

	ifmsg(head->casio_mcshead_count == 1, (ll_info, "Is a single variable!"));

	/* Copy and decode the variables. */

	b = (casio_mcsbcd_t const *)buf;
	for (i = 0; i < head->casio_mcshead_count; i++) {
		casio_bcd_frommcs(&h->casio_mcsfile_vars[i].casio_mcscell_real, b++);
		casio_bcd_frommcs(&h->casio_mcsfile_vars[i].casio_mcscell_imgn, b++);
		h->casio_mcsfile_vars[i].casio_mcscell_flags = casio_mcscellflag_used;
	}

	/* No problem, woop woop. */

	return (0);
}
