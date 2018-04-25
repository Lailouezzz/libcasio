/* ****************************************************************************
 * picture/encode.c -- picture management.
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
 *	casio_encode_picture:
 *	Encode the picture.
 *
 *	@arg	pixels		the pixels to encode.
 *	@arg	format		the format.
 *	@arg	raw			the destination buffer.
 *	@arg	width		the picture width.
 *	@arg	height		the picture height.
 */

int CASIO_EXPORT casio_encode_picture(void *vraw,
	const casio_pixel_t **pixels, casio_pictureformat_t format,
	unsigned int width, unsigned int height)
{
	int msk;
	unsigned int y, x;
	unsigned char *raw = (void*)vraw;

	switch (format) {
	case casio_pictureformat_1bit:
		for (y = 0; y < height; y++) {
			msk = 0x80; *raw = 0;
			for (x = 0; x < width; x++) {
				/* get pixel */
				if (!pixels[y][x]) *raw |= msk;

				/* go to next */
				raw += msk & 1;
				msk = (msk >> 1) | ((msk & 1) << 7);
				*raw &= ~(msk - 1);
			}
			if (width & 0x7) raw++;
		}
		break;

	case casio_pictureformat_1bit_r:
		for (y = 0; y < height; y++) {
			msk = 0x80; *raw = 0;
			for (x = 0; x < width; x++) {
				/* get pixel */
				if (pixels[y][x]) *raw |= msk;

				/* go to next */
				raw += msk & 1;
				msk = (msk >> 1) | ((msk & 1) << 7);
				*raw &= ~(msk - 1);
			}
			if (width & 0x7) raw++;
		}
		break;

	case casio_pictureformat_1bit_packed:
		msk = 0x80;
		for (y = 0; y < height; y++)
		  for (x = 0; x < width; x++) {
			/* set pixel */
			if (pixels[y][x] & 0xFFFFFF) *raw = msk;

			/* go to next */
			raw += msk & 1;
			msk = (msk >> 1) | ((msk & 1) << 7);
		}
		break;

	case casio_pictureformat_4bit_code:
		msk = 0xF0;
		for (y = 0; y < height; y++) for (x = 0; x < width; x++) {
			/* set pixel */
			casio_uint32_t color = 0;
			color |= !!(0xFF0000 & pixels[y][x]) << 2;
			color |= !!(0x00FF00 & pixels[y][x]) << 1;
			color |= !!(0x0000FF & pixels[y][x]);
			*raw |= color << (msk & 4);

			/* go to next */
			raw += msk & 1;
			msk = ~msk & 0xFF;
		}
		break;

	case casio_pictureformat_16bit:
		for (y = 0; y < height; y++) for (x = 0; x < width; x++) {
			/* set pixel */
			casio_uint32_t r = (pixels[y][x] & (0x1F << 19)) >> 19;
			casio_uint32_t g = (pixels[y][x] & (0x3F << 10)) >> 10;
			casio_uint32_t b = (pixels[y][x] & (0x1F <<  3)) >>  3;
			*raw++ = (r << 3) | (g >> 5);
			*raw++ = (g << 5) | b;
		}
		break;

	default:
		/* unknown format! */
		return (casio_error_op);
	}

	/* everything went well :) */
	return (0);
}
