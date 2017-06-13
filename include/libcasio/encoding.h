/* ****************************************************************************
 * libcasio/encoding.h -- libcasio character encoding.
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
#ifndef  LIBCASIO_ENCODING_H
# define LIBCASIO_ENCODING_H
# include "cdefs.h"

typedef int casio_encode_string_t OF((void *casio__dest,
	const void *casio__data, size_t *casio__size));

CASIO_EXTERN int CASIO_EXPORT casio_register_conv
	OF((const char *casio__to, const char *casio__from,
		casio_encode_t *casio__encode_from));

CASIO_EXTERN int CASIO_EXPORT casio_encode_string
	OF((void *casio__dest, const void *casio__data,
		size_t *casio__size));

#endif /* LIBCASIO_ENCODING_H */
