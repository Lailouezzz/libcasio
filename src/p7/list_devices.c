/* *****************************************************************************
 * p7/list_devices.c -- p7 device listing functions.
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

/**
 *	list_a_device:
 *	List one serial device.
 *
 *	@arg	cookie		the cookie (unused)
 *	@arg	path		the path.
 */

static int initialized = 0;
static void list_a_device(void *cookie, const char *path)
{
	(void)cookie;
	if (!initialized) {
		printf("Available devices:\n");
		initialized = 1;
	}
	printf("- %s\n", path);
}

/**
 *	list_devices:
 *	List serial devices.
 *
 *	@return		the program return code.
 */

int list_devices(void)
{
	casio_comlist(list_a_device, NULL);
	if (!initialized)
		fprintf(stderr, "Could not find any devices.\n");
	return (0);
}
