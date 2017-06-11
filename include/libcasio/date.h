/* ****************************************************************************
 * libcasio/date.h -- description of CASIO's dates.
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
#ifndef  LIBCASIO_DATE_H
# define LIBCASIO_DATE_H
# include <libcasio/cdefs.h>
# include <time.h>
CASIO_BEGIN_NAMESPACE

/* CASIO's dates look like 'YYYY.MMDD.HHmm', where 'YYYY' is the major,
 * 'MM' is the month, 'DD' is the day, 'HH' is the hour and 'mm' is the
 * minutes. The date is considered UTC. */

CASIO_BEGIN_DECLS
extern int casio_decode_date  OF((time_t *casio__date,
	const char *casio__raw));
extern int casio_encode_date  OF((char *casio__raw,
	const time_t *casio__date));
CASIO_END_DECLS

CASIO_END_NAMESPACE
#endif /* LIBCASIO_VERSION_H */
