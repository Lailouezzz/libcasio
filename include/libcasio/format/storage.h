/* ****************************************************************************
 * libcasio/format/storage.h -- the storage file format description.
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
#ifndef  LIBCASIO_FORMAT_STORAGE_H
# define LIBCASIO_FORMAT_STORAGE_H
# include "../cdefs.h"
# pragma pack(1)
CASIO_BEGIN_NAMESPACE

/* Storage backup files (G1S) contain backups of the storage memory.
 * It corresponds exactly to the on-calc storage memory structure.
 *
 * According to Simon Lothar, the files contain zeroes up to 0x00270000
 * (this is the mapped location of the SMEM in real fx-9860 calculators),
 * then it has the same structure than the one used by the calculator.
 *
 * These files are not managed by the calculator directly; however, they
 * are managed by FA-124. */
/* ************************************************************************* */
/*  Directory list                                                           */
/* ************************************************************************* */
/* The SMEM (content of the G1S file once 0x270000 bytes were skipped) starts
 * with a big entry list. An entry is 32-bytes long: it starts with a common
 * part, then with a type-specific part (then unused bytes if the subheader
 * is less than 28-bytes long). Here is its structure: */

typedef struct casio_storage_entry_s {
	/* the type number - see below */
	casio_uint16_t casio_storage_entry_type;

	/* the number of the entry; if this field is 0x00, just ignore the entry.
	 * we don't know why entries with this field set to 0x00 exist...
	 * (probably backups?) */
	casio_uint16_t casio_storage_entry_id;

	/* the raw subheader */
	casio_uint8_t  casio_storage_entry_raw_subheader[28];
} casio_storage_entry_t;

/* The `type` is composed of three nibbles of identification (`type & 0xFFF`)
 * and a (high) nibble of special properties, keep that in mind.
 * Here are the known types: */

# define storage_entrytype_directory  0x110
# define storage_entrytype_file       0x120
# define storage_entrytype_fragment   0x130
# define storage_entrytype_sector     0x200

/* The special nibble is usually zero when deleted, and non-zero when active.
 * According to Simon Lothar, this is because it is possible to clear bits
 * on the flash by flashwriting a short value only; but when it comes to
 * set bits, it becomes more tricky.
 *
 * This is also why optimizing is not done straight away (to save the flash
 * a little).
 *
 * The list can contain up to 0x800 entries, and occupies the first sector.
 * Simon Lothar says the list can expand to the second sector (which is used
 * only if there is no space in the index sector or the other data sectors).
 * Entries with the 0xFFFF type should not be read.
 *
 * Some entry types can have parents. In this case, they have parent type,
 * and parent number fields. This is because elements with different types
 * follow different numerations; so the type field is here on which numeration
 * to search for the member. If the parent type is 0xFFFF (or -1, as some say),
 * then there is no parent.
 *
 * For now, only directories as parents for files have been encountered. */
/* ************************************************************************* */
/*  Sectors                                                                  */
/* ************************************************************************* */
/* Sectors represent the physical sectors of the storage memory (space!).
 * They are 64 KiB large.
 *
 * Simon Lothar says that there are 27 entries for them (or 14 on the AU models
 * because of australian exam regulations). If the logical number
 * is 0xFFFFFFFF, the sector is unused. (sometimes the logical number field
 * contains gibberish, I don't know why yet)
 *
 * Their special nibble is either 0x04 or 0x00, the signification of it is not
 * known to me yet. Here is their subheader structure: */

typedef struct casio_storage_sector_s {
	/* the sector start address - 0xFF */
	casio_uint32_t casio_storage_sector_startaddr;

	/* the sector ID - probably not using the UUID so sectors can
	 *	not be set as parent nodes :) */
	casio_uint32_t casio_storage_sector_logical_sector_number;
} casio_storage_sector_t;
/* ************************************************************************* */
/*  Directories                                                              */
/* ************************************************************************* */
/* After the sectors come the directories.
 *
 * Their spec# include "../cdefs.h"ial nibble is either 0x05 if active or 0x00 if deleted.
 * Here is their subheader structure: */

typedef struct casio_storage_directory_s {
	/* the parent type and id */
	casio_uint16_t casio_storage_directory_parent_type;
	casio_uint16_t casio_storage_directory_parent_id;

	/* the name (FONTCHARACTER-encoded) */
	casio_uint16_t  casio_storage_directory_name[12];
} casio_storage_directory_t;
/* ************************************************************************* */
/*  Files                                                                    */
/* ************************************************************************* */
/* After the directories come the files (interrupted by fragments - see after).
 *
 * Their special nibble have the same meaning that for directories.
 * Here is their subheader structure: */

typedef struct casio_storage_file_s {
	/* the parent type and id (not functional?) */
	casio_uint16_t casio_storage_file_parent_type;
	casio_uint16_t casio_storage_file_parent_id;

	/* the name (FONTCHARACTER-encoded) */
	casio_uint16_t  casio_storage_file_name[12];
} casio_storage_file_t;
/* ************************************************************************* */
/*  File fragments                                                           */
/* ************************************************************************* */
/* After each file entry comes the corresponding fragments entries.
 * Fragments are in fact links to the sectors, with some more information.
 *
 * Their special nibble have the same meaning that for files... but are not
 * always accurate, so only check on files!
 *
 * The first fragment corresponding to a file contain this file's type,
 * probably detected when the file is transferred/created/whatever. Here are
 * the known values: */

# define casio_storage_filetype_regular  0x01
# define casio_storage_filetype_unknown  casio_storage_filetype_regular
# define casio_storage_filetype_addin    0x02 /* G1A */
# define casio_storage_filetype_mcs      0x06 /* G1M */
# define casio_storage_filetype_g2r      0x2E /* G1R */

/* Why is the file type here and not in the file header? I don't know.
 * And here is the fragments' subheader structure: */

typedef struct casio_storage_fragment_s {
	/* the parent (file node) type and id */
	casio_uint16_t casio_storage_fragment_parent_type;
	casio_uint16_t casio_storage_fragment_parent_id;

	/* the file type */
	casio_uint16_t casio_storage_fragment_file_type;

	/* the fragment count and id */
	casio_uint16_t casio_storage_fragment_count;
	casio_uint16_t casio_storage_fragment_id;

	/* the sector id that the fragment is linked to */
	casio_uint16_t casio_storage_fragment_sector_id;

	/* number and offset in the sector of the fragment's bytes */
	casio_uint16_t casio_storage_fragment_data_offset;
	casio_uint16_t casio_storage_fragment_data_length;
} casio_storage_fragment_t;

CASIO_END_NAMESPACE
# pragma pack()
#endif /* LIBCASIO_FORMAT_STORAGE_H */
