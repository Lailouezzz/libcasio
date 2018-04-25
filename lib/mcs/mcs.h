/* ****************************************************************************
 * mcs/mcs.h -- libcasio RAM filesystem internals.
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
#ifndef  LOCAL_MCS_H
# define LOCAL_MCS_H 1
# include "../internals.h"

/* Here is the structure of a basic MCS filesystem.
 * It is a really simple abstraction. */

struct casio_mcs_s {
	void             *casio_mcs_cookie;
	casio_mcsfuncs_t  casio_mcs_funcs;
};

#endif /* LOCAL_MCS_H */
