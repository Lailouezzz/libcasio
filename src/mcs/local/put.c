/* ****************************************************************************
 * mcs/local/put.c -- put an MCS file into a local main memory.
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
 *	casio_localmcs_put:
 *	Put a main memory file to a local main memory.
 *
 *	@arg	cookie		the local main memory cookie.
 *	@arg	mcsfile		the main memory file to put.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_localmcs_put(localmcs_t *cookie,
	casio_mcsfile_t *mcsfile)
{
	int err; casio_mcsfile_t **pfile = NULL;

	/* Find the entry. */
	err = casio_localmcs_find(cookie, &pfile, &mcsfile->casio_mcsfile_head, 1);
	if (err) { casio_free_mcsfile(mcsfile); return (err); }

	/* We have found the precious thing! */
	if (*pfile) casio_free_mcsfile(*pfile);
	*pfile = mcsfile;
	return (0);
}
