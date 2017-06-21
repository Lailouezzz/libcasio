/* ****************************************************************************
 * stream/builtin/file.c -- built-in FILE stream.
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
#include "../../internals.h"
#ifndef LIBCASIO_DISABLED_FILE
# include <stdlib.h>
# include <string.h>
# include <errno.h>
# if defined(__linux__)
#  include <stdio_ext.h>
# else
#  define __freadable(F) (1)
#  define __fwritable(F) (1)
# endif

/* cookie structure */
typedef struct {
	int _rstream_cl, _wstream_cl;
	FILE *_rstream, *_wstream;
} file_cookie_t;
/* ************************************************************************* */
/*  Callbacks                                                                */
/* ************************************************************************* */
/**
 *	casio_file_read:
 *	Read from a FILE.
 *
 *	@arg	cookie		the cookie.
 *	@arg	data		the data pointer
 *	@arg	size		the data size.
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL int casio_file_read(file_cookie_t *cookie,
	unsigned char *dest, size_t size)
{
	size_t recv;

	/* main receiving loop */
	recv = 0;
	do {
		/* read */
		recv = fread(dest, 1, size, cookie->_rstream);

		/* iterate */
		dest = (void*)((char*)dest + recv);
		size -= recv;

		/* check error */
		if (!recv) {
			/* approximation */
			if (errno != EAGAIN)
				break;

			/* i'm absolutely unsure about this... */
			msg((ll_info, "received EAGAIN, sleep and retry"));
			errno = 0;
			casio_sleep(3000);
			continue;
		}
	} while (size);

	/* check error */
	if (!recv) switch (errno) {
		/* end of file */
		case 0:
			msg((ll_error, "encountered an end of file"));
			return (casio_error_eof);

		/* - timeout - */
		case EINTR: /* alarm */
		case ETIMEDOUT:
# ifdef ETIME
		case ETIME:
# endif
			msg((ll_error, "timeout received"));
			return (casio_error_timeout);

		/* - device error - */
		case ENODEV:
		case EPIPE: case ESPIPE:
			msg((ll_error, "calculator was disconnected"));
			return (casio_error_nocalc);

		default:
			msg((ll_fatal, "errno was %d: %s", errno, strerror(errno)));
			return (casio_error_unknown);
	}

	return (0);
}

/**
 *	casio_file_write:
 *	Write to a FILE.
 *
 *	@arg	cookie		the cookie.
 *	@arg	data		the source
 *	@arg	size		the source size
 *	@return				the error code (0 if ok)
 */

CASIO_LOCAL int casio_file_write(file_cookie_t *cookie,
	const unsigned char *data, size_t size)
{
	size_t sent;

	/* main sending */
	sent = fwrite(data, size, 1, cookie->_wstream);

	/* check the error */
	if (!sent) switch (errno) {
		/* end of file */
		case 0:
			msg((ll_error, "encountered an end of file"));
			return (casio_error_eof);

		/* - timeout error - */
		case EINTR: /* alarm */
		case ETIMEDOUT:
# ifdef ETIME
		case ETIME:
# endif
			msg((ll_error, "timeout received"));
			return (casio_error_timeout);

		/* - device disconnected - */
		case ENODEV:
			msg((ll_fatal, "calculator was disconnected"));
			return (casio_error_nocalc);

		/* - unknown error - */
		default:
			msg((ll_fatal, "errno was %d: %s", errno, strerror(errno)));
			return (casio_error_unknown);
	}

	/* no error */
	return (0);
}

/**
 *	casio_file_seek:
 *	Seek within a file.
 *
 *	@arg	cookie		the cookie.
 *	@arg	offset		the offset.
 *	@arg	whence		the whence.
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL int casio_file_seek(file_cookie_t *cookie, casio_off_t *offset,
	casio_whence_t whence)
{
	int wh;

	/* Seek. */
	wh = whence == CASIO_SEEK_SET ? SEEK_SET
		: whence == CASIO_SEEK_CUR ? SEEK_CUR
		: SEEK_END;
	if (fseek(cookie->_rstream, (long)*offset, wh) < 0)
		return (casio_error_op); /* TODO: check errno? */

	/* Hide. */
	*offset = (casio_off_t)ftell(cookie->_rstream);
	return (0);
}

/**
 *	casio_file_close:
 *	Close a FILE cookie.
 *
 *	@arg	vcookie		the cookie.
 *	@return				the error code (0 if ok)
 */

CASIO_LOCAL int casio_file_close(file_cookie_t *cookie)
{
	if (cookie->_rstream && cookie->_rstream_cl)
		fclose(cookie->_rstream);
	if (cookie->_wstream && cookie->_wstream != cookie->_rstream
	 && cookie->_wstream_cl)
		fclose(cookie->_wstream);
	casio_free(cookie);
	return (0);
}
/* ************************************************************************* */
/*  Opening functions                                                        */
/* ************************************************************************* */
CASIO_LOCAL const casio_streamfuncs_t casio_file_callbacks =
casio_stream_callbacks_for_virtual(casio_file_close, casio_file_read,
	casio_file_write, casio_file_seek);

/**
 *	casio_open_stream_file:
 *	Open a FILE stream.
 *
 *	@arg	stream		the stream to make.
 *	@arg	rstream		the FILE stream to read from.
 *	@arg	wstream		the FILe stream to write to.
 *	@arg	rstream_cl	should we close the read stream?
 *	@arg	wstream_cl	should we close the write stream?
 *	@return				the error (0 if ok).
 */

int CASIO_EXPORT casio_open_stream_file(casio_stream_t **stream,
	FILE *rstream, FILE *wstream, int rstream_cl, int wstream_cl)
{
	file_cookie_t *cookie = NULL;
	int err; casio_openmode_t mode = 0;

	/* setup the mode. */
	if (rstream && __freadable(rstream)) mode |= CASIO_OPENMODE_READ;
	if (wstream && __fwritable(wstream)) mode |= CASIO_OPENMODE_WRITE;
	if (!mode) {
		msg((ll_error, "Neither readable or writable..."));
		err = casio_error_nostream;
		goto fail;
	}

	/* allocate the cookie */
	cookie = casio_alloc(1, sizeof(file_cookie_t));
	if (!cookie) { err = casio_error_alloc; goto fail; }

	/* fill the cookie */
	cookie->_rstream_cl = rstream_cl;
	cookie->_wstream_cl = wstream_cl;
	cookie->_rstream = rstream;
	cookie->_wstream = wstream;

	/* initialize the stream */
	return (casio_open(stream, mode, 0, cookie, &casio_file_callbacks));
fail:
	if (rstream && rstream_cl) fclose(rstream);
	if (wstream != rstream && wstream_cl) fclose(wstream);
	return (err);
}

#endif
