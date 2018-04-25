/* ****************************************************************************
 * fs/pathnode.c -- manage path nodes.
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
#include "fs.h"

/**
 *	casio_make_pathnode:
 *	Make a path node.
 *
 *	@arg	pnode		the path node to make.
 *	@arg	size		the path node data size.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_make_pathnode(casio_pathnode_t **pnode, size_t size)
{
	casio_pathnode_t *node;

	/* Allocate the node. */
	*pnode = casio_alloc(offsetof(casio_pathnode_t,
		casio_pathnode_name) + size, 1);
	node = *pnode; if (!node) return (casio_error_alloc);

	/* Set the static attributes. */
	node->casio_pathnode_next = NULL;
	node->casio_pathnode_size = size;

	/* Everything went well! */
	return (0);
}

/**
 *	casio_free_pathnode:
 *	Free a path node.
 *
 *	@arg	node		the path node to free.
 *	@return				the error code (0 if ok).
 */

void CASIO_EXPORT casio_free_pathnode(casio_pathnode_t *node)
{
	casio_free(node);
}

/**
 *	casio_duplicate_pathnode:
 *	Duplicate a path node.
 *
 *	@arg	new			the path node to make.
 *	@arg	old			the path node to copy.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_duplicate_pathnode(casio_pathnode_t **new,
	const casio_pathnode_t *old)
{
	int err; size_t sz;

	/* Make the new path node. */
	sz = old->casio_pathnode_size;
	err = casio_make_pathnode(new, sz);
	if (err) return (err);

	/* Copy the data and return. */
	memcpy((*new)->casio_pathnode_name, old->casio_pathnode_name, sz);
	return (0);
}
