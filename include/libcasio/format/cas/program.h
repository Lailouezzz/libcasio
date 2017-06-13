/* ****************************************************************************
 * libcasio/format/cas/program.h -- description of the CAS program format.
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
#ifndef  LIBCASIO_FORMAT_CAS_PROGRAM_H
# define LIBCASIO_FORMAT_CAS_PROGRAM_H
# include "../../cdefs.h"
# pragma pack(1)
CASIO_BEGIN_NAMESPACE

/* TODO: describe programs to the newcomer.
 * The CAS40 specific header is the following: */

# define casiolink_program_stats   0x02 /* store stats data */
# define casiolink_program_matrix  0x04 /* matrix mode */
# define casiolink_program_stddev  0x10 /* standard deviation mode */
# define casiolink_program_linreg  0x20 /* linear regression mode */
# define casiolink_program_basen   0x40 /* base-n mode */
# define casiolink_program_drawst  0x80 /* draw stats graph */

typedef struct casio_cas_spe_program_s {
	casio_uint8_t  casio_cas_spe_program__unknown0;
	casio_uint16_t casio_cas_spe_program_length;
	casio_uint8_t  casio_cas_spe_program_flags;
	casio_uint8_t  casio_cas_spe_program__unknown1;
} casio_cas_spe_program_t;

/* Then the program, using the multi-byte FONTCHARACTER encoding, comes.
 * F-MEM (saves of series of commands) are stored like very small programs. */

CASIO_END_NAMESPACE
# pragma pack()
#endif /* LIBCASIO_FORMAT_CAS_PROGRAM_H */
