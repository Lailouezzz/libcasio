/* *****************************************************************************
 * g1a-wrapper/main.h -- g1a-wrapper main header.
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
#ifndef MAIN_H
# define MAIN_H
# include <stdio.h>
# include <libcasio.h>
# define warn(M, ...) \
	fprintf(stderr, "g1a-wrapper: warning: " M "\n", ##__VA_ARGS__)
# define err(M, ...) \
	fprintf(stderr, "g1a-wrapper: error: " M "\n", ##__VA_ARGS__)

typedef struct {
	/* General options. */

	const char *infile;    /* path to the input file */
	const char *outfile;   /* default: addin.g1a */
	const char *iconfile;  /* icon file, default is blank */

	/* Build options. */

	const char *name;        /* default: truncated output filename */
	const char *intname;     /* default: @ADDIN */
	casio_version_t version; /* default: 00.00.0000 */
	time_t date;             /* default: current time */
} args_t;

/* Command-line arguments parsing function */
extern int parse_args(int ac, char **av, args_t *args);

/* Icon decoding function */
extern int open_icon(const char *path, uint32_t **icon);

#endif /* MAIN_H */
