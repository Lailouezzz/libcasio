/* ****************************************************************************
 * utils/endian.c -- internal endian things.
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
 * These utilities are inspired by `libusb_cpu_to_le16` utility from libusb.
 * ************************************************************************* */
#define LIBCASIO_NO_ENDIAN
#include "endian.h"

/**
 *	casio_be16toh:
 *	Big endian to host 16-bit conversion.
 *
 *	@arg	x	the original.
 *	@return		the converted integer.
 */

casio_uint16_t casio_be16toh(casio_uint16_t x)
{
#ifdef casio_int_be16toh
	return (casio_int_be16toh(x));
#else
	union {
		casio_uint8_t  b8[2];
		casio_uint16_t b16;
	} tmp;

	tmp.b16 = x;
	return ((tmp.b8[0] << 8) | tmp.b8[1]);
#endif
}

/**
 *	casio_le16toh:
 *	Little endian to host 16-bit conversion.
 *
 *	@arg	x	the original.
 *	@return		the converted integer.
 */

casio_uint16_t casio_le16toh(casio_uint16_t x)
{
#ifdef casio_le16toh
	return (casio_int_le16toh(x));
#else
	union {
		casio_uint8_t  b8[2];
		casio_uint16_t b16;
	} tmp;

	tmp.b16 = x;
	return ((tmp.b8[1] << 8) | tmp.b8[0]);
#endif
}

/**
 *	casio_be32toh:
 *	Big endian to host 32-bit conversion.
 *
 *	@arg	x	the original.
 *	@return		the converted integer.
 */

casio_uint32_t casio_be32toh(casio_uint32_t x)
{
#ifdef casio_int_be32toh
	return (casio_int_be32toh(x));
#else
	union {
		casio_uint8_t  b8[4];
		casio_uint32_t b32;
	} tmp;

	tmp.b32 = x;
	return ((tmp.b8[0] << 24) | (tmp.b8[1] << 16)
	      | (tmp.b8[2] <<  8) | (tmp.b8[3]      ));
#endif
}

/**
 *	casio_le32toh:
 *	Little endian to host 32-bit conversion.
 *
 *	@arg	x	the original.
 *	@return		the converted integer.
 */

casio_uint32_t casio_le32toh(casio_uint32_t x)
{
#ifdef casio_int_le32toh
	return (casio_int_le32toh(x));
#else
	union {
		casio_uint8_t  b8[4];
		casio_uint32_t b32;
	} tmp;

	tmp.b32 = x;
	return ((tmp.b8[3] << 24) | (tmp.b8[2] << 16)
	      | (tmp.b8[1] <<  8) | (tmp.b8[0]      ));
#endif
}

/**
 *	casio_htobe16:
 *	Host to big endian 16-bit conversion.
 *
 *	@arg	x	the original.
 *	@return		the converted integer.
 */

casio_uint16_t casio_htobe16(casio_uint16_t x)
{
#ifdef casio_int_htobe16
	return (casio_int_htobe16(x));
#else
	union {
		casio_uint8_t  b8[2];
		casio_uint16_t b16;
	} tmp;

	tmp.b8[0] = x >> 8;
	tmp.b8[1] = x & 0xFF;
	return (tmp.b16);
#endif
}

/**
 *	casio_htole16:
 *	Host to little endian 16-bit conversion.
 *
 *	@arg	x	the original.
 *	@return		the converted integer.
 */

casio_uint16_t casio_htole16(casio_uint16_t x)
{
#ifdef casio_int_htole16
	return (casio_int_htole16(x));
#else
	union {
		casio_uint8_t  b8[2];
		casio_uint16_t b16;
	} tmp;

	tmp.b8[0] = x & 0xFF;
	tmp.b8[1] = x >> 8;
	return (tmp.b16);
#endif
}

/**
 *	casio_htobe32:
 *	Host to big endian 32-bit conversion.
 *
 *	@arg	x	the original.
 *	@return		the converted integer.
 */

casio_uint32_t casio_htobe32(casio_uint32_t x)
{
#ifdef casio_int_htobe32
	return (casio_int_htobe32(x));
#else
	union {
		casio_uint8_t  b8[4];
		casio_uint32_t b32;
	} tmp;

	tmp.b8[0] = (x >> 24);
	tmp.b8[1] = (x >> 16) & 0xFF;
	tmp.b8[2] = (x >>  8) & 0xFF;
	tmp.b8[3] = (x      ) & 0xFF;
	return (tmp.b32);
#endif
}

/**
 *	casio_htole32:
 *	Host to little endian 32-bit conversion.
 *
 *	@arg	x	the original.
 *	@return		the converted integer.
 */

casio_uint32_t casio_htole32(casio_uint32_t x)
{
#ifdef casio_int_htole32
	return (casio_int_htole32(x));
#else
	union {
		casio_uint8_t  b8[4];
		casio_uint32_t b32;
	} tmp;

	tmp.b8[0] = (x      ) & 0xFF;
	tmp.b8[1] = (x >>  8) & 0xFF;
	tmp.b8[2] = (x >> 16) & 0xFF;
	tmp.b8[3] = (x >> 24);
	return (tmp.b32);
#endif
}
