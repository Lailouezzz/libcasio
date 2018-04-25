/* ****************************************************************************
 * cafix/main.c -- cafix main function.
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

/**
 *	main:
 *	Entry point.
 */

int main(int ac, char **av)
{
	args_t args;

	if (parse_args(ac, av, &args))
		return (1);

	switch (args.mode) {
	case MODE_CONTROL:
		fprintf(stderr,
			"This mode prints \"doesn't work yet, sorry\" in the original\n"
			"cafix, so I don't know what the original author meant...\n"
			"sorry if you were expecting something cool to happen!\n");
		return (1);

	default:
		fprintf(stderr, "Temporarily unmanaged mode.\n");
		return (1);
	}

	return (0);
}
