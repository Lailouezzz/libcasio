/* ****************************************************************************
 * link/seven_fs/topath.c -- make a Protocol 7.00 filesystem "native" path.
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
#include "seven_fs.h"

/**
 *	casio_make_sevenfs_path:
 *	Make a Protocol 7.00 filesystem native path.
 *
 *	@arg	cookie		the cookie (unused).
 *	@arg	ppath		the path to make.
 *	@arg	array		the path array.
 *	@return				the error code (0 if ok).
 */

int  CASIO_EXPORT casio_make_sevenfs_path(sevenfs_cookie_t *cookie,
	sevenfs_path_t **ppath, casio_path_t *array)
{
	sevenfs_path_t *path;
	const char *dirname, *filename;
	size_t dirsz, filesz;
	char *data; int off;
	casio_pathnode_t *node;

	(void)cookie;

	/* Check the device. */

	if (memcmp(array->casio_path_device, "fls0", 4) != 0
	 && memcmp(array->casio_path_device, "crd0", 4) != 0)
		return (casio_error_invalid);

	/* Get directory name and file name. */

	if (!array->casio_path_nodes)
		return (casio_error_invalid);
	node = array->casio_path_nodes;
	if (!node) return (casio_error_invalid);
	if (node->casio_pathnode_next) {
		dirsz = node->casio_pathnode_size + 1;
		if (dirsz == 1 || dirsz > 9) return (casio_error_invalid);
		dirname = (const char*)node->casio_pathnode_name;
		node = node->casio_pathnode_next;
	} else {
		dirname = NULL;
		dirsz = 0;
	}
	if (node->casio_pathnode_next) {
		/* too deep! */

		return (casio_error_invalid);
	}
	filesz = node->casio_pathnode_size + 1;
	if (filesz == 1 || filesz > 9) return (casio_error_invalid);
	filename = (const char*)node->casio_pathnode_name;

	/* Make the node. */

	*ppath = casio_alloc(offsetof(sevenfs_path_t, sevenfs_path_data)
		+ 4 + dirsz + filesz, 1);
	path = *ppath; if (!path) return (casio_error_alloc);

	/* Copy the data into the node. */

	data = path->sevenfs_path_data; off = 0;
	if (dirname) {
		memcpy(data, dirname, dirsz - 1);
		data[dirsz - 1] = 0;
		path->sevenfs_path_dir = off;
		data += dirsz; off += dirsz;
	} else
		path->sevenfs_path_dir = 0xFF;
	memcpy(data, filename, filesz - 1);
	data[filesz - 1] = 0;
	path->sevenfs_path_file = off;
	data += filesz; off += filesz;
	memcpy(data, array->casio_path_device, 4);
	data[4] = 0;
	path->sevenfs_path_dev = off;

	/* No error! */

	return (0);
}

/**
 *	casio_free_sevenfs_path:
 *	Free a Protocol 7.00 filesystem path.
 *
 *	@arg	cookie		the cookie (unused).
 *	@arg	path		the native path.
 */

void CASIO_EXPORT casio_free_sevenfs_path(sevenfs_cookie_t *cookie,
	sevenfs_path_t *path)
{
	(void)cookie;
	free(path);
}
