/* ****************************************************************************
 * libcasio/cdefs/integers.h -- libcasio's implementation of `stdint.h`.
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
#ifndef  LIBCASIO_CDEFS_INTEGERS_H
# define LIBCASIO_CDEFS_INTEGERS_H 1
# include "../cdefs.h"

CASIO_BEGIN_NAMESPACE

# if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#  include <stdint.h>
#  include <inttypes.h>

/* `stdint.h` and `inttypes.h` are standard C99 headers.
 * `stdint.h` provides the `uintN_t` types, and
 * `inttypes.h` provides the `PRI[uxX]N` macros. */

typedef uint8_t  casio_uint8_t;
typedef uint16_t casio_uint16_t;
typedef uint32_t casio_uint32_t;

#  define CASIO_PRIu8  PRIu8
#  define CASIO_PRIx8  PRIx8
#  define CASIO_PRIX8  PRIX8
#  define CASIO_PRIu16 PRIu16
#  define CASIO_PRIx16 PRIx16
#  define CASIO_PRIX16 PRIX16
#  define CASIO_PRIu32 PRIu32
#  define CASIO_PRIx32 PRIx32
#  define CASIO_PRIX32 PRIX32

# else /* C89 */
#  include <limits.h>

/* Here, we ought to do some C89 hacking.
 * We'll use the `limits.h` definitions to try and guess which one of the
 * default types are the 8-bit, 16-bit and 32-bit integer. */

#  define CASIO_P8 "hh"
typedef unsigned char casio_uint8_t;

/* 16-bit integer. */

#  if (USHRT_MAX > 0xffffUL)
#   error "No 16-bit type, exiting!"
#  endif
#  define CASIO_P16 "h"
typedef unsigned short casio_uint16_t;

/* 32-bit integer. */

#  if    (UINT_MAX == 0xffffffffUL)
#   define CASIO_P32 ""
typedef unsigned int   casio_uint32_t;
#  elif (ULONG_MAX == 0xffffffffUL)
#   define CASIO_P32 "l"
typedef unsigned long  casio_uint32_t;
#  else

/* There is nothing between `char` and `short`, and `char` is always
 * byte-wide;
 * `long long` is not defined in C89, and even if it can be used as a
 * compiler extension for C89, it is supposed to be 64-bit or more.
 * So basically we're running out of options here. */

#   error "No 32-bit type, exiting!"
#  endif

#  define CASIO_PRIu8  CASIO_P8  "u"
#  define CASIO_PRIx8  CASIO_P8  "x"
#  define CASIO_PRIX8  CASIO_P8  "X"
#  define CASIO_PRIu16 CASIO_P16 "u"
#  define CASIO_PRIx16 CASIO_P16 "x"
#  define CASIO_PRIX16 CASIO_P16 "X"
#  define CASIO_PRIu32 CASIO_P32 "u"
#  define CASIO_PRIx32 CASIO_P32 "x"
#  define CASIO_PRIX32 CASIO_P32 "X"
# endif

/* printf thing for `size_t` */

# if defined(_WIN64)
#  define CASIO_PRIuSIZE "l64u"
#  define CASIO_PRIxSIZE "l64x"
#  define CASIO_PRIXSIZE "l64X"
# elif defined(_WIN32)
#  define CASIO_PRIuSIZE "u"
#  define CASIO_PRIxSIZE "x"
#  define CASIO_PRIXSIZE "X"
# else
#  define CASIO_PRIuSIZE "zu"
#  define CASIO_PRIxSIZE "zx"
#  define CASIO_PRIXSIZE "zX"
# endif

CASIO_END_NAMESPACE
#endif /* LIBCASIO_CDEFS_INTEGERS_H */
