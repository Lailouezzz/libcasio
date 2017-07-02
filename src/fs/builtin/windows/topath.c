/* ****************************************************************************
 * fs/builtin/windows/topath.c -- make a Windows path.
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
 *
 * Based on this documentation:
 * https://msdn.microsoft.com/en-us/library/windows/desktop/aa365247.aspx
 * ************************************************************************* */
#include "windows.h"
#include <ctype.h>
#ifndef LIBCASIO_DISABLED_WINDOWS

/**
 *	validate_name:
 *	Validate element name.
 *
 *	We'll check that names respect the short name formats, also called the 8.3.
 *	Folder names are 8 max chars, and file names are 8 max chars of name
 *	plus 3 max chars of extensions.
 *
 *	TODO: check for filesystems supporting long file names.
 *
 *	@arg	name		the name.
 *	@arg	size		the name size.
 *	@arg	flags		the validation flags.
 *	@return				if it is validated (0 if not).
 */

CASIO_LOCAL int validate_name(const char *name, size_t size,
	unsigned int flags)
{
	int i;
	const char *nm;

	/* Check the short format. */
	i = 8; if (i >= size) i = size;

	/* - Look for the dot marking the extension. */
	for (; i >= 0 && name[i] != '.'; i--);
	if (i < 0) {
		/* No extension; check if the file is 8 chars or under. */
		if (size > 8) return (casio_error_invalid);
	} else {
		/* Check if there is a dot after the limit for the extension. */
		if (memchr(&name[i], '.', size - i)) return (casio_error_invalid);

		/* Check if the extension is longer than 3 chars. */
		if (size > i + 3) return (casio_error_invalid);
	}

	/* Look for reserved names. */
	if (size == 3) {
		if (!memcmp(name, "CON") || !memcmp(name, "PRN")
		 || !memcmp(name, "AUX") || !memcmp(name, "NUL"))
			return (0);
	} else if (size == 4) {
		if ((!memcmp(name, "COM") && name[3] >= '1' && name[3] <= '9')
		 || (!memcmp(name, "LPT") && name[3] >= '1' && name[3] <= '8'))
			return (0);
	}

	/* Look for reserved characters. */
	for (i = size; i; i--, name++) {
		if (*name < 0x20 || memchr("<>:\"/\\|?*", *name, 9))
			return (0);
	}

	/* We're good! */
	return (1);
}

/**
 *	casio_make_windows_path:
 *	Make a Windows path.
 *
 *	@arg	cookie		the filesystem cookie (unused).
 *	@arg	ppath		the native path to make.
 *	@arg	array		the abstract path.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_make_windows_path(void *cookie,
	void *ppath, casio_path_t *array)
{
	size_t length = 0;
	char *path;
	casio_pathnode_t *node;

	if (array->casio_path_device) {
		if (array->casio_path_device[1]
		 || !isupper(array->casio_path_device[0]))
			return (casio_error_invalid);
		length += 2; /* <drive letter>: */
	}

	if (~array->casio_path_flags & casio_pathflag_rel)
		length++; /* initial backslash */

	node = array->casio_path_nodes;
	if (!node) return (casio_error_invalid);

	while (node) {
		if (!validate_name(node->casio_pathnode_name,
		  node->casio_pathnode_size))
			return (casio_error_invalid);
		length += node->casio_pathnode_size;

		node = node->casio_pathnode_size;
		if (node) length++; /* '\\' */

		if (length > 259)
			return (casio_error_invalid);
	}

	/* Allocate the path. */
	*ppath = casio_alloc(length + 1, 1);
	path = *ppath; if (!path) return (casio_error_alloc);

	/* Fill the path. */
	if (array->casio_path_device) {
		*path++ = array->casio_path_device[0];
		*path++ = ':';
	}
	if (~array->casio_path_flags & casio_pathflag_rel)
		*path++ = '\\';

	node = array->casio_path_nodes;
	while (node) {
		size_t nodesize = node->casio_pathnode_size;

		memcpy(path, node->casio_pathnode_name, nodesize);
		path += nodesize;

		node = node->casio_pathnode_next;
		if (node) *path++ = '\\';
	}
	*path = 0;
	return (0);
}

/**
 *	casio_free_windows_path:
 *	Free a Windows path.
 *
 *	@arg	cookie		the filesystem cookie (unused).
 *	@arg	path		the native path to free.
 */

void CASIO_EXPORT casio_free_windows_path(void *cookie,
	void *native_path)
{
	(void)cookie;
	free(native_path);
}

#endif
