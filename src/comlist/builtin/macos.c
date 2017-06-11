/* ****************************************************************************
 * comlist/builtin/macos.c -- find out MacOS/OS X serial devices.
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
#include "../../internals.h"
#if defined(__APPLE__) && defined(__MACH__)
# include <dirent.h>

/**
 *	casio_comlist_macos:
 *	List serial devices under MacOS/OS X.
 *
 *	@arg	callback	the callback.
 *	@arg	cookie		the cookie.
 *	@return				the error.
 */

int casio_comlist_macos(casio_list_com_t *callback, void *cookie)
{
	char path[PATH_MAX + 1], *f; DIR *d;
	struct dirent *dr;

	/* prepare the folder */
	strcpy(path, "/dev/");
	f = strchr(path, 0);

	/* open the directory */
	d = opendir(path);
	if (!d) return (casio_error_unknown);

	/* read the entries */
	while ((dr = readdir(d))) {
		/* check name */
		if (strncmp(dr->d_name, "cu.", 3))
			continue;

		/* copy and set to the user */
		strcpy(f, dr->d_name);
		(*callback)(cookie, path);
	}

	/* close the dir and we're done listing */
	closedir(d);
	return (0);
}

#endif /* __APPLE__, __MACH__ */
