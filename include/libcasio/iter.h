/* ****************************************************************************
 * libcasio/iter.h -- libcasio iterators.
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
#ifndef  LIBCASIO_ITER_H
# define LIBCASIO_ITER_H 2018050901
# include "cdefs.h"

CASIO_BEGIN_NAMESPACE

/* Many things in libcasio work with iterators. This is the centralized
 * iterator interface. */

struct casio_iter_s;
typedef struct casio_iter_s casio_iter_t;

/* ---
 * Define something.
 * --- */

/* The way this is done:
 *
 * When `casio_next()` is called, if there was a previous element,
 * the `casio_nextfree_t` callback, if not NULL, will be called to free
 * it, then in all cases, the `casio_next_t` callback will be called to
 * get the next element. If the `casio_next_t` callback returns
 * `casio_error_iter`, all following calls to `casio_next()` with this
 * iterator won't call the callbacks and directly return the same error.
 *
 * When `casio_end()` is called, the `casio_end_t` callback, if not NULL,
 * will be called. */

typedef int CASIO_EXPORT casio_next_t
	OF((void *casio__cookie, void **casio__ptr));
typedef void CASIO_EXPORT casio_nextfree_t
	OF((void *casio__cookie, void *casio__ptr));
typedef void CASIO_EXPORT casio_end_t
	OF((void *casio__cookie));

typedef struct casio_iter_funcs_s {
	casio_next_t *casio_iterfunc_next;
	casio_nextfree_t *casio_iterfunc_nextfree;
	casio_end_t *casio_iterfunc_end;
} casio_iter_funcs_t;

/* ---
 * Functions.
 * --- */

CASIO_BEGIN_DECLS

/* Start an iterator with `casio_iter_<whatever>(&iter, <other data>)`,
 * with `iter` being of the `casio_iter_t *` type.
 * Get the next element through `casio_next_<whatever>(iter, &ptr)`,
 * which is usually a macro or function defined as
 * `casio_next(iter, (void **)(PTRP))`.
 * Then end the iterator using `casio_end(iter)`.
 *
 * Beware: any time you use `casio_next()` or `casio_end()`, the previous
 * element might be free'd or re-used, so if you are interested in what is
 * in it, copy the data before using one of the previous functions. */

CASIO_EXTERN int CASIO_EXPORT casio_iter
	OF((casio_iter_t **casio__iterp, void *casio__cookie,
		casio_iter_funcs_t const *casio__funcs));
CASIO_EXTERN int CASIO_EXPORT casio_next
	OF((casio_iter_t *casio__iter, void **casio__ptr));
CASIO_EXTERN void CASIO_EXPORT casio_end
	OF((casio_iter_t *casio__iter));

CASIO_END_DECLS
CASIO_END_NAMESPACE

#endif
