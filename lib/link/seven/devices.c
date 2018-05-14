/* ****************************************************************************
 * link/seven/devices.c -- libcasio known Protocol 7.00 devices.
 * Copyright (C) 2016-2017 Thomas "Cakeisalie5" Touhey <thomas@touhey.fr>
 *
 * This file is part of libcasio.
 * libcasio is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3.0 of the License,
 * or (at your option) any later version.
 *
 * libcasio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libcasio; if not, see <http://www.gnu.org/licenses/>.
 *
 * One of the Protocol 7 problems is that it has too few errors, and one of
 * them, the 0x04 error (`casio_seven_err_generic`), can mean many things.
 * Knowing the device we're talking to, and its capabilities, help us reduce
 * the number of possible errors it can be.
 * ************************************************************************* */
#include "../link.h"
#include <string.h>

/* Correspondance */
typedef struct {
	const char        *model;
	casio_seven_env_t  env;
} env_corresp_t;

/* ---
 * Main data.
 * --- */

/* mask bits */
#define MASK_ALL 1
#define MASK_RESET 2
#define MASK_MCS 8
#define MASK_FLS 16
#define MASK_OSUPDATE_1 64
#define MASK_OSUPDATE_2 128

/* supported commands */
CASIO_LOCAL unsigned int command_masks[] = {
	/* [SYSTEM COMMANDS] */
	/* restart/reset */
	MASK_RESET,

	/* - get device info - */
	MASK_ALL,

	/* - set link settings */
	MASK_ALL,

	/* unused */
	0, 0, 0,

	/* set link timeout */
	MASK_RESET,

	/* verifications (OS Update) */
	MASK_ALL, MASK_OSUPDATE_1, MASK_OSUPDATE_1, MASK_OSUPDATE_2,

	/* unused */
	0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

	/* [MCS COMMANDS] */
	/* create, delete, rename directory */
	MASK_MCS, MASK_MCS, MASK_MCS,

	/* change working directory */
	MASK_MCS,

	/* file transfer request, file transfer */
	MASK_MCS, MASK_MCS,

	/* delete, rename, copy file */
	MASK_MCS, MASK_MCS, MASK_MCS,

	/* file transfer all request */
	MASK_MCS,

	/* reset the MCS */
	MASK_MCS,

	/* capacity transmit request, capacity transmit */
	MASK_MCS, MASK_MCS,

	/* file info transfer all request, file info transfer */
	MASK_MCS, MASK_MCS,

	/* RAM image transfer request, RAM image transfer */
	MASK_ALL, MASK_ALL,

	/* Setup entry transfer request, setup entry transfer,
	 * setup entry transfer all request */
	MASK_MCS, MASK_MCS, MASK_MCS,

	/* unused */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

	/* [FLASH COMMANDS] */
	/* create, delete, rename directory */
	MASK_FLS, MASK_FLS, MASK_FLS,

	/* change working directory */
	MASK_FLS,

	/* file transfer request, file transfer */
	MASK_FLS, MASK_FLS,

	/* delete, rename, copy file */
	MASK_FLS, MASK_FLS, MASK_FLS,

	/* file transfer all request */
	MASK_FLS,

	/* reset a filesystem */
	MASK_FLS,

	/* capacity transmit request */
	MASK_FLS,

	/* capacity transmit */
	MASK_FLS,

	/* file info transfer all request, file info transfer */
	MASK_FLS, MASK_FLS,

	/* flash image transfer request, flash image transfer */
	MASK_ALL, MASK_ALL,

	/* optimize filesystem */
	MASK_FLS,

	/* [OS UPDATE] */
	/* CASIOWIN entry transfer request, CASIOWIN entry transfer */
	MASK_ALL, MASK_ALL,

	/* Bootcode transfer request, bootcode transfer */
	MASK_ALL, MASK_ALL,

	/* Upload and run */
	MASK_ALL,

	0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

	/* [fxRemote]: transfer data */
	MASK_ALL,

	/* [fxRemote]: copy data */
	MASK_ALL,

	/* [fxRemote]: clear sector */
	MASK_ALL,
	0, 0, 0,

	/* [fxRemote]: get id bytes */
	MASK_ALL,
	0, 0,

	/* [fxRemote]: terminate communication */
	MASK_ALL,

	/* last is 0x79 */
};

/* Known environments.
 * TODO: add fx-7400GII (Graph 25+Pro SH3)
 *
 * For this list, thanks to:
 * - Simon Lothar (documentation about supported commands and bootcodes);
 * - Critor (quite a few models);
 * - Lephenixnoir, Hackcell (some but less models). */

CASIO_LOCAL env_corresp_t known_environments[] = {
	/* Bootcodes. */

	{"Gy362000", {"fx-7400GII/fx-9860GII bootcode", MASK_ALL}},
	{"Gy363000", {"fx-9750GII-2/fx-9860GII(-2) bootcode", MASK_ALL}},
	{"Gy490000", {"fx-7400GII-2 bootcode", MASK_ALL}},
	{"Ly755000", {"fx-CG20 bootcode", MASK_ALL | MASK_MCS}},

	/* fx devices. */

	{"Gy490006", {"fx-7400GII (Graph 25)",
		MASK_ALL | MASK_RESET | MASK_MCS}},
	{"Gy49000F", {"Graph 25+E (modified fx-7400GII-2)",
		MASK_ALL | MASK_RESET | MASK_MCS}},
	{"Gy362006", {"fx-9750GII (Graph 25+Pro)",
		MASK_ALL | MASK_RESET | MASK_MCS}},
	{"Gy362007", {"fx-9750GII-2 (Graph 35+ SH4)",
		MASK_ALL | MASK_RESET | MASK_MCS}},
	{"Gy36200F", {"Graph 35+E (modified fx-9750GII-2)",
		MASK_ALL | MASK_RESET | MASK_MCS}},
	{"Gy363004", {"fx-9860G (Graph 85)",
		MASK_ALL | MASK_RESET | MASK_MCS | MASK_FLS}},
	{"Gy363006", {"fx-9860GII (Graph 75 SH3)",
		MASK_ALL | MASK_RESET | MASK_MCS | MASK_FLS
			| MASK_OSUPDATE_1}},
	{"Gy363007", {"fx-9860GII-2 (Graph 75 SH4)",
		MASK_ALL | MASK_RESET | MASK_MCS | MASK_FLS
			| MASK_OSUPDATE_1}},
	{"Gy36300F", {"Graph 75+E (modified fx-9860GII-2)",
		MASK_ALL | MASK_RESET | MASK_MCS | MASK_FLS
			| MASK_OSUPDATE_1}},

	/* Terminating entry. */

	{NULL, {"Default environment", MASK_ALL | MASK_MCS | MASK_FLS}}
};

/* ---
 * Internal functions.
 * --- */

/**
 *	casio_seven_getenv:
 *	Get the environment type.
 *
 *	@arg	env			the environment to fill.
 *	@arg	model		the model string
 *	@return				if the given environment is the default one.
 */

int CASIO_EXPORT casio_seven_getenv(casio_seven_env_t *env, const char *model)
{
	env_corresp_t *e;

	for (e = known_environments; e->model && strcmp(model, e->model); e++);
	memcpy(env, &e->env, sizeof(casio_seven_env_t));
	return (!!e->model);
}

/**
 *	casio_seven_command_is_supported:
 *	Check if a command is supported by the environment.
 *
 *	@arg	env			the environment
 *	@arg	code		command code
 *	@return				if it is supported (0 if not)
 */

int CASIO_EXPORT casio_seven_command_is_supported(const casio_seven_env_t *env,
	unsigned int code)
{
	if (command_masks[code] & MASK_ALL) return (1); /* all devices. */
	return (command_masks[code] & env->casio_seven_env_mask);
}
