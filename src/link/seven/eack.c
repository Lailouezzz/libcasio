/* ****************************************************************************
 * link/seven/eack.c -- extended ACK management.
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
#include "../link.h"
#include <string.h>

/* ************************************************************************* */
/*  Raw layout of an extended ack data                                       */
/* ************************************************************************* */
typedef struct {
	/* hardware identifier - ASCII */
	unsigned char hwid[8];
	/* processor identifier - ASCII */
	unsigned char cpuid[16];

	/* preprogrammed ROM capacity - ASCII-hex (Ko) */
	unsigned char preprog_rom_capacity[8];
	/* flash ROM capacity - ASCII-hex (Ko) */
	unsigned char flash_rom_capacity[8];
	/* RAM capacity - ASCII-hex (Ko) */
	unsigned char ram_capacity[8];

	/* preprogrammed ROM version - "xx.xx.xxxx" + 0xff bytes */
	unsigned char preprog_rom_version[16];

	/* bootcode version - "xx.xx.xxxx" + 0xff bytes */
	unsigned char bootcode_version[16];
	/* bootcode offset - ASCII-hex */
	unsigned char bootcode_offset[8];
	/* bootcode size - ASCII-hex (Ko) */
	unsigned char bootcode_size[8];

	/* OS version - "xx.xx.xxxx" + 0xff bytes */
	unsigned char os_version[16];
	/* OS offset - ASCII-hex */
	unsigned char os_offset[8];
	/* OS size - ASCII-hex (Ko) */
	unsigned char os_size[8];

	/* protocol version - "x.xx" */
	unsigned char protocol_version[4];
	/* product ID */
	unsigned char product_id[16];
	/* name set by user in SYSTEM */
	unsigned char username[16];
} packetdata_ackext_t;

/* ************************************************************************* */
/*  Utilities                                                                */
/* ************************************************************************* */
/**
 *	version_of_string:
 *	Get version from 16-bytes char buffer.
 *
 *	data is not a string as it doesn't finish with an '\0'.
 *
 *	@arg	ver			the version destination
 *	@arg	data		the raw version data ("xx.xx.xx" + type)
 */

CASIO_LOCAL void version_of_string(casio_version_t *ver, const unsigned char *data)
{
	casio_decode_version(ver, (const char*)data);
}

/**
 *	string_of_version:
 *	Put version in 16-bytes char buffer.
 *
 *	@arg	ver		the version source.
 *	@arg	data	the raw version data destination ("xx.xx.xx" + type).
 */

CASIO_LOCAL void string_of_version(unsigned char *data,
	const casio_version_t *ver)
{
	casio_encode_version((char*)data, ver);
	memset(&data[10], '\xFF', 6);
}
/* ************************************************************************* */
/*  Send ACK packets                                                         */
/* ************************************************************************* */
/**
 *	casio_seven_send_eack:
 *	Sends an extended ack.
 *
 *	@arg	handle		the link handle
 *	@arg	info		the link information to return.
 *	@return				the error code (0 if ok)
 */

int CASIO_EXPORT casio_seven_send_eack(casio_link_t *handle,
	casio_link_info_t *info)
{
	packetdata_ackext_t raw;

	/* initialize the structure with 0xFFs */
	memset(&raw, 0xFF, sizeof(packetdata_ackext_t));

	/* put in the structure */
	memcpy(raw.hwid, info->casio_link_info_hwid,
		strnlen(info->casio_link_info_hwid, 8));
	memcpy(raw.cpuid, info->casio_link_info_cpuid,
		strnlen(info->casio_link_info_cpuid, 16));

	if (!(info->casio_link_info_wiped & casio_link_info_wiped_preprog)) {
		casio_putascii(raw.preprog_rom_capacity,
			info->casio_link_info_rom_capacity, 8);
		string_of_version(raw.preprog_rom_version,
			&info->casio_link_info_rom_version);
	}

	casio_putascii(raw.flash_rom_capacity,
		casio_gethex(info->casio_link_info_flash_rom_capacity / 1024), 8);
	casio_putascii(raw.ram_capacity,
		casio_gethex(info->casio_link_info_ram_capacity / 1024), 8);

	if (!(info->casio_link_info_wiped & casio_link_info_wiped_bootcode)) {
		string_of_version(raw.bootcode_version,
			&info->casio_link_info_bootcode_version);
		casio_putascii(raw.bootcode_offset,
			info->casio_link_info_bootcode_offset, 8);
		casio_putascii(raw.bootcode_size,
			casio_gethex(info->casio_link_info_bootcode_size / 1024), 8);
	}

	if (!(info->casio_link_info_wiped & casio_link_info_wiped_os)) {
		string_of_version(raw.os_version, &info->casio_link_info_os_version);
		casio_putascii(raw.os_offset, info->casio_link_info_os_offset, 8);
		casio_putascii(raw.os_size,
			casio_gethex(info->casio_link_info_os_size / 1024), 8);
	}

	memcpy(raw.protocol_version, "7.00", 4);
	memcpy(raw.product_id, info->casio_link_info_product_id,
		strnlen(info->casio_link_info_product_id, 16));
	memcpy(raw.username, info->casio_link_info_username,
		strnlen(info->casio_link_info_username, 16));

	/* send the packet */
	return (casio_seven_send_ext(handle, casio_seven_type_ack, 0x02, &raw,
		sizeof(packetdata_ackext_t), 1));
}

/* ************************************************************************* */
/*  Decode an ACK packet                                                     */
/* ************************************************************************* */
/**
 *	cpy_string:
 *	Copy a string terminated with 0xFFs, with a maximum size.
 *
 *	@arg	dest		the destination string.
 *	@arg	src			the source string.
 *	@arg	n			the maximum size of the string.
 */

CASIO_LOCAL void cpy_string(char *dest, const char *src, size_t n)
{
	const char *l = memchr(src, 0xFF, n);

	memset(dest, 0, n + 1);
	memcpy(dest, src, l ? (size_t)(l - src) : n);
}

/**
 *	casio_seven_decode_ack:
 *	Get data from ack data field.
 *
 *	Layout is described in the fxReverse projet documentation.
 *
 *	@arg	handle		the handle
 *	@arg	data		the raw data
 *	@arg	data_size	the raw data size
 *	@return				if there was an error
 */

int CASIO_EXPORT casio_seven_decode_ack(casio_link_t *handle,
	const unsigned char *data, unsigned int data_size)
{
	casio_seven_packet_t *packet = &handle->casio_link_response;
	casio_link_info_t *info = &packet->casio_seven_packet_info;
	const packetdata_ackext_t *d = (const void*)data;

	/* check the data size */
	if (data_size != sizeof(packetdata_ackext_t)) return (1);
	info->casio_link_info_wiped = 0;

	/* log */
	msg((ll_info, "ack packet is extended"));

	/* hardware identifier */
	cpy_string(info->casio_link_info_hwid, (const char*)d->hwid, 8);
	msg((ll_info, "hardware identifier is '%s'", info->casio_link_info_hwid));
	/* processor identifier */
	cpy_string(info->casio_link_info_cpuid, (const char*)d->cpuid, 16);
	msg((ll_info, "cpu identifier is '%s'", info->casio_link_info_cpuid));

	/* preprogrammed ROM information is wiped */
	if (d->preprog_rom_version[2] == 0xFF)
		info->casio_link_info_wiped |= casio_link_info_wiped_preprog;
	ifmsg(info->casio_link_info_wiped & casio_link_info_wiped_preprog,
		(ll_info, "Preprogrammed ROM information looks wiped out!"));
	elsemsg((ll_info, "Preprogrammed ROM version is %.10s",
		d->preprog_rom_version));

	/* preprogrammed ROM capacity */
	info->casio_link_info_rom_capacity =
		casio_getascii(d->preprog_rom_capacity, 8) * 1000;
	msg((ll_info, "preprogrammed ROM capacity is %luo",
		info->casio_link_info_rom_capacity));
	/* preprogrammed ROM version */
	version_of_string(&info->casio_link_info_rom_version,
		d->preprog_rom_version);

	/* flash ROM capacity */
	info->casio_link_info_flash_rom_capacity =
		casio_getdec(casio_getascii(d->flash_rom_capacity, 8)) * 1024;
	msg((ll_info, "flash ROM capacity is %luKiB",
		info->casio_link_info_flash_rom_capacity / 1024));
	/* RAM capacity */
	info->casio_link_info_ram_capacity =
		casio_getdec(casio_getascii(d->ram_capacity, 8)) * 1024;
	msg((ll_info, "RAM capacity is %luKiB",
		info->casio_link_info_ram_capacity / 1024));

	/* bootcode information is wiped */
	if (d->bootcode_version[2] == 0xFF)
		info->casio_link_info_wiped |= casio_link_info_wiped_bootcode;
	ifmsg(info->casio_link_info_wiped & casio_link_info_wiped_bootcode,
		(ll_info, "Bootcode information looks wiped out!"));
	elsemsg((ll_info, "Bootcode version is %.10s", d->bootcode_version));

	/* bootcode version */
	version_of_string(&info->casio_link_info_bootcode_version,
		d->bootcode_version);
	/* bootcode offset */
	info->casio_link_info_bootcode_offset =
		casio_getascii(d->bootcode_offset, 8);
	msg((ll_info, "bootcode offset is 0x%08lX",
		info->casio_link_info_bootcode_offset));
	/* bootcode size */
	info->casio_link_info_bootcode_size =
		casio_getdec(casio_getascii(d->bootcode_size, 8)) * 1024;
	msg((ll_info, "bootcode size is %luKiB",
		info->casio_link_info_bootcode_size / 1024));

	/* OS information is wiped */
	if (d->os_version[2] == 0xFF)
		info->casio_link_info_wiped |= casio_link_info_wiped_os;
	ifmsg(info->casio_link_info_wiped & casio_link_info_wiped_os,
		(ll_info, "OS information looks wiped out!"));
	elsemsg((ll_info, "OS version is %.10s", d->os_version));

	/* OS version */
	version_of_string(&info->casio_link_info_os_version, d->os_version);
	/* OS offset */
	info->casio_link_info_os_offset = casio_getascii(d->os_offset, 8);
	msg((ll_info, "OS offset is 0x%08lX" , info->casio_link_info_os_offset));
	/* OS size */
	info->casio_link_info_os_size =
		casio_getdec(casio_getascii(d->os_size, 8)) * 1024;
	msg((ll_info, "OS size is %luKiB", info->casio_link_info_os_size / 1024));

	/* product ID */
	cpy_string(info->casio_link_info_product_id,
		(const char*)d->product_id, 16);
	msg((ll_info, "product ID is %s", info->casio_link_info_product_id));
	/* username */
	cpy_string(info->casio_link_info_username, (const char*)d->username, 16);
	msg((ll_info, "username is %s", info->casio_link_info_username));

	/* no error */
	return (0);
}
