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
 * - Data Transfer Length (4B): —
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
 * - Data Residue (4B):
 * - Status (1B): success or failure.
 *   - 0: command passed (good).
 *   - 1: command failed (bad).
 *   - 2: phase error. */

/**
 *	casio_libusb_scsi_request:
 *	Make an SCSI request on a libusb connected device, using CBW.
 */

int CASIO_EXPORT casio_libusb_scsi_request(cookie_libusb_t *cookie,
	casio_scsi_t *request)
{


	/* TODO */
	return (casio_error_op);
}

#endif
