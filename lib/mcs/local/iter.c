/* ****************************************************************************
 * mcs/local/iter.c -- iterate on heads in a local main memory.
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

typedef struct localmcs_iter_s {
	localmcs_t *mcs;
	int id;
} localmcs_iter_t;

/**
 *	next_mcsfile:
 *	Get the next element.
 *
 *	@arg	cookie		the iteration cookie.
 *	@arg	nextp		the next element to get.
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL int CASIO_EXPORT next_mcsfile(localmcs_iter_t *cookie,
	void **nextp)
{
	casio_mcsfile_t *file;

	if (cookie->id >= cookie->mcs->localmcs_count)
		return (casio_error_iter);

	file = cookie->mcs->localmcs_files[cookie->id++];
	*nextp = &file->casio_mcsfile_head;
	return (0);
}

/**
 *	end_iter:
 *	End the iterator.
 *
 *	@arg	cookie		the iteration cookie.
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL void CASIO_EXPORT end_iter(localmcs_iter_t *cookie)
{
	free(cookie);
}

/**
 *	casio_localmcs_iter:
 *	Iterate on files in a local main memory.
 *
 *	@arg	cookie		the local main memory cookie.
 *	@arg	iterp		the iterator to make.
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL casio_iter_funcs_t const casio_localmcs_iter_funcs = {
	(casio_next_t *)next_mcsfile,
	NULL,
	(casio_end_t *)end_iter
};

int CASIO_EXPORT casio_localmcs_iter(localmcs_t *cookie, casio_iter_t **iterp)
{
	localmcs_iter_t *icookie;

	icookie = casio_alloc(1, sizeof(*icookie));
	if (!cookie)
		return (casio_error_alloc);

	icookie->mcs = cookie;
	icookie->id = 0;

	return (casio_iter(iterp, icookie, &casio_localmcs_iter_funcs));
}
