/* ****************************************************************************
 * utils/endian.h -- cross-platform endian utilities.
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
 *
 * This header is based on some work provided by Mathias Panzenböck.
 * ************************************************************************* */
#ifndef  LOCAL_UTILS_ENDIAN_H
# define LOCAL_UTILS_ENDIAN_H 1
# include <libcasio/cdefs.h>

# ifndef LIBCASIO_ENDIAN_DEFINED
#  define be16toh(CASIO__X) casio_be16toh(CASIO__X)
#  define le16toh(CASIO__X) casio_le16toh(CASIO__X)
#  define be32toh(CASIO__X) casio_be32toh(CASIO__X)
#  define le32toh(CASIO__X) casio_le32toh(CASIO__X)

#  define htobe16(CASIO__X) casio_htobe16(CASIO__X)
#  define htole16(CASIO__X) casio_htole16(CASIO__X)
#  define htobe32(CASIO__X) casio_htobe32(CASIO__X)
#  define htole32(CASIO__X) casio_htole32(CASIO__X)
# endif

#endif /* LOCAL_UTILS_ENDIAN_H */
