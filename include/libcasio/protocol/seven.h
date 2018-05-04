/* ****************************************************************************
 * libcasio/protocol/seven.h -- CASIO's latest protocol.
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
#ifndef  LIBCASIO_PROTOCOL_SEVEN_H
# define LIBCASIO_PROTOCOL_SEVEN_H
# include "../cdefs.h"
# include "../link.h"
# include "typz.h"
CASIO_BEGIN_NAMESPACE

/* ---
 * Packets.
 * --- */

/* Protocol 7.00 (the latest proprietary protocol from CASIO) is a protocol
 * using several-bytes packets. Every packet, with one exception we'll detail
 * later, contains a type, a subtype, an optional payload, and a checksum.
 *
 * Here are the known packet types: */

typedef unsigned int casio_seven_type_t;

# define casio_seven_type_cmd  0x01 /* ask something (initiate packet flow) */
# define casio_seven_type_data 0x02 /* send some data */
# define casio_seven_type_swp  0x03 /* change roles */
# define casio_seven_type_chk  0x05 /* check if there is an interlocutor */
# define casio_seven_type_ack  0x06 /* acknowledge ('okay') */
# define casio_seven_type_ohp  0x0B /* screenstreaming packet */
# define casio_seven_type_nak  0x15 /* acknowledge negatively ('nope') */
# define casio_seven_type_end  0x18 /* end the communication */

/* The subtype has a different meaning according to the type it is used with.
 * For check packets, it can mean: */

# define casio_seven_chk_ini   0x00 /* initial check */
# define casio_seven_chk_wait  0x01 /* in-communication check, while waiting */

/* For ACK packets: */

# define casio_seven_ack_normal 0x00 /* normal ACK */
# define casio_seven_ack_ow     0x01 /* confirm overwrite */
# define casio_seven_ack_ext    0x02 /* extended ACK */
# define casio_seven_ack_term   0x03 /* ACK, end the communication (0x56) */

/* For NAK packets (error packets): */

typedef unsigned int casio_seven_err_t;

# define casio_seven_err_default        0x00 /* default error (unused?) */
# define casio_seven_err_resend         0x01 /* checksum/timeout error,
                                              * resend: please let the
                                              * library use it. */
# define casio_seven_err_overwrite      0x02 /* server response:
                                              * confirm ow? */
# define casio_seven_err_dont_overwrite 0x03 /* denial */
# define casio_seven_err_other          0x04 /* generic error
                                              * (other errors) */
# define casio_seven_err_fullmem        0x05 /* full memory (terminates!) */

/* For END packets (terminate packets): */

typedef unsigned int casio_seven_term_t;

# define casio_seven_term_default   0x00 /* normal termination */
# define casio_seven_term_user      0x01 /* user has interrupted comm. */
# define casio_seven_term_timeouts  0x02 /* terminated due to timeouts */
# define casio_seven_term_overwrite 0x03 /* in response to `ow_terminate` */

/* I said there was an exception earlier on, I wasn't lying: screenstreaming
 * packets have a different format than other packets. Basically, after the
 * type, there is a five-letter subtype (such as "TYP01" or "TYPZ1"), and
 * for some types, subheaders. libcasio translates this into a VRAM and a
 * picture format, which you can see in the Protocol 7.00 packet
 * representation. */

/* ---
 * Screenstreaming.
 * --- */

/* Screenstreaming packet flow is totally different from the normal packet
 * flow: it's just the calculator sending its screen repeateadly, not
 * expecting any answer from the 'passive' side. It is only available when
 * the calculator is connected using USB, not 3-pin, to communicate with the
 * current machine.
 *
 * There are two different types of screenstreaming, distinguished by a
 * five-letter capture type:
 * - "TYP01" is the basic monochrome capture (1-bit, 128x64);
 * - "TYPZ1"/"TYPZ2" is the extended capture (see
 *   `libcasio/protocol/typz.h`). */

/* ---
 * Command payload.
 * --- */

/* The command has a payload which contain, even in the cases where they are
 * not useful, the overwrite status (OW), the MCS raw data type (DT),
 * the file size (FS), and the six arguments (of variable length,
 * although they never are above 16 bytes long).
 *
 * Some commands have a completely different format than others, such as
 * the 0x56 command (upload'n'run), which also finishes with a binary zero.
 *
 * The overwrite status (OW) is useful for when sending a file. It can
 * basically say, using the codes: */

typedef unsigned int casio_seven_ow_t;

# define casio_seven_ow_confirm    0x00 /* ask for confirmation */
# define casio_seven_ow_terminate  0x01 /* terminate if the file exists */
# define casio_seven_ow_force      0x02 /* force overwriting */

/* With `casio_ow_confirm`, if the file exists, a
 * `casio_err_overwrite` NAK packet, then, you will either confirm with an
 * ACK packet, or infirm with a `casio_err_dont_overwrite` error packet.
 *
 * The MCS raw data type (DT) should be used with other arguments that are
 * found within the arguments -- it is all documented in `libcasio/format*`.
 * The filesize (FS) is used when sending a file or file information only.
 * For some commands (such as 0x4E), it can also be the free capacity.
 *
 * The arguments usually are:
 *
 * # | Argument signification  |      Alternative significations
 * --+-------------------------+----------------------------------------
 * 1 | Directory name          | Setup entry name
 * 2 | File name               | New directory name (directory renaming)
 *   |                         | Setup entry content (ASCII-hex)
 * 3 | New directory name      | MCS group name
 * 4 | New file name           |
 * 5 | Storage device name     |
 * 6 | ??? (unused everywhere) |
 *
 * Whether arguments are used or not depend on the command. */

/* ---
 * Protocol 7.00 limits.
 * --- */

/* The Protocol 7.00 packet representation includes all the data
 * you could have in a packet. First, here are the buffer sizes: */

# define CASIO_SEVEN_MAX_VRAM_WIDTH  384 /* max. encountered VRAM width */
# define CASIO_SEVEN_MAX_VRAM_HEIGHT 500 /* max. encountered VRAM height */

# define CASIO_SEVEN_MAX_VRAM_SIZE \
	(CASIO_SEVEN_MAX_VRAM_WIDTH * CASIO_SEVEN_MAX_VRAM_HEIGHT * 2)

# define CASIO_SEVEN_MAX_RAWDATA_SIZE 256 /* maximum raw data size */
# define CASIO_SEVEN_MAX_ENCDATA_SIZE ((CASIO_SEVEN_MAX_RAWDATA_SIZE) * 2)

# define CASIO_SEVEN_MAX_CMDARG_SIZE 256 /* maximum command argument size */

# define MAX_PACKET_SIZE (1 + 5 + CASIO_SEVEN_MAX_VRAM_SIZE + 2)

/* VRAM data isn't decoded directly by Protocol 7.00 Packet I/O utilities,
 * although some functions such as `casio_getscreen` do decode it to a 32-bit
 * pixel matrix (easier for the user, and extensible).
 * Check `libcasio/picture.h` for picture format descriptions and utilities. */

/* ---
 * Protocol 7.00 packet receiving and sending utilities.
 * --- */

CASIO_BEGIN_DECLS

/* This is the main function to receive a packet.
 * It manages checksum errors, and timeouts; the only thing you should care
 * about is the returned error.
 *
 * There are very few cases where you should this function directly, as the
 * packet sending functions also manage receiving if necessary, so don't use
 * it unless you're sure about what you're doing.
 *
 * Flags can be the following:
 *
 * `CHECKSUMS`: shall we care about checksums? Do not use if you don't want
 *              to react to checksums; for example, in the screenstreaming
 *              packet flow, the receiver should not send anything.
 * `SCRALIGN`:  shall we try aligning ourselves to the next screen packet? */

# define CASIO_SEVEN_RECEIVEFLAG_CHECKSUMS  1
# define CASIO_SEVEN_RECEIVEFLAG_SCRALIGN   2

CASIO_EXTERN int CASIO_EXPORT casio_seven_receive
	OF((casio_link_t *casio__handle, unsigned int casio__flags));

/* These are the base functions to send a packet.
 * Unless `resp` is zero, it will also get the response to the packet,
 * and store its info in the handle's packet representation.
 *
 * You shouldn't use them directly to send a packet, but here they are: */

CASIO_EXTERN int CASIO_EXPORT casio_seven_send_basic
	OF((casio_link_t *casio__handle,
		casio_seven_type_t casio__type, unsigned int casio__subtype,
		int casio__resp));
CASIO_EXTERN int CASIO_EXPORT casio_seven_send_ext
	OF((casio_link_t *casio__handle,
		casio_seven_type_t casio__type, unsigned int casio__subtype,
		const void *casio__data, unsigned int casio__size,
		int casio__resp));

/* Send checks.
 * Initial checks are useful to check if there is another device speaking
 * Protocol 7.00 at the other end of the line.
 * Waiting checks are for the other device which is waiting for a command
 * not to timeout (which takes six minutes). */

# define casio_seven_send_check(CASIO__H) \
	(casio_seven_send_basic((CASIO__H), casio_seven_type_chk, \
		casio_seven_chk_wait, 1))
# define casio_seven_send_ini_check(CASIO__H) \
	(casio_seven_send_basic((CASIO__H), casio_seven_type_chk, \
		casio_seven_chk_ini, 1))

/* Send acknowledgements (ACK packets).
 * Basic ACK packets are used for plenty of things (`resp` should always be
 * different from zero, as zero for this is used by data exchanging functions).
 * Extended ACK is used for sending server information, usually as a response
 * to the 0x01 command (sys_getinfo). */

# define casio_seven_send_ack(CASIO__H, CASIO__R) \
	(casio_seven_send_basic((CASIO__H), casio_seven_type_ack, \
		0x00, (CASIO__R)))
# define casio_seven_confirm_ow(CASIO__H) \
	(casio_seven_send_basic((CASIO__H), casio_seven_type_ack, 0x01, 1))
CASIO_EXTERN int CASIO_EXPORT casio_seven_send_eack
	OF((casio_link_t *casio__handle, casio_link_info_t *casio__info));

/* Send negative acknowledgements (NAK packets).
 * These are used to report errors. */

# define casio_seven_send_err(CASIO__H, CASIO__C) \
	(casio_seven_send_basic((CASIO__H), casio_seven_type_nak, (CASIO__C), 1))
# define casio_seven_deny_ow(CASIO__H) \
	(casio_seven_send_err((CASIO__H), casio_seven_err_dont_overwrite))

/* Send termination packets.
 * Will end the communication from the handle point of view. */

# define casio_seven_send_term_because(CASIO__H, CASIO__R) \
	(casio_seven_send_basic((CASIO__H), casio_seven_type_end, (CASIO__R), 1))
# define casio_seven_send_term(CASIO__H) \
	(casio_seven_send_term_because((CASIO__H), casio_seven_term_default))

/* Send a roleswap notice.
 * Will pass the handle in passive mode. */

# define casio_seven_send_roleswp(CASIO__H) \
	(casio_seven_send_basic((CASIO__H), casio_seven_type_swp, 0, 1))
# define casio_seven_send_swp(CASIO__H) \
	(casio_seven_send_roleswp(CASIO__H))

/* Here are the base functions for sending a command.
 * There are specific command-sending in `libcasio/protocol/seven/commands.h`,
 * so if you're using a classical command, you should use them instead. */

CASIO_EXTERN int CASIO_EXPORT casio_seven_send_cmd
	OF((casio_link_t *casio__handle, unsigned int casio__code));
CASIO_EXTERN int CASIO_EXPORT casio_seven_send_cmd_data
	OF((casio_link_t *casio__handle,
		unsigned int casio__code,  int casio__overwrite,
		unsigned int casio__datatype, unsigned long casio__filesize,
		const char *casio__arg0, const char *casio__arg1,
		const char *casio__arg2, const char *casio__arg3,
		const char *casio__arg4, const char *casio__arg5));

CASIO_END_DECLS
CASIO_END_NAMESPACE

# include <libcasio/protocol/seven/commands.h>

/* ---
 * Protocol 7.00 environment.
 * --- */

CASIO_BEGIN_NAMESPACE

/* A Protocol 7.00 environment is the type of calculator there is on the
 * other side. Environment identification is required for a more precise
 * error guessing. */

typedef struct casio_seven_env_s {
	const char  *casio_seven_env_name;     /* environment name, for logging */
	unsigned int casio_seven_env_mask;     /* the supported commands
	                                        * mask bit */
} casio_seven_env_t;

CASIO_BEGIN_DECLS
/* And here is everything you need lol. */

CASIO_EXTERN int CASIO_EXPORT casio_seven_getenv
	OF((casio_seven_env_t *casio__env, const char *casio__id));
CASIO_EXTERN int CASIO_EXPORT casio_seven_command_is_supported
	OF((const casio_seven_env_t *casio__env, unsigned int casio__code));

CASIO_END_DECLS

/* ---
 * Protocol 7.00 packet representation.
 * --- */

/* This representation contains all the possible fields, and only some of them
 * are used depending on the packet type/code. */

typedef struct casio_seven_packet_s {
	/* main info */
	casio_seven_type_t    casio_seven_packet_type;
	int                   casio_seven_packet_extended;

	/* error, termination, check */
	int                   casio_seven_packet_code;
	int                   casio_seven_packet_initial;

	/* commands */
	int                   casio_seven_packet_ow;
	int                   casio_seven_packet_mcstype;
	unsigned long         casio_seven_packet_filesize;
	char                 *casio_seven_packet_args[6];
	unsigned long         casio_seven_packet_uploadsize;
	unsigned long         casio_seven_packet_loadaddr;
	unsigned long         casio_seven_packet_straddr;

	/* data */
	unsigned int          casio_seven_packet_id;
	unsigned int          casio_seven_packet_total;
	unsigned int          casio_seven_packet_data_size;

	/* server information */
	casio_link_info_t     casio_seven_packet_info;

	/* screen */
	casio_pictureformat_t casio_seven_packet_pictype;
	unsigned int          casio_seven_packet_height;
	unsigned int          casio_seven_packet_width;

	/* big things */
	unsigned char casio_seven_packet_data[CASIO_SEVEN_MAX_RAWDATA_SIZE];
	char casio_seven_packet__argsdata[6][CASIO_SEVEN_MAX_CMDARG_SIZE + 1];
	char casio_seven_packet_vram[CASIO_SEVEN_MAX_VRAM_SIZE];
} casio_seven_packet_t;

/* To extract it from the handle, use the extractors in `libcasio/link.h`. */

/* ---
 * Packet flows.
 * --- */

/* Once the communication is initialized (`casio_init_link` with the
 * `CASIO_LINKFLAG_ACTIVE | CASIO_LINKFLAG_CHECK` flags takes care of that),
 * Protocol 7.00 is basically a set of packet flows.
 *
 * A packet flow always start with a command from the active side to the
 * passive side. Then, once the command is ACK-ed, according to the command,
 * different packet flows can occur. Here are a few characteristic examples:
 *
 * - Simple action. For example, file removing require nothing after the
 *   command confirmation;
 * - File sending. After the overwrite flow, the active side sends data
 *   packets containing the file data
 *   (`casio_seven_send_buffer`/`casio_seven_send_data`);
 * - File receiving. The active side sends a role swap packet, then the
 *   server (now active) does the file sending procedure (with command). Once
 *   it has finished sending the file, it swaps roles again;
 * - File listing. The active side sends a role swap packet, then the
 *   server (now active) sends file information commands for each entry.
 *   Once it has finished, it swap roles again.
 *
 * If you have finished doing what you wanted to do and the communication is
 * still active, send an END packet, to which the other calculator should
 * respond with an ACK. */

/* ---
 * Protocol 7.00 packet flow utilities.
 * --- */

CASIO_BEGIN_DECLS

/* Start and end the communication.
 * Is used by the link opening and closing functions;
 * you shouldn't use them unless you know what you're doing
 * (and you probably don't if you actually want to use them). */

CASIO_EXTERN int CASIO_EXPORT casio_seven_start
	OF((casio_link_t *casio__handle));
CASIO_EXTERN int CASIO_EXPORT casio_seven_end
	OF((casio_link_t *casio__handle));

/* Send and receive data, using buffers.
 * This will automatically divide your data into packets, or make up the
 * data from packets.
 *
 * Packet shifting (enabled with `shift != 0`) is a technique discovered by
 * Nessotrin (from Planète Casio), and made accessible by Cakeisalie5 in
 * libcasio (previously libp7). It makes data exchanges quicker.
 * It is advised not to use it for receiving data, or for sensitive
 * data. */

CASIO_EXTERN int CASIO_EXPORT casio_seven_send_buffer
	OF((casio_link_t *casio__handle,
		casio_stream_t *casio__stream, casio_off_t casio__size,
		int casio__shift, casio_link_progress_t *casio__progress,
		void *casio__pcookie));

CASIO_EXTERN int CASIO_EXPORT casio_seven_get_buffer
	OF((casio_link_t *casio__handle,
		casio_stream_t *casio__stream, casio_off_t casio__size,
		int casio__shift, casio_link_progress_t *casio__progress,
		void *casio__pcookie));

/* Send and receive data, using memory areas.
 * Beyond the buffer/memory difference, those and the previous ones are
 * basically the same thing. */

CASIO_EXTERN int CASIO_EXPORT casio_seven_send_data
	OF((casio_link_t *casio__handle,
		const void *casio__buf, casio_off_t casio__size,
		int casio__shift, casio_link_progress_t *casio__progress,
		void *casio__pcookie));

CASIO_EXTERN int CASIO_EXPORT casio_seven_get_data
	OF((casio_link_t *casio__handle,
		void *casio__buf, casio_off_t casio__size,
		int casio__shift, casio_link_progress_t *casio__progress,
		void *casio__pcookie));

/* Send and receive data, using a stream.
 * This is practicle when you want to encode and write on the fly. */

CASIO_EXTERN int CASIO_EXPORT casio_seven_open_data_stream
	OF((casio_stream_t **casio__stream,
		casio_link_t *casio__link, casio_off_t casio__size,
		casio_link_progress_t *casio__disp, void *casio__dcookie));

/* ---
 * Protocol 7.00 high-level abstractions.
 * --- */

/* Make the MCS interface. */

CASIO_EXTERN int CASIO_EXPORT casio_open_seven_mcs
	OF((casio_mcs_t **casio__mcs, casio_link_t *casio__link));

/* Make the filesystem interface. */

CASIO_EXTERN int CASIO_EXPORT casio_open_seven_fs
	OF((casio_fs_t **casio__filesystem, casio_link_t *casio__link));

CASIO_END_DECLS
CASIO_END_NAMESPACE
#endif /* LIBCASIO_PROTOCOL_SEVEN_H */
