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
#ifndef  LIBCASIO_CDEFS_H
# define LIBCASIO_CDEFS_H
# include <libcasio/config.h>

/* ************************************************************************* */
/*  Check for compilers                                                      */
/* ************************************************************************* */
/* First, GCC. */

# ifdef  CASIO_GNUC_PREREQ
#  undef CASIO_GNUC_PREREQ
# endif
# if defined(__GNUC__) && defined(__GNUC_MINOR__)
#  define CASIO_GNUC_PREREQ(CASIO__MAJ, CASIO__MIN) \
	((__GNUC__ << 16) + __GNUC_MINOR__ >= ((CASIO__MAJ) << 16) + (CASIO__MIN))
# else
#  define CASIO_GNUC_PREREQ(CASIO__MAJ, CASIO__MIN) 0
# endif

/* Then, Microsoft's Compiler. */

# ifdef  CASIO_MSC_PREREQ
#  undef CASIO_MSC_PREREQ
# endif
# if defined(_MSC_VER)
#  define CASIO_MSC_PREREQ(CASIO__MAJ, CASIO__MIN) \
	(_MSC_VER >= (CASIO__MAJ) * 1000 + (CASIO__MIN))
# else
#  define CASIO_MSC_PREREQ(CASIO__MAJ, CASIO__MIN) 0
# endif
/* ************************************************************************* */
/*  Function declaration arguments                                           */
/* ************************************************************************* */
/* Pre-ANSI C compilers don't support arguments (and argument types) in
 * function declarations. This macro is inspired from zlib. */

# ifdef  OF
#  undef OF
# endif
# if defined(__STDC__) && __STDC__
#  define OF(CASIO_ARGS) CASIO_ARGS
# else
#  define OF(CASIO_ARGS) ()
# endif
/* ************************************************************************* */
/*  C++ declarations and namespaces management                               */
/* ************************************************************************* */
/* libcasio is made in C. */

# ifdef  CASIO_BEGIN_DECLS
#  undef CASIO_BEGIN_DECLS
# endif
# ifdef  CASIO_END_DECLS
#  undef CASIO_END_DECLS
# endif
# ifdef  CASIO_BEGIN_NAMESPACE
#  undef CASIO_BEGIN_NAMESPACE
# endif
# ifdef  CASIO_END_NAMESPACE
#  undef CASIO_END_NAMESPACE
# endif

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
/* ************************************************************************* */
/*  `casio_attr_wur`: warn if the result is unused.                          */
/* ************************************************************************* */
/* That's pretty much all of it. */

# ifdef  casio_attr_wur
#  undef casio_attr_wur
# endif
# if CASIO_GNUC_PREREQ(4, 0)
#  define casio_attr_wur __attribute__((warn_unused_result))
# elif CASIO_MSC_PREREQ(17, 0)
#  include <sal.h>
#  define casio_attr_wur _Check_return_
# endif

# include <libcasio/cdefs/integers.h>
# include <libcasio/cdefs/endian.h>
#endif /* LIBCASIO_CDEFS_H */
