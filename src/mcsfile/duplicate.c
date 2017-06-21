/* ****************************************************************************
 * mcsfile/duplicate.c -- duplicate an MCS file handle.
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
 *	casio_duplicate_mcsfile:
 *	Copy an MCS file.
 *
 *	@arg	h			the file to allocate.
 *	@arg	orig		the original file.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_duplicate_mcsfile(casio_mcsfile_t **h,
	casio_mcsfile_t *orig)
{
	int err; casio_mcsfile_t *handle = NULL;

	/* Make the handle. */
	*h = casio_alloc(1, sizeof(casio_mcsfile_t)); handle = *h;
	if (!handle) return (casio_error_alloc);

	/* Copy the file. */
	err = casio_copy_mcsfile(handle, orig);
	if (err) { casio_free(handle); return (err); }

	/* Set the 'alloc' flag and return. */
	handle->casio_mcsfile_head.casio_mcshead_flags |= casio_mcsflag_alloc;
	return (0);
}
