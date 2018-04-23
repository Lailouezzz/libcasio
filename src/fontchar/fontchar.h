/* ****************************************************************************
 * fontchar/fontchar.h -- libcasio internal macros for FONTCHARACTER utils.
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
#ifndef  LOCAL_FONTCHAR_H
# define LOCAL_FONTCHAR_H 1
# include "../internals.h"

/* Unicode string to FONTCHARACTER character lookup table.
 * Works as a chained list on the same axe, with children.
 * As I am a practical kind of person, here are examples:
 *
 *	"hell":
 *		"o, wor":
 *			"ld!": 1
 *			"d": 54
 *		" is great": 22
 *	"i love it": 99
 *
 * The `code` shall only be read when there is no `child` (`child == NULL`).
 */

struct uni_node {
	struct uni_node *next;
	struct uni_node *child;

	casio_uint32_t  *string;
	casio_uint16_t   code;
};

CASIO_EXTERN struct uni_node CASIO_EXPORT casio_fontchar_uni;

typedef struct casio_fontref_s {
	/* Leaders, actually a string. */

	casio_uint8_t* casio_fontref_leaders;


} casio_fontref_t;

CASIO_EXTERN

/* TODO: FONTCHARACTER to Unicode string lookup table. */

#endif /* LOCAL_FONTCHAR_H */
