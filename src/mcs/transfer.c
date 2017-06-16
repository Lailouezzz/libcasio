/* ****************************************************************************
 * mcs/transfer.c -- transfer a file from one main memory to another.
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
 *	casio_transfer_mcsfile:
 *	Transfer a file from one main memory to another.
 *
 *	Doesn't actually require the MCS internals, which means the user
 *	could reproduce this function, but we're here to ease the lib usage :)
 *
 *	@arg	dest	the destination main memory.
 *	@arg	source	the source main memory.
 *	@arg	head	the head to require.
 *	@return			the error code (0 if ok).
 */

int CASIO_EXPORT casio_transfer_mcsfile(casio_mcs_t *dest, casio_mcs_t *source,
	casio_mcshead_t *head)
{
	int err; casio_mcsfile_t *handle = NULL;

	/* Get the file. */
	err = casio_get_mcsfile(source, &handle, head);
	if (err) return (err);

	/* Put the file on the destination. */
	return (casio_put_mcsfile(dest, handle, 1));
}
