/* ****************************************************************************
 * mcsfile/match.c -- check if two main memory files match.
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
#include "mcsfile.h"

/**
 *	casio_match_mcsfiles:
 *	Check if two main memory files match.
 *
 *	@arg	first	the first head.
 *	@arg	second	the second head.
 *	@return			0 if they don't match, something else otherwise.
 */

int casio_match_mcsfiles(casio_mcshead_t *first, casio_mcshead_t *second)
{
	unsigned long mcsfor;

	/* Check that the two heads are defined. */
	if (!first || !second) return (casio_error_invalid);

	/* Compare. */
	if (first->casio_mcshead_type != second->casio_mcshead_type)
		return (0);
	if (first->casio_mcshead_type) {
		if (casio_mcshead_uses_id(first)) {
			if (first->casio_mcshead_id != second->casio_mcshead_id)
				return (0);
		} else if (strcmp(first->casio_mcshead_name,
		  second->casio_mcshead_name))
			return (0);
	} else {
		mcsfor = first->casio_mcshead_flags & casio_mcsfor_mask;
		if (mcsfor != (second->casio_mcshead_flags & casio_mcsfor_mask))
			return (0);
		switch (mcsfor) {
		case casio_mcsfor_mcs:
			if (strncmp(first->casio_mcshead_group,
			    second->casio_mcshead_group, 16)
			 || strncmp(first->casio_mcshead_dirname,
			    second->casio_mcshead_dirname, 8)
			 || strncmp(first->casio_mcshead_name,
			    second->casio_mcshead_name, 8))
				return (0);
			break;

		case casio_mcsfor_cas:
		case casio_mcsfor_caspro:
			if (strncmp(first->casio_mcshead_datatype,
			    second->casio_mcshead_datatype, 2))
				return (0);
			break;

		default: /* have a */ break /* have a kit kat */;
		}
	}

	/* They do match! */
	return (1);
}
