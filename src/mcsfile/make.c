/* ****************************************************************************
 * mcsfile/make.c -- make an MCS file handle.
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
#include "mcsfile.h"

/**
 *	casio_make_mcsfile:
 *	Make an MCS file out of a head.
 *
 *	@arg	h		the file to allocate.
 *	@arg	rawhead	the head to use.
 *	@return			the error code (0 if ok).
 */

int CASIO_EXPORT casio_make_mcsfile(casio_mcsfile_t **h,
	const casio_mcshead_t *rawhead)
{
	int err; casio_mcsfile_t *handle;

	/* Allocate the handle. */
	*h = malloc(sizeof(casio_mcsfile_t)); handle = *h;
	if (!handle) return (casio_error_alloc);
	handle->casio_mcsfile_head.casio_mcshead_flags = 0; /* & ~mcsflag_valid */

	/* Prepare it. */
	err = casio_prepare_mcsfile(handle, rawhead);
	if (err) { free(handle); return (err); }

	/* Add the flag and return. */
	handle->casio_mcsfile_head.casio_mcshead_flags |= casio_mcsflag_alloc;
	return (0);
}
