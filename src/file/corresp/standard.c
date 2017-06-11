/* ****************************************************************************
 * file/corresp/standard.c -- standard header types.
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
#include "standard.h"

/* ************************************************************************* */
/*  Helpers                                                                  */
/* ************************************************************************* */
/* Flags */
#define f_c1  casio_stdflag_check1
#define f_c2  casio_stdflag_check2
#define f_sub casio_stdflag_sub
#define f_pic casio_stdflag_pic

/* Subtype correspondance type */
struct type_info {
	/* identification */
	const char *type;

	/* info */
	const char  *info;
	unsigned int flags;

	/* types */
	casio_filefor_t  platform;
	casio_filetype_t libtype;
};

/* Main type correspondance type */
struct main_info {
	/* identification */
	const char *type;

	/* subtypes */
	struct type_info *subtypes;
};

/* Extension correspondance type */
struct ext_corresp {
	/* identification */
	const char *ext;

	/* data */
	const char *info;
	unsigned int platform;
	unsigned int libtype;
	unsigned int flags;
};
/* ************************************************************************* */
/*  Main correspondances                                                     */
/* ************************************************************************* */
#define magic_common "\x00\x10\x00\x10\x00"
#define cp_magic "\x00\x01\x00\x01\x00"
#define blank "\xFF\xFF\xFF\xFF\xFF\xFF"

static struct type_info subtypes_usbpower[] = {
	/* add-ins */
	{"\xf3" magic_common, "add-in", f_c1 | f_c2,
		casio_filefor_fx, casio_filetype_addin},
	{"\x2c" cp_magic, "fx-CG add-in", f_c1 | f_c2 | f_sub,
		casio_filefor_cg, casio_filetype_addin},

	/* MCS */
	{"\x62" magic_common, "mcs (g2r)", f_c1 | f_c2,
		casio_filefor_fx, casio_filetype_mcs}, /* TODO: add flag? */
	{"\x31" magic_common, "mcs", f_c1 | f_c2,
		casio_filefor_fx, casio_filetype_mcs},
	{"\x75" magic_common, "mcs (fx-CG)", f_c1 | f_c2,
		casio_filefor_cg, casio_filetype_mcs},

	/* Language */
	{"\x12" magic_common, "fx language file", f_c1 | f_c2,
		casio_filefor_fx, casio_filetype_lang},

	/* E-Activities */
	{"\x49" magic_common, "e-activity (document)", f_c1 | f_c2,
		casio_filefor_fx, casio_filetype_eact},

	/* Pictures */
	{"\x7d" magic_common, "fx-CG picture", f_c1 | f_c2 | f_pic,
		casio_filefor_cg, casio_filetype_pict},

	{NULL, NULL, 0, 0, 0}
};
static struct type_info subtypes_ly755[] = {
	{"\x2c" cp_magic, "fx-CG language file", f_c1 | f_c2 | f_sub,
		casio_filefor_cg, casio_filetype_lang},

	{NULL, NULL, 0, 0, 0}
};
static struct type_info subtypes_casio[] = {
	{"c2p\0\0\0", "Classpad picture", f_pic,
		casio_filefor_cp, casio_filetype_pict},

	{NULL, NULL, 0, 0, 0}
};

static struct main_info standard_types[] = {
	/* USBPower (the most common one) */
	{"USBPower", subtypes_usbpower},

	/* Ly755 (Classpad-specific) */
	{"Ly755   ", subtypes_ly755},

	/* CASIO (only used for c2p...?) */
	{"CASIO\0\0\0", subtypes_casio},

	/* terminating */
	{NULL, NULL}
};

/* Extension correspondances */
static struct ext_corresp ext_types[] = {
	/* fx types */
	{"g1l", "fx language file",
		casio_filefor_fx, casio_filetype_lang,  0},
	{"g1n", "fx fkeys file",
		casio_filefor_fx, casio_filetype_fkey,  0},
	{"g1m", "fx mcs archive",
		casio_filefor_fx, casio_filetype_mcs,   0},
	{"g1r", "fx mcs backup",
		casio_filefor_fx, casio_filetype_mcs,   0},
	{"g2m", "fx OS2 mcs archive",
		casio_filefor_fx, casio_filetype_mcs,   0},
	{"g2r", "fx OS2 mcs backup",
		casio_filefor_fx, casio_filetype_mcs,   0},
	{"g1a", "fx add-in",
		casio_filefor_fx, casio_filetype_addin, 0},
	{"g1e", "fx e-activity",
		casio_filefor_fx, casio_filetype_eact,  0},

	/* fx-CP types */
	{"c1a", "classpad add-in",
		casio_filefor_cp, casio_filetype_addin, f_sub},
	{"c2p", "classpad picture",
		casio_filefor_cp, casio_filetype_pict,  f_pic},

	/* fx-CG types */
	{"g3l", "fx-CG language file",
		casio_filefor_cg, casio_filetype_lang,  f_sub},
	{"g3n", "fx-CG fkeys file",
		casio_filefor_cg, casio_filetype_fkey,  f_sub},
	{"g3a", "fx-CG add-in",
		casio_filefor_cg, casio_filetype_addin, f_sub},
	{"g3e", "fx-CG e-activity",
		casio_filefor_cg, casio_filetype_eact,  0},
	{"g3p", "fx-CG picture",
		casio_filefor_cg, casio_filetype_pict,  f_pic},

	/* sentinel */
	{NULL, NULL, 0, 0, 0}
};
/* ************************************************************************* */
/*  Get the type information                                                 */
/* ************************************************************************* */
/**
 *	casio_maketype_std:
 *	Get the standard header type information.
 */

int casio_maketype_std(const char *path, casio_standard_header_t *std,
	unsigned int *flags, casio_filefor_t *platform, casio_filetype_t *type)
{
	const char blank14[14] =
		"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";
	const char *info;

	/* look if blank */
	if (!memcmp(&std->casio_standard_header_main_id, blank14, 14)) {
		struct ext_corresp *e; char ext[5];

		if (!path) return (1);
		msg((ll_info,
			"Blank type! Let's use the extension to try and identify it."));
		casio_getext(path, ext, 5);

		for (e = ext_types; e->ext && strcmp(e->ext, ext); e++);
		if (!e->ext) {
			msg((ll_fatal, "No matching extension with '%s'!", ext));
			return (1);
		}

		/* fill in info */
		info = e->info;
		if (platform) *platform = e->platform;
		if (type) *type = e->libtype;
		if (flags) *flags = e->flags;
	} else {
		struct main_info *mt; struct type_info *sub;
		casio_uint8_t *main_id = std->casio_standard_header_main_id;
		casio_uint8_t *subtype = std->casio_standard_header_subtype;

		/* look for main_id */
		for (mt = standard_types; mt->type
		  && memcmp(mt->type, main_id, 8); mt++);
		if (!mt->type) {
			msg((ll_fatal, "Main ID not recognized:"));
			mem((ll_fatal, main_id, 8));
			return (1);
		}

		/* look for subtype */
		for (sub = mt->subtypes; sub->type
		  && memcmp(sub->type, subtype, 6); sub++);
		if (!sub->type) {
			msg((ll_fatal, "Type not managed (yet?):"));
			mem((ll_fatal, subtype, 6));
			return (1);
		}

		/* fill in info */
		info = sub->info;
		if (platform) *platform = sub->platform;
		if (type) *type = sub->libtype;
		if (flags) *flags = sub->flags;
	}

	msg((ll_info, "Read type: %s", info));
	return (0);
}
