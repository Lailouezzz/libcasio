/* ****************************************************************************
 * utils/extension.c -- file extension utilities.
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
 *	casio_getext:
 *	Get extension from a path.
 *
 *	@arg	path		the path.
 *	@arg	buf			the extension buffer.
 *	@arg	n			the extension buffer size.
 *	@return				the size of the extension.
 */

int CASIO_EXPORT casio_getext(const char *path, char *buf, size_t n)
{
	const char *filename, *ext;
	int i;

	/* no size? */
	if (!n) return (0);

	/* get filename */
	filename = strrchr(path, '/');
	filename = filename ? filename + 1 : path;

	/* get extension */
	ext = strrchr(filename, '.');
	ext = ext ? ext + 1 : "";

	/* copy it */
	buf[n - 1] = 0; strncpy(buf, ext, n - 1);
	for (i = 0; buf[i]; i++)
		buf[i] = tolower(buf[i]);

	/* return the size */
	return (strlen(buf));
}
