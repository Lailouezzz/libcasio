/* ****************************************************************************
 * libcasio/utils.h -- libcasio utilities.
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
#ifndef  LIBCASIO_UTILS_H
# define LIBCASIO_UTILS_H 1
# include "cdefs.h"

/* ASCII-HEX, ASCII-DEC utilities */
CASIO_EXTERN void          CASIO_EXPORT casio_putascii
	OF((unsigned char *casio__p, unsigned long casio__i, int casio__n));
CASIO_EXTERN unsigned long CASIO_EXPORT casio_getascii
	OF((const unsigned char *casio__p, int casio__n));

CASIO_EXTERN unsigned long CASIO_EXPORT casio_getdec
	OF((unsigned long casio__h));
CASIO_EXTERN unsigned long CASIO_EXPORT casio_gethex
	OF((unsigned long casio__d));

#endif /* LIBCASIO_UTILS_H */
