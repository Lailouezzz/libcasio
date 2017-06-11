/* ****************************************************************************
 * file/corresp/standard.h -- standard header types.
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
#ifndef  LOCAL_FILE_CORRESP_STANDARD_H
# define LOCAL_FILE_CORRESP_STANDARD_H
# include "../file.h"

# define casio_stdflag_check1 0x0001 /* check first control value */
# define casio_stdflag_check2 0x0002 /* check second control value */
# define casio_stdflag_sub    0x0004 /* is followed by a standard subheader */
# define casio_stdflag_pic    0x0008 /* is followed by a picture subheader */

/* Get the correspondance. */

extern int casio_maketype_std OF((const char *casio__path,
	casio_standard_header_t *casio__std, unsigned int *casio__flags,
	casio_filefor_t *casio__platform, casio_filetype_t *casio__type));

extern int casio_maketype_sub OF((casio_standard_subheader_t *casio__sub,
	unsigned int *casio__flags, casio_filetype_t *casio__type,
	casio_filefor_t *casio__platform));

#endif /* LOCAL_FILE_CORRESP_STANDARD_H */
