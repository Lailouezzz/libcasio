/* ****************************************************************************
 * mcs/list.c -- list the files in a main memory.
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
 *	casio_iter_mcsfiles:
 *	Get an iterator to iterate on MCS file entries (heads).
 *
 *	@arg	mcs		the main memory interface.
 *	@arg	iter	the iterator to make.
 *	@return			the error code (0 if ok).
 */

int CASIO_EXPORT casio_iter_mcsfiles(casio_mcs_t *mcs, casio_iter_t **iterp)
{
	casio_mcs_iter_t *func;

	if (!mcs)
		return (casio_error_invalid);
	func = mcs->casio_mcs_funcs.casio_mcsfuncs_iter;
	if (!func)
		return (casio_error_op);
	return ((*func)(mcs->casio_mcs_cookie, iterp));
}

/**
 *	casio_list_mcsfiles:
 *	List MCS files on a main memory interface (deprecated function).
 *
 *	@arg	mcs		the main memory interface.
 *	@arg	list	the listing callback.
 *	@arg	cookie	the listing callback cookie.
 *	@return			the error code (0 if ok).
 */

int CASIO_EXPORT casio_list_mcsfiles(casio_mcs_t *mcs,
	casio_mcslist_t *list, void *cookie)
{
	casio_iter_t *iter;
	casio_mcshead_t *head;
	int err;

	if ((err = casio_iter_mcsfiles(mcs, &iter)))
		return (err);

	while (1) {
		if ((err = casio_next_mcshead(iter, &head))) {
			casio_end(iter);

			if (err == casio_error_stopped)
				break;
			return (err);
		}

		(*list)(cookie, head);
	}

	return (0);
}
