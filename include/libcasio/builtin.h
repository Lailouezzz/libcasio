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
# include <libcasio/cdefs.h>
# include <libcasio/stream.h>
# ifndef LIBCASIO_DISABLED_FILE
#  include <stdio.h>
# endif
CASIO_BEGIN_NAMESPACE
CASIO_BEGIN_DECLS

/* Microsoft Windows thingies */
# if (defined(_WIN16) || defined(_WIN32) || defined(_WIN64)) \
	&& !defined(__WINDOWS__)
#  define __WINDOWS__
# endif
/* ************************************************************************* */
/*  Built-in streams                                                         */
/* ************************************************************************* */
/* Make a stream using the standard FILE interface. */
# ifndef LIBCASIO_DISABLED_FILE
extern int casio_open_stream_file OF((casio_stream_t **casio__stream,
	FILE *casio__readstream, FILE *casio__writestream,
	int casio__close_readstream, int casio__close_writestream));
# endif

/* Make a stream using the POSIX STREAMS interface. */
# if defined(__linux__) || (defined(__APPLE__) && defined(__MACH__))
extern int casio_opencom_streams OF((casio_stream_t **casio__stream,
	const char *casio__path));

extern int casio_open_stream_streams OF((casio_stream_t **casio__stream,
	const char *casio__path, casio_openmode_t casio__mode));
extern int casio_open_stream_fd      OF((casio_stream_t **casio__stream,
	int casio__readfd, int casio__writefd,
	int casio__closeread, int casio__closewrite));
# else
#  define LIBCASIO_DISABLED_STREAMS
# endif

/* Make a stream using libusb. */
# ifndef LIBCASIO_DISABLED_LIBUSB
extern int casio_openusb_libusb OF((casio_stream_t **casio__stream));
# endif

/* Make a stream using the Microsoft Windows API. */
# ifdef __WINDOWS__
extern int casio_openusb_windows OF((casio_stream_t **casio__stream));
extern int casio_opencom_windows OF((casio_stream_t **casio__stream,
	const char *casio__path));
# else
#  define LIBCASIO_DISABLED_WINDOWS
# endif
/* ************************************************************************* */
/*  Built-in serial devices listing                                          */
/* ************************************************************************* */
/* List serial devices on Linux. */
# ifdef __linux__
extern int casio_comlist_linux OF((casio_list_com_t *casio__callback,
	void *casio__cookie));
# endif

/* List serial devices on MacOS/OS X. */
# if defined(__APPLE__) && defined(__MACH__)
extern int casio_comlist_macos OF((casio_list_com_t *casio__callback,
	void *casio__cookie));
# endif

/* List serial devices on Microsoft Windows. */
# ifdef __WINDOWS__
extern int casio_comlist_windows OF((casio_list_com_t *casio__callback,
	void *casio__cookie));
# endif
/* ************************************************************************* */
/*  Built-in sleep function                                                  */
/* ************************************************************************* */
/* As there is no portable sleep function, libcasio implements one.
 * It takes a callback, of the following form: */

typedef void casio_sleep_t OF((unsigned long casio__ms));

extern void casio_set_sleep OF((casio_sleep_t *casio__func));
extern int  casio_sleep     OF((unsigned long casio__ms));

CASIO_END_DECLS
CASIO_END_NAMESPACE
#endif /* LIBCASIO_BUILTIN_H */
