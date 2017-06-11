/* ****************************************************************************
 * mcsfile/corresp/cas_app.c -- get the CAS app out of raw identification data.
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
/* Correspondance type */
struct app_corresp {
	/* identification */
	const char *name;

	/* things: nothing yet */
};

/* Extension */
struct ext_corresp {
	/* identification */
	int value;

	/* things */
	casio_mcsinfo_t info;

	/* app correspondances */
	const struct app_corresp *apps;
};
/* ************************************************************************* */
/*  Correspondances                                                          */
/* ************************************************************************* */
/* App correspondances. Remarks:
 * - Correspondances with a NULL data type means the data type isn't to be
 *   read. */

static const struct app_corresp apps_9850[] = {
	{"TXT"}, /* editor */
	{"VAL"}, /* RUN? */
	{"IMG"}, /* screenshooter? */
	{NULL}};
static const struct app_corresp apps_end[] = {
	{"END"}, /* end packet... used with 9850 packets? */
	{NULL}};
static const struct app_corresp apps_g100[] = {
	{"MDL"}, /* Model, initialize */
	{"END"}, /* Terminate */
	{"REQ"}, /* Receive */
	{"ADN"}, /* Send */

	{"FMV"}, /* ? */
	{"FCL"}, /* ? */
	{"MCS"}, /* ? */
	{NULL}};
static const struct app_corresp apps_g100b[] = {
	{"ADN"}, /* Send (bis) */
	{"REQ"}, /* Request (bis) */
	{NULL}};

static const struct ext_corresp apps[] = {
	/* CFX-9850G headers (very first headers with this extension system?) */
	{casio_casdyn_9850,  casio_mcsinfo_cas50,  apps_9850},
	/* CFX-9850G header alias, used with the END packet only */
	{casio_casdyn_end,   casio_mcsinfo_cas50,  apps_end},
	/* Graph 100 (Algebra FX) headers */
	{casio_casdyn_g100,  casio_mcsinfo_cas100, apps_g100},
	/* Graph 100 (Algebra FX) alternate headers */
	{casio_casdyn_g100b, casio_mcsinfo_cas100, apps_g100b},
	/* (sentinel) */
	{0,                  0,                    NULL}
};
/* ************************************************************************* */
/*  Main functions                                                           */
/* ************************************************************************* */
/**
 *	casio_maketype_casapp:
 *	Get the CAS application from raw CASDYN identification data.
 *
 *	@arg	head		the head to fill.
 *	@arg	ext			the extension value.
 *	@arg	app			the application name.
 *	@return				the error (if any).
 */

int casio_maketype_casapp(casio_mcshead_t *head,
	int ext, const char *app)
{
	const struct ext_corresp *e;
	const struct app_corresp *a;

	/* look for the extension */
	for (e = apps; e->apps; e++) {
		if (e->value == ext)
			break;
	} if (!e->apps)
		goto notfound;

	/* look for the application */
	for (a = e->apps; a->name; a++) {
		if (!strncmp(a->name, app, 3))
			break;
	} if (!a->name) goto notfound;

	/* copy raw information */
	memset(head, 0, sizeof(casio_mcshead_t));
	head->casio_mcshead_info = e->info; /* FIXME */
	msg((ll_info, "Head info is 0x%04X", e->info));
	strncpy((char*)head->casio_mcshead_appname, app, 3);
	head->casio_mcshead_appname[3] = 0;

	/* fill in info and return */
	return (0);

notfound:
	msg((ll_error, "Type with 0x%02X extension and '%.3s' app name was not "
		"implemented or recognized", ext, app));
	head->casio_mcshead_type = 0;
	return (1);
}
