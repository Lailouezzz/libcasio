/* ****************************************************************************
 * link/usage/getscreen.c -- getting the screen.
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
#include "usage.h"
#define failure(CASIO__ERR) { err = casio_error_ ## CASIO__ERR; goto fail; }
#define WIDTH  CASIO_SEVEN_MAX_VRAM_WIDTH
#define HEIGHT CASIO_SEVEN_MAX_VRAM_HEIGHT

/**
 *	casio_getscreen:
 *	Get the screen.
 *
 *	@arg	handle		the link handle.
 *	@arg	callback	the main callback for the function.
 *	@arg	scookie		the callback cookie.
 *	@return				if it worked.
 */

int CASIO_EXPORT casio_getscreen(casio_link_t *handle,
	casio_link_screen_t *callback, void *cbcookie)
{
	int err = 0, y;
	uint32_t **pixels = NULL, *base;
	unsigned int width, height;

	/* make checks. */
	chk_handle(handle);
	chk_seven(handle); /* TODO: SCSI? */
	chk_passive(handle);

	/* allocate pixels. */
	pixels = malloc(sizeof(uint32_t*) * HEIGHT
		+ sizeof(uint32_t) * WIDTH * HEIGHT);
	if (!pixels) failure(alloc);

	/* prepare pixels. */
	base = (casio_pixel_t*)(pixels + HEIGHT);
	for (y = HEIGHT - 1; y >= 0; y--)
		pixels[y] = base + y * WIDTH;

	/* main loop */
	while (1) {
		/* get packet */
		if ((err = casio_seven_receive(handle, 0)))
			goto fail;
		if (response.casio_seven_packet_type != casio_seven_type_ohp)
			failure(unknown);

		/* convert */
		width  = response.casio_seven_packet_width;
		height = response.casio_seven_packet_height;
		casio_decode_picture(pixels,
			response.casio_seven_packet_vram,
			response.casio_seven_packet_pictype,
			width, height);
		(*callback)(cbcookie, width, height, pixels);
	}

	err = 0;
fail:
	free(pixels);
	return (0);
}
