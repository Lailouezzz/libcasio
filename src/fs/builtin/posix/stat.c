/* ****************************************************************************
 * fs/builtin/posix/stat.c -- get file information from POSIX filesystem.
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
 *	casio_posix_stat:
 *	POSIX stat.
 *
 *	@arg	cookie		the POSIX cookie.
 *	@arg	apath		the path to get.
 *	@arg	stat		the file information to fill.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_posix_stat(void *cookie,
	casio_path_t *apath, casio_stat_t *file_info)
{
	int err; char *path = NULL;
	struct stat statbuf;

	(void)cookie;
	/* Make the POSIX path. */
	err = casio_make_posix_path(&path, apath);
	if (err) return (err);

	/* Make the stat. */
	if (lstat(path, &statbuf)) {
		/* FIXME: different kinds of errnos? */
		err = casio_error_unknown;
		goto fail;
	}

	/* Gather the type. */
	switch (statbuf.st_mode & S_IFMT) {
	case S_IFIFO: /* FALLTHRU */
	case S_IFCHR:
		file_info->casio_stat_type = CASIO_STAT_TYPE_CHAR;
		break;
	case S_IFDIR:
		file_info->casio_stat_type = CASIO_STAT_TYPE_DIR;
		break;
	case S_IFBLK:
		file_info->casio_stat_type = CASIO_STAT_TYPE_BLK;
		break;
	case S_IFREG:
		file_info->casio_stat_type = CASIO_STAT_TYPE_REG;
		break;
	case S_IFLNK:
		file_info->casio_stat_type = CASIO_STAT_TYPE_LNK;
		break;
	case S_IFSOCK:
		file_info->casio_stat_type = CASIO_STAT_TYPE_SOCK;
		break;
	default:
		file_info->casio_stat_type = 0;
		break;
	}

	/* Get the rest, and return. */
	file_info->casio_stat_flags = 0;
	file_info->casio_stat_size  = statbuf.st_size;

	err = 0;
fail:
	free(path);
	return (err);
}

#endif
