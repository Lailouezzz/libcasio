/* ****************************************************************************
 * file/manage.c -- create, free a handle.
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
#include "file.h"
#include <ctype.h>
#define mkhandle() \
	if (!(*(h) = malloc(sizeof(casio_file_t)))) return (casio_error_alloc); \
	handle = *h; memset(handle, 0, sizeof(casio_file_t))

/* ************************************************************************* */
/*  Make a handle                                                            */
/* ************************************************************************* */
/**
 *	casio_make_picture:
 *	Make a picture handle.
 *
 *	@arg	h			the file handle to make.
 *	@arg	width		the picture width.
 *	@arg	height		the picture height.
 *	@return				the error code (0 if ok).
 */

int casio_make_picture(casio_file_t **h,
	unsigned int width, unsigned int height)
{
	casio_file_t *handle;

	/* make the handle */
	mkhandle();
	handle->casio_file_type = casio_filetype_picture;
	handle->casio_file_width = width;
	handle->casio_file_height = height;
	handle->casio_file_pixels = alloc_pixels(width, height);
	if (!handle->casio_file_pixels) {
		free(*h); *h = NULL; return (casio_error_alloc); }
	prepare_pixels(handle->casio_file_pixels, width, height)

	/* everything went well! */
	return (0);
}

/**
 *	casio_make_mcs:
 *	Make an MCS file.
 *
 *	@arg	h		pointer to the handle to create.
 *	@arg	count	the number of slots in the index.
 *	@return			the error code (0 if ok).
 */

int casio_make_mcs(casio_file_t **h, int count)
{
	casio_file_t *handle;

	/* make the handle */
	mkhandle();
	handle->casio_file_type = casio_filetype_mcs;
	handle->casio_file_for = casio_filefor_fx;
	handle->casio_file_count = 0;
	handle->casio_file__size = 0;
	handle->casio_file_mcsfiles = NULL;

	/* allocate space */
	if (count) {
		handle->casio_file_mcsfiles =
			malloc(sizeof(casio_mcsfile_t*) * count);
		if (!handle->casio_file_mcsfiles) goto fail;
		memset(handle->casio_file_mcsfiles, 0,
			sizeof(casio_mcsfile_t*) * count);
		handle->casio_file__size = count;
	}

	/* no error */
	return (0);
fail:
	casio_free_file(*h); *h = NULL;
	return (casio_error_alloc);
}

/**
 *	casio_make_fkey:
 *	Make a Function Keys handle.
 *
 *	@arg	h		pointer to the handle to create.
 *	@arg	pf		the platform.
 *	@arg	count	the number of slots in the index.
 *	@return			the error code (0 if ok).
 */

int casio_make_fkey(casio_file_t **h, casio_filefor_t pf, int count)
{
	casio_file_t *handle;

	/* make the handle */
	mkhandle();
	handle->casio_file_type = casio_filetype_fkey;
	handle->casio_file_for = pf;
	handle->casio_file_count = 0;
	handle->casio_file__size = 0;
	handle->casio_file_fkeys = NULL;

	/* allocate index */
	if (count) {
		handle->casio_file_fkeys = malloc(sizeof(uint32_t**) * count);
		if (!handle->casio_file_fkeys) goto fail;
		memset(handle->casio_file_fkeys, 0, sizeof(uint32_t**) * count);
		handle->casio_file__size = count;
	}

	/* no error */
	return (0);
fail:
	casio_free_file(*h); *h = NULL;
	return (casio_error_alloc);
}

/**
 *	casio_make_lang:
 *	Make a language handle.
 *
 *	@arg	h			pointer to the handle to create.
 *	@arg	platform	the platform.
 *	@arg	count		the number of slots in the index.
 *	@return				the error code (0 if ok).
 */

int casio_make_lang(casio_file_t **h, casio_filefor_t platform, int count)
{
	casio_file_t *handle;

	/* make the handle */
	mkhandle();
	handle->casio_file_type = casio_filetype_lang;
	handle->casio_file_for = platform;
	handle->casio_file_count = 0;
	handle->casio_file__size = 0;
	handle->casio_file_messages = NULL;

	/* allocate index */
	if (count) {
		handle->casio_file_messages = malloc(sizeof(char*) * count);
		if (!handle->casio_file_messages) goto fail;
		memset(handle->casio_file_messages, 0, sizeof(char*) * count);
		handle->casio_file__size = count;
	}

	/* no error */
	return (0);
fail:
	casio_free_file(*h); *h = NULL;
	return (casio_error_alloc);
}

/**
 *	casio_make_addin:
 *	Make an add-in handle.
 *
 *	@arg	h			pointer to the handle to create.
 *	@arg	platform	the platform for which to make the add-in.
 *	@arg	size		the code size.
 *	@arg	name		the name.
 *	@arg	internal	the internal name.
 *	@arg	version		the version of the add-in.
 *	@arg	created		the creation date of the add-in.
 *	@return				the error code (0 if ok).
 */

int casio_make_addin(casio_file_t **h, casio_filefor_t platform, size_t size,
	const char *name, const char *internal,
	const casio_version_t *version, const time_t *created)
{
	casio_file_t *handle;
	unsigned int width, height;
	int titlesize, intsize, i;

	/* make checks */
	if (platform != casio_filefor_fx && platform != casio_filefor_cg
	 && platform != casio_filefor_cp)
		return (casio_error_op);
	if (platform == casio_filefor_fx && size > 512 * 1024)
		return (casio_error_op);
	if (!isupper(name[0]) || internal[0] != '@' || !isupper(internal[1]))
		return (casio_error_op);

	/* make the handle */
	mkhandle();
	handle->casio_file_type = casio_filetype_addin;
	handle->casio_file_for = platform;
	handle->casio_file_version = *version;
	handle->casio_file_creation_date = *created;
	handle->casio_file_size = size;

	/* allocate the content */
	handle->casio_file_content = malloc(size);
	if (!handle->casio_file_content) goto fail;

	/* check the platform */
	titlesize = 8;
	intsize = 8;
	switch (platform) {
	case casio_filefor_fx:
		width = G1A_ICON_WIDTH; height = G1A_ICON_HEIGHT;
		break;
	case casio_filefor_cp:
		width = C1A_ICON_WIDTH; height = C1A_ICON_HEIGHT;
		titlesize = 16; break;
	default: /* case casio_filefor_cg: */
		width = G3A_ICON_WIDTH;
		height = G3A_ICON_HEIGHT;
		titlesize = 16; break;
	}

	/* copy the addin title */
	for (i = 0; (isupper(name[i]) || isdigit(name[i]))
	  && i < titlesize; i++)
		handle->casio_file_title[i] = name[i];
	handle->casio_file_title[i] = 0;
	if (!handle->casio_file_title[0])
		strcpy(handle->casio_file_title, "@ADDIN");

	/* copy the internal name */
	handle->casio_file_intname[0] = '@';
	for (i = 1; (isupper(internal[i]) || isdigit(internal[i]))
	  && i < intsize; i++)
		handle->casio_file_intname[i] = internal[i];
	handle->casio_file_intname[i] = 0;

	/* allocate pictures */
	handle->casio_file_icon_unsel = alloc_pixels(width, height);
	if (!handle->casio_file_icon_unsel) goto fail;
	handle->casio_file_icon_sel = alloc_pixels(width, height);
	if (!handle->casio_file_icon_sel) goto fail;

	/* prepare pictures */
	handle->casio_file_width = width;
	handle->casio_file_height = height;
	prepare_pixels(handle->casio_file_icon_unsel, width, height)
	prepare_pixels(handle->casio_file_icon_sel, width, height)

	/* end my suffering */
	return (0);
fail:
	casio_free_file(*h); *h = NULL;
	return (casio_error_alloc);
}
/* ************************************************************************* */
/*  Free a handle                                                            */
/* ************************************************************************* */
/**
 *	casio_free_file:
 *	Free a handle and its data.
 *
 *	@arg	handle		the handle.
 */

void casio_free_file(casio_file_t *handle)
{
	int i;

	/* check if there is something to free */
	if (!handle) return ;

	/* addin time! */
	if (handle->casio_file_type & casio_filetype_addin) {
		free(handle->casio_file_content);
		free(handle->casio_file_icon_unsel);
		free(handle->casio_file_icon_sel);
	}

	/* mcs time! */
	if (handle->casio_file_type & casio_filetype_mcs) {
		casio_mcsfile_t **files = handle->casio_file_mcsfiles;

		/* check if mcs */
		if (!files) return ;

		/* foreach file in mcs */
		for (i = handle->casio_file_count - 1; i >= 0; i--) {
			/* free the file if exists */
			if (files[i]) casio_free_mcsfile(files[i]);
		}
		free(handle->casio_file_mcsfiles);
		handle->casio_file_mcsfiles = NULL;
	}

	/* messages time! */
	if (handle->casio_file_type & casio_filetype_lang
	 && handle->casio_file_messages) {
		for (i = 0; i < handle->casio_file_count; i++)
			free(handle->casio_file_messages[i]);
		free(handle->casio_file_messages);
	}

	/* function keys time! */
	if (handle->casio_file_type & casio_filetype_fkey
	 && handle->casio_file_fkeys) {
		for (i = 0; i < handle->casio_file_count; i++)
			free(handle->casio_file_fkeys[i]);
		free(handle->casio_file_fkeys);
	}

	/* picture time! */
	if (handle->casio_file_type & casio_filetype_picture)
		free(handle->casio_file_pixels);

	/* e-activities time! */
/*	if (handle->casio_file_type & casio_filetype_eact)
		casio_free_line_content(handle->casio_file_line); */

	/* free the handle itself! */
	free(handle);
}
