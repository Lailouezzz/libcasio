/* ****************************************************************************
 * fs/builtin/posix/topath.c -- POSIX path conversions.
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
#define PATH_FTW
#include "posix.h"
#ifndef LIBCASIO_DISABLED_POSIX_FS

/**
 *	casio_make_posix_path:
 *	Make a POSIX path from a platform-agnostic path.
 *
 *	@arg	cookie		the filesystem cookie (unused).
 *	@arg	ppath		the path to make.
 *	@arg	array		the path array.
 *	@return				the error code (0 if ok).
 */

int  CASIO_EXPORT casio_make_posix_path(void *cookie,
	void **ppath, casio_path_t *array)
{
	size_t length; const casio_pathnode_t *node;
	char *path;

	(void)cookie;
	/* Make up the length and validate. */
	length = !(array->casio_path_flags & casio_pathflag_rel);
	node = array->casio_path_nodes;
	while (node) {
		size_t nodesize = node->casio_pathnode_size;

		/* Check that there is no forbidden characters. */
		if (nodesize > 255
		 || memchr(node->casio_pathnode_name,   0, nodesize)
		 || memchr(node->casio_pathnode_name, '/', nodesize))
			return (casio_error_invalid);

		/* Iterate on the node, add to the length. */
		length += nodesize;
		node = node->casio_pathnode_next;
		if (node) length++; /* '/' */
	}

	/* Allocate the path. */
	*ppath = casio_alloc(length + 1, 1);
	path = (char*)*ppath;
	if (!path) return (casio_error_alloc);

	/* Fill the path. */
	if (~array->casio_path_flags & casio_pathflag_rel)
		*path++ = '/';
	node = array->casio_path_nodes;
	while (node) {
		size_t nodesize = node->casio_pathnode_size;

		/* Copy the node data. */
		memcpy(path, node->casio_pathnode_name, nodesize);
		path += nodesize;

		/* Iterate on the node, copy a slash if needed. */
		node = node->casio_pathnode_next;
		if (node) *path++ = '/';
	}
	*path = 0;

	/* No error has occured! */
	return (0);
}

/**
 *	casio_free_posix_path:
 *	Free a POSIX path.
 *
 *	@arg	cookie		the cookie.
 *	@arg	nat			the native path.
 */

void CASIO_EXPORT casio_free_posix_path(void *cookie, void *nat)
{
	(void)cookie;
	casio_free(nat);
}

#endif
