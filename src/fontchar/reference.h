/* ****************************************************************************
 * fontchar/reference.h -- description of the community-made FONTCHARACTER
 *                         reference binary set file.
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
#ifndef  LOCAL_FONTCHAR_REFERENCE_H
# define LOCAL_FONTCHAR_REFERENCE_H 1
# define PATH "/usr/share/fontcharacter/latest.set"
# pragma pack(1)

/* The file starts with a magic field followed by a version field, to check
 * that we're parsing a file which isn't supposed to be nonsense. */

# define FONTCHAR_MAGIC "CASIOFC\x7F"

struct file_magic {
	casio_uint8_t magic[8]; /* should be "CASIOFC\x7F" */
	casio_uint8_t version;  /* should be 0x01 */
};

/* Then comes the header with all of the data.
 * The flags are to check if the different character encodings are enabled. */

# define FONTCHAR_FLAG_UNI     0x01 /* Unicode */
# define FONTCHAR_FLAG_CAT     0x02 /* CAT tokens */
# define FONTCHAR_FLAG_NEWCAT  0x04 /* Newcat tokens */
# define FONTCHAR_FLAG_CTF     0x08 /* CTF tokens */
# define FONTCHAR_FLAG_CASEMUL 0x10 /* Casemul tokens */

/* Picture formats for the icons. */

# define FONTCHAR_FORMAT_MONO  0x0100 /* Monochrome with fill bits. */

struct file_header {
	casio_uint8_t  leading; /* number of majors. */
	casio_uint16_t total;  /* total number of characters. */
	casio_uint8_t  flags;
	casio_uint8_t  height; /* characters picture height. */
	casio_uint16_t format; /* picture format. */
	casio_uint32_t casio_fh__reserved;
	casio_uint32_t checksum; /* basic checksums for all pools */
	casio_uint32_t filesize; /* file size */
	casio_uint32_t datasize; /* data zone size */
};

/* Then comes a list of the character leaders and their associated start
 * in the character table. */

struct file_lead {
	casio_uint8_t  code;
	casio_uint8_t  casio_fl__reserved;
	casio_uint16_t start;
};

/* Then a list of the characters themselves with their string-like data
 * as offsets in the file's data pool. */

struct file_char {
	casio_uint8_t  leading;
	casio_uint8_t  main;

	casio_uint8_t  seqlen;
	casio_uint8_t  unilen;
	casio_uint8_t  otherlen[4];

	casio_uint32_t seqoff;
	casio_uint32_t unioff;
	casio_uint32_t otheroff[4];
};

# pragma pack()
#endif /* LOCAL_FONTCHAR_REFERENCE_H */
