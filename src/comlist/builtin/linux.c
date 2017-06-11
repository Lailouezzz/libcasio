/* ****************************************************************************
 * comlist/builtin/linux.c -- find out Linux serial devices.
 * Copyright (C) 2016-2017 Thomas "Cakeisalie5" Touhey <thomas@touhey.fr>
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
#ifdef __linux__
# include <sys/stat.h>
# include <dirent.h>
# include <fcntl.h>
# include <unistd.h>
# include <errno.h>

/**
 *	casio_comlist_linux:
 *	List serial devices under Linux.
 *
 *	Links in the /dev/serial/by-id/ should resolve to relative paths, but
 *	I also managed absolute paths, in case.
 *
 *	@arg	callback		the callback.
 *	@arg	cookie			the cookie.
 *	@return					the error.
 */

int casio_comlist_linux(casio_list_com_t *callback, void *cookie)
{
	char path[PATH_MAX + 1], devname[PATH_MAX + 1], *f, *p;
	DIR *d; struct dirent *dr; struct stat st;
	ssize_t rl;

	/* open the thing */
	strcpy(path, "/dev/serial/by-id/");
	d = opendir(path); if (!d) return (casio_error_unknown);

	/* prepare */
	f = strchr(path, 0);

	/* read the entries */
	while ((dr = readdir(d))) {
		/* check type */
		strcpy(f, dr->d_name);
		if (lstat(path, &st) || (st.st_mode & S_IFMT) != S_IFLNK)
			continue;

		/* get destination path and send it */
		rl = readlink(path, devname, PATH_MAX + 1);
		if (rl < 0) continue;
		devname[rl] = 0;
		if (devname[0] == '/')
			(*callback)(cookie, devname);
		else {
			strcpy(f, devname);
			p = realpath(path, devname);
			if (!p) continue;
			(*callback)(cookie, p);
		}
	}

	/* close the dir and we're done listing */
	closedir(d);
	return (0);
}

# endif /* __linux__ */
