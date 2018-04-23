/* ****************************************************************************
 * stream/builtin/libusb/open.c -- open a libusb stream.
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

/* Stream callbacks. */
CASIO_LOCAL const casio_streamfuncs_t casio_libusb_callbacks = {
	(casio_stream_close_t*)&casio_libusb_close,
	(casio_stream_settm_t*)&casio_libusb_settm,
	(casio_stream_read_t*)&casio_libusb_read,
	(casio_stream_write_t*)&casio_libusb_write,
	NULL, NULL, NULL
};

/**
 *	casio_openusb_libusb:
 *	Initialize a stream with USB device using libusb.
 *
 *	@arg	handle		the handle to create.
 *	@arg	flags		the flags.
 *	@return				the error code (0 if you're a knoop).
 */

int CASIO_EXPORT casio_openusb_libusb(casio_stream_t **stream)
{
	int err = 0, uerr, id, device_count;
	libusb_context *context = NULL;
	libusb_device *calc = NULL, **device_list = NULL;
	libusb_device_handle *dhandle = NULL;
	cookie_libusb_t *cookie = NULL;
	casio_openmode_t openmode = CASIO_OPENMODE_USB;

	/* open up context */
	if (libusb_init(&context)) {
		msg((ll_fatal, "Couldn't create libusb context."));
		return (casio_error_nocalc);
	}

	/* get device list */
	device_count = libusb_get_device_list(context, &device_list);
	if (device_count < 0) {
		msg((ll_fatal, "couldn't get device list."));
		err = casio_error_nocalc;
		goto fail;
	}

	/* look for the calculator */
	for (id = 0; id < device_count; id++) {
		struct libusb_device_descriptor descriptor;

		/* get the device descriptor */
		if (libusb_get_device_descriptor(device_list[id], &descriptor))
			continue;

		/* check if is a CASIO Protocol 7.00 device */
		if (descriptor.idVendor  == 0x07cf
		 && descriptor.idProduct == 0x6101) {
			openmode |= CASIO_OPENMODE_READ | CASIO_OPENMODE_WRITE;
			calc = device_list[id];
			break;
		}

		/* check if is a CASIO SCSI device */
		if (descriptor.idVendor  == 0x07cf
		 && descriptor.idProduct == 0x6102) {
			openmode |= CASIO_OPENMODE_SCSI;
			calc = device_list[id];
			break;
		}
	}

	/* the calculator wasn't found */
	if (!calc) {
		libusb_free_device_list(device_list, 1);
		err = casio_error_nocalc;
		goto fail;
	}

	/* get calculator handle */
	msg((ll_info, "getting the device handle"));
	uerr = libusb_open(calc, &dhandle);
	libusb_free_device_list(device_list, 1);

	/* check if we have the handle */
	switch (uerr) {
	/* couldn't get access to the device */
	case LIBUSB_ERROR_ACCESS:
		err = casio_error_noaccess;
		goto fail;

	/* default cases */
	case 0: break;
	default:
		msg((ll_fatal, "libusb_open returned %d: %s",
			uerr, libusb_error_name(uerr)));
		err = casio_error_noaccess;
		goto fail;
	}

	/* disconnect any kernel driver */
	msg((ll_info, "Detaching kernel driver, if any."));
	uerr = libusb_detach_kernel_driver(dhandle, 0);
	switch (uerr) {
		/* cases where it's okay */
		case 0: case LIBUSB_ERROR_NOT_SUPPORTED:
		case LIBUSB_ERROR_NOT_FOUND: break;

		/* cases where it's not okay */
		case LIBUSB_ERROR_INVALID_PARAM:
			msg((ll_fatal, "Interface 0 doesn't exist...?"));
			err = casio_error_nocalc;
			goto fail;
		case LIBUSB_ERROR_NO_DEVICE:
			err = casio_error_nocalc;
			goto fail;
		default:
			msg((ll_fatal, "libusb returned %d: %s",
				uerr, libusb_error_name(uerr)));
			goto fail;
	}

	/* check if the interface is active */
	libusb_kernel_driver_active(dhandle, 0);

	/* claim the interface */
	msg((ll_info, "Claiming the interface."));
	uerr = libusb_claim_interface(dhandle, 0);
	switch (uerr) {
		/* cases where it's okay (not a lot) */
		case 0: break;

		/* cases where it's not okay */
		case LIBUSB_ERROR_NO_DEVICE:
		case LIBUSB_ERROR_NOT_FOUND:
			err = casio_error_nocalc;
			goto fail;
		case LIBUSB_ERROR_BUSY:
			msg((ll_info,
				"Another program/driver has claimed the interface..."));
			err = casio_error_noaccess;
			goto fail;
		default:
			msg((ll_info, "libusb returned %d: %s",
				uerr, libusb_error_name(uerr)));
			goto fail;
	}

	/* make the cookie */
	cookie = casio_alloc(1, sizeof(cookie_libusb_t));
	err = casio_error_alloc;
	if (!cookie) goto fail;
	cookie->_context = context;
	cookie->_handle = dhandle;
	cookie->_start = 0;
	cookie->_end = -1;

	/* final call. */
	return (casio_open_stream(stream, openmode,
		cookie, &casio_libusb_callbacks, 0));
fail:
	if (cookie)  casio_free(cookie);
	if (dhandle) libusb_close(dhandle);
	if (context) libusb_exit(context);
	return (err);
}

#endif
