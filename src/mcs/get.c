/* ****************************************************************************
 * mcs/get.c -- get an MCS file.
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
 *	casio_get_mcsfile:
 *	Get an MCS file from the main memory.
 *
 *	@arg	mcs		the main memory interface to get the file from.
 *	@arg	file	the file to get.
 *	@arg	head	the head with the identification information.
 *	@return			the error code (0 if ok).
 */

int CASIO_EXPORT casio_get_mcsfile(casio_mcs_t *mcs, casio_mcsfile_t **file,
	casio_mcshead_t *head)
{
	casio_mcs_get_t *func;

	if (!mcs) return (casio_error_invalid);
	func = mcs->casio_mcs_funcs.casio_mcsfuncs_get;
	if (!func) return (casio_error_op);
	return ((*func)(mcs->casio_mcs_cookie, file, head));
}
