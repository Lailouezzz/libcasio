/* ****************************************************************************
 * stream/builtin/libusb/scsi.c -- make an scsi request on a libusb stream.
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
#include "libusb.h"
#ifndef LIBCASIO_DISABLED_LIBUSB

/* SCSI over USB in libusb uses the Bulk-Only protocol defined here:
 * http://www.usb.org/developers/docs/devclass_docs/usbmassbulk_10.pdf
 *
 * CBW (command block wrappers) for USB-attached SCSI Mass Storage,
 * with integer fields using little-endian:
 *
 * - Signature (4B): identify the packet as a CBW.
 *   [0x55, 0x53, 0x42, 0x43] (USBC)
 * - Tag (4B): answerer will echo this field to us in the associated CSW.
 *   Used by BrandonW: [0x41, 0x42, 0x43, 0x44] (ABCD)
 * - Data Transfer Length (4B): what is to come as data.
 * - Flags (1B):
 *   - Bit 7 (128): direction, 0 for host to device, 1 for device to host.
 *   - Other bits:  reserved or obsolete, set to zero (0).
 * - LUN (1B): LUN (from 0 to 15).
 * - Command Block Length (1B): (6, 10, 12 or 16).
 * - (Command Descriptor Block content)
 *
 * CSW (command status wrappers) for USB-attached SCSI Mass Storage,
 * with integer fields using little-endian:
 *
 * - Signature (4B): identify the packet as a CSW.
 *   [0x55, 0x53, 0x42, 0x53] (USBS)
 * - Tag (4B): echo the field from the CBW.
 *   Used by BrandonW: [0x41, 0x42, 0x43, 0x44] (ABCD)
 * - Data Residue (4B): —
 * - Status (1B): success or failure.
 *   - 0: command passed (good).
 *   - 1: command failed (bad).
 *   - 2: phase error. */

struct cbw {
	casio_uint8_t signature[4];
	casio_uint8_t tag[4];
	casio_uint8_t data_length[4];
	casio_uint8_t flags;
	casio_uint8_t lun;
	casio_uint8_t cmd_len;
};

struct csw {
	casio_uint8_t signature[4];
	casio_uint8_t tag[4];
	casio_uint8_t residue[4];
	casio_uint8_t status;
};

/**
 *	casio_libusb_scsi_request:
 *	Make an SCSI request on a libusb connected device, using CBW.
 */

int CASIO_EXPORT casio_libusb_scsi_request(cookie_libusb_t *cookie,
	casio_scsi_t *request)
{
	unsigned char cmd[32];
	struct cbw *cbw;
	struct csw csw;
	int sent, recv, libusberr;

	/* Prepare and send the request. */

	memset(cmd, 0, 32);
	cbw = (void *)&cmd[0];
	cbw->signature[0] = 'U';
	cbw->signature[1] = 'S';
	cbw->signature[2] = 'B';
	cbw->signature[3] = 'C';
	cbw->tag[0] = 'A';
	cbw->tag[1] = 'B';
	cbw->tag[2] = 'C';
	cbw->tag[3] = 'D';
	cbw->data_length[0] = request->casio_scsi_data_len & 0xFF;
	cbw->data_length[1] = (request->casio_scsi_data_len >> 8) & 0xFF;
	cbw->data_length[2] = 0;
	cbw->data_length[3] = 0;
	cbw->flags = request->casio_scsi_data_len && request->casio_scsi_direction
		== CASIO_SCSI_DIREC_FROM_DEV ? 128 : 0;
	cbw->lun = 0;
	cbw->cmd_len = request->casio_scsi_cmd_len;
	memcpy(&cmd[15], request->casio_scsi_cmd, request->casio_scsi_cmd_len);

	libusberr = libusb_bulk_transfer(cookie->_handle, ENDPOINT_OUT,
		cmd, 31, &sent, cookie->tmwrite);
	switch (libusberr) {
	case 0: break;

	case LIBUSB_ERROR_PIPE:
	case LIBUSB_ERROR_NO_DEVICE:
	case LIBUSB_ERROR_IO:
		msg((ll_error, "The calculator is not here anymore :("));
		return (casio_error_nocalc);

	default:
		msg((ll_fatal, "libusb error was %d: %s", libusberr,
			libusb_strerror(libusberr)));
		return (casio_error_unknown);
	}

	/* Send or receive the data. */

	if (!request->casio_scsi_data_len) {
		/* Nothing, no data is sent. */
	} else if (request->casio_scsi_direction == CASIO_SCSI_DIREC_FROM_DEV) {
		int left = (int)request->casio_scsi_data_len;
		unsigned char *data = (unsigned char *)request->casio_scsi_data;

		/* Data is received. */

		while (left) {
			libusberr = libusb_bulk_transfer(cookie->_handle, ENDPOINT_IN,
				data, (int)left, &recv, cookie->tmread);
			switch (libusberr) {
			case 0:
				break;

			case LIBUSB_ERROR_PIPE:
			case LIBUSB_ERROR_NO_DEVICE:
			case LIBUSB_ERROR_IO:
				msg((ll_error, "The calculator is not here anymore :("));
				return (casio_error_nocalc);

			case LIBUSB_ERROR_TIMEOUT:
				return (casio_error_timeout);

			default:
				msg((ll_fatal, "libusb error was %d: %s", libusberr,
					libusb_strerror(libusberr)));
				return (casio_error_unknown);
			}

			data += recv;
			left -= recv;
		}
	} else {
		int left = (int)request->casio_scsi_data_len;
		unsigned char *data = (unsigned char *)request->casio_scsi_data;

		msg((ll_info, "Sending %d bytes.", left));

		/* Data is sent. */

		while (left) {
			libusberr = libusb_bulk_transfer(cookie->_handle, ENDPOINT_OUT,
				data, (int)left, &sent, cookie->tmwrite);
			switch (libusberr) {
			case 0:
				break;

			case LIBUSB_ERROR_PIPE:
			case LIBUSB_ERROR_NO_DEVICE:
			case LIBUSB_ERROR_IO:
				msg((ll_error, "The calculator is not here anymore :("));
				return (casio_error_nocalc);

			case LIBUSB_ERROR_TIMEOUT:
				return (casio_error_timeout);

			default:
				msg((ll_fatal, "libusb error was %d: %s", libusberr,
					libusb_strerror(libusberr)));
				return (casio_error_unknown);
			}

			data += sent;
			left -= sent;
		}
	}

	/* Get the status. */

	{
		int left = 13;
		unsigned char *data = (void *)&csw;

		do {
			libusberr = libusb_bulk_transfer(cookie->_handle, ENDPOINT_IN,
				data, (int)left, &recv, cookie->tmread);
			switch (libusberr) {
			case 0:
				break;

			case LIBUSB_ERROR_PIPE:
			case LIBUSB_ERROR_NO_DEVICE:
			case LIBUSB_ERROR_IO:
				msg((ll_error, "The calculator is not here anymore :("));
				return (casio_error_nocalc);

			case LIBUSB_ERROR_TIMEOUT:
				return (casio_error_timeout);

			default:
				msg((ll_fatal, "libusb error was %d: %s", libusberr,
					libusb_strerror(libusberr)));
				return (casio_error_unknown);
			}

			data += recv;
			left -= recv;
		} while (left);
	}

	/* Check the status block itself. */

	if (memcmp(csw.signature, "USBS", 4)
	 || memcmp(csw.tag, cbw->tag, 4))
		return (casio_error_unknown);
	/* TODO: check if csw->status is good and find the error to return
	 * if not */

	/* We're good! */

	return (0);
}

#endif
