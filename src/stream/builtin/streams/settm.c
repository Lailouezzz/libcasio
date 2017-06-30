/* ****************************************************************************
 * stream/builtin/streams/settm.c -- set timeouts for a STREAMS stream.
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
 *	casio_streams_settm:
 *	Set timeouts.
 *
 *	@arg	cookie		the cookie.
 *	@arg	timeouts	the timeouts.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_streams_settm(streams_cookie_t *cookie,
	const casio_timeouts_t *timeouts)
{
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

#endif
