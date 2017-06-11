/* ****************************************************************************
 * file/mcs.c -- manage an MCS archive.
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
#include "file.h"
#define MCS_CHUNK_SIZE 16

/* ************************************************************************* */
/*  General function for inserting files                                     */
/* ************************************************************************* */
/**
 *	casio_mcs_insert:
 *	Find space for a file and allocate.
 *
 *	@arg	handle		the handle.
 *	@arg	tofile		the user file pointer, to set.
 *	@arg	head		the head.
 *	@return				the mcs file (NULL if allocation error).
 */

int casio_mcs_insert(casio_file_t *handle, casio_mcsfile_t **tofile,
	const casio_mcshead_t *head)
{
	casio_mcsfile_t **pfile = NULL;
	int i, err, istyp;

	*tofile = NULL;
	/* look if this file isn't already in the tab */
	istyp = casio_mcshead_uses_id(head);
	for (i = 0; i < handle->casio_file_count; i++) {
		casio_mcsfile_t *file = handle->casio_file_mcsfiles[i];
		if (!file) {
			pfile = &handle->casio_file_mcsfiles[i];
			continue;
		}

		/* loop assertions */
		if (file->casio_mcsfile_head.casio_mcshead_type
		 != head->casio_mcshead_type)
			continue;
		if (head->casio_mcshead_type) {
			if (istyp) {
				if (head->casio_mcshead_id
				 != file->casio_mcsfile_head.casio_mcshead_id)
					continue;
			} else if (strcmp(file->casio_mcsfile_head.casio_mcshead_name,
			  head->casio_mcshead_name))
				continue;
		} else switch (head->casio_mcshead_info) {
			case casio_mcsinfo_mcs:
				if (strncmp((char*)head->casio_mcshead_group,
				  (char*)file->casio_mcsfile_head.casio_mcshead_group, 16))
					continue;
				if (strncmp((char*)head->casio_mcshead_dirname,
				  (char*)file->casio_mcsfile_head.casio_mcshead_dirname, 8))
					continue;
				if (strncmp((char*)head->casio_mcshead_name,
				  (char*)file->casio_mcsfile_head.casio_mcshead_name, 8))
					continue;
				break;
			case casio_mcsinfo_cas:
			case casio_mcsinfo_caspro:
				if (strncmp((char*)head->casio_mcshead_datatype,
				  (char*)file->casio_mcsfile_head.casio_mcshead_datatype, 2))
					continue;
				break;
			default: continue;
		}

		/* slot was found! */
		casio_free_mcsfile(file);
		pfile = &handle->casio_file_mcsfiles[i];
		goto found;
	}

	/* if no empty space was found, take one at the end (allocate if needed) */
	if (!pfile) {
		if (handle->casio_file_count >= handle->casio_file__size) {
			/* allocate new index */
			int newsize = handle->casio_file_count + MCS_CHUNK_SIZE;
			casio_mcsfile_t **newindex =
				malloc(newsize * sizeof(casio_mcsfile_t*));
			if (!newindex) return (casio_error_alloc);

			/* copy the data from the old index */
			memcpy(newindex, handle->casio_file_mcsfiles,
				handle->casio_file__size * sizeof(casio_mcsfile_t*));
			memset(&newindex[handle->casio_file__size], 0,
				(newsize - handle->casio_file__size)
				* sizeof(casio_mcsfile_t*));

			/* free old, assign new */
			free(handle->casio_file_mcsfiles);
			handle->casio_file_mcsfiles = newindex;
			handle->casio_file__size = newsize;
		}
		pfile = &handle->casio_file_mcsfiles[handle->casio_file_count++];
	}

found:;
	/* make the file */
	err = casio_make_mcsfile(pfile, head);
	if (err) return (err);

	/* return it */
	*tofile = *pfile;
	return (0);
}
/* ************************************************************************* */
/*  Reorganize an MCS handle for encoding                                    */
/* ************************************************************************* */
/**
 *	casio_mcs_sort:
 *	Sort an MCS archive.
 *
 *	@arg	handle		the MCS archive handle.
 *	@return				the error code (0 if ok).
 */

typedef int casio_qsort_callback OF((const void*, const void*));
int casio_mcs_sort(casio_file_t *handle)
{
	int err, i;

	/* make checks */
	if (handle->casio_file_type != casio_filetype_mcs)
		return (casio_error_op);
	if (handle->casio_file_for  != casio_filefor_fx)
		return (casio_error_op);

	/* correct each head */
	for (i = 0; i < handle->casio_file_count; i++) {
		err = casio_correct_mcsfile_head(
			&handle->casio_file_mcsfiles[i]->casio_mcsfile_head);
		if (err) return (err);
	}

	/* sort */
	qsort(handle->casio_file_mcsfiles, handle->casio_file_count,
		sizeof(casio_mcsfile_t*),
		(casio_qsort_callback*)&casio_compare_mcsfiles);

	/* TODO: check for duplicates */
	return (0);
}
