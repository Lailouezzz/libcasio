/* ****************************************************************************
 * mcs/delete.c -- delete an MCS file.
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
 *	casio_delete_mcsfile:
 *	Delete an MCS file from a main memory.
 *
 *	@arg	mcs		the main memory interface.
 *	@arg	head	the identification data of the file to delete.
 *	@return			the error code (0 if ok).
 */

int CASIO_EXPORT casio_delete_mcsfile(casio_mcs_t *mcs,
	casio_mcshead_t *head)
{
	casio_mcs_delete_t *func;

	if (!mcs) return (casio_error_invalid);
	func = mcs->casio_mcs_funcs.casio_mcsfuncs_delete;
	if (!func) return (casio_error_op);
	return ((*func)(mcs->casio_mcs_cookie, head));
}
