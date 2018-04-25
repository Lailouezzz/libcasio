/* ****************************************************************************
 * mcsfile/free.c -- free an MCS file handle.
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
 *	casio_free_mcsfile:
 *	Free an MCS file handle content.
 *
 *	@arg	handle		the main memory file handle to free.
 */

void CASIO_EXPORT casio_free_mcsfile(casio_mcsfile_t *handle)
{
	int i;

	/* check if exists */
	if (!handle) return ;

	switch (handle->casio_mcsfile_head.casio_mcshead_type) {
	/* free the cells */
	case casio_mcstype_mat:  case casio_mcstype_vct:
	case casio_mcstype_list: case casio_mcstype_ssheet:
		if (handle->casio_mcsfile_head.casio_mcshead_width
		 && handle->casio_mcsfile_head.casio_mcshead_height) {
			casio_free(handle->casio_mcsfile_cells[0]);
			casio_free(handle->casio_mcsfile_cells);
		}
		break;

	/* free the set of pixels */
	case casio_mcstype_pict: case casio_mcstype_capt:
		for (i = 0; i < handle->casio_mcsfile_head.casio_mcshead_count; i++)
			casio_free(handle->casio_mcsfile_pics[i]);
		if (handle->casio_mcsfile_pics != &handle->casio_mcsfile_pic)
			casio_free(handle->casio_mcsfile_pics);
		break;

	/* free the variables */
	case casio_mcstype_alphamem:
		if (handle->casio_mcsfile_vars != &handle->casio_mcsfile_var)
			casio_free(handle->casio_mcsfile_vars);
		break;

	/* free nothing */
	case casio_mcstype_end:
	case casio_mcstype_setup:
	case casio_mcstype_string: /* TEMPORARY XXX */
		break;

	/* free the raw content */
	default:
		casio_free(handle->casio_mcsfile_content);
	}

	/* free the content */
	handle->casio_mcsfile_head.casio_mcshead_flags &= ~casio_mcsflag_valid;
	if (handle->casio_mcsfile_head.casio_mcshead_flags & casio_mcsflag_alloc)
		casio_free(handle);
}
