/* ****************************************************************************
 * libcasio/link.h -- libcasio communications part.
 * Copyright (C) 2017 Thomas "Cakeisalie5" Touhey <thomas@touhey.fr>
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
#ifndef  LIBCASIO_LINK_H
# define LIBCASIO_LINK_H
# include "cdefs.h"
# include "stream.h"
# include "fs.h"
# include "version.h"
# include "picture.h"
CASIO_BEGIN_NAMESPACE

/* ************************************************************************* */
/*  Link and link information                                                */
/* ************************************************************************* */
/* The link structure is private, as it contains diiiirty things.
 * You should only manipulate pointers to it (unless you're contributing
 * to libcasio). */
struct casio_link_s;
typedef struct casio_link_s casio_link_t;

/* Link information.
 * This is basically the identification information sent by the calculator,
 * only in Protocol 7.00 (Graph 85 and following) and CAS100 (AFX,
 * Graph 100). */

# define casio_link_info_wiped_preprog  0x0001
# define casio_link_info_wiped_bootcode 0x0002
# define casio_link_info_wiped_os       0x0004

typedef struct casio_link_info_s {
	unsigned int casio_link_info_wiped;

	/* preprogrammed ROM information */
	unsigned long   casio_link_info_rom_capacity;
	casio_version_t casio_link_info_rom_version;

	/* flash ROM and RAM information */
	unsigned long   casio_link_info_flash_rom_capacity;
	unsigned long   casio_link_info_ram_capacity;

	/* bootcode information */
	casio_version_t casio_link_info_bootcode_version;
	unsigned long   casio_link_info_bootcode_offset;
	unsigned long   casio_link_info_bootcode_size;

	/* OS information */
	casio_version_t casio_link_info_os_version;
	unsigned long   casio_link_info_os_offset;
	unsigned long   casio_link_info_os_size;

	/* other information */
	char casio_link_info_product_id[17];
	char casio_link_info_username[17];
	char casio_link_info_hwid[9];
	char casio_link_info_cpuid[17];
} casio_link_info_t;
/* ************************************************************************* */
/*  Basic callbacks                                                          */
/* ************************************************************************* */
/* The user shall confirm or not a link action. */

typedef int casio_link_confirm_t OF((void *casio__cookie));

/* This callback is for displaying the progress of an operation (usually
 * file/data transfer). It receives the packet ID and the total number of
 * packets. For initialization of the display, this callback is called
 * with a packet ID superior to the number of packets. */

typedef void casio_link_progress_t OF((void *casio__cookie,
	unsigned int casio__id, unsigned int casio__total));

/* This callback is for displaying/using a received picture, with
 * `casio_getscreen` for example. */

typedef void casio_link_screen_t OF((void *casio__cookie,
	int casio__w, int casio__h,
	casio_pixel_t **casio__pixels));

/* List files. */

typedef void casio_link_list_t OF((void *casio__cookie,
	const char *casio__path, const casio_stat_t *casio__stat));
/* ************************************************************************* */
/*  Basic link handle operations                                             */
/* ************************************************************************* */
/* Initialization flags */
# define CASIO_LINKFLAG_ACTIVE 0x00000001 /* start off as active */
# define CASIO_LINKFLAG_CHECK  0x00000002 /* check (initial packet) */
# define CASIO_LINKFLAG_TERM   0x00000004 /* terminate */
# define CASIO_LINKFLAG_NODISC 0x00000008 /* if we are checking,
                                           * no environment discovery */

CASIO_BEGIN_DECLS

/* Cross-platform initialization. */
CASIO_EXTERN int casio_open_usb  OF((casio_link_t **casio__h,
	unsigned long casio__flags));
CASIO_EXTERN int casio_open_com  OF((casio_link_t **casio__h,
	unsigned long casio__flags, const char *casio__path,
	const casio_streamattrs_t *casio__attributes));

/* Initialize a handle using a custom stream. */
CASIO_EXTERN int casio_open_link OF((casio_link_t **casio__h,
	unsigned long casio__flags, casio_stream_t *casio__stream,
	const casio_streamattrs_t *casio__attributes));

/* Get things. */

CASIO_EXTERN const casio_link_info_t *casio_get_link_info OF((
	casio_link_t *casio__handle));

/* De-initialize. */
CASIO_EXTERN void casio_close_link OF((casio_link_t *casio__h));
/* ************************************************************************* */
/*  Run servers.                                                             */
/* ************************************************************************* */
/* Run a Protocol 7.00 server. */

typedef int casio_seven_server_func_t OF((void *casio__cookie,
	casio_link_t *casio__handle));

CASIO_EXTERN int casio_seven_serve OF((casio_link_t *casio__handle,
	casio_seven_server_func_t **casio__callbacks, void *casio__cookie));
/* ************************************************************************* */
/*  General-purpose link operations                                          */
/* ************************************************************************* */
/* Set the link settings. */

CASIO_EXTERN int CASIO_EXPORT casio_setlink
	OF((casio_link_t *casio__handle, const casio_streamattrs_t *casio__attrs));

/* Set up a screen streaming receiver. */

CASIO_EXTERN int CASIO_EXPORT casio_getscreen
	OF((casio_link_t *casio__handle,
		casio_link_screen_t *casio__callback, void *casio__scookie));

/* Backup the ROM. */

CASIO_EXTERN int CASIO_EXPORT casio_backup_rom
	OF((casio_link_t *casio__handle,
		casio_stream_t *casio__buffer, casio_link_progress_t *casio__progress,
		void *casio__pcookie));

# ifndef LIBCASIO_DISABLED_FILE
CASIO_EXTERN int CASIO_EXPORT casio_backup_rom_file
	OF((casio_link_t *casio__handle,
		FILE *casio__file, casio_link_progress_t *casio__progress,
		void *casio__pcookie));
# endif

/* Upload and run an executable. */

CASIO_EXTERN int CASIO_EXPORT casio_upload_and_run
	OF((casio_link_t *casio__handle,
		casio_stream_t *casio__buffer, casio_off_t casio__size,
		unsigned long casio__loadaddr, unsigned long casio__straddr,
		casio_link_progress_t *casio__disp, void *casio__pcookie));

# ifndef LIBCASIO_DISABLED_FILE
CASIO_EXTERN int CASIO_EXPORT casio_upload_and_run_file
	OF((casio_link_t *casio__handle, FILE *casio__program,
		unsigned long casio__loadaddr, unsigned long casio__straddr,
		casio_link_progress_t *casio__disp, void *casio__pcookie));
# endif

CASIO_END_DECLS
CASIO_END_NAMESPACE
# include "protocol/legacy.h"
# include "protocol/seven.h"
#endif /* LIBCASIO_LINK_H */
