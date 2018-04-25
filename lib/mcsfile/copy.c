/* ****************************************************************************
 * mcsfile/copy.c -- copy an MCS file handle.
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
#include "mcsfile.h"

/**
 *	casio_copy_mcsfile:
 *	Copy an MCS file.
 *
 *	@arg	handle	the destination file.
 *	@arg	orig	the source file.
 *	@return			the error code (0 if ok).
 */

int CASIO_EXPORT casio_copy_mcsfile(casio_mcsfile_t *handle,
	casio_mcsfile_t *orig)
{
	int err; casio_mcshead_t *head;
	int n; unsigned int y, wd, ht;

	/* Prepare the new file. */
	err = casio_prepare_mcsfile(handle, &orig->casio_mcsfile_head);
	if (err) return (err);
	head = &handle->casio_mcsfile_head;

	/* Copy the data. */
	switch (head->casio_mcshead_type) {
	case casio_mcstype_list: case casio_mcstype_mat:
	case casio_mcstype_vct:  case casio_mcstype_ssheet:
		wd = head->casio_mcshead_width;
		ht = head->casio_mcshead_height;
		msg((ll_info, "Copying %d*%d matrix", wd, ht));

		for (y = 0; y < ht; y++) {
			memcpy(handle->casio_mcsfile_cells[y],
				orig->casio_mcsfile_cells[y],
				wd * sizeof(casio_mcscell_t));
		}
		break;

	case casio_mcstype_alphamem:
		memcpy(handle->casio_mcsfile_vars, orig->casio_mcsfile_vars,
			sizeof(casio_mcscell_t) * head->casio_mcshead_count);
		break;

	case casio_mcstype_pict: case casio_mcstype_capt:
		for (n = 0; n < head->casio_mcshead_count; n++)
		  for (y = 0; y < head->casio_mcshead_height; y++) {
			memcpy(handle->casio_mcsfile_pics[n][y],
				orig->casio_mcsfile_pics[n][y],
				sizeof(casio_pixel_t) * head->casio_mcshead_width);
		}
		break;

	case casio_mcstype_setup:
		memcpy(&handle->casio_mcsfile_setup, &orig->casio_mcsfile_setup,
			sizeof(casio_setup_t));
		break;

	case casio_mcstype_end: case casio_mcstype_string: break;

	default:
		memcpy(handle->casio_mcsfile_content, orig->casio_mcsfile_content,
			head->casio_mcshead_size);
		break;
	}

	/* No error! */
	return (0);
}
