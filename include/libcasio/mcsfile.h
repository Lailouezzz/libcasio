/* ****************************************************************************
 * libcasio/mcsfile.h -- main memory file description.
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
#ifndef  LIBCASIO_MCSFILE_H
# define LIBCASIO_MCSFILE_H
# include "cdefs.h"
# include "number.h"
# include "picture.h"
# include "setup.h"

# define casio_theta 27
# define casio_r     28
# define casio_ans   29
CASIO_BEGIN_NAMESPACE

/* A lot of CASIO calculators (like the fx-9xxx, fx-CGxx, fx-CPxxx) have
 * a main memory, also called MCS (for Main Control Structure).
 *
 * This memory contains all of the configuration files or main data every
 * calculator should manage, such as setup, programs, spreadsheets, pictures,
 * et caetera.
 *
 * As there are few formats, it should be possible to manage them all
 * in libcasio. */
/* ************************************************************************* */
/*  Main memory file types                                                   */
/* ************************************************************************* */
/* Here are all the file types that are managed by libcasio. */

typedef unsigned long casio_mcsfile_type_t;
typedef unsigned long casio_mcstype_t;

# define casio_mcstype_unknown     0x00000000,

/* A program. */

# define casio_mcstype_prog        0x00000001
# define casio_mcstype_program     0x00000001

/* A list. */

# define casio_mcstype_lst         0x00000002
# define casio_mcstype_list        0x00000002

/* A matrix. */

# define casio_mcstype_mat         0x00000004
# define casio_mcstype_matrix      0x00000004

/* A picture (double-side capture). */

# define casio_mcstype_pic         0x00000008
# define casio_mcstype_pict        0x00000008
# define casio_mcstype_picture     0x00000008

/* A capture. */

# define casio_mcstype_capt        0x00000010
# define casio_mcstype_capture     0x00000010

/* A spreadsheet. */

# define casio_mcstype_ssheet      0x00000020
# define casio_mcstype_spreadsheet 0x00000020

/* A string. */

# define casio_mcstype_str         0x00000040
# define casio_mcstype_string      0x00000040

/* A settings file. */

# define casio_mcstype_setup       0x00000080
# define casio_mcstype_settings    0x00000080

/* Variable(s). */

# define casio_mcstype_var         0x00000100
# define casio_mcstype_alpha       0x00000100
# define casio_mcstype_variable    0x00000100
# define casio_mcstype_alphamem    0x00000100
# define casio_mcstype_variables   0x00000100

/* A vector. */

# define casio_mcstype_vct         0x00000200
# define casio_mcstype_vector      0x00000200

/* An end packet (link-specific). */

# define casio_mcstype_end         0x00000400

/* Macros to check if the type uses the ID, and to interact with it */
# define casio_mcshead_uses_id(CASIO__H) (((CASIO__H)->casio_mcshead_type & (\
	casio_mcstype_list | casio_mcstype_mat  | casio_mcstype_vct | \
	casio_mcstype_pict | casio_mcstype_capt | casio_mcstype_string) || \
	((CASIO__H)->casio_mcshead_type  == casio_mcstype_var && \
	 (CASIO__H)->casio_mcshead_count == 1)))
# define casio_get_id_major(CASIO__I) ((CASIO__I) >> 6)
# define casio_get_id_minor(CASIO__I) ((CASIO__I) & 0x3F)
/* ************************************************************************* */
/*  Main memory flags and platforms                                          */
/* ************************************************************************* */
/* Main memory files in libcasio have various options.
 * Here are the core flags: */

# define casio_mcsflag_valid      0x00000001 /* the file is valid. */
# define casio_mcsflag_alloc      0x00000002 /* file was/shall be allocated. */

# define casio_mcsflag_unfinished 0x00000008 /* still parts to read. */
# define casio_mcsflag_multiple   0x00000010 /* is a set of files */
# define casio_mcsflag_request    0x00000020 /* is a request (link) */

/* Then comes the platform for which the MCS file is made.
 * The most obvious one is the entries in an MCS archive (G1M, G1R), but
 * others are directly linked to the protocol and header type with which
 * the file is described -- see `libcasio/format/cas.h`. */

typedef unsigned int casio_mcsinfo_t;

# define casio_mcsfor_mask      0x7F000000

# define casio_mcsfor_none      0x00000000 /* no specific platform */
# define casio_mcsfor_mcs       0x01000000 /* standard MCS archive */
# define casio_mcsfor_cas40     0x02000000 /* legacy CAS40  header */
# define casio_mcsfor_cas50     0x04000000 /* legacy CAS50  header */
# define casio_mcsfor_cas100    0x08000000 /* legacy CAS100 header */

/* Aliases */
# define casio_mcsfor_cas       casio_mcsfor_cas40
# define casio_mcsfor_caspro    casio_mcsfor_cas50
# define casio_mcsfor_graph100  casio_mcsfor_cas100
/* ************************************************************************* */
/*  MCS cell                                                                 */
/* ************************************************************************* */
/* Numbers in various MCS file formats (lists, matrixes, variables,
 * spreadsheets) have a real part (BCD encoded), an imaginary part
 * (BCD encoded) and flags. Here is the structure representing this.
 *
 * First, the flags: */

# define casio_mcscellflag_used   0x0001

/* And the main structure: */

typedef struct casio_mcscell_s {
	unsigned int casio_mcscell_flags;
	casio_bcd_t  casio_mcscell_real;
	casio_bcd_t  casio_mcscell_imgn;
} casio_mcscell_t;
/* ************************************************************************* */
/*  Main memory file head                                                    */
/* ************************************************************************* */
/* Filenames and directory names are not the only things which characterize
 * MCS files; that's why a separate structure representing the meta information
 * about MCS files are preferable. This head structure is also used to
 * receive/send file information (sometimes not using the content, e.g. with
 * CAS heads, and sometimes also using the beginning of it, e.g. with MCS
 * archives).
 *
 * The available (filled, not left uninitialized) information in the head
 * depend on the platform the MCS head comes from
 * (see `Main memory platforms`).
 * Remember that heads are not only used to characterize files, but can also
 * contain various information, depending on the protocol it is used with.
 *
 * Here is the main structure. Don't be afraid, it doesn't bite. */

typedef struct casio_mcshead_s {
# define casio_mcshead_for casio_mcshead_flags
	unsigned long    casio_mcshead_flags;

	/* file main information */
	casio_mcstype_t  casio_mcshead_type;
	unsigned int     casio_mcshead_id;

	/* content size */
	int            casio_mcshead_count;
	unsigned int   casio_mcshead_width;
	unsigned int   casio_mcshead_height;
	unsigned long  casio_mcshead_size;

	/* raw data */
	unsigned int           casio_mcshead_rawtype;
	unsigned int           casio_mcshead_cas_ext;
	casio_pictureformat_t  casio_mcshead_picformat;

	/* strings */
	char casio_mcshead_name[13];
	char casio_mcshead_password[9];
	char casio_mcshead_group[17];
	char casio_mcshead_dirname[9];
	char casio_mcshead_cas_app[4];
	char casio_mcshead_datatype[3];
} casio_mcshead_t;
/* ************************************************************************* */
/*  Main memory file                                                         */
/* ************************************************************************* */
/* Here is the file data. Its content depends on what's in the head. */

typedef struct casio_mcsfile_s {
	/* head */
	casio_mcshead_t   casio_mcsfile_head;

	/* content (useful when not read) */
	char             *casio_mcsfile_content;

	/* variables */
	casio_mcscell_t   casio_mcsfile_var;
	casio_mcscell_t  *casio_mcsfile_vars;
	casio_mcscell_t **casio_mcsfile_cells;

	/* for pictures and captures */
	casio_pixel_t   **casio_mcsfile_pic; /* 0x0RGB */
	casio_pixel_t  ***casio_mcsfile_pics;

	/* for settings */
	casio_setup_t     casio_mcsfile_setup;
} casio_mcsfile_t;

/* Some macros to manipulate the password. */

# define casio_has_password(F) \
	(F)->casio_mcsfile_head.casio_mcshead_password[0]
# define casio_remove_password(F) \
	(F)->casio_mcsfile_head.casio_mcshead_password[0] = 0
/* ************************************************************************* */
/*  Utilities                                                                */
/* ************************************************************************* */
CASIO_BEGIN_DECLS
/* Make a main memory file, prepare it, and free it. */

CASIO_EXTERN int  CASIO_EXPORT casio_make_mcsfile
	OF((casio_mcsfile_t **casio__handle, const casio_mcshead_t *casio__head));
CASIO_EXTERN int  CASIO_EXPORT casio_prepare_mcsfile
	OF((casio_mcsfile_t  *casio__handle, const casio_mcshead_t *casio__head));
CASIO_EXTERN void CASIO_EXPORT casio_free_mcsfile
	OF((casio_mcsfile_t  *casio__handle));

/* Copy or duplicate a main memory file. */

CASIO_EXTERN int  CASIO_EXPORT casio_duplicate_mcsfile
	OF((casio_mcsfile_t **casio__handle, casio_mcsfile_t *casio__old));
CASIO_EXTERN int  CASIO_EXPORT casio_copy_mcsfile
	OF((casio_mcsfile_t  *casio__handle, casio_mcsfile_t *casio__old));

/* Decode and encode an MCS file from an MCS archive. */

CASIO_EXTERN int CASIO_EXPORT casio_decode_mcsfile_head
	OF((casio_mcshead_t *casio__head, int casio__raw_type,
		const unsigned char *casio__groupname,
		const unsigned char *casio__dirname,
		const unsigned char *casio__filename,
		unsigned long casio__filesize));

CASIO_EXTERN int CASIO_EXPORT casio_decode_mcsfile
	OF((casio_mcsfile_t **casio__handle,
		const casio_mcshead_t *casio__head,
		casio_stream_t *casio__buffer));
CASIO_EXTERN int CASIO_EXPORT casio_decode_mcsfile_data
	OF((casio_mcsfile_t **casio__handle,
		const casio_mcshead_t *casio__head,
		const void *casio__data, size_t casio__size));

CASIO_EXTERN int CASIO_EXPORT casio_encode_mcsfile
	OF((casio_mcsfile_t  *casio__handle, casio_stream_t *casio__buffer));

/* Decode and encode an MCS file from a CASIOLINK environment. */

CASIO_EXTERN int CASIO_EXPORT casio_decode_casfile_head
	OF((casio_mcshead_t *casio__head, casio_stream_t *casio__buffer));
CASIO_EXTERN int CASIO_EXPORT casio_decode_casfile_part
	OF((casio_mcsfile_t *casio__file, casio_stream_t *casio__stream));

CASIO_EXTERN int CASIO_EXPORT casio_encode_casfile_head
	OF((casio_mcshead_t *casio__head, casio_stream_t *casio__buffer));
CASIO_EXTERN int CASIO_EXPORT casio_encode_casfile_part
	OF((casio_mcsfile_t *casio__file, casio_stream_t *casio__buffer));

/* Correct a head (add raw data). */

CASIO_EXTERN int CASIO_EXPORT casio_correct_mcshead
	OF((casio_mcshead_t *casio__head, unsigned long casio__mcsfor));

/* Compare MCS files (for ordering). */

CASIO_EXTERN int CASIO_EXPORT casio_match_mcsfiles
	OF((casio_mcshead_t *casio__first, casio_mcshead_t *casio__second));
CASIO_EXTERN int CASIO_EXPORT casio_compare_mcsfiles
	OF((casio_mcshead_t *casio__first, casio_mcshead_t *casio__second));

CASIO_END_DECLS
CASIO_END_NAMESPACE
#endif /* LIBCASIO_MCSFILE_H */
