/* ****************************************************************************
 * link/seven/scsi.c -- use SCSI to exchange data.
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
 * These functions are the one used behind the others to send a packet.
 * ************************************************************************* */
#include "data.h"

/* The fx-CG* calculators (fx-CG10/20, known as the Prizm, and the fx-CG50,
 * known as the Graph 90+E in France) uses some vendor-specific commands
 * to exchange bytes over an SCSI connexion. This stream makes that
 * accessible through a simple stream.
 *
 * To gather more information, here's the page dealing with that on Cemetech:
 * https://www.cemetech.net/prizm/USB_Communication
 *
 * Multi-byte integers are expressed in big endian.
 *
 * To receive data, we must probe first if there is data available.
 * In order to do that, we use the 0xC0 command which returns the following
 * data:
 *
 * D0 00 00 00 00 00 00 SS SS 00 AA AA 00 00 00 00
 *
 * Where `SS SS` represents the amount of available data and `AA AA`
 * is the activity status of the Prizm (`10 00` if busy).
 *
 * TODO: The initial `D0 00` probably links to the `CAL00D0` we receive
 * in Protocol 7.00… or not?
 *
 * Then, if data is available, we want to receive it, so we send the
 * following command:
 *
 * C1 00 00 00 00 00 SS SS
 *
 * Where `SS SS` is the amount of bytes we want to receive. The Prizm will
 * answer with the raw data.
 *
 * To send data, we first have to probe (each time!) to check if the
 * calculator is busy or not (by checking the activity field). Then, if it
 * is not busy, we send the following command:
 *
 * C2 00 00 00 00 00 SS SS
 *
 * Where `SS SS` is the quantity of data we are about to send next. After
 * this command is sent, we send our raw data. */

/* ---
 * Cookie definition.
 * --- */

/* The cookie contains the following data:
 * - the original stream to use for SCSI requests;
 * - the buffer (with size). */

#define COOKIE_BUFFER_SIZE MAX_PACKET_SIZE
#define reset_cookie(COOKIE) \
	(COOKIE)->off = 0; \
	(COOKIE)->left = 0; \
	(COOKIE)->ptr = (casio_uint8_t *)&(COOKIE)[1]

typedef struct {
	casio_stream_t *stream;
	size_t size, off, left;
	casio_uint8_t *ptr;
} seven_scsi_cookie_t;

/* ---
 * Read and write from the stream.
 * --- */

CASIO_LOCAL int seven_scsi_read(seven_scsi_cookie_t *cookie,
	unsigned char *buffer, size_t *size)
{
	casio_scsi_t scsi; int err;

	/* Empty what's already in the buffer. */

	if (cookie->left) {
		if (cookie->left >= *size) {
			memcpy(buffer, cookie->ptr, *size);
			cookie->ptr += *size;
			cookie->left -= *size;
			return (0);
		}

		memcpy(buffer, cookie->ptr, cookie->left);
		buffer += cookie->left;
		*size -= cookie->left;
		reset_cookie(cookie);
	}

	do {
		casio_uint8_t *to;
		size_t avail;

		/* Polling loop. */

		while (1) {
			casio_uint8_t poll_command[16] = {0xC0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
			casio_uint8_t poll_data[16];

			msg((ll_info, "Polling available data using command C0..."));

			/* Send the polling command, extract the data. */

			scsi.casio_scsi_cmd = poll_command;
			scsi.casio_scsi_cmd_len = 16;
			scsi.casio_scsi_direction = CASIO_SCSI_DIREC_FROM_DEV;
			scsi.casio_scsi_data = poll_data;
			scsi.casio_scsi_data_len = 16;

			if ((err = casio_scsi_request(cookie->stream, &scsi)))
				return (err);

			mem((ll_info, poll_data, 16));

			avail = (poll_data[6] << 8) | poll_data[7];

			/* Check if there are some bytes to get. */

			if (!avail) {
				msg((ll_info, "No available data, sleeping for 1 second."));
				casio_sleep(1000);
				/* FIXME: check the timeout!!! */

				continue;
			}

			break;
		}

		/* Decide which buffer to use and what size to read.
		 * We want to get the most bytes and directly into the buffer
		 * if possible.
		 * We could also check that `avail < 0x10000` because we need to
		 * express it later, but it is imposed by the source format. */

		if (avail > *size && *size <= cookie->size) {
			to = cookie->ptr;
			if (avail > cookie->size)
				avail = cookie->size;
		} else {
			to = buffer;
			if (avail > *size)
				avail = *size;
		}

		/* Actually get the data. */

		{
			casio_uint8_t recv_command[16] = {0xC1,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

			recv_command[6] = (avail >> 8) & 0xFF;
			recv_command[7] = avail & 0xFF;

			scsi.casio_scsi_cmd = recv_command;
			scsi.casio_scsi_cmd_len = 16;
			scsi.casio_scsi_direction = CASIO_SCSI_DIREC_FROM_DEV;
			scsi.casio_scsi_data = cookie->ptr;
			scsi.casio_scsi_data_len = avail;

			if ((err = casio_scsi_request(cookie->stream, &scsi)))
				return (err);
		}

		if (to == buffer) {
			buffer += avail;
			*size -= avail;
		} else {
			cookie->left = avail;

			memcpy(buffer, cookie->ptr, *size);
			cookie->ptr += *size;
			cookie->left -= *size;
			*size = 0;
		}
	} while (*size);

	return (0);
}

CASIO_LOCAL int seven_scsi_write(seven_scsi_cookie_t *cookie,
	unsigned char const *buffer, size_t size)
{
	casio_scsi_t scsi;
	int err;

	do {
		casio_uint16_t activity;

		/* Polling loop. */

		while (1) {
			casio_uint8_t poll_command[16] = {0xC0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
			casio_uint8_t poll_data[16];

			msg((ll_info, "Polling the activity using command C0..."));

			/* Poll to check the activity. */

			scsi.casio_scsi_cmd = poll_command;
			scsi.casio_scsi_cmd_len = 16;
			scsi.casio_scsi_direction = CASIO_SCSI_DIREC_FROM_DEV;
			scsi.casio_scsi_data = poll_data;
			scsi.casio_scsi_data_len = 16;

			if ((err = casio_scsi_request(cookie->stream, &scsi)))
				return (err);

#if 0
			activity = (poll_data[10] << 8) | poll_data[11];

			if (activity == 0x1000) {
				/* The calculator is busy.
				 * FIXME: delay and check the timeout!! */

				continue;
			}
#endif

			break;
		}

		/* Actually send some of the data. */

		{
			casio_uint16_t to_send = (size > 0xFFFF) ? 0xFFFF : size;
			casio_uint8_t send_command[16] = {0xC2,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

			msg((ll_info, "Sending the data using command C2..."));

			send_command[6] = (to_send >> 8) & 0xFF;
			send_command[7] = to_send & 0xFF;

			scsi.casio_scsi_cmd = send_command;
			scsi.casio_scsi_cmd_len = 16;
			scsi.casio_scsi_direction = CASIO_SCSI_DIREC_TO_DEV;
			scsi.casio_scsi_data = (casio_uint8_t *)buffer;
			scsi.casio_scsi_data_len = to_send;

			if ((err = casio_scsi_request(cookie->stream, &scsi)))
				return (err);

			buffer += to_send;
			size -= to_send;
		}
	} while (size);

	return (0);
}

/* ---
 * SCSI requests.
 * --- */

CASIO_LOCAL int seven_scsi_request(seven_scsi_cookie_t *cookie,
	casio_scsi_t *request)
{
	/* Actually, just transmit the request to the source stream. */

	return (casio_scsi_request(cookie->stream, request));
}

/* ---
 * Manage the stream.
 * --- */

CASIO_LOCAL int seven_scsi_close(seven_scsi_cookie_t *cookie)
{
	casio_close(cookie->stream);
	free(cookie);
	return (0);
}

CASIO_LOCAL casio_streamfuncs_t seven_scsi_funcs = {
	(casio_stream_close_t *)seven_scsi_close,
	NULL,
	(casio_stream_read_t *)seven_scsi_read,
	(casio_stream_write_t *)seven_scsi_write,
	NULL,
	NULL,
	(casio_stream_scsi_t *)seven_scsi_request
};

int CASIO_EXPORT casio_open_seven_scsi(casio_stream_t **streamp,
	casio_stream_t *original)
{
	seven_scsi_cookie_t *cookie = NULL;

	/* Check if the original stream supports SCSI. */

	if (~casio_get_openmode(original) & CASIO_OPENMODE_SCSI) {
		/* We shall not close the original stream because the caller
		 * might want to use it if this stream hasn't been opened. */

		return (casio_error_op);
	}

	/* Allocate and prepare the cookie. */

	cookie = casio_alloc(1, sizeof(seven_scsi_cookie_t) + COOKIE_BUFFER_SIZE);
	if (!cookie) {
		casio_close(original);
		return (casio_error_alloc);
	}

	cookie->stream = original;
	cookie->size = COOKIE_BUFFER_SIZE;
	reset_cookie(cookie);

	/* Create the stream. */

	return (casio_open_stream(streamp,
		CASIO_OPENMODE_READ | CASIO_OPENMODE_WRITE | CASIO_OPENMODE_SCSI,
		cookie, &seven_scsi_funcs, 0));
}
