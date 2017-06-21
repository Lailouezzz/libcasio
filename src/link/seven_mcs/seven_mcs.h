/* ****************************************************************************
 * link/seven_mcs/seven_mcs.h -- protocol seven MCS interface internals.
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
#ifndef  LINK_SEVEN_MCS_H
# define LINK_SEVEN_MCS_H 1
# include "../usage/usage.h"

typedef struct {
	casio_link_t *sevenmcs_link;
} sevenmcs_t;

CASIO_EXTERN int CASIO_EXPORT casio_sevenmcs_get
	OF((sevenmcs_t *casio__cookie, casio_mcsfile_t **casio__mcsfile,
		casio_mcshead_t *casio__head));

CASIO_EXTERN int CASIO_EXPORT casio_sevenmcs_put
	OF((sevenmcs_t *casio__cookie, casio_mcsfile_t  *casio__mcsfile));

CASIO_EXTERN int CASIO_EXPORT casio_sevenmcs_delete
	OF((sevenmcs_t *casio__cookie, casio_mcshead_t  *casio__mcshead));

CASIO_EXTERN int CASIO_EXPORT casio_sevenmcs_list
	OF((sevenmcs_t *casio__cookie, casio_mcslist_t  *casio__mcslist,
		void *casio__mcslist_cookie));

/* Other globals. */

CASIO_EXTERN const casio_mcshead_t CASIO_EXPORT
	casio_sevenmcs_list_setup_entry;
CASIO_EXTERN const casio_mcshead_t CASIO_EXPORT
	casio_sevenmcs_list_alpha_entry;

#endif /* LINK_SEVEN_MCS_H */
