/* ****************************************************************************
 * link/seven_mcs/get.c -- get a file from a protocol seven main memory.
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
 *
 * FIXME: add the special cases of setup and alpha memory.
 * ************************************************************************* */
#include "seven_mcs.h"

/* ---
 * Protocol 7.00 MCS file gathering.
 * --- */

struct thecookie {
	int                    _called;
	int                    _err;
	casio_mcshead_t       *_head;
	casio_mcsfile_t      **_mcsfile;
	casio_link_progress_t *_disp;
	void                  *_disp_cookie;
};

/**
 *	receive_file:
 *	Get the main memory file.
 *
 *	@arg	cookie		the cookie (uncasted).
 *	@arg	handle		the link handle.
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL int receive_file(struct thecookie *cookie, casio_link_t *handle)
{
	casio_stream_t  *data_stream = NULL;
	casio_mcshead_t *head        = cookie->_head;

	/* Check if the callback is already called. */
	if (cookie->_called)
		return (casio_error_unknown);
	cookie->_called = 1;

	/* Correct the head. */
	head->casio_mcshead_size = response.casio_seven_packet_filesize;

	/* Make the data stream. */
	cookie->_err = casio_seven_open_data_stream(&data_stream, handle, 0,
		cookie->_disp, cookie->_disp_cookie);
	if (cookie->_err) return (casio_error_unknown);

	/* Decode the MCS file. */
	cookie->_err = casio_decode_mcsfile(cookie->_mcsfile, head, data_stream);
	casio_close(data_stream);
	if (cookie->_err) return (casio_error_unknown);

	/* Send the final ack and return. */
	return (casio_seven_send_ack(handle, 1));
}

/**
 *	get_setup_entry:
 *	Get a setup entry.
 *
 *	@arg	setup		the setup to feed.
 *	@arg	handle		the link handle.
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL int get_setup_entry(casio_setup_t *setup, casio_link_t *handle)
{
	if (response.casio_seven_packet_args[0]
	 && response.casio_seven_packet_args[1]) {
		unsigned int value = (unsigned int)casio_getascii(
			(void*)response.casio_seven_packet_args[1], 2);

		casio_feed_setup_seven(setup,
			response.casio_seven_packet_args[0], value);
	}

	/* Once the setup has been fed, send the ACK. */
	return (casio_seven_send_ack(handle, 1));
}

/* ---
 * File request function.
 * --- */

/**
 *	request_file:
 *	Request a file.
 *
 *	@arg	cookie		the interface cookie.
 *	@arg	mcsfile		the MCS file to get.
 *	@arg	head		the MCS head.
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL int request_file(sevenmcs_t *cookie,
	casio_mcsfile_t **mcsfile, casio_mcshead_t *head)
{
	int err; casio_link_t *handle = cookie->sevenmcs_link;
	struct thecookie thecookie;

	/* Send the command. */
	msg((ll_info, "Sending the file request."));
	err = casio_seven_send_cmd_data(handle,
		casio_seven_cmdmcs_reqfile, 0, head->casio_mcshead_rawtype, 0,
		head->casio_mcshead_dirname, head->casio_mcshead_name,
		head->casio_mcshead_group, NULL, NULL, NULL);
	if (err) return (err);

	/* Check the answer. */
	if (response.casio_seven_packet_type == casio_seven_type_nak
	 && response.casio_seven_packet_code == casio_seven_err_other)
		return (casio_error_notfound);
	else if (response.casio_seven_packet_type != casio_seven_type_ack)
		return (casio_error_unknown);

	/* Prepare the cookie. */
	thecookie._called      = 0;
	thecookie._err         = 0;
	thecookie._mcsfile     = mcsfile;
	thecookie._head        = head;
	thecookie._disp        = NULL;
	thecookie._disp_cookie = NULL;

	msg((ll_info, "Running the server."));
	err = casio_seven_serve(handle, handle->casio_link_seven_callbacks,
		&thecookie);
	if (err) return (err);

	/* Check if the function was called. */
	return (thecookie._called ? thecookie._err : casio_error_unknown);
}

/* ---
 * Special types.
 * --- */

/**
 *	request_alphamem:
 *	Get the alpha memory.
 *
 *	@arg	cookie		the interface cookie.
 *	@arg	mcsfile		the MCS file to get.
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL const char *variable_names[] = {
	"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N",
	"O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
	/* theta */ "\xCE", /* r: */ "\xCD", /* Ans: */ "\xC0"};

CASIO_LOCAL int request_alphamem(sevenmcs_t *cookie, casio_mcsfile_t **mcsfile)
{
	int err, i;
	casio_mcscell_t *vars;
	casio_mcshead_t  varhead = {
		casio_mcsfor_mcs, 0, 0,
		1, 0, 0, 0,
		0x00, 0, 0,
		/* var name (will be completed in the loop): */ "", "",
		/* group: */ "ALPHA MEM",
		/* dirname: */ "$GLOBAL",
		"", ""
	};

	/* Allocate the alpha memory file. */
	err = casio_make_mcsfile(mcsfile, &casio_sevenmcs_list_alpha_entry);
	if (err) return (err);
	vars = (*mcsfile)->casio_mcsfile_vars;

	/* Get each variable. */
	for (i = 0; i < 29; i++) {
		casio_mcsfile_t *varfile = NULL;

		/* Request the variable file. */
		strcpy(varhead.casio_mcshead_name, variable_names[i]);
		msg((ll_info, "Requesting variable %d/29", i + 1));
		err = request_file(cookie, &varfile, &varhead);
		if (err) goto fail;

		/* Copy the variable. */
		memcpy(&vars[i], &varfile->casio_mcsfile_var, sizeof(casio_mcscell_t));

		/* Free the file. */
		casio_free_mcsfile(varfile);
	}

	return (0);
fail:
	casio_free_mcsfile(*mcsfile);
	*mcsfile = NULL;
	return (err);
}

/**
 *	request_setup:
 *	Get the calculator settings.
 *
 *	@arg	cookie		the interface cookie.
 *	@arg	mcsfile		the MCS file to get.
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL int request_setup(sevenmcs_t *cookie, casio_mcsfile_t **mcsfile)
{
	int err; casio_setup_t *setup;
	casio_link_t *handle = cookie->sevenmcs_link;

	/* Allocate the alpha memory file. */
	err = casio_make_mcsfile(mcsfile, &casio_sevenmcs_list_setup_entry);
	if (err) return (err);
	setup = &(*mcsfile)->casio_mcsfile_setup;

	/* Request all setup entries. */
	err = casio_seven_send_cmdmcs_reqallsetup(handle);
	if (err) goto fail;

	/* Setup the server and go! */
	handle->casio_link_seven_callbacks[casio_seven_cmdmcs_sendfile] = NULL;
	handle->casio_link_seven_callbacks[casio_seven_cmdmcs_putsetup] =
		(casio_seven_server_func_t*)&get_setup_entry;

	/* Run the server. */
	err = casio_seven_serve(handle, handle->casio_link_seven_callbacks,
		setup);
	if (err) goto fail;

	/* We're done! */
	return (0);
fail:
	casio_free_mcsfile(*mcsfile);
	*mcsfile = NULL;
	return (err);
}

/* ---
 * Main function.
 * --- */

/**
 *	casio_sevenmcs_get:
 *	Get a file from a Protocol 7.00 main memory interface.
 *
 *	@arg	cookie		the interface cookie.
 *	@arg	mcsfile		the MCS file to get.
 *	@arg	mcshead		the MCS head.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_sevenmcs_get(sevenmcs_t *cookie,
	casio_mcsfile_t **mcsfile, casio_mcshead_t *mcshead)
{
	casio_mcshead_t head;
	casio_link_t *handle = cookie->sevenmcs_link;

	/* MCS file not gathered by default. */
	*mcsfile = NULL;

	/* Prepare the callbacks. */
	memset(handle->casio_link_seven_callbacks, 0,
		256 * sizeof(casio_seven_server_func_t*));
	handle->casio_link_seven_callbacks[casio_seven_cmdmcs_sendfile] =
		(casio_seven_server_func_t*)&receive_file;

	/* Correct the head. */
	msg((ll_info, "Correcting the head."));
	memcpy(&head, mcshead, sizeof(head));
	if (casio_correct_mcshead(&head, casio_mcsfor_mcs))
		return (casio_error_notfound);

	/* Check if we ought to get the alpha memory.
	 * XXX: should we check the directory name? */
	if (!strcmp(head.casio_mcshead_group, "ALPHA MEM")
	 && !strcmp(head.casio_mcshead_name,  "ALPHA MEM"))
		return (request_alphamem(cookie, mcsfile));

	/* Check if we ought to get the setup.
	 * XXX: should we check the directory name? */
	if (!strcmp(head.casio_mcshead_group, "SETUP")
	 && !strcmp(head.casio_mcshead_name,  "SETUP"))
		return (request_setup(cookie, mcsfile));

	/* Request the file. */
	return (request_file(cookie, mcsfile, &head));
}
