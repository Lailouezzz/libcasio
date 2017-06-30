/* ****************************************************************************
 * stream/builtin/streams/setattrs.c -- STREAMS attributes.
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

/**
 *	set_attrs_for_fd:
 *	Set communication things for a STREAMS device.
 *
 *	Some flags have the same effects as `cfmakeraw()` (see termios(3)).
 *	Other effects are linked to the settings (see `libcasio/stream.h`).
 *
 *	@arg	fd			the file descriptor.
 *	@arg	settings	the settings to set.
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL int set_attrs_for_fd(const int fd,
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
 *	casio_streams_setattrs:
 *	Set the communication status.
 *
 *	@arg	cookie		the cookie.
 *	@arg	settings	the settings to set.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_streams_setattrs(streams_cookie_t *cookie,
	const casio_streamattrs_t *settings)
{
	int err;

	/* set attributes */
	if (cookie->_readfd >= 0) {
		err = set_attrs_for_fd(cookie->_readfd, settings);
		if (err) return (err);
	}
	if (cookie->_writefd >= 0 && cookie->_writefd != cookie->_readfd) {
		err = set_attrs_for_fd(cookie->_writefd, settings);
		if (err) return (err);
	}

	/* no error */
	return (0);
}

#endif
