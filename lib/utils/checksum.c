/* ****************************************************************************
 * utils/checksum.c -- Checksum-ing techniques.
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

/**
 *	casio_checksum_cas:
 *	Make checksums great again.
 *
 *	@arg	mem		the memory zone.
 *	@arg	size	the memory zone size.
 *	@arg	cs		the current checksum.
 *	@return			the new checksum.
 */

int CASIO_EXPORT casio_checksum_cas(void *mem, size_t size, int cs)
{
	unsigned char *m = mem;

	while (size--)
		cs = (cs - *m++) & 255;
	return (cs);
}

/**
 *	casio_checksum_sub:
 *	Make the CASIOWIN type of checksums.
 *
 *	@arg	mem		the memory zone.
 *	@arg	size	the memory zone size.
 *	@arg	cs		the current checksum.
 *	@return			the new checksum.
 */

int CASIO_EXPORT casio_checksum_sub(void *mem, size_t size, int cs)
{
	unsigned char *m = mem;

	while (size--)
		cs = (cs - *m++) & 255;
	return (cs);
}

/**
 *	casio_checksum32:
 *	Make checksums great again.
 *
 *	@arg	mem		the memory zone.
 *	@arg	size	the memory zone size.
 *	@arg	cs		the current checksum.
 *	@return			the new checksum.
 */

casio_uint32_t CASIO_EXPORT casio_checksum32(void *mem, size_t size,
	casio_uint32_t cs)
{
	casio_uint8_t *m = mem;

	while (size--)
		cs += *m++;
	return (cs);
}
