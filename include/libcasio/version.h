/* ****************************************************************************
 * libcasio/version.h -- description of CASIO's versions.
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
#ifndef  LIBCASIO_VERSION_H
# define LIBCASIO_VERSION_H
# include <libcasio/cdefs.h>
CASIO_BEGIN_NAMESPACE

/* CASIO's versions look like 'MM.mm.ABCD', where 'MM' is the major, 'mm' is
 * the minor, and 'FFFF' contain various information.
 *
 * 'A' is the zone (localization) of the OS. It is one the following: */

# define casio_version_zone_none  0x00 /* (international) */
# define casio_version_zone_aus   0x01 /* Australia */
# define casio_version_zone_fr    0x02 /* France */
# define casio_version_zone_nam   0x03 /* North america */
# define casio_version_zone_ch    0x04 /* China */
# define casio_version_zone_sing  0x05 /* Singapour */

/* 'B' is the math input/output features of the OS (OS type).
 * It is one of the following: */

# define casio_version_math_slim     0x01 /* Slim */
# define casio_version_math_all      0x02 /* All features (fx-9860GII-2) */
# define casio_version_math_reduced  0x03 /* Reduced features (fx-7400GII) */
# define casio_version_math_none     0x07 /* No math features? */

/* 'C' is the in-development status of the OS.
 * It is one of the following: */

# define casio_version_status_standard  0x00 /* Standard build */
# define casio_version_status_indev     0x01 /* In-development/special build */

/* 'D' is the hardware platform ID for which the OS is built.
 * It is one of the following: */

# define casio_version_platform_basic    0x00 /* SH7337/SH7355 */
# define casio_version_platform_special  0x01 /* SH7305 */
# define casio_version_platform_prizm    0x02 /* SH7305 (Prizm) */

/* Here is the libcasio structure: */

typedef struct casio_version_s {
	unsigned char casio_version_major;
	unsigned char casio_version_minor;
	unsigned char casio_version_zone;
	unsigned char casio_version_math;
	unsigned char casio_version_status;
	unsigned char casio_version_platform;
} casio_version_t;

CASIO_BEGIN_DECLS
/* And here are the utilities to check that a version has the
 * right format (for user interface, such as command-line argument checking),
 * and to decode/encode a version. */

extern int casio_check_version  OF((const char *casio__raw));
extern int casio_decode_version OF((casio_version_t *casio__version,
	const char *casio__raw));
extern int casio_encode_version OF((char *casio__raw,
	const casio_version_t *casio__version));

CASIO_END_DECLS
CASIO_END_NAMESPACE
#endif /* LIBCASIO_VERSION_H */
