/* ****************************************************************************
 * libcasio/file.h -- file handle.
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
#ifndef  LIBCASIO_FILE_H
# define LIBCASIO_FILE_H
# include <libcasio/mcsfile.h>
# include <libcasio/version.h>
# include <libcasio/date.h>

/* ************************************************************************* */
/*  Description                                                              */
/* ************************************************************************* */
/* This header is about files that are adapted for most filesystems out there,
 * such as G1M, G1A, G3E, et caetera.
 *
 * Here are the managed file types: */

typedef unsigned int casio_filetype_t;
# define casio_filetype_addin        0x0001
# define casio_filetype_mcs          0x0002
# define casio_filetype_eact         0x0004
# define casio_filetype_picture      0x0008
# define casio_filetype_lang         0x0010
# define casio_filetype_fkey         0x0020
# define casio_filetype_storage      0x0040

/* (with aliases) */
# define casio_filetype_pict         casio_filetype_picture
# define casio_filetype_eactivity    casio_filetype_eact
# define casio_filetype_add_in       casio_filetype_addin

/* Those types can exist for each of the platformes.
 * For example, G1E is e-activities for the fx platform, and C2P are
 * pictures for the fx-CP platform. */

typedef unsigned int casio_filefor_t;
# define casio_filefor_none     0x0000
# define casio_filefor_fx       0x0001
# define casio_filefor_cp       0x0002
# define casio_filefor_cg       0x0004
# define casio_filefor_cas      0x0008
# define casio_filefor_casemul  0x0010

/* And here is the file structure.
 * The elements can be initialized or uninitialized depending on the
 * file type. */

typedef struct casio_file_s {
	/* file type, destination platform */
	casio_filetype_t casio_file_type;
	casio_filefor_t  casio_file_for;

	/* Add-in related data */
	time_t           casio_file_creation_date;
	unsigned char   *casio_file_content;
	size_t           casio_file_size;

	/* Lists for various purposes */
	int               casio_file_count;
	int               casio_file__size;
	casio_mcsfile_t **casio_file_mcsfiles;
	char            **casio_file_messages;
	casio_pixel_t  ***casio_file_fkeys;

	/* Picture-related data (also used for add-in icons) */
	int             casio_file_width;
	int             casio_file_height;
	casio_pixel_t **casio_file_pixels;     /* 0x0RGB */
	casio_pixel_t **casio_file_icon_unsel; /* 0x0RGB */
	casio_pixel_t **casio_file_icon_sel;   /* 0x0RGB */

	/* Other string data */
	char             casio_file_title[17];
	char             casio_file_intname[12];
	casio_version_t  casio_file_version;
} casio_file_t;
/* ************************************************************************* */
/*  Methods                                                                  */
/* ************************************************************************* */
/* Make a file. */

extern int casio_make_picture OF((casio_file_t **casio__handle,
	unsigned int casio__width, unsigned int casio__height));
extern int casio_make_mcs     OF((casio_file_t **casio__handle,
	int casio__count));
extern int casio_make_fkey    OF((casio_file_t **casio__handle,
	casio_filefor_t casio__for, int casio__count));
extern int casio_make_lang    OF((casio_file_t **casio__handle,
	casio_filefor_t casio__for, int casio__count));
extern int casio_make_addin   OF((casio_file_t **casio__handle,
	casio_filefor_t casio__for, size_t casio__size,
	const char *casio__name, const char *casio__internal,
	const casio_version_t *casio__version, const time_t *casio__created));

/* Manage an MCS archive. */

extern int casio_mcs_insert OF((casio_file_t *handle, casio_mcsfile_t **tofile,
	const casio_mcshead_t *head));
extern int casio_mcs_sort   OF((casio_file_t *handle));

/* Free a file. */

extern void casio_free_file OF((casio_file_t *casio__handle));

/* Decode a file. */

extern int casio_decode OF((casio_file_t **casio__handle,
	const char *casio__path, casio_stream_t *casio__buffer,
	casio_filetype_t casio__expected_types));

/* Open and decode a file. */

extern int casio_open_file OF((casio_file_t **casio__handle,
	const char *casio__path, casio_filetype_t casio__expected_types));

#endif /* LIBCASIO_FILE_H */
