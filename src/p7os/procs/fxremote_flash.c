/* ****************************************************************************
 * p7os/procs/fxremote_flash.c -- fxremote procedure for flashing.
 * Copyright (C) 2017 Thomas "Cakeisalie5" Touhey <thomas@touhey.fr>
 *
 * This file is part of p7utils.
 * p7utils is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2.0 of the License,
 * or (at your option) any later version.
 *
 * p7utils is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with p7utils; if not, see <http://www.gnu.org/licenses/>.
 *
 * [EN] WARNING: Two calculators were bricked during the correction of
 * this file. Any change to this file has to be made very carefully.
 *
 * [FR] AVERTISSEMENT : Deux calculatrices ont été rendues inutilisables
 * pendant la correction de ce fichier. Tout changement doit être fait
 * avec la plus grande prudence.
 * ************************************************************************* */
#include "../main.h"

/* FIXME: this procedure already bricked two calculators.
 * Avoid bricking a third one and wait for Dark Storm for Planète Casio
 * to try to find the problem before using it. PLEASE. */

#define KiB * 1024
#ifndef  min
# define min(a, b) ((a) < (b) ? (a) : (b))
#endif

struct data_transfer_block {
	casio_uint32_t destination;
	casio_uint32_t length;
	unsigned char  data[0x3FC];
};

struct data_copy_block {
	casio_uint32_t destination;
	casio_uint32_t length;
	casio_uint32_t source;
};

/**
 *	send_sector:
 *	Send a sector data and flashwrite it.
 *
 *	@arg	handle	the link handle.
 *	@arg	stream	the update.exe stream.
 *	@arg	addr	the address.
 *	@arg	size	the sector data size.
 *	@return			the error code (0 if ok).
 */

static int send_sector(casio_link_t *handle, casio_stream_t *buffer,
	unsigned long addr, unsigned long size)
{
	int err, buf_err;
	osdisp_t osdisp_cookie;

	/* Sending the data to the Update.Exe's. */
	{
		char osdisp_string[100];
		struct data_transfer_block block;
		unsigned long buf = 0x88030000, left = size;
		int num, total;

		osdisp_init(&osdisp_cookie, osdisp_string, "Flashwriting.");
		sprintf(osdisp_string, "Copying sector at 0x%08lX (%lu bytes).",
			addr, size);

		num = 1;
		total = size / 0x3FC + !!(size % 0x3FC);
		osdisp(&osdisp_cookie, 1, 0);
		while (left) {
			unsigned long len = left < 0x3FC ? left : 0x3FC;

			/* Prepare the block. */
			block.destination = casio_be32toh(buf);
			block.length      = casio_be32toh(len);
			if ((buf_err = casio_read(buffer, block.data, len))) {
				osdisp_interrupt(&osdisp_cookie);
				return (casio_error_read);
			}

			/* Pass the block. */
			buf += len; left -= len;

			/* Send the command with the block. */
			err = casio_seven_send_ext(handle, casio_seven_type_cmd,
				0x70, &block, 8 + len, 1);
			if (err) { osdisp_interrupt(&osdisp_cookie); return (err); }
			osdisp(&osdisp_cookie, num++, total);
		}
		osdisp_success(&osdisp_cookie);
	}

	/* Copy the internal buffer to the flash sector. */
	{
		struct data_copy_block block;

		block.destination = casio_htobe32(addr);
		block.length      = casio_htobe32(size);
		block.source      = casio_htobe32(0x88030000);
		err = casio_seven_send_ext(handle, casio_seven_type_cmd,
			0x71, &block, 12, 1);
		if (err) return (err);
		printf("Flashwrote the internal buffer at 0x%08lX.\n", addr);
	}

	return (0);
}

/**
 *	fxremote_flash:
 *	Flash the fxRemote way.
 *
 *	@arg	args		the arguments.
 *	@return				the error code (0 if ok).
 */

int fxremote_flash(args_t *args)
{
	int err, buf_err;
	casio_link_t *handle = NULL;
	casio_streamattrs_t attrs;
	casio_stream_t *buffer = args->local;
	unsigned long addr, top; casio_off_t size;
	osdisp_t osdisp_cookie;

	/* Get the file size. */

	err = casio_getsize(buffer, &size);
	if (err)
		return (err);
	if (!size)
		return (casio_error_empty);

	top = (unsigned long)size | 0xA0000000;
	printf("Image to flash goes from 0xA0010000 to 0x%08lX.\n", top - 1);

	/* Open the link. */

	attrs.casio_streamattrs_flags =
		  CASIO_TWOSTOPBITS | CASIO_PARDIS | CASIO_DTRCTL_HANDSHAKE
		| CASIO_RTSCTL_HANDSHAKE | CASIO_XONCTL_ENABLE | CASIO_XOFFCTL_ENABLE;
	attrs.casio_streamattrs_speed = CASIO_B115200;
	attrs.casio_streamattrs_cc[CASIO_XON]  = 0x11;
	attrs.casio_streamattrs_cc[CASIO_XOFF] = 0x13;
	err = open_link(&handle, args,
		CASIO_LINKFLAG_ACTIVE | CASIO_LINKFLAG_NODISC, &attrs);
	if (err)
		return (err);

	/* Get the 52 bytes at 0xA000FFC0.
	 * Probably checking if the two first bytes are "00 20" (SH3) or
	 * else ("00 00", "FF FF", ..., SH4).
	 * The calculator first sends the ACK (managed by `casio_seven_send_cmd`),
	 * then the data packet (managed by `casio_seven_receive`).
	 *
	 * The original fxRemote does this, so will I.
	 * XXX: should I check as well? */

	if ((err = casio_seven_send_cmd(handle, 0x76))
	 || (err =  casio_seven_receive(handle,    1)))
		goto fail;

	/* Clear the sectors.
	 * The original fxRemote does that before setting the sectors.
	 * Maybe it just ANDs, or something...? Anyway, I'll do it as well. */

	{
		int num = 1, total = (top - 0xA0010000) / 0x10000;

		osdisp_init(&osdisp_cookie, "Clearing the sectors.", "Cleared!");
		osdisp(&osdisp_cookie, 1, 0);
		for (addr = 0xA0010000; addr < top; addr += 0x10000) {
			casio_uint32_t localaddr;

			localaddr = casio_htobe32(addr);
			err = casio_seven_send_ext(handle, casio_seven_type_cmd,
				0x72, &localaddr, 4, 1);
			if (err) {
				osdisp_interrupt(&osdisp_cookie);
				goto fail;
			}

			osdisp(&osdisp_cookie, num++, total);
		}

		osdisp_success(&osdisp_cookie);
	}

	/* Send each sector. */

	for (addr = 0xA0020000; addr < top; addr += 0x10000) {
		/* Seek to the address. */

		buf_err = casio_seek(buffer, addr & 0x0FFFFFFF, CASIO_SEEK_SET);
		if (buf_err) {
			err = casio_error_noseek;
			goto fail;
		}

		/* Send the sector from there. */

		err = send_sector(handle, buffer, addr, min(size - addr, 0x10000));
		if (err)
			goto fail;
	}

	/* fxRemote sends the first OS sector once all of the other sectors
	 * are sent, for some reason. I'll do the same here. */

	addr = 0xA0010000;
	{
		/* Seek to the address. */

		buf_err = casio_seek(buffer, addr & 0x0FFFFFFF, CASIO_SEEK_SET);
		if (buf_err) {
			err = casio_error_noseek;
			goto fail;
		}

		/* Send the sector from there. */
		err = send_sector(handle, buffer, addr, min(size - addr, 0x10000));
		if (err)
			goto fail;
	}

	/* Final command. */
	err = casio_seven_send_basic(handle, casio_seven_type_cmd, 0x78, 0);
	if (err)
		goto fail;

	err = 0;
fail:
	if (err)
		casio_seven_send_term_because(handle, casio_seven_term_user);
	casio_close_link(handle);
	return (err);
}
