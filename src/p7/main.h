/* *****************************************************************************
 * p7/main.h -- p7 main header.
 * Copyright (C) 2016-2017 Thomas "Cakeisalie5" Touhey <thomas@touhey.fr>
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
# define Q(x) #x
# define QUOTE(x) Q(x)
# include <stdio.h>
# include <libcasio.h>
# define log(S, ...) fprintf(stderr, S, ##__VA_ARGS__)

/* Menus */
typedef enum {
	mn_send, mn_get, mn_copy, mn_del, mn_list, mn_reset,
	mn_optimize, mn_info, mn_idle, mn_unlock
} menu_t;

/* Arguments */
typedef struct {
	/* basic things */
	menu_t menu; int nicedisp;

	/* libp7 settings */
	unsigned int initflags;
	casio_streamattrs_t *use, _use;
	casio_streamattrs_t *set, _set;
	int do_the_set;

	/* for file transferring menus */
	const char *dirname, *filename;
	const char *newdir, *newname;
	FILE *local; const char *localpath;
	int force;

	/* other options */
	const char *com, *storage;
} args_t;

/* Parsing function */
int parse_args(int ac, char **av, args_t *args);

/* List devices function */
int list_devices(void);

/* Dumping function */
int dump(casio_link_t *handle);

#endif /* MAIN_H */
