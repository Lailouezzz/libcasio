/* ****************************************************************************
 * link/seven_fs/delete.c -- delete an element from a Protocol 7.00 filesystem.
 * Copyright (C) 2019 Alan "Lailouezzz" Le Bouder <alanlebouder@gmail.com>
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
 *	casio_sevenfs_list:
 *	List all files/directories from a Protocol 7.00 filesystem.
 *
 *	@arg	cookie		the cookie.
 *	@arg	path		the path.
 *  @arg    callback    the callback list function.
 *  @arg    cbcookie    the callback cookie.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_sevenfs_list(sevenfs_cookie_t *cookie, sevenfs_path_t *path,
		casio_fs_list_func_t *callback, void *cbcookie)
{
    int err; casio_link_t *handle = cookie;
	const char *dev;

	/* Make the vars. */
	dev = &path->sevenfs_path_data[path->sevenfs_path_dev];

    /* send command packet */
    msg((ll_info, "Sending the list command"));
    if((err = casio_seven_send_cmdfls_reqallinfo(handle, dev))) {
        msg((ll_fatal, "Couldn't send file transfer request/didn't receive answer"));
        return (err);
    } else if (response.casio_seven_packet_type == casio_seven_type_nak
     && response.casio_seven_packet_code == casio_seven_err_other) {
        msg((ll_fatal, "Invalid filesystem"));
        return (casio_error_device);
    } else if (response.casio_seven_packet_type != casio_seven_type_ack) {
        msg((ll_fatal, "Didn't receive ack or known error..."));
        return (casio_error_unknown);
    }

    /* swap roles */
    msg((ll_info, "Sending roleswap"));
    if((err = casio_seven_send_roleswp(handle))) {
        msg((ll_fatal, "Couldn't swap roles"));
        return (err);
    }

    /* - Note: we are now in passive mode - */
    while(1) {
        /* check answer */
        char *dir, *filename, *cdev; unsigned int fs, ldir, lfilename;
        casio_pathnode_t *fnode = NULL;
        casio_stat_t fstat = { 0 };
        switch (response.casio_seven_packet_type)
        {
        /* - If is roleswap, we have finished our job here - */
        case casio_seven_type_swp:
            return (0);
            break;

        /* - If is command, should be another file info - */
        case casio_seven_type_cmd:
            /* Check args */
            dir = response.casio_seven_packet_args[0];
            filename = response.casio_seven_packet_args[1];
            cdev = response.casio_seven_packet_args[4];
            fs = response.casio_seven_packet_filesize;

            /* Device root should not be sent */
            if(!dir && !filename) continue;

            /* Create node and stat */
            ldir = dir ? strlen(dir) : 0;
            lfilename = filename ? strlen(filename) : 0;

            if(dir && filename) {
                /* Create node size 2 */
                casio_make_pathnode(&fnode, 2);
                casio_make_pathnode(&fnode->casio_pathnode_next, 1);

                /* Dir node */
                memcpy(&fnode->casio_pathnode_name, dir, ldir);
                fnode->casio_pathnode_name[ldir] = '\0';
                
                /* File node */
                memcpy(&fnode->casio_pathnode_next->casio_pathnode_name, filename, lfilename);
                fnode->casio_pathnode_next->casio_pathnode_name[lfilename] = '\0';
            } else if(filename && !dir) {
                /* Create node size 1 */
                casio_make_pathnode(&fnode, 1);

                /* File node */
                memcpy(&fnode->casio_pathnode_name, filename, lfilename);
                fnode->casio_pathnode_name[lfilename] = '\0';
            } else if(dir && !filename) {
                /* Create node size 1 */
                casio_make_pathnode(&fnode, 1);

                /* Dir node */
                memcpy(&fnode->casio_pathnode_name, dir, ldir);
                fnode->casio_pathnode_name[ldir] = '\0';
            }

            fstat.casio_stat_size = fs;
            fstat.casio_stat_type = (dir && !filename) ? CASIO_STAT_TYPE_DIR : CASIO_STAT_TYPE_REG;

            /* Call callback and free node */
            (*callback)(cbcookie, fnode, &fstat);
            if(dir && filename) {
                casio_free_pathnode(fnode->casio_pathnode_next);
                fnode->casio_pathnode_next = NULL;
            }
            casio_free_pathnode(fnode);
            fnode = NULL;
            break;

        default:
            /* Unknown */
            msg((ll_fatal, "Error packet type unknown"));
            return (casio_error_unknown);
            break;
        }

        /* send ack to continue */
        msg((ll_info, "Sending ack to continue"));
        if ((err = casio_seven_send_ack(handle, 1))) {
            msg((ll_fatal, "Unable to send ack/receive answer!"));
            return (err);
        }
    }
    return (0);
}