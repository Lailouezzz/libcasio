/* *****************************************************************************
 * g1a-wrapper/main.c -- g1a-wrapper main source.
 * Copyright (C) 2016-2017 Thomas "Cakeisalie5" Touhey <thomas@touhey.fr>
 *
 * This file is part of p7utils.
 * p7utils is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2.0 of the License,
 * or (at your option) any later version.
 *
 * p7utils is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with p7utils; if not, see <http://www.gnu.org/licenses/>.
 * ************************************************************************** */
#include "main.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <libg1m.h>

/* ************************************************************************** */
/*  Main function                                                             */
/* ************************************************************************** */
/**
 *	main:
 *	User entry point of the program.
 *
 *	@arg	ac		arguments count.
 *	@arg	av		arguments values.
 *	@return			return code (0 if ok)
 */

int main(int ac, char **av)
{
	int ret = 1; FILE *in = NULL;
	g1m_handle_t *handle = NULL;

	/* Parse args */
	args_t args;
	if (!parse_args(ac, av, &args))
		return (0);

	/* get file length */
	in = fopen(args.infile, "r");
	if (!in) {
		err("couldn't open input file: %s", strerror(errno));
		goto fail;
	}
	if (fseek(in, 0L, SEEK_END) < 0) {
		err("couldn't seek in file.");
		goto fail;
	}
	long off = ftell(in);
	if (off < 0L)
		goto fail;
	size_t sz = (off < 0) ? SIZE_MAX : (size_t)off;
	if (fseek(in, 0L, SEEK_SET))
		goto fail;

	/* make the handle */
	int err = g1m_make_addin(&handle, g1m_platform_fx, sz,
		args.name, args.intname, &args.version, &args.date);
	if (err) {
		err("couldn't make libg1m handle: %s", g1m_strerror(err));
		goto fail;
	}

	/* decode the icon */
	open_icon(args.iconfile, handle->icon_unsel);

	/* copy the file content */
	unsigned char *p = handle->content;
	while (sz) {
		size_t rd = fread(p, 1, sz, in);
		if (!rd) {
			err("couldn't read binary data: %s", strerror(errno));
			goto fail;
		}
		sz -= rd; p += rd;
	}

	/* encode */
	err = g1m_write(handle, args.outfile);
	if (err) {
		err("couldn't write to output file: %s",
			err == g1m_error_nostream ? strerror(errno) : g1m_strerror(err));
		goto fail;
	}

	/* we're good */
	ret = 0;
fail:
	if (in) fclose(in);
	if (handle) g1m_free(handle);
	return (ret);
}
