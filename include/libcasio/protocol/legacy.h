/* ****************************************************************************
 * libcasio/protocol/legacy.h -- CASIO's legacy protocol.
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
 * Aside from the public and easy interface described in `libcasio.h`, you may
 * want to have more control on the communication. This file is one of
 * the ones: it allows you to exchange packets directly, using legacy protocols
 * (libcasio internals will take care of the odd details).
 * ************************************************************************* */
#ifndef  LIBCASIO_PROTOCOL_LEGACY_H
# define LIBCASIO_PROTOCOL_LEGACY_H
# include "../cdefs.h"

/* In these protocols, there are two types of packets: one-byte packets,
 * and multi-byte packets (headers, headers data, data).
 * Multi-byte packets are managed elsewhere. */

/* ---
 * Checks.
 * --- */

/* At the beginning of the communication, the active side sends one of two
 * possible bytes: 0x15 if the communication is supposed to be interactive,
 * 0x16 if not. */

# define casio_legacy_start_interactive    0x15
# define casio_legacy_start_noninteractive 0x16

/* Then the passive side is supposed to send an initial packet confirmation: */

# define casio_legacy_start_ack            0x13

/* ---
 * Main communication.
 * --- */

/* The active side starts by sending a header (starting with 0x3A, ':').
 * The passive side then answers with either an ACK (0x06), or
 * an overwrite error (0x21) if the file already exists.
 *
 * If we, as te active side, are in an overwrite confirmation situation,
 * we can either send an ABORT (0x15) to not overwrite, or an ACK if it
 * accepts. If we have something else than an ACK after the overwrite
 * confirmation, then this means that the passive side cannot overwrite.
 *
 * Once the header is accepted, the active side sends the header data parts
 * and the data parts corresponding to the headers.
 *
 * For the headers or (headers) data parts, there is a checksum. If this
 * checksum isn't right, the passive side asks the header/part again, with
 * the 0x2B error. If the header data type is unknown, the passive side
 * sends 0x24 (or 0x00?). If the header has some errors, the passive side
 * sends 0x51, and the transfer aborts. */

# define casio_legacy_unknown      0x00 /* zero length data,
                                         * send no more, ok? */
# define casio_legacy_ack          0x06
# define casio_legacy_abort        0x15 /* "something is wrong with
                                         *  the header" */
# define casio_legacy_exists       0x21 /* file exists, overwrite? */
# define casio_legacy_cafixunknown 0x22 /* this is in cafix, no action. */
# define casio_legacy_unknown_type 0x24 /* or out of memory? */
# define casio_legacy_resend       0x2B
# define casio_legacy_errors       0x51

/* TODO: what the hell is interactive? */

#endif /* LIBCASIO_PROTOCOL_LEGACY_H */
