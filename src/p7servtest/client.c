/* *****************************************************************************
 * p7servtest/client.c -- p7servtest virtual client.
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

int run_client(int in, int out)
{
	int err; casio_link_t *link = NULL;
	casio_stream_t *stream = NULL;

	/* make the stream */
	err = casio_open_stream_fd(&stream, in, out, 1, 1);
	if (err) {
		fprintf(stderr, "Client stream initialization has encountered "
			"an error: %s\n", casio_strerror(err));
		return (1);
	}

	/* make the handle */
	err = casio_open_link(&link,
		CASIO_LINKFLAG_ACTIVE | CASIO_LINKFLAG_CHECK | CASIO_LINKFLAG_TERM,
		stream, NULL);
	if (err) {
		fprintf(stderr, "Client initialization has encountered an error: %s\n",
			casio_strerror(err));
		return (1);
	}

	/* change working directory. */
	casio_seven_send_cmdfls_cwd(link, "oui", "fls0");
	casio_seven_send_cmdfls_cwd(link, "non", "fls0");

	/* we're done. */
	casio_close_link(link);
	return (0);
}
