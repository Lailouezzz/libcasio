/* ****************************************************************************
 * libcasio/cdefs.h -- C helpers.
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
/* Pre-ANSI C compilers don't support arguments (and argument types) in
 * function declarations. This macro is inspired from zlib.
 *
 * It is defined outside of the include guard because it is not in
 * the libcasio defined namespace, so we want it to be redefined, in case. */

#if defined(OF)
#elif defined(__STDC__) && __STDC__
# define OF(CASIO_ARGS) CASIO_ARGS
#else
# define OF(CASIO_ARGS) ()
#endif

/* "Normal start" of the file is here. */

#ifndef  LIBCASIO_CDEFS_H
# define LIBCASIO_CDEFS_H
# include "config.h"
# include <stddef.h>
# include <time.h>

/* Check the library version. */

# define LIBCASIO_PREREQ(CASIO__MAJ, CASIO__MIN) \
	((CASIO__MAJ) >  (LIBCASIO_MAJOR) || \
	((CASIO__MAJ) == (LIBCASIO_MAJOR) && (CASIO__MIN) >= (LIBCASIO_MINOR)))

/* ---
 * Check for compilers.
 * --- */

/* First, GCC. */

# if defined(CASIO_GNUC_PREREQ)
# elif defined(__GNUC__) && defined(__GNUC_MINOR__)
#  define CASIO_GNUC_PREREQ(CASIO__MAJ, CASIO__MIN) \
	((__GNUC__ << 16) + __GNUC_MINOR__ >= ((CASIO__MAJ) << 16) + (CASIO__MIN))
# else
#  define CASIO_GNUC_PREREQ(CASIO__MAJ, CASIO__MIN) 0
# endif

/* Then, Microsoft's Compiler. */

# if defined(CASIO_MSC_PREREQ) || defined(_MSC_VER)
#  define CASIO_MSC_PREREQ(CASIO__MAJ, CASIO__MIN) \
	(_MSC_VER >= (CASIO__MAJ) * 1000 + (CASIO__MIN))
# else
#  define CASIO_MSC_PREREQ(CASIO__MAJ, CASIO__MIN) 0
# endif

/* ---
 * Extern functions.
 * --- */

/* Some platforms require more than simply 'extern'.
 * Here are macros to control this. */

# define CASIO_EXTERN extern
# define CASIO_EXPORT
# define CASIO_LOCAL  static

/* ---
 * Enumerations.
 * --- */

/* Enumerations can be great thanks to the compiler: better warnings,
 * better debug, better coding! */

# if   defined(LIBCASIO_USE_ENUMS)
# elif defined(__STDC__) && __STDC__
#  define LIBCASIO_USE_ENUMS 1
# else /* K&R C, no enums! */
#  define LIBCASIO_USE_ENUMS 0
# endif

/* ---
 * C++ declarations and namespaces management.
 * --- */

/* libcasio is made in C. */

# if 0 /* hey, what about this? */
#  define CASIO_BEGIN_NAMESPACE namespace casio {
#  define   CASIO_END_NAMESPACE }
# endif

# ifdef  __cplusplus
#  define CASIO_BEGIN_NAMESPACE
#  define     CASIO_BEGIN_DECLS extern "C" {
#  define       CASIO_END_DECLS }
#  define   CASIO_END_NAMESPACE
# else
#  define CASIO_BEGIN_NAMESPACE
#  define     CASIO_BEGIN_DECLS
#  define       CASIO_END_DECLS
#  define   CASIO_END_NAMESPACE
# endif

/* ---
 * Warn if the result is unused.
 * --- */

/* To do that, we'll use the `casio_attr_wur` attribute. */

# if CASIO_GNUC_PREREQ(4, 0)
#  define casio_attr_wur __attribute__((warn_unused_result))
# elif CASIO_MSC_PREREQ(17, 0)
#  include <sal.h>
#  define casio_attr_wur _Check_return_
# endif

# include "cdefs/integers.h"
# include "cdefs/endian.h"
#endif /* LIBCASIO_CDEFS_H */
