/* ****************************************************************************
 * libcasio/format/mcs/setup.h -- description of the MCS setup file format.
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
#ifndef  LIBCASIO_FORMAT_MCS_SETUP_H
# define LIBCASIO_FORMAT_MCS_SETUP_H

/* Setup files have no headers, they are just a bunch of bytes you can copy.
 * Legacy fx setup files have 100 entries (100 bytes), fx-CG setup files
 * have twice as much (even though the first 100 entries have the same
 * signification in the two).
 *
 * Known settings are:
 * [0x13] Angle (0x00: degrees, 0x01: radians, 0x02: gradians);
 * [0x14] SHIFT/Alpha status (0x00: both off, 0x01: shift only on,
 *        0x04: alpha only on, 0x84: both on);
 * [0x15] Insert/Overwrite status (0x01: overwrite, 0x02: insert);
 * [0x17] Number mode (0x00: Comp, 0x01: Bin, 0x07: Oct, 0x09: Dec, 0x0F: Hex);
 * [0x18] Function type (0x00: Y=, 0x01: r=, 0x02: Param, 0x03: X=c,
 *        0x04: Y>, 0x05: Y<, 0x06: Y>=, 0x07: Y<=);
 * [0x19] Draw type (0x00: Con, 0x01: Plot);
 * [0x1A] Derivative;
 * [0x1B] Coord;
 * [0x1C] Grid;
 * [0x1D] Axes;
 * [0x1E] Label;
 * [0x20] Stat Wind (0x00: Auto, 0x01: Manual);
 * [0x21] Graph Func;
 * [0x22] Dual Screen (0x03: OFF);
 * [0x23] Simul Graph;
 * [0x24] Dynamic Type (0x00: Cont, 0x01: Stop);
 * [0x25] SigmaDisplay;
 * [0x26] Slope;
 * [0x27] Payment (0x00: Bgn, 0x01: End);
 * [0x28] Date Mode (0x00: 365, 0x01: 360);
 * [0x29] Answer Type (0x00: Real, 0x01: Complex);
 * [0x2A] Complex Mode (0x00: Real, 0x01: a+bi, 0x02: r<T);
 * [0x2B] Display (0x00: Norm1, 0x10: Norm2, 0x20-0x29: Fix[0-9],
 *        0x30-0x39: Sci[0-9] -- bit 0x80 toggles /E mode);
 * [0x2C] Background (0x00: None, 0x01-0x14: Pict[1-20]);
 * [0x2D] Resid List (0x00: None, 0x01-0x1A: List[1-26]);
 * [0x2E] List File (0x01-0x06: File[1-6]);
 * [0x2F] Variable (0x00: Range, 0x01-0x26: List[1-26]);
 * [0x30] Recur Type (unknown values, default: 0x01?);
 * [0x31] Recur Dual (Dual Screen) (0x00: T+G, 0x01: OFF);
 * [0x4E] Auto Calc;
 * [0x4F] Show Sell (Show cell) (0x00: Formula, 0x01: Value);
 * [0x50] Move (0x00: Lower, 0x01: Right);
 * [0x51] Sub Name;
 * [0x53] Input Mode (0x00: Math, 0x01: Linear);
 * [0x54] Locus;
 * [0x55] Y=Draw Speed (0x00: Norm, 0x01: High);
 * [0x56] Sketch Line (0x00: Norm, 0x01: Thick, 0x02: Broken, 0x03: Dot);
 * [0x57] Frac Result (0x00: d/c, 0x01: a+b/c);
 * [0x5C] Q1Q3 Type (0x00: Std, 0x01: OnD [STAT]);
 * [0x5D] Ineq Type (0x00: Intersect (And), 0x01: Union (Or) [GRAPH]);
 * [0x5E] Periods/Yr. (0x00: Annu, 0x01: Semi [TVM]);
 * [0x5F] Simplify (0x00: Auto, 0x01: Manual);
 *
 * When options aren't given, it's 0x00 for ON, and 0x01 for OFF. */

#endif /* LIBCASIO_FORMAT_MCS_SETUP_H */
