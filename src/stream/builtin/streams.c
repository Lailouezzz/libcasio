/* ****************************************************************************
 * stream/builtin/streams.c -- built-in STREAMS stream.
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
#ifndef LIBCASIO_DISABLED_STREAMS
# include <stdlib.h>
# include <string.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <unistd.h>
# include <errno.h>
# include <sys/ioctl.h>
# include <termios.h>

/* the cookie type */
# define BUFSIZE 2048
typedef struct {
	int _readfd, _writefd;
	int _closeread, _closewrite;

	/* buffer [control] */
	ssize_t _start, _end;
	unsigned char _buffer[BUFSIZE];
} streams_cookie_t;
/* ************************************************************************* */
/*  Settings, close callbacks                                                */
/* ************************************************************************* */
/**
 *	setcomm_for_fd:
 *	Set communication things for a STREAMS device.
 *
 *	Some flags have the same effects as `cfmakeraw()` (see termios(3)).
 *	Other effects are linked to the settings (see `libcasio/stream.h`).
 *
 *	@arg	fd			the file descriptor.
 *	@arg	settings	the settings to set.
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL int setcomm_for_fd(const int fd,
	const casio_streamattrs_t *settings)
{
	struct termios term;
	speed_t speed;
	unsigned int status, dtrflags, rtsflags;

	/* get the speed */
	switch (settings->casio_streamattrs_speed) {
		case CASIO_B1200:   speed = B1200;   break;
		case CASIO_B2400:   speed = B2400;   break;
		case CASIO_B4800:   speed = B4800;   break;
		case CASIO_B9600:   speed = B9600;   break;
		case CASIO_B19200:  speed = B19200;  break;
		case CASIO_B38400:  speed = B38400;  break;
		case CASIO_B57600:  speed = B57600;  break;
		case CASIO_B115200: speed = B115200; break;
		default:
			msg((ll_info, "Speed was unsupported by termios: %u",
				settings->casio_streamattrs_speed));
			return (casio_error_op);
	}

	/* get the current configuration */
	if (tcgetattr(fd, &term) < 0)
		return (casio_error_unknown);

	/* set the speed */
	cfsetispeed(&term, speed);
	cfsetospeed(&term, speed);

	/* input flags */
	term.c_iflag &= ~(IGNBRK | IGNCR | BRKINT | PARMRK | ISTRIP | INLCR
		| ICRNL | IGNPAR | IXON | IXOFF);
	if (settings->casio_streamattrs_flags & CASIO_XONMASK)
		term.c_iflag |= IXON;
	if (settings->casio_streamattrs_flags & CASIO_XOFFMASK)
		term.c_iflag |= IXOFF;

	/* output flags, local modes */
	term.c_oflag = 0;
	term.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);

	/* control flags */
	term.c_cflag &= ~(PARENB | PARODD | CREAD | CSTOPB | CSIZE);
	term.c_cflag |= CREAD | CS8;
	if (settings->casio_streamattrs_flags & CASIO_TWOSTOPBITS)
		term.c_cflag |= CSTOPB;
	if (settings->casio_streamattrs_flags & CASIO_PARENB)
		term.c_cflag |= PARENB;
	if (settings->casio_streamattrs_flags & CASIO_PARODD)
		term.c_cflag |= PARODD;

	/* control characters */
	term.c_cc[VSTART] = settings->casio_streamattrs_cc[CASIO_XON];
	term.c_cc[VSTOP] =  settings->casio_streamattrs_cc[CASIO_XOFF];
	term.c_cc[VMIN] =   0; /* ? */

	/* update the termios settings! */
	if (tcsetattr(fd, TCSANOW, &term))
		return (casio_error_unknown);

	/* get line status */
	if (ioctl(fd, TIOCMGET, &status) >= 0) status = 0;
	status &= ~(TIOCM_DTR | TIOCM_RTS);

	/* activate DTR and RTS */
	dtrflags = settings->casio_streamattrs_flags & CASIO_DTRMASK;
	rtsflags = settings->casio_streamattrs_flags & CASIO_RTSMASK;
	if (dtrflags == CASIO_DTRCTL_ENABLE || dtrflags == CASIO_DTRCTL_HANDSHAKE)
		status |= TIOCM_DTR;
	if (rtsflags == CASIO_RTSCTL_ENABLE || rtsflags == CASIO_RTSCTL_HANDSHAKE)
		status |= TIOCM_RTS;
	if (ioctl(fd, TIOCMSET, &status) < 0)
		return (casio_error_unknown);

	/* no error! */
	return (0);
}

/**
 *	casio_streams_setcomm:
 *	Set the communication status.
 *
 *	@arg	vcookie		the cookie (uncasted).
 *	@arg	settings	the settings to set.
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL int casio_streams_setcomm(void *vcookie,
	const casio_streamattrs_t *settings)
{
	int err; streams_cookie_t *cookie = (streams_cookie_t*)vcookie;

	/* set attributes */
	if (cookie->_readfd >= 0) {
		err = setcomm_for_fd(cookie->_readfd, settings);
		if (err) return (err);
	}
	if (cookie->_writefd >= 0 && cookie->_writefd != cookie->_readfd) {
		err = setcomm_for_fd(cookie->_writefd, settings);
		if (err) return (err);
	}

	/* no error */
	return (0);
}

/**
 *	casio_streams_settm:
 *	Set timeouts.
 *
 *	@arg	vcookie		the cookie (uncasted).
 *	@arg	timeouts	the timeouts.
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL int casio_streams_settm(void *vcookie,
	const casio_timeouts_t *timeouts)
{
	streams_cookie_t *cookie = (void*)vcookie;
	struct termios term;

	/* set on the read thing */
	if (!tcgetattr(cookie->_readfd, &term)) {
		/* set the timeout */
		term.c_cc[VTIME] = timeouts->casio_timeouts_read / 100;

		/* update */
		if (tcsetattr(cookie->_readfd, TCSANOW, &term))
			return (0);
	}

	/* set on the write thing */
	if (cookie->_readfd != cookie->_writefd
	 && !tcgetattr(cookie->_writefd, &term)) {
		term.c_cc[VTIME] = timeouts->casio_timeouts_write / 100;

		/* update */
		if (tcsetattr(cookie->_writefd, TCSANOW, &term))
			return (0);
	}

	/* no error! */
	return (0);
}

/**
 *	casio_streams_close:
 *	Close the cookie.
 *
 *	@arg	vcookie		the cookie (uncasted).
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL int casio_streams_close(void *vcookie)
{
	streams_cookie_t *cookie = (streams_cookie_t*)vcookie;

	/* close the file descriptors. */
	if (cookie->_readfd >= 0 && cookie->_closeread)
		close(cookie->_readfd);
	if (cookie->_writefd >= 0 && cookie->_readfd != cookie->_writefd
	 && cookie->_closewrite)
		close(cookie->_writefd);

	/* free the cookie. */
	casio_free(cookie);
	return (0);
}
/* ************************************************************************* */
/*  Character stream callbacks                                               */
/* ************************************************************************* */
/**
 *	casio_streams_read:
 *	Read from a terminal.
 *
 *	@arg	vcookie		the cookie (uncasted).
 *	@arg	data		the data pointer.
 *	@arg	size		the data size.
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL int casio_streams_read(void *vcookie,
	unsigned char *dest, size_t size)
{
	streams_cookie_t *cookie = (streams_cookie_t*)vcookie;
	int fd = cookie->_readfd;

	/* transmit what's already in the buffer */
	if (cookie->_start <= cookie->_end) {
		size_t tocopy = cookie->_end - cookie->_start + 1;
		if (tocopy > size) tocopy = size;

		memcpy(dest, &cookie->_buffer[cookie->_start], tocopy);
		cookie->_start += tocopy;
		dest += tocopy;
		size -= tocopy;
	}

	/* main receiving loop */
	while (size) {
		ssize_t recv;
		size_t tocopy;

		/* receive */
		recv = read(fd, cookie->_buffer, BUFSIZE);
		if (!recv) continue;

		/* check error */
		if (recv < 0) switch (errno) {
			case 0: continue;
			case ENODEV: case EIO:
				return (casio_error_nocalc);
			default:
				msg((ll_fatal, "error was %d: %s",
					errno, strerror(errno)));
				return (casio_error_unknown);
		}

		/* get the current size to copy */
		tocopy = (size_t)recv;
		if (tocopy > size) tocopy = size;

		/* copy to destination */
		memcpy(dest, cookie->_buffer, tocopy);
		dest += tocopy;
		size -= tocopy;

		/* correct start and end points */
		cookie->_start = tocopy;
		cookie->_end = (size_t)recv - 1;
	}

	/* no error */
	return (0);
}

/**
 *	casio_streams_write:
 *	Write to a terminal.
 *
 *	@arg	vcookie		the cookie (uncasted)
 *	@arg	data		the source.
 *	@arg	size		the source size.
 *	@return				the error (0 if ok).
 */

CASIO_LOCAL int casio_streams_write(void *vcookie,
	const unsigned char *data, size_t size)
{
	streams_cookie_t *cookie = (streams_cookie_t*)vcookie;
	int fd = cookie->_writefd;

	/* send */
	while (size) {
		ssize_t wr = write(fd, data, size);
		if (wr < 0) break;
		size -= (size_t)wr;
	}

	/* be sure it's written, or check the error */
	if (size) switch (errno) {
		case ENODEV:
			return (casio_error_nocalc);
		default:
			msg((ll_fatal, "errno was %d: %s", errno, strerror(errno)));
			return (casio_error_unknown);
	}

	/* no error! */
	return (0);
}
/* ************************************************************************* */
/*  SCSI request callback                                                    */
/* ************************************************************************* */
# if 0 && defined(__linux__)
#  include <scsi/sg.h>

/**
 *	casio_streams_scsi_request:
 *	Make an SCSI request using the SG interface.
 *
 *	@arg	vcookie		the cookie (uncasted).
 *	@arg	request		the SCSI request.
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL int casio_streams_scsi_request(void *vcookie,
	casio_scsi_t *request)
{
	streams_cookie_t *cookie = (streams_cookie_t*)vcookie;

	/* correct the request */
	int err = casio_scsi_correct(request);
	if (err) return (err);

	/* make an `sg_io_hdr_t` out of the request */
	sg_io_hdr_t h = {
		/* main input data */
		.interface_id = 'S', /* magic */
		.flags = 0, /* no special things, just normal SCSI */
		.dxfer_direction = request->direction, /* FIXME: macro to macro? */
		.cmd_len = request->cmd_len, .cmdp = request->cmd, /* command */
		.timeout = 0, /* FIXME (also, in ms) */

		/* data buffer */
		.iovec_count = 0,
		.dxfer_len = request->data_len,
		.dxferp = request->data, /* data buffer */

		/* sense buffer */
		.max_sb_len = request->slen, .sbp = request->sense, /* sense buffer */
	};

	/* make the request */
	if (ioctl(cookie->_writefd, SG_IO, &h) < 0) switch (errno) {
	case EACCES:
		log_error("Root perms required!");
	default:
		log_error("Errno: %s (0x%04X)", strerror(errno), errno);
		return (casio_error_unknown);
	}

	/* everything went well */
	return (0);
}

# endif
/* ************************************************************************* */
/*  Stream initialization                                                    */
/* ************************************************************************* */
/* callbacks */
CASIO_LOCAL const casio_streamfuncs_t casio_streams_callbacks =
casio_stream_callbacks_for_serial(casio_streams_close,
	casio_streams_setcomm, casio_streams_settm,
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

	/* make up the flags */
	flags = O_NOCTTY;
	if ((mode & CASIO_OPENMODE_READ) && (mode & CASIO_OPENMODE_WRITE))
		flags |= O_RDWR;
	else if (mode & CASIO_OPENMODE_READ)
		flags |= O_RDONLY;
	else if (mode & CASIO_OPENMODE_WRITE)
		flags |= O_WRONLY;
	else return (casio_error_invalid);

	/* open the stream */
	openfd = open(path, flags);
	if (openfd < 0) switch (errno) {
		/* no such device */
		case ENODEV: case ENOENT: case ENXIO:
		case EPIPE: case ESPIPE:
			msg((ll_error, "couldn't open calculator: %s",
				strerror(errno)));
			return (casio_error_nocalc);

		/* no access */
		case EACCES:
			msg((ll_error, "permission denied"));
			return (casio_error_noaccess);

		/* default */
		default:
			msg((ll_error, "unknown error: %s (0x%X)",
				strerror(errno), errno));
			return (casio_error_unknown);
	}

	/* make the file descriptors */
	readfd  = mode & CASIO_OPENMODE_READ  ? openfd : -1;
	writefd = mode & CASIO_OPENMODE_WRITE ? openfd : -1;

	/* make the final stream */
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

	/* check if the devices are valid. */
	if ( readfd < 0 ||   read(readfd, NULL, 0) < 0)
		mode &= ~CASIO_OPENMODE_READ;
	if (writefd < 0 || write(writefd, NULL, 0) < 0)
		mode &= ~CASIO_OPENMODE_WRITE;

	/* check if we have at least read/write file descriptors. */
	if (!mode) { err = casio_error_invalid; goto fail; }

	/* allocate cookie */
	cookie = casio_alloc(1, sizeof(streams_cookie_t));
	if (!cookie) { err = casio_error_alloc; goto fail; }
	cookie->_readfd = readfd;
	cookie->_writefd = writefd;
	cookie->_closeread = closeread;
	cookie->_closewrite = closewrite;
	cookie->_start = 0;
	cookie->_end = -1;

	/* init for real */
	msg((ll_info, "Initializing STREAMS stream with fds: (%d, %d)",
		readfd, writefd));

	/* final call */
	return (casio_open(stream, mode, cookie,
		&casio_streams_callbacks));
fail:
	if (readfd >= 0 && closeread)
		close(readfd);
	if (writefd >= 0 && writefd != readfd && closewrite)
		close(writefd);
	return (err);
}

#endif
