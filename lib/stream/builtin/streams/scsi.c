/* ****************************************************************************
 * stream/builtin/streams/scsi.c -- make an SCSI request on a STREAMS device.
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
#include "streams.h"
#if 0 && !defined(LIBCASIO_DISABLED_STREAMS)
# include <scsi/sg.h>

/**
 *	casio_streams_scsi_request:
 *	Make an SCSI request using the SG interface.
 *
 *	@arg	vcookie		the cookie (uncasted).
 *	@arg	request		the SCSI request.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_streams_scsi_request(void *vcookie,
	casio_scsi_t *request)
{
	streams_cookie_t *cookie = (streams_cookie_t*)vcookie;

	/* correct the request */
	int err = casio_scsi_correct(request);
	if (err) return (err);

	/* make an `sg_io_hdr_t` out of the request */
	sg_io_hdr_t h = {
		/* main input data */
		.interface_id = 'S', /* magic */
		.flags = 0, /* no special things, just normal SCSI */
		.dxfer_direction = request->direction, /* FIXME: macro to macro? */
		.cmd_len = request->cmd_len, .cmdp = request->cmd, /* command */
		.timeout = 0, /* FIXME (also, in ms) */

		/* data buffer */
		.iovec_count = 0,
		.dxfer_len = request->data_len,
		.dxferp = request->data, /* data buffer */

		/* sense buffer */
		.max_sb_len = request->slen, .sbp = request->sense, /* sense buffer */
	};

	/* make the request */
	if (ioctl(cookie->_writefd, SG_IO, &h) < 0) switch (errno) {
	case EACCES:
		log_error("Root perms required!");
	default:
		log_error("Errno: %s (0x%04X)", strerror(errno), errno);
		return (casio_error_unknown);
	}

	/* everything went well */
	return (0);
}

#endif
