/* ****************************************************************************
 * libcasio/stream.h -- libcasio stream definition.
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
 *
 * The libcasio stream abstraction is there so that the core code can be more
 * platform-agnostic (althrough platform-specific helpers are built-in for
 * popular platforms like Microsoft Windows or GNU/Linux distributions).
 * A stream is basically what separates libcasio from the calculator.
 * When data is read from the stream, what is expected is what the calculator
 * has sent, and when data is written to the stream, it is what the calculator
 * shall receive.
 * ************************************************************************* */
#ifndef  LIBCASIO_STREAM_H
# define LIBCASIO_STREAM_H
# include "cdefs.h"
CASIO_BEGIN_NAMESPACE

/* forward structure declarations (don't mind) */
struct         casio_stream_s;
typedef struct casio_stream_s       casio_stream_t;
struct         casio_streamfuncs_s;
typedef struct casio_streamfuncs_s  casio_streamfuncs_t;
struct         casio_streamattrs_s;
typedef struct casio_streamattrs_s  casio_streamattrs_t;
struct         casio_timeouts_s;
typedef struct casio_timeouts_s     casio_timeouts_t;
struct         casio_scsi_s;
typedef struct casio_scsi_s         casio_scsi_t;
/* ************************************************************************* */
/*  Stream                                                                   */
/* ************************************************************************* */
/* The stream is a private structure that has more or less all of the features
 * that the libc FILE interface has, but has more and is cross-platform.
 * It is basically what libcasio uses to communicate with the calculator.
 *
 * Notice that in case of "partial success" (e.g. parts of the buffer were
 * written, but not all of it), you shall return an error, because there is
 * no partial success.
 *
 * Here are the stream types. The callbacks that will be taken in `casio_open`
 * is directly linked to the stream type you pass to it. */

typedef unsigned int casio_streamtype_t;
# define casio_streamtype_virtual 0x0000
# define casio_streamtype_serial  0x0001
# define casio_streamtype_scsi    0x0002
# define casio_streamtype_usb     0x0004

/* Here are the open modes: */

typedef enum casio_openmode_e {
	CASIO_OPENMODE_READ  = 0x0001, /* the stream is readable. */
	CASIO_OPENMODE_WRITE = 0x0002  /* the stream is writable. */
} casio_openmode_t;

/* Here is the offset type, to move within a stream: */

typedef long casio_off_t;
typedef int casio_whence_t;
# define CASIO_SEEK_SET 1
# define CASIO_SEEK_CUR 2
# define CASIO_SEEK_END 4

/* Here are the callback types: */

typedef int casio_stream_close_t    OF((void*));

typedef int casio_stream_setattrs_t OF((void*, const casio_streamattrs_t*));
typedef int casio_stream_settm_t    OF((void*, const casio_timeouts_t*));

typedef int casio_stream_read_t     OF((void*, unsigned char*, size_t));
typedef int casio_stream_write_t    OF((void*, const unsigned char*, size_t));
typedef int casio_stream_seek_t     OF((void*, casio_off_t*, casio_whence_t));

typedef int casio_stream_scsi_t     OF((void*, casio_scsi_t*));

/* Here is the callbacks structure: */

struct casio_streamfuncs_s {
	/* main callbacks */
	casio_stream_close_t    *casio_streamfuncs_close;

	/* settings callbacks */
	casio_stream_setattrs_t *casio_streamfuncs_setattrs;
	casio_stream_settm_t    *casio_streamfuncs_settm;

	/* serial callbacks */
	casio_stream_read_t     *casio_streamfuncs_read;
	casio_stream_write_t    *casio_streamfuncs_write;
	casio_stream_seek_t     *casio_streamfuncs_seek;

	/* SCSI callbacks */
	casio_stream_scsi_t     *casio_streamfuncs_scsi;
};

/* And here are some macros, for better API compatibility */

# define casio_stream_callbacks_for_serial(CASIO__CLOSE, \
	CASIO__SETCOMM, CASIO__SETTM, CASIO__READ, CASIO__WRITE) \
{CASIO__CLOSE, CASIO__SETCOMM, CASIO__SETTM, \
	CASIO__READ, CASIO__WRITE, NULL, NULL}

# define casio_stream_callbacks_for_virtual(CASIO__CLOSE, \
	CASIO__READ, CASIO__WRITE, CASIO__SEEK) \
{CASIO__CLOSE, NULL, NULL, CASIO__READ, \
	CASIO__WRITE, CASIO__SEEK, NULL}
/* ************************************************************************* */
/*  Stream settings values and flags                                         */
/* ************************************************************************* */
/* Here are the different baud speeds you can encounter, in bauds.
 * Note that one speed is not supported by all models. */

# define CASIO_B1200               1200 /* old models */
# define CASIO_B2400               2400 /* old models */
# define CASIO_B4800               4800 /* old models */
# define CASIO_B9600               9600 /* protocol seven default speed */
# define CASIO_B19200             19200 /* seven alternative speed */
# define CASIO_B38400             38400 /* algebrafx default speed */
# define CASIO_B57600             57600 /* seven alternative speed */
# define CASIO_B115200           115200 /* seven alternative speed */

/* Here are the control characters and other values you have in the
 * stream settings. */

# define CASIO_NCCS                0x02 /* number of control characters */
# define CASIO_XON                 0x00 /* XON  character: re-enable transm. */
# define CASIO_XOFF                0x01 /* XOFF character: disable   transm. */

/* From here, those are all in the stream settings flags.
 * Here are the stop bits settings: */

# define CASIO_STOPBITSMASK      0x0001
# define CASIO_ONESTOPBIT        0x0000 /* one stop bit */
# define CASIO_TWOSTOPBITS       0x0001 /* two stop bits */

/* Here are the parity settings: */

# define CASIO_PARMASK           0x0006
# define CASIO_PARDIS            0x0000 /* disable parity checking */
# define CASIO_PARENB            0x0002 /* enable  parity checking */
# define CASIO_PAREVEN           0x0000 /* even    parity */
# define CASIO_PARODD            0x0004 /* odd     parity */

/* Here are the DTR/RTS settings.
 * Notice that not all platforms implement this. Just do as you can. */

# define CASIO_DTRMASK           0x0018
# define CASIO_DTRCTL_DISABLE    0x0000 /* disable DTR */
# define CASIO_DTRCTL_ENABLE     0x0008 /* enable  DTR */
# define CASIO_DTRCTL_HANDSHAKE  0x0010 /* enable  DTR and handshake */

# define CASIO_RTSMASK           0x0060
# define CASIO_RTSCTL_DISABLE    0x0000 /* disable RTS */
# define CASIO_RTSCTL_ENABLE     0x0020 /* enable  RTS */
# define CASIO_RTSCTL_HANDSHAKE  0x0040 /* enable  RTS and handshake */

/* Here are the XON/XOFF software control settings.
 * XOFF disables the transmission temporarily, usually because the device at
 * the other end can't manage too much data too quickly. */

# define CASIO_XONMASK           0x0080
# define CASIO_XONCTL_DISABLE    0x0000 /* disable XON */
# define CASIO_XONCTL_ENABLE     0x0080 /* enable  XON */

/* XON re-enables the transmission, probably because the device at the end
 * has finished processing the data you sent and is ready to process more. */

# define CASIO_XOFFMASK          0x0100
# define CASIO_XOFFCTL_DISABLE   0x0000 /* disable XOFF */
# define CASIO_XOFFCTL_ENABLE    0x0100 /* enable  XOFF */
/* ************************************************************************* */
/*  Stream settings                                                          */
/* ************************************************************************* */
/* Here is the stream settings structure: */

struct casio_streamattrs_s {
	/* flags - see the above section */
	unsigned int  casio_streamattrs_flags;

	/* speed: one of the CASIO_B* constants */
	unsigned int  casio_streamattrs_speed;

	/* characters */
	unsigned char casio_streamattrs_cc[CASIO_NCCS];
};

/* This structure will be sent to your `setcomm` callback to set serial
 * communication settings.
 * And here is the stream timeouts structure: */

struct casio_timeouts_s {
	/* Initial read timeout */
	unsigned int casio_timeouts_read;

	/* In-between bytes read timeout */
	unsigned int casio_timeouts_read_bw;

	/* Total write timeout */
	unsigned int casio_timeouts_write;
};

/* This structure will be sent to your `settm` callback, usually after a state
 * change in the communication. Also, all timeouts are in milliseconds (ms). */
/* ************************************************************************* */
/*  SCSI request                                                             */
/* ************************************************************************* */
/* CASIO's fx-CG devices, also known as Prizms or Graph 90+E, use SCSI along
 * with Protocol 7.00 to communicate with the PC for things like file
 * transferring or screenstreaming (which use vendor-specific command slots).
 * As systems usually have some standard methods for SCSI, it is worth it
 * to implement an SCSI interface into libcasio streams.
 *
 * This is libcasio's SCSI request structure, inspired from Linux's
 * `sg_io_hdr_t` structure, except this structure is cross-platform.
 * Here is the different values for the data direction: */

# define CASIO_SCSI_DXFER_NONE     -1 /* no content */
# define CASIO_SCSI_DXFER_TO_DEV   -2 /* outgoing */
# define CASIO_SCSI_DXFER_FROM_DEV -3 /* incoming */

/* And here is the request structure: */

struct casio_scsi_s {
	/* command description */
	int           casio_scsi_type;
	int           casio_scsi_direction;
	unsigned int  casio_scsi_byte_transfer_length;
	unsigned long casio_scsi_logical_block;
	unsigned long casio_scsi_allocation_length;
	unsigned char casio_scsi_cbp[4];
	unsigned char casio_scsi_misc;

	/* raw data */
	unsigned int   casio_scsi_cmd_len;
	unsigned int   casio_scsi_data_len;
	unsigned int   casio_scsi_sense_len;
	unsigned char  casio_scsi_cmd[16];
	unsigned char *casio_scsi_data;
	unsigned char *casio_scsi_sense;

	/* TODO: output thingies? */
};

/* It will be sent to your `scsi_request` callback. */
/* ************************************************************************* */
/*  Public stream functions                                                  */
/* ************************************************************************* */
CASIO_BEGIN_DECLS
/* Default stream serial settings utilities. */

CASIO_EXTERN int CASIO_EXPORT casio_make_attrs
	OF((casio_streamattrs_t *casio__attrs, const char *casio__raw));

/* List serial devices (platform agnostic). */

typedef void casio_list_com_t OF((void *casio__cookie,
	const char *casio__str));

CASIO_EXTERN int CASIO_EXPORT casio_comlist
	OF((casio_list_com_t *casio__callback, void *casio__cookie));

/* Open and close a stream. */

CASIO_EXTERN int CASIO_EXPORT casio_open
	OF((casio_stream_t **casio__stream, casio_openmode_t mode,
		casio_streamtype_t casio__type, void *casio__cookie,
		const casio_streamfuncs_t *casio__callbacks));
CASIO_EXTERN int CASIO_EXPORT casio_close
	OF((casio_stream_t *casio__stream));

/* Get stream various data. */

CASIO_EXTERN int CASIO_EXPORT casio_isreadable
	OF((casio_stream_t *casio__stream));
CASIO_EXTERN int CASIO_EXPORT casio_iswritable
	OF((casio_stream_t *casio__stream));

# define casio_isreadable(CASIO__STREAM) \
	(casio_get_openmode(CASIO__STREAM) & CASIO_OPENMODE_READ)
# define casio_iswritable(CASIO__STREAM) \
	(casio_get_openmode(CASIO__STREAM) & CASIO_OPENMODE_WRITE)

CASIO_EXTERN casio_streamtype_t CASIO_EXPORT casio_get_type
	OF((casio_stream_t *casio__stream));
CASIO_EXTERN casio_openmode_t CASIO_EXPORT casio_get_openmode
	OF((casio_stream_t *casio__stream));
CASIO_EXTERN void* CASIO_EXPORT casio_get_cookie
	OF((casio_stream_t *casio__stream));
CASIO_EXTERN int CASIO_EXPORT casio_get_lasterr
	OF((casio_stream_t *casio__stream));

/* Read and write data from and to a stream. */

CASIO_EXTERN int CASIO_EXPORT casio_read
	OF((casio_stream_t *casio__stream,
		void *casio__dest, size_t casio__size));
CASIO_EXTERN int CASIO_EXPORT casio_write
	OF((casio_stream_t *casio__stream,
		const void *casio__data, size_t casio__size));

CASIO_EXTERN int CASIO_EXPORT casio_write_char
	OF((casio_stream_t *casio__stream, int casio__char));

/* Skip bytes from a stream. */

CASIO_EXTERN int CASIO_EXPORT casio_skip
	OF((casio_stream_t *casio__stream, size_t casio__size));

/* Set and get the attributes of a stream. */

CASIO_EXTERN int CASIO_EXPORT casio_init_attrs
	OF((casio_stream_t *stream));
CASIO_EXTERN int CASIO_EXPORT casio_set_attrs
	OF((casio_stream_t *casio__stream,
		const casio_streamattrs_t *casio__attrs));
CASIO_EXTERN int CASIO_EXPORT casio_get_attrs
	OF((casio_stream_t *casio__stream, casio_streamattrs_t *casio__attrs));

/* Set and get the timeouts of a stream. */

CASIO_EXTERN int CASIO_EXPORT casio_init_timeouts
	OF((casio_stream_t *casio__stream));
CASIO_EXTERN int CASIO_EXPORT casio_set_timeouts
	OF((casio_stream_t *casio__stream,
		const casio_timeouts_t *casio__timeouts));
CASIO_EXTERN int CASIO_EXPORT casio_get_timeouts
	OF((casio_stream_t *casio__stream, casio_timeouts_t *casio__timeouts));

/* Move in a file. */

CASIO_EXTERN int CASIO_EXPORT casio_seek
	OF((casio_stream_t *casio__stream,
		casio_off_t casio__offset, casio_whence_t casio__whence));
CASIO_EXTERN casio_off_t CASIO_EXPORT casio_tell
	OF((casio_stream_t *casio__stream));

/* Make out the size of a file (shortcut for making out the size). */

CASIO_EXTERN int CASIO_EXPORT casio_getsize
	OF((casio_stream_t *casio__stream, casio_off_t *casio__size));

/* Make a stream out of memory. */

CASIO_EXTERN int CASIO_EXPORT casio_open_memory
	OF((casio_stream_t **casio__stream,
		const void *casio__memory, size_t casio__size));

/* Make a stream out of another, with a limit (and empty it). */

CASIO_EXTERN int CASIO_EXPORT casio_open_limited
	OF((casio_stream_t **casio__stream,
		casio_stream_t *casio__original, size_t casio__size));
CASIO_EXTERN int CASIO_EXPORT casio_empty_limited
	OF((casio_stream_t *casio__stream));
/* ************************************************************************* */
/*  USB or serial stream opening management                                  */
/* ************************************************************************* */
/* For platforms whose the utilities aren't built-in, here is a way to add
 * your defaults, that will be used with the default functions!
 *
 * Communication port listing. */

typedef int casio_comlist_t OF((casio_list_com_t *casio__callback,
	void *casio__cookie));

CASIO_EXTERN int CASIO_EXPORT casio_add_default_comlist
	OF((casio_comlist_t *casio__function));

/* Serial communication stream opening. */

typedef int casio_opencomstream_t OF((casio_stream_t **casio__stream,
	const char *casio__path));

CASIO_EXTERN int CASIO_EXPORT casio_open_com_stream
	OF((casio_stream_t **casio__stream,
		const char *casio__path));
CASIO_EXTERN int CASIO_EXPORT casio_add_default_com_stream
	OF((casio_opencomstream_t *casio__function));

/* USB stream opening. */

typedef int casio_openusbstream_t OF((casio_stream_t **casio__stream));

CASIO_EXTERN int CASIO_EXPORT casio_open_usb_stream
	OF((casio_stream_t **casio__stream));
CASIO_EXTERN int CASIO_EXPORT casio_add_default_usb_stream
	OF((casio_openusbstream_t *casio__function));

CASIO_END_DECLS
CASIO_END_NAMESPACE
# include "builtin.h"
#endif /* LIBCASIO_STREAM_H */
