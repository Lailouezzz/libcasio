/* ****************************************************************************
 * mcsfile/corresp/cas_data.c -- get the CAS type out of raw id. data.
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
#include "../mcsfile.h"

/* ************************************************************************* */
/*  Local types                                                              */
/* ************************************************************************* */
/* Flags */
#define mult       0x0001
#define noarg      0x0000
#define arg        0x0002
#define arg_is_num 0x0000

/* Correspondance type */
struct type_corresp {
	/* identification */
	const char *datatype;
	unsigned int flags;

	/* libcasio MCS file type, flags */
	casio_mcstype_t type;
	casio_pictureformat_t picformat;
};

/* ************************************************************************* */
/*  Correspondances                                                          */
/* ************************************************************************* */
/* All correspondances found by Tom Wheeley, Tom Lynn (creators of CaS),
 * and Göran Weinholt (creator of Cafix). */

#define BASIC(S, T)      {S,    noarg, T,                     0}
#define CAPT(S, PT)      {S,    noarg, casio_mcstype_capture, PT}
#define UNIMPLEMENTED(S) {S,    noarg, 0,                     0}
#define TTERM            {NULL, noarg, 0,                     0}
CASIO_LOCAL const struct type_corresp cas_groups[] = {
	/* basic things */
	BASIC("LT",       casio_mcstype_list),
	BASIC("MT",       casio_mcstype_matrix),
	BASIC("PG",       casio_mcstype_program),
	BASIC("VM",       casio_mcstype_variable),
	BASIC("\xFF\xFF", casio_mcstype_end),

	/* programs */
	{"P", arg | arg_is_num, casio_mcstype_program, 0},
	{"PZ", mult | noarg, casio_mcstype_program, 0}, /* 38 progs: 0-9,A-Z,... */

	/* captures */
	CAPT("DC", casio_pictureformat_4bit_color),
	CAPT("DD", casio_pictureformat_4bit_mono),

	/* decoding is being implemented */
	UNIMPLEMENTED("GF"), /* graph zoom factor (graph function?) */
	UNIMPLEMENTED("AA"), /* dynamic graph functions */
	UNIMPLEMENTED("BU"), /* backup */

	/* not implemented yet */
	UNIMPLEMENTED("AD"), /* variable memory */
	UNIMPLEMENTED("AL"), /* all */
	UNIMPLEMENTED("AM"), /* alpha variable memory */
	UNIMPLEMENTED("DM"), /* defined memory */
	UNIMPLEMENTED("EN"), /* one editor file */
	UNIMPLEMENTED("FT"), /* ??? (cafix) */
	UNIMPLEMENTED("F1"), /* one function memory */
	UNIMPLEMENTED("GM"), /* gmem (cafix) */
	UNIMPLEMENTED("GR"), /* graph range */
	UNIMPLEMENTED("GT"), /* function table */
	UNIMPLEMENTED("PC"), /* picture from 9xxx (cafix) */
	UNIMPLEMENTED("PD"), /* polynomial equations */
	UNIMPLEMENTED("RF"), /* ??? (cafix) */
	UNIMPLEMENTED("RR"), /* ??? (cafix) */
	UNIMPLEMENTED("RT"), /* recursion table */
	UNIMPLEMENTED("SD"), /* simultaneous equations */
	UNIMPLEMENTED("SE"), /* equation (cafix) */
	UNIMPLEMENTED("SR"), /* linear regression data */
	UNIMPLEMENTED("SS"), /* standard deviation data */
	UNIMPLEMENTED("TR"), /* ??? (cafix) */
	UNIMPLEMENTED("WD"), /* window data (cafix) */

	/* sets */
	UNIMPLEMENTED("FN"), /* set of editor files */
	UNIMPLEMENTED("F6"), /* set of function memories */
	UNIMPLEMENTED("GA"), /* set of graph functions */
	UNIMPLEMENTED("MA"), /* set of matrices */

	/* terminating entry */
	TTERM
};

/* ************************************************************************* */
/*  Main functions                                                           */
/* ************************************************************************* */
/**
 *	get_number:
 *	Get number from string.
 *
 *	Adapted from `type/mcs.c`. If you need letter ID decoding or other
 *	stuff, go and see there.
 *
 *	@arg	s			the string.
 *	@arg	num			pointer to the num to fill.
 *	@return				if there was an error (0 if ok).
 */

CASIO_LOCAL int get_number(const char *s, int *num)
{
	if (!(*num = atoi(s))) return (1);
	return (0);
}

/**
 *	casio_maketype_cas:
 *	Get the libcasio MCS type from the raw CAS40/CAS50 data type.
 *
 *	@arg	head		the head to fill.
 *	@arg	datatype	the data type string (2 bytes long).
 *	@return				the error (if any).
 */

int CASIO_EXPORT casio_maketype_cas(casio_mcshead_t *head,
	const char *datatype)
{
	const struct type_corresp *c;
	int id = 0;

	/* copy information */
	memcpy(head->casio_mcshead_datatype, datatype, 2);
	head->casio_mcshead_datatype[2] = 0;

	/* look for correspondance */
	for (c = cas_groups; c->type; c++) {
		size_t pl = strlen(c->datatype);

		/* check if pattern is there */
		if (strncmp(c->datatype, datatype, pl))
			continue;
		if ((c->flags & arg) && get_number(&head->casio_mcshead_name[pl], &id))
			continue;
		break;
	}
	if (!c->type) goto notfound;

	/* fill in info and return */
	head->casio_mcshead_type = c->type;
	head->casio_mcshead_id = id;
	head->casio_mcshead_picformat = c->picformat;
	if (c->flags & mult)
		head->casio_mcshead_flags |= casio_mcsflag_multiple;
	return (0);

notfound:
	msg((ll_info, "Type with '%.2s' data string was not implemented or not "
		"recognized.", datatype));
	head->casio_mcshead_type = 0;
	return (1);
}

/**
 *	casio_correct_casfile_head:
 *	Correct a CAS file head.
 *
 *	@arg	head		the head to correct.
 *	@return				the error (0 if ok).
 */

int CASIO_EXPORT casio_correct_casfile_head(casio_mcshead_t *head)
{
	const struct type_corresp *c;

	/* look for correspondance */
	for (c = cas_groups; c->type; c++) {
		if (head->casio_mcshead_type == c->type) /* FIXME: picture format? */
			goto found;
		break;
	}

	/* not found :( */
	return (casio_error_unknown);

	/* found :) */
found:
	memcpy(head->casio_mcshead_datatype, c->datatype, 2);
	/* TODO: things with IDs, etc */
	/* TODO: add app? */
	return (0);
}
