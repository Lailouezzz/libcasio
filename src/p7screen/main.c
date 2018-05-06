/* *****************************************************************************
 * p7screen/main.c -- p7screen main source.
 * Copyright (C) 2016-2017 Thomas "Cakeisalie5" Touhey <thomas@touhey.fr>
 *
 * This file is part of p7utils.
 * p7utils is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2.0 of the License,
 * or (at your option) any later version.
 *
 * p7utils is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with p7utils; if not, see <http://www.gnu.org/licenses/>.
 * ************************************************************************** */
#include "main.h"
#include <SDL.h>

/* ---
 * Error messages.
 * --- */

/* Couldn't initialize connexion to calculator. */

static const char error_noconnexion[] =
"Could not connect to the calculator.\n"
"- Is it plugged in and in PROJ mode?\n"
"- Have you tried unplugging, plugging and selecting Projector on pop-up?\n"
"- Have you tried changing the cable?\n";

/* Calculator was found but program wasn't allowed to communicate with it. */

static const char error_noaccess[] =
"Could not get access to the calculator.\n"
"Install the appropriate udev rule, or run as root.\n";

/* The calculator acted in a weird way. */

static const char error_unplanned[] =
"The calculator didn't act as planned.\n"
"Stop receive mode on calculator and start it again before re-running " \
	BIN ".\n"
"Error was: %s\n";

/* ---
 * Globals.
 * --- */

/* The z00m (omG). */

static unsigned int zoom;

/* ---
 * Main functions.
 * --- */

/**
 *	display_screen:
 *	Display the screen.
 *
 *	@arg	link	the link handle to use.
 *	@return			the main code.
 */

static int display_screen(casio_link_t *link)
{
	SDL_Window *window = NULL;
	SDL_Renderer *rendr = NULL;
	SDL_Texture *texture = NULL;
	int ret = 1, err;
	unsigned int w, saved_w, h, saved_h;
	casio_screen_t *screen = NULL;
	casio_pixel_t **pixels;

	while (1) {
		/* Get the screen. */

		switch ((err = casio_get_screen(link, &screen))) {
		case 0:
			break;
		case casio_error_nocalc:
			goto end;
		case casio_error_timeout:
			fprintf(stderr, error_noconnexion);
			goto fail;
		default:
			fprintf(stderr, error_unplanned, casio_strerror(err));
			goto fail;
		}

		w = screen->casio_screen_width;
		h = screen->casio_screen_height;
		pixels = screen->casio_screen_pixels;

		if (!window) {
			/* We haven't got a window, our objective is to create one,
			 * with a renderer and a texture. First, let's create the
			 * window. */

			window = SDL_CreateWindow("p7screen",
				SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
				w * zoom, h * zoom, 0);
			if (!window) {
				fprintf(stderr, "Couldn't create the window: %s\n",
					SDL_GetError());
				goto fail;
			}

			/* Then let's create the renderer. */

			rendr = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
			if (!rendr) {
				fprintf(stderr, "Couldn't create the renderer: %s\n",
					SDL_GetError());
				goto fail;
			}

			/* Finally, create the texture we're gonna use for drawing
			 * the picture as a classic ARGB pixel matric (8 bits per
			 * component). */

			texture = SDL_CreateTexture(rendr, SDL_PIXELFORMAT_ARGB8888,
				SDL_TEXTUREACCESS_STREAMING, w * zoom, h * zoom);
			if (!texture) {
				fprintf(stderr, "Couldn't create the texture: %s\n",
					SDL_GetError());
				goto fail;
			}

			/* Save data and display message. */

			saved_w = w;
			saved_h = h;

			puts("Turn off your calculator (SHIFT+AC) "
				"when you have finished.");
		} else if (saved_w != w || saved_h != h) {
			/* The dimensions have changed somehow, we're gonna manage it!
			 * FIXME: one day. */

			fprintf(stderr, "Unmanaged dimensions changed.\n");
			goto fail;
		}

		/* Copy the data. */

		{
			Uint32 *px;
			int pitch, linesize = w * zoom;
			unsigned int x, y, zx, zy;

			SDL_LockTexture(texture, NULL, (void **)&px, &pitch);

			for (y = 0; y < h; y++) {
				Uint32 *refline = px;

				for (x = 0; x < w; x++) {
					Uint32 pixel = pixels[y][x];

					for (zx = 0; zx < zoom; zx++)
						*px++ = pixel;
				}
				for (zy = 1; zy < zoom; zy++) {
					memcpy(px, refline, linesize * sizeof(uint32_t));
					px += linesize;
				}
			}

			SDL_UnlockTexture(texture);
		}

		/* Flippin' flip the screen! */

		SDL_RenderCopy(rendr, texture, NULL, NULL);
		SDL_RenderPresent(rendr);
	}

end:
	ret = 0;
fail:
	casio_free_screen(screen);
	if (texture)
		SDL_DestroyTexture(texture);
	if (rendr)
		SDL_DestroyRenderer(rendr);
	if (window)
		SDL_DestroyWindow(window);
	return (ret);
}

/**
 *	main:
 *	Entry point of the program.
 *
 *	@arg	ac		arguments count
 *	@arg	av		arguments values
 *	@return			if it worked (0 if OK)
 */

int main(int ac, char **av)
{
	int err; casio_link_t *handle = NULL;

	/* Parse the arguments. */

	if (parse_args(ac, av, &zoom))
		return (0);

	/* Make the libcasio link handle. */

	if ((err = casio_open_usb(&handle, 0, -1, -1))) {
		switch (err) {
			case casio_error_nocalc:
				fprintf(stderr, error_noconnexion);
				break;
			case casio_error_noaccess:
				fprintf(stderr, error_noaccess);
				break;
			default:
				fprintf(stderr, error_unplanned, casio_strerror(err));
				break;
		}

		/* return */
		return (1);
	}

	/* Initialize the SDL. */

	if (SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "Failed to initialize SDL: %s\n", SDL_GetError());
		return (3);
	}
	atexit(SDL_Quit);

	/* Display the screen. */

	display_screen(handle);

	/* Close the link and exit. */

	casio_close_link(handle);
	return (0);
}
