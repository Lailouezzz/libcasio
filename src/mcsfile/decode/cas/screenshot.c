/* ****************************************************************************
 * mcsfile/decode/cas/screenshot.c -- decode a CAS screenshot.
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
#include "../decode.h"

/**
 *	casio_decode_caspart_capture:
 *	Decode a CAS picture.
 *
 *	@arg	handle		the handle to contribute to.
 *	@arg	buffer		the buffer to read from.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_decode_caspart_capture(casio_mcsfile_t *handle,
	casio_stream_t *buffer)
{
	int err;
	unsigned int width, height;
	size_t pic_size; unsigned char *pic_data = NULL;

	/* read the picture data */
	width = handle->casio_mcsfile_head.casio_mcshead_width;
	height = handle->casio_mcsfile_head.casio_mcshead_height;
	pic_size = casio_get_picture_size(NULL,
		casio_pictureformat_4bit_color, width, height);

	/* read the picture size */
	err = casio_error_alloc;
	pic_data = casio_alloc(pic_size, 1);
	if (!pic_data) return (err);
	GREAD(pic_data, pic_size)

	/* decode the picture data */
	err = casio_decode_picture(handle->casio_mcsfile_pics[0],
		pic_data, handle->casio_mcsfile_head.casio_mcshead_picformat,
		width, height);
	if (err) goto fail;

	/* no error */
	handle->casio_mcsfile_head.casio_mcshead_flags &=
		~casio_mcsflag_unfinished;
	err = 0;
fail:
	if (err) casio_free_mcsfile(handle);
	casio_free(pic_data);
	return (0);
}
