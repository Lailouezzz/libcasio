/* ****************************************************************************
 * link/seven/encoderaw.c -- encode and decode raw data.
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
#include "../link.h"

/**
 *	casio_seven_encoderaw:
 *	Encode raw data.
 *
 *	The fxReverse project documentation says that bytes lesser or equal to
 *	0x1F and 0x5C ('\') must be preceded by a 0x5C character.
 *	Moreover, bytes lesser or equal to 0x1F must be offset by 0x20.
 *
 *	@arg	vdest	the destination buffer (encoded data).
 *	@arg	vraw	the original buffer (raw data).
 *	@arg	size	the size of the data in the original buffer.
 *	@return			the size of the encoded data.
 */

unsigned int casio_seven_encoderaw(void *vdest, const void *vraw,
	unsigned int size)
{
	unsigned char       *dest = (void*)vdest;
	const unsigned char *raw  = (void*)vraw;
	unsigned int         destsize = size;

	while (size--) {
		int c = *raw++;

		/* special values management */
		if (c < 0x20 || c == '\\') {
			*dest++ = '\\';
			destsize++;
			if (c < 0x20) c += 0x20;
		}

		/* normal value management */
		*dest++ = (unsigned char)c;
	}

	return (destsize);
}

/**
 *	casio_seven_decoderaw:
 *	Decode encoded data.
 *
 *	This function does the opposite of the previous function: it copies data,
 *	and in case of a 0x5C ('\') character, copies what's next and if it is
 *	not the 0x5C character, it removes the 0x20 offset.
 *
 *	(because yeah, even if it's better, CASIO wouldn't use 0x5C7C for '\\')
 *
 *	@arg	vdest	the destination buffer (raw data).
 *	@arg	venc	the original buffer (encoded data).
 *	@arg	size	the encoded data size.
 *	@return			the raw data size.
 */

unsigned int casio_seven_decoderaw(void *vdest, const void *venc,
	unsigned int size)
{
	unsigned char       *dest = (void*)vdest;
	const unsigned char *enc  = (void*)venc;
	unsigned int         rawsize = size;

	while (size--) {
		int c = *enc++;

		/* special value management */
		if (c == '\\') {
			c = *enc++;
			rawsize--;
			size--;
			if (c != '\\')
				c -= 0x20;
		}

		/* normal value management */
		*dest++ = (unsigned char)c;
	}

	return (rawsize);
}
