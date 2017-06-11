/* ****************************************************************************
 * log/mem.c -- log memory content.
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
#undef islog
#define islog(CASIO__LOGLEVEL) \
	(casio_getlog() <= (CASIO__LOGLEVEL))
#ifndef LIBCASIO_DISABLED_LOG

/**
 *	log_mem_hex:
 *	Prints the octal interpretation of a max of two octets.
 *
 *	@arg	s		the string where to put it
 *	@arg	m		the memory zone to print
 *	@arg	n		the size of the memory zone
 */

static void log_mem_hex(char *s, const unsigned char *m, size_t n)
{
	size_t l = 0;
	while (l < 8) {
		/* put the hex number */
		if (n) { casio_putascii((unsigned char*)s, *m++, 2); s += 2; }
		else { *s++ = ' '; *s++ = ' '; }

		/* decrement size of the memory zone to go */
		n -= !!n;

		/* go to next character if s is at the ending of a group */
		if (l++ % 2) s++;
	}
}

/**
 *	log_mem_asc:
 *	Prints the ascii interpretation of a max of two octets.
 *
 *	@arg	s		the string where to put it
 *	@arg	m		the memory zone to print
 *	@arg	n		the size of the memory zone
 */

static void log_mem_asc(char *s, const unsigned char *m, size_t n)
{
	size_t l = 0;
	/* for each byte */
	while (n-- && l++ < 8) {
		/* put the character (or a dot if non printable) */
		if (isprint(*m++)) *s++ = *((const char*)m - 1);
		else *s++ = '.';
	}
	/* put the line ending */
	*s++ = '\n';
	*s   = '\0';
}

/**
 *	casio_log_mem:
 *	Print memory zone.
 *
 *	@arg	name		the handle name.
 *	@arg	loglevel	the message log level.
 *	@arg	func		the function name.
 *	@arg	m			the memory zone to print.
 *	@arg	n			the size of the memory zone.
 */

void casio_log_mem(casio_loglevel_t loglevel, const char *func,
	const void *m, size_t n)
{
	char linebuf[58];
	const unsigned char *p;

	/* check if the log level is good */
	if (!islog(loglevel)) return ;

	/* if nothing, print it directly */
	if (!n) {
		casio_log_prefix(loglevel, func);
		fprintf(stderr, "(nothing)\n");
		return ;
	}

	/* prepare line buffer, and loop-loop-loop-loop-loop */
	memcpy(linebuf, "0000 0000 0000 0000 ", 20);
	for (p = m; n > 0;) {
		/* fill in ascii-hex part */
		log_mem_hex(&linebuf[0], p, n);
		/* fill in ascii part */
		log_mem_asc(&linebuf[20], p, n);
		/* then print line */
		casio_log_prefix(loglevel, func);
		fputs(linebuf, stderr);
		/* and increment pointer */
		p += 8;
		n -= min(8, n);
	}
}

#endif
