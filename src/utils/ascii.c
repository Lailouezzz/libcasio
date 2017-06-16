/* ****************************************************************************
 * utils/ascii.c -- ASCII/hex/dec utilities.
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
#include "../internals.h"
#include <ctype.h>

/* ************************************************************************* */
/*  ASCII-HEX utilities                                                      */
/* ************************************************************************* */
/**
 *	casio_putascii:
 *	Put a number in ASCII-hex, in a n-dimensionned field.
 *
 *	@arg	p		pointer where to put ASCII number
 *	@arg	i		ASCII number
 *	@arg	n		the size of the field
 */

void CASIO_EXPORT casio_putascii(unsigned char *p, unsigned long i, int n)
{
	p += (n - 1);
	while (n--) {
		int j = i % 16;
		*p-- = j >= 10 ? j - 10 + 'A' : j + '0';
		i /= 16;
	}
}

/**
 *	casio_getascii:
 *	Gets a number in ASCII-hex, in a n-dimensionned field.
 *
 *	@arg	p		pointer where the ASCII number is
 *	@arg	n		the size of the field
 *	@return			the number
 */

unsigned long CASIO_EXPORT casio_getascii(const unsigned char *p, int n)
{
	unsigned long i = 0;

	while (n--) {
		unsigned long j = *p++;
		j = isdigit(j) ? j - '0' : j + 10 - 'A';
		i = i * 16 + j;
	}
	return (i);
}
/* ************************************************************************* */
/*  BIN-HEX to BIN-DEC (BCD) utilities                                       */
/* ************************************************************************* */
/**
 *	casio_getdec:
 *	Get decimal of hex.
 *
 *	@arg	h		the hex number.
 *	@return			the decimal number.
 */

unsigned long CASIO_EXPORT casio_getdec(unsigned long h)
{
	return ((h & 15) + ((h >> 4) & 15) * 10
		+ ((h >> 8) & 15) * 100 + ((h >> 12) & 15) * 1000
		+ ((h >> 16) & 15) * 10000 + ((h >> 20) & 15) * 1000000
		+ ((h >> 24 & 15) * 10000000 + ((h >> 28) & 15) * 100000000));
}

/**
 *	casio_gethex:
 *	Get hex of decimal.
 *
 *	@arg	d		the decimal number.
 *	@return			the hex number.
 */

unsigned long CASIO_EXPORT casio_gethex(unsigned long d)
{
	return ((d % 10) + ((d / 10 % 10) << 4)
		+ ((d / 100 % 10) << 8) + ((d / 1000 % 10) << 12)
		+ ((d / 10000 % 10) << 16) + ((d / 100000 % 10) << 20)
		+ ((d / 1000000) << 24) + ((d / 10000000) << 28));
}
