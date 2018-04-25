/* *****************************************************************************
 * p7servtest/main.c -- p7servtest main source.
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
#include <unistd.h>

/**
 *	init:
 *	Let's go baby.
 *
 *	@arg	sin		the server input.
 *	@arg	sout	the server output.
 *	@arg	cin		the client input.
 *	@arg	cout	the client output.
 */

static void init(int sin, int sout, int cin, int cout)
{
	/* everybody do the fork */
	pid_t pid = fork();
	if (pid == (pid_t)-1)
		return ;

	/* yay */
	if (!pid) run_server(sin, sout);
	else {
		sleep(1);
		run_client(cin, cout);
	}
}

/**
 *	main:
 *	A handful of feet.
 *
 *	@arg	ac		the arguments count.
 *	@arg	av		the arguments values.
 *	@return			the error code (0 if ok).
 */

int main(int ac, char **av)
{
	/* parse the arguments */
	if (parse_args(ac, av))
		return (0);

	/* create the pipes */
	int fds1[2], fds2[2];
	if (pipe(fds1)) {
		fprintf(stderr,
			"Unable to make the first set of pipes (server to client)\n");
		return (1);
	}
	if (pipe(fds2)) {
		fprintf(stderr,
			"Unable to make the second set of pipes (client to server)\n");
		close(fds1[0]);
		close(fds1[1]);
		return (1);
	}

	/* initialize. */
	init(fds1[0], fds2[1], fds2[0], fds1[1]);

	/* everything went well. */
	return (0);
}
