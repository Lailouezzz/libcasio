/* ****************************************************************************
 * utils/sleep.c -- Sleep in a cross-platform way.
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
#include "../internals.h"

/* ************************************************************************* */
/*  Microsoft Windows environment                                            */
/* ************************************************************************* */
#if defined(__WINDOWS__)
# define default_callback &casio_winsleep
# include <windows.h>

static void casio_winsleep(unsigned long ms)
{
	Sleep(ms);
}
/* ************************************************************************* */
/*  UNIX environments                                                        */
/* ************************************************************************* */
#elif defined(__unix__) || defined(__unix)
# define default_callback &casio_unixsleep
# include <unistd.h>
# include <time.h>

static void casio_unixsleep(unsigned long ms)
{
	struct timespec requested_timestamp;

	requested_timestamp.tv_sec = ms / 1000;
	requested_timestamp.tv_nsec = ms * 1000;
	nanosleep(&requested_timestamp, NULL);
}
/* ************************************************************************* */
/*  Default and main function                                                */
/* ************************************************************************* */
#else
# define default_callback NULL
#endif
static casio_sleep_t *casio_sleep_callback = default_callback;

/**
 *	casio_set_sleep:
 *	Set the sleep function.
 *
 *	@arg	func	the function to set.
 */

void casio_set_sleep(casio_sleep_t *func)
{
	casio_sleep_callback = func;
}

/**
 *	casio_sleep:
 *	Sleep.
 *
 *	@arg	ms		the milliseconds.
 */

int casio_sleep(unsigned long ms)
{
	if (!casio_sleep_callback)
		return (casio_error_op);
	if (!ms) return (0);

	(*casio_sleep_callback)(ms);
	return (0);
}
