/* *****************************************************************************
 * g1a-wrapper/args.c -- g1a-wrapper command-line argument parsing.
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
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* ---
 * Help and version messages.
 * --- */

/* Version message. */

static const char version_message[] =
BIN " - from " NAME " v" VERSION
	" (licensed under GPLv2)\n"
"Maintained by " MAINTAINER ".\n"
"\n"
"This is free software; see the source for copying conditions.\n"
"There is NO warranty; not even for MERCHANTABILITY or\n"
"FITNESS FOR A PARTICULAR PURPOSE.";

/* Main help message. */

static const char help_message[] =
"Usage: " BIN " [-v] [--help|-h] <addin.bin>\n"
"          [-o <output>] [-i <icon>] [-n <name>]\n"
"          [--version <MM.mm.pppp>] [--internal <@ADDIN>]\n"
"          [--date <yyyy.MMdd.hhmm]\n"
"\n"
"General options:\n"
"  -h, --help        Display the help page of the (sub)command and quit.\n"
"  -v, --ver         Display the version message and quit.\n"
"  -o <output>       Output file name, default is 'addin.g1a'.\n"
"  -i <icon file>    The application icon, must be 30x19.\n"
"                    Default is blank.\n"
"  -n <name>         The application name. '[A-Z]{,8}' formatted.\n"
"                    Default name is the truncated output filename.\n"
"  --version <ver>   The application version, 'MM.mm.pppp' formatted.\n"
"                    Default is '00.00.0000'.\n"
"  --internal <int>  The application internal name. '@[A-Z]{,7}' formatted.\n"
"                    Default is '@ADDIN'.\n"
"  --date <date>     The application build date. 'yyyy.MMdd.hhmm' formatted.\n"
"                    Default is the current time.\n"
"\n"
"Report bugs to " MAINTAINER ".";

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
	static char name[9];
	const char *s_ver = NULL, *s_date = NULL, *p, *e;
	int i, c, pc, help = 0;
	char **pv;
	const char *short_opts = "hvo:i:n:";
	struct option long_opts[] = {
		{"help",     no_argument,       NULL, 'h'},
		{"version",  required_argument, NULL, 'V'},
		{"internal", required_argument, NULL, 'I'},
		{"date",     required_argument, NULL, 'D'},
		{NULL,       0,                 NULL, 0},
	};

	/* Initialize the arguments. */

	args->outfile = "addin.g1a";
	args->iconfile = NULL;
	args->name = name;
	args->intname = "@ADDIN";
	args->version.casio_version_major = 0;
	args->version.casio_version_minor = 0;
	args->version.casio_version_zone = casio_version_zone_none;
	args->version.casio_version_math = casio_version_math_all;
	args->version.casio_version_status = casio_version_status_standard;
	args->version.casio_version_platform = casio_version_platform_special;
	args->date = time(NULL);

	/* get all options */
	opterr = 0;
	while ((c = getopt_long(ac, av, short_opts, long_opts, NULL)) != -1)
	  switch (c) {
		/* Help and version flags. */

		case 'h':
			help = 1;
			break;
		case 'v':
			puts(version_message);
			return (1);

		/* General options. */

		case 'o':
			args->outfile = optarg;
			break;
		case 'i':
			args->iconfile = optarg;
			break;

		/* Build options. */

		case 'n':
			args->name = optarg;
			break;
		case 'V':
			s_ver = optarg;
			break;
		case 'I':
			args->intname = optarg;
			break;
		case 'D':
			s_date = optarg;
			break;

		/* in case of error */
		case '?':
			switch (optopt) {
				case 'o':
					err("-o: expected an argument");
					return (1);
				case 'i':
					err("-i: expected an argument");
					return (1);
				case 'n':
					err("-n: expected an argument");
					return (1);
				case 'V':
					err("--version: expected an argument");
					return (1);
				case 'I':
					err("--internal: expected an argument");
					return (1);
				case 'D':
					err("--date: expected an argument");
					return (1);
				default:
					break;
			}
			break;
	}

	/* Get parameters. */

	pc = ac - optind;
	pv = &av[optind];

	if (pc != 1)
		help = 1;
	args->infile = pv[0];

	/* Put the help message. */

	if (help) {
		puts(help_message);
		return (1);
	}

	/* Get the name.
	 * TODO: manage MS-Windows like paths. */

	p = strrchr(pv[0], '/');
	if (!p)
		p = pv[0];

	e = strrchr(p, '.');
	name[0] = 0;
	for (int i = 0; p < e && i < 8; p++) {
		if (isupper(*p)) {
			name[i++] = *p;
			name[i] = 0;
		}
	}
	if (!name[0])
		strcpy(name, "ADDIN");

	/* Check the internal name. */

	if (args->intname[0] != '@') {
		err("internal name should start with an '@'");
		return (1);
	} for (i = 1; args->intname[i]; i++) {
		if (i >= 8) {
			err("internal name should be up to 8 characters only!");
			return (1);
		}

		if (!isupper(args->intname[i])) {
			err("internal name should be an '@' then up to seven uppercase "
				"letters!");
			return (1);
		}
	}

	/* Get the version. */

	if (s_ver) {
		if (casio_check_version(s_ver)) {
			err("version string '%s' does not have "
				"expected format 'MM.mm.ffff'", s_ver);
			return (1);
		}

		casio_decode_version(&args->version, s_ver);
	}

	/* Get the date. */

	if (s_date) {
		if (casio_decode_date(&args->date, s_date)) {
			err("date string '%s' does not have "
				"expected format 'yyyy.MMdd.hhmm'", s_date);
			return (1);
		}
	}

	return (0);
}
