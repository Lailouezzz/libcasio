/* ****************************************************************************
 * libcasio/mcs/local.h -- open a local main memory.
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
#ifndef  LOCAL_MCS_LOCAL_H
# define LOCAL_MCS_LOCAL_H 1
# include "../../internals.h"
# define LOCALMCS_CHUNK_SIZE 16

/* Cookie definition. */
typedef struct {
	int               localmcs_count; /* number of MCS files actually set */
	int               localmcs_size;  /* the size of the array */
	casio_mcsfile_t **localmcs_files; /* the file pointer list */
} localmcs_t;

/* Internal function to get a file pointer. */

CASIO_EXTERN int CASIO_EXPORT casio_localmcs_find
	OF((localmcs_t *casio__cookie, casio_mcsfile_t ***casio__pfile,
		casio_mcshead_t *casio__mcshead, int casio__find_free));

/* MCS ballcracks. */

CASIO_EXTERN int CASIO_EXPORT casio_localmcs_get
	OF((localmcs_t *casio__cookie, casio_mcsfile_t **casio__mcsfile,
		casio_mcshead_t *casio__head));

CASIO_EXTERN int CASIO_EXPORT casio_localmcs_put
	OF((localmcs_t *casio__cookie, casio_mcsfile_t *casio__mcsfile));

CASIO_EXTERN int CASIO_EXPORT casio_localmcs_delete
	OF((localmcs_t *casio__cookie, casio_mcshead_t *casio__mcshead));

CASIO_EXTERN int CASIO_EXPORT casio_localmcs_list
	OF((localmcs_t *casio__cookie, casio_mcslist_t *casio__mcslist,
		void *casio__mcslist_cookie));

#endif /* LOCAL_MCS_LOCAL_H */
