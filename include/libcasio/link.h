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
# include "mcs.h"
# include "fs.h"
# include "version.h"
# include "picture.h"
CASIO_BEGIN_NAMESPACE

/* ---
 * Link and link information.
 * --- */

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

/* ---
 * Basic callbacks.
 * --- */

/* The user shall confirm or not a link action. */

typedef int CASIO_EXPORT casio_link_confirm_t
	OF((void *casio__cookie));

/* This callback is for displaying the progress of an operation (usually
 * file/data transfer). It receives the packet ID and the total number of
 * packets. For initialization of the display, this callback is called
 * with a packet ID superior to the number of packets. */

typedef void CASIO_EXPORT casio_link_progress_t
	OF((void *casio__cookie, unsigned int casio__id,
		unsigned int casio__total));

/* List files. */

typedef void CASIO_EXPORT casio_link_list_t
	OF((void *casio__cookie, const char *casio__path,
		const casio_stat_t *casio__stat));

/* ---
 * Other structures.
 * --- */

/* Screen. */

typedef struct casio_screen_s {
	unsigned int casio_screen_width;
	unsigned int casio_screen_height;

	unsigned int casio_screen_realwidth;
	unsigned int casio_screen_realheight;

	casio_pixel_t **casio_screen_pixels;
} casio_screen_t;

/* ---
 * Basic link handle operations.
 * --- */

/* Initialization flags.
 * `CASIO_LINKFLAG_ACTIVE`: start off as active;
 * `CASIO_LINKFLAG_CHECK`: check (initial packet);
 * `CASIO_LINKFLAG_TERM`: terminate;
 * `CASIO_LINKFLAG_NODISC`: if we are checking, no environment discovery. */

# define CASIO_LINKFLAG_ACTIVE 0x00000001
# define CASIO_LINKFLAG_CHECK  0x00000002
# define CASIO_LINKFLAG_TERM   0x00000004
# define CASIO_LINKFLAG_NODISC 0x00000008

CASIO_BEGIN_DECLS

/* Cross-platform initialization. */

CASIO_EXTERN int CASIO_EXPORT casio_open_usb
	OF((casio_link_t **casio__h, unsigned long casio__flags,
		int casio__bus, int casio__address));
CASIO_EXTERN int CASIO_EXPORT casio_open_com
	OF((casio_link_t **casio__h, unsigned long casio__flags,
		char const *casio__path,
		casio_streamattrs_t const *casio__attributes));

/* Initialize a handle using a custom stream. */

CASIO_EXTERN int CASIO_EXPORT casio_open_link
	OF((casio_link_t **casio__h,
		unsigned long casio__flags, casio_stream_t *casio__stream,
		casio_streamattrs_t const *casio__attributes));

/* De-initialize. */

CASIO_EXTERN void CASIO_EXPORT casio_close_link
	OF((casio_link_t *casio__h));

/* Lock and unlock the associated mutex. */

CASIO_EXTERN int  CASIO_EXPORT casio_lock_link
	OF((casio_link_t *casio__h));
CASIO_EXTERN int  CASIO_EXPORT casio_trylock_link
	OF((casio_link_t *casio__h));
CASIO_EXTERN void CASIO_EXPORT casio_unlock_link
	OF((casio_link_t *casio__h));

/* Getters. */

CASIO_EXTERN const casio_link_info_t* CASIO_EXPORT casio_get_link_info
	OF((casio_link_t *casio__handle));

/* ---
 * General-purpose link operations.
 * --- */

/* Set the link settings. */

CASIO_EXTERN int CASIO_EXPORT casio_setlink
	OF((casio_link_t *casio__handle, const casio_streamattrs_t *casio__attrs));

/* Receive and free a screen streaming frame.
 * The screen is a double pointer because it is allocated or reallocated
 * when required only. */

CASIO_EXTERN int CASIO_EXPORT casio_get_screen
	OF((casio_link_t *casio__handle, casio_screen_t **casio__screen));

CASIO_EXTERN int CASIO_EXPORT casio_make_screen
	OF((casio_screen_t **casio__screen, unsigned int casio__width,
		unsigned int casio__height));
CASIO_EXTERN int CASIO_EXPORT casio_free_screen
	OF((casio_screen_t *casio__screen));

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
