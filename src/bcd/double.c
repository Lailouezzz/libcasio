/* ****************************************************************************
 * bcd/double.c -- make a libcasio BCD out of a double.
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
#include "../internals.h"
#include <math.h>

/**
 *	casio_bcd_fromdouble:
 *	Make a libcasio BCD number out of a C-double.
 *
 *	@arg	bcd		the BCD to make.
 *	@arg	dbl		the source double.
 */

void CASIO_EXPORT casio_bcd_fromdouble(casio_bcd_t *bcd, double dbl)
{
	int neg = 0, exp = 1, i;

	/* check if is negative */
	if (dbl < 0) {
		neg = 1;
		dbl = -dbl;
	}

	/* check the exponent, normalize the number */
	while (dbl >= 10) {
		exp++;
		dbl /= 10;
	}
	if (dbl > 0) while (dbl < 1) {
		exp--;
		dbl *= 10;
	}

	/* get the mantissa */
	for (i = 0; i < 15; i++) {
		double work = floor(dbl);
		bcd->casio_bcd_mant[i] = (int)work;
		dbl = (dbl - work) * 10;
	}

	/* set the flags */
	bcd->casio_bcd_flags = casio_make_bcdflags(0, neg, 15);
	bcd->casio_bcd_exp = exp;
}

/**
 *	casio_bcd_todouble:
 *	Make a C-double out of a libcasio BCD number.
 *
 *	@arg	bcd		the BCD to convert.
 *	@return			the double.
 */

double CASIO_EXPORT casio_bcd_todouble(const casio_bcd_t *bcd)
{
	/* TODO */
	(void)bcd;
	return (0);
}
