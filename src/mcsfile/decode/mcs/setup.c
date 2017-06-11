/* ****************************************************************************
 * mcsfile/decode/mcs/setup.c -- decode an MCS setup file.
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
 *	casio_decode_mcs_setup:
 *	Decode settings.
 *
 *	@arg	h			the handle to make.
 *	@arg	buffer		the buffer to read from.
 *	@arg	head		the pre-filled head to complete and use.
 *	@return				the error code (0 if ok).
 */

int casio_decode_mcs_setup(casio_mcsfile_t **h, casio_stream_t *buffer,
	casio_mcshead_t *head)
{
	int err;
	unsigned char *content = NULL;
	casio_setup_t *sp;

	/* read content */
	err = casio_error_alloc;
	content = malloc(head->casio_mcshead_size);
	if (!content) goto fail;
	GREAD(content, head->casio_mcshead_size)

	/* make final head and file */
	err = casio_make_mcsfile(h, head);
	if (err) return (err);
	sp = &(*h)->casio_mcsfile_setup;

	/* get input flags */
	switch (content[0x14]) {
	case 0x84: sp->casio_setup_iflags |= casio_setupiflag_alpha;
		/* FALLTHRU */
	case 0x01: sp->casio_setup_iflags |= casio_setupiflag_shift; break;
	case 0x04: sp->casio_setup_iflags |= casio_setupiflag_alpha; break; }
	if (content[0x15] == 0x02)
		sp->casio_setup_iflags |= casio_setupiflag_insert;
	if (content[0x53] == 0x00)
		sp->casio_setup_iflags |= casio_setupiflag_math;

	/* get window flags */
	if (content[0x1C]) sp->casio_setup_wflags |= casio_setupwflag_grid;
	if (content[0x1D]) sp->casio_setup_wflags |= casio_setupwflag_axes;
	if (content[0x19]) sp->casio_setup_wflags |= casio_setupwflag_plot;

	/* TODO: decode more options! */
	/* no error! */
	err = 0;
fail:
	free(content);
	if (err) {
		casio_free_mcsfile(*h);
		*h = NULL;
	}
	return (0);
}
