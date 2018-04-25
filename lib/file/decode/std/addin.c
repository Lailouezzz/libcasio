/* ****************************************************************************
 * file/decode/std/addin.c -- decode an add-in file.
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

/* ************************************************************************* */
/*  Add-in for the fx-9860G (G1A)                                            */
/* ************************************************************************* */
/**
 *	casio_decode_std_addin:
 *	Decodes a "normal" add-in (after Standard Header).
 *
 *	@arg	h			the file handle to make.
 *	@arg	buffer		the buffer to read from.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_decode_std_addin(casio_file_t **h,
	casio_stream_t *buffer, casio_standard_header_t *std)
{
	int err = 0;
	casio_addin_subheader_t hd; size_t content_size;
	casio_version_t version; time_t created;
	casio_file_t *handle;
	(void)std;

	/* get the subheader */
	DREAD(hd)

	/* make the handle */
	casio_decode_version(&version,
		(char*)hd.casio_addin_subheader_version);
	casio_decode_date(&created,
		(char*)hd.casio_addin_subheader_creation_date);
	content_size =
		be32toh(hd.casio_addin_subheader_filesize); /* already corrected */
	content_size -= sizeof(casio_standard_header_t)
		+ sizeof(casio_addin_subheader_t);
	err = casio_make_addin(h, casio_filefor_fx, content_size,
		(char*)hd.casio_addin_subheader_title,
		(char*)hd.casio_addin_subheader_internal_name, &version, &created);
	if (err) return (err);
	handle = *h;

	/* log info about the subheader */
	msg((ll_info, "title is '%s'", handle->casio_file_title));
	msg((ll_info, "internal name is '%s'", handle->casio_file_intname));
	msg((ll_info, "estrips count is %" CASIO_PRIu8,
		hd.casio_addin_subheader_estrips_count));
	msg((ll_info, "version is %02u.%02u",
		handle->casio_file_version.casio_version_major,
		handle->casio_file_version.casio_version_minor));
	msg((ll_info, "creation date is: %.24s",
		ctime(&handle->casio_file_creation_date)));

	/* fill icon */
	casio_decode_picture(handle->casio_file_icon_unsel,
		hd.casio_addin_subheader_icon,casio_pictureformat_1bit,
		handle->casio_file_width, handle->casio_file_height);
	casio_decode_picture(handle->casio_file_icon_sel,
		hd.casio_addin_subheader_icon, casio_pictureformat_1bit_r,
		handle->casio_file_width, handle->casio_file_height);

	/* read content */
	GREAD(handle->casio_file_content, handle->casio_file_size)

	/* no errors */
	return (err);
fail:
	casio_free_file(*h); *h = NULL;
	return (err);
}
/* ************************************************************************* */
/*  Add-in for the fx-CP/Classpad (C1A)                                      */
/* ************************************************************************* */
/**
 *	casio_decode_std_cp_addin:
 *	Decode fx-CP add-in.
 *
 *	@arg	h			the file handle to make.
 *	@arg	buffer		the buffer to read from.
 *	@arg	std			the standard header.
 *	@arg	sub			the standard subheader.
 *	@arg	cp			the classpad-specific subheader.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_decode_std_cp_addin(casio_file_t **h,
	casio_stream_t *buffer, casio_standard_header_t *std,
	casio_standard_subheader_t *sub, casio_standard_classpad_subheader_t *cp)
{
	int err = 0;
	casio_file_t *handle;
	casio_classpad_addin_subheader_t cphd; size_t content_size;
	casio_version_t version; time_t created;
	(void)std; (void)cp;

	/* read the add-in subheader */
	DREAD(cphd)

	/* make the handle */
	casio_decode_version(&version,
		(char*)sub->casio_standard_subheader_version);
	casio_decode_date(&created,
		(char*)sub->casio_standard_subheader_timestamp);
	content_size =
		be32toh(sub->casio_standard_subheader_filesize) - 0x1000;
	err = casio_make_addin(h, casio_filefor_cp, content_size,
		(char*)sub->casio_standard_subheader_title,
		(char*)sub->casio_standard_subheader_internal_name,
		&version, &created);
	if (err) return (err);
	handle = *h;

	/* decode pictures */
	casio_decode_picture(handle->casio_file_icon_unsel,
		cphd.casio_classpad_addin_subheader_icon,
		casio_pictureformat_1bit_packed,
		handle->casio_file_width, handle->casio_file_height);
	casio_decode_picture(handle->casio_file_icon_sel,
		cphd.casio_classpad_addin_subheader_icon,
		casio_pictureformat_1bit_packed_r,
		handle->casio_file_width, handle->casio_file_height);

	/* log */
	msg((ll_info, "title is '%s'", handle->casio_file_title));
	msg((ll_info, "internal name is '%s'", handle->casio_file_intname));
	msg((ll_info, "version is %02u.%02u",
		handle->casio_file_version.casio_version_major,
		handle->casio_file_version.casio_version_minor));
	msg((ll_info, "timestamp is %.24s",
		ctime(&handle->casio_file_creation_date)));

	/* get content */
	GREAD(handle->casio_file_content, handle->casio_file_size)

	/* no error */
	return (0);
fail:
	casio_free_file(*h); *h = NULL;
	return (err);
}
/* ************************************************************************* */
/*  Add-in for the fx-CG/Prizm (G3A)                                         */
/* ************************************************************************* */
/**
 *	casio_decode_std_cg_addin:
 *	Decode fx-CG add-in.
 *
 *	@arg	h			the file handle to make.
 *	@arg	buffer		the buffer to read from.
 *	@arg	std			the standard header.
 *	@arg	sub			the standard subheader.
 *	@arg	prizm		the prizm-specific subheader.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_decode_std_cg_addin(casio_file_t **h,
	casio_stream_t *buffer, casio_standard_header_t *std,
	casio_standard_subheader_t *sub, casio_standard_prizm_subheader_t *prizm)
{
	int err = 0;
	casio_prizm_addin_subheader_t cghd; size_t content_size;
	casio_version_t version; time_t created;
	casio_file_t *handle;
	(void)std; (void)prizm;

	/* read the add-in subheader */
	DREAD(cghd)

	/* make the handle */
	casio_decode_version(&version,
		(char*)sub->casio_standard_subheader_version);
	casio_decode_date(&created,
		(char*)sub->casio_standard_subheader_timestamp);
	content_size = be32toh(sub->casio_standard_subheader_filesize);
	content_size -= sizeof(casio_standard_header_t)
		+ sizeof(casio_standard_subheader_t)
		+ sizeof(casio_standard_prizm_subheader_t)
		+ sizeof(casio_prizm_addin_subheader_t)
		+ sizeof(casio_uint32_t);
	msg((ll_info, "Content size is %" CASIO_PRIuSIZE, content_size));
	err = casio_make_addin(h, casio_filefor_cg, content_size,
		(char*)sub->casio_standard_subheader_title,
		(char*)sub->casio_standard_subheader_internal_name,
		&version, &created);
	if (err) return (err);
	handle = *h;

	/* decode pictures */
	casio_decode_picture(handle->casio_file_icon_unsel,
		cghd.casio_prizm_addin_subheader_unselected_icon_image,
		casio_pictureformat_16bit,
		handle->casio_file_width, handle->casio_file_height);
	casio_decode_picture(handle->casio_file_icon_sel,
		cghd.casio_prizm_addin_subheader_selected_icon_image,
		casio_pictureformat_16bit,
		handle->casio_file_width, handle->casio_file_height);

	/* log */
	msg((ll_info, "title is '%s'", handle->casio_file_title));
	msg((ll_info, "internal name is '%s'", handle->casio_file_intname));
	msg((ll_info, "version is %02u.%02u",
		handle->casio_file_version.casio_version_major,
		handle->casio_file_version.casio_version_minor));
	msg((ll_info, "timestamp is %.24s",
		ctime(&handle->casio_file_creation_date)));

	/* read content */
	GREAD(handle->casio_file_content, handle->casio_file_size)

	/* no error */
	return (0);
fail:
	casio_free_file(*h); *h = NULL;
	return (err);
}
