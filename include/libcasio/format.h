/* ****************************************************************************
 * libcasio/format.h -- CASIO file formats description.
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
#ifndef  LIBCASIO_FORMAT_H
# define LIBCASIO_FORMAT_H
# include <libcasio/cdefs.h>
# include <stdint.h>

/* Welcome on this new episode of the Monster Circus show! Today we'll present
 * to you the different formats surrounding CASIO calculators: open or closed,
 * legacy or legacy, obfuscated or not, I hope you'll enjoy them as much
 * as I don't! */
/* ************************************************************************* */
/*  The CASIOLINK (CAS) format                                               */
/* ************************************************************************* */
/* This is the first file format used by the CASIO community; it was
 * more or less a dump of the communications protocol CASIO used until
 * the fx-9860G (Graph 85) came out, around 2004/2005.
 *
 * It was mainly managed by the CaS software, made by Tom Wheeler with the help
 * of other people like Tom Lynn. This software was developed around 1997.
 * Other software did support the CAS protocol, such as cafix or Flash100.
 *
 * You can find its description in the following header: */

# include <libcasio/format/cas.h>
/* ************************************************************************* */
/*  The GraphCard (GRC) format                                               */
/* ************************************************************************* */
/* The GraphCard is an external micro SD card reader for CASIO calculators,
 * made by Util Pocket (not sold anymore).
 *
 * It uses its own format, built above the CAS format/protocol.
 * Basically, before headers/parts (colons included), it inserts a 16-bit
 * size of the header/part which is following, and a zero 16-bit size in
 * the end.
 *
 * See the description of the CAS format above. */
/* ************************************************************************* */
/*  The FXI format                                                           */
/* ************************************************************************* */
/* fx-Interface, an old but well done proprietary interface by CASIO, has its
 * own format: the FXI format. It's an obfuscated format (althrough the
 * obfuscation algorithm has been found).
 *
 * It starts with the following obfuscated string: */

# define FXI_OBF_MAGIC "\x32\x30" "\xF8\xA1"
# define FXI_DEC_MAGIC "\xD5\xD7" "\x1F\x46" "FX-INTERFACE - YELLOW COMPUTING"

/* Followed by loads of zero, then, the content. The description of the rest
 * of the format is described in this header: */

/*# include <libcasio/format/fxi.h> -- TODO */
/* ************************************************************************* */
/*  The Casemul format                                                       */
/* ************************************************************************* */
/* Casemul is a CASIO emulator for Microsoft Windows. It is an old software
 * that has not been updated since 2005. It can import and export other files,
 * but save its files in its own format, which use the .cas extension
 * (like the CASIOLINK format, but the two formats are completely different).
 *
 * It starts with a four-letter magic string, either "CASF" (CasFile) if the
 * file was written on a big endian platform, or "ACFS" if the platform was
 * little-endian. */

# define CASEMUL_MAGIC_BE "CASF"
# define CASEMUL_MAGIC_LE "ACFS"

/* Discover the rest in its dedicated header: */

# include <libcasio/format/casemul.h>
/* ************************************************************************* */
/*  The G1S format                                                           */
/* ************************************************************************* */
/* This format is basically a raw dump of the storage memory from the fx-9860G.
 * It won't work with fx-CP or fx-CG calculators, as the storage memory
 * has changed since. */

# include <libcasio/format/storage.h>
/* ************************************************************************* */
/*  The standard format                                                      */
/* ************************************************************************* */
/* Since around 2004/2005, CASIO has adopted a single "superformat"; we call it
 * CASIO's standard format, previously known as the G1M format (it doesn't
 * really have a *public* name, other than its magics, like 'USBPower' or
 * 'CASIO'). Basically, G1M, G2R, G1A, G3A, G3P, C2P, C1A, and others, are
 * formats defined under the standard format.
 *
 * It doesn't have one single magic string, but a few, although the standard
 * header has the same format, so we consider it as the same format. */

# include <libcasio/format/std.h>
#endif /* LIBCASIO_FORMAT_H */
