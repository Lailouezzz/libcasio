/* ****************************************************************************
 * libcasio/format/std/mcs.h -- the standard MCS file format description.
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
#ifndef  LIBCASIO_FORMAT_STD_MCS_H
# define LIBCASIO_FORMAT_STD_MCS_H
# include <libcasio/cdefs.h>
# include <stdint.h>
# pragma pack(1)
CASIO_BEGIN_NAMESPACE

/* MCS is the main filesystem on CASIO calculators. They contain settings,
 * programs, lists, pictures, captures, matrixes, and other data CASIO
 * calculators own. This format is common to fx and fx-CG MCS archives.
 *
 * Keep in mind that all numbers in the MCS format are BCD-encoded.
 * Because of some precision issue (0.3), we'll deliver them to the user
 * almost as is.
 *
 * MCS files have a group, a directory, a name and a type. This type depends
 * on the group it's in. So for example, the 0xFE type in the 'S-SHEET' group
 * will correspond to a spreadsheet! In an MCS std. file, files are more or
 * less regrouped by their group (but sometimes they're not, there are several
 * 'PROGRAM' groups with one element for example).
 *
 * The `number` field in the Standard Header corresponds to the total number
 * of MCS files, not group instances!
 *
 * Each group instance have a header, and here it is: */

typedef struct casio_mcs_subheader_s {
	/* group name (e.g. "PROGRAM", zero padded) */
	casio_uint8_t  casio_mcs_subheader_intname[16];

	/* subitem count */
	casio_uint32_t casio_mcs_subheader_subcount;
} casio_mcs_subheader_t;

/* Then follows the files, with their headers and contents.
 * And the header iiiiiiis: */

typedef struct casio_mcs_fileheader_s {
	/* the directory name (zero-padded).*/
	casio_uint8_t  casio_mcs_fileheader_dirname[8];

	/* the file name (item name) */
	casio_uint8_t  casio_mcs_fileheader_filename[8];

	/* the file type. */
	casio_uint8_t  casio_mcs_fileheader_filetype;

	/* the size of the part data, without the part header. */
	casio_uint32_t casio_mcs_fileheader_datalength;

	/* some alignment */
	casio_uint8_t  casio_mcs_fileheader__align[3];
} casio_mcs_fileheader_t;

CASIO_END_NAMESPACE
# pragma pack()

/* Beneath the file header, the file have different structures according to
 * their group name and MCS type. Pick your poison, once again! */

# include <libcasio/format/mcs/cells.h>
# include <libcasio/format/mcs/picture.h>
# include <libcasio/format/mcs/program.h>
# include <libcasio/format/mcs/setup.h>
# include <libcasio/format/mcs/spreadsheet.h>
#endif /* LIBCASIO_FORMAT_STD_MCS_H */
