/* ****************************************************************************
 * link/seven_mcs/open.c -- open a protocol seven MCS interface.
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
#include "seven_mcs.h"

/* ************************************************************************* */
/*  Close callback                                                           */
/* ************************************************************************* */
/**
 *	casio_sevenmcs_close:
 *	Close a Protocol 7.00 MCS cookie.
 *
 *	@arg	cookie	the cookie.
 *	@return			the error code (0 if ok).
 */

CASIO_LOCAL int casio_sevenmcs_close(sevenmcs_t *cookie)
{
	/* XXX: should we close the link or something? */
	casio_free(cookie);
	return (0);
}

/* Callbacks. */
CASIO_LOCAL casio_mcsfuncs_t seven_mcs_funcs = {
	(casio_mcs_get_t*)&casio_sevenmcs_get,
/*	(casio_mcs_put_t*)&casio_sevenmcs_put, */ NULL,
	(casio_mcs_delete_t*)&casio_sevenmcs_delete,
	(casio_mcs_list_t*)&casio_sevenmcs_list,
	(casio_mcs_close_t*)&casio_sevenmcs_close};
/* ************************************************************************* */
/*  Main opening function                                                    */
/* ************************************************************************* */
/**
 *	casio_open_seven_mcs:
 *	Open a Protocol 7.00 MCS interface.
 *
 *	@arg	mcs		the interface to open.
 *	@arg	link	the link.
 */

int CASIO_EXPORT casio_open_seven_mcs(casio_mcs_t **mcs, casio_link_t *link)
{
	int err; sevenmcs_t *cookie = NULL;

	/* Allocate the cookie. */
	cookie = casio_alloc(1, sizeof(sevenmcs_t));
	if (!cookie) return (casio_error_alloc);
	cookie->sevenmcs_link = link;

	/* Make the main memory. */
	err = casio_open_mcs(mcs, cookie, &seven_mcs_funcs);
	if (err) return (err);

	/* We're done! */
	msg((ll_info, "The link MCS is open!"));
	return (0);
}
