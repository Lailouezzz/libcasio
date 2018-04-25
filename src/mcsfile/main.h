/* *****************************************************************************
 * mcsfile/main.h -- mcsfile main header.
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
# include <libcasio.h>

/* Some printf types */

# ifdef _WIN64
#  define PRIuSIZE "l64u"
# elif _WIN32
#  define PRIuSIZE "u"
# else
#  define PRIuSIZE "zu"
# endif

/* Prototypes. */

int parse_args(int ac, char **av, int *num, const char ***paths);
void print_files(casio_mcs_t *handle);

#endif /* MAIN_H */
