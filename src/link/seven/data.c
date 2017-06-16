/* ****************************************************************************
 * link/seven/data.c -- data packet and buffer sending.
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
 *
 * These functions are about sending and receiving data buffers.
 * ************************************************************************* */
#include "data.h"

/* ************************************************************************* */
/*  Main functions                                                           */
/* ************************************************************************* */
/**
 *	casio_seven_send_data_packet:
 *	Send data packet.
 *
 *	Carries 'raw' data in the context of a command. E.g. file data.
 *	Maximum data size is 256 octets.
 *
 *	@arg	handle		the link handle
 *	@arg	total		the total number of data packets in trans
 *	@arg	id			the packet id
 *	@arg	data		the data part
 *	@arg	datasize	the data part size (in bytes)
 *	@arg	resp		should listen to response (shifting-related)
 *	@return				if it worked
 */

int CASIO_EXPORT casio_seven_send_data_packet(casio_link_t *handle,
	unsigned int total, unsigned int id,
	const void *data, unsigned int datasize, int resp)
{
	/* make new buffer */
	unsigned char buf[8 + datasize];
	casio_putascii(buf, total, 4);
	casio_putascii(&buf[4], id, 4);
	memcpy(&buf[8], data, datasize);

	/* send packet */
	return (casio_seven_send_ext(handle, casio_seven_type_data,
		handle->casio_link_last_command, buf, 8 + datasize, resp));
}

/**
 *	casio_seven_send_quick_data_packet:
 *	Send data packet.
 *
 *	Carries 'raw' data in the context of a command. E.g. file data.
 *	Maximum data size is 256 octets. Buffer must have 8 spare bytes because
 *	that's the advantages of this function above the one before : it doesn't
 *	memcpy again.
 *
 *	@arg	handle		the link handle
 *	@arg	total		the total number of data packets in trans
 *	@arg	id			the packet id
 *	@arg	buf			the buffer with 8 spare bytes at the beginning
 *	@arg	datasize	the data part size (in bytes)
 *	@arg	resp		should listen to response (shifting-related)
 *	@return				if it worked
 */

int CASIO_EXPORT casio_seven_send_quick_data_packet(casio_link_t *handle,
	unsigned int total, unsigned int id,
	void *buf, unsigned int datasize, int resp)
{
	/* make new buffer */
	unsigned char *cbuf = buf;
	casio_putascii(cbuf, total, 4);
	casio_putascii(&cbuf[4], id, 4);

	/* send packet */
	return (casio_seven_send_ext(handle, casio_seven_type_data,
		handle->casio_link_last_command, buf, 8 + datasize, resp));
}

/**
 *	casio_seven_unshift:
 *	Unshift packet.
 *
 *	@arg	handle		the link handle
 *	@return				if it worked
 */

int CASIO_EXPORT casio_seven_unshift(casio_link_t *handle)
{
	/* truly unshift */
	int err;
	if ((err = casio_seven_receive(handle, 1)))
		return (err);
	handle->casio_link_flags &= ~casio_linkflag_shifted;

	/* then return */
	return (0);
}
/* ************************************************************************* */
/*  Decode a data packet data                                                */
/* ************************************************************************* */
/**
 *	casio_seven_decode_data:
 *	Get data from data packet data field.
 *
 *	@arg	handle		the link handle
 *	@arg	raw			raw data
 *	@arg	raw_size	raw data size
 *	@return				if there was an error.
 */

int CASIO_EXPORT casio_seven_decode_data(casio_link_t *handle,
	const unsigned char *raw, unsigned int raw_size)
{
	casio_seven_packet_t *packet = &handle->casio_link_response;

	/* Command 0x70-0x7F and above are used by Simon Lothar in fxRemote.
	 * They do not have the "ID"/"TOTAL" fields.
	 *
	 * XXX: these data packets can be identified using the fact that
	 * directly before, an ACK is sent by the same device than this
	 * data packet, without any need for an answer from the other part.
	 * This is a preferable method of identifying these strangly
	 * formatted data packets. */
	if (packet->casio_seven_packet_code < 0x70
	 || packet->casio_seven_packet_code > 0x7F) {
		/* total number */
		packet->casio_seven_packet_total = casio_getascii(raw, 4);
		msg((ll_info, "Total data packets : %u",
			packet->casio_seven_packet_total));

		/* current id */
		packet->casio_seven_packet_id = casio_getascii(&raw[4], 4);
		msg((ll_info, "Data packet ID : %u", packet->casio_seven_packet_id));

		raw = &raw[8];
		raw_size -= 8;
	}

	/* Data. */
	packet->casio_seven_packet_data_size = raw_size;
	memcpy(packet->casio_seven_packet_data, raw, raw_size);
	msg((ll_info, "Decoded data (%uo) :",
		packet->casio_seven_packet_data_size));
	mem((ll_info, packet->casio_seven_packet_data,
		packet->casio_seven_packet_data_size));

	/* no error */
	return (0);
}
