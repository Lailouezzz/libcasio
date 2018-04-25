/* ****************************************************************************
 * mcsfile/decode/mcs/picture.c -- decode an MCS picture.
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
 *	casio_decode_mcs_capture:
 *	Decode a capture.
 *
 *	@arg	h			the handle to make.
 *	@arg	buffer		the buffer to read from.
 *	@arg	head		the pre-filled head to complete and use.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_decode_mcs_capture(casio_mcsfile_t **h,
	casio_stream_t *buffer, casio_mcshead_t *head)
{
	int err;
	casio_mcs_captureheader_t hd;
	unsigned char *pic_raw = NULL; size_t pic_size;
	casio_mcsfile_t *handle;

	/* read header */
	DREAD(hd)

	/* make final head and file */
	head->casio_mcshead_count  = 1;
	head->casio_mcshead_width  = be16toh(hd.casio_mcs_captureheader_width);
	head->casio_mcshead_height = be16toh(hd.casio_mcs_captureheader_height);
	err = casio_make_mcsfile(h, head);
	if (err) return (err);
	handle = *h;

	/* print info */
	msg((ll_info, "capture is %dx%d sized",
		handle->casio_mcsfile_head.casio_mcshead_width,
		handle->casio_mcsfile_head.casio_mcshead_height));

	/* read raw data */
	pic_size = casio_get_picture_size(NULL, casio_pictureformat_1bit_packed,
		head->casio_mcshead_width, head->casio_mcshead_height);
	err = casio_error_alloc;
	if (!(pic_raw = casio_alloc(pic_size, 1))) goto fail;
	READ(pic_raw, pic_size)

	/* get the image and return */
	if ((err = casio_decode_picture(handle->casio_mcsfile_pics[0],
	  pic_raw, casio_pictureformat_1bit_packed,
	  head->casio_mcshead_width, head->casio_mcshead_height)))
		goto fail;

	/* no error! */
	casio_free(pic_raw);
	return (0);
fail:
	casio_free(pic_raw);
	casio_free_mcsfile(*h);
	*h = NULL;
	return (err);
}

/**
 *	casio_decode_mcs_picture:
 *	Decode a picture.
 *
 *	@arg	h			the handle to make.
 *	@arg	buffer		the buffer to read from.
 *	@arg	head		the pre-filled head to complete and use.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_decode_mcs_picture(casio_mcsfile_t **h,
	casio_stream_t *buffer, casio_mcshead_t *head)
{
	int err;
	casio_mcsfile_t *handle;
	unsigned char *pics_raw = NULL; size_t pic_size;

	/* make final head */
	msg((ll_info, "Size is 0x%04X", head->casio_mcshead_size));
	head->casio_mcshead_count = head->casio_mcshead_size / 1024;
	head->casio_mcshead_width = 128;
	head->casio_mcshead_height = 64;
	err = casio_make_mcsfile(h, head);
	if (err) return (err);
	handle = *h;

	/* get the images */
	pic_size = casio_get_picture_size(NULL, casio_pictureformat_1bit_packed,
		head->casio_mcshead_width, head->casio_mcshead_height);
	err = casio_error_alloc;
	if (!(pics_raw = casio_alloc(pic_size * 2, 1))) goto fail;
	READ(pics_raw, pic_size * 2)

	/* decode the images */
	if ((err = casio_decode_picture(handle->casio_mcsfile_pics[0],
	  pics_raw, casio_pictureformat_1bit_packed,
	  head->casio_mcshead_width, head->casio_mcshead_height))
	 || (err = casio_decode_picture(handle->casio_mcsfile_pics[1],
	  &pics_raw[pic_size], casio_pictureformat_1bit_packed,
	  head->casio_mcshead_width, head->casio_mcshead_height)))
		goto fail;

	/* no error */
	casio_free(pics_raw);
	return (0);
fail:
	casio_free_mcsfile(*h); *h = NULL;
	casio_free(pics_raw);
	return (0);
}
