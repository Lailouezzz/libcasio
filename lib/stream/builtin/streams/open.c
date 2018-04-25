/* ****************************************************************************
 * stream/builtin/streams/open.c -- open a STREAMS stream.
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
#include "streams.h"
#ifndef LIBCASIO_DISABLED_STREAMS

/* Callbacks. */

CASIO_LOCAL const casio_streamfuncs_t casio_streams_callbacks =
casio_stream_callbacks_for_serial(casio_streams_close,
	casio_streams_setattrs, casio_streams_settm,
	casio_streams_read, casio_streams_write);

/**
 *	casio_opencom_streams:
 *	Callback for `opencom`.
 *
 *	@arg	stream		the stream to make.
 *	@arg	path		the path.
 *	@return				the error (0 if ok).
 */

int CASIO_EXPORT casio_opencom_streams(casio_stream_t **stream,
	const char *path)
{
	return (casio_open_stream_streams(stream, path,
		CASIO_OPENMODE_READ | CASIO_OPENMODE_WRITE));
}

/**
 *	casio_open_stream_streams:
 *	Initialize libcasio with char device.
 *
 *	@arg	stream		the stream to make.
 *	@arg	path		the path.
 *	@arg	mode		the mode.
 *	@return				the error (0 if ok).
 */

int CASIO_EXPORT casio_open_stream_streams(casio_stream_t **stream,
	const char *path, casio_openmode_t mode)
{
	int openfd, readfd, writefd, flags;

	/* Make up the flags. */

	flags = O_NOCTTY;
	if ((mode & CASIO_OPENMODE_READ) && (mode & CASIO_OPENMODE_WRITE))
		flags |= O_RDWR;
	else if (mode & CASIO_OPENMODE_READ)
		flags |= O_RDONLY;
	else if (mode & CASIO_OPENMODE_WRITE)
		flags |= O_WRONLY;
	else return (casio_error_invalid);

	/* Open the stream. */

	openfd = open(path, flags);
	if (openfd < 0) switch (errno) {
		case ENODEV: case ENOENT: case ENXIO:
		case EPIPE: case ESPIPE:
			/* No such device. */

			msg((ll_error, "couldn't open calculator: %s",
				strerror(errno)));
			return (casio_error_nocalc);

		case EACCES:
			/* No access. */

			msg((ll_error, "permission denied"));
			return (casio_error_noaccess);

		default:
			/* Default case. */

			msg((ll_error, "unknown error: %s (0x%X)",
				strerror(errno), errno));
			return (casio_error_unknown);
	}

	/* Make the file descriptors. */

	readfd  = mode & CASIO_OPENMODE_READ  ? openfd : -1;
	writefd = mode & CASIO_OPENMODE_WRITE ? openfd : -1;

	/* Make the final stream. */

	return (casio_open_stream_fd(stream, readfd, writefd, 1, 1));
}

/**
 *	casio_open_stream_fd:
 *	Initialize libcasio with file descriptors.
 *
 *	@arg	stream		the stream to make.
 *	@arg	readfd		the read file descriptor.
 *	@arg	writefd		the write file descriptor.
 *	@arg	closeread	close the read file descriptor?
 *	@arg	closewrite	close the write file descriptor?
 *	@return				the error (0 if ok)
 */

int CASIO_EXPORT casio_open_stream_fd(casio_stream_t **stream,
	int readfd, int writefd, int closeread, int closewrite)
{
	int err; streams_cookie_t *cookie = NULL;
	casio_openmode_t mode =
		CASIO_OPENMODE_READ | CASIO_OPENMODE_WRITE | CASIO_OPENMODE_SERIAL;

	/* Check if the devices are valid. */

	if ( readfd < 0 ||   read(readfd, NULL, 0) < 0)
		mode &= ~CASIO_OPENMODE_READ;
	if (writefd < 0 || write(writefd, NULL, 0) < 0)
		mode &= ~CASIO_OPENMODE_WRITE;

	/* Check if we have at least read/write file descriptors. */

	if (!mode) {
		err = casio_error_invalid;
		goto fail;
	}

	/* Allocate cookie. */

	cookie = casio_alloc(1, sizeof(streams_cookie_t));
	if (!cookie) {
		err = casio_error_alloc;
		goto fail;
	}

	cookie->_readfd = readfd;
	cookie->_writefd = writefd;
	cookie->_closeread = closeread;
	cookie->_closewrite = closewrite;
	cookie->_start = 0;
	cookie->_end = -1;

	/* Init for real. */

	msg((ll_info, "Initializing STREAMS stream with fds: (%d, %d)",
		readfd, writefd));

	/* Final call. */

	return (casio_open_stream(stream, mode, cookie,
		&casio_streams_callbacks, 0));
fail:
	if (readfd >= 0 && closeread)
		close(readfd);
	if (writefd >= 0 && writefd != readfd && closewrite)
		close(writefd);
	return (err);
}

#endif
