/* ****************************************************************************
 * mcs/local/delete.c -- delete an MCS file in a local main memory.
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
 *	casio_localmcs_delete:
 *	Delete a file from a local main memory.
 *
 *	@arg	cookie		the local main memory interface.
 *	@arg	head		the head of the file to delete.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_localmcs_delete(localmcs_t *cookie,
	casio_mcshead_t *head)
{
	int err; casio_mcsfile_t **pfile;

	/* Find the entry. */
	err = casio_localmcs_find(cookie, &pfile, head, 0);
	if (err) return (err);

	/* Delete it. */
	casio_free_mcsfile(*pfile);
	*pfile = NULL;
	cookie->localmcs_count--;
	return (0);
}
