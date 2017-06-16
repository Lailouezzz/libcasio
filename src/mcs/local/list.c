/* ****************************************************************************
 * mcs/local/list.c -- list files in a local main memory.
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
 *	casio_localmcs_list:
 *	List files in a local main memory.
 *
 *	@arg	cookie		the local main memory cookie.
 *	@arg	mcslist		the listing callback.
 *	@arg	mcookie		the callback cookie.
 */

int CASIO_EXPORT casio_localmcs_list(localmcs_t *cookie,
	casio_mcslist_t *mcslist, void *mcookie)
{
	int err, i, count = cookie->localmcs_count;

	for (i = 0; count && i < cookie->localmcs_size; i++) {
		casio_mcsfile_t *file = cookie->localmcs_files[i];

		/* Check that the file exists. */
		if (!file) continue;

		/* Decrement the count, so that we don't have to go to the end. */
		count--;

		/* Call back the callback. */
		err = (*mcslist)(mcookie, &file->casio_mcsfile_head);
		if (err) return (err);
	}

	/* Everything went well :) */
	return (0);
}
