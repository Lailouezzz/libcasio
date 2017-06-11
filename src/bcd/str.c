/* ****************************************************************************
 * bcd/str.c -- make a string out of a libcasio BCD.
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
 *
 * These functions are experimental!
 * I wrote most of this while comparing the results to what I wanted.
 * If you think you can write a clean equivalent of this, while understanding
 * the theory behind what you do, please try! :)
 * ************************************************************************* */
#include "../internals.h"

/**
 *	getsize:
 *	Get the size.
 *
 *	@arg	exp		the exponent.
 *	@arg	neg		whether the number is negative or not.
 *	@arg	offset	the offset.
 *	@arg	digits	the number of digits.
 *	@return			the size.
 */

static size_t getsize(int exp, int neg, int offset, int digits)
{
	size_t sz = digits;
	int eexp;

	if (neg)
		sz++;
	if (offset + digits < 0)
		sz += -digits - offset; /* zeroes */
	if (offset + digits > 0)
		sz++; /* point */
	if (offset >= 0)
		sz += 1 /* leading zero */ + offset;
	if (exp) {
		sz++; /* e */
		if (exp < 0) sz += 1;
		eexp = abs(exp);
		if (eexp >= 100) sz += 3;
		else if (eexp >= 10) sz += 2;
		else sz += 1;
	}
	return (sz);
}

/**
 *	casio_bcdtoa:
 *	Make a string out of a BCD number.
 *
 *	If the buffer is not big enough, the number will not be written down.
 *	Will terminate the buffer if the string is written.
 *
 *	@arg	buf		the buffer.
 *	@arg	len		the length.
 *	@arg	bcd		the BCD.
 *	@return			the required length if needed, otherwise, zero.
 */

size_t casio_bcdtoa(char *buf, size_t len, const casio_bcd_t *bcd)
{
	int exp, offset, leftdigits, neg, acceptable_alt, origdigits;
	const char *digits; size_t sz;

	/* get base digit */
	exp = casio_bcd_exponent(bcd); offset = 0;
	leftdigits = casio_bcd_precision(bcd);
	digits = bcd->casio_bcd_mant;
	for (; leftdigits > 0; leftdigits--) {
		if (*digits) break;
		exp--;
		digits++;
	}

	/* get number of digits, check if zero */
	for (; leftdigits && !digits[leftdigits - 1]; leftdigits--);
	if (!leftdigits) {
		int l = (len >= 2) ? 0 : 2;
		memcpy(buf, "0", 2 - l);
		return (len);
	}

	/* try to play with the settings to make a more acceptable output */
	neg = !!casio_bcd_is_negative(bcd);
	acceptable_alt = max(0, 5 - leftdigits);
	if (-acceptable_alt < exp && exp <= 5) {
		offset -= exp;
		exp = 0;
	}
	if (offset >= 0) {
		exp -= offset + 1;
		offset = -1;
	}

	/* calculate the size */
	sz = getsize(exp, neg, offset, leftdigits);

	/* check if there's enough space in the buffer */
	if (sz > len) return (sz);

	/* then fill */
	origdigits = leftdigits;
	if (neg)
		*buf++ = '-';
	for (; leftdigits && offset < 0; offset++, leftdigits--)
		*buf++ = *digits++ + '0';
	for (; offset < 0; offset++)
		*buf++ = '0';
	if (leftdigits == origdigits)
		*buf++ = '0';
	if (offset > 0 || leftdigits)
		*buf++ = '.';
	for (; offset > 0; offset--)
		*buf++ = '0';
	while (leftdigits--)
		*buf++ = *digits++ + '0';
	if (exp) {
		*buf++ = 'e';
		if (exp < 0) { *buf++ = '-'; exp = -exp; }
		if (exp >= 100) *buf++ = exp / 100 + '0';
		if (exp >= 10) *buf++ = (exp % 100) / 10 + '0';
		*buf++ = exp % 10 + '0';
	}
	*buf = 0;

	/* it's done! */
	return (0);
}
