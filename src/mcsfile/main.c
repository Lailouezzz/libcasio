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
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <locale.h>

struct entry {
	int valid;
	casio_file_t *handle;
	const char *path;
};

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
	int num, i, err, fst;
	const char **paths;
	struct entry *entries, *ep;

	/* Set the locale and parse arguments. */

	setlocale(LC_ALL, "");
	if (parse_args(ac, av, &num, &paths))
		return (0);

	/* Prepare the list. */

	if (!(entries = malloc(sizeof(struct entry) * num))) {
		fprintf(stderr, "error: a memory allocation has failed\n");
		return (1);
	}

	for (ep = entries, i = num; i; ep++, i--) {
		ep->valid = 0;
		ep->handle = NULL;
		ep->path = paths[num - i];

		/* Decode the file. */

		err = casio_open_file(&ep->handle, ep->path, casio_filetype_mcs);

		if (err) {
			fprintf(stderr, "error: %s: ", ep->path);
			switch (err) {
			case casio_error_wrong:
				fprintf(stderr, "an MCS file was expected "
					"(g1m, g1r, g2m, g2r, g3m)\n");
				break;

			case casio_error_nostream:
				fprintf(stderr, "could not open file: %s\n", strerror(errno));
				break;

			case casio_error_magic:
			case casio_error_eof:
				fprintf(stderr, "file might be corrupted\n");
				break;

			default:
				fprintf(stderr, "%s\n", casio_strerror(err));
				break;
			}

			continue;
		}

		ep->valid = 1;
	}

	/* List files in all of the given archives, and free the
	 * file handles while we're at it. */

	fst = 1;
	for (ep = entries, i = num; i; ep++, i--) {
		if (!ep->valid)
			continue;

		/* Print the header, with some spacing if not the first entry. */

		if (fst)
			fst = 0;
		else
			fputc('\n', stdout);

		if (num > 1)
			fprintf(stdout, "%s:\n\n", ep->path);

		/* Print the entries in the archive. */

		print_files(ep->handle->casio_file_mcs);

		/* Free the handle. */

		casio_free_file(ep->handle);
	}

	return (0);
}
