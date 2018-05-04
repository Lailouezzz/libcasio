/* ****************************************************************************
 * libcasio/setup.h -- main memory setup.
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
#ifndef  LIBCASIO_SETUP_H
# define LIBCASIO_SETUP_H
# include "cdefs.h"
CASIO_BEGIN_NAMESPACE

/* The real setup from the calculator is either a group of 100/200 byte
 * register, or a string-byte map (e.g. "Axes" -> 0x01).
 * This structure reduces this big bulky thing to a single usable structure.
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

# define casio_setup_nvals         7

# define casio_setupval_number     0 /* 0 being comp, 2,8,10,16 being
                                      * the base */
# define casio_setupval_function   1 /* function type */
# define casio_setupval_draw       2 /* draw type */
# define casio_setupval_listfile   3 /* List File 1-6, 0 being none */
# define casio_setupval_background 4 /* Background picture ID, 0 being none */
# define casio_setupval_dispid     5 /* Display ID, see dpfix/dpnorm/dpemode */
# define casio_setupval_residlist  6 /* Resid List */

/* Here are the function types you can encounter/use.
 * TODO: why are they here? */

# define casio_functype_yequ      0 /* Y= */
# define casio_functype_requ      1 /* r= */
# define casio_functype_param     2 /* Param */
# define casio_functype_xequc     3 /* X=c */
# define casio_functype_ygt       4 /* Y> */
# define casio_functype_ylt       5 /* Y< */
# define casio_functype_yge       6 /* Y>= */
# define casio_functype_yle       7 /* Y<= */

/* Here are the draw types you can encounter/use. */

# define casio_drawtype_con       0
# define casio_drawtype_plot      1

/* Main structure */
typedef struct casio_setup_s {
	unsigned int  casio_setup_iflags;
	unsigned int  casio_setup_wflags;
	unsigned int  casio_setup_mflags;
	unsigned char casio_setup_vals[casio_setup_nvals];
} casio_setup_t;

/* ---
 * Utilities.
 * --- */

CASIO_BEGIN_DECLS

/* Initialize the structure. */

CASIO_EXTERN void CASIO_EXPORT casio_init_setup
	OF((casio_setup_t *casio__setup));

/* Feed the structure. */

CASIO_EXTERN int CASIO_EXPORT casio_feed_setup_seven
	OF((casio_setup_t *casio__setup, const char *casio__name, int casio__val));
CASIO_EXTERN int CASIO_EXPORT casio_feed_setup_mcs
	OF((casio_setup_t *casio__setup, int casio__id, int casio__val));

/* Export settings into a map. */

# if 0
typedef void casio_setup_export_t
	OF((void *casio__cookie, const char *casio__name, int casio__value));

CASIO_EXTERN void CASIO_EXPORT casio_export_setup
	OF((casio_setup_t *casio__setup, casio_setup_export_t *casio__export,
		void *casio__cookie));
# endif

CASIO_END_DECLS
CASIO_END_NAMESPACE
#endif /* LIBCASIO_SETUP_H */
