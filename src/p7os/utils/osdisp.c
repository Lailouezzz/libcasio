/* ****************************************************************************
 * p7os/utils/osdisp.c -- progress displayer.
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
#include "../main.h"
#include <string.h>

/* ---
 * Main function.
 * --- */

/**
 *	osdisp:
 *	Nice little loading bar.
 *
 *	Taken from `src/p7/main.c`.
 *	"Initialization" is when id > total (called in main).
 *
 *	@arg	id		data packet ID.
 *	@arg	total	total number of packets.
 */

void osdisp(void *vcookie, unsigned int id, unsigned int total)
{
	osdisp_t *cookie = (void*)vcookie;
	int current; unsigned int percent;

	/* if is initialize, fill */
	if (id > total) {
		/* initialize */
		strcpy(cookie->buf,
			"\r|---------------------------------------| 00.00%");
		cookie->bar = &cookie->buf[2];
		cookie->pos = 0;
		cookie->init = 1;

		/* put message and initial buffer. */
		printf("%s\n%s", cookie->msg, cookie->buf);

		/* save cursor position and we're done */
		fputs("\x1B[s", stdout);
		return ;
	}

	/* id and total start from 1, let them start from zero */
	id--; total--;

	/* modify buffer */
	/* - # - */
	current = 38 * id / total;
	while (cookie->pos <= current) cookie->bar[cookie->pos++] = '#';
	/* - % - */
	percent = 10000 * id / total;
	sprintf(&cookie->buf[43], "%02u.%02u",
		percent / 100 % 100, percent % 100);

	/* put it */
	printf("%s", cookie->buf);
	/* force cursor position */
	fputs("\x1B""8", stdout);
	fflush(stdout);
}

/* ---
 * Initialization, miscallaneous actions.
 * --- */

/**
 *	osdisp_init:
 *	Initialize a display cookie.
 *
 *	@arg	cookie		the display cookie to initialize.
 *	@arg	init		the initial message.
 *	@arg	success		the success message.
 */

void osdisp_init(osdisp_t *cookie, const char *init, const char *success)
{
	cookie->msg = init;
	cookie->success = success;
	cookie->init = 0;
	cookie->pos = 0;
}

/**
 *	osdisp_interrupt:
 *	End with an error.
 *
 *	@arg	cookie		the display cookie.
 */

void osdisp_interrupt(osdisp_t *cookie)
{
	/* interrupt loading bar */
	if (cookie->init)
		printf("\b\b\b\b\b\bError!\n");
}

/**
 *	osdisp_success:
 *	End with a success.
 *
 *	@arg	cookie		the display cookie.
 */

void osdisp_success(osdisp_t *cookie)
{
	printf("\b\b\b\b\b\b%s\n", cookie->success);
}
