/* ****************************************************************************
 * utils/iter.c -- iterator internals.
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
#include <libcasio.h>

/* Internal structure of an iterator. */

struct casio_iter_s {
	int stopped, has_last;
	void *last;
	void *cookie;
	casio_next_t *next;
	casio_nextfree_t *nextfree;
	casio_end_t *end;
};

/* `default_next()`: default next function. */

CASIO_LOCAL int default_next(void *cookie, void **ptr)
{
	(void)cookie;
	(void)ptr;
	return (casio_error_iter);
}

/* `default_nextfree()`: default nextfree function. */

CASIO_LOCAL void default_nextfree(void *cookie, void *ptr)
{
	/* Do not free. */

	(void)cookie;
	(void)ptr;
}

/* `default_end()`: default iterator end function. */

CASIO_LOCAL void default_end(void *cookie)
{
	(void)cookie;
}

/* `casio_iter()`: create an iterator. */

int CASIO_EXPORT casio_iter(casio_iter_t **iterp, void *cookie,
	casio_iter_funcs_t const *funcs)
{
	casio_iter_t *iter;

	/* Allocate the iterator. */

	iter = casio_alloc(1, sizeof(*iter));
	if (!iter) {
		if (funcs->casio_iterfunc_end)
			(*funcs->casio_iterfunc_end)(cookie);
		return (casio_error_alloc);
	}

	/* Prepare the iterator. */

	iter->stopped = 0;
	iter->has_last = 0;
	iter->cookie = cookie;
	iter->next = funcs->casio_iterfunc_next;
	if (!iter->next)
		iter->next = &default_next;
	iter->nextfree = funcs->casio_iterfunc_nextfree;
	if (!iter->nextfree)
		iter->nextfree = &default_nextfree;
	iter->end = funcs->casio_iterfunc_end;
	if (!iter->end)
		iter->end = &default_end;

	*iterp = iter;
	return (0);
}

/* `casio_next()`: get the next element from an iterator. */

int CASIO_EXPORT casio_next(casio_iter_t *iter, void **ptrp)
{
	int err;

	if (iter->stopped)
		return (casio_error_iter);
	if (iter->has_last) {
		(*iter->nextfree)(iter->cookie, iter->last);
		iter->has_last = 0;
	}

	err = (*iter->next)(iter->cookie, ptrp);
	if (err == casio_error_iter) {
		iter->stopped = 1;
		return (err);
	} else if (err)
		return (err);

	iter->has_last = 1;
	iter->last = *ptrp;

	return (0);
}

/* `casio_end()`: end and free the iterator. */

void CASIO_EXPORT casio_end(casio_iter_t *iter)
{
	if (iter->has_last) {
		(*iter->nextfree)(iter->cookie, iter->last);
		iter->has_last = 0;
	}

	(*iter->end)(iter->cookie);
	casio_free(iter);
}
