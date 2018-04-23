/* ****************************************************************************
 * libcasio/number.h -- BCD-encoded numbers manipulation.
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
 * BCD are the main number format CASIO uses in its calculators.
 * This format has the huge advantage to make 0.1 + 0.2 and 0.3 equal.
 *
 * There are several raw BCD formats, libcasio provides conversion to its own
 * format, easier to read and write (but not optimized for storing).
 * ************************************************************************* */
#ifndef  LIBCASIO_NUMBER_H
# define LIBCASIO_NUMBER_H
# include "cdefs.h"
CASIO_BEGIN_NAMESPACE

# define CASIO_BCD_MANTISSA_SIZE 16
# define CASIO_BCD_EXPMIN -99
# define CASIO_BCD_EXPMAX  99

/* String constants; here are some examples:
 * - 1.23456789ABCDEFe99
 * - -0.0000123456789ABCDEF
 * - 123456789ABCDEF0000000 */

# define CASIO_BCD_GOODBUFSIZE \
	(2 + CASIO_BCD_MANTISSA_SIZE + 4 + 2 + 1)

/* ************************************************************************* */
/*  Main type                                                                */
/* ************************************************************************* */
/* libcasio type definition.
 * This is the "unzipped" BCD format. There are several, this is the one
 * libcasio will always convert to.
 *
 * `flags` is the flags (negative, special, precision) of the BCD.
 * `exp` is the 10-exponent, between -99 and 99.
 * `mant` is the mantissa: unpacked BCD digits (one digit per byte).
 *
 * The first digit of the mantissa is the integer part, the others are
 * the decimal part.
 *
 * To interact the flags or the exponent, it is advised to use the
 * `casio_bcd_*` macros, as some of them could change. */

# define casio_bcdflag_spe 0x80
# define casio_bcdflag_neg 0x40
# define casio_bcdmask_pre 0x3F

# define casio_bcd_has_special(B) ((B)->casio_bcd_flags & casio_bcdflag_spe)
# define casio_bcd_is_negative(B) ((B)->casio_bcd_flags & casio_bcdflag_neg)
# define casio_bcd_precision(B)   ((B)->casio_bcd_flags & casio_bcdmask_pre)
# define casio_bcd_exponent(B)    ((B)->casio_bcd_exp)
# define casio_make_bcdflags(SPE, NEG, PREC) \
	(((SPE) << 7) | ((NEG) << 6) | (PREC))

typedef struct casio_bcd_s {
	unsigned char casio_bcd_flags;
	char          casio_bcd_exp;
	char          casio_bcd_mant[CASIO_BCD_MANTISSA_SIZE];
} casio_bcd_t;

/* ************************************************************************* */
/*  Raw formats                                                              */
/* ************************************************************************* */
/* CAS BCD -- the old BCD format.
 *
 * First two nibbles are the integer part, the fourteen that follow are
 * the decimal part. They are followed by the sign info byte, then with a
 * BCD exponent (two nibbles).
 *
 * The sign info byte is basically the flags of the number.
 * The negative is two bits/flags (I don't know why).
 * `casio_casbcd_pow_neg`, if there, means the power is negative. */

# define casio_casbcdflag_special  0x80
# define casio_casbcdflag_negative 0x50
# define casio_casbcdflag_pow_neg  0x01

typedef struct casio_casbcd_s {
	unsigned char casio_casbcd_mant[8];
	unsigned char casio_casbcd_flags;
	unsigned char casio_casbcd_exp;
} casio_casbcd_t;

/* MCS BCD -- the most recent BCD format.
 * Only the first 9 bytes are significant.
 *
 * The highest bit of the first byte isn't to be considered as part of the
 * first nibble, it means something else (generally "the number has a complex
 * part").
 *
 * The first three nibbles are the exponent. If it is more than 500, the
 * number is negative. Its offset is -99.
 * The other values (from the fourth nibble) are the packed BCD mantissa.
 * It starts at 10^0. */

typedef struct casio_mcsbcd_s {
	unsigned char casio_mcsbcd_BCDval[9];
	unsigned char casio_mcsbcd__align[3];
} casio_mcsbcd_t;

/* ************************************************************************* */
/*  Conversion utilities                                                     */
/* ************************************************************************* */
CASIO_BEGIN_DECLS

/* From and to MCS BCD. */

CASIO_EXTERN int CASIO_EXPORT casio_bcd_frommcs
	OF((casio_bcd_t *casio__bcd, const casio_mcsbcd_t *casio__raw));
CASIO_EXTERN int CASIO_EXPORT casio_bcd_tomcs
	OF((casio_mcsbcd_t *casio__raw, const casio_bcd_t *casio__bcd));

/* From and to CAS BCD. */

CASIO_EXTERN int CASIO_EXPORT casio_bcd_fromcas
	OF((casio_bcd_t *casio__bcd, const casio_casbcd_t *casio__raw));
CASIO_EXTERN int CASIO_EXPORT casio_bcd_tocas
	OF((casio_casbcd_t *casio__raw, const casio_bcd_t *casio__bcd));

/* From and to C-double */

CASIO_EXTERN void   CASIO_EXPORT casio_bcd_fromdouble
	OF((casio_bcd_t *casio__bcd, double casio__raw));
CASIO_EXTERN double CASIO_EXPORT casio_bcd_todouble
	OF((const casio_bcd_t *casio__bcd));

/* Make a string out of a BCD */

CASIO_EXTERN size_t CASIO_EXPORT casio_bcdtoa
	OF((char *casio__buf, size_t casio__len, const casio_bcd_t *casio__bcd));

CASIO_END_DECLS
CASIO_END_NAMESPACE
#endif /* LIBCASIO_NUMBER_H */
