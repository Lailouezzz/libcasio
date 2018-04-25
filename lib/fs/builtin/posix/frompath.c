/* ****************************************************************************
 * fs/builtin/posix/frompath.c -- POSIX path to libcasio path.
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
 *	casio_make_posix_path_array:
 *	Make a path array from a POSIX path.
 *
 *	POSIX paths are soooo well done. <3
 *
 *	@arg	ppath		the final path.
 *	@arg	rawpath		the raw POSIX path.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_make_posix_path_array(casio_path_t **ppath,
	const char *rawpath)
{
	int err = casio_error_alloc;
	casio_path_t *path = NULL;
	casio_pathnode_t **node = NULL;
	casio_pathnode_t  *localnode = NULL;
	size_t nodelen;

	/* Make and initialize the path structure. */
	*ppath = malloc(sizeof(casio_path_t));
	path = *ppath; if (!path) return (casio_error_alloc);
	path->casio_path_flags = casio_pathflag_alloc | casio_pathflag_rel;
	path->casio_path_device = NULL;
	path->casio_path_nodes = NULL;

	/* Check if it is an absolute path. */
	if (*rawpath == '/') {
		path->casio_path_flags &= ~casio_pathflag_rel;
		rawpath++;
	}

	/* Split the path. */
	node = &path->casio_path_nodes;
	while (rawpath) {
		rawpath += strspn(rawpath, "/");
		if (!rawpath) break;

		/* Get the entry size (to '/' or NUL). */
		nodelen = strcspn(rawpath, "/");

		/* Make the entry. */
		err = casio_make_pathnode(&localnode, nodelen + 1);
		if (err) goto fail;
		memcpy(localnode->casio_pathnode_name, rawpath, nodelen);
		localnode->casio_pathnode_name[nodelen] = 0;
		*node = localnode;
		node = &localnode->casio_pathnode_next;

		/* Iterate. */
		rawpath += nodelen;
	}

	return (0);
fail:
	while (path->casio_path_nodes) {
		localnode = path->casio_path_nodes;
		path->casio_path_nodes = localnode->casio_pathnode_next;
		free(localnode);
	}
	if (!err) err = casio_error_alloc;
	return (err);
}

#endif
