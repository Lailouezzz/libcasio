/* ****************************************************************************
 * p7os/main.c -- p7os main source.
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

/* Couldn't initialize connexion to calculator. */

static const char error_noconnexion[] =
"Could not connect to the calculator.\n"
"- Is it plugged in and in Receive mode/OS Update?\n"
"- Have you tried changing the cable ?\n";

/* Calculator was found but program wasn't allowed to communicate with it. */

static const char error_noaccess[] =
"Could not get access to the calculator.\n"
"Install the appropriate udev rule, or run as root.\n";

/* The calculator acted in a weird way. */

static const char error_unplanned[] =
"The calculator didn't act as planned: %s.\n"
"Stop receive mode on calculator and start it again before re-running " \
	BIN ".\n";

/* Unsupported operation -> OS Update, not receive mode! */

static const char error_unsupported[] =
"Required operation was unsupported by the calculator.\n"
"If you did not prepare, perhaps you should prepare?\n";

/**
 *	main:
 *	User entry point of the program.
 *
 *	@arg	ac		arguments count
 *	@arg	av		arguments values
 *	@return			return code (0 if OK)
 */

int main(int ac, char **av)
{
	int err; args_t args;

	/* Parse the arguments. */

	if (parse_args(ac, av, &args))
		return (0);

	/* Prepare. */

	if (!args.noprepare) {
		err = prepare(&args);
		if (err) goto fail;
	}

	/* Check according to menu. */

	switch (args.menu) {
	case mn_prepare_only:
		/* Nothing here. */
		break;
	case mn_get:
		err = backup_rom(&args);
		break;
	case mn_flash:
		err = fxremote_flash(&args);
		break;
	}

fail:
	/* Close the file, remove if necessary. */

	if (args.localpath) {
		casio_close(args.local);
		if (err && casio_iswritable(args.local))
			remove(args.localpath);
	}

	/* Display the error, if any. */

	switch (err) {
	case 0:
		break;
	case casio_error_nocalc:
		fprintf(stderr, error_noconnexion);
		break;
	case casio_error_noaccess:
		fprintf(stderr, error_noaccess);
		break;
	case casio_error_command:
		fprintf(stderr, error_unsupported);
		break;
	default:
		fprintf(stderr, error_unplanned, casio_strerror(err));
	}
	
	return (1);
}
