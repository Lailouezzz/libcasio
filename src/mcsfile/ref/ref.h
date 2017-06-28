/* ****************************************************************************
 * mcsfile/ref/ref.h -- reference functions.
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
#ifndef  LOCAL_MCSFILE_REF_H
# define LOCAL_MCSFILE_REF_H 1
# include "../mcsfile.h"

/* MCS metadata. */

CASIO_EXTERN int CASIO_EXPORT casio_correct_mcshead_from_mcs
	OF((casio_mcshead_t *casio__head));
CASIO_EXTERN int CASIO_EXPORT casio_correct_mcshead_to_mcs
	OF((casio_mcshead_t *casio__head));

/* CAS40 & CAS50 data type. */

CASIO_EXTERN int CASIO_EXPORT casio_correct_mcshead_from_casdata
	OF((casio_mcshead_t *casio__head));
CASIO_EXTERN int CASIO_EXPORT casio_correct_mcshead_to_casdata
	OF((casio_mcshead_t *casio__head));

/* Check if a CAS app exists. */

CASIO_EXTERN int CASIO_EXPORT casio_check_cas_app
	OF((int casio__ext, const char *casio__app));

#endif /* LOCAL_MCSFILE_REF_H */
