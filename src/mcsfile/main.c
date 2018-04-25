/* *****************************************************************************
 * mcsfile/main.c -- mcsfile main source.
 * Copyright (C) 2017 Thomas "Cakeisalie5" Touhey <thomas@touhey.fr>
 *
 * This file is part of p7utils.
 * p7utils is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2.0 of the License,
 * or (at your option) any later version.
 *
 * p7utils is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with p7utils; if not, see <http://www.gnu.org/licenses/>.
 * ************************************************************************** */
#include "main.h"
#include <string.h>
#include <errno.h>
#include <locale.h>
#define cry(S, ...) fprintf(stderr, S "\n", ##__VA_ARGS__)

/**
 *	main:
 *	Entry point of the program.
 *
 *	@arg	ac		the arguments count.
 *	@arg	av		the arguments values.
 *	@return			the status code (0 if ok).
 */

int main(int ac, char **av)
{
	const char *path;
	casio_file_t *handle;
	int err;

	/* Set the locale and parse arguments. */

	setlocale(LC_ALL, "");
	if (parse_args(ac, av, &path))
		return (0);

	/* parse */
	if ((err = casio_open_file(&handle, path, casio_filetype_mcs)))
	  switch (err) {
		case casio_error_wrong:
			cry("An MCS file was expected (g1m/g1r, g1m/g2r, g3m)");
			return (1);
		case casio_error_nostream:
			cry("Could not open file: %s", strerror(errno));
			return (1);
		case casio_error_magic:
			cry("Magic error: file might be corrupted");
			return (1);
		case casio_error_eof:
			cry("Unexpected end of file");
			return (1);
		default:
			cry("Unknown error: %s", casio_strerror(err));
			return (1);
	}

	/* Read the files, free the handle and exit. */

	print_files(handle->casio_file_mcs);
	casio_free_file(handle);

	return (0);
}
