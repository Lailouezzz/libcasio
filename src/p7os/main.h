/* *****************************************************************************
 * p7os/main.h -- p7os main header.
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

/* ---
 * Command-line arguments.
 * --- */

/* Menu. */

enum menu_e {
	mn_prepare_only = 1,
	mn_get          = 2,
	mn_flash        = 3
};

/* Arguments. */

typedef struct {
	enum menu_e  menu;
	int noprepare;

	/* communication and tweaks */
	const char *com;

	/* others */
	casio_stream_t *local;
	const char     *localpath;
	casio_stream_t *uexe;
} args_t;

/* ---
 * Progress displayer.
 * --- */

typedef struct {
	const char *msg, *success;

	char buf[50], *bar;
	int init, pos;
} osdisp_t;

extern void osdisp_init(osdisp_t *cookie, const char *init,
	const char *success);
extern void osdisp(void *cookie, unsigned int id, unsigned int total);

extern void osdisp_interrupt(osdisp_t *cookie);
extern void osdisp_success(osdisp_t *cookie);

/* ---
 * Central functions.
 * --- */

/* Utilities. */

extern int parse_args(int ac, char **av, args_t *args);
extern int open_link(casio_link_t **link, args_t *args,
	unsigned long flags, casio_streamattrs_t *attrs);

/* Main functions. */

extern int prepare(args_t *args);
extern int backup_rom(args_t *args);
extern int fxremote_flash(args_t *args);

#endif /* MAIN_H */
