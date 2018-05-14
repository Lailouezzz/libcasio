/* ****************************************************************************
 * link/seven_mcs/iter.c -- iterate on files on a protocol seven main memory.
 * Copyright (C) 2018 Thomas "Cakeisalie5" Touhey <thomas@touhey.fr>
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
#include "seven_mcs.h"

#define IS_ALPHA 0
#define IS_SETUP 1
#define IS_OTHER 2

struct thecookie {
	casio_link_t *handle;
	int status;
	casio_mcshead_t head;
};

/* Alpha memory and settings are two special entries that are not explicitely
 * listed and got/sent as files, we ought to make an abstraction powerful
 * enough to treat them as they are. */

const casio_mcshead_t casio_sevenmcs_list_alpha_entry = {
	0, casio_mcstype_alphamem, 0,
	/* count: */ 29, 0, 0, 0,
	/* rawtype: */ 0x00, 0, 0,
	/* name: */ "ALPHA MEM", "",
	/* group: */ "ALPHA MEM",
	/* dirname: */ "$GLOBAL",
	"", ""
};
const casio_mcshead_t casio_sevenmcs_list_setup_entry = {
	0, casio_mcstype_setup, 0,
	0, 0, 0, 100,
	/* rawtype: */ 0x14, 0, 0,
	/* name: */ "SETUP", "",
	/* group: */ "SETUP",
	/* dirname: */ "$GLOBAL",
	"", ""
};

/**
 *	next_entry:
 *	Get the next file.
 *
 *	@arg	cookie		the iterator cookie.
 *	@arg	ptr			the pointer to set.
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL int next_entry(struct thecookie *cookie, void **ptr)
{
	int err;
	casio_link_t *handle = cookie->handle;
	casio_mcshead_t *head = &cookie->head;

	/* The pointer will always be on the head contained in the cookie. */

	*ptr = head;

	/* Special file. */

	if (cookie->status == IS_ALPHA) {
		cookie->status++;
		memcpy(head, &casio_sevenmcs_list_alpha_entry, sizeof(*head));
		return (0);
	}

	if (cookie->status == IS_SETUP) {
		cookie->status++;
		memcpy(head, &casio_sevenmcs_list_setup_entry, sizeof(*head));
		return (0);
	}

	/* Main loop. */

	while (1) {
		/* Get the next request. */

		err = casio_seven_get_next_request(handle);
		if (err) { /* `casio_error_iter` included */
			*ptr = NULL;
			return (err);
		}

		/* Copy the raw information. */

		head->casio_mcshead_flags = casio_mcsfor_mcs;
		head->casio_mcshead_size = response.casio_seven_packet_filesize;
		head->casio_mcshead_rawtype = response.casio_seven_packet_mcstype;

		if (response.casio_seven_packet_args[0]) {
			strncpy(head->casio_mcshead_dirname,
				response.casio_seven_packet_args[0], 8);
			head->casio_mcshead_dirname[8] = 0;
		} else
			head->casio_mcshead_dirname[0] = 0;

		if (response.casio_seven_packet_args[1]) {
			strncpy(head->casio_mcshead_name,
				response.casio_seven_packet_args[1], 12);
			head->casio_mcshead_name[12] = 0;
		} else
			head->casio_mcshead_name[0] = 0;

		if (response.casio_seven_packet_args[2]) {
			strncpy(head->casio_mcshead_group,
				response.casio_seven_packet_args[2], 16);
			head->casio_mcshead_group[16] = 0;
		} else
			head->casio_mcshead_group[0] = 0;

		casio_correct_mcshead(head, 0);

		/* Send an ACK. */

		if ((err = casio_seven_send_ack(handle, 1)))
			return (err);

		/* Check if it is a file we want to ignore. */

		if (head->casio_mcshead_type
		 & (casio_mcstype_alpha | casio_mcstype_setup))
			continue;

		break;
	}

	/* The head is ready, and `*ptr` is already filled (since the beginning
	 * of the function), so let's send this! */

	return (0);
}

/**
 *	free_cookie:
 *	Free the cookie.
 *
 *	@arg	cookie		the cookie.
 */

CASIO_LOCAL void free_cookie(struct thecookie *cookie)
{
	int err;

	/* Try to empty the iterator until we're active */

	while (!casio_seven_get_next_request(cookie->handle)
	 && !casio_seven_send_ack(cookie->handle, 1));

	/* Free the cookie. */

	casio_free(cookie);
}

/* The functions structure. */

CASIO_LOCAL casio_iter_funcs_t funcs = {
	(casio_next_t *)next_entry,
	NULL,
	(casio_end_t *)free_cookie
};

/**
 *	casio_sevenmcs_iter:
 *	Iterate on a Protocol 7.00 MCS.
 *
 *	@arg	cookie		the cookie.
 *	@arg	iterp		the iterator to create.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_sevenmcs_iter(sevenmcs_t *cookie,
	casio_iter_t **iterp)
{
	casio_link_t *handle = cookie->sevenmcs_link;
	struct thecookie *itercookie;
	int err;

	/* Allocate the cookie. */

	itercookie = casio_alloc(sizeof(*cookie), 1);
	if (!itercookie)
		return (casio_error_alloc);
	itercookie->handle = handle;
	itercookie->status = IS_ALPHA;

	/* Prepare the link. */

	if ((err = casio_seven_send_cmdmcs_reqallinfo(handle))
	 || (err = casio_seven_start_server(handle))) {
		casio_free(cookie);
		return (err);
	}

	/* We're good, let's create the iterator. */

	return (casio_iter(iterp, itercookie, &funcs));
}
