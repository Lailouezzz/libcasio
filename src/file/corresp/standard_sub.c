/* ****************************************************************************
 * file/corresp/standard_sub.c -- standard subheader types.
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
#include "standard.h"

/* Platforms and types */
#define p_cp    0x00
#define p_cg    0x01
#define t_addin 0x01
#define t_fkey  0x02
#define t_lang  0x04

/* ************************************************************************* */
/*  Helpers                                                                  */
/* ************************************************************************* */
/* Type correspondance type */
struct type_info {
	/* identification */
	casio_uint32_t raw_platform;
	casio_uint32_t raw_type;

	/* types */
	casio_filefor_t platform;
	casio_filetype_t type;
};
/* ************************************************************************* */
/*  Correspondances                                                          */
/* ************************************************************************* */
/* Main types */
static struct type_info types[] = {
	/* fx-CP types */
	{p_cp, t_addin, casio_filefor_cp, casio_filetype_addin},

	/* fx-CG types */
	{p_cg, t_addin, casio_filefor_cg, casio_filetype_addin},
	{p_cg, t_fkey,  casio_filefor_cg, casio_filetype_fkey},
	{p_cg, t_lang,  casio_filefor_cg, casio_filetype_lang},

	/* sentinel */
	{0, 0, 0, 0}
};

/**
 *	casio_maketype_sub:
 *	Get type info from Standard Subheader.
 *
 *	@arg	raw_type	The raw type.
 *	@arg	raw_pf		The raw platform.
 *	@arg	flags		The flags to set.
 *	@arg	type		The type to get.
 *	@arg	platform	The platform to get.
 *	@return				If there was an error.
 */

int casio_maketype_sub(casio_standard_subheader_t *sub,
	unsigned int *flags, casio_filetype_t *type, casio_filefor_t *platform)
{
	struct type_info *c;
	casio_uint32_t raw_type = sub->casio_standard_subheader_filetype;
	casio_uint32_t raw_pf   = sub->casio_standard_subheader_platform;

	/* look for the correspondance */
	for (c = types; c->platform
	 && (c->raw_type != raw_type || c->raw_platform != raw_pf); c++);
	if (!c->platform) return (casio_error_unknown);

	/* fill things */
	if (flags) *flags = 0;
	if (platform) *platform = c->platform;
	if (type) *type = c->type;
	return (0);
}
