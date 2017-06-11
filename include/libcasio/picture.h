/* ****************************************************************************
 * libcasio/picture.h -- picture formats managed by libcasio.
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
#ifndef  LIBCASIO_PICTURE_H
# define LIBCASIO_PICTURE_H
# include <libcasio/cdefs.h>
CASIO_BEGIN_NAMESPACE

/* This file describes the picture formats CASIO has used for various purposes
 * (screenshots, pictures, screenstreaming), and that libcasio is able to
 * decode.
 *
 * libcasio decodes from, encodes to, and converts using 32-bit pixels, which
 * are basically 0x00RRGGBB. */

typedef casio_uint32_t casio_pixel_t;

/* The format pictures this file describes fit in a `casio_pictureformat_t`.
 * The (arbitrary) codes for them is more or less 0xBBVS, where 'BB' is the
 * number of bits each pixel occupy (in total), 'V' is the variation for
 * this number of bits, and 'S' is the special hex digit (e.g. reverse).
 *
 * Do not use the format described above other than to define your own
 * formats, as it might change! */

typedef enum casio_pictureformat_s {
/* ************************************************************************* */
/*  Monochrome pictures with fill bits                                       */
/* ************************************************************************* */
/* In this format, each bit represents a pixel (so one byte contains eight
 * pixels). If the width is not divisible by eight, then the last bits of the
 * last byte of the line are unused (fill bits), and the next line starts at
 * the beginning of the next byte ; this makes the navigation between lines
 * easier, but takes up more space.
 *
 * An off bit (0b0) represents a white pixel, and an on bit (0b1) represents
 * a black pixel. Reverse white and black in the `_r` special type. */

	casio_pictureformat_1bit   = 0x0100,
	casio_pictureformat_1bit_r = 0x0101,

/* To calculate the size, it's simple: just calculate the number of bytes
 * a line occupies, then multiply it by the number of lines. */
/* ************************************************************************* */
/*  Packed monochrome pictures                                               */
/* ************************************************************************* */
/* Packed monochrome pictures is basically the same than the previous format,
 * except there are no fill bits: if a picture width is 6 pixels, then the
 * second line will start at the seventh bit of the first byte (where it would
 * start at the first bit of the second byte with fill bits).
 *
 * The navigation to a line is less easy as it takes at least one division. */

	casio_pictureformat_1bit_packed   = 0x0110,
	casio_pictureformat_1bit_packed_r = 0x0111,

/* To calculate the size, find out the number of occupied bits, divide by
 * eight to get the bytes, and make sure to keep an extra byte if there are
 * left bits. */
/* ************************************************************************* */
/*  Old monochrome format                                                    */
/* ************************************************************************* */
/* The old monochrome format used by CASIO is basically a normal monochrome
 * format (the width is usually 96 or 128, so no need for fill bits), except
 * that it starts with the last byte (where the bits are in left to right
 * order), but then it goes from right to left, and from bottom to top. */

	casio_pictureformat_1bit_old = 0x0120,

/* The size is the same as for normal 1-bit monochrome pictures, only the
 * byte order changes. */
/* ************************************************************************* */
/*  Dual monochrome format                                                   */
/* ************************************************************************* */
/* This is the format used for the Prizm's projector mode. It is composed of
 * two monochrome pictures (with sizes divisible by eight).
 * It is basically gray pictures, with white, light gray, dark gray and
 * black. */

	casio_pictureformat_2bit_dual = 0x0200,

/* To calculate the size, well, we just have two monochrome screens. */
/* ************************************************************************* */
/*  4-bit RGB_ format                                                        */
/* ************************************************************************* */
/* This is a 4 bit per pixel format. There is no need for fill nibbles.
 * Each nibble (group of 4 bits) is made of the following:
 * - one bit for red   (OR by 0xFF0000);
 * - one bit for green (OR by 0x00FF00);
 * - one bit for blue  (OR by 0x0000FF);
 * - one alignment bit. */

	casio_pictureformat_4bit     = 0x0400,
	casio_pictureformat_4bit_rgb = 0x0400,

/* Calculating the size is trivial: just divide the number of pixels by two. */
/* ************************************************************************* */
/*  4-bit code format                                                        */
/* ************************************************************************* */
/* In this encoding, each nibble for a pixel represents one of these colors: */

# define casio_color_black    0x0 /* 0x000000 */
# define casio_color_blue     0x1 /* 0x0000FF */
# define casio_color_green    0x2 /* 0x00FF00 */
# define casio_color_cyan     0x3 /* 0x00FFFF */
# define casio_color_red      0x4 /* 0xFF0000 */
# define casio_color_magenta  0x5 /* 0xFF00FF */
# define casio_color_yellow   0x6 /* 0xFFFF00 */
# define casio_color_white    0x7 /* 0xFFFFFF */

/* Here is the encoding code: */

	casio_pictureformat_4bit_code = 0x0410,

/* The size is calculated the same way as previously. */
/* ************************************************************************* */
/*  Quad-monochrome VRAM formats                                             */
/* ************************************************************************* */
/* This format is used by old CASIO models. It is made of four monochrome
 * pictures (no need for fill bits), where the palettes are:
 * - for the `color` variant: [orange, green, blue, white (bg)]
 * - for the `mono` variant:  [(unused), (unused), black, white (bg)] */

	casio_pictureformat_4bit_color = 0x0420,
	casio_pictureformat_4bit_mono  = 0x0421,

/* To get the size, just multiply the size of a VRAM by four. */
/* ************************************************************************* */
/*  Casemul format                                                           */
/* ************************************************************************* */
/* This format is used by Casemul.
 * It is basically arbitrary color codes, where, for example, 1 is orange.
 * You can check the full color codes in `src/picture.c`. */

	casio_pictureformat_casemul = 0x0800,

/* Each pixel takes one byte. */
/* ************************************************************************* */
/*  16-bit R5G6B5 format                                                     */
/* ************************************************************************* */
/* This is the format of the Prizm's VRAM. Each pixel is two bytes long,
 * the first five bits represent the high five (clap!) bits of the red part,
 * the next six bits represent the high six bits of the green part,
 * the last five bits represent the high five (re-clap!) bits of the blue
 * part. */

	casio_pictureformat_16bit = 0x1000
} casio_pictureformat_t;

/* Two bytes per pixel. */
/* ************************************************************************* */
/*  Utilities                                                                */
/* ************************************************************************* */
/* Here are the functions to decode/encode pictures using this format: */

CASIO_BEGIN_DECLS

extern size_t casio_get_picture_size OF((casio_pixel_t **casio__pixels,
	casio_pictureformat_t casio__format,
	unsigned int casio__width, unsigned int casio__height));

extern int casio_decode_picture OF((casio_pixel_t **casio__pixels,
	const void *casio__raw, casio_pictureformat_t casio__format,
	unsigned int casio__width, unsigned casio__height));

extern int casio_encode_picture OF((void *casio__raw,
	const casio_pixel_t **casio__pixels, casio_pictureformat_t casio__format,
	unsigned int casio__width, unsigned casio__height));

CASIO_END_DECLS
CASIO_END_NAMESPACE
#endif /* LIBCASIO_PICTURE_H */
