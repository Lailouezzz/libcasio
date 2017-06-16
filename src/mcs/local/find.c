/* ****************************************************************************
 * mcs/local/find.c -- find an MCS file or free offset in a local main mem.
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
#include "local.h"

/**
 *	casio_localmcs_find:
 *	Find a specific or empty MCS entry in a local main memory.
 *
 *	@arg	cookie		the local main memory cookie.
 *	@arg	ppfile		the entry to give.
 *	@arg	head		the MCS head.
 *	@arg	find_free	if we should get an empty offset if we don't suceed.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_localmcs_find(localmcs_t *cookie,
	casio_mcsfile_t ***ppfile, casio_mcshead_t *mcshead, int find_free)
{
	casio_mcsfile_t **pfile;
	int i, count = cookie->localmcs_count;

	pfile = NULL;
	for (i = 0; i < cookie->localmcs_size && count; i++) {
		casio_mcsfile_t **cpfile = &cookie->localmcs_files[i];
		casio_mcsfile_t    *file = *cpfile;

		/* If the file doesn't exist, we'll take that slot. */
		if (!file) { pfile = cpfile; continue; }

		/* There's one less! */
		count--;

		/* Check that the files matches. */
		if (!casio_match_mcsfiles(&file->casio_mcsfile_head, mcshead))
			continue;

		/* If the file does exist and matches with the current one,
		 * we WANT to take its place. */
		pfile = cpfile;
		break;
	}

	/* Check if we just want to find the entry. */
	if (!find_free && (!pfile || !*pfile))
		return (casio_error_notfound);

	/* If no empty or appropriate space was found, take one at the end
	 * (allocate if needed). */
	if (!pfile) {
		if (i == cookie->localmcs_size) {
			int newsize = cookie->localmcs_size + LOCALMCS_CHUNK_SIZE;
			casio_mcsfile_t **newindex = NULL;

			/* Allocate the new index. */
			newindex = malloc(newsize * sizeof(casio_mcsfile_t*));
			if (!newindex) return (casio_error_alloc);

			/* Copy the old index data. */
			memcpy(newindex, cookie->localmcs_files,
				cookie->localmcs_size * sizeof(casio_mcsfile_t*));

			/* Initialize the new entries.
			 * (XXX: this shouldn't be necessary, should we remove it?) */
			memset(&newindex[cookie->localmcs_size], 0,
				(newsize - cookie->localmcs_size) * sizeof(casio_mcsfile_t*));

			/* Free the old index, assign the new one. */
			free(cookie->localmcs_files);
			cookie->localmcs_files = newindex;
			cookie->localmcs_size  = newsize;
		}

		/* Take the (new?) space, increment the file count. */
		pfile = &cookie->localmcs_files[i];
		*pfile = NULL; /* important: the entry is possibly not initialized! */
		cookie->localmcs_count++;
	}

	*ppfile = pfile;
	return (0);
}
