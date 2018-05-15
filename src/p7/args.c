/* *****************************************************************************
 * p7/args.c -- p7 command-line argument parsing.
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
#include "main.h"
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* ---
 * Help and version messages.
 * --- */

/* Version message */

static const char version_message[] =
BIN " - from " NAME " v" VERSION
	" (licensed under GPLv2)\n"
"Maintained by " MAINTAINER ".\n"
"\n"
"This is free software; see the source for copying conditions.\n"
"There is NO warranty; not even for MERCHANTABILITY or\n"
"FITNESS FOR A PARTICULAR PURPOSE.";

/* Parts */

#define FOOT \
"\nType \"" BIN " --help\" for other subcommands and general options."

/* Sending help message */

static const char help_send[] =
"Usage: " BIN " send [-f] [-o <on-calc filename>]\n"
"               [-d <on-calc directory>] [-#] <local file>\n"
"Send a file to the calculator.\n"
"\n"
"Options are:\n"
"  -f, --force    Overwrite without asking\n"
"  -o <name>      The output filename on the calculator (by default, the same\n"
"                 as the local file)\n"
"  -d <dir>       The directory on-calc in which the file will be stored (by\n"
"                 default, the root directory)\n"
"  -#             Display a nice little loading bar\n"
FOOT;

/* Getting help message */

static const char help_get[] =
"Usage: " BIN " get [-o <local file>]\n"
"              [-d <on-calc directory>] <on-calc filename>\n"
"Request a file from the calculator.\n"
"\n"
"Options are:\n"
"  -o <name>   The output filename (by default, the same as the on-calc file)\n"
"  -d <dir>    The directory on-calc in which to get the file (by default,\n"
"              the root directory)\n"
"  -#          Display a nice little loading bar (if output isn't stdout)\n"
FOOT;

/* Copying help message */

static const char help_copy[] =
"Usage: " BIN " copy [-d <source directory]\n"
"               [-t <destination directory>] <source file> <dest file>\n"
"Copies a file into the other on the calculator.\n"
"\n"
"Options are:\n"
"  -d <srcdir>    The source directory (by default, the root directory)\n"
"  -t <dstdir>    The dest. directory (by default, the root directory)\n"
FOOT;

/* Deleting help message */

static const char help_del[] =
"Usage: " BIN " delete [-d <on-calc directory] <on-calc filename>\n"
"Delete a file on the calculator.\n"
"\n"
"Options are:\n"
"  -d <dir>    The directory on-calc in which to remove the file (by default,\n"
"              the root directory)\n"
FOOT;

/* Listing help message */

static const char help_list[] =
"Usage: " BIN " list\n"
"List files on the distant filesystem.\n"
FOOT;

/* Resetting help message */

static const char help_reset[] =
"Usage: " BIN " reset\n"
"Reset the distant filesystem.\n"
FOOT;

/* Optimizing help message */

static const char help_optimize[] =
"Usage: " BIN " optimize\n"
"Optimize the distant filesystem.\n"
FOOT;

/* Dumping help message */

static const char help_info[] =
"Usage: " BIN " info\n"
"Dump information about the calculator.\n"
FOOT;

/* List serial devices */

static const char help_listcom[] =
"Usage: " BIN " list-devices\n"
"List serial devices.\n"
FOOT;

/* Idle */

static const char help_idle[] =
"Usage: " BIN " idle|laze\n"
"Only initialize or end the communication.\n"
"\n"
"This subcommand is useful when used with `--no-init` and/or `--no-exit`.\n"
"It allows you to prepare the communication state for the next calls, or to\n"
"end a series of calls, without any side effects.\n"
FOOT;

/* Unlock */

static const char help_unlock[] =
"Usage: " BIN " unlock\n"
"Unlock examination mode.\n"
FOOT;

/* ---
 * Help helpers.
 * --- */

/* Main help message parts */

static const char help_main_part0[] =
"Usage: " BIN " [--version|-v] [--help|-h] [--no-init] [--no-exit]\n"
"          [--storage <fls0>] [--com <device>]\n"
"          <subcommand> [options...]\n"
"\n"
"Subcommands you can use are:\n"
"   send         Send a file to the calculator.\n"
"   get          Get a file from the calculator.\n"
"   copy         Copy a file into another on the calculator.\n"
"   delete       Delete a file on the calculator.\n"
"   reset        Reset the flash memory.\n"
"   optimize     Optimize the distant filesystem.\n"
"   list         List files on the distant filesystem.\n"
"   info         Dump info about the calculator.\n"
"   idle         Do nothing. (only start or end the communication)\n"
"   unlock       Unlock examination mode.\n"
"\n"
"General options:\n"
"  -h, --help        Display the help page of the (sub)command and quit.\n"
"  -v, --version     Display the version message and quit.\n"
"  --com <device>    The serial device, if you want to communicate with a\n"
"                    calculator connected using a USB-to-serial cable.\n"
"                    If this option isn't used, the program will look for a\n"
"                    calculator connected using direct USB.\n"
"  --storage <abc0>  The storage device with which to interact (fls0, crd0).\n"
"                    Default storage device is '" DEFAULT_STORAGE "'.\n"
"  --no-exit         Does not terminate connection when action is completed.\n"
"  --no-init         Does not initialize connection (should only be used\n"
"                    when --no-exit was used last time p7 was called).\n"
"  --use <settings>  Use the following serial settings (when used with `--com`).\n"
"                    For example, \"9600N2\" represents 9600 bauds, no parity,\n"
"                    and two stop bits. (E for even parity, O for odd parity)\n"
"  --set <settings>  Set the following serial settings (when used with `--com`).\n"
"                    The string has the same format than for `--use`.\n";

static const char help_main_loglevel_init[] =
"  --log <level>     The library log level (default: %s).\n"
"                    One of: %s";

static const char help_main_part1[] =
"  --reset           Reset the default communication settings (9600N2).\n"
"\n"
"Type \"" BIN " <subcommand> --help\" for some help about the subcommand.\n"
"Report bugs to " MAINTAINER ".\n";

/**
 *	put_main_help:
 *	Put the main help on standard output.
 */

static void put_main_help(void)
{
	/* First big part. */

	fputs(help_main_part0, stdout);

	/* Loglevels. */

	{
		casio_iter_t *iter;
		char *first = NULL, *current;
		int pos = 0;

		if (!casio_iter_log(&iter)) {
			while (!casio_next_log(iter, &current)) {
				if (!first) {
					size_t len = strlen(current) + 1;

					first = malloc(len);
					if (!first)
						break ;
					memcpy(first, current, len);
					pos++;
					continue ;
				}

				if (pos == 1)
					printf(help_main_loglevel_init, casio_getlog(), first);

				printf(", %s", current);
				pos++;
			}

			if (pos > 1)
				fputc('\n', stdout);

			free(first);
			casio_end(iter);
		}
	}

	/* Second big part. */

	fputs(help_main_part1, stdout);
}

/* ---
 * Main argument parsing function.
 * --- */

/* Useful macros. */

#define sub_init(CMD, NARGS) \
	args->menu = mn_##CMD; \
	if (help || aac != 1 + (NARGS)) { \
		puts(help_##CMD); \
		return (0); \
	}

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
	int c, help = 0, rst = 0;
	const char *s_out = NULL, *s_dir = NULL, *s_todir = NULL;
	const char *s_use = NULL, *s_set = NULL, *s_log = NULL;
	char short_opts[] = "hvfo:d:t:#";
	struct option long_opts[] = {
		{"help",            no_argument, NULL, 'h'},
		{"version",         no_argument, NULL, 'v'},
		{"com",       required_argument, NULL, 'c'},
		{"storage",   required_argument, NULL, 's'},
		{"force",           no_argument, NULL, 'f'},
		{"output",    required_argument, NULL, 'o'},
		{"directory", required_argument, NULL, 'd'},
		{"to",        required_argument, NULL, 't'},
		{"no-init",         no_argument, NULL, 'i'},
		{"no-start",        no_argument, NULL, 'i'},
		{"no-exit",         no_argument, NULL, 'e'},
		{"no-term",         no_argument, NULL, 'e'},
		{"set",       required_argument, NULL, 'S'},
		{"reset",           no_argument, NULL, 'R'},
		{"use",       required_argument, NULL, 'U'},
		{"log",       required_argument, NULL, 'L'},

		/* sentinel */
		{NULL, 0, NULL, 0}
	};

	/* initialize args */
	args->menu = 0;
	args->nicedisp = 0;
	args->dirname = NULL;
	args->filename = NULL;
	args->newdir = NULL;
	args->newname = NULL;
	args->local = NULL;
	args->force = 0;
	args->com = 0;
	args->storage = DEFAULT_STORAGE;
	args->initflags = CASIO_LINKFLAG_ACTIVE | CASIO_LINKFLAG_CHECK
		| CASIO_LINKFLAG_TERM;
	args->use = NULL;
	args->set = NULL;
	args->do_the_set = 0;

	/* get all options */
	opterr = 0;
	while ((c = getopt_long(ac, av, short_opts, long_opts, NULL)) != -1) {
		switch (c) {
		/* help */
		case 'h': help = 1; break;
		/* version */
		case 'v': puts(version_message); return (0); break;
		/* force */
		case 'f': args->force = 1; break;
		/* nice display */
		case '#': args->nicedisp = 1; break;
		/* logging */
		case 'L': s_log = optarg; break;

		/* output file (on calc or local) */
		case 'o': s_out = optarg; break;
		/* directory name */
		case 'd': s_dir = optarg; break;
		/* destination directory name */
		case 't': s_todir = optarg; break;

		/* com port */
		case 'c': args->com = optarg; break;
		/* storage */
		case 's': args->storage = optarg; break;
		/* force no initialization */
		case 'i': args->initflags &= ~CASIO_LINKFLAG_CHECK; break;
		/* force no exit */
		case 'e': args->initflags &= ~CASIO_LINKFLAG_TERM; break;

		/* use and set settings */
		case 'U': s_use = optarg; break;
		case 'S': s_set = optarg; break;
		case 'R': rst = 1; break;

		/* in case of error */
		case '?':
			if (optopt == 'o')
				fprintf(stderr, "-o, --output: expected an argument\n");
			else if (optopt == 'd')
				fprintf(stderr, "-d, --directory: expected an argument\n");
			else if (optopt == 't')
				fprintf(stderr, "-t, --to: expected an argument\n");
			else if (optopt == 'c')
				fprintf(stderr, "--com: expected an argument\n");
			else if (optopt == 's')
				fprintf(stderr, "--storage: expected an argument\n");
			else
				break;
			return (0);
			break;
		}
	}

	/* get non-option arguments (subcommand and parameters) */
	int aac = ac - optind;
	char **aav = &av[optind];

	/* get subcommand and things to check */
	char fpmode[2] = " ";
	args->localpath = NULL;
	/* - all subcommands - */
	if (!aac || !strcmp(aav[0], "help")) {
		put_main_help();
		return (0);
	} else if (!strcmp(aav[0], "version")) {
		puts(version_message);
		return (0);
	} else if (!strcmp(aav[0], "list-devices")) {
		if (help || aac > 1) puts(help_listcom);
		else list_devices();
		return (0);
	} else if (!strcmp(aav[0], "info")) {
		sub_init(info, 0)
	} else if (!strcmp(aav[0], "list") || !strcmp(aav[0], "ls")) {
		sub_init(list, 0)
	} else if (!strcmp(aav[0], "reset")) {
		sub_init(reset, 0)
	} else if (!strcmp(aav[0], "optimize")) {
		sub_init(optimize, 0)
	} else if (!strcmp(aav[0], "send")) {
		sub_init(send, 1)

		/* put arguments to check */
		fpmode[0] = 'r';
		args->localpath = aav[1];
		args->dirname = s_dir ? s_dir : NULL;
		if (s_out) args->filename = s_out;
		else {
			char *rs = strrchr(args->localpath, '/');
			args->filename = rs ? rs + 1 : args->localpath;
		}
	} else if (!strcmp(aav[0], "get")) {
		sub_init(get, 1)

		/* put arguments to check */
		fpmode[0] = 'w';
		args->filename = aav[1];
		args->dirname = s_dir;
		args->localpath = s_out ? s_out : args->filename;
	} else if (!strcmp(aav[0], "copy")) {
		sub_init(copy, 2)

		/* get filename */
		args->filename = aav[1];
		args->dirname = s_dir;
		args->newname = aav[2];
		args->newdir = s_todir;
	} else if (!strcmp(aav[0], "del") || !strcmp(aav[0], "delete")) {
		sub_init(del, 1)

		/* get filename */
		args->filename = aav[1];
		args->dirname = s_dir;
	} else if (!strcmp(aav[0], "idle") || !strcmp(aav[0], "laze")) {
		sub_init(idle, 0)
	} else if (!strcmp(aav[0], "unlock")) {
		sub_init(unlock, 0)
	} else {
		/* unknown subcommand ! */
		fprintf(stderr, "Unknown subcommand '%s'.\n", aav[0]);
		return (0);
	}

	/* check string lengths */
	int noerror = 0;
	if (args->filename && !memchr(args->filename, '\0', 13))
		fprintf(stderr, "On-calc filename must have 12 chars or less!\n");
	else if (args->newname && !memchr(args->newname, '\0', 13))
		fprintf(stderr, "Destination filename must have 12 chars or less!\n");
	else if (args->dirname && !memchr(args->dirname, '\0', 9))
		fprintf(stderr, "On-calc directory name must have 8 chars or less!\n");
	else if (args->newdir && !memchr(args->newdir, '\0', 9))
		fprintf(stderr,
			"Destination directory name must have 8 chars or less!\n");
	else if (!memchr(args->storage, '\0', 5))
		fprintf(stderr, "Storage device (%s) should be 4 chars long!\n",
			args->storage);
	else
		noerror = 1;
	if (!noerror) return (0);

	/* use serial settings */
	if (s_use) {
		if (args->com) args->use = &args->_use;
		if (casio_make_attrs(&args->_use, s_use)) {
			fprintf(stderr, "--use: invalid format!\n");
			fprintf(stderr, "--use: expected <speed><parity><stopbits>, "
				"e.g. 9600N2 or 115200E1!\n");
			return (0);
		}
	}

	/* set serial settings */
	if (rst) {
		args->do_the_set = 1;
		args->set = &args->_set;
		casio_make_attrs(&args->_set, NULL);
	} else if (s_set) {
		args->do_the_set = 1;
		if (args->com) args->set = &args->_set;
		if (casio_make_attrs(&args->_set, s_set)) {
			fprintf(stderr, "--set: invalid format!\n");
			fprintf(stderr, "--set: expected <speed><parity><stopbits>, "
				"e.g. 9600N2 or 115200E1!\n");
			return (0);
		}
	}

	/* check local path */
	if (args->localpath) {
		if (fpmode[0] == 'w' && !strcmp(args->localpath, "-"))
			args->local = stdout;
		else if (!(args->local = fopen(args->localpath, fpmode))) {
			fprintf(stderr, "Could not open local file : %s\n",
				strerror(errno));
			if (fpmode[0] == 'w')
				remove(args->localpath);
			return (0);
		}
	}

	/* set the log level */
	if (s_log)
		casio_setlog(s_log);

	/* everything went well */
	return (1);
}
