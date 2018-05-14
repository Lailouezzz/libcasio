/* ****************************************************************************
 * link/usage/usage.h -- internal header for link usage.
 * Copyright (C) 2016-2017 Thomas "Cakeisalie5" Touhey <thomas@touhey.fr>
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
#ifndef  LOCAL_LINK_USAGE_H
# define LOCAL_LINK_USAGE_H 1
# include "../link.h"

/* ---
 * Quick-checking macros.
 * --- */

/* Check that the handle exists. */

# define chk_handle_exists(CASIO__H) \
	if (!(CASIO__H)) return (casio_error_init);

/* Check that the handle exists and is usable. */

# define chk_handle(CASIO__H) \
	chk_handle_exists(CASIO__H) \
	if ((CASIO__H)->casio_link_flags & casio_linkflag_ended) \
		return (casio_error_init);

/* Check that the protocol we're using is for Protocol Seven. */

# define chk_seven(CASIO__H) /* TODO */

/* Check that the handle is active. */

# define chk_active(CASIO__H) \
	if (~(CASIO__H)->casio_link_flags & casio_linkflag_active) \
		return (casio_error_active);

/* Check that the handle is passive. */

# define chk_passive(CASIO__H) \
	if ((CASIO__H)->casio_link_flags & casio_linkflag_active) \
		return (casio_error_active);

/* Check that the file name is correct. */

# define chk_filename(CASIO__FN) /* TODO */

/* Check that the filename is there and ok. */

# define chk_required_filename(CASIO__FN) \
	if (!(CASIO__FN)) return (casio_error_invalid);

/* Check that the directory name is ok. */

# define chk_dirname(CASIO__DN) /* TODO */

/* Check that the MCS request head is ok. */

# define chk_head(CASIO__HD) \
	casio_correct_mcsfile_head(CASIO__HD); \
	if (!(CASIO__HD)->casio_mcshead_name[0]) return (casio_error_invalid); \
	if (!(CASIO__HD)->casio_mcshead_group[0]) return (casio_error_invalid);

/* Check if the buffer is readable. */

# define chk_bufread(CASIO__BUF) /* TODO */

/* Check if the buffer is writable. */

# define chk_bufwrite(CASIO__BUF) /* TODO */

/* Check that the memory buffer is valid and not empty. */

# define chk_mem(CASIO__MEM) \
	if (!(CASIO__MEM)) return (casio_error_nostream);

/* Check if the file is not empty. */

# define chk_filesize(CASIO__SZ) \
	if (!(casio_off_t)(CASIO__SZ)) return (casio_error_empty);

#endif /* LOCAL_LINK_USAGE_H */
