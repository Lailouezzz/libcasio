/* ****************************************************************************
 * stream/attrs.c -- streams management.
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
#include "stream.h"

/* ************************************************************************* */
/*  Manage attributes                                                        */
/* ************************************************************************* */
/**
 *	casio_make_attrs:
 *	Make attributes from a string.
 *
 *	@arg	attrs	the attributes to make.
 *	@arg	raw		the string.
 *	@return			the error code (0 if ok).
 */

int CASIO_EXPORT casio_make_attrs(casio_streamattrs_t *attrs, const char *raw)
{
	unsigned int speed; char par; int stop;

	/* check if we just want to initialize */
	if (!raw) {
		memset(attrs, 0, sizeof(casio_streamattrs_t)); /* ABI XXX */
		attrs->casio_streamattrs_flags =
			CASIO_TWOSTOPBITS | CASIO_DTRCTL_ENABLE | CASIO_RTSCTL_ENABLE;
		attrs->casio_streamattrs_speed = CASIO_B9600;
		return (0);
	}

	/* extract data from the string */
	if (sscanf(raw, "%u%c%d", &speed, &par, &stop) < 3
	 || (par != 'N' && par != 'E' && par != 'O')
	 || (stop != 1 && stop != 2)) {
		msg((ll_error, "Invalid format!"));
		return (casio_error_op);
	}

	/* get the speed */
	switch (speed) {
	case   1200: speed = CASIO_B1200;   break;
	case   2400: speed = CASIO_B2400;   break;
	case   4800: speed = CASIO_B4800;   break;
	case   9600: speed = CASIO_B9600;   break;
	case  19200: speed = CASIO_B19200;  break;
	case  38400: speed = CASIO_B38400;  break;
	case  57600: speed = CASIO_B57600;  break;
	case 115200: speed = CASIO_B115200; break;
	default:
		msg((ll_error, "%u isn't a valid speed!", speed));
		return (casio_error_op);
	}

	/* set the settings */
	casio_make_attrs(attrs, NULL);
	attrs->casio_streamattrs_speed = speed;
	attrs->casio_streamattrs_flags &= ~(CASIO_TWOSTOPBITS | CASIO_PARMASK);
	attrs->casio_streamattrs_flags |= --stop
		? CASIO_TWOSTOPBITS : CASIO_ONESTOPBIT;
	attrs->casio_streamattrs_flags |= par == 'N' ? CASIO_PARDIS
		: par == 'E' ? CASIO_PARENB | CASIO_PAREVEN
		: CASIO_PARENB | CASIO_PARODD;
	return (0);
}
/* ************************************************************************* */
/*  Set the attributes of a stream                                           */
/* ************************************************************************* */
/**
 *	casio_init_attrs:
 *	Initialize the attributes.
 *
 *	@arg	attrs	the settings to initialize.
 *	@return			the error.
 */

int CASIO_EXPORT casio_init_attrs(casio_stream_t *stream)
{
	int err; casio_stream_setattrs_t *s;
	casio_streamattrs_t attrs;

	if (!stream) return (casio_error_op);
	s = getcb(stream, setattrs);
	if (!s) return (casio_error_op);
	casio_make_attrs(&attrs, NULL);
	err = (*s)(stream->casio_stream_cookie, &attrs);
	if (err) return (err);

	memcpy(&stream->casio_stream_attrs, &attrs,
		sizeof(casio_streamattrs_t));
	return (0);
}

/**
 *	casio_set_attrs:
 *	Set the attributes of a libcasio stream.
 *
 *	@arg	stream		the stream to set the attributes of.
 *	@arg	attrs		the attributes to set.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_set_attrs(casio_stream_t *stream,
	const casio_streamattrs_t *attrs)
{
	int err; casio_stream_setattrs_t *s;

	/* check if the attributes aren't valid */
	if (!attrs) return (casio_error_invalid);

	/* check if the callback exists */
	if (!stream) return (casio_error_op);
	s = getcb(stream, setattrs);
	failure(!s, casio_error_op)

	/* call it */
	err = (*s)(stream->casio_stream_cookie, attrs);
	failure(err, err)

	memcpy(&stream->casio_stream_attrs, attrs, sizeof(casio_streamattrs_t));
	err = 0;
fail:
	stream->casio_stream_lasterr = err;
	return (err);
}
