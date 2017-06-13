/* ****************************************************************************
 * libcasio/cdefs/endian.h -- libcasio's implementation of `endian.h`.
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
#ifndef  LIBCASIO_CDEFS_ENDIAN_H
# define LIBCASIO_CDEFS_ENDIAN_H
# include "../cdefs.h"

/* These function always exist. They are the default ones, if the platform
 * does not have any equivalent function (or has one but indicates that
 * fact badly). */

CASIO_EXTERN casio_uint16_t CASIO_EXPORT casio_be16toh
	OF((casio_uint16_t casio__x));
CASIO_EXTERN casio_uint16_t CASIO_EXPORT casio_le16toh
	OF((casio_uint16_t casio__x));
CASIO_EXTERN casio_uint32_t CASIO_EXPORT casio_be32toh
	OF((casio_uint32_t casio__x));
CASIO_EXTERN casio_uint32_t CASIO_EXPORT casio_le32toh
	OF((casio_uint32_t casio__x));

CASIO_EXTERN casio_uint16_t CASIO_EXPORT casio_htobe16
	OF((casio_uint16_t casio__x));
CASIO_EXTERN casio_uint16_t CASIO_EXPORT casio_htole16
	OF((casio_uint16_t casio__x));
CASIO_EXTERN casio_uint32_t CASIO_EXPORT casio_htobe32
	OF((casio_uint32_t casio__x));
CASIO_EXTERN casio_uint32_t CASIO_EXPORT casio_htole32
	OF((casio_uint32_t casio__x));

# if defined(__APPLE__)
#  include <libkern/OSByteOrder.h>

#  define casio_int_be16toh(CASIO__X) OSSwapBigToHostInt16(CASIO__X)
#  define casio_int_le16toh(CASIO__X) OSSwapLittleToHostInt16(CASIO__X)
#  define casio_int_be32toh(CASIO__X) OSSwapBigToHostInt32(CASIO__X)
#  define casio_int_le32toh(CASIO__X) OSSwapLittleToHostInt32(CASIO__X)

#  define casio_int_htobe16(CASIO__X) OSSwapHostToBigInt16(CASIO__X)
#  define casio_int_htole16(CASIO__X) OSSwapHostToLittleInt16(CASIO__X)
#  define casio_int_htobe32(CASIO__X) OSSwapHostToBigInt32(CASIO__X)
#  define casio_int_htole32(CASIO__X) OSSwapHostToLittleInt32(CASIO__X)

# elif defined(__OpenBSD__)
#  include <sys/endian.h>
#  define LIBCASIO_ENDIAN_DEFINED

# elif defined(_WIN16) || defined(_WIN32) || defined(_WIN64) \
	|| defined(__WINDOWS__)
#  include <winsock2.h>
#  include <sys/param.h>

#  if BYTE_ORDER == LITTLE_ENDIAN
#   define casio_int_be16toh(x) ntohs(CASIO__X)
#   define casio_int_le16toh(x) (CASIO__X)
#   define casio_int_be32toh(x) ntohl(CASIO__X)
#   define casio_int_le32toh(x) (CASIO__X)

#   define casio_int_htobe16(CASIO__X) htons(CASIO__X)
#   define casio_int_htole16(CASIO__X) (CASIO__X)
#   define casio_int_htobe32(CASIO__X) htonl(CASIO__X)
#   define casio_int_htole32(CASIO__X) (CASIO__X)
#  else
#   define casio_int_be16toh(CASIO__X) (CASIO__X)
#   define casio_int_le16toh(CASIO__X) ntohs(CASIO__X)
#   define casio_int_be32toh(CASIO__X) (CASIO__X)
#   define casio_int_le32toh(CASIO__X) ntohl(CASIO__X)

#   define casio_int_htobe16(CASIO__X) (CASIO__X)
#   define casio_int_htole16(CASIO__X) htons(CASIO__X)
#   define casio_int_htobe32(CASIO__X) (CASIO__X)
#   define casio_int_htole32(CASIO__X) htonl(CASIO__X)
#  endif

# elif defined(__GLIBC__) && defined(__USE_MISC)
#  include <endian.h>
#  define LIBCASIO_ENDIAN_DEFINED

# endif

/* *e*toh, hto*e* macros are already defined, just make `casio_` aliases. */

# ifdef LIBCASIO_ENDIAN_DEFINED
#  define casio_int_be16toh(CASIO__X) be16toh(CASIO__X)
#  define casio_int_le16toh(CASIO__X) le16toh(CASIO__X)
#  define casio_int_be32toh(CASIO__X) be32toh(CASIO__X)
#  define casio_int_le32toh(CASIO__X) le32toh(CASIO__X)

#  define casio_int_htobe16(CASIO__X) htobe16(CASIO__X)
#  define casio_int_htole16(CASIO__X) htole16(CASIO__X)
#  define casio_int_htobe32(CASIO__X) htobe32(CASIO__X)
#  define casio_int_htole32(CASIO__X) htole32(CASIO__X)
# endif

/* If we aren't including this from libcasio's endian source functions,
 * `LIBCASIO_NO_ENDIAN` should not be defined. Otherwise, as we will be
 * defining the "dynamic" symbols, do not define the functions. */

# if !defined(LIBCASIO_NO_ENDIAN) && defined(casio_int_be16toh)
#  define casio_be16toh(CASIO__X) casio_int_be16toh(CASIO__X)
#  define casio_le16toh(CASIO__X) casio_int_le16toh(CASIO__X)
#  define casio_be32toh(CASIO__X) casio_int_be32toh(CASIO__X)
#  define casio_le32toh(CASIO__X) casio_int_le32toh(CASIO__X)

#  define casio_htobe16(CASIO__X) casio_int_htobe16(CASIO__X)
#  define casio_htole16(CASIO__X) casio_int_htole16(CASIO__X)
#  define casio_htobe32(CASIO__X) casio_int_htobe32(CASIO__X)
#  define casio_htole32(CASIO__X) casio_int_htole32(CASIO__X)
# endif

#endif /* LIBCASIO_CDEFS_ENDIAN_H */
