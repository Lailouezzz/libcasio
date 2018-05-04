/* ****************************************************************************
 * libcasio/protocol/seven/commands.h -- Protocol 7.00 classical commands.
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
 * ************************************************************************* */
#ifndef  LIBCASIO_PROTOCOL_SEVEN_COMMANDS_H
# define LIBCASIO_PROTOCOL_SEVEN_COMMANDS_H
# include "../seven.h"
# include "../../mcsfile.h"
CASIO_BEGIN_NAMESPACE
CASIO_BEGIN_DECLS

/* Utility for MCS commands */

CASIO_EXTERN int CASIO_EXPORT casio_seven_send_typical_mcs_command
	OF((casio_link_t *casio__handle,
		int casio__code, casio_mcshead_t *casio__head,
		casio_mcsfile_t *casio__mcsfile, int casio__ow));

/* ---
 * Basic commands.
 * --- */

/* Get the platform information. */

# define casio_seven_cmdsys_getinfo 0x01
# define casio_seven_send_cmdsys_getinfo(CASIO__HANDLE) \
	casio_seven_send_cmd(CASIO__HANDLE, 0x01)

/* Set the link settings. */

# define casio_seven_cmdsys_setlink     0x02
CASIO_EXTERN int CASIO_EXPORT casio_seven_send_cmdsys_setlink
	OF((casio_link_t *casio__handle,
		int casio__baudrate, int casio__parity, int casio__stopbits));

/* ---
 * Backup commands.
 * --- */

/* Backup the RAM */

# define casio_seven_cmdbak_reqram 0x2F
# define casio_seven_send_cmdbak_reqram(CASIO__HANDLE) \
	casio_seven_send_cmd(CASIO__HANDLE, 0x2F)

/* Send the RAM */

# define casio_seven_cmdbak_putram 0x30
# define casio_seven_send_cmdbak_putram(CASIO__HANDLE) \
	casio_seven_send_cmd(CASIO__HANDLE, 0x30)

/* Backup the ROM */

# define casio_seven_cmdbak_reqrom 0x4F
# define casio_seven_send_cmdbak_reqrom(CASIO__HANDLE) \
	casio_seven_send_cmd(CASIO__HANDLE, 0x4F)

/* Send the ROM */

# define casio_seven_cmdbak_putrom 0x50
# define casio_seven_send_cmdbak_putrom(CASIO__HANDLE) \
	casio_seven_send_cmd(CASIO__HANDLE, 0x50)

/* Backup the CASIOWIN entry */

# define casio_seven_cmdbak_reqcwe 0x52
# define casio_seven_send_cmdbak_reqcwe(CASIO__HANDLE) \
	casio_seven_send_cmd(CASIO__HANDLE, 0x52)

/* Send the CASIOWIN entry */

# define casio_seven_cmdbak_putcwe 0x53
# define casio_seven_send_cmdbak_putcwe(CASIO__HANDLE) \
	casio_seven_send_cmd(CASIO__HANDLE, 0x53)

/* Backup the bootcode */

# define casio_seven_cmdbak_reqboot 0x54
# define casio_seven_send_cmdbak_reqboot(CASIO__HANDLE) \
	casio_seven_send_cmd(CASIO__HANDLE, 0x54)

/* Send the bootcode */

# define casio_seven_cmdbak_putboot 0x55
# define casio_seven_send_cmdbak_putboot(CASIO__HANDLE) \
	casio_seven_send_cmd(CASIO__HANDLE, 0x55)

/* ---
 * Main memory commands.
 * --- */

/* Request a file */

# define casio_seven_cmdmcs_reqfile     0x24
# define casio_seven_send_cmdmcs_reqfile(CASIO__HANDLE, CASIO__HEAD) \
	casio_seven_send_typical_mcs_command(CASIO__HANDLE, 0x24, \
		CASIO__HEAD, 0, 0)

/* Send a file */

# define casio_seven_cmdmcs_sendfile    0x25
# define casio_seven_send_cmdmcs_sendfile(CASIO__HANDLE, CASIO__OW, \
  CASIO__FILE) \
	casio_seven_send_typical_mcs_command(CASIO__HANDLE, 0x25, \
		(CASIO__FILE)->casio_mcsfile_head, CASIO__OW)

/* Delete a file */

# define casio_seven_cmdmcs_delfile     0x26
# define casio_seven_send_cmdmcs_delfile(CASIO__HANDLE, CASIO__HEAD) \
	casio_seven_send_typical_mcs_command(CASIO__HANDLE, 0x26, \
		(CASIO__HEAD), NULL, 0)

/* Request all file information */

# define casio_seven_cmdmcs_reqallinfo  0x2D
# define casio_seven_send_cmdmcs_reqallinfo(CASIO__HANDLE) \
	casio_seven_send_cmd(CASIO__HANDLE, 0x2D)

/* Send one file's information */

# define casio_seven_cmdmcs_fileinfo    0x2E

/* Send a setup entry */

# define casio_seven_cmdmcs_putsetup    0x32

/* Request all setup entries. */

# define casio_seven_cmdmcs_reqallsetup 0x33
# define casio_seven_send_cmdmcs_reqallsetup(CASIO__HANDLE) \
	casio_seven_send_cmd(CASIO__HANDLE, 0x33)

/* ---
 * Storage commands.
 * --- */

/* Make a directory */

# define casio_seven_cmdfls_mkdir       0x40
# define casio_seven_send_cmdfls_mkdir(CASIO__HANDLE, CASIO__DIR, CASIO__DEV) \
	casio_seven_send_cmd_data(CASIO__HANDLE, 0x40, 0, 0, 0, \
		CASIO__DIR, NULL, NULL, NULL, CASIO__DEV, NULL)

/* Delete a directory */

# define casio_seven_cmdfls_rmdir       0x41
# define casio_seven_send_cmdfls_rmdir(CASIO__HANDLE, CASIO__DIR, CASIO__DEV) \
	casio_seven_send_cmd_data(CASIO__HANDLE, 0x41, 0, 0, 0, \
		CASIO__DIR, NULL, NULL, NULL, CASIO__DEV, NULL)

/* Rename (move) a directory */

# define casio_seven_cmdfls_mvdir       0x42
# define casio_seven_send_cmdfls_mvdir(CASIO__HANDLE, CASIO__DIR, \
  CASIO__NEWDIR, CASIO__DEV) \
	casio_seven_send_cmd_data(CASIO__HANDLE, 0x42, 0, 0, 0, \
		CASIO__DIR, CASIO__NEWDIR, NULL, NULL, CASIO__DEV, NULL)

/* Change working directory */

# define casio_seven_cmdfls_cwd         0x43
# define casio_seven_send_cmdfls_cwd(CASIO__HANDLE, CASIO__DIR, CASIO__DEV) \
	casio_seven_send_cmd_data(CASIO__HANDLE, 0x43, 0, 0, 0, \
		CASIO__DIR, NULL, NULL, NULL, CASIO__DEV, NULL)

/* Request a file */

# define casio_seven_cmdfls_reqfile     0x44
# define casio_seven_send_cmdfls_reqfile(CASIO__HANDLE, CASIO__DIR, \
  CASIO__FILE, CASIO__DEV) \
	casio_seven_send_cmd_data(CASIO__HANDLE, 0x44, 0, 0, 0, \
		CASIO__DIR, CASIO__FILE, NULL, NULL, CASIO__DEV, NULL)

/* Send a file */

# define casio_seven_cmdfls_sendfile    0x45
# define casio_seven_send_cmdfls_sendfile(CASIO__HANDLE, CASIO__OW, \
  CASIO__SIZE, CASIO__DIR, CASIO__FILE, CASIO__DEV) \
	casio_seven_send_cmd_data(CASIO__HANDLE, 0x45, CASIO__OW, 0, CASIO__SIZE, \
		CASIO__DIR, CASIO__FILE, NULL, NULL, CASIO__DEV, NULL)

/* Delete a file */

# define casio_seven_cmdfls_delfile     0x46
# define casio_seven_send_cmdfls_delfile(CASIO__HANDLE, CASIO__DIR, \
  CASIO__FILE, CASIO__DEV) \
	casio_seven_send_cmd_data(CASIO__HANDLE, 0x46, 0, 0, 0, \
		CASIO__DIR, CASIO__FILE, NULL, NULL, CASIO__DEV, NULL)

/* Rename a file */

# define casio_seven_cmdfls_renamefile  0x47
# define casio_seven_send_cmdfls_renamefile(CASIO__HANDLE, CASIO__DIR, \
  CASIO__NEWDIR, CASIO__DEV) \
	casio_seven_send_cmd_data(CASIO__HANDLE, 0x47, 0, 0, 0, \
		CASIO__DIR, CASIO__NEWDIR, NULL, NULL, CASIO__DEV, NULL)

/* Copy a file */

# define casio_seven_cmdfls_copyfile    0x48
# define casio_seven_send_cmdfls_copyfile(CASIO__HANDLE, CASIO__DIR, \
  CASIO__FILE, CASIO__NEWDIR, CASIO__NEWFILE, CASIO__DEV) \
	casio_seven_send_cmd_data(CASIO__HANDLE, 0x48, 0, 0, 0, \
		CASIO__DIR, CASIO__FILE, CASIO__NEWDIR, CASIO__NEWFILE, \
		CASIO__DEV, NULL)

/* Reset a storage device */

# define casio_seven_cmdfls_reset       0x4A
# define casio_seven_send_cmdfls_reset(CASIO__HANDLE, CASIO__DEV) \
	casio_seven_send_cmd_data(CASIO__HANDLE, 0x4A, 0, 0, 0, \
		NULL, NULL, NULL, NULL, CASIO__DEV, NULL)

/* Request the capacity */

# define casio_seven_cmdfls_reqcapacity 0x4B
# define casio_seven_send_cmdfls_reqcapacity(CASIO__HANDLE, CASIO__DEV) \
	casio_seven_send_cmd_data(CASIO__HANDLE, 0x4B, 0, 0, 0, \
		NULL, NULL, NULL, NULL, CASIO__DEV, NULL)

/* Request all file information */

# define casio_seven_cmdfls_reqallinfo  0x4D
# define casio_seven_send_cmdfls_reqallinfo(CASIO__HANDLE, CASIO__DEV) \
	casio_seven_send_cmd_data(CASIO__HANDLE, 0x4D, 0, 0, 0, \
		NULL, NULL, NULL, NULL, CASIO__DEV, NULL)

/* Send one file's information */

# define casio_seven_cmdfls_fileinfo    0x4E

/* Optimize a storage device */

# define casio_seven_cmdfls_opt         0x51
# define casio_seven_send_cmdfls_opt(CASIO__HANDLE, CASIO__DEV) \
	casio_seven_send_cmd_data(CASIO__HANDLE, 0x51, 0, 0, 0, \
		NULL, NULL, NULL, NULL, CASIO__DEV, NULL)

/* ---
 * OS Update commands.
 * --- */

/* Upload and run */

# define casio_seven_cmdosu_upandrun    0x56
CASIO_EXTERN int CASIO_EXPORT casio_seven_send_cmdosu_upandrun
	OF((casio_link_t *casio__handle,
		unsigned long casio__upsize, unsigned long casio__loadaddr,
		unsigned long casio__straddr));

CASIO_END_DECLS
CASIO_END_NAMESPACE
#endif /* LIBCASIO_PROTOCOL_SEVEN_COMMANDS_H */
