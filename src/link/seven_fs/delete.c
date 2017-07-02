/* ****************************************************************************
 * link/seven_fs/delete.c -- delete an element from a Protocol 7.00 filesystem.
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

/**
 *	casio_sevenfs_delete:
 *	Delete a file from a Protocol 7.00 filesystem.
 *
 *	@arg	cookie		the cookie.
 *	@arg	path		the path.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_sevenfs_delete(sevenfs_cookie_t *cookie,
	sevenfs_path_t *path)
{
	int err; casio_link_t *handle = cookie;
	const char *dir, *file, *dev;

	/* Make the vars. */
	dir = path->sevenfs_path_dir == 0xFF ? NULL
		: &path->sevenfs_path_data[path->sevenfs_path_dir];
	file = &path->sevenfs_path_data[path->sevenfs_path_file];
	dev = &path->sevenfs_path_data[path->sevenfs_path_dev];

	msg((ll_info, "Sending the deletion command."));
	err = casio_seven_send_cmdfls_delfile(handle, dir, file, dev);
	if (err) return (err);
	if (response.casio_seven_packet_type == casio_seven_type_nak)
		return (casio_error_notfound);
	if (response.casio_seven_packet_type != casio_seven_type_ack)
		return (casio_error_unknown);

	return (0);
}
