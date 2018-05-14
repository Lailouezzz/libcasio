/* ****************************************************************************
 * libcasio/mcs.h -- libcasio RAM filesystem.
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
#ifndef  LIBCASIO_MCS_H
# define LIBCASIO_MCS_H
# include "cdefs.h"
# include "mcsfile.h"
# include "iter.h"
CASIO_BEGIN_NAMESPACE

/* Forward structure declarations (don't mind). */

struct         casio_mcs_s;
typedef struct casio_mcs_s      casio_mcs_t;
struct         casio_mcsfuncs_s;
typedef struct casio_mcsfuncs_s casio_mcsfuncs_t;

/* ---
 * Main Memory filesystem description.
 * --- */

/* The Main Memory, also called MCS (for Main Control Structure),
 * is a filesystem that represents all the RAM filesystems CASIO has had
 * over the years in its calculators.
 *
 * It is the filesystem where the calculator stores all of the main and little
 * informations, the biggest elements you can find in it being the programs
 * most of the time. Up to the monochrome models, this main memory, index
 * included, are of a maximum capacity of 64 KiB.
 *
 * Here are the callbacks you should set: */

typedef int CASIO_EXPORT casio_mcs_get_t
	OF((void *casio__cookie, casio_mcsfile_t **casio__mcsfile,
		casio_mcshead_t *casio__mcshead));

typedef int CASIO_EXPORT casio_mcs_put_t
	OF((void *casio__cookie, casio_mcsfile_t *casio__mcsfile));

typedef int CASIO_EXPORT casio_mcs_delete_t
	OF((void *casio__cookie, casio_mcshead_t *casio__mcshead));

typedef int CASIO_EXPORT casio_mcs_iter_t
	OF((void *casio__cookie, casio_iter_t **casio__iter));

typedef int CASIO_EXPORT casio_mcs_close_t
	OF((void *casio__cookie));

struct casio_mcsfuncs_s {
	casio_mcs_get_t    *casio_mcsfuncs_get;
	casio_mcs_put_t    *casio_mcsfuncs_put;
	casio_mcs_delete_t *casio_mcsfuncs_delete;
	casio_mcs_iter_t   *casio_mcsfuncs_iter;
	casio_mcs_close_t  *casio_mcsfuncs_close;
};

/* ---
 * Main Memory filesystem public functions.
 * --- */

CASIO_BEGIN_DECLS

/* Open and close. */

CASIO_EXTERN int CASIO_EXPORT casio_open_mcs
	OF((casio_mcs_t **casio__mcs, void *casio__cookie,
		const casio_mcsfuncs_t *casio__funcs));

CASIO_EXTERN int CASIO_EXPORT casio_close_mcs
	OF((casio_mcs_t *casio__mcs));

/* Get and put an MCS file. */

CASIO_EXTERN int CASIO_EXPORT casio_get_mcsfile
	OF((casio_mcs_t *casio__mcs, casio_mcsfile_t **casio__mcsfile,
		casio_mcshead_t *casio__mcshead));

CASIO_EXTERN int CASIO_EXPORT casio_put_mcsfile
	OF((casio_mcs_t *casio__mcs, casio_mcsfile_t *casio__mcsfile,
		int casio__change_ownership));

/* Transfer an MCS file. */

CASIO_EXTERN int CASIO_EXPORT casio_transfer_mcsfile
	OF((casio_mcs_t *casio__dest, casio_mcs_t *casio__source,
		casio_mcshead_t *casio__mcshead));

/* Delete an MCS file. */

CASIO_EXTERN int CASIO_EXPORT casio_delete_mcsfile
	OF((casio_mcs_t *casio__mcs, casio_mcshead_t *casio__mcshead));

/* List MCS files (the deprecated way). */

typedef void CASIO_EXPORT casio_mcslist_t
	OF((void *casio__cookie, const casio_mcshead_t *casio__mcshead));

CASIO_EXTERN CASIO_DEPRECATED int CASIO_EXPORT casio_list_mcsfiles
	OF((casio_mcs_t *casio__mcs, casio_mcslist_t *casio__mcslist,
		void *casio__mcookie));

/* Iterate on MCS entries. */

CASIO_EXTERN int CASIO_EXPORT casio_iter_mcsfiles
	OF((casio_mcs_t *casio__mcs, casio_iter_t **casio__iter));

# define casio_next_mcshead(ITER, MCSFILEP) \
	(casio_next((ITER), (void **)(casio_mcshead_t **)(MCSFILEP)))

/* Make a temporary main memory. */

CASIO_EXTERN int CASIO_EXPORT casio_open_local_mcs
	OF((casio_mcs_t **casio__mcs));

CASIO_END_DECLS
CASIO_END_NAMESPACE
#endif /* LIBCASIO_MCS_H */
