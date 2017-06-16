/* ****************************************************************************
 * version.c -- libcasio version printing.
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
#include "internals.h"
#ifndef LIBCASIO_STATIC
# include <unistd.h>

/**
 *	version_message:
 *	The message that should be displayed when the library is executed.
 */

CASIO_LOCAL const char version_message[] =
"libcasio v" LIBCASIO_VERSION " (licensed under LGPL3)\n"
"Maintained by " LIBCASIO_MAINTAINER ".\n"
"\n"
"This is free software; see the source for copying conditions.\n"
"There is NO warranty; not even for MERCHANTABILITY or\n"
"FITNESS FOR A PARTICULAR PURPOSE.";

/**
 *	libcasio__version:
 *	Display version when the library is executed.
 */

extern void libcasio__version(void)
	__attribute__((noreturn));
void libcasio__version(void)
{
	puts(version_message);
	_exit(0);
}

#endif
