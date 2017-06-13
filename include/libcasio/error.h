/* ****************************************************************************
 * libcasio/error.h -- libcasio errors.
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
#ifndef  LIBCASIO_ERROR_H
# define LIBCASIO_ERROR_H
# include "cdefs.h"
CASIO_BEGIN_NAMESPACE

/* ************************************************************************* */
/*  Error codes                                                              */
/* ************************************************************************* */
/* Warning: whenever you add/remove errors, think about updating
 * core/strerror! */

typedef enum casio_error_e {
	casio_error_none     = 0x00,
	casio_error_success  = 0x00,
	casio_noerror        = 0x00,

/* Miscallaneous errors. */

	casio_error_unknown  = 0x01, /* an unknown error has occured. */
	casio_error_alloc    = 0x02, /* a memory allocation has failed. */
	casio_error_op       = 0x03, /* an operation is not supported. */
	casio_error_arg      = 0x04, /* an argument was invalid. */
	casio_error_invalid  = 0x04, /* (alias) */

/* Stream errors. */

	casio_error_stream   = 0x10, /* the stream is invalid. */
	casio_error_nostream = 0x10, /* (alias) */
	casio_error_nocalc   = 0x10, /* (alias) */
	casio_error_read     = 0x11, /* a read operation on the stream's failed */
	casio_error_noread   = 0x11, /* (alias) */
	casio_error_write    = 0x12, /* a write operation on the stream's failed */
	casio_error_nowrite  = 0x12, /* (alias) */
	casio_error_seek     = 0x13, /* a seek operation on the stream's failed */
	casio_error_noseek   = 0x13, /* (alias) */
	casio_error_timeout  = 0x14, /* a timeout has occured. */
	casio_error_access   = 0x15, /* could not access the device (perms) */
	casio_error_noaccess = 0x15, /* (alias) */
	casio_error_eof      = 0x16, /* end of file */

/* Link errors. */

	casio_error_init     = 0x30, /* uninitialized link. */
	casio_error_shift    = 0x31, /* double shift. */
	casio_error_damned   = 0x32, /* irrecoverable link error */
	casio_error_int      = 0x33, /* interrupted by the user */
	casio_error_stopped  = 0x33, /* (alias) */
	casio_error_active   = 0x34, /* should have been in active/passive mode */
	casio_error_passive  = 0x34, /* (alias) */
	casio_error_noow     = 0x35, /* denied overwrite */
	casio_error_command  = 0x36, /* unsupported command */

/* Filesystem errors. */

	casio_error_device   = 0x50, /* unsupported device */
	casio_error_fullmem  = 0x51, /* full memory */
	casio_error_notfound = 0x52, /* entry not found */
	casio_error_empty    = 0x53, /* empty file not allowed */

/* Decoding errors. */

	casio_error_magic    = 0x70, /* corrupted or unknown file format. */
	casio_error_csum     = 0x71, /* invalid checksum. */
	casio_error_checksum = 0x71, /* (alias) */
	casio_error_wrong    = 0x72  /* was not one of the allowed file types. */
} casio_error_t;

/* Get a string describing the error. */
CASIO_EXTERN const char* CASIO_EXPORT casio_error_strings[];
CASIO_EXTERN const char* CASIO_EXPORT casio_strerror
	OF((int casio__error));

# ifndef LIBCASIO_NO_STRERROR
#  define casio_strerror(CASIO_N) casio_error_strings[(CASIO_N)]
# endif

CASIO_END_NAMESPACE
#endif /* LIBCASIO_ERROR_H */
