/* *****************************************************************************
 * p7servtest/server.c -- p7servtest virtual server.
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
#include <string.h>

/* ************************************************************************** */
/*  Server callbacks                                                          */
/* ************************************************************************** */
/**
 *	directory_exists:
 *	Check if a directory exists.
 *
 *	@arg	cookie		the cookie.
 *	@arg	dirname		the directory.
 *	@arg	devname		the devname.
 *	@return				the error (0 if ok).
 */

static int directory_exists(void *cookie, const char *dirname)
{
	(void)cookie;

	/* check directory name */
	if (!strcmp(dirname, "oui"))
		return (0);
	else return (casio_error_notfound);
}
/* ************************************************************************** */
/*  Server configuration                                                      */
/* ************************************************************************** */
/* the server information */
static casio_link_info_t server_information = {
	/* main calculator information */
	.casio_link_info_cpuid = "The CPU ID, wow!",
	.casio_link_info_hwid = "TESTSERV",
	.casio_link_info_product_id = "OMGOMGOMGOMGOMGO",

	/* system configuration */
	.casio_link_info_username = "Cow",

	/* wiped things */
	.casio_link_info_wiped =
		casio_link_info_wiped_preprog | casio_link_info_wiped_bootcode,

	/* flash ROM and RAM information */
	.casio_link_info_flash_rom_capacity = 8 * 1024 * 1024,
	.casio_link_info_ram_capacity = 256 * 1024,

	/* OS information */
	.casio_link_info_os_offset = 0x80000000,
	.casio_link_info_os_size = 0x100000,
	.casio_link_info_os_version = {
		.casio_version_major = 2, .casio_version_minor = 9,
		.casio_version_zone = casio_version_zone_fr,
		.casio_version_math = casio_version_math_all,
		.casio_version_status = casio_version_status_standard,
		.casio_version_platform = casio_version_platform_special
	},
};

/* the server filesystems */
static casio_filesystem_t server_filesystems[] = {
	/* main filesystem: the flash */ {
		.casio_filesystem_name = "fls0",
		.casio_filesystem_directory_exists = directory_exists
	},
	{NULL}
};
/* ************************************************************************** */
/*  Server functions                                                          */
/* ************************************************************************** */
/**
 *	run_server:
 *	Run the server!
 *
 *	@arg	in		the input.
 *	@arg	out		the output.
 */

int run_server(int in, int out)
{
	int err; casio_link_t *handle = NULL;
	casio_stream_t *stream = NULL;

	/* make the stream */
	err = casio_open_stream_fd(stream, in, out, 1, 1);
	if (err) {
		fprintf(stderr, "Server stream initialization has encountered "
			"an error: %s\n", p7_strerror(err));
		return (1);
	}

	/* make the handle */
	err = casio_open_link(&handle, 0, stream, NULL);
	if (err) {
		fprintf(stderr, "Server initialization has encountered an error: %s\n",
			p7_strerror(err));
		return (1);
	}

	/* protect and serve */
	casio_serve(handle, &server_information, server_filesystems);
	casio_close_link(handle);
	return (0);
}
