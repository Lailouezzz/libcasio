/* ****************************************************************************
 * mcsfile/manage.c -- create, free an MCS file handle.
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
 *	casio_make_mcsfile:
 *	Make an MCS file out of a head.
 *
 *	Will allocate all of the required parts of the MCS file for it to be
 *	filled later.
 *
 *	@arg	handle		the file to allocate.
 *	@arg	rawhead		the head to use.
 *	@return				the error (if any).
 */

int casio_make_mcsfile(casio_mcsfile_t **h, const casio_mcshead_t *rawhead)
{
	casio_mcsfile_t *handle;
	casio_mcshead_t *head;
	unsigned int y, wd, ht;
	int i, j;

	/* allocate the handle */
	*h = malloc(sizeof(casio_mcsfile_t)); handle = *h;
	if (!handle) return (casio_error_alloc);

	/* initialize the handle, copy the head */
	memset(h, 0, sizeof(casio_mcsfile_t));
	memcpy(&handle->casio_mcsfile_head, rawhead, sizeof(casio_mcshead_t));
	head = &handle->casio_mcsfile_head;

	switch (head->casio_mcshead_type) {
	/* allocate the cells */
	case casio_mcstype_list: case casio_mcstype_mat:
	case casio_mcstype_vct: case casio_mcstype_ssheet:
		wd = head->casio_mcshead_width;
		ht = head->casio_mcshead_height;
		msg((ll_info, "Preparing %d*%d matrix", wd, ht));

		if (wd && ht) {
			handle->casio_mcsfile_cells =
				malloc(sizeof(casio_mcscell_t*) * ht);
			if (!handle->casio_mcsfile_cells) goto fail;
			handle->casio_mcsfile_cells[0] =
				malloc(sizeof(casio_mcscell_t) * wd * ht);
			if (!handle->casio_mcsfile_cells[0]) {
				free(handle->casio_mcsfile_cells); goto fail; }

			for (y = 1; y < ht; y++)
				handle->casio_mcsfile_cells[y] = &handle->casio_mcsfile_cells
					[0][handle->casio_mcsfile_head.casio_mcshead_width * y];
		}
		break;

	/* allocate the variables */
	case casio_mcstype_alphamem:
		if (head->casio_mcshead_count <= 1)
			handle->casio_mcsfile_vars = &handle->casio_mcsfile_var;
		else {
			handle->casio_mcsfile_vars = malloc(sizeof(casio_mcscell_t)
				* head->casio_mcshead_count);
			if (!handle->casio_mcsfile_vars) goto fail;
			memset(handle->casio_mcsfile_vars, 0, sizeof(casio_mcscell_t)
				* head->casio_mcshead_count);
		}
		break;

	/* allocate the set of pixels */
	case casio_mcstype_pict: case casio_mcstype_capt:
		/* set count */
		head->casio_mcshead_count =
			(head->casio_mcshead_type == casio_mcstype_pict) ? 2 : 1;

		/* allocate directory */
		if (head->casio_mcshead_count <= 1)
			handle->casio_mcsfile_pics = &handle->casio_mcsfile_pic;
		else {
			handle->casio_mcsfile_pics = malloc(sizeof(casio_pixel_t**)
				* head->casio_mcshead_count);
			if (!handle->casio_mcsfile_pics) goto fail;
		}

		/* allocate */
		for (i = 0; i < head->casio_mcshead_count; i++) {
			handle->casio_mcsfile_pics[i] = alloc_pixels(
				head->casio_mcshead_width, head->casio_mcshead_height);
			if (!handle->casio_mcsfile_pics[i]) {
				for (j = 0; j < i; j++)
					free(handle->casio_mcsfile_pics[j]);
				if (handle->casio_mcsfile_pics != &handle->casio_mcsfile_pic)
					free(handle->casio_mcsfile_pics);
				goto fail;
			}
		}

		/* prepare */
		for (i = 0; i < head->casio_mcshead_count; i++)
			prepare_pixels(handle->casio_mcsfile_pics[i],
				head->casio_mcshead_width, head->casio_mcshead_height)
		break;

	/* allocate nothing */
	case casio_mcstype_end:
	case casio_mcstype_setup:
	case casio_mcstype_string: /* TEMPORARY XXX */
		break;

	/* allocate raw content */
	default:
		handle->casio_mcsfile_content = malloc(head->casio_mcshead_size);
		if (!handle->casio_mcsfile_content) goto fail;
		break;
	}

	/* finish */
	return (0);
fail:
	free(*h); *h = NULL;
	return (casio_error_alloc);
}

/**
 *	casio_free_mcsfile:
 *	Free an MCS file handle content.
 *
 *	@arg	handle		the handle which has the content to free.
 */

void casio_free_mcsfile(casio_mcsfile_t *handle)
{
	int i;

	/* check if exists */
	if (!handle) return ;

	switch (handle->casio_mcsfile_head.casio_mcshead_type) {
	/* free the cells */
	case casio_mcstype_mat: case casio_mcstype_vct:
	case casio_mcstype_list: case casio_mcstype_ssheet:
		if (handle->casio_mcsfile_head.casio_mcshead_width
		 && handle->casio_mcsfile_head.casio_mcshead_height) {
			free(handle->casio_mcsfile_cells[0]);
			free(handle->casio_mcsfile_cells);
		}
		break;

	/* free the set of pixels */
	case casio_mcstype_pict: case casio_mcstype_capt:
		for (i = 0; i < handle->casio_mcsfile_head.casio_mcshead_count; i++)
			free(handle->casio_mcsfile_pics[i]);
		if (handle->casio_mcsfile_pics != &handle->casio_mcsfile_pic)
			free(handle->casio_mcsfile_pics);
		break;

	/* free the variables */
	case casio_mcstype_alphamem:
		if (handle->casio_mcsfile_vars != &handle->casio_mcsfile_var)
			free(handle->casio_mcsfile_vars);
		break;

	/* free nothing */
	case casio_mcstype_end:
	case casio_mcstype_setup:
	case casio_mcstype_string: /* TEMPORARY XXX */
		break;

	/* free the raw content */
	default:
		free(handle->casio_mcsfile_content);
	}

	/* free the content */
	free(handle);
}
