/* ****************************************************************************
 * link/seven_fs/open.c -- open a Protocol 7.00 filesystem.
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
 * ************************************************************************* */
#include "seven_fs.h"

/* Callbacks. */

CASIO_LOCAL casio_fsfuncs_t sevenfs_callbacks = {
	NULL,
	(casio_fs_makepath_t*)&casio_make_sevenfs_path,
	(casio_fs_freepath_t*)&casio_free_sevenfs_path,
	NULL,
	NULL,
	(casio_fs_del_t*)&casio_sevenfs_delete,
	NULL,
	(casio_fs_list_t*)&casio_sevenfs_list,
	(casio_fs_open_t*)&casio_sevenfs_open,
	(casio_fs_optim_t*)&casio_sevenfs_optimize
};

/**
 *	casio_open_seven_fs:
 *	Open a Protocol 7.00 filesystem.
 *
 *	@arg	fs		the filesystem to open.
 *	@arg	link	the link above which to access the file system.
 *	@return			the error code (0 if ok).
 */

int CASIO_EXPORT casio_open_seven_fs(casio_fs_t **fs,
	casio_link_t *link)
{
	/* Open the filesystem. (no need for cookie allocating for now) */
	return (casio_open_fs(fs, link, &sevenfs_callbacks));
}

/**
 *	casio_sevenfs_open_read
 *	Open a file in read mode
 *
 *	@arg	cookie		the sevenfs cookie
 *	@arg	path		the path to th file
 *	@arg	stream		pointer to a pointer who contain the stream
 */

CASIO_LOCAL int casio_sevenfs_open_read(sevenfs_cookie_t *cookie, sevenfs_path_t *path,
		casio_stream_t **stream)
{
	casio_link_t *handle = cookie; int err;
	char *dirname = (path->sevenfs_path_dir != 0xFF) ? &path->sevenfs_path_data[path->sevenfs_path_dir] : NULL;
	char *filename = &path->sevenfs_path_data[path->sevenfs_path_file];
	char *devname = &path->sevenfs_path_data[path->sevenfs_path_dev];

	/* Make checks */
	chk_required_filename(filename);
	chk_dirname(dirname);
	chk_handle(handle);
	chk_active(handle);


	/* Send command packet */
	msg((ll_info, "Sending file transfer request"));
	if ((err = casio_seven_send_cmdfls_reqfile(handle, dirname, filename, devname))) {
		msg((ll_fatal, "Couldn't send file transfer request/didn't receive answer"));
		return (err);
	}

	/* Check response packet */
	if (response.casio_seven_packet_type == casio_seven_type_nak
	 && response.casio_seven_packet_code == casio_seven_err_other) {
		msg((ll_fatal, "Requested file doesn't exist"));
		err = casio_error_notfound;
		return (err);
	} else if (response.casio_seven_packet_type != casio_seven_type_ack) {
		msg((ll_fatal, "Didn't receive ack or known error..."));
		return (casio_error_unknown);
	}

	/* Send swap roles */
	msg((ll_info, "Sending roleswap"));
	if ((err = casio_seven_send_roleswp(handle)))
	{
		msg((ll_fatal, "Couldn't swap roles"));
		return (err);
	} else if (response.casio_seven_packet_type != casio_seven_type_cmd
			|| response.casio_seven_packet_code != 0x45)
	{
		msg((ll_fatal, "Didn't receive the excepted command"));
		return (casio_error_unknown);
	}
	
	return casio_seven_open_data_stream(stream, handle, response.casio_seven_packet_filesize, NULL, NULL);
}

/**
 *	casio_sevenfs_open_write
 *	Open a file in write mode
 *
 *	@arg	cookie		the sevenfs cookie
 *	@arg	path		the path to th file
 *	@arg	size		size of the file
 *	@arg	stream		pointer to a pointer who contain the stream
 *	@return				the error (0 if okey)
 */

CASIO_LOCAL int casio_sevenfs_open_write(sevenfs_cookie_t *cookie, sevenfs_path_t *path,
		casio_off_t size, casio_stream_t **stream)
{
	casio_link_t *handle = cookie; int err;
	char *dirname = &path->sevenfs_path_data[path->sevenfs_path_dir];
	char *filename = &path->sevenfs_path_data[path->sevenfs_path_file];
	char *devname = &path->sevenfs_path_data[path->sevenfs_path_dev];

	/* Make checks */
	chk_required_filename(filename);
	chk_dirname(dirname);
	chk_handle(handle);
	chk_active(handle);

	return (0);

}

/**
 *	casio_sevenfs_open:
 *	Open a file.
 *
 * 	@arg	cookie		the sevenfs cookie
 *	@arg	path		the path to the file
 *	@arg	size		size of the file if WRITE (NULL if READ)
 *	@arg	mode		the open mode
 *	@arg	stream		pointer to a pointer who contain the stream
 *	@return				the error (0 if okey)
 */

int CASIO_EXPORT casio_sevenfs_open(sevenfs_cookie_t *cookie, sevenfs_path_t *path,
		casio_off_t size, casio_openmode_t mode, casio_stream_t **stream)
{
	casio_link_t *handle = cookie; int err;
	char *dirname = &path->sevenfs_path_data[path->sevenfs_path_dir];
	char *filename = &path->sevenfs_path_data[path->sevenfs_path_file];
	char *devname = &path->sevenfs_path_data[path->sevenfs_path_dev];

	/* Make checks (not really required) */
	chk_required_filename(filename);
	chk_dirname(dirname);
	chk_handle(handle);
	chk_active(handle);

	if ((mode & CASIO_OPENMODE_WRITE))
	{

	} else if ((mode & CASIO_OPENMODE_READ))
	{
		return (casio_sevenfs_open_read(cookie, path, stream));
	}

#if 0
	/* Send command packet */
	msg((ll_info, "Sending file transfer request"));
	if ((err = casio_seven_send_cmdfls_reqfile(handle, dirname, filename, devname))) {
		msg((ll_fatal, "Couldn't send file transfer request/didn't receive answer"));
		return (err);
	}

	/* Check response */
	if (response.casio_seven_packet_type == casio_seven_type_nak
	 && response.casio_seven_packet_code == casio_seven_err_other)
	{
		/* If file doesn't exist and we want read : error */
		if ((mode & CASIO_OPENMODE_READ)) {
			msg((ll_fatal, "Requested file doesn't exist"));
			return (casio_error_notfound);
		}
	} else if (response.casio_seven_packet_type != casio_seven_type_ack) {
		msg((ll_fatal, "Didn't receive ack or known error..."));
		return (casio_error_unknown);
	}


	/* Check what happened if it is an error packet */
	if (response.casio_seven_packet_type == casio_seven_type_nak)
	{
		switch (response.casio_seven_packet_type)
		{

			/* Calc need overwrite confirmation */
		case casio_seven_err_overwrite:
			
			break;
		
			/* Calc denie overwrite or err other */
		case casio_seven_err_dont_overwrite:
		case casio_seven_err_other:
			msg((ll_error, "Overwrite impossible"));
			err = casio_error_device;
			return (err);
			break;

				/* Memory full */
		case casio_seven_err_fullmem:
			msg((ll_error, "Calc mem is full"));
			err = casio_error_fullmem;
			return (err);
			break;

				/* stfu warning */
		case casio_seven_err_resend: break;
		}
	}
	
	msg((ll_info, "WE CAN"));
#endif
}
