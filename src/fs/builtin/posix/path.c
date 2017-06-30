/* ****************************************************************************
 * fs/builtin/posix/path.c -- POSIX path conversions.
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
#include "posix.h"
#ifndef LIBCASIO_DISABLED_POSIX_FS

/**
 *	makepathnode:
 *	Make a directory node.
 *
 *	@arg	size		the name size.
 *	@return				the allocated path node.
 */

CASIO_LOCAL casio_pathnode_t *makepathnode(size_t size)
{
	casio_pathnode_t *node;

	node = malloc(offsetof(casio_pathnode_t, casio_pathnode_name) + size);
	if (!node) return (NULL);
	node->casio_pathnode_next = NULL;
	node->casio_pathnode_size = size;
	return (node);
}

/**
 *	casio_make_posix_path:
 *	Make a POSIX path from a platform-agnostic path.
 *
 *	@arg	ppath		the path to make.
 *	@arg	array		the path array.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_make_posix_path(char **ppath, casio_path_t *array)
{
	size_t length; const casio_pathnode_t *node;
	char *path;

	/* Make up the length and validate. */
	length = !(array->casio_path_flags & casio_pathflag_rel);
	node = array->casio_path_nodes;
	while (node) {
		size_t nodesize = node->casio_pathnode_size;

		/* Check that there is no forbidden characters. */
		if (memchr(node->casio_pathnode_name,   0, nodesize)
		 || memchr(node->casio_pathnode_name, '/', nodesize))
			return (casio_error_invalid);

		/* Iterate on the node, add to the length. */
		length += nodesize;
		node = node->casio_pathnode_next;
		if (node) length++; /* '/' */
	}

	/* Allocate the path. */
	*ppath = malloc(length + 1);
	path = *ppath; if (!path) return (casio_error_alloc);

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

	/* No error has occured! */
	return (0);
}

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
		localnode = makepathnode(nodelen + 1);
		if (!localnode) goto fail;
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
	return (casio_error_alloc);
}

#endif
