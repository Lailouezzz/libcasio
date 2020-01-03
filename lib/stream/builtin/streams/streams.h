/* ****************************************************************************
 * stream/builtin/streams/streams.h -- built-in STREAMS stream internals.
 * Copyright (C) 2016-2017 Thomas "Cakeisalie5" Touhey <thomas@touhey.fr>
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
#ifndef  LOCAL_STREAM_BUILTIN_STREAMS_H
# define LOCAL_STREAM_BUILTIN_STREAMS_H 1
# include "../../../internals.h"
# ifndef LIBCASIO_DISABLED_STREAMS
#  include <sys/stat.h>
#  include <sys/ioctl.h>
#  include <fcntl.h>
#  include <unistd.h>
#  include <errno.h>
#  include <termios.h>

/* The cookie type. */

# define BUFSIZE 2048

typedef struct {
	int _readfd, _writefd;
	int _closeread, _closewrite;

	/* Buffer [control] */

	ssize_t _start, _end;
	unsigned char _buffer[BUFSIZE];
} streams_cookie_t;

/* General callbacks. */

CASIO_EXTERN int CASIO_EXPORT casio_streams_close
	OF((streams_cookie_t *casio__cookie));
CASIO_EXTERN int CASIO_EXPORT casio_streams_settm
	OF((streams_cookie_t *casio__cookie,
		const casio_timeouts_t *casio__timeouts));

/* Character device callbacks. */

CASIO_EXTERN ssize_t CASIO_EXPORT casio_streams_read
	OF((streams_cookie_t *casio__cookie,
		unsigned char *casio__dest, size_t casio__size));
CASIO_EXTERN int CASIO_EXPORT casio_streams_write
	OF((streams_cookie_t *casio__cookie,
		const unsigned char *casio__data, size_t casio__size));

/* Serial callbacks. */

CASIO_EXTERN int CASIO_EXPORT casio_streams_setattrs
	OF((streams_cookie_t *casio__cookie,
		const casio_streamattrs_t *casio__settings));

/* SCSI callbacks. */

#  if 0
CASIO_EXTERN int CASIO_EXPORT casio_streams_scsi_request
	OF((streams_cookie_t *casio__cookie, casio_scsi_t *casio__request));
#  endif

# endif
#endif /* LOCAL_STREAM_BUILTIN_STREAMS_H */
