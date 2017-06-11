/* ****************************************************************************
 * link/seven/command.c -- the command utilities.
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
 * Actually, most of the commands have the same format, so most are inline
 * functions defined in `libcasio/packetio.h`. These are the functions for the
 * general format, and the commands with custom formats.
 * ************************************************************************* */
#include "../link.h"
#include <stdlib.h>
#include <string.h>

/* ************************************************************************* */
/*  Logging                                                                  */
/* ************************************************************************* */
/**
 *	getowstring:
 *	Get overwrite string (useful for logging).
 *
 *	@arg	code		overwrite code
 *	@return				the string
 */

static const char *getowstring(casio_seven_ow_t code)
{
	static const char *moo[] = {
		"request confirmation before overwriting",
		"terminate if file exists",
		"force overwrite"
	};

	if (code > 0x02) return ("<unknown overwriting mode>");
	return (moo[code]);
}
/* ************************************************************************* */
/*  Macros                                                                   */
/* ************************************************************************* */
/* if command is not supported, return */
#define CHECK_IF_COMMAND_IS_SUPPORTED(N) \
	if (!command_is_supported(N)) \
		return (casio_error_op);

/**
 *	casio_seven_send_cmd:
 *	Send command packet without data field.
 *
 *	Used to perform or request actions. The subtype selects the action.
 *
 *	@arg	handle		the link handle
 *	@arg	subtype		the subcommand
 *	@return				if it worked
 */

int casio_seven_send_cmd(casio_link_t *handle, unsigned int subtype)
{
	CHECK_IF_COMMAND_IS_SUPPORTED(subtype)

	/* save last sent command */
	handle->casio_link_last_command = subtype;

	/* send */
	return (casio_seven_send_basic(handle, casio_seven_type_cmd, subtype, 1));
}

/**
 *	casio_seven_send_cmd_data:
 *	Send command packet with data field.
 *
 *	Used to perform or request actions. The subtype selects the action.
 *
 *	@arg	handle		the link handle
 *	@arg	subtype		The subcommand
 *	@arg	overwrite	Mode of operation if a file exists
 *	@arg	datatype	MCS data type
 *	@arg	filesize	the filesize
 *	@arg	arg1		first command argument
 *	@arg	arg2		second command argument
 *	@arg	arg3		third command argument
 *	@arg	arg4		fourth command argument
 *	@arg	arg5		fifth command argument
 *	@arg	arg6		sixth command argument
 *	@return				if it worked
 */

int casio_seven_send_cmd_data(casio_link_t *handle, unsigned int subtype,
	int overwrite, unsigned int datatype, unsigned long filesize,
	const char *arg1, const char *arg2, const char *arg3,
	const char *arg4, const char *arg5, const char *arg6)
{
	unsigned char buf[120 /* 24 + 96 */], *b;
	unsigned int buflen;
	size_t l1, l2, l3, l4, l5, l6;

	CHECK_IF_COMMAND_IS_SUPPORTED(subtype)
	handle->casio_link_last_command = subtype;

	/* get args length */
	l1 = arg1 ? strlen(arg1) : 0;
	l2 = arg2 ? strlen(arg2) : 0;
	l3 = arg3 ? strlen(arg3) : 0;
	l4 = arg4 ? strlen(arg4) : 0;
	l5 = arg5 ? strlen(arg5) : 0;
	l6 = arg6 ? strlen(arg6) : 0;
	if (l1 > 16 || l2 > 16 || l3 > 16 || l4 > 16 || l5 > 16 || l6 > 16)
		return (casio_error_op);

	/* prepare data buffer */
	buflen = (unsigned int)(24 + l1 + l2 + l3 + l4 + l5 + l6);

	/* put info */
	casio_putascii(buf, overwrite, 2);
	casio_putascii(&buf[2], datatype, 2);
	casio_putascii(&buf[4], filesize, 8);
	casio_putascii(&buf[12], l1, 2);
	casio_putascii(&buf[14], l2, 2);
	casio_putascii(&buf[16], l3, 2);
	casio_putascii(&buf[18], l4, 2);
	casio_putascii(&buf[20], l5, 2);
	casio_putascii(&buf[22], l6, 2);

	/* put strings */
	b = &buf[24];
	memcpy(b, arg1, l1); b += l1;
	memcpy(b, arg2, l2); b += l2;
	memcpy(b, arg3, l3); b += l3;
	memcpy(b, arg4, l4); b += l4;
	memcpy(b, arg5, l5); b += l5;
	memcpy(b, arg6, l6);

	/* send */
	return (casio_seven_send_ext(handle,
		casio_seven_type_cmd, subtype, buf, buflen, 1));
}

/* ************************************************************************* */
/*  Special commands                                                         */
/* ************************************************************************* */
/**
 *	casio_seven_send_cmdsys_setlink:
 *	Set link settings.
 *
 *	@arg	handle		the link handle.
 *	@arg	baudrate	the baud rate.
 *	@arg	parity		the parity (0 for none, 1 for odd, 2 for even).
 *	@arg	stopbits	the number of stop bits (1 or 2).
 *	@return				if it worked.
 */

int casio_seven_send_cmdsys_setlink(casio_link_t *handle,
	int baudrate, int parity, int stopbits)
{
	char sbaud[10], sparity[10], sstopbits[2];

	/* check if is a serial connexion */
	if (~casio_get_type(handle->casio_link_stream) & casio_streamtype_serial)
		return (casio_error_op);

	/* make arguments */
	sprintf(sbaud, "%d",
		baudrate >= 0 && baudrate < 1000000 ? baudrate : 9600);
	sprintf(sstopbits, "%d", (stopbits >= 2) + 1);
	if (!parity)         strcpy(sparity, "NONE");
	else if (parity % 2) strcpy(sparity, "ODD");
	else                 strcpy(sparity, "EVEN");

	/* send the command and receive answer */
	return (casio_seven_send_cmd_data(handle, casio_seven_cmdsys_setlink,
		0, 0, 0, sbaud, sparity, sstopbits, NULL, NULL, NULL));
}

/**
 *	casio_seven_send_cmdosu_upandrun:
 *	Upload and run a binary.
 *
 *	Risky and can only be interpreted by bootcode (OS Update mode).
 *
 *	@arg	handle		the link handle
 *	@arg	upsize		the uploaded file size
 *	@arg	loadaddr	the load address (usually 0x88030000)
 *	@arg	straddr		the start adress (usually 0x88030000)
 *	@return				if it worked
 */

int casio_seven_send_cmdosu_upandrun(casio_link_t *handle,
	unsigned long upsize, unsigned long loadaddr, unsigned long straddr)
{
	unsigned char buf[24];

	CHECK_IF_COMMAND_IS_SUPPORTED(casio_seven_cmdosu_upandrun)
	handle->casio_link_last_command = casio_seven_cmdosu_upandrun;

	/* put info */
	casio_putascii(&buf[0],    upsize, 8);
	casio_putascii(&buf[8],  loadaddr, 8);
	casio_putascii(&buf[16],  straddr, 8);

	/* send packet */
	return (casio_seven_send_ext(handle, casio_seven_type_cmd,
		casio_seven_cmdosu_upandrun, buf, 24, 1));
}
/* ************************************************************************* */
/*  Decode incoming packet                                                   */
/* ************************************************************************* */
/* macro to copy an argument into the packet */
#define cpy_arg(I) { \
	size_t ALEN = arglengths[I]; \
	memcpy(packet->casio_seven_packet__argsdata[I], p, ALEN); \
	packet->casio_seven_packet__argsdata[I][ALEN] = 0; \
	packet->casio_seven_packet_args[I] = ALEN ? \
		packet->casio_seven_packet__argsdata[I] : NULL; \
	p += ALEN; \
	\
	msg((ll_info, "D%d is '%s'", I, packet->casio_seven_packet_args[I])); }

/**
 *	casio_seven_decode_command:
 *	Get data from command data field.
 *
 *	Layout is described in the fxReverse projet documentation.
 *
 *	@arg	handle		the link handle
 *	@arg	raw			the raw data
 *	@arg	raw_size	the raw data size
 *	@return				if there was an error
 */

int casio_seven_decode_command(casio_link_t *handle,
	const unsigned char *raw, unsigned int raw_size)
{
	casio_seven_packet_t *packet = &handle->casio_link_response;
	size_t arglengths[6], total_args;
	const unsigned char *p;

	/* check up_and_run command */
	if (packet->casio_seven_packet_code == casio_seven_cmdosu_upandrun) {
		if (raw_size != 12) return (1);
		packet->casio_seven_packet_uploadsize = casio_getascii(raw, 4);
		packet->casio_seven_packet_loadaddr = casio_getascii(&raw[4], 4);
		packet->casio_seven_packet_straddr = casio_getascii(&raw[8], 4);
		return (0);
	}

	/* check minimum size */
	if (raw_size < 24) return (1);

	/* get overwrite */
	packet->casio_seven_packet_ow = casio_getascii(raw, 2);
	msg((ll_info, "overwrite mode is '%s' (0x%02x)",
		getowstring(packet->casio_seven_packet_ow),
		packet->casio_seven_packet_ow));

	/* get data type */
	packet->casio_seven_packet_mcstype = casio_getascii(&raw[2], 2);
	msg((ll_info, "datatype is 0x%02x", packet->casio_seven_packet_mcstype));

	/* filesize */
	packet->casio_seven_packet_filesize = casio_getascii(&raw[4], 8);
	msg((ll_info, "filesize is %lo", packet->casio_seven_packet_filesize));

	/* args length */
	arglengths[0] = casio_getascii(&raw[12], 2);
	arglengths[1] = casio_getascii(&raw[14], 2);
	arglengths[2] = casio_getascii(&raw[16], 2);
	arglengths[3] = casio_getascii(&raw[18], 2);
	arglengths[4] = casio_getascii(&raw[20], 2);
	arglengths[5] = casio_getascii(&raw[22], 2);
	total_args = arglengths[0] + arglengths[1]
		+ arglengths[2] + arglengths[3] + arglengths[4]
		+ arglengths[5];

	/* re-check size */
	if (raw_size != 24 + total_args) return (1);

	/* copy the arguments */
	p = &raw[24];
	cpy_arg(0) cpy_arg(1) cpy_arg(2)
	cpy_arg(3) cpy_arg(4) cpy_arg(5)

	/* it worked */
	return (0);
}
