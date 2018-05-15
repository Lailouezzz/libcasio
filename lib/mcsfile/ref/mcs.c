/* ****************************************************************************
 * mcsfile/ref/mcs.c -- get the MCS type out of raw data.
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
#include "ref.h"

/* ---
 * Local types.
 * --- */

/* Type flags. */

#define noarg          0x0000
#define arg            0x0001
#define arg_is_num     0x0002
#define arg_is_let     0x0000
#define weight_by_gid  0x0004
#define groupable      0x0008

/* Correspondance type. */

struct type_corresp {
	/* identification */
	unsigned int rawtype;
	const char *name; /* NULL: don't check */
	unsigned int flags; /* TODO: move */

	/* information for logging and re-constituting information */
	const char *dirname;
	const char *info;

	/* libcasio information */
	casio_mcstype_t type;
};

/* Group correspondance type. */

struct group_corresp {
	/* identification */
	const char *name; /* NULL = don't check */
	unsigned int flags;

	/* types */
	const struct type_corresp *types;
};

/* ---
 * Correspondances.
 * --- */

/*	All correspondances. Some remarks:
 *	- I think files with "PROGRAM" group are split because it was originally
 *	  planned they would be in groups like 'PROGRAM <name>', like for captures
 *	  or lists, but they abandoned that. Why they made such groups, I have no
 *	  idea yet.
 *	- 0xfe is more or less the generic type ID for "data".
 *	- Types with a directory name starting with '@' are app-specific files,
 *	  with the dirname being the internal name of the related application. */

#define TTERM {0, NULL, 0, NULL, NULL, 0}
CASIO_LOCAL const struct type_corresp types_setup[] = {
	/* There are setup files with three different types…? */

	{0x14, "SETUP", 0, "$GLOBAL",
		"setup (0x14)", casio_mcstype_setup},
	{0x12, "SETUP", 0, "$GLOBAL",
		"setup (0x12)", casio_mcstype_setup},
	{0x00, "SETUP", 0, "$GLOBAL",
		"setup", casio_mcstype_setup},
	TTERM};
CASIO_LOCAL const struct type_corresp types_alphamem[] = {
	{0x00, "ALPHA MEM", 0, "$GLOBAL",
		"alpha memory", casio_mcstype_alphamem},
	{0x00, "", arg | arg_is_let, "$GLOBAL",
		"variable", casio_mcstype_variable},
	TTERM};
CASIO_LOCAL const struct type_corresp types_string[] = {
	{0x04, "STR", arg | arg_is_num, "main",
		"string", casio_mcstype_string},
	TTERM};
CASIO_LOCAL const struct type_corresp types_list[] = {
	{0x05, "1LIST", arg | arg_is_num, "main",
		"list", casio_mcstype_list},
	TTERM};
CASIO_LOCAL const struct type_corresp types_listfile[] = {
	{0x05, "1LIST", arg | arg_is_num | weight_by_gid, "main",
		"list", casio_mcstype_list},
	TTERM};
CASIO_LOCAL const struct type_corresp types_mat[] = {
	{0x06, "MAT_", arg | arg_is_let, "main",
		"matrix", casio_mcstype_mat},
	TTERM};
CASIO_LOCAL const struct type_corresp types_vct[] = {
	{0x06, "VCT_", arg | arg_is_let, "main",
		"vector", casio_mcstype_vct},
	TTERM};
CASIO_LOCAL const struct type_corresp types_program[] = {
	{0x01, NULL, 0, "system",
		"program", casio_mcstype_program},
	TTERM};
CASIO_LOCAL const struct type_corresp types_yeqdata[] = {
	{0x02, NULL /* e.g. "1" */, 0, "main",
		"unknown", 0x00},
	{0x05, NULL /* "VWIN" */, 0, "main",
		"unknown", 0x00},
	TTERM};
CASIO_LOCAL const struct type_corresp types_vwin[] = {
	{0x05, "VMEM", arg | arg_is_num, "main",
		"unknown", 0x00},
	TTERM};
CASIO_LOCAL const struct type_corresp types_picture[] = {
	{0x07, "PICT", arg | arg_is_num, "main",
		"picture", casio_mcstype_pict},
	TTERM};
CASIO_LOCAL const struct type_corresp types_capt[] = {
	{0x0A, "CAPT", arg | arg_is_num, "@REV2",
		"capture", casio_mcstype_capt},
	TTERM};
CASIO_LOCAL const struct type_corresp types_system[] = {
	{0xFE, NULL /* REPLAY/RUN2D1 */, 0, "@RUNMAT",
		"replay", 0x00},
	{0x0A, NULL /* CLIP/NAT_CLIP */, 0, "@REV2",
		"clip", 0x00},
	TTERM};
CASIO_LOCAL const struct type_corresp types_ssheet[] = {
	{0xFE, "@SNAME", 0, "@SSHEET",
		"spreadsheet name?", 0x00},
	{0xFE, NULL, 0, "@SSHEET",
		"spreadsheet", casio_mcstype_spreadsheet},
	TTERM};
CASIO_LOCAL const struct type_corresp types_conics[] = {
	{0xFE, NULL /* "conicmem" */, 0, "@CONICS",
		"conics data", 0x00},
	TTERM};
CASIO_LOCAL const struct type_corresp types_dynamem[] = {
	{0xFE, NULL /* "COND" */, 0, "@DYNA",
		"dyna data", 0x00},
	TTERM};
CASIO_LOCAL const struct type_corresp types_econ3now[] = {
	{0xFE, NULL /* "Econ3Now" */, 0, "@E-CON2",
		"econ3 data", 0x00},
	TTERM};
CASIO_LOCAL const struct type_corresp types_econ4[] = {
	{0xFE, "Econ4N", arg | arg_is_num, "@E-CON2",
		"econ4 data", 0x00},
	TTERM};
CASIO_LOCAL const struct type_corresp types_financial[] = {
	{0xFE, NULL /* "fina_mem" */, 0, "@FINANCE",
		"financial data", 0x00},
	TTERM};
CASIO_LOCAL const struct type_corresp types_recursion[] = {
	{0xFE, NULL /* "RECRG" */, 0, "@RECUR",
		"recursion data", 0x00},
	TTERM};
CASIO_LOCAL const struct type_corresp types_stat[] = {
	{0xFE, "G_CND", 0, "@STAT",
		"stat data?", 0x00},
	{0xFE, "G_CNDEX", 0, "@STAT",
		"stat data?", 0x00},
	{0xFE, "C_CND", 0, "@STAT",
		"stat data?", 0x00},
	{0xFE, "STATap", 0, "@STAT",
		"stat data?", 0x00},
	{0xFE, "STATapX", 0, "@STAT",
		"stat data?", 0x00},
	{0x05, "STATV", arg | arg_is_num, "main",
		"stat data?", 0x00},
	TTERM};
CASIO_LOCAL const struct type_corresp types_table[] = {
	{0xFE, "RANGE", 0, "@TABLE",
		"range data?", 0x00},
	TTERM};
CASIO_LOCAL const struct type_corresp types_3dgraph[] = {
	{0xFE, "AREACOL", 0, "@3DGRAPH",
		"???", 0x00},
	{0xFE, "LINECOL", 0, "@3DGRAPH",
		"???", 0x00},
	{0xFE, "SELECT", 0, "@3DGRAPH",
		"???", 0x00},
	{0xFE, "TEMPLATE", 0, "@3DGRAPH",
		"???", 0x00},
	{0xFE, "SETUP", 0, "@3DGRAPH",
		"???", 0x00},
	{0xFE, "TYPE", 0, "@3DGRAPH",
		"???", 0x00},
	{0xFE, "VWIN", 0, "@3DGRAPH",
		"???", 0x00},
	{0xFE, "Z", arg | arg_is_num /* 1 to 3 */, "@3DGRAPH",
		"???", 0x00},
	TTERM};
CASIO_LOCAL const struct type_corresp types_geom[] = {
	{0xFE, NULL, 0, "@GEOM",
		"???", 0x00},
	TTERM};
CASIO_LOCAL const struct type_corresp types_probsim[] = {
	{0xFE, "ProbRand", 0, "@PROBSIM",
		"???", 0x00},
	{0xFE, "ProbAdv", 0, "@PROBSIM",
		"???", 0x00},
	{0xFE, "ProbSeed", 0, "@PROBSIM",
		"???", 0x00},
	{0xFE, "ProbSet", 0, "@PROBSIM",
		"???", 0x00},
	{0xFE, "ProbVer", 0, "@PROBSIM",
		"???", 0x00},
	TTERM};
CASIO_LOCAL const struct type_corresp types_pictplot[] = {
	{0x05, "PGRAPH", arg | arg_is_num /* 0 to 3 */, "main",
		"???", 0x00},
	{0xFE, "PATH", 0, "@PICTPLT",
		"???", 0x00},
	{0xFE, "@SC_CNT", 0, "@PICTPLT",
		"???", 0x00},
	{0xFE, "@SG_CND", 0, "@PICTPLT",
		"???", 0x00},
	{0xFE, "@SG_CDX", 0, "@PICTPLT",
		"???", 0x00},
	{0xFE, "SETUP", 0, "@PICTPLT",
		"???", 0x00},
	TTERM};

/* And the main tab. */

CASIO_LOCAL const struct group_corresp mcs_groups[] = {
	/* Main Memory */
	{"SETUP",     noarg,                            types_setup},
	{"ALPHA MEM", noarg,                            types_alphamem},
	{"STRING ",   arg | arg_is_num,                 types_string},
	{"LIST ",     arg | arg_is_num,                 types_list},
	{"LISTFILE ", arg | arg_is_num | weight_by_gid, types_listfile},
	{"MAT ",      arg | arg_is_let,                 types_mat},
	{"VCT ",      arg | arg_is_let,                 types_vct},
	{"PROGRAM",   noarg,                            types_program},
	{"Y=DATA",    noarg,                            types_yeqdata},
	{"V-WIN ",    arg | arg_is_num,                 types_vwin},
	{"PICTURE ",  arg | arg_is_num,                 types_picture},

	/* application-specific data */
	{"CAPT ",     arg | arg_is_num,                 types_capt},
	{"SYSTEM",    noarg,                            types_system},
	{"S-SHEET",   noarg,                            types_ssheet},
	{"CONICS",    noarg,                            types_conics},
	{"DYNA MEM",  noarg,                            types_dynamem},
	{"Econ3Now",  noarg,                            types_econ3now},
	{"ECON4_",    arg | arg_is_num,                 types_econ4},
	{"FINANCIAL", noarg,                            types_financial},
	{"RECURSION", noarg,                            types_recursion},
	{"STAT",      noarg,                            types_stat},
	{"TABLE",     noarg,                            types_table},
	{"3DGRAPH",   noarg,                            types_3dgraph},
	{"@GEOM",     noarg,                            types_geom},
	{"@PROBSIM",  noarg,                            types_probsim},
	{"PICTPLOT",  noarg,                            types_pictplot},

	/* terminating entry */
	{NULL, 0, NULL}
};

/* ---
 * Main functions.
 * --- */

/**
 *	get_number:
 *	Get number from string.
 *
 *	@arg	s			the string.
 *	@arg	num			pointer to the num to fill.
 *	@arg	isnum		is it a number or a letter?
 *	@return				if there was an error (0 if ok).
 */

CASIO_LOCAL int get_number(const char *s, int *num, int isnum)
{
	if (!strcmp(s, "\xC0") || !strcmp(s, "Ans"))
		*num = casio_ans;
	else if (!strcmp(s, "\xCE"))
		*num = casio_theta;
	else if (!strcmp(s, "\xCD"))
		*num = casio_r;
	else if (isnum) {
		if (!(*num = atoi(s)))
			return (1);
	} else {
		if (*s < 'A' || *s > 'Z')
			return (1);
		*num = *s - 'A' + 1;
	}
	return (0);
}

/**
 *	casio_correct_mcshead_from_mcs:
 *	Make libcasio abstract data from raw MCS identification data.
 *
 *	@arg	head		the head to correct.
 *	@return				if an error occured.
 */

int CASIO_EXPORT casio_correct_mcshead_from_mcs(casio_mcshead_t *head)
{
	int gid = 0, fid = 0;
	const char *gname = head->casio_mcshead_group;
	const char *fname = head->casio_mcshead_name;
	unsigned int rawtype = head->casio_mcshead_rawtype;
	const struct group_corresp *g;
	const struct type_corresp *t;

	msg((ll_info, "Looking for type with '%.8s' group, '%.8s' name and "
		"0x%02X raw type", gname, fname, rawtype));

	/* Look for group correspondance. */

	for (g = mcs_groups; g->types; g++) {
		size_t pl = strlen(g->name);

		/* Check if our pattern is there. */

		if (strncmp(g->name, gname, pl))
			continue;
		if ((g->flags & arg)
		 && get_number(&gname[pl], &gid, g->flags & arg_is_num))
			continue;

		break;
	}

	if (!g->types)
		goto notfound;

	/* Look for the type correspondance. */

	for (t = g->types; t->dirname; t++) {
		if (t->rawtype != rawtype)
			continue;

		/* Check if we match the pattern. */

		if (t->name) {
			size_t fl = strlen(t->name);

			if (strncmp(t->name, fname, fl))
				continue;

			fid = 0;
			if ((t->flags & arg)
			 && get_number(&fname[fl], &fid, t->flags & arg_is_num))
				continue;
			if (t->flags & weight_by_gid)
				fid |= (gid << 6);
		}
		break;
	}

	if (!t->dirname)
		goto notfound;

	/* Fill in the info and return. */

	head->casio_mcshead_type = t->type;
	head->casio_mcshead_id = fid;

	return (0);

notfound:
	msg((ll_info, "Type with '%s' group, '%s' name and 0x%02x type "
		"wasn't recognized.", gname, fname, rawtype));

	head->casio_mcshead_type = 0;
	head->casio_mcshead_id = 0;

	return (1);
}

/**
 *	casio_correct_mcshead_to_mcs:
 *	Make MCS raw data from the abstract data.
 *
 *	@arg	head		the MCS head to correct.
 *	@return				if an error was encountered.
 */

int CASIO_EXPORT casio_correct_mcshead_to_mcs(casio_mcshead_t *head)
{
	const struct group_corresp *g;
	const struct type_corresp *t;
	char *gr, *nm;

	/* Check if there is a type */

	if (!head)
		return (0);
	if (!head->casio_mcshead_type && (!head->casio_mcshead_group[0]
	 || !head->casio_mcshead_name[0] || !head->casio_mcshead_dirname[0]))
		return (casio_error_op);

	/* Find the group/type. */

	for (g = mcs_groups; g->types; g++) for (t = g->types; t->dirname; t++) {
		/* Check if the libcasio type corresponds. */

		if (t->type != head->casio_mcshead_type)
			continue;

		/* Check if id is weighted by major. */

		if (casio_get_id_major(head->casio_mcshead_id)
		 && ~t->flags & weight_by_gid)
			continue;

		/* We have found the entry! */

		goto found;
	}

	/* Not found… */

	return (casio_error_op);

found:
	/* Put the group name. */

	gr = (char*)head->casio_mcshead_group;
	if (g->flags & arg) {
		int grid = casio_get_id_major(head->casio_mcshead_id);

		if ((t->flags & arg) && ~t->flags & weight_by_gid)
			grid = casio_get_id_minor(head->casio_mcshead_id);
		if (grid == casio_ans)
			sprintf(gr, "%sAns", g->name);
		else if (grid == casio_theta)
			sprintf(gr, "%s\xCE", g->name);
		else if (grid == casio_r)
			sprintf(gr, "%s\xCD", g->name);
		else if (g->flags & arg_is_num)
			sprintf(gr, "%s%d", g->name, grid);
		else
			sprintf(gr, "%s%c", g->name, grid + 'A' - 1);
	} else
		strcpy(gr, g->name);

	/* Put the directory name. */

	strcpy((char*)head->casio_mcshead_dirname, t->dirname);

	/* Put the filename. */

	nm = (char*)head->casio_mcshead_name;
	if (t->flags & arg) {
		int namid = casio_get_id_minor(head->casio_mcshead_id);

		if (namid == casio_ans)
			sprintf(nm, "%sAns", t->name);
		else if (namid == casio_theta)
			sprintf(nm, "%s\xCE", t->name);
		else if (namid == casio_r)
			sprintf(nm, "%s\xCD", t->name);
		else if (t->flags & arg_is_num)
			sprintf(nm, "%s%d", t->name, namid);
		else
			sprintf(nm, "%s%c", t->name, namid + 'A' - 1);
	} else if (t->name)
		strcpy(nm, t->name);

	/* Put the raw type. */

	head->casio_mcshead_rawtype = t->rawtype;

	/* No error. */

	return (0);
}

/* ---
 * Compare functions.
 * --- */

/**
 *	find_offset_in_group:
 *	Find offset in a group.
 *
 *	@arg	group	the group correspondance.
 *	@arg	head	the head.
 *	@return			the offset (-1 if not there).
 */

CASIO_LOCAL int find_offset_in_group(const struct group_corresp *g,
	const casio_mcshead_t *head)
{
	int i; const struct type_corresp *f;

	/* group check */
	if (strcmp((char*)head->casio_mcshead_group, g->name))
		return (-1);

	/* file check */
	for (f = g->types, i = 0; f->info; f++, i++) {
		if (strcmp((char*)head->casio_mcshead_group, f->name)
		 || head->casio_mcshead_rawtype != f->rawtype)
			continue;

		return (i);
	}

	/* not found, but shouldn't look for it in other groups */
	return (-2);
}

/**
 *	casio_compare_mcsfiles:
 *	Compare MCS files using their heads, for sorting.
 *
 *	@arg	first	the first head.
 *	@arg	second	the second head.
 *	@return			negative value if the 1st is considered less than the 2nd,
 *					positive value otherwise.
 */

int CASIO_EXPORT casio_compare_mcsfiles(casio_mcshead_t *first,
	casio_mcshead_t *second)
{
	int offset1 = -1, offset2 = -1;
	const struct group_corresp *g;

	for (g = mcs_groups; g->types; g++) {
		/* get offsets */
		if (offset1 >= -1)
			offset1 = find_offset_in_group(g, first);
		if (offset2 >= -1)
			offset2 = find_offset_in_group(g, second);

		/* check if the first one corresponds */
		if (offset1 < 0 && offset2 < 0)
			continue;
		if (offset1 == offset2)
			break;
		if (offset1 < offset2)
			return (-1);
		if (offset1 > offset2)
			return (1);
	}

	/* so they're equal, huh... */
	return (0);
}
