/* ****************************************************************************
 * bcd/mcs.c -- make a libcasio BCD out of an MCS BCD number.
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
#define upr(N) (bytes[(N)] >> 4)
#define lwr(N) (bytes[(N)] & 15)

/**
 *	casio_bcd_frommcs:
 *	Make a libcasio BCD out of an MCS BCD number.
 *
 *	@arg	bcd		the not raw BCD.
 *	@arg	raw		the raw BCD.
 *	@return			the special bit, for simplicity.
 */

int casio_bcd_frommcs(casio_bcd_t *bcd, const casio_mcsbcd_t *raw)
{
	const unsigned char *bytes = raw->casio_mcsbcd_BCDval;

	/* get the exponent and correct it */
	int exp = (upr(0) & 7) * 100 + lwr(0) * 10 + upr(1), neg = 0;
	if (exp >= 500) exp -= 500, neg = 1;
	bcd->casio_bcd_exp = (char)(exp - 99);

	/* set the flags */
	bcd->casio_bcd_flags =
		casio_make_bcdflags((bytes[0] & 0x80) >> 7, neg, 15);

	/* get the mantissa */
	bcd->casio_bcd_mant[0]  = lwr(1);
	bcd->casio_bcd_mant[1]  = upr(2);
	bcd->casio_bcd_mant[2]  = lwr(2);
	bcd->casio_bcd_mant[3]  = upr(3);
	bcd->casio_bcd_mant[4]  = lwr(3);
	bcd->casio_bcd_mant[5]  = upr(4);
	bcd->casio_bcd_mant[6]  = lwr(4);
	bcd->casio_bcd_mant[7]  = upr(5);
	bcd->casio_bcd_mant[8]  = lwr(5);
	bcd->casio_bcd_mant[9]  = upr(6);
	bcd->casio_bcd_mant[10] = lwr(6);
	bcd->casio_bcd_mant[11] = upr(7);
	bcd->casio_bcd_mant[12] = lwr(7);
	bcd->casio_bcd_mant[13] = upr(8);
	bcd->casio_bcd_mant[14] = lwr(8);

	/* return the special bit */
	return (casio_bcd_has_special(bcd));
}

/**
 *	casio_bcd_tomcs:
 *	Make an MCS BCD number out of a libcasio BCD number.
 *
 *	@arg	raw		the raw BCD.
 *	@arg	bcd		the not raw BCD.
 *	@return			the special bit.
 */

# define BCDM casio_bcd_mant
int casio_bcd_tomcs(casio_mcsbcd_t *raw, const casio_bcd_t *bcd)
{
	int exp;
	div_t c, d;

	exp = bcd->casio_bcd_exp + 99;
	if (casio_bcd_is_negative(bcd)) exp += 500;

	/* put the exponent (and first digit of the mantissa) */
	c = div(exp, 100);
	d = div(c.rem, 10);
	raw->casio_mcsbcd_BCDval[0] = (c.quot << 4) | d.quot
		| (!!casio_bcd_has_special(bcd) << 7);
	raw->casio_mcsbcd_BCDval[1] = (d.rem << 4) | bcd->casio_bcd_mant[0];

	/* put the mantissa */
	raw->casio_mcsbcd_BCDval[2] = (bcd->BCDM[1]  << 4) | bcd->BCDM[2];
	raw->casio_mcsbcd_BCDval[3] = (bcd->BCDM[3]  << 4) | bcd->BCDM[4];
	raw->casio_mcsbcd_BCDval[4] = (bcd->BCDM[5]  << 4) | bcd->BCDM[6];
	raw->casio_mcsbcd_BCDval[5] = (bcd->BCDM[7]  << 4) | bcd->BCDM[8];
	raw->casio_mcsbcd_BCDval[6] = (bcd->BCDM[9]  << 4) | bcd->BCDM[10];
	raw->casio_mcsbcd_BCDval[7] = (bcd->BCDM[11] << 4) | bcd->BCDM[12];
	raw->casio_mcsbcd_BCDval[8] = (bcd->BCDM[13] << 4) | bcd->BCDM[14];

	/* set the align zone */
	raw->casio_mcsbcd__align[0] = 0;
	raw->casio_mcsbcd__align[1] = 0;
	raw->casio_mcsbcd__align[2] = 0;

	/* return the special bit, hugh. */
	return (casio_bcd_has_special(bcd));
}
