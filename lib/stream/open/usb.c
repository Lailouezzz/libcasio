/* ****************************************************************************
 * stream/open/usb.c -- open a USB stream.
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
#include "../stream.h"
#define NUM 5

/* Correspondance type. */
struct corresp {
	int                    _valid;
	casio_openusbstream_t *_openusb;
};

/* List of streams. */
CASIO_LOCAL struct corresp openusbs[NUM + 1] = {
#if   !defined(LIBCASIO_DISABLED_LIBUSB)
	{1, casio_openusb_libusb},
#endif
#if   !defined(LIBCASIO_DISABLED_WINDOWS)
	{1, casio_openusb_windows},
#endif
	{0, NULL}
};

/**
 *	casio_add_default_usb_stream:
 *	Add a USB stream opener.
 *
 *	@arg	function	the function pointer to add.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_add_default_usb_stream(casio_openusbstream_t *function)
{
	struct corresp *c; int num;

	for (c = openusbs, num = NUM; c->_valid && num; c++, num--);
	if (!num)
		return (casio_error_op);

	c->_valid = 1;
	c->_openusb = function;
	return (0);
}

/**
 *	casio_open_usb_stream:
 *	Open the USB stream.
 *
 *	@arg	stream		the stream to make.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_open_usb_stream(casio_stream_t **stream)
{
	int err; struct corresp *c = openusbs;

	if (!c->_valid)
		return (casio_error_op);
	for (; c->_valid; c++) {
		err = (*c->_openusb)(stream);

		if (!err)
			return (0);
		if (err != casio_error_nocalc)
			return (err);
	}

	return (casio_error_nocalc);
}
