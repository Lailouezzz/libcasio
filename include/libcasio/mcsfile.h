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
# include <libcasio/cdefs.h>
# include <libcasio/number.h>
# include <libcasio/picture.h>
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

typedef long unsigned int    casio_mcsfile_type_t;
typedef casio_mcsfile_type_t casio_mcstype_t;

# define casio_mcstype_unknown      0x00000000
# define casio_mcstype_program      0x00000001
# define casio_mcstype_list         0x00000002
# define casio_mcstype_mat          0x00000004
# define casio_mcstype_pict         0x00000008
# define casio_mcstype_capt         0x00000010
# define casio_mcstype_ssheet       0x00000020
# define casio_mcstype_string       0x00000040
# define casio_mcstype_setup        0x00000080
# define casio_mcstype_var          0x00000100
# define casio_mcstype_vct          0x00000200
# define casio_mcstype_end          0x00000400

/* And here are aliases, to ease the access to those. */

# define casio_mcstype_matrix       casio_mcstype_mat
# define casio_mcstype_pic          casio_mcstype_pict
# define casio_mcstype_picture      casio_mcstype_pict
# define casio_mcstype_capture      casio_mcstype_capt
# define casio_mcstype_spreadsheet  casio_mcstype_ssheet
# define casio_mcstype_vector       casio_mcstype_vct
# define casio_mcstype_alpha        casio_mcstype_var
# define casio_mcstype_alphamem     casio_mcstype_var
# define casio_mcstype_variable     casio_mcstype_var
# define casio_mcstype_variables    casio_mcstype_var

/* Macros to check if the type uses the ID, and to interact with it */
# define casio_mcshead_uses_id(CASIO__H) (((CASIO__H)->casio_mcshead_type & (\
	casio_mcstype_list | casio_mcstype_mat  | casio_mcstype_vct | \
	casio_mcstype_pict | casio_mcstype_capt | casio_mcstype_string) || \
	((CASIO__H)->casio_mcshead_type  == casio_mcstype_var && \
	 (CASIO__H)->casio_mcshead_count == 1)))
# define casio_get_id_major(CASIO__I) ((CASIO__I) >> 6)
# define casio_get_id_minor(CASIO__I) ((CASIO__I) & 0x3F)
/* ************************************************************************* */
/*  Main memory platforms                                                    */
/* ************************************************************************* */
/* There are several situations where we can find main memory files.
 * The most obvious one is the entries in an MCS archive (G1M, G1R), but
 * others are directly linked to the protocol and header type with which
 * the file is described -- see `libcasio/format/cas.h`. */

typedef unsigned int casio_mcsinfo_t;

# define casio_mcsinfo_none       0x0000 /* no specific platform */
# define casio_mcsinfo_mcs        0x0001 /* standard MCS archive */
# define casio_mcsinfo_cas40      0x0002 /* legacy CAS40  header */
# define casio_mcsinfo_cas50      0x0004 /* legacy CAS50  header */
# define casio_mcsinfo_cas100     0x0008 /* legacy CAS100 header */

/* Aliases */
# define casio_mcsinfo_cas        casio_mcsinfo_cas40
# define casio_mcsinfo_caspro     casio_mcsinfo_cas50
# define casio_mcsinfo_graph100   casio_mcsinfo_cas100
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
/*  Setup                                                                    */
/* ************************************************************************* */
/* The real setup from the calculator is either a group of 100/200 entries,
 * or a string-byte map (e.g. "Axes" -> 0x01). This structure reduces this
 * big bulky thing to a single usable structure.
 *
 * There are different type of flags and options.
 * First, the input flags. */

# define casio_setupiflag_shift    0x0001 /* SHIFT is pressed */
# define casio_setupiflag_alpha    0x0002 /* ALPHA is pressed */
# define casio_setupiflag_insert   0x0004 /* Insert/overwrite mode */
# define casio_setupiflag_math     0x0008 /* Math input mode, not linear */

/* Then, the window flags. */

# define casio_setupwflag_grid     0x0001 /* Grid */
# define casio_setupwflag_axes     0x0002 /* Axes */
# define casio_setupwflag_plot     0x0004 /* Draw Plot Type instead of Con */
# define casio_setupwflag_coord    0x0010 /* Coord */
# define casio_setupwflag_sigma    0x0020 /* Sigma Display */
# define casio_setupwflag_stat     0x0040 /* Stat Window */
# define casio_setupwflag_graph    0x0080 /* Graph Function */
# define casio_setupwflag_simul    0x0100 /* Simul Graph */
# define casio_setupwflag_slope    0x0200 /* Slope */

/* Then, miscallaneous flags. */

# define casio_setupmflag_deriv    0x0001 /* Derivative */
# define casio_setupmflag_label    0x0002 /* label */
# define casio_setupmflag_date360  0x0004 /* 360 Date Mode, instead of 365 */
# define casio_setupmflag_complex  0x0008 /* Complex answer instead of real */
# define casio_setupmflag_simp     0x0010 /* Auto Simplify (not Manual) */
# define casio_setupmflag_dynamic  0x0020 /* Continue Dynamic Type
                                           * (not Stop) */
# define casio_setupmflag_payment  0x0040 /* Begin Payment (not End) */
# define casio_setupmflag_autocalc 0x0080 /* Auto Calc */
# define casio_setupmflag_cformula 0x0100 /* Show Cell Formula (not Value) */
# define casio_setupmflag_dpfix    0x1000 /* Display Fix instead of Sci */
# define casio_setupmflag_dpnorm   0x2000 /* Display Norm */
# define casio_setupmflag_dpemode  0x4000 /* Toggle /E-mode */

/* Here are some values that cannot be reduced to simple flags.
 * It works just as the `c_cc` member in termios, where `NCCS` is
 * `casio_setup_nvals`. */

# define casio_setup_nvals         6

# define casio_setupval_number     0 /* 0 being comp, 2,8,10,16 being
                                      * the base */
# define casio_setupval_function   1 /* -> 0x18 */
# define casio_setupval_listfile   2 /* List File 1-6, 0 being none */
# define casio_setupval_bg         3 /* Background picture ID, 0 being none */
# define casio_setupval_dispid     4 /* Display ID, see dpfix/dpnorm/dpemode */
# define casio_setupval_residlist  5 /* Resid List */

/* Here are the function types you can encounter/use.
 * TODO: why are they here? */

# define casio_functype_yequ      0x00 /* Y= */
# define casio_functype_requ      0x01 /* r= */
# define casio_functype_param     0x02 /* Param */
# define casio_functype_xequc     0x03 /* X=c */
# define casio_functype_ygt       0x04 /* Y> */
# define casio_functype_ylt       0x05 /* Y< */
# define casio_functype_yge       0x06 /* Y>= */
# define casio_functype_yle       0x07 /* Y<= */

/* Main structure */
typedef struct casio_setup_s {
	unsigned int  casio_setup_iflags;
	unsigned int  casio_setup_wflags;
	unsigned int  casio_setup_mflags;
	unsigned char casio_setup_vals[casio_setup_nvals];
} casio_setup_t;
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
 * First, here are the flags: */

# define casio_mcsflag_unfinished 0x8000 /* is there still parts to read? */
# define casio_mcsflag_multiple   0x4000 /* is a group */
# define casio_mcsflag_request    0x2000 /* is a request */

# define casio_mcsflag_complex    0x0001 /* is a complex variable */

/* And here is the main structure. Don't be afraid, it doesn't bite. */

typedef struct casio_mcshead_s {
	/* file main information */
	casio_mcstype_t  casio_mcshead_type;
	casio_mcsinfo_t  casio_mcshead_info;
	unsigned int     casio_mcshead_flags;
	unsigned int     casio_mcshead_id;

	/* content size */
	int            casio_mcshead_count;
	unsigned int   casio_mcshead_width;
	unsigned int   casio_mcshead_height;
	unsigned long  casio_mcshead_size;

	/* raw data */
	unsigned int           casio_mcshead_rawtype;
	casio_pictureformat_t  casio_mcshead_picformat;

	/* strings */
	char casio_mcshead_name[13];
	char casio_mcshead_password[9];
	char casio_mcshead_group[17];
	char casio_mcshead_dirname[9];
	char casio_mcshead_appname[4];
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
/* Manage a main memory file from a head, and free a main memory file. */

extern int  casio_make_mcsfile OF((casio_mcsfile_t **casio__handle,
	const casio_mcshead_t *casio__head));
extern void casio_free_mcsfile OF((casio_mcsfile_t  *casio__handle));

/* Decode and encode an MCS file from an MCS archive. */

extern int casio_decode_mcsfile_head OF((casio_mcshead_t *casio__head,
	int casio__raw_type, const unsigned char *casio__groupname,
	const unsigned char *casio__dirname, const unsigned char *casio__filename,
	unsigned long filesize));

extern int casio_decode_mcsfile      OF((casio_mcsfile_t **casio__handle,
	const casio_mcshead_t *casio__head, casio_stream_t *casio__buffer));
extern int casio_decode_mcsfile_data OF((casio_mcsfile_t **casio__handle,
	const casio_mcshead_t *casio__head,
	const void *casio__data, size_t casio__size));

extern int casio_encode_mcsfile      OF((casio_mcsfile_t  *casio__handle,
	casio_stream_t *casio__buffer));

/* Decode and encode an MCS file from a CASIOLINK environment. */

extern int casio_decode_casfile_head OF((casio_mcshead_t *casio__head,
	casio_stream_t *casio__buffer));
extern int casio_decode_casfile_part OF((casio_mcsfile_t *casio__file,
	casio_stream_t *casio__stream));

extern int casio_encode_casfile_head OF((casio_mcshead_t *casio__head,
	casio_stream_t *casio__buffer));
extern int casio_encode_casfile_part OF((casio_mcsfile_t *casio__file,
	casio_stream_t *casio__buffer));

/* Correct a head (add raw data). */

extern int casio_correct_mcsfile_head OF((casio_mcshead_t *casio__head));
extern int casio_correct_casfile_head OF((casio_mcshead_t *casio__head));

/* Compare MCS files (for ordering). */

extern int casio_compare_mcsfiles OF((const casio_mcsfile_t *first,
	const casio_mcsfile_t *second));

CASIO_END_DECLS
CASIO_END_NAMESPACE
#endif /* LIBCASIO_MCSFILE_H */
