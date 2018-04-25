/* ****************************************************************************
 * fs/builtin/posix/make.c -- make a POSIX filesystem element.
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
#define PATHS_FTW
#include "posix.h"
#ifndef LIBCASIO_DISABLED_POSIX

/**
 *	casio_posix_make:
 *	Make a POSIX filesystem element.
 *
 *	@arg	cookie		the cookie.
 *	@arg	path		the file path.
 *	@arg	stat		the file information.
 *	@arg	...			other information depending on the file type.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_posix_make(void *cookie, const char *path,
	const casio_stat_t *info, ...)
{
	int err = 0;
	mode_t mode;
	va_list ap;

	(void)cookie;

	/* Start the variable argument list. */
	va_start(ap, info);

	/* Deduce the mode. */
	if (info->casio_stat_flags & CASIO_STAT_FLAG_PERM) {
		mode = 0;
		if (info->casio_stat_perm & CASIO_STAT_PERM_IRUSR)
			mode |= S_IRUSR;
		if (info->casio_stat_perm & CASIO_STAT_PERM_IWUSR)
			mode |= S_IWUSR;
		if (info->casio_stat_perm & CASIO_STAT_PERM_IXUSR)
			mode |= S_IXUSR;
		if (info->casio_stat_perm & CASIO_STAT_PERM_IRGRP)
			mode |= S_IRGRP;
		if (info->casio_stat_perm & CASIO_STAT_PERM_IWGRP)
			mode |= S_IWGRP;
		if (info->casio_stat_perm & CASIO_STAT_PERM_IXGRP)
			mode |= S_IXGRP;
		if (info->casio_stat_perm & CASIO_STAT_PERM_IROTH)
			mode |= S_IROTH;
		if (info->casio_stat_perm & CASIO_STAT_PERM_IWOTH)
			mode |= S_IWOTH;
		if (info->casio_stat_perm & CASIO_STAT_PERM_IXOTH)
			mode |= S_IXOTH;
	} else
		mode = S_IRWXU | S_IRWXG | S_IRWXO;

	/* Make the file. */
	switch (info->casio_stat_type) {
	case CASIO_STAT_TYPE_DIR:
		if (mkdir(path, mode))
			goto fail;
		break;

	case CASIO_STAT_TYPE_LNK:
		if (symlink(va_arg(ap, const char*), path))
			goto fail;
		break;

	default:
		err = casio_error_op;
	}

	/* End. */
	va_end(ap);
	return (err);

fail:
	va_end(ap);
	/* TODO: check on `errno` */
	return (casio_error_unknown);
}

#endif
