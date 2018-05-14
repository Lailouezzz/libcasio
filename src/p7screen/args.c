/* ****************************************************************************
 * p7screen/args.c -- p7screen argument parsing.
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
 * ************************************************************************* */
#include "main.h"
#include <stdlib.h>
#include <ctype.h>
#include <getopt.h>

/* ---
 * Help and version messages.
 * --- */

/* Version message */

static const char version_message[] =
BIN " - from " NAME " v" VERSION " (licensed under GPLv2)\n"
"Maintained by " MAINTAINER ".\n"
"\n"
"This is free software; see the source for copying conditions.\n"
"There is NO warranty; not even for MERCHANTABILITY or\n"
"FITNESS FOR A PARTICULAR PURPOSE.";

/* Help message */

static const char help_main0[] =
"Usage: " BIN " [--help|-h] [--version|-v]\n"
"\n"
"Displays the streamed screen from a CASIO fx calculator.\n"
"\n"
"Options are:\n"
"  -h, --help        Display this help page\n"
"  -v, --version     Displays the version\n";

static const char help_log[] =
"  --log <level>     The library log level (default: %s).\n"
"                    One of: %s";

static const char help_main1[] =
"  -z ZOOM           Change the zoom (1 to 16)\n"
"                    By default, the zoom will be " DEFAULT_ZOOM ".\n"
"\n"
"Report bugs to " MAINTAINER ".\n";

/* ---
 * Main function.
 * --- */

/**
 *	put_loglevel:
 *	Put a loglevel (for listing).
 *
 *	@arg	first	the first log level.
 *	@arg	level	the level string.
 */

static void put_loglevel(char **first, const char *level)
{
	if (!*first) {
		*first = malloc(strlen(level) + 2);
		if (!*first) return ;
		strcpy(*first + 1, level);
		**first = 'F';
		return ;
	}

	if (**first == 'F') {
		printf(help_log, casio_getlog(), *first + 1);
		**first = 'N';
	}

	printf(", %s", level);
}

/**
 *	put_help:
 *	Put the help message.
 */

static void put_help(void)
{
	char *first;

	/* first big part */
	fputs(help_main0, stdout);

	/* loglevels */
	first = NULL;
	casio_listlog((casio_log_list_t*)&put_loglevel, (void*)&first);
	if (first && *first == 'N') fputc('\n', stdout);
	free(first);

	/* second big part */
	fputs(help_main1, stdout);
}

/**
 *	parse_args:
 *	Args parsing main function.
 *
 *	Inspired of the edits of my first experiment with getopt.
 *	Interesting, huh?
 *
 *	@arg	ac		the arguments count
 *	@arg	av		the arguments values
 *	@arg	zoom	pointer the zoom
 *	@return			0 if ok, other if not.
 */

int parse_args(int ac, char **av, unsigned int *zoom)
{
	int c, help = 0, version = 0;
	const char *s_log = NULL;
	const char short_options[] = "hvz:";
	const struct option long_options[] = {
		{"help", no_argument, NULL, 'h'},
		{"version", no_argument, NULL, 'v'},
		{"zoom", required_argument, NULL, 'z'},
		{"log", required_argument, NULL, 'l'},
		{NULL, 0, NULL, 0}
	};

	/* Initialize the arguments. */

	*zoom = DEFAULT_ZOOM_I;

	/* get all options */
	opterr = 0;
	while ((c = getopt_long(ac, av, short_options, long_options, NULL)) != -1)
	  switch (c) {
		/* Help, version, log level. */

		case 'h':
			help = 1;
			break;
		case 'v':
			version = 1;
			break;
		case 'l':
			s_log = optarg;
			break;

		/* Zoom. */

		case 'z':
			*zoom = (unsigned int)atoi(optarg);
			if (*zoom <= 0 || *zoom > 16) {
				fprintf(stderr, "-z, --zoom: should be between 1 and 16");
				return (1);
			}
			break;

		/* Error (ignore unknown options?). */

		case '?':
			if (optopt == 'z')
				fprintf(stderr, "-z, --zoom: expected an argument\n");
			else
				break;
			return (1);
	}

	/* Check if there is any parameter. */

	if (ac - optind)
		help = 1;

	/* Set the log level. */

	if (s_log)
		casio_setlog(s_log);

	/* Print help or version if required, and return. */

	if (version)
		puts(version_message);
	else if (help)
		put_help();
	else
		return (0);
	return (1);
}
