/* ****************************************************************************
 * mcsfile/decode/decode.h -- mcsfile internal macros for decoding.
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
#ifndef  LOCAL_MCSFILE_DECODE_H
# define LOCAL_MCSFILE_DECODE_H 1
# include "../mcsfile.h"

/* ---
 * MCS files specific decoding functions.
 * --- */

/* Schemes. */

# define CASIO_MCSFUNC(CASIO__NAME) \
extern int casio_decode_mcs_##CASIO__NAME \
OF((casio_mcsfile_t **casio__handle, casio_stream_t *casio__buffer, \
	casio_mcshead_t *casio__head));

CASIO_MCSFUNC(var)
CASIO_MCSFUNC(cells)
CASIO_MCSFUNC(picture)
CASIO_MCSFUNC(capture)
CASIO_MCSFUNC(program)
CASIO_MCSFUNC(setup)
CASIO_MCSFUNC(spreadsheet)
CASIO_MCSFUNC(string)

/* ---
 * CAS-specific decoding functions.
 * --- */

/* Schemes. */

# define CASIO_CASFUNC(CASIO__NAME) \
extern int casio_decode_caspart_##CASIO__NAME \
OF((casio_mcsfile_t *casio__handle, casio_stream_t *casio__buffer));

CASIO_CASFUNC(var)
CASIO_CASFUNC(program)
CASIO_CASFUNC(matrix)
CASIO_CASFUNC(capture)

#endif /* LOCAL_MCSFILE_DECODE_H */
