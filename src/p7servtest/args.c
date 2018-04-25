/* *****************************************************************************
 * p7servtest/args.c -- p7servtest command-line argument parsing utility.
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

/* ************************************************************************** */
/*  Help and version messages                                                 */
/* ************************************************************************** */
/* Version message */
static const char version_message[] =
QUOTE(BIN) " - from " QUOTE(NAME) " v" QUOTE(VERSION)
	" (licensed under GPLv2)\n"
"Maintained by " QUOTE(MAINTAINER) ".\n"
"\n"
"This is free software; see the source for copying conditions.\n"
"There is NO warranty; not even for MERCHANTABILITY or\n"
"FITNESS FOR A PARTICULAR PURPOSE.";

/* Main help message */
static const char help_main[] =
"Usage: " QUOTE(BIN) " [--help] [--version]\n"
"\n"
"This utility is only there to test the libcasio server feature.\n"
"It should not be used in production for anything else.\n"
"\n"
"General options:\n"
"  -h, --help        Display the help page of the (sub)command and quit.\n"
"  -v, --version     Display the version message and quit.\n"
"\n"
"Report bugs to " QUOTE(MAINTAINER) ".";

/* ************************************************************************** */
/*  Main args parsing function                                                */
/* ************************************************************************** */
/**
 *	parse_args:
 *	Args parsing main function.
 *
 *	@arg	ac		the arguments count.
 *	@arg	av		the arguments values.
 *	@return			if execution should stop after this call.
 */

int parse_args(int ac, char **av)
{
	/* define options */
	const char shopts[] = "hv";
	const struct option longopts[] = {
		{"help", no_argument, NULL, 'h'},
		{"version", no_argument, NULL, 'v'},
		{NULL, 0, NULL, 0}
	};

	/* get all options */
	int c; opterr = 0;
	int help = 0, version = 0;
	while ((c = getopt_long(ac, av, shopts, longopts, NULL)) >= 0) switch (c) {
		case 'h': help = 1; break;
		case 'v': version = 1; break;

		case '?': break;
	}

	/* get parameters */
	int pc = ac - optind;
	if (pc) help = 1;

	/* print help or version */
	if (help) {
		puts(help_main);
		return (1);
	} else if (version) {
		puts(version_message);
		return (1);
	}

	/* no problem */
	return (0);
}
