/* ****************************************************************************
 * link/seven/receive.c -- receive and decode the packet.
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
 * These functions are about receiving and decoding a packet.
 * It is also about managing checksum problems and timeouts.
 * ************************************************************************* */
#include "../link.h"
#include <stdlib.h>
#include <string.h>
#define checksub8(BUF, SIZE, INI) \
	casio_checksum_sub(&((char*)(BUF))[1], (SIZE) - 3, (INI))

/* ---
 * Functions useful for logging.
 * --- */

/** 
 *	getcmdstring:
 *	Get command code string (useful for logging).
 *
 *	@arg	code		command code
 *	@return				the string
 */

CASIO_LOCAL const char *getcmdstring(unsigned int code)
{
	static const char *commands[] = {
		/* system commands */
		"restart the equipment",
		"get device info",
		"set link settings",
		NULL, NULL, NULL,
		"set link timeout",
		"OS Update verification #1",
		"OS Update verification #2",
		"OS Update verification #3",
		"OS Update verification #4",
		NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

		/* MCS commands */
		"create directory on mcs",
		"delete directory on mcs",
		"rename directory on mcs",
		"change working directory on mcs",
		"request file transfer from mcs",
		"transfer file on mcs",
		"delete file on mcs",
		"rename file on mcs",
		"copy file on mcs",
		"request all files transfer on mcs",
		"reset mcs",
		"request mcs capacity",
		"transmit mcs capacity",
		"request all mcs file info",
		"transfer all mcs file info",
		"request RAM image",
		"transfer RAM image",
		"request setup entry",
		"transfer setup entry",
		"request all setup entry",
		NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

		/* Flash commands */
		"create directory on flash",
		"delete directory on flash",
		"rename directory on flash",
		"change working directory on flash",
		"request flash file",
		"transfer flash file",
		"delete file on flash",
		"rename file on flash",
		"copy file on flash",
		"request all flash file",
		"reset flash",
		"capacity transmit request",
		"capacity transmit",
		"request flash file info",
		"transfer flash file info",
		"request ROM image",
		"transfer ROM image",
		"optimize flash filesystem",

		/* OS Update */
		"request CASIOWIN entry",
		"send CASIOWIN entry",
		"request boot code",
		"send boot code",
		"upload and run"
	};

	/* return the string */
	if (code > 0x56 || !commands[code])
		return ("<unknown>");
	return (commands[code]);
}

/**
 *	geterrstring:
 *	Get error string (useful for logging).
 *
 *	@arg	code		error code
 *	@return				the string
 */

CASIO_LOCAL const char *geterrstring(unsigned int code)
{
	static const char *strings[] = {
		"default error",
		"should resend",
		"file exists, overwrite request",
		"will not overwrite",
		"overwrite impossible",
		"memory is full"
	};

	if (code > 0x05) return ("<unknown error>");
	return (strings[code]);
}

/**
 *	gettermstring:
 *	Get termination cause string (useful for logging).
 *
 *	@arg	code		cause code
 *	@return				the string
 */

CASIO_LOCAL const char *gettermstring(casio_seven_term_t code)
{
	static const char *strings[] = {
		"default",
		"user requested",
		"timeouts",
		"on overwrite request"
	};

	if (code > 0x03) return ("<unknown termination reason>");
	return (strings[code]);
}

/* ---
 * Main receiving function.
 * --- */

/**
 *	casio_seven_decode:
 *	Receive a packet.
 *
 *	This is the main receiving function. It receives, parses, and returns
 *	if everything has been successfully done or not.
 *	It uses buffered I/O to read progressively all of the packet.
 *
 *	@arg	handle		the link handle
 *	@arg	scralign	shall we align screen?
 *	@return				the error (0 if ok)
 */

#define buffer handle->casio_link_recv_buffer
#define COMPLETE_PACKET(N) { \
	size_t size##__LINE__ = (size_t)N; \
	int COMP_PACKET_err = casio_read(handle->casio_link_stream, \
		&buffer[received], &size##__LINE__); \
	received += size##__LINE__; if (COMP_PACKET_err) return (COMP_PACKET_err); }

CASIO_LOCAL int casio_seven_decode(casio_link_t *handle, int scralign)
{
	size_t received = 0;
	int check_sum = 1, is_extended;
	unsigned long csum, csum_ex;
	unsigned int subtype, data_size;

	/* Get first three bytes, check if is CAL. */

	do {
		COMPLETE_PACKET(3)
		if (memcmp(buffer, "CAL", 3))
			break;

		/* get the CAL value */
		msg((ll_info, "is the magic CAL!"));
		COMPLETE_PACKET(4) /* TODO: check if is 00D0, and what is it? */
		msg((ll_info, "CAL value is %.4s", &buffer[3]));
		check_sum = 0;

		/* good as new! */
		received = 0;
		msg((ll_info, "Restart receiving the packet."));
	} while (1);

	/* Align on the screen if required. */

	if (scralign) {
		int tries = MAX_PACKET_SIZE + 1;

		/* Adjust. */

		while (memcmp(buffer, "\x0BTY", 3)) {
			if (!--tries)
				return (casio_error_unknown);

			buffer[0] = buffer[1];
			buffer[1] = buffer[2];
			received = 2;
			COMPLETE_PACKET(1)
		}
	}

	/* Get the type. */

	response.casio_seven_packet_type = buffer[0];

	/* Image has a particular format starting from here, look for it now! */

	if (response.casio_seven_packet_type == casio_seven_type_ohp) {
		unsigned int image_size;
		size_t hdsize;

		msg((ll_info, "Johnson, this is a screen."));
		COMPLETE_PACKET(3)

		/* get image by its type */
		if (!memcmp(&buffer[1], "TYP01", 5)) {
			msg((ll_info, "This is normal VRAM, we know it !"));
			response.casio_seven_packet_pictype = casio_pictureformat_1bit;
			response.casio_seven_packet_width = 128;
			response.casio_seven_packet_height = 64;
			image_size = 1024;
		} else if (!memcmp(&buffer[1], "TYPZ1", 5)
		 || !memcmp(&buffer[1], "TYPZ2", 5)) {
			int typz_size_length; /* size field length */
			casio_typz_t *s = (void*)&buffer[6];

			msg((ll_info, "Prizm VRAM!"));
			if (buffer[5] == '1')
				typz_size_length = 6;
			else /* '2' */
				typz_size_length = 8;

			COMPLETE_PACKET(typz_size_length)
			COMPLETE_PACKET(sizeof(casio_typz_t))

			image_size = casio_getascii(&buffer[6], typz_size_length);
			s = (void*)&buffer[6 + typz_size_length];

			response.casio_seven_packet_width =
				casio_getascii(s->casio_typz_width, 4);
			response.casio_seven_packet_height =
				casio_getascii(s->casio_typz_height, 4);

			/* check the type */
			response.casio_seven_packet_pictype = 0;
			if (!memcmp(s->casio_typz_enc, "RC2", 3))
				response.casio_seven_packet_pictype =
					casio_pictureformat_16bit;
			else if (!memcmp(s->casio_typz_enc, "RC3", 3))
				response.casio_seven_packet_pictype =
					casio_pictureformat_4bit_rgb;
			else if (!memcmp(s->casio_typz_enc, "RM2", 3))
				response.casio_seven_packet_pictype =
					casio_pictureformat_2bit_dual;
			elsemsg((ll_error, "Unknown encoding: %.3s", s->casio_typz_enc));

			/* FIXME: check size according to format? */
			msg((ll_info, "Image size: %uB", image_size));
			check_sum = 1;
		} else {
			msg((ll_error, "Unknown picture encoding: %.5s", &buffer[1]));
			return (casio_error_csum);
		}

		/* check if we should skip */
		if (!response.casio_seven_packet_pictype
		 || image_size > CASIO_SEVEN_MAX_VRAM_SIZE) {
			casio_skip(handle->casio_link_stream, image_size + check_sum * 2);
			return (casio_error_unknown);
		}

		/* complete packet */
		hdsize = received;
		msg((ll_info, "Get screen content (%uo)", image_size));
		COMPLETE_PACKET(image_size)
		memcpy(&response.casio_seven_packet_vram, &buffer[hdsize], image_size);

		/* check the sum */
		if (check_sum) {
			COMPLETE_PACKET(2)

			/* calculate the checksums */
			csum    = checksub8(buffer, received, 0);
			csum_ex = casio_getascii(&buffer[received - 2], 2);

			/* check them */
			if (csum != csum_ex) {
				msg((ll_error, "Checksum problem: expected 0x%02lX, "
					"got 0x%02lX", csum_ex, csum));
				return (casio_error_csum);
			}
		}
		check_sum = 1;

		/* log */
		msg((ll_info,
			"received the following [screen] packet (%" CASIO_PRIuSIZE "o) :",
			received));
		mem((ll_info, buffer, received));

		/* and return the packet */
		return (0);
	}

	/* type, subtype and extended */
	COMPLETE_PACKET(1) subtype = casio_getascii(&buffer[1], 2);

	/* - extended (beginning) - */
	data_size = 0;
	is_extended = (buffer[3] == '1');
	if (is_extended) {
		/* get data size */
		COMPLETE_PACKET(4)
		data_size = casio_getascii(&buffer[4], 4);

		/* check data size */
		if (data_size > 8 + CASIO_SEVEN_MAX_ENCDATA_SIZE) {
			int err;

			msg((ll_warn,
				"Was 0x%02X/0x%02X extended packet with %u data bytes",
				response.casio_seven_packet_type, subtype, data_size));
			msg((ll_warn, "That's bigger than our internal buffer size, "
				"skipping."));
			err = casio_skip(handle->casio_link_stream, data_size + 2);
			if (err) return (err);
			return (casio_error_csum);
		}

		/* get data */
		COMPLETE_PACKET(data_size + 2)
	} else
		COMPLETE_PACKET(2)

	/* log */
	msg((ll_info, "received the following [normal] packet (%uo) :",
		received));
	mem((ll_info, buffer, received));

	/* calculate checksum */
	csum_ex = checksub8(buffer, received, 0);
	csum    = casio_getascii(&buffer[received - 2], 2);
	if (csum_ex != csum)
		return (casio_error_csum);

	/* check if we should read a binary zero */
	if (response.casio_seven_packet_type
	 == casio_seven_type_cmd && subtype == casio_seven_cmdosu_upandrun)
		COMPLETE_PACKET(1)

	/* - extended (finish) - */
	if (is_extended)
		data_size = casio_seven_decoderaw(&buffer[8], &buffer[8], data_size);

	/* get fields out for specific packets */
	response.casio_seven_packet_code = subtype;
	switch (response.casio_seven_packet_type) {
	/* - for command - */
	case casio_seven_type_cmd:
		msg((ll_info, "packet was interpreted as a command one"));
		msg((ll_info, "command is '%s' (0x%02X)",
			getcmdstring(subtype), subtype));
		if (is_extended
		 && casio_seven_decode_command(handle, &buffer[8], data_size))
		 	return (casio_error_csum);
		break;

	/* - for data - */
	case casio_seven_type_data:
		msg((ll_info, "packet was interpreted as a data one"));
		casio_seven_decode_data(handle, &buffer[8], data_size);
		break;

	/* - for roleswap - */
	case casio_seven_type_swp:
		msg((ll_info, "packet was interpreted as a roleswap one"));
		msg((ll_info, "becoming active again"));
		handle->casio_link_flags |= casio_linkflag_active;
		break;

	/* - for check - */
	case casio_seven_type_chk:
		msg((ll_info, "packet was interpreted as a check one"));
		response.casio_seven_packet_initial = !subtype;
		break;

	/* - for ack - */
	case casio_seven_type_ack:
		msg((ll_info, "packet was interpreted as an ack one"));
		response.casio_seven_packet_ow = (subtype == 0x01);
		response.casio_seven_packet_extended = (subtype == 0x02);
		if (response.casio_seven_packet_extended
		 && casio_seven_decode_ack(handle, &buffer[8], data_size))
			return (casio_error_csum);
		if (subtype == 0x03
		 || handle->casio_link_last_type == casio_seven_type_end)
			handle->casio_link_flags |= casio_linkflag_ended;
		break;

	/* - for error - */
	case casio_seven_type_nak:
		msg((ll_info, "packet was interpreted as an error one"));
		msg((ll_info, "error is '%s' (0x%02X)",
			geterrstring(subtype), subtype));
		response.casio_seven_packet_code = subtype;
		if (subtype == casio_seven_err_fullmem)
			handle->casio_link_flags |= casio_linkflag_ended;
		break;

	/* - for termination - */
	case casio_seven_type_end:
		msg((ll_info, "packet was interpreted as a terminate one"));
		msg((ll_info, "termination cause is '%s' (0x%02X)",
			gettermstring(subtype), subtype));
		response.casio_seven_packet_code = subtype;
		if (subtype == casio_seven_term_user)
			return (casio_error_int);
		break;

	/* other */
	default:
		msg((ll_fatal, "bad type received ! type was 0x%02x",
			response.casio_seven_packet_type));
		return (casio_error_unknown);
	}

	/* finally, return the packet */
	return (0);
}

/* ---
 * Receiving function with error management.
 * --- */

/**
 *	casio_seven_receive:
 *	Receives packet, checks for errors.
 *
 *	@arg	handle			the link handle
 *	@arg	flags			receive flags.
 *	@return					if it worked
 */

int CASIO_EXPORT casio_seven_receive(casio_link_t *handle, unsigned int flags)
{
	int err, wasresend = 0;

	/* Make checks. */

	if (!handle)
		return (casio_error_init);

	/* Main loop. */

	while (1) {
		/* Receive the packet. */

		msg((ll_info, "Receiving the packet..."));
		err = casio_seven_decode(handle,
			!!(flags & CASIO_SEVEN_RECEIVEFLAG_SCRALIGN));

		/* Check out the error. */

		if (err && !(err == casio_error_csum
		 && ~flags & CASIO_SEVEN_RECEIVEFLAG_CHECKSUMS)) {
			switch (err) {
			case casio_error_nocalc:
				/* If the calculator couldn't be found, terminate
				 * the communication. */

				goto fail;

			case casio_error_timeout:
				/* If it is a timeout, send a check, get the corresponding
				 * ACK and remove a try. */

				err = casio_seven_send_timeout_check(handle);
				if (err)
					goto fail;

				err = casio_seven_decode(handle, 0);
				if (!err && response.casio_seven_packet_type
				 != casio_seven_type_ack)
					err = casio_error_unknown;
				if (err)
					goto fail;

				/* Try again.
				 * TODO: check if we should resend the previous packet...? */
				break;

			case casio_error_csum:
				/* If it is a bad checksum, ask for resend; also,
				 * reset tries number in case of bad checksum between
				 * timeouts.
				 *
				 * First of all, check if we didn't already give a chance
				 * to the other side. */

				if (wasresend) {
					msg((ll_error, "Two checksum failures, ending now."));
					casio_seven_send_basic(handle, casio_seven_type_end,
						casio_seven_term_timeouts, 0);

					handle->casio_link_flags |= casio_linkflag_ended;
					err = casio_error_damned;
					goto fail;
				}

				/* If we receive an invalid checksum while we are in packet
				 * shifting mode, or if we received an error when we sent a
				 * resend error, cut connexion... we can't do anything. */

				if ((handle->casio_link_flags & casio_linkflag_shifted) \
				 || wasresend) {
					handle->casio_link_flags |= casio_linkflag_ended;
					return (casio_error_damned);
				}

				/* Otherwise, check tries and send resend error. */

				err = casio_seven_send_err_resend(handle);
				if (err)
					return (err);

				/* set things */
				wasresend = 1;
				err = casio_error_csum;
				break;

			default:
				/* Should not catch bad type & others. */

				return (casio_error_csum);
			}

			/* Then try again to get the answer :) */

			continue;
		}

		/* TODO: check out the type and stuff...? */
		/* Go away! */

		break;
	}

	return (0);
fail:
	handle->casio_link_flags |= casio_linkflag_ended;
	return (err);
}
