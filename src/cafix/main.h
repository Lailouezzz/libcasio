/* ****************************************************************************
 * cafix/main.h -- cafix internals.
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
 * ************************************************************************* */
#ifndef  MAIN_H
# define MAIN_H 2018042501
# include <errno.h>
# include <string.h>
# include <stdlib.h>
# include <ctype.h>
# include <libcasio.h>

/* Command-line arguments. */

# define MODE_SEND    1 /* send a file */
# define MODE_RECEIVE 2 /* receive data */
# define MODE_CONTROL 3 /* remote control */

typedef struct {
	int mode;
	int should_rename; /* should prompt to rename file */

	const char *filename;
} args_t;

/* Utilities. */

extern int parse_args(int ac, char **av, args_t *args);

#endif /* MAIN_H */
