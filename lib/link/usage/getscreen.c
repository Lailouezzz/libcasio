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
 * Gather the screen on a legacy fx device, or using legacy fx-CG formats.
 * This also gathers a screen from the Prizm with protocol 7.00 compatibility.
 * --- */

/* Legacy screenstreaming uses tweaks in Protocol 7.00 (managed in the
 * libcasio Protocol 7.00 link core code) to transmit a continuous feed
 * of images, not expecting the other party to answer (so we're not answering).
 *
 * In fact, not continuous: the calculator only sends a frame when it feels
 * like it, usually when the screen changes. Also, as the calculator doesn't
 * wait for us to send screens (it starts sending screens as soon as it is
 * on), it might be in the middle of sending a screen when we're joining in.
 * Fortunately, there is a "screen alignment" function in the packet receiving
 * function that we can use, that looks for the packet beginning. It may fail
 * sometimes, and get a weird screen, but it does its best. */

/**
 *	casio_get_seven_screen:
 *	Get the screen through protocol 7.00.
 */

CASIO_LOCAL int casio_get_seven_screen(casio_link_t *handle,
	casio_screen_t **screenp)
{
	int err = 0;
	casio_screen_t *screen;

	/* Make checks. */

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

	/* Convert the screen buffer and return. */

	screen->casio_screen_width  = response.casio_seven_packet_width;
	screen->casio_screen_height = response.casio_seven_packet_height;
	casio_decode_picture(screen->casio_screen_pixels,
		response.casio_seven_packet_vram,
		response.casio_seven_packet_pictype,
		screen->casio_screen_width,
		screen->casio_screen_height);

	return (0);
}

/* ---
 * Public function.
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

	chk_handle(handle);
	chk_seven(handle); /* TODO: SCSI? */

	return (casio_get_seven_screen(handle, screenp));
}
