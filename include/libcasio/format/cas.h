/* ****************************************************************************
 * libcasio/format/cas.h -- the older CAS file format description.
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
#ifndef  LIBCASIO_FORMAT_CAS_H
# define LIBCASIO_FORMAT_CAS_H
# include "../cdefs.h"
# pragma pack(1)
CASIO_BEGIN_NAMESPACE

/* See `libcasio/format.h` for general information about what the CASIOLINK
 * format is.
 *
 * The CASIOLINK file format is linked to how the legacy (CAS) protocol works:
 * one-byte packets are sent between the two machines (for initiating
 * communication, ACKing, NAKing, ...), unless the sent/receiving byte is
 * the double-colon ':' (0x3A) -- then it is the beginning of something
 * that is more than one-byte long. CAS files do not include the one-byte
 * packets, or repeated headers/parts (because of bad checksum or timeouts,
 * for example) -- see the legacy protocol documentation for this.
 *
 * Actually, there are three main things that start with a ':'. The main type
 * is a header, that describes the content that follows. A content can have
 * no data part (e.g. END packet), one data part (e.g. programs), or
 * more (e.g. width*height content parts for lists and matrixes, which
 * represent the individual cells). These content parts have different formats
 * according to the global content part.
 *
 * But I told you there were three things. The third is a little subtle:
 * one header can correspond to several contents. Then we have what we
 * call the CAS file*s* part (or heads part).
 * An example is 'FN' (set of editor files): we receive the main head,
 * with the count of editor files that are sent to us, then we receive one part
 * indicating the size of each files, then for each file, we receive one part,
 * which is the file content.
 *
 * We have one protocol, the CASIOLINK protocol (also called 'legacy protocol'
 * in the libcasio scope), but we have two different type of headers. The two
 * have a fixed width, so they are named by the number of bytes they occupy.
 * Notice that, for what I know, content formats don't vary between header
 * types. The checksuming technique neither. */
/* ************************************************************************* */
/*  CAS40 header                                                             */
/* ************************************************************************* */
/* The first header to have appeared is the CAS40 (40 bytes long).
 * It is known in CaS as the protocol the CFX-9700G uses.
 *
 * The header format is the following: */

typedef struct casio_cas40_s {
	casio_uint8_t casio_cas40_data[2];      /* data type -- see `type/cas.c` */
	casio_uint8_t casio_cas40_misc[5];      /* type-specific subheader */
	casio_uint8_t casio_cas40_filename[12]; /* editor filename. */
	casio_uint8_t casio_cas40_password[12]; /* editor password. */
	casio_uint8_t casio_cas40__reserved0[7]; /* 0xFFs */
	casio_uint8_t casio_cas40_checksum;
} casio_cas40_t;

/* The specific bytes are different according to the datatype, but its
 * length is fixed. */
/* ************************************************************************* */
/*  CASDYN header                                                            */
/* ************************************************************************* */
/* The CASDYN (dynamic size, super header) header appeared later.
 * It is known in CaS as the protocol the CFX-9850G uses.
 *
 * This header format was previously named 'caspro' in libcasio, then CAS50
 * when I thought the header was always 50 bytes long, then CASDYN as soon as
 * I realized this application system was there on headers of different sizes.
 *
 * I started understanding that this byte between the app and data type I
 * thought was useless was in fact not when I saw in Flash100 sources that it
 * was always 0x31 ('1' in ASCII), which are 40 bytes long. From there and
 * other pieces of documentation, here are the extension types I could find: */

# define casio_casdyn_9850  0x00 /* 50 bytes long */
# define casio_casdyn_end   0xFF /* yet an alias to `casdyn_ext_9850`,
                                  * used by the END packet. */
# define casio_casdyn_g100  0x31 /* 40 bytes long */
# define casio_casdyn_g100b 0x32 /* yet an alias to `casdyn_ext_g100`,
                                  * used for some commands */

/* Here are the common fields to all packets: */

typedef struct casio_casdyn_s {
	/* app */
	casio_uint8_t casio_casdyn_app[3];

	/* type of extension - see the `casdyn_ext_*` macros above */
	casio_uint8_t casio_casdyn_ext;
} casio_casdyn_t;

/* The known apps are:
 *
 *  Ext. | App. | Description/role
 * ------+------+----------------------------------
 *  0x00 | TXT  | Editor (not program editor);
 *  0x00 | VAL  | Send() or Receive() function
 *  0x00 | IMG  | Screen shooter?
 *  0x00 | REQ  | Request a file.
 * ------+------+----------------------------------
 *  0xFF | END  | End packet.
 * ------+------+----------------------------------
 *  0x31 | MDL  | Model description (reference)
 *  0x31 | END  | End packet.
 *  0x31 | REQ  | Request a file.
 *  0x31 | ADN  | Send a file.
 *  0x31 | FMV  | ???
 *  0x31 | FCL  | ???
 *  0x31 | MCS  | ??? (main memory?)
 * ------+------+----------------------------------
 *  0x32 | ADN  | Send a file. (bis, Flash100?)
 *  0x32 | REQ  | Request a file. (bis, Flash100?)
 *
 * The format the CFX-9850G uses (extension bytes 0x00 and 0xFF) is 50 bytes
 * long, so we'll name it CAS50. Here is the header format the CASDYN header
 * is read: */

typedef struct casio_cas50_s {
	/* types */
	casio_uint8_t  casio_cas50_data[2];

	/* data length */
	casio_uint16_t casio_cas50_width;
	casio_uint16_t casio_cas50_height;
	casio_uint8_t  casio_cas50_name[8];

	/* variable-related data */
	casio_uint8_t  casio_cas50_prefix[8]; /* "Variable" for vars,
	                                 * "PROG\x99" "0\xFF\xFF" for progs,
	                                 *  0xFFs otherwise */
	casio_uint8_t  casio_cas50_aux[8]; /* variable: "R\x0A"/"C\x0A",
	                              * editor: password */

	/* something else (?) */
	casio_uint8_t  casio_cas50_option1[2]; /* 'NL'? "\xFF\xFF" for progs */
	casio_uint8_t  casio_cas50__reserved[12]; /* other options?
	                                           * -> cf. CAT format */

	/* end of packet */
	casio_uint8_t  casio_cas50_checksum;
} casio_cas50_t;

/* The format the G100 (AlgebraFX, extension bytes 0x31 and 0x32) uses is
 * 40 bytes long, but as CAS40 already exists, we'll name this format CAS100
 * (which is funny because the Graph 100 has a CAS, Computer Algebra System).
 *
 * Information from here is mainly decoded from Flash100 sources.
 * Anyway, here is the CAS100 header after the CASDYN header is read: */

typedef struct casio_cas100_s {
	/* drive?
	 * - "INF": system
	 * - "FR0": segment
	 * - "MSG": language
	 * - "MR0": ?
	 * - "S00": ? */
	casio_uint8_t  casio_cas100_drive[3];

	/* driver number, in ASCII (0x30 + <num>)
	 * "INF" is 1, "FR0" is 1-6, "MSG" is 1, "MR0" is 4, "S00" is 0 */
	casio_uint8_t  casio_cas100_id;

	/* group size (size of each fragment group part to be sent): 0x400, 1024 */
	casio_uint32_t casio_cas100_size;

	/* ExportDrive: 0x80, type? */
	casio_uint32_t casio_cas100_type;

	/* drive size: 0x20000 bytes */
	casio_uint32_t casio_cas100_drive_size;

	/* 0xFFs */
	casio_uint8_t  casio_cas100__unknown[18];

	/* checksum */
	casio_uint8_t  casio_cas100_checksum;
} casio_cas100_t;

/* However, the CAS100 header has a variant, with the 'MDL' application: it
 * looks like system info. The information here is obtained from Flash100's
 * source code, once again. */

typedef struct casio_cas100info_s {
	/* board identifier? "ZX945" */
	casio_uint8_t  casio_cas100info_board[5];
	casio_uint8_t  casio_cas100info_delim0; /* 0xFF */

	/* serial settings? "038400N"
	 * this would mean 38400 bauds, no parity (2 stop bits?) */
	casio_uint8_t  casio_cas100info_settings[11];

	/* ROM version? "1.00" or "1.01" */
	casio_uint8_t  casio_cas100info_version[4];

	/* values? */
	casio_uint32_t casio_cas100info__val1; /* 0xF00 or 0x1000;common: 0x1000 */
	casio_uint32_t casio_cas100info__val2; /* 0x400 */
	casio_uint32_t casio_cas100info__val3; /* 0x100 */

	/* hex value with prefix... what? */
	casio_uint8_t  casio_cas100info_hex[4]; /* "0x07", litterally,
	                                   * or 0x07 followed by three 0xFFs */
	casio_uint8_t  casio_cas100info_delim1; /* 0xFF */

	/* checksum */
	casio_uint8_t  casio_cas100info_checksum;
} casio_cas100info_t;

/* As you can guess, CAS40 and CASDYN are, in theory, incompatible.
 * In practice, anyhow, they more or less are: libcasio reads the first 4 bytes
 * from the header, and tries to identify a CASDYN header; if it doesn't
 * manage, it falls back on CAS40.
 *
 * Here are the content formats for the two header types: */

CASIO_END_NAMESPACE
# pragma pack()
# include "cas/program.h" /* programs, f-mem */
# include "cas/cell.h" /* lists, matrixes, variables */
# include "cas/backup.h"
# include "cas/picture.h"
# include "cas/graph.h"
# include "cas/gmem.h"
#endif /* LIBCASIO_FORMAT_CAS_H */
