/* ****************************************************************************
 * link/seven_fs/getfreemem.c -- get the available free space on a 7.00 fs.
 * Copyright (C) 2020 Alan "Lailouezzz" Le Bouder <alanlebouder@gmail.com>
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
#include "seven_fs.h"

/**
 *	casio_sevenfs_getfreemem:
 *	Get the available free space on a Protocol 7.00 filesystem.
 *
 *	@arg	cookie		the cookie.
 *	@arg	path		the path.
 *  @arg    capacity    the pointer to capacity
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_sevenfs_getfreemem(sevenfs_cookie_t *cookie,
                                          sevenfs_path_t *path,
		                                  size_t *capacity)
{
    int err;
    casio_link_t *handle = cookie;
	const char *devname = (path->sevenfs_path_dev != 0xFF) ? 
                          &path->sevenfs_path_data[path->sevenfs_path_dev] : 
                           NULL;

    /* Some checks */
    chk_handle(handle);
    chk_active(handle);
    if (devname == NULL)
        return (casio_error_op);

    /* Send command packet */
    if ((err = casio_seven_send_cmdfls_reqcapacity(handle, devname))) {
        msg((ll_fatal, "Couldn't send request/didn't receive answer"));
        return (casio_error_unknown);
    }

    /* Check response packet */
    if (response.casio_seven_packet_type == casio_seven_type_nak
     && response.casio_seven_packet_code == casio_seven_err_other)  {
        msg((ll_fatal, "File system probably doesn't exist"));
        return (casio_error_device);
    } else if (response.casio_seven_packet_type != casio_seven_type_ack) {
        msg((ll_fatal, "Didn't receive ack ro known error..."));
        return (casio_error_unknown);
    }

    /* Swap roles */
    msg((ll_info, "Sending roleswap"));
    if ((err = casio_seven_send_roleswp(handle))) {
        msg((ll_fatal, "Couldn't swap roles"));
        return (err);
    } else if (response.casio_seven_packet_type != casio_seven_type_cmd
            || response.casio_seven_packet_code != 0x4C
            || !response.casio_seven_packet_args[4]) {
        msg((ll_fatal, "Didn't receive expected comand"));
        return (casio_error_unknown);
    }

    /* Decode thingy */
    *capacity = response.casio_seven_packet_filesize;

    /* ack and check for the roleswap */
    msg((ll_info, "Return ack and wait for roleswp"));
    if ((err = casio_seven_send_ack(handle, 1))) {
        msg((ll_fatal, "Unable to send ack"));
        return (err);
    } else if (response.casio_seven_packet_type != casio_seven_type_swp) {
        return (casio_error_unknown);
    }

    /* No error */
    return (0);
}