/* ****************************************************************************
 * link/link.h -- internal header for link management.
 * Copyright (C) 2016-2017 Thomas "Cakeisalie5" Touhey <thomas@touhey.fr>
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
#ifndef  LOCAL_LINK_H
# define LOCAL_LINK_H 1
# include "../internals.h"

/* Internals macros. */
# define response (handle->casio_link_response)
# define command_is_supported(CASIO__N) \
	casio_seven_command_is_supported(&handle->casio_link_env, CASIO__N)
/* ************************************************************************* */
/*  Link structure                                                           */
/* ************************************************************************* */
/* Link handle internal flags. */
# define casio_linkflag_sendalt  0x0001 /* use alternative buffer */
# define casio_linkflag_alloc    0x0002 /* was allocated */
# define casio_linkflag_active   0x0004 /* active status */
# define casio_linkflag_shifted  0x0008 /* shift status */
# define casio_linkflag_init     0x0010 /* is initialized */
# define casio_linkflag_term     0x0020 /* is terminated */
# define casio_linkflag_check    0x0040 /* make the initial check */
# define casio_linkflag_disc     0x0080 /* make the dev. discovery */
# define casio_linkflag_ended    0x0100 /* the communication has ended. */

/* Link handle structure. */
struct casio_link_s {
	/* flags - see above */
	unsigned long  casio_link_flags;

	/* stream settings */
	casio_stream_t      *casio_link_stream;

	/* protocol 7.00-related core things */
	casio_seven_env_t    casio_link_env; /* see `protocol/seven/devices.c` */
	casio_seven_packet_t casio_link_response;
	casio_link_info_t    casio_link_info;
	casio_seven_type_t   casio_link_last_type;
	casio_seven_type_t   casio_link_curr_type;
	unsigned int         casio_link_last_command;

	/* MCS head */
	casio_mcshead_t casio_link_mcshead;

	/* Protocol 7.00 server callbacks. */
	casio_seven_server_func_t *casio_link_seven_callbacks[256];

	/* Raw sending packet buffers. */
	size_t        casio_link_send_buffers_size[2];
	unsigned char casio_link_send_buffers[2][MAX_PACKET_SIZE];

	/* Raw receiving packet buffers. */
	unsigned char casio_link_recv_buffer[MAX_PACKET_SIZE];
};

/* Decode the data field of specific packets. */
extern int casio_seven_decode_data    OF((casio_link_t *casio__handle,
	const unsigned char *casio__raw, unsigned int casio__raw_size));
extern int casio_seven_decode_command OF((casio_link_t *casio__handle,
	const unsigned char *casio__raw, unsigned int casio__raw_size));
extern int casio_seven_decode_ack     OF((casio_link_t *casio__handle,
	const unsigned char *casio__raw, unsigned int casio__raw_size));

/* Special send functions. */
extern int casio_seven_send_again OF((casio_link_t *casio__handle));

/* Special packet functions. */
extern int casio_seven_send_err_resend    OF((casio_link_t *casio__handle));
extern int casio_seven_send_timeout_check OF((casio_link_t *casio__handle));

/* Encode and decode raw data. */
extern unsigned int casio_seven_decoderaw OF((void *casio__dest,
	const void *casio__encoded, unsigned int casio__size));
extern unsigned int casio_seven_encoderaw OF((void *casio__dest,
	const void *casio__raw, unsigned int casio__size));

#endif
