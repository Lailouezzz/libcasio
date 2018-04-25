/* ****************************************************************************
 * stream.h -- stream internals.
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
#ifndef  LOCAL_STREAM_H
# define LOCAL_STREAM_H 1
# include "../internals.h"
# define failure(CASIO__COND, CASIO__ERR) \
	if (CASIO__COND) { err = CASIO__ERR; goto fail; }

/* Pretty much everything about this structure is discussed
 * in `libcasio/stream.h`. */

struct casio_stream_s {
	/* stream information */
	casio_openmode_t         casio_stream_mode;
	void                    *casio_stream_cookie;
	int                      casio_stream_lasterr;

	/* private properties (do not modify these!) */
	casio_off_t              casio_stream_offset;
	casio_streamattrs_t      casio_stream_attrs;
	casio_timeouts_t         casio_stream_timeouts;

	/* callbacks */
	casio_streamfuncs_t      casio_stream_callbacks;
};

/* Get a callback. */
#define getcb(CASIO__S, CASIO__NAME) \
	((CASIO__S)->casio_stream_callbacks.casio_streamfuncs_ ## CASIO__NAME)

#endif
