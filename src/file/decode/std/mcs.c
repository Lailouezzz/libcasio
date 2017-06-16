/* ****************************************************************************
 * file/decode/std/mcs.c -- decode an MCS archive file.
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
 *	casio_decode_std_mcs:
 *	Decode an MCS file, after the Standard Header.
 *
 *	@arg	h			the handle to make.
 *	@arg	buffer		the buffer to read from.
 *	@arg	num			number of sizes.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_decode_std_mcs(casio_file_t **h,
	casio_stream_t *buffer, casio_standard_header_t *std)
{
	int err = casio_error_alloc;
	/* get number of subparts from the standard header */
	unsigned int num = std->casio_standard_header_number;
	unsigned long i;
	casio_mcshead_t head; casio_file_t *handle;
	casio_mcsfile_t *mcsfile;

	/* make the handle */
	err = casio_make_mcs(h, num);
	if (err) return (err);
	handle = *h;

	/* read all of the parts */
	msg((ll_info, "%" PRIuFAST16 " total mcs files to browse", num));
	while (num) {
		casio_mcs_subheader_t hd;

		/* get the subheader */
		GDREAD(hd)

		/* correct endianess */
		hd.casio_mcs_subheader_subcount =
			be32toh(hd.casio_mcs_subheader_subcount);

		/* log info about part */
		msg((ll_info, "New group! Group name is '%.16s'.",
			hd.casio_mcs_subheader_intname));
		msg((ll_info, "%d mcs files to browse in that group",
			hd.casio_mcs_subheader_subcount));

		/* foreach subpart */
		for (i = 0; i < hd.casio_mcs_subheader_subcount; i++) {
			casio_mcs_fileheader_t fhd;
			casio_uint32_t datalength;

			/* get the part header */
			GDREAD(fhd)
			datalength = be32toh(fhd.casio_mcs_fileheader_datalength);

			/* log info about the subpart */
			msg((ll_info, "[%lu] directory name is '%.8s'",
				i, fhd.casio_mcs_fileheader_dirname));
			msg((ll_info, "[%lu] filename is '%.8s'", i,
				fhd.casio_mcs_fileheader_filename));
			msg((ll_info, "[%lu] data length is %" CASIO_PRIu32,
				i, datalength));

			/* decode the head */
			casio_decode_mcsfile_head(&head,
				fhd.casio_mcs_fileheader_filetype,
				hd.casio_mcs_subheader_intname,
				fhd.casio_mcs_fileheader_dirname,
				fhd.casio_mcs_fileheader_filename,
				datalength);

			/* decode */
			mcsfile = NULL;
			err = casio_decode_mcsfile(&mcsfile, &head, buffer);
			if (err) goto fail;
			err = casio_put_mcsfile(handle->casio_file_mcs, mcsfile, 1);
			if (err) goto fail;
		}
	}

	/* no error */
	return (0);

	/* was error! */
fail:
	casio_free_file(*h); *h = NULL;
	return (err);
}
