/* ****************************************************************************
 * log/log.h -- libcasio's logging system.
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
 *
 * The libcasio logging system is quite simple and defined at compilation time.
 * It cannot be tweaked at runtime (I should probably correct that someday).
 * Also, as it's an internal header, only the libcasio core and built-in
 * streams can use it.
 * ************************************************************************* */
#ifndef  LOCAL_LOG_H
# define LOCAL_LOG_H 1
# include "../internals.h"
# include <stdio.h>

/* These definitions used to be public, but they went private in order to
 * not have numbers *and* strings in the public interface.
 * Strings allow an easier future compatibility. */

typedef int casio_loglevel_t;
# define casio_loglevel_info   0
# define casio_loglevel_warn  10
# define casio_loglevel_error 20
# define casio_loglevel_fatal 30
# define casio_loglevel_none  40

/* Cross-compiler and cross-standard `__func__` variable to display
 * the function on logging. */

# if defined(__cplusplus) ? CASIO_GNUC_PREREQ(2, 6) \
   : !defined(__STRICT_ANSI__) && CASIO_GNUC_PREREQ(2, 4)
#  define CASIO_LOGFUNC __PRETTY_FUNCTION__
# elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#  define CASIO_LOGFUNC __func__
# elif !defined(__STRICT_ANSI__) && CASIO_GNUC_PREREQ(2, 0)
#  define CASIO_LOGFUNC __FUNCTION__
# else
#  define CASIO_LOGFUNC NULL
# endif

/* Log levels */
# define ll_info  casio_loglevel_info,  CASIO_LOGFUNC
# define ll_warn  casio_loglevel_warn,  CASIO_LOGFUNC
# define ll_error casio_loglevel_error, CASIO_LOGFUNC
# define ll_fatal casio_loglevel_fatal, CASIO_LOGFUNC
# define ll_none  casio_loglevel_none,  CASIO_LOGFUNC

/* check if we can log */
# if defined(LIBCASIO_DISABLED_FILE) && !defined(LIBCASIO_DISABLED_LOG)
#  define LIBCASIO_DISABLED_LOG
# endif
/* ************************************************************************* */
/*  Log utilities                                                            */
/* ************************************************************************* */
/* Main functions */
# if defined(LIBCASIO_DISABLED_LOG)
#  define msg(CASIO__ARGS)
#  define mem(CASIO__ARGS)
#  define islog(CASIO__LOGLEVEL) 0

#  define   ifmsg(CASIO__COND, CASIO__ARGS)
#  define   ifmem(CASIO__COND, CASIO__ARGS)
#  define elifmsg(CASIO__COND, CASIO__ARGS)
#  define elifmem(CASIO__COND, CASIO__ARGS)
#  define elsemsg(             CASIO__ARGS)
#  define elsemem(             CASIO__ARGS)

# else

/* Main macros, which redirect to whether the logging functions if logging
 * is enabled, or nothing if it is not (`LIBCASIO_DISABLED_LOG` defined).
 * Use double parenthesis to transmit args, e.g.:
 *
 *		msg((ll_info, "my %sformat string number %d", "wonderful", 1));
 *		mem((ll_info, my_data_buffer, the_size_of_my_data_buffer));
 *
 * This is some kind of "variadic macro", but for K&R/ANSI C. */

#  define msg(CASIO__ARGS) \
	casio_log_msg CASIO__ARGS
#  define mem(CASIO__ARGS) \
	casio_log_mem CASIO__ARGS
#  define islog(CASIO__LOGLEVEL) \
	casio_islog(CASIO__LOGLEVEL)

/* Here are the linked to conditional kind of macros. */

#  define   ifmsg(CASIO__COND, CASIO__ARGS) \
	if (CASIO__COND) { msg(CASIO__ARGS); }
#  define   ifmem(CASIO__COND, CASIO__ARGS) \
	if (CASIO__COND) { mem(CASIO__ARGS); }
#  define elifmsg(CASIO__COND, CASIO__ARGS) \
	else if (CASIO__COND) { msg(CASIO__ARGS); }
#  define elifmem(CASIO__COND, CASIO__ARGS) \
	else if (CASIO__COND) { mem(CASIO__ARGS); }
#  define elsemsg(             CASIO__ARGS) \
	else { msg(CASIO__ARGS); }
#  define elsemem(             CASIO__ARGS) \
	else { mem(CASIO__ARGS); }

/* Conversion functions between strings and numbers.
 * Strings are for the external API, numbers for the internal one. */

CASIO_EXTERN const char*      CASIO_EXPORT casio_loglevel_tostring
	OF((casio_loglevel_t casio__level));
CASIO_EXTERN casio_loglevel_t CASIO_EXPORT casio_loglevel_fromstring
	OF((const char *casio__string));

/* Here are the main functions. Don't use them directly, prefer the
 * `msg` and `mem` macros as they are sensible to the fact that logging
 * is enabled or not. */

CASIO_EXTERN int  CASIO_EXPORT casio_islog
	OF((casio_loglevel_t casio__level, const char *casio__func));
CASIO_EXTERN void CASIO_EXPORT casio_log_prefix
	OF((casio_loglevel_t casio__loglevel, const char *casio__func));

#  if defined(__STDC__) && __STDC__
CASIO_EXTERN void CASIO_EXPORT casio_log_msg
	(casio_loglevel_t casio__loglevel, const char *casio__func,
	const char *casio__format, ...);
CASIO_EXTERN void CASIO_EXPORT casio_log_mem
	(casio_loglevel_t casio__loglevel, const char *casio__func,
	const void *casio__m, size_t casio__n);
#  else
CASIO_EXTERN void CASIO_EXPORT casio_log_msg();
CASIO_EXTERN void CASIO_EXPORT casio_log_mem();
#  endif
# endif

#endif /* LOCAL_LOG_H */
