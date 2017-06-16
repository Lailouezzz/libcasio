/* ****************************************************************************
 * mcs/open.c -- open a virtual main memory.
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
#include "mcs.h"

/**
 *	casio_open_mcs:
 *	Open a main memory interface.
 *
 *	@arg	m		the main memory to create.
 *	@arg	cookie	the main memory cookie.
 *	@arg	funcs	the functions.
 *	@return			the error code (0 if ok).
 */

int CASIO_EXPORT casio_open_mcs(casio_mcs_t **m, void *cookie,
	const casio_mcsfuncs_t *funcs)
{
	int err; casio_mcs_t *mcs = NULL;

	/* Allocate the filesystem. */
	*m = malloc(sizeof(casio_mcs_t)); mcs = *m;
	if (!mcs) { err = casio_error_alloc; goto fail; }

	/* Set it up and return it. */
	mcs->casio_mcs_cookie = cookie;
	memcpy(&mcs->casio_mcs_funcs, funcs, sizeof(*funcs));
	return (0);
fail:
	(*funcs->casio_mcsfuncs_close)(cookie);
	return (err);
}

/**
 *	casio_close_mcs:
 *	Close a main memory interface.
 *
 *	@arg	mcs		the main memory to close.
 *	@return			the error (0 if ok).
 */

int CASIO_EXPORT casio_close_mcs(casio_mcs_t *mcs)
{
	int err; casio_mcs_close_t *func;

	if (!mcs || !(func = mcs->casio_mcs_funcs.casio_mcsfuncs_close))
		return (0);
	err = (*func)(mcs->casio_mcs_cookie);
	free(mcs);
	return (err);
}
