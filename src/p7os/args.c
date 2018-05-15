/* ****************************************************************************
 * p7os/args.c -- p7os command-line arguments parsing utility.
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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include <errno.h>

/* ---
 * Help and version messages.
 * --- */

/* The version message - that's when the President comes in */

static const char version_message[] =
BIN " - from " NAME " v" VERSION " (licensed under GPLv2)\n"
"Maintained by " MAINTAINER ".\n"
"\n"
"This is free software; see the source for copying conditions.\n"
"There is NO warranty; not even for MERCHANTABILITY or\n"
"FITNESS FOR A PARTICULAR PURPOSE.";

/* Main help message */

static const char help_main0[] =
"Usage: " BIN " [--version|-v] [--help|-h] [--com <device>]\n"
"            [--no-prepare] [--uexe <path>]\n"
"            <subcommand> [options...]\n"
"\n"
"This program interacts with a CASIO calculator's firmware.\n"
"Keep in mind that using it is HIGHLY DANGEROUS and could easily brick your\n"
"calculator if you aren't careful enough. AVOID USING IT IF YOU DO NOT\n"
"KNOW WHAT YOU'RE DOING.\n"
"\n"
"Subcommands you can use are :\n"
"   prepare-only      Set-up the update program, but leave it for other "
	"programs\n"
"                     to interact with it.\n"
"   get               Get the OS image.\n"
"   flash             Flash the OS image.\n"
"\n"
"General options:\n"
"  -h, --help         Display the help page of the (sub)command and quit.\n"
"  -v, --version      Display the version message and quit.\n";

static const char help_log[] =
"  --log <level>      The library log level (default: %s).\n"
"                     One of: %s";

static const char help_main1[] =
"  --com <device>     The serial device, if you want to communicate with a\n"
"                     calculator connected using a USB-to-serial cable.\n"
"                     If this option isn't used, the program will look for a\n"
"                     calculator connected using direct USB.\n"
"  --no-prepare       Use the current environment, instead of uploading one.\n"
"  -u, --uexe <path>  Use a custom update program.\n"
"                     If `--no-prepare` is not given, this option is "
	"required.\n"
"\n"
"Type \"" BIN " <subcommand> --help\" for some help about a subcommand.\n"
"Report bugs to " MAINTAINER ".";

/* Subcommands help messages footer. */

#define FOOT \
	"\nType \"" BIN " --help\" for other subcommands and general options."

/* Help message for prepare subcommand  */

static const char help_prepare_only[] =
"Usage: " BIN " prepare-only\n"
"Send the P7 server on the calculator for further operations.\n"
"This must be used before any other p7os operation.\n"
FOOT;

/* Help message for get subcommand */

static const char help_get[] =
"Usage: " BIN " get [-o <os.bin>]\n"
"Get the calculator OS image.\n"
"\n"
"Options are :\n"
"  -o <os.bin>    Where to store the image (default is \"os.bin\")\n"
FOOT;

/* Help message for flash subcommand. */

static const char help_flash[] =
"Usage: " BIN " flash <rom.bin>\n"
"Flash the calculator's OS image.\n"
FOOT;

/* ---
 * Put the help message.
 * --- */

/**
 *	put_help:
 *	Put the main help message.
 */

static void put_help(void)
{
	/* first big part */
	fputs(help_main0, stdout);

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
					printf(help_log, casio_getlog(), first);

				printf(", %s", current);
				pos++;
			}

			if (pos > 1)
				fputc('\n', stdout);

			free(first);
			casio_end(iter);
		}
	}

	/* second big part */
	puts(help_main1);
}

/* ---
 * Main argument parsing function.
 * --- */

/* Help macro. */

#define sub_init(CMD, NARGS) { \
	args->menu = mn_##CMD; \
	if (help || pc != (NARGS)) { \
		puts(help_##CMD); \
		return (1); \
	}}

/* Options. */

static const char shopts[] = "hvu:o:#";
static const struct option longopts[] = {
	{"help",             no_argument, NULL, 'h'},
	{"version",          no_argument, NULL, 'v'},
	{"com",        required_argument, NULL, 'c'},
	{"no-prepare",       no_argument, NULL, 'n'},
	{"uexe",       required_argument, NULL, 'u'},
	{"output",     required_argument, NULL, 'o'},
	{"log",        required_argument, NULL, 'l'},
	{NULL, 0, NULL, 0}
};

/**
 *	parse_args:
 *	Args parsing main function.
 *
 *	Based on my very first experiment with getopt.
 *
 *	@arg	ac		the arguments count
 *	@arg	av		the arguments values
 *	@arg	args	the parsed args pointer
 *	@return			if has been parsed successfully
 */

int parse_args(int ac, char **av, args_t *args)
{
	int c, help = 0, version = 0, pc;
	char **pv, *sub;
	const char *s_out = "os.bin", *s_uexe = NULL, *s_log = NULL;

	/* Initialize the arguments. */

	args->menu = 0;
	args->noprepare = 0;
	args->com = NULL;
	args->local = NULL;
	args->localpath = NULL;
	args->uexe = NULL;

	/* Get all options. */

	opterr = 0;
	while ((c = getopt_long(ac, av, shopts, longopts, NULL)) >= 0) switch (c) {
		case 'h':
			help = 1;
			break;
		case 'v':
			version = 1;
			break;

		/* COM port, should prepare or not. */

		case 'c':
			args->com = optarg;
			break;
		case 'n':
			args->noprepare = 1;
			break;

		/* log level, Update.Exe, output path */

		case 'l':
			s_log = optarg;
			break;
		case 'u':
			s_uexe = optarg;
			break;
		case 'o':
			s_out = optarg;
			break;

		/* Error. */

		case '?':
			if (optopt == 'o')
				fprintf(stderr, "-o, --output: expected an argument\n");
			else if (optopt == 'c')
				fprintf(stderr, "--com: expected an argument\n");
			else if (optopt == 'u')
				fprintf(stderr, "-u, --uexe: expected an argument\n");
			else
				break;
			return (1);
	}

	/* Check for version. */

	if (version) {
		puts(version_message);
		return (1);
	}

	/* Get non-option arguments (subcommand and parameters). */

	pc = ac - optind;
	pv = &av[optind];
	sub = pc ? pv[0] : NULL;
	pc--;
	pv++;

	/* Subcommand parsing. */

	char fpmode[3] = "r\0";
	if (!sub || !strcmp(sub, "help")) {
		put_help();
		return (1);
	} else if (!strcmp(sub, "version")) {
		puts(version_message);
		return (1);
	} else if (!strcmp(sub, "prepare-only")) {
		sub_init(prepare_only, 0)

		if (args->noprepare) {
			fprintf(stderr,
				"So we should prepare but we should not prepare? Duh!\n");
			return (1);
		}
	} else if (!strcmp(sub, "get")) {
		sub_init(get, 0)
		args->localpath = s_out;
		fpmode[0] = 'w';
	} else if (!strcmp(sub, "flash")) {
		sub_init(flash, 1)
		args->localpath = pv[0];
	} else {
		fprintf(stderr, "Unknown subcommand '%s'.\n", sub);
		return (1);
	}

	/* Open destination file. */

	if (args->localpath) {
		FILE *localfile = fopen(args->localpath, fpmode);
		if (!localfile) {
			fprintf(stderr, "Could not open local file: %s\n",
				strerror(errno));
			return (1);
		}

		int err = casio_open_stream_file(&args->local,
			fpmode[0] == 'r' ? localfile : NULL,
			fpmode[0] == 'w' || fpmode[1] == '+' ? localfile : NULL,
			1, 1);
		if (err) {
			fprintf(stderr, "Could not make a stream out of local file: %s\n",
				casio_strerror(err));
			return (1);
		}
	}

	/* Open update.exe file. */

	if (!args->noprepare) {
		FILE *uexe;
		int err;

		if (!s_uexe) {
			fprintf(stderr,
				"One of `-u <file>` or `--no-prepare` is expected!\n");
			if (args->local)
				casio_close(args->local);
			return (1);
		}

		uexe = fopen(s_uexe, "r");
		if (!uexe) {
			fprintf(stderr, "Could not open update program: %s\n",
				strerror(errno));
			if (args->local)
				casio_close(args->local);
			return (1);
		}

		err = casio_open_stream_file(&args->uexe,
			uexe, NULL, 1, 0);
		if (err) {
			fprintf(stderr,
				"Could not make a stream out of the update.exe: %s\n",
				casio_strerror(err));
			if (args->local)
				casio_close(args->local);
			return (1);
		}
	}

	/* Set the log level. */

	if (s_log)
		casio_setlog(s_log);

	return (0);
}
