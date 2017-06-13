/* ****************************************************************************
 * libcasio/log.h -- libcasio logging.
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
#ifndef  LIBCASIO_LOG_H
# define LIBCASIO_LOG_H
# include "cdefs.h"
CASIO_BEGIN_NAMESPACE

/* Log levels -- prefer the functions to these values, that can change! */
typedef int casio_loglevel_t;
# define casio_loglevel_info   0
# define casio_loglevel_warn  10
# define casio_loglevel_error 20
# define casio_loglevel_fatal 30
# define casio_loglevel_none  40

CASIO_BEGIN_DECLS

/* Get and set the log level at runtime */
CASIO_EXTERN void             CASIO_EXPORT casio_setlog
	OF((casio_loglevel_t casio__level));
CASIO_EXTERN casio_loglevel_t CASIO_EXPORT casio_getlog
	OF((void));

/* Get, set and list log level strings */
CASIO_EXTERN const char*      CASIO_EXPORT casio_loglevel_tostring
	OF((casio_loglevel_t casio__level));
CASIO_EXTERN casio_loglevel_t CASIO_EXPORT casio_loglevel_fromstring
	OF((const char *casio__string));

/* List log levels */

typedef void casio_log_list_t OF((void *casio__cookie,
	const char *casio__str));

CASIO_EXTERN void CASIO_EXPORT casio_listlog
	OF((casio_log_list_t *casio__callback, void *casio__cookie));

CASIO_END_DECLS
CASIO_END_NAMESPACE
#endif /* LIBCASIO_LOG_H */
