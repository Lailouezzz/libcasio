/* ****************************************************************************
 * p7os/utils/open_link.c -- open the link based on cli arguments and other stuff.
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
 *	open_link:
 *	Open the link using the arguments.
 *
 *	@arg	link	the link to open.
 *	@arg	args	the command-line arguments.
 *	@arg	flags	the required flags.
 *	@arg	attrs	the serial attributes.
 *	@return			the error code (0 if ok).
 */

int open_link(casio_link_t **link, args_t *args,
	unsigned long flags, casio_streamattrs_t *attrs)
{
	int err;

	/* Open the link. */

	if (args->com)
		err = casio_open_com(link, flags, args->com, attrs);
	else
		err = casio_open_usb(link, flags, -1, -1);
	if (err)
		return (err);

	/* XXX: `--set`, `--use`? */
	return (0);
}
