/* ****************************************************************************
 * picture/size.c -- get picture size.
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
#include "picture.h"

/**
 *	casio_get_picture_size:
 *	Get a 'raw' picture size.
 *
 *	@arg	pixels	the picture.
 *	@arg	fmt		the picture format.
 *	@arg	width	the picture width.
 *	@arg	height	the picture height.
 *	@return			the raw size.
 */

size_t casio_get_picture_size(casio_pixel_t **pixels,
	casio_pictureformat_t fmt, unsigned int width, unsigned int height)
{
	(void)pixels;

	switch (fmt) {
	case casio_pictureformat_1bit: /* FALLTHRU */
	case casio_pictureformat_1bit_r: /* FALLTHRU */
	case casio_pictureformat_1bit_old:
		return (((width / 8) + !!(width % 8)) * height);

	case casio_pictureformat_1bit_packed: /* FALLTHRU */
	case casio_pictureformat_1bit_packed_r:
		return ((width * height / 8) + !!(width * height % 8));

	case casio_pictureformat_2bit_dual:
		return (((width / 8) + !!(width % 8)) * height * 2);

	case casio_pictureformat_4bit: /* FALLTHRU */
	case casio_pictureformat_4bit_code:
		return (width * height / 2);

	case casio_pictureformat_4bit_color: /* FALLTHRU */
	case casio_pictureformat_4bit_mono:
		return (((width / 8) + !!(width % 8)) * height * 4);

	case casio_pictureformat_casemul:
		return (width * height);

	case casio_pictureformat_16bit:
		return (width * height * 2);
	}

	return (0);
}
