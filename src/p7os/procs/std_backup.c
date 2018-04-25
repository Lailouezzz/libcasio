/* ****************************************************************************
 * p7os/procs/std_backup.c -- backup the ROM.
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
 *	backup_rom:
 *	Backup the ROM.
 *
 *	@arg	args	the CLI args.
 *	@return			the error code (0 if ok).
 */

int backup_rom(args_t *args)
{
	int err; casio_link_t *link = NULL;
	osdisp_t osdisp_cookie;

	/* Open the link. */
	err = open_link(&link, args,
		CASIO_LINKFLAG_ACTIVE | CASIO_LINKFLAG_CHECK | CASIO_LINKFLAG_TERM,
		NULL);
	if (err) return (err);

	/* Use the "standard" way. */
	osdisp_init(&osdisp_cookie, "Gathering the OS...", "Backed up!");
	err = casio_backup_rom(link, args->local, osdisp, &osdisp_cookie);
	if (err) { osdisp_interrupt(&osdisp_cookie); goto fail; }
	osdisp_success(&osdisp_cookie);

	err = 0;
fail:
	casio_close_link(link);
	return (err);
}
