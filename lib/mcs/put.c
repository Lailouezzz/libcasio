/* ****************************************************************************
 * mcs/put.c -- put an MCS file.
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
 *	casio_put_mcsfile:
 *	Put an MCS file to the main memory.
 *
 *	@arg	mcs				the main memory interface to interact with.
 *	@arg	file			the file to put.
 *	@arg	chg_ownership	should we copy or take control over the file?
 *	@return					the error code (0 if ok).
 */

int CASIO_EXPORT casio_put_mcsfile(casio_mcs_t *mcs, casio_mcsfile_t *file,
	int chg_ownership)
{
	int err; casio_mcs_put_t *func;
	casio_mcsfile_t *ourfile = NULL;

	/* Get the function. */
	if (!mcs) return (casio_error_invalid);
	func = mcs->casio_mcs_funcs.casio_mcsfuncs_put;
	if (!func) return (casio_error_invalid);

	/* Check if we should duplicate the file. */
	if (chg_ownership) ourfile = file;
	else {
		err = casio_duplicate_mcsfile(&ourfile, file);
		if (err) return (err);
	}

	/* Put the file. */
	return ((*func)(mcs->casio_mcs_cookie, ourfile));
}
