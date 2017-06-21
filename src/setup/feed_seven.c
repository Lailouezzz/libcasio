/* ****************************************************************************
 * setup/feed_seven.c -- setup entries through Protocol 7.00.
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

struct corresp {
	const char *name;
	int         offset;
};

CASIO_LOCAL struct corresp correspondances[] = {
	{"Angle",        0x13},
	{"Mode",         0x14},
	{"Func Type",    0x18},
	{"Draw Type",    0x19},
	{"Derivative",   0x1A},
	{"Coord",        0x1B},
	{"Grid",         0x1C},
	{"Axes",         0x1D},
	{"Label",        0x1E},

	{"Stat Wind",    0x20},
	{"Graph Func",   0x21},
	{"Dual Screen",  0x22},
	{"Simul Graph",  0x23},
	{"Dynamic Type", 0x24},
	{"SigmaDisplay", 0x25},
	{"Slope",        0x26},
	{"Payment",      0x27},
	{"Date Mode",    0x28},
	{"Answer Type",  0x29},
	{"Complex Mode", 0x2A},
	{"Display",      0x2B},
	{"Background",   0x2C},
	{"Resid List",   0x2D},
	{"List File",    0x2E},
	{"Variable",     0x2F},
	{"Recur Dual",   0x31},
	{"Auto Calc",    0x4E},
	{"Show Sell",    0x4F},

	{"Move",         0x50},
	{"Sub Name",     0x51},
	{"Input Mode",   0x53},
	{"Locus",        0x54},
	{"Y=Draw Speed", 0x55},
	{"Sketch Line",  0x56},
	{"Frac Result",  0x57},
	{"Recur Type",   0x30},
	{"Inequa Type",  0x5D},
	{"Periods YR",   0x5E},
	{"Q1Q3 Type",    0x5C},
	{"Simplify",     0x5F},

	{NULL,           0}
};

/**
 *	casio_feed_setup_seven:
 *	Feed a setup using a Protocol 7.00 setting.
 *
 *	@arg	setup		the setup.
 *	@arg	name		the setting name.
 *	@arg	value		the setting value.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_feed_setup_seven(casio_setup_t *setup,
	const char *name, int value)
{
	struct corresp *c;

	for (c = correspondances; c->name && strcmp(name, c->name); c++);
	if (!c->name) return (casio_error_op);
	return (casio_feed_setup_mcs(setup, c->offset, value));
}
