/* *****************************************************************************
 * mcsfile/args.c -- mcsfile command-line arguments parsing utility.
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
#include <getopt.h>
#define Q(x) #x
#define QUOTE(x) Q(x)

/* ---
 * Help and version messages.
 * --- */

/* Help message. */

static const char *help_start =
"Usage: mcsfile [--version|-v] [--help|-h] <g1m file>\n"
"\n"
"Reads mcs files in a g1m file.\n"
"\n"
"Options are:\n"
"  -h, --help        Display this help message.\n"
"  -v, --version     Display the version message.\n";

static const char *help_loglevel_init =
"  --log <level>     The library log level (default: %s).\n"
"                    One of: %s";

static const char *help_end =
"\n"
"Report bugs to " QUOTE(MAINTAINER) ".\n";

/* Version message. */

static const char *version_message =
"mcsfile - from p7utils v" QUOTE(VERSION) " (licensed under GPLv2)\n"
"Maintained by " QUOTE(MAINTAINER) ".\n"
"\n"
"This is free software; see the source for copying conditions.\n"
"There is NO warranty; not even for MERCHANTABILITY or\n"
"FITNESS FOR A PARTICULAR PURPOSE.\n";

/* ---
 * Main argument parsing functions.
 * --- */

/**
 *	put_loglevel:
 *	Put a loglevel (for listing).
 *
 *	@arg	initialized	if the list was initialized.
 *	@arg	level		the level string.
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
		printf(help_loglevel_init, casio_getlog(), *first + 1);
		**first = 'N';
	}

	printf(", %s", level);
}

/**
 *	put_help:
 *	Put the help message on standard output.
 */

static void put_help(void)
{
	char *first;

	/* First big part. */

	fputs(help_start, stdout);

	/* Loglevels. */

	first = NULL;
	casio_listlog((casio_log_list_t*)&put_loglevel, (void*)&first);
	if (first && *first == 'N') fputc('\n', stdout);
	free(first);

	/* Second big part. */

	fputs(help_end, stdout);
}

/**
 *	put_version:
 *	Put the version message on standard output.
 */

static void put_version(void)
{
	fputs(version_message, stdout);
}

/**
 *	parse_args:
 *	Args parsing main function.
 *
 *	@arg	ac		the arguments count.
 *	@arg	av		the arguments values.
 *	@arg	files	pointer to the file paths tab to allocate.
 *	@return			if execution should stop.
 */

int parse_args(int ac, char **av, const char **path)
{
	/* getopt elements */
	const char *optstring = "hv";
	const struct option longopts[] = {
		{"help", no_argument, NULL, 'h'},
		{"version", no_argument, NULL, 'v'},
		{}
	};

	/* get options */
	int c; opterr = 0;
	int help = 0, version = 0;
	while ((c = getopt_long(ac, av, optstring, longopts, NULL)) != -1)
	  switch (c) {
		case 'h': help = 1; break;
		case 'v': version = 1; break;
		default: switch (optopt) {
			default:
				fprintf(stderr, "-%c: unknown option.\n", optopt);
		}
		return (1);
	}

	/* check parameters */
	int pc = ac - optind;
	char **pv = &av[optind];
	if (pc != 1)
		help = 1;
	else
		*path = *pv;

	/* display version or help message */
	if (version) {
		put_version();
		return (1);
	} else if (help) {
		put_help();
		return (1);
	}

	/* no error */
	return (0);
}
