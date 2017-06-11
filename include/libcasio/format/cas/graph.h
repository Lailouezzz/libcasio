/* ****************************************************************************
 * libcasio/format/cas/graph.h -- description of the CAS graph function format.
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
#ifndef  LIBCASIO_FORMAT_CAS_GRAPH_H
# define LIBCASIO_FORMAT_CAS_GRAPH_H

/* This is the data type used for the expression of each function which can be
 * used to draw a graph. They are sent when you select 'Y=Data' in the
 * transfer menu.
 * TODO: find out if these have a CAS40 version.
 *
 * The length of the graph function is stored in `height`.
 * The `name` field is the function type, the type and subtype/equality type.
 * The type can be one of these: Y, r, Param, X(constant), Y graph function
 * The equ. type can be one of these: =, >, <, >=, <=.
 *
 * Then, possibly at `prefix` (the doc is unclear about this), you have a
 * string like 'GR BL SLD YBL NSGT BL NS':
 * - 'GR' means graph function;
 * - 'BL' is the color code (BL: blue, OR: orange, GR: green);
 * - 'SLD' is whether it's selected or not (SLD: selected, NSD: not selected);
 * - 'YBL' is another color code (see previous 'BL');
 * - 'NSGT' indicates presence of a variable? (sometimes SLGT);
 * - 'BL' is another color code (see previous 'BL');
 * - 'NS' is linked to 'NSGT' ('NSGT' -> 'NS', 'SLGT' -> 'SL').
 *
 * The function is FONTCHARACTER-encoded. */

#endif /* LIBCASIO_FORMAT_CAS_GRAPH_H */
