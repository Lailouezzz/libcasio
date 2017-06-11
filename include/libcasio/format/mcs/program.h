/* ****************************************************************************
 * libcasio/format/mcs/program.h -- description of the MCS program format.
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
#ifndef  LIBCASIO_FORMAT_MCS_PROGRAM_H
# define LIBCASIO_FORMAT_MCS_PROGRAM_H
# include <libcasio/cdefs.h>
# include <stdint.h>
# pragma pack(1)
CASIO_BEGIN_NAMESPACE

/* Programs have a simple header: */

typedef struct casio_mcs_programheader_s {
	/* the program password. not encrypted, anything */
	casio_uint8_t casio_mcs_programheader_password[8];

	/* and some alignment. */
	casio_uint8_t casio_mcs_programheader__align[2];
} casio_mcs_programheader_t;

/* Then comes their content, multi-byte FONTCHARACTER encoded. */

CASIO_END_NAMESPACE
# pragma pack()
#endif /* LIBCASIO_FORMAT_MCS_PROGRAM_H */
