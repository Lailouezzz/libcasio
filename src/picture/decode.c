/* ****************************************************************************
 * picture/decode.c -- picture management.
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

/* ************************************************************************* */
/*  Constants                                                                */
/* ************************************************************************* */
/* The dual 2-bit format colors */
CASIO_LOCAL casio_uint32_t dual2b_colors[] = {
	0xFFFFFF,
	0xAAAAAA,
	0x777777,
	0x000000
};

/* prizm colors */
CASIO_LOCAL const casio_uint32_t prizm_colors[16] = {
	/* [casio_color_black]   = */ 0x000000,
	/* [casio_color_blue]    = */ 0x0000ff,
	/* [casio_color_green]   = */ 0x00ff00,
	/* [casio_color_cyan]    = */ 0x00ffff,
	/* [casio_color_red]     = */ 0xff0000,
	/* [casio_color_magenta] = */ 0xff00ff,
	/* [casio_color_yellow]  = */ 0xffff00,
	/* [casio_color_white]   = */ 0xffffff,
	/* RESERVED */
};

/* Colors used in Casemul pictures. */
CASIO_LOCAL const casio_uint32_t casemul_colors[256] = {
	0xFFFFFF, /* white */
	0xFF8000, /* orange */
	0x00FF00, /* green */
	0x0000FF  /* blue */
	/* other colours are black */
};
/* ************************************************************************* */
/*  Picture decoding                                                         */
/* ************************************************************************* */
/**
 *	casio_decode_picture:
 *	Decode a picture.
 *
 *	@arg	pixels		the pixels to fill.
 *	@arg	format		the format to use.
 *	@arg	raw			the raw bytes to decode from.
 *	@arg	width		the width.
 *	@arg	height		the height.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_decode_picture(casio_pixel_t **pixels,
	const void *vraw, casio_pictureformat_t format,
	unsigned int width, unsigned int height)
{
	const unsigned char *raw = (const void*)vraw;
	const unsigned char *o, *g, *b, *r2; /* pointers on the data */
	int msk, bit; size_t off; /* mask and offset */
	unsigned int y, x, bx; /* coordinates */

	switch (format) {
	case casio_pictureformat_1bit:
		for (y = 0; y < height; y++) {
			msk = 0x80;
			for (x = 0; x < width; x++) {
				/* get pixel */
				pixels[y][x] = (*raw & msk) ? 0x000000 : 0xFFFFFF;

				/* go to next */
				bit = msk & 1; raw += bit;
				msk = (msk >> 1) | (bit << 7);
			}
			if (width & 0x7) raw++;
		}
		break;

	case casio_pictureformat_1bit_r:
		for (y = 0; y < height; y++) {
			msk = 0x80;
			for (x = 0; x < width; x++) {
				/* get pixel */
				pixels[y][x] = (*raw & msk) ? 0xFFFFFF : 0x000000;

				/* go to next */
				raw += msk & 1;
				msk = (msk >> 1) | ((msk & 1) << 7);
			}
			if (width & 0x7) raw++;
		}
		break;

	case casio_pictureformat_4bit_mono:
		raw = &raw[(height * width / 2) * 2];
		/* FALLTHRU */
	case casio_pictureformat_1bit_packed:
		msk = 0x80;
		for (y = 0; y < height; y++)
		  for (x = 0; x < width; x++) {
			/* get pixel */
			pixels[y][x] = (*raw & msk) ? 0x000000 : 0xFFFFFF;

			/* go to next */
			raw += msk & 1;
			msk = (msk >> 1) | ((msk & 1) << 7);
		}
		break;

	case casio_pictureformat_1bit_packed_r:
		msk = 0x80;
		for (y = 0; y < height; y++)
		  for (x = 0; x < width; x++) {
			/* get pixel */
			pixels[y][x] = (*raw & msk) ? 0xFFFFFF : 0x000000;

			/* go to next */
			raw += msk & 1;
			msk = (msk >> 1) | ((msk & 1) << 7);
		}
		break;

	case casio_pictureformat_1bit_old:
		for (bx = width - 8; bx != (unsigned int)-8; bx -= 8)
		  for (y = height - 1; y != (unsigned int)-1; y--) {
			msk = 0x80;
			for (x = bx; x < bx + 8; x++) {
				/* get pixel */
				pixels[y][x] = (*raw & msk) ? 0x000000 : 0xFFFFFF;

				/* go to next */
				msk >>= 1;
			}

			/* go to next byte */
			raw++;
		}
		break;

	case casio_pictureformat_2bit_dual:
		msk = 0x80; r2 = &raw[height * width / 8];
		for (y = 0; y < height; y++)
		  for (x = 0; x < width; x++) {
			/* get pixel */
			casio_uint32_t val = (!!(*raw & msk) << 1) | !!(*r2 & msk);
			pixels[y][x] = dual2b_colors[val];

			/* go to next */
			raw += msk & 1; r2 += msk & 1;
			msk = (msk >> 1) | ((msk & 1) << 7);
		}
		break;

	case casio_pictureformat_4bit_code:
		msk = 0xF0;
		for (y = 0; y < height; y++)
		  for (x = 0; x < width; x++) {
			casio_uint32_t px = *raw & msk;

			/* get pixel */
			px = px | (px >> 4);
			pixels[y][x] = prizm_colors[px & 0x0F];

			/* go to next */
			raw += msk & 1;
			msk = ~msk & 0xFF;
		}
		break;

	case casio_pictureformat_4bit_rgb:
		msk = 0xF0;
		for (y = height - 1; y != (unsigned int)-1; y--)
		  for (x = width - 1; x != (unsigned int)-1; x--) {
			casio_uint32_t val = *raw & msk;
			casio_uint32_t px = 0;

			/* get pixel */
			val |= val >> 4;
			if (px & 8) px |= 0xFF0000;
			if (px & 4) px |= 0x00FF00;
			if (px & 2) px |= 0x0000FF;
			pixels[y][x] = px;

			/* go to next */
			raw += msk & 1;
			msk = ~msk & 0xFF;
		}
		break;

	case casio_pictureformat_4bit_color:
		off = height * width / 8; o = raw; g = &raw[off]; b = &raw[off * 2];
		for (bx = width - 8; bx != (unsigned int)-8; bx -= 8)
		  for (y = height - 1; y != (unsigned int)-1; y--) {
			msk = 0x80;
			for (x = bx; x < bx + 8; x++) {
				/* get pixel */
				if      (*o & msk) pixels[y][x] = 0xFF8C00; /* orange */
				else if (*g & msk) pixels[y][x] = 0x00FF00; /* green */
				else if (*b & msk) pixels[y][x] = 0x0000FF; /* blue */
				else               pixels[y][x] = 0xFFFFFF; /* white */

				/* go to next */
				msk >>= 1;
			}

			/* go to next byte */
			o++; g++; b++;
		}
		break;

	case casio_pictureformat_casemul:
		for (y = 0; y < height; y++) for (x = 0; x < width; x++)
			pixels[y][x] = casemul_colors[*raw++];
		break;

	case casio_pictureformat_16bit:
		for (y = 0; y < height; y++)
		  for (x = 0; x < width; x++) {
			/* get pixel */
			casio_uint32_t one = raw[0], two = raw[1];
			casio_uint32_t px = (one >> 3) << (16 + 3);
			px |= (((one & 7) << 3) | (two >> 5)) << (8 + 2);
			px |= (two & 31) << 3;
			pixels[y][x] = px;

			/* go to next */
			raw += 2;
		}
		break;

	default:
		/* unknown format! */
		return (casio_error_op);
	}

	/* everything went well :) */
	return (0);
}
