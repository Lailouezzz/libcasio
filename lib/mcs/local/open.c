/* ****************************************************************************
 * mcs/local/open.c -- open a local main memory.
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

/* ************************************************************************* */
/*  Close callback, not Close Kombat                                         */
/* ************************************************************************* */
/**
 *	casio_localmcs_close:
 *	Close a local main memory.
 *
 *	@arg	cookie		the cookie.
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL int casio_localmcs_close(localmcs_t *cookie)
{
	int i, count = cookie->localmcs_count;

	/* Free the files. */
	for (i = 0; count && i < cookie->localmcs_size; i++) {
		if (!cookie->localmcs_files[i]) continue;
		casio_free_mcsfile(cookie->localmcs_files[i]);
	}

	/* Free the index and the cookie. */
	casio_free(cookie->localmcs_files); casio_free(cookie);
	return (0);
}

/* Callbacks */
CASIO_LOCAL casio_mcsfuncs_t funcs = {
	(casio_mcs_get_t*)&casio_localmcs_get,
	(casio_mcs_put_t*)&casio_localmcs_put,
	(casio_mcs_delete_t*)&casio_localmcs_delete,
	(casio_mcs_list_t*)&casio_localmcs_list,
	(casio_mcs_close_t*)&casio_localmcs_close};
/* ************************************************************************* */
/*  Main opening function                                                    */
/* ************************************************************************* */
/**
 *	casio_open_local_mcs:
 *	Open a local main memory.
 *
 *	@arg	mcs		the main memory to open.
 *	@return			the error code (0 if ok).
 */

int CASIO_EXPORT casio_open_local_mcs(casio_mcs_t **mcs)
{
	localmcs_t *cookie = NULL;

	/* Allocate the cookie. */
	cookie = casio_alloc(1, sizeof(*cookie));
	if (!cookie) return (casio_error_alloc);
	cookie->localmcs_count = 0;
	cookie->localmcs_size = 0;
	cookie->localmcs_files = NULL;

	/* Make the main memory. */
	return (casio_open_mcs(mcs, cookie, &funcs));
}
