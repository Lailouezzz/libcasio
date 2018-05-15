/* ****************************************************************************
 * log/iter.c -- iterate on log levels.
 * Copyright (C) 2016-2017 Thomas "Cakeisalie5" Touhey <thomas@touhey.fr>
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
#include "log.h"

CASIO_LOCAL const char *log_levels[] = {
	"none",
#if !defined(LIBCASIO_DISABLED_LOG)
	"info",
	"warn",
	"error",
	"fatal",
#endif
	NULL
};

/* `next_log()`: next log level. */

CASIO_LOCAL int CASIO_EXPORT next_log(const char ***cookie, const char **level)
{
	if (!**cookie)
		return (casio_error_iter);

	*level = *(*cookie)++;
	return (0);
}

/* `end_log_iter()`: end the iteration log. */

CASIO_LOCAL int CASIO_EXPORT end_log_iter(const char ***cookie)
{
	free(cookie);
	return (0);
}

/* Functions. */

CASIO_LOCAL casio_iter_funcs_t log_funcs = {
	(casio_next_t *)next_log,
	NULL,
	(casio_end_t *)end_log_iter
};

/* `casio_iter_log()`: make a loglevels iterator. */

int CASIO_EXPORT casio_iter_log(casio_iter_t **iterp)
{
	const char ***ptr;

	ptr = malloc(sizeof(char **));
	if (!ptr)
		return (casio_error_alloc);

	*ptr = log_levels;
	return (casio_iter(iterp, (void *)ptr, &log_funcs));
}

/* ---
 * List logs, per compatibility.
 * --- */

/* `casio_listlog()`: list log levels. */

void CASIO_EXPORT casio_listlog(casio_log_list_t *callback, void *cookie)
{
	casio_iter_t *iter;
	const char *level;

	if (casio_iter_log(&iter))
		return ;
	while (casio_next_log(iter, &level))
		(*callback)(cookie, level);

	casio_end(iter);
}
