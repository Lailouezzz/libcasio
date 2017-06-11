/* ****************************************************************************
 * comlist/builtin.c -- find out serial devices.
 * Copyright (C) 2016-2017 Thomas "Cakeisalie5" Touhey <thomas@touhey.fr>
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
#include "../internals.h"
#define NUM 5

/* Correspondance type. */
struct corresp {
	int              _valid;
	casio_comlist_t *_comlist;
};

/* List of com listing functions. */
static struct corresp comlists[NUM + 1] = {
#if   defined(__linux__)
	{1, casio_comlist_linux},
#elif defined(__WINDOWS__)
	{1, casio_comlist_windows},
#elif defined(__APPLE__) && defined(__MACH__)
	{1, casio_comlist_macos},
#endif
	{0, NULL}
};

/**
 *	casio_add_default_comlist:
 *	Add a default `comlist` function.
 *
 *	@arg	callback	the callback.
 *	@return				the error code (0 if ok).
 */

int casio_add_default_comlist(casio_comlist_t *function)
{
	struct corresp *c; int num;

	for (c = comlists, num = NUM; c->_valid && num; c++, num--);
	if (!num) return (casio_error_op);

	c->_valid = 1;
	c->_comlist = function;
	return (0);
}

/**
 *	casio_comlist:
 *	List the communication ports.
 *
 *	@arg	callback	the callback.
 *	@arg	cookie		the cookie.
 *	@return				the error (if any).
 */

int casio_comlist(casio_list_com_t callback, void *cookie)
{
	int err; struct corresp *c = comlists;

	if (!c->_valid)
		return (casio_error_op);
	for (; c->_valid; c++) {
		err = (*c->_comlist)(callback, cookie);

		if (!err) return (0);
		if (err != casio_error_nocalc)
			return (err);
	}

	return (casio_error_nocalc);
}
