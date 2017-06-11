/* ****************************************************************************
 * bcd/cas.c -- make a libcasio BCD out of an CAS BCD number.
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
#define upr(N) ((bytes[(N)] & 0xf0) >> 4)
#define lwr(N) (bytes[(N)] & 0xf)

/**
 *	casio_bcd_fromcas:
 *	Make a libcasio BCD out of an CAS BCD number.
 *
 *	@arg	bcd		the not raw BCD.
 *	@arg	raw		the raw BCD.
 *	@return			the special bit, for simplicity.
 */

int casio_bcd_fromcas(casio_bcd_t *bcd, const casio_casbcd_t *raw)
{
	const unsigned char *bytes = raw->casio_casbcd_mant;

	/* get the exponent and sign */
	bcd->casio_bcd_exp = (raw->casio_casbcd_exp >> 4) * 10
		+ (raw->casio_casbcd_exp & 0x0F);
	if (raw->casio_casbcd_flags & casio_casbcdflag_pow_neg)
		bcd->casio_bcd_exp = -bcd->casio_bcd_exp;

	/* get the mantissa */
	bcd->casio_bcd_mant[0]  = lwr(0);
	bcd->casio_bcd_mant[1]  = upr(1);
	bcd->casio_bcd_mant[2]  = lwr(1);
	bcd->casio_bcd_mant[3]  = upr(2);
	bcd->casio_bcd_mant[4]  = lwr(2);
	bcd->casio_bcd_mant[5]  = upr(3);
	bcd->casio_bcd_mant[6]  = lwr(3);
	bcd->casio_bcd_mant[7]  = upr(4);
	bcd->casio_bcd_mant[8]  = lwr(4);
	bcd->casio_bcd_mant[9]  = upr(5);
	bcd->casio_bcd_mant[10] = lwr(5);
	bcd->casio_bcd_mant[11] = upr(6);
	bcd->casio_bcd_mant[12] = lwr(6);
	bcd->casio_bcd_mant[13] = upr(7);
	bcd->casio_bcd_mant[14] = lwr(7);

	/* set the special things */
	bcd->casio_bcd_flags = casio_make_bcdflags(
		!!(raw->casio_casbcd_flags & casio_casbcdflag_special),
		(raw->casio_casbcd_flags & casio_casbcdflag_negative)
		== casio_casbcdflag_negative, 15);

	/* end, hugh. */
	return (casio_bcd_has_special(bcd));
}

/**
 *	casio_bcd_tocas:
 *	Make an CAS BCD number out of a libcasio BCD number.
 *
 *	@arg	raw		the raw BCD.
 *	@arg	bcd		the not raw BCD.
 *	@return			the special bit, for simplicity.
 */

# define BCDM casio_bcd_mant
int casio_bcd_tocas(casio_casbcd_t *raw, const casio_bcd_t *bcd)
{
	/* put the exponent */
	div_t d = div(abs(bcd->casio_bcd_exp), 10);
	raw->casio_casbcd_exp = (d.quot << 4) | d.rem;

	/* put the mantissa */
	raw->casio_casbcd_mant[0] =  bcd->BCDM[0];
	raw->casio_casbcd_mant[1] = (bcd->BCDM[1]  << 4) | bcd->BCDM[2];
	raw->casio_casbcd_mant[2] = (bcd->BCDM[3]  << 4) | bcd->BCDM[4];
	raw->casio_casbcd_mant[3] = (bcd->BCDM[5]  << 4) | bcd->BCDM[6];
	raw->casio_casbcd_mant[4] = (bcd->BCDM[7]  << 4) | bcd->BCDM[8];
	raw->casio_casbcd_mant[5] = (bcd->BCDM[9]  << 4) | bcd->BCDM[10];
	raw->casio_casbcd_mant[6] = (bcd->BCDM[11] << 4) | bcd->BCDM[12];
	raw->casio_casbcd_mant[7] = (bcd->BCDM[13] << 4) | bcd->BCDM[14];

	/* put the flags */
	raw->casio_casbcd_flags = 0;
	if (bcd->casio_bcd_exp < 0)
		raw->casio_casbcd_flags |= casio_casbcdflag_pow_neg;
	if (casio_bcd_is_negative(bcd))
		raw->casio_casbcd_flags |= casio_casbcdflag_negative;
	if (casio_bcd_has_special(bcd))
		raw->casio_casbcd_flags |= casio_casbcdflag_special;

	/* end, hugh. */
	return (casio_bcd_has_special(bcd));
}
