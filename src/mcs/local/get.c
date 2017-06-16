/* ****************************************************************************
 * mcs/local/get.c -- get an MCS file from a local main memory.
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
 *	casio_localmcs_get:
 *	Get from a local MCS save.
 *
 *	@arg	cookie		the local MCS cookie.
 *	@arg	mcsfile		the file to get.
 *	@arg	head		the head to look for.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_localmcs_get(localmcs_t *cookie,
	casio_mcsfile_t **mcsfile, casio_mcshead_t *head)
{
	int err; casio_mcsfile_t **pfile;

	/* Find the file. */
	err = casio_localmcs_find(cookie, &pfile, head, 0);
	if (err) return (err);

	/* Duplicate it. */
	return (casio_duplicate_mcsfile(mcsfile, *pfile));
}
