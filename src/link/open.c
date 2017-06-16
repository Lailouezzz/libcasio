/* ****************************************************************************
 * link/open.c -- link utilities.
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
 * The `casio_open_link` is the function that really initializes
 * the communication using the given stream (once the stream is initialized).
 * It went public recently to give the possibility to the user to make a
 * custom stream, probably for its platform.
 * ************************************************************************* */
#include "link.h"
#include <stdlib.h>
#include <string.h>

/**
 *	casio_open_link:
 *	Open a link handle.
 *
 *	@arg	h				the handle to create
 *	@arg	flags			the flags.
 *	@arg	stream			the stream to use.
 *	@arg	settings		the stream settings to use.
 *	@return					the error (0 if ok)
 */

int CASIO_EXPORT casio_open_link(casio_link_t **h, unsigned long flags,
	casio_stream_t *stream, const casio_streamattrs_t *settings)
{
	int err = 0;
	casio_link_t *handle;

	/* allocate handle */
	msg((ll_info, "Allocating and building the link handle!"));
	*h = NULL;
	if (!(*h = malloc(sizeof(casio_link_t)))) {
		err = casio_error_alloc;
		goto fail;
	}

	/* initialize handle */
	handle = *h; memset(handle, 0, sizeof(casio_link_t)); /* important! */
	handle->casio_link_stream = stream;

	/* initialize flags */
	handle->casio_link_flags = 0;
	if (flags & CASIO_LINKFLAG_ACTIVE)
		handle->casio_link_flags |= casio_linkflag_active;
	if (flags & CASIO_LINKFLAG_TERM)
		handle->casio_link_flags |= casio_linkflag_term;
	if (flags & CASIO_LINKFLAG_CHECK)
		handle->casio_link_flags |= casio_linkflag_check;
	if (~flags & CASIO_LINKFLAG_NODISC)
		handle->casio_link_flags |= casio_linkflag_disc;
	msg((ll_info, "[Options] Active: %s",
		flags & CASIO_LINKFLAG_ACTIVE ? "yes" : "no"));
	msg((ll_info, "[Options] Check: %s",
		flags & CASIO_LINKFLAG_CHECK  ? "yes" : "no"));
	ifmsg(flags & CASIO_LINKFLAG_CHECK, (ll_info,
		"[Options] Make discovery: %s",
		flags & CASIO_LINKFLAG_NODISC ? "no" : "yes"));
	msg((ll_info, "[Options] Terminate: %s",
		flags & CASIO_LINKFLAG_TERM ? "yes" : "no"));

	/* set communication thingies */
	msg((ll_info, "Initializing stream settings."));
	if (!settings) casio_init_attrs(handle->casio_link_stream);
	else casio_set_attrs(handle->casio_link_stream, settings);

	/* if active, start */
	err = casio_seven_start(handle);
	if (err) goto fail;

	/* initialization went alright */
	return (0);
fail:
	if (*h) casio_close_link(*h);
	*h = NULL;
	return (err);
}

/**
 *	casio_close_link:
 *	De-initialize a link handle..
 *
 *	@arg	handle		the handle to free.
 */

void CASIO_EXPORT casio_close_link(casio_link_t *handle)
{
	/* check if handle is already freed */
	if (!handle) return ;
	msg((ll_info, "Let's end that mess."));

	/* end communication -- FIXME: check error? */
	casio_seven_end(handle);

	/* close stream */
	casio_close(handle->casio_link_stream);

	/* free handle */
	msg((ll_info, "freeing the handle!"));
	free(handle);
}
