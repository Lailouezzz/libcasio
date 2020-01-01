/* ****************************************************************************
 * p7os/procs/std_prepare.c -- upload and run the update.exe.
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

/**
 *	prepare:
 *	Upload and run the given update.exe.
 *
 *	@arg	args	the command-line arguments.
 *	@return			the error code (0 if ok).
 */

int prepare(args_t *args)
{
	int err; casio_link_t *link = NULL;
	osdisp_t osdisp_cookie;
	casio_off_t usize;

	/* Get the update.exe size. */
	err = casio_getsize(args->uexe, &usize);
	if (err) return (err);

	/* Open the link. */
	err = open_link(&link, args,
		CASIO_LINKFLAG_ACTIVE | CASIO_LINKFLAG_CHECK, NULL);
	if (err) goto fail;

	/* Upload and run. */
	osdisp_init(&osdisp_cookie, "Uploading the Update.Exe...", "Uploaded!");
	err = casio_upload_and_run(link, args->uexe, usize,
		0x88024000, 0x88024000, osdisp, &osdisp_cookie);
	if (err) {
		/* interrupt loading bar */
		osdisp_interrupt(&osdisp_cookie);
		goto fail;
	}
	osdisp_success(&osdisp_cookie);

	/* Sleep while the software on the calculator sets up the
	 * communication interface. */
	printf("Waiting for the Update.Exe to set up the communication...\n");
	casio_sleep(3000);

	err = 0;
fail:
	if (link) casio_close_link(link);
	casio_close(args->uexe);
	return (err);
}
