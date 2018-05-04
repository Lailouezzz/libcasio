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
CASIO_BEGIN_DECLS

/* Get and set the log level at runtime. */

CASIO_EXTERN void        CASIO_EXPORT casio_setlog
	OF((const char *casio__level));
CASIO_EXTERN const char* CASIO_EXPORT casio_getlog
	OF((void));

/* List log levels */

typedef void casio_log_list_t OF((void *casio__cookie,
	const char *casio__str));

CASIO_EXTERN void CASIO_EXPORT casio_listlog
	OF((casio_log_list_t *casio__callback, void *casio__cookie));

CASIO_END_DECLS
CASIO_END_NAMESPACE
#endif /* LIBCASIO_LOG_H */
