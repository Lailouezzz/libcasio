/* *****************************************************************************
 * mcsfile/print.c -- mcsfile directory printing utilities.
 * Copyright (C) 2017 Thomas "Cakeisalie5" Touhey <thomas@touhey.fr>
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

/**
 *	print_file:
 *	Put the description (with the newline).
 *
 *	@arg	file		the MCS file.
 */

static void print_file(const casio_mcshead_t *head)
{
	const char *password;

	if (head->casio_mcshead_dirname[0])
		printf("%s/", head->casio_mcshead_dirname);
	printf("%s: ", head->casio_mcshead_name);

	switch (head->casio_mcshead_type) {
	case casio_mcstype_unknown:
		printf("unknown content (%" PRIuSIZE " octets)\n",
			head->casio_mcshead_size);
		break;

	case casio_mcstype_program:
		password = head->casio_mcshead_password;

		printf("program (");
		if (password && password[0])
			printf("password: '%s')\n", password);
		else
			printf("no password)\n");
		break;

	case casio_mcstype_list:
		printf("list %d (", casio_get_id_minor(head->casio_mcshead_id));
		if (casio_get_id_major(head->casio_mcshead_id))
			printf("from listfile %d, ",
				casio_get_id_major(head->casio_mcshead_id));
		printf("%d columns)\n", head->casio_mcshead_width);
		break;

	case casio_mcstype_mat:
		printf("matrix %c (%d columns, %d rows)\n",
			'A' + casio_get_id_minor(head->casio_mcshead_id) - 1,
			head->casio_mcshead_width, head->casio_mcshead_height);
		break;

	case casio_mcstype_vct:
		printf("vector %c (%d rows)\n",
			'A' + casio_get_id_minor(head->casio_mcshead_id) - 1,
			head->casio_mcshead_height);
		break;

	case casio_mcstype_pict:
		printf("picture %d (double %dx%d image)\n",
			casio_get_id_minor(head->casio_mcshead_id),
			head->casio_mcshead_width, head->casio_mcshead_height);
		break;

	case casio_mcstype_capt:
		printf("capture %d (%dx%d)\n",
			casio_get_id_minor(head->casio_mcshead_id),
			head->casio_mcshead_width, head->casio_mcshead_height);
		break;

	case casio_mcstype_ssheet:
		printf("spreadsheet (%d columns, %d rows)\n",
			head->casio_mcshead_width, head->casio_mcshead_height);
		break;

	case casio_mcstype_string:
		printf("string %d\n", casio_get_id_minor(head->casio_mcshead_id));
		break;

	case casio_mcstype_setup:
		printf("setup\n");
		break;

	case casio_mcstype_alphamem:
		printf("alpha memory\n");
		break;

	default:
		break;
	}
}

/**
 *	print_files:
 *	Put the files.
 *
 *	@arg	handle		the handle.
 */

void print_files(casio_mcs_t *handle)
{
	casio_iter_t *iter;
	casio_mcshead_t *head;

	if (casio_iter_mcsfiles(&iter, handle))
		return ;

	while (!casio_next_mcshead(iter, &head))
		print_file(head);

	casio_end(iter);
}
