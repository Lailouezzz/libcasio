/* ****************************************************************************
 * setup/feed_mcs.c -- setup entries through an MCS setup file.
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
#include "setup.h"

/**
 *	casio_feed_setup_mcs:
 *	Feed setup using an ID.
 *
 *	@arg	setup		the setup.
 *	@arg	id			the MCS setting ID.
 *	@arg	value		the setting value.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_feed_setup_mcs(casio_setup_t *setup, int id, int value)
{
	switch (id) {
	case 0x14: /* "Mode" */
		setup->casio_setup_iflags &=
			~(casio_setupiflag_shift | casio_setupiflag_alpha);
		if (value == 0x01 || value == 0x84)
			setup->casio_setup_iflags |= casio_setupiflag_shift;
		if (value == 0x04 || value == 0x84)
			setup->casio_setup_iflags |= casio_setupiflag_alpha;
		break;
	case 0x15: /* Insert/overwrite */
		if (value - 1) setup->casio_setup_iflags |=  casio_setupiflag_insert;
		else           setup->casio_setup_iflags &= ~casio_setupiflag_insert;
		break;
	case 0x17: /* Number mode */
		switch (value) {
		case 1:
			setup->casio_setup_vals[casio_setupval_number] = 2;
			break;
		case 7:
			setup->casio_setup_vals[casio_setupval_number] = 8;
			break;
		case 9:
			setup->casio_setup_vals[casio_setupval_number] = 10;
			break;
		case 16:
			setup->casio_setup_vals[casio_setupval_number] = 16;
			break;
		default:
			setup->casio_setup_vals[casio_setupval_number] = 0;
		}
		break;
	case 0x18: /* "Func Type" */
		setup->casio_setup_vals[casio_setupval_function] = value;
		break;
	case 0x19: /* "Draw Type" */
		setup->casio_setup_vals[casio_setupval_draw] = value;
		break;

	case 0x1A: /* "Derivative" */
		if (value) setup->casio_setup_mflags |=  casio_setupmflag_deriv;
		else       setup->casio_setup_mflags &= ~casio_setupmflag_deriv;
		break;
	case 0x1B: /* "Coord" */
		if (value) setup->casio_setup_wflags |=  casio_setupwflag_coord;
		else       setup->casio_setup_wflags &= ~casio_setupwflag_coord;
		break;
	case 0x1C: /* "Grid" */
		if (value) setup->casio_setup_wflags |=  casio_setupwflag_grid;
		else       setup->casio_setup_wflags &= ~casio_setupwflag_grid;
		break;
	case 0x1D: /* "Axes" */
		if (value) setup->casio_setup_wflags |=  casio_setupwflag_axes;
		else       setup->casio_setup_wflags &= ~casio_setupwflag_axes;
		break;
	case 0x1E: /* "Label" */
		if (value) setup->casio_setup_wflags |=  casio_setupmflag_label;
		else       setup->casio_setup_wflags &= ~casio_setupmflag_label;
		break;

	case 0x20: /* "Stat Wind" */
		if (value) setup->casio_setup_wflags |=  casio_setupwflag_simul;
		else       setup->casio_setup_wflags &= ~casio_setupwflag_simul;
		break;
	case 0x21: /* "Dynamic Type" */
		if (value) setup->casio_setup_mflags &= ~casio_setupmflag_dynamic;
		else       setup->casio_setup_mflags |=  casio_setupmflag_dynamic;
		break;
	/* TODO: "Dual Screen" */
	case 0x23: /* "Simul Graph" */
		if (value) setup->casio_setup_wflags |=  casio_setupwflag_simul;
		else       setup->casio_setup_wflags &= ~casio_setupwflag_simul;
		break;
	case 0x24: /* "Dynamic Type" */
		if (value) setup->casio_setup_mflags &= ~casio_setupmflag_dynamic;
		else       setup->casio_setup_mflags |=  casio_setupmflag_dynamic;
		break;
	case 0x25: /* "SigmaDisplay" */
		if (value) setup->casio_setup_wflags |=  casio_setupwflag_sigma;
		else       setup->casio_setup_wflags &= ~casio_setupwflag_sigma;
		break;
	case 0x26: /* "Slope" */
		if (value) setup->casio_setup_wflags |=  casio_setupwflag_slope;
		else       setup->casio_setup_wflags &= ~casio_setupwflag_slope;
		break;
	case 0x27: /* "Payment" */
		if (value) setup->casio_setup_mflags |=  casio_setupmflag_payment;
		else       setup->casio_setup_mflags &= ~casio_setupmflag_payment;
		break;
	case 0x28: /* "Date Mode" */
		if (value) setup->casio_setup_mflags |=  casio_setupmflag_date360;
		else       setup->casio_setup_mflags &= ~casio_setupmflag_date360;
		break;
	case 0x29: /* "Answer Type" */
		if (value) setup->casio_setup_mflags |=  casio_setupmflag_complex;
		else       setup->casio_setup_mflags &= ~casio_setupmflag_complex;
		break;
	/* TODO: Complex Mode, Display */
	case 0x2C: /* "Background" */
		setup->casio_setup_vals[casio_setupval_background] = value;
		break;
	case 0x2D: /* "Resid List" */
		setup->casio_setup_vals[casio_setupval_residlist] = value;
		break;
	case 0x2E: /* "List File" */
		setup->casio_setup_vals[casio_setupval_listfile] = value;
		break;
	/* TODO: "Variable" */

	default: return (casio_error_op);
	}

	return (0);
}
