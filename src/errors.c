/* ****************************************************************************
 * errors.c -- error strings.
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
#define LIBCASIO_NO_STRERROR
#include "internals.h"

const char* CASIO_EXPORT casio_error_strings[128] = {
/* Miscallaneous errors. */

	"no error has occured",
	"an unknown error has occured",
	"a memory allocation has failed",
	"this operation is unsupported",
	"an argument was invalid",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

/* Stream errors. */

	"the stream is invalid",
	"a read operation on the stream has failed",
	"a write operation on the stream has failed",
	"a seek operation on the stream has failed",
	"a timeout has occurred",
	"could not get access to the device",
	"an end of file event has occured",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL,

/* Link errors. */

	"the link was uninitialized",
	"illegal double shifting",
	"irrecoverable link error",
	"interrupted by user",
	"not in active/passive mode",
	"denied overwrite",
	"unsupported command",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL,

/* Filesystem errors. */

	"unsupported storage device",
	"full memory",
	"file not found",
	"empty files aren't allowed",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL,

/* Decoding errors. */

	"corrupted or unknown file format",
	"invalid checksum",
	"was recognized but not one of the allowed file types."
};

/**
 *	casio_strerror:
 *	Get the error string.
 *
 *	@arg	error	the error code.
 *	@return			the error string.
 */

const char* CASIO_EXPORT casio_strerror(int error)
{
	return (casio_error_strings[error]);
}
