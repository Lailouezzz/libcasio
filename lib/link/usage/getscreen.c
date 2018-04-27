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

/* ---
 * Screen management.
 * --- */

int CASIO_EXPORT casio_make_screen(casio_screen_t **screenp,
	unsigned int width, unsigned int height)
{
	casio_screen_t *screen;
	casio_pixel_t **pixels, *base;
	int y;

	/* Allocate and prepare the screen. */

	screen = casio_alloc(sizeof(casio_screen_t)
		+ sizeof(casio_pixel_t*) * width
		+ sizeof(casio_pixel_t) * width * height, 1);
	if (!screen)
		return (casio_error_alloc);

	screen->casio_screen_width = width;
	screen->casio_screen_realwidth = width;
	screen->casio_screen_height = height;
	screen->casio_screen_realheight = height;
	screen->casio_screen_pixels = (casio_pixel_t **)&screen[1];

	/* Prepare the pixels. */

	pixels = screen->casio_screen_pixels;
	base = (casio_pixel_t*)(&pixels[height]);
	for (y = height - 1; y >= 0; y--)
		pixels[y] = base + y * width;

	*screenp = screen;
	return (0);
}

int CASIO_EXPORT casio_free_screen(casio_screen_t *screen)
{
	casio_free(screen);
	return (0);
}

/* ---
 * Gather the screen.
 * --- */

/**
 *	casio_get_screen:
 *	Get the screen.
 *
 *	@arg	handle		the link handle.
 *	@arg	screen		the screen to allocate/reallocate/reuse.
 *	@return				if it worked.
 */

int CASIO_EXPORT casio_get_screen(casio_link_t *handle,
	casio_screen_t **screenp)
{
	int err = 0;
	casio_screen_t *screen;

	/* Make checks. */

	chk_handle(handle);
	chk_seven(handle); /* TODO: SCSI? */
	chk_passive(handle);

	/* Prepare the screen. */

	screen = *screenp;
	if (!(screen && screen->casio_screen_realwidth == WIDTH
	 && screen->casio_screen_realheight)) {
		/* Allocate the thing first, so that if we fail, the screen is
		 * still there to be used by the user, in case. */

		if ((err = casio_make_screen(&screen, WIDTH, HEIGHT)))
			return (err);

		if (*screenp)
			casio_free_screen(*screenp);
		*screenp = screen;
	}

	/* Get the screen packet. */

	if ((err = casio_seven_receive(handle, CASIO_SEVEN_RECEIVEFLAG_SCRALIGN)))
		return (err);
	if (response.casio_seven_packet_type != casio_seven_type_ohp)
		return (casio_error_unknown);

	/* Convert the screen buffer. */

	screen->casio_screen_width  = response.casio_seven_packet_width;
	screen->casio_screen_height = response.casio_seven_packet_height;
	casio_decode_picture(screen->casio_screen_pixels,
		response.casio_seven_packet_vram,
		response.casio_seven_packet_pictype,
		screen->casio_screen_width,
		screen->casio_screen_height);

	return (0);
}
