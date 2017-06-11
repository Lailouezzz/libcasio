/* ****************************************************************************
 * file/decode/decode.h -- libcasio internal macros for decoding.
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
#ifndef  LOCAL_FILE_DECODE_H
# define LOCAL_FILE_DECODE_H 1
# include "../file.h"
# include <libcasio/format.h>

/* ************************************************************************* */
/*  Main decoding utilities                                                  */
/* ************************************************************************* */
/* Main decoding functions with expected types. */
extern int casio_decode_std OF((casio_file_t **casio__handle,
	const char *casio__path, casio_stream_t *casio__buffer,
	casio_standard_header_t *casio__standard,
	casio_filetype_t casio__expected_types));

/* Main decoding functions w/o expected types. */
extern int casio_decode_casemul OF((casio_file_t **casio__handle,
	casio_stream_t *casio__buffer, int casio__big_endian));
extern int casio_decode_storage OF((casio_file_t **casio__handle,
	casio_stream_t *casio__buffer));
extern int casio_decode_cas     OF((casio_file_t **casio__handle,
	casio_stream_t *casio__buffer));
extern int casio_decode_grc     OF((casio_file_t **casio__handle,
	casio_stream_t *casio__buffer));
/* ************************************************************************* */
/*  "Standard"-specific decoding functions                                   */
/* ************************************************************************* */
/* Schemes */
# define CASIO_STDFUNC(CASIO__NAME) \
extern int casio_decode_std_##CASIO__NAME \
OF((casio_file_t **casio__file, \
	casio_stream_t *casio__buffer, casio_standard_header_t *casio__std));
# define CASIO_PRIZMFUNC(CASIO__NAME) \
extern int casio_decode_std_cg_##CASIO__NAME \
OF((casio_file_t **casio__file, \
	casio_stream_t *casio__buffer, casio_standard_header_t *casio__std, \
	casio_standard_subheader_t *casio__sub, \
	casio_standard_prizm_subheader_t *casio__pzm, uint32_t *casio__check));
# define CASIO_CPFUNC(CASIO__NAME) \
extern int casio_decode_std_cp_##CASIO__NAME \
OF((casio_file_t **casio__handle, casio_stream_t *casio__buffer, \
	casio_standard_header_t *casio__std, \
	casio_standard_subheader_t *casio__sub, \
	casio_standard_classpad_subheader_t *casio__cp, uint32_t *casio__check));
# define CASIO_PICFUNC(CASIO__NAME) \
extern int casio_decode_std_##CASIO__NAME \
OF((casio_file_t **casio__handle, \
	casio_stream_t *casio__buffer, casio_standard_header_t *casio__std, \
	casio_standard_picheader_t *casio__pic));

/* Prototypes */
CASIO_STDFUNC(mcs)
CASIO_STDFUNC(eact)
CASIO_STDFUNC(addin)
CASIO_STDFUNC(lang)
CASIO_STDFUNC(fkey)

CASIO_CPFUNC(addin)
CASIO_PRIZMFUNC(addin)
CASIO_PRIZMFUNC(lang)
CASIO_PRIZMFUNC(fkey)
CASIO_PICFUNC(g3p)
CASIO_PICFUNC(c2p)

#endif /* LOCAL_FILE_DECODE_H */
