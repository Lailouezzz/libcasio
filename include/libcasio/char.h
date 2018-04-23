/* ****************************************************************************
 * libcasio/char.h -- libcasio character encoding.
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
#ifndef  LIBCASIO_CHAR_H
# define LIBCASIO_CHAR_H
# include "fontchar.h"

/* These character encoding conversion utilities are basic and should be
 * as portable as libcasio. Notice that I haven't used the `wchar_t` for
 * UTF-32/UCS-4 character tabs as the `wchar_t` is actually 16-bits on
 * some commonly used systems (e.g. Microsoft Windows).
 *
 * Each of these utilities update their arguments, and return:
 * - `casio_noerror` (0) if the conversion has succeeded;
 * - `casio_error_seq` if an invalid sequence has been found in the source;
 * - `casio_error_eof` if an incomplete sequence has been found in the source;
 * - `casio_error_write` if we ran out of space in the output buffer.
 *
 * For example, to convert FONTCHAR-8 to UTF-32:
 *
 *	casio_uint8_t source[] = "\x7F\x29Hello";
 *	casio_uint32_t dest[10];
 *	size_t sourcelen = 7, destlen = 10;
 *	casio_uint8_t *sourceptr = source, *destptr = dest;
 *	int status;
 *
 *	status = casio_fontchar8_to_utf32(&sourceptr, &sourcelen, &destptr,
 *		&destlen);
 *	switch (status) {
 *	case casio_noerror:
 *		break;
 *	case casio_error_seq:
 *		fprintf(stderr, "An invalid character sequence has been found!\n");
 *		break;
 *	case casio_error_eof:
 *		fprintf(stderr, "Incomplete source! Feed me paper!\n");
 *		break;
 *	case casio_error_write:
 *		fprintf(stderr, "Make the destination buffer bigger!\n");
 *		break;
 *	default:
 *		fprintf(stderr, "Unknown error...?\n");
 *		break;
 *	}
 *
 * By the way, FONTCHAR-8 is the FONTCHARACTER encoding with variable
 * character length (1 to 2 bytes) and FONTCHAR-16 is the FONTCHARACTER
 * encoding with fixed-width big-endian 16-bit characters. */

CASIO_EXTERN int CASIO_EXPORT casio_fontchar8_to_utf32
	OF((casio_uint8_t **casio__inbuf, size_t *casio__inleft,
		casio_uint32_t **casio__outbuf, size_t *casio__outleft));

CASIO_EXTERN int CASIO_EXPORT casio_fontchar16_to_utf32
	OF((FONTCHARACTER **casio__inbuf, size_t *casio__inleft,
		casio_uint32_t **casio__outbuf, size_t *casio__outleft));

CASIO_EXTERN int CASIO_EXPORT casio_utf32_to_fontchar8
	OF((casio_uint32_t **casio__inbuf, size_t *casio__inleft,
		casio_uint8_t **casio__outbuf, size_t *casio__outleft));

CASIO_EXTERN int CASIO_EXPORT casio_utf32_to_fontchar16
	OF((casio_uint32_t **casio__inbuf, size_t *casio__inleft,
		FONTCHARACTER **casio__outbuf, size_t *casio__outleft));

#endif /* LIBCASIO_CHAR_H */
