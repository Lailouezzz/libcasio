/* ****************************************************************************
 * mcsfile/mcsfile.h -- MCS file internals.
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
#ifndef  LOCAL_MCSFILE_H
# define LOCAL_MCSFILE_H 1
# include "../internals.h"

/* ---
 * Macros for interacting with the buffer.
 * --- */

/* Read from a stream. */

# define  READ(CASIO__TO, CASIO__SZ) /* normal read */ { \
	ssize_t READ_err = casio_read(buffer, (CASIO__TO), (CASIO__SZ)); \
	if (READ_err < 0) return -(READ_err); }
# define FREAD(CASIO__TO, CASIO__SZ) /* fail-less read */ { \
	ssize_t READ_err = casio_read(buffer, (CASIO__TO), (CASIO__SZ)); \
	err = READ_err < 0 ? -READ_err : 0; }
# define GREAD(CASIO__TO, CASIO__SZ) /* read with goto fail */ { \
	ssize_t READ_err = casio_read(buffer, (CASIO__TO), (CASIO__SZ)); \
	err = READ_err < 0 ? -READ_err : 0; \
	if ((err)) \
		goto fail; }

/* Read using size of the object. */

# define  DREAD(CASIO__TO) \
	READ(&CASIO__TO, sizeof(CASIO__TO))
# define GDREAD(CASIO__TO) \
	GREAD(&CASIO__TO, sizeof(CASIO__TO))

/* Skip. */

# define  SKIP(CASIO__SZ) { \
	int SKIP_err = casio_skip(buffer, CASIO__SZ); \
	if (SKIP_err) return (SKIP_err); }
# define GSKIP(CASIO__SZ) { \
	err = casio_skip(buffer, CASIO__SZ); \
	if (err) goto fail; }

/* Write. */

# define  WRITE(CASIO__BUF, CASIO__SZ) { \
	ssize_t WRITE_ssize = casio_write(buffer, (CASIO__BUF), (CASIO__SZ)); \
	int WRITE_err = WRITE_ssize < 0 ? -WRITE_ssize : 0; \
	if (WRITE_err) return (WRITE_err); }
# define DWRITE(CASIO__OBJECT) \
	WRITE(&(CASIO__OBJECT), sizeof(CASIO__OBJECT))

/* ---
 * Picture utilities.
 * --- */

# define alloc_pixels(W, H) \
	casio_alloc(sizeof(casio_pixel_t*) \
		* (H) + sizeof(casio_pixel_t) * (W) * (H), 1)
# define prepare_pixels(I, W, H) { \
	unsigned int PIXPREP_y; \
	casio_pixel_t *PIXPREP_line = (casio_pixel_t*)&(I)[(H)]; \
	for (PIXPREP_y = 0; PIXPREP_y < (H); PIXPREP_y++) { \
		(I)[PIXPREP_y] = PIXPREP_line; \
		PIXPREP_line += (W); \
	}}

#endif
