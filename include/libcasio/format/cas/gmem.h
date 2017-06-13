/* ****************************************************************************
 * libcasio/format/cas/gmem.h -- description of the CAS G-MEM format.
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
#ifndef  LIBCASIO_FORMAT_CAS_GMEM_H
# define LIBCASIO_FORMAT_CAS_GMEM_H
# include "../../cdefs.h"
# pragma pack(1)
CASIO_BEGIN_NAMESPACE

/* You can save some graph function's expression inside a graph memory (G-MEM).
 * The data size is stored in CAS50's `length` field.
 * TODO: find out the equivalent with CAS40 headers?
 *
 * It starts with 20 graph entries, which have this format: */

typedef struct casio_cas_gmem_entry_s {
	/* function type code */
	casio_uint8_t casio_cas_gmem_entry_type;

	/* flags */
	casio_uint8_t casio_cas_gmem_entry_flags;
} casio_cas_gmem_entry_t;

/* Where the function types are the following: */

# define casio_cas_gmem_type_ye  0x00 /* Y= */
# define casio_cas_gmem_type_re  0x40 /* r= */
# define casio_cas_gmem_type_yg  0x0C /* Y> */
# define casio_cas_gmem_type_yl  0x0D /* Y< */
# define casio_cas_gmem_type_yge 0x0E /* Y>= */
# define casio_cas_gmem_type_yle 0x0F /* Y<= */

/* And the flags are the following: */

# define casio_cas_gmem_flag_orange 0x01
# define casio_cas_gmem_flag_blue   0x02
# define casio_cas_gmem_flag_green  0x04
# define casio_cas_gmem_flag_sel    0x80
# define casio_cas_gmem_flag_eqsel  0xB0

/* If there are no flags (`entry.flags == 0`), then the function is
 * not defined.
 * If the function type is one of: Y=, r=, Param (?)
 * Then use `cas_gmem_flag_eqsel` instead of `cas_gmem_flag_sel`.
 *
 * Then what we have after is the real content of the G-MEM (Y=DATA).
 * Each function's expression is separated from the others by a 0xFF character.
 * Casetta's documentation says the data is reversed, I don't know what they
 * mean (order? bitwise not?).
 *
 * There are 90 characters at the end of the content. It seems that they don't
 * change if you have one or more Y= function defined (independently of the
 * function expression).
 * TODO: find out what these are. */

CASIO_END_NAMESPACE
# pragma pack()
#endif /* LIBCASIO_FORMAT_CAS_GMEM_H */
