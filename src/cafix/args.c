/* ****************************************************************************
 * cafix/args.c -- cafix command-line argument parsing.
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
#include "main.h"
#include <getopt.h>

/* Version message. */

static const char version_message[] =
BIN " - from " NAME " v" VERSION
	" (licensed under GPLv2)\n"
"Maintained by " MAINTAINER ".\n"
"\n"
"This is free software; see the source for copying conditions.\n"
"There is NO warranty; not even for MERCHANTABILITY or\n"
"FITNESS FOR A PARTICULAR PURPOSE.";

static void put_version(void)
{
	fputs(version_message, stderr);
}

/* Help message. */

static const char help_begin[] =
"Usage: " BIN " [OPTIONS]... [SOURCE | DESTPATH]\n"
"\n"
"Remake of the 90s utility known as the Calculator File-Xferer, made by\n"
"Tom Lynn, and which used to work with the CFX-9850 GB PLUS and the \n"
"CFX-9990GT, \"maybe other as well\".\n"
"\n"
"Available options:\n"
"  -h,  --help       Display this help page and exit.\n"
"  -v,  --version    Display the version message and exit.\n";

static const char help_loglevel_init[] =
"  --log <level>     The library log level (default: %s).\n"
"                    One of: %s";

static const char help_end[] =
"  -r,  --receive    Try to receive data.\n"
"  -s,  --send       Send a file.\n"
"  -rn, --rename     Prompts for filename (must be combined with -s or -r).\n"
"  -c,  --control    Remote control mode.\n"
"\n"
"Report bugs to " MAINTAINER ".\n";

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
		if (!*first)
			return ;
		strcpy(*first + 1, level);
		**first = 'F';
		return ;
	}

	if (**first == 'F') {
		fprintf(stderr, help_loglevel_init, casio_getlog(), *first + 1);
		**first = 'N';
	}

	fprintf(stderr, ", %s", level);
}

/**
 *	put_help:
 *	Put the help message on standard output.
 */

static void put_help(void)
{
	char *first;

	/* Put the beginning. */

	fputs(help_begin, stderr);

	/* Put the loglevels. */

	first = NULL;
	casio_listlog((casio_log_list_t*)&put_loglevel, (void*)&first);
	if (first && *first == 'N')
		fputc('\n', stderr);
	free(first);

	/* Put the end. */

	fputs(help_end, stderr);
}

/* ---
 * Main argument parsing function.
 * --- */

/**
 *	parse_args:
 *	Args parsing main function.
 *
 *	Was my very first experiment with getopt.
 *	Then I took an arrow in the knee.
 *
 *	@arg	ac		the arguments count
 *	@arg	av		the arguments values
 *	@arg	args	the parsed args pointer
 *	@return			if it was successfully parsed
 */

int parse_args(int ac, char **av, args_t *args)
{
	int c, help = 0, pm;
	const char *s_log = NULL;
	char **pv;
	char short_opts[] = "hvrsc";
	struct option long_opts[] = {
		{"help",            no_argument, NULL, 'h'},
		{"version",         no_argument, NULL, 'v'},
		{"log",       required_argument, NULL, 'L'},
		{"receive",         no_argument, NULL, 'r'},
		{"send",            no_argument, NULL, 's'},
		{"rn",              no_argument, NULL, 'R'},
		{"rename",          no_argument, NULL, 'R'},
		{"control",         no_argument, NULL, 'c'},

		/* sentinel */

		{NULL, 0, NULL, 0}
	};

	/* Initialize the arguments. */

	args->mode = 0; /* unknown menu. */
	args->should_rename = 0;
	args->filename = NULL;

	/* Gather the options. */

	opterr = 0;
	while ((c = getopt_long_only(ac, av, short_opts, long_opts, NULL)) != -1)
	  switch (c) {
		/* Help and version message. */

		case 'h':
			help = 1;
			break;
		case 'v':
			put_version();
			return (1);

		/* Log level. */

		case 'L':
			s_log = optarg;
			break;

		/* Command. */

		case 'r':
			if (args->mode != MODE_CONTROL)
				args->mode = MODE_RECEIVE;
			break;
		case 's':
			if (args->mode != MODE_CONTROL)
				args->mode = MODE_SEND;
			break;
		case 'c':
			args->mode = MODE_CONTROL;
			break;

		case 'R':
			args->should_rename = 1;
			break;

		/* In case of error. */

		case '?':
			if (optopt == 'L')
				fprintf(stderr, "--log: expected an argument\n");
			else
				break;
			return (1);
	}

	/* Get the parameters. */

	pm = ac - optind;
	pv = &av[optind];

	if (help) {
		put_help();
		return (1);
	}

	if (args->mode == 0) {
		fprintf(stderr, "error: no command specified\n");
		return (1);
	}

	switch (args->mode) {
	case MODE_SEND:
		if (pm != 1) {
			fprintf(stderr, "error: path of the file to send required\n");
			return (1);
		}
		args->filename = *pv;
		break;

	case MODE_RECEIVE:
		if (pm != 1) {
			fprintf(stderr, "error: path of the file to receive required\n");
			return (1);
		}
		args->filename = *pv;
		break;

	case MODE_CONTROL:
		if (pm) {
			fprintf(stderr, "error: too many parameters\n");
			return (1);
		}
		if (args->should_rename) {
			fprintf(stderr, "error: cannot rename and control\n");
			return (1);
		}
		break;
	}

	args->filename = *pv;

	/* Set the log level. */

	if (s_log)
		casio_setlog(s_log);

	/* Everything went well. */

	return (0);
}
