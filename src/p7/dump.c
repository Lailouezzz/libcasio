/* *****************************************************************************
 * p7/dump.c -- p7 information dumping utility.
 * Copyright (C) 2016-2017 Thomas "Cakeisalie5" Touhey <thomas@touhey.fr>
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
#include <inttypes.h>
#define numformat  "%lu"
#define addrformat "0x%08lX"

/**
 *	dump:
 *	Dump calculator information.
 *
 *	@arg	handle		the libp7 handle
 *	@return				the error code (0 if ok).
 */

int dump(casio_link_t *handle)
{
	/* get server info */
	const casio_link_info_t *info = casio_get_link_info(handle);

	/* Wiped out things */
	if (info->casio_link_info_wiped & casio_link_info_wiped_preprog)
		fprintf(stderr,
			"Warning: Preprogrammed ROM information looks wiped out!\n");
	if (info->casio_link_info_wiped & casio_link_info_wiped_bootcode)
		fprintf(stderr, "Warning: Bootcode information looks wiped out!\n");
	if (info->casio_link_info_wiped & casio_link_info_wiped_os)
		fprintf(stderr, "Warning: OS information looks wiped out!\n");
	if (!info->casio_link_info_username[0])
		fprintf(stderr, "Warning: Username is not set.\n");

	/* main information */
	printf("CPU ID (probably out of date): %s\n", info->casio_link_info_cpuid);
	printf("Environnement ID: %s\n", info->casio_link_info_hwid);
	printf("Product ID: %s\n", info->casio_link_info_product_id);

	/* Preprogrammed ROM */
	if (~info->casio_link_info_wiped & casio_link_info_wiped_preprog) {
		printf("Preprogrammed ROM version: %02u.%02u",
			info->casio_link_info_rom_version.casio_version_major,
			info->casio_link_info_rom_version.casio_version_minor);
		printf("\nPreprogrammed ROM capacity: " numformat "o\n",
			info->casio_link_info_rom_capacity);
	}

	/* ROM and RAM */
	printf("ROM capacity: " numformat "KiB\n",
		info->casio_link_info_flash_rom_capacity / 1024);
	printf("RAM capacity: " numformat "KiB\n",
		info->casio_link_info_ram_capacity / 1024);

	/* Bootcode */
	if (~info->casio_link_info_wiped & casio_link_info_wiped_bootcode) {
		printf("Bootcode version: %02u.%02u",
			info->casio_link_info_bootcode_version.casio_version_major,
			info->casio_link_info_bootcode_version.casio_version_minor);
		printf("\nBootcode offset: " addrformat "\n",
			info->casio_link_info_bootcode_offset);
		printf("Bootcode size: " numformat "KiB\n",
			info->casio_link_info_bootcode_size / 1024);
	}

	/* OS */
	if (~info->casio_link_info_wiped & casio_link_info_wiped_os) {
		printf("OS version: %02u.%02u",
			info->casio_link_info_os_version.casio_version_major,
			info->casio_link_info_os_version.casio_version_minor);
		printf("\nOS offset: " addrformat "\n",
			info->casio_link_info_os_offset);
		printf("OS size: " numformat "KiB\n",
			info->casio_link_info_os_size / 1024);
	}

	/* Miscallenous information */
	if (info->casio_link_info_username[0])
		printf("Username: %s\n", info->casio_link_info_username);

	return (0);
}
