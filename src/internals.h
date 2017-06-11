/* ****************************************************************************
 * internals.h -- libcasio main internal header.
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
#ifndef  LOCAL_INTERNALS_H
# define LOCAL_INTERNALS_H 1
# define _DEFAULT_SOURCE /* XXX: glibc hacking */
# define _POSIX_C_SOURCE 199309L
# include <libcasio.h>
# include <libcasio/format.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <ctype.h>
# include "utils/endian.h"
# include "log/log.h"

/* MS-Windows stuff */
# if (defined(_WIN16) || defined(_WIN32) || defined(_WIN64)) \
	&& !defined(__WINDOWS__)
#  define __WINDOWS__
# endif

/* Standard library macros. */
# ifndef  min
#  define min(CASIO__A, CASIO__B) \
	((CASIO__A) < (CASIO__B) ? (CASIO__A) : (CASIO__B))
# endif
# ifndef  max
#  define max(CASIO__A, CASIO__B) \
	((CASIO__A) > (CASIO__B) ? (CASIO__A) : (CASIO__B))
# endif
# ifndef  abs
#  define abs(CASIO__A) \
	((CASIO__A) < 0 ? -(CASIO__A) : (CASIO__A))
# endif

/* Checksum. */

extern unsigned char  casio_checksum8  OF((void *casio__mem,
	size_t casio__size, unsigned char  casio__current));
extern casio_uint32_t casio_checksum32 OF((void *casio__mem,
	size_t casio__size, casio_uint32_t casio__current));

/* Extension. */

extern int casio_getext OF((const char *casio__path,
	char *casio__buf, size_t casio__n));

#endif /* LOCAL_INTERNALS_H */
