/* ****************************************************************************
 * libcasio/builtin.h -- libcasio built-in platform-specific things.
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
#ifndef  LIBCASIO_BUILTIN_H
# define LIBCASIO_BUILTIN_H
# include "cdefs.h"
# include "stream.h"
# ifndef LIBCASIO_DISABLED_FILE
#  include <stdio.h>
# endif
CASIO_BEGIN_NAMESPACE
CASIO_BEGIN_DECLS

/* ************************************************************************* */
/*  Built-in streams                                                         */
/* ************************************************************************* */
/* Make a stream using the standard FILE interface. */
# ifndef LIBCASIO_DISABLED_FILE
CASIO_EXTERN int CASIO_EXPORT casio_open_stream_file
	OF((casio_stream_t **casio__stream,
		FILE *casio__readstream, FILE *casio__writestream,
		int casio__close_readstream, int casio__close_writestream));
# endif

/* Make a stream using the POSIX STREAMS interface. */
# if defined(__linux__) || (defined(__APPLE__) && defined(__MACH__))
CASIO_EXTERN int CASIO_EXPORT casio_opencom_streams
	OF((casio_stream_t **casio__stream, const char *casio__path));

CASIO_EXTERN int CASIO_EXPORT casio_open_stream_streams
	OF((casio_stream_t **casio__stream,
		const char *casio__path, casio_openmode_t casio__mode));
CASIO_EXTERN int CASIO_EXPORT casio_open_stream_fd
	OF((casio_stream_t **casio__stream,
		int casio__readfd, int casio__writefd,
		int casio__closeread, int casio__closewrite));
# else
#  define LIBCASIO_DISABLED_STREAMS
# endif

/* Make a stream using libusb. */
# ifndef LIBCASIO_DISABLED_LIBUSB
CASIO_EXTERN int CASIO_EXPORT casio_openusb_libusb
	OF((casio_stream_t **casio__stream));
# endif

/* Make a stream using the Microsoft Windows API. */
# if defined(_WIN16) || defined(_WIN32) || defined(_WIN64) \
	|| defined(__WINDOWS__)
CASIO_EXTERN int CASIO_EXPORT casio_openusb_windows
	OF((casio_stream_t **casio__stream));
CASIO_EXTERN int CASIO_EXPORT casio_opencom_windows
	OF((casio_stream_t **casio__stream, const char *casio__path));
# else
#  define LIBCASIO_DISABLED_WINDOWS
# endif
/* ************************************************************************* */
/*  Built-in filesystems                                                     */
/* ************************************************************************* */
/* Make a local POSIX filesystem interface. */
# if defined(__linux__) || (defined(__APPLE__) && defined(__MACH__))
/* TODO */
# else
#  define LIBCASIO_DISABLED_POSIX_FS
# endif
/* ************************************************************************* */
/*  Built-in serial devices listing                                          */
/* ************************************************************************* */
/* List serial devices on Linux. */
# ifdef __linux__
CASIO_EXTERN int CASIO_EXPORT casio_comlist_linux
	OF((casio_list_com_t *casio__callback, void *casio__cookie));
# endif

/* List serial devices on MacOS/OS X. */
# if defined(__APPLE__) && defined(__MACH__)
CASIO_EXTERN int CASIO_EXPORT casio_comlist_macos
	OF((casio_list_com_t *casio__callback, void *casio__cookie));
# endif

/* List serial devices on Microsoft Windows. */
# ifndef LIBCASIO_DISABLED_WINDOWS
CASIO_EXTERN int CASIO_EXPORT casio_comlist_windows
	OF((casio_list_com_t *casio__callback, void *casio__cookie));
# endif
/* ************************************************************************* */
/*  Built-in functions                                                       */
/* ************************************************************************* */
/* As there is no portable sleep function, libcasio implements one.
 * It takes a callback, of the following form: */

typedef void casio_sleep_t OF((unsigned long casio__ms));

CASIO_EXTERN void CASIO_EXPORT casio_set_sleep
	OF((casio_sleep_t *casio__func));
CASIO_EXTERN int  CASIO_EXPORT casio_sleep
	OF((unsigned long casio__ms));

/* And here are cross-platform allocation functions.
 * They are defined just in case. */

CASIO_EXTERN void* CASIO_EXPORT casio_alloc
	OF((size_t casio__num_elements, size_t casio__element_size));
CASIO_EXTERN void  CASIO_EXPORT casio_free
	OF((void *casio__ptr));

CASIO_END_DECLS
CASIO_END_NAMESPACE
#endif /* LIBCASIO_BUILTIN_H */
