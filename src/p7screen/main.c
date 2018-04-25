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

/* ************************************************************************** */
/*  Error messages                                                            */
/* ************************************************************************** */
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

/* ************************************************************************** */
/*  Globals                                                                   */
/* ************************************************************************** */
/* The z00m (omG) */
static int zoom;

/* ************************************************************************** */
/*  Auxiliary functions                                                       */
/* ************************************************************************** */
/**
 *	display_callback:
 *	The main callback for screen streaming.
 *
 *	@arg	vcookie		the cookie (unused).
 *	@arg	w			the width of the received image
 *	@arg	h			the height of the received image
 *	@arg	pixels		the image data
 */

static void display_callback(void *vcookie,
	int w, int h, casio_uint32_t **pixels)
{
	static SDL_Window *window = NULL;
	static SDL_Renderer *rendr = NULL;
	static SDL_Texture *texture = NULL;
	static int saved_w = 0, saved_h = 0;

	(void)vcookie;

	if (!window) {
		int ret;

		/* We haven't got a window, our objective is to create one,
		 * with a renderer and a texture. First, let's create the window. */

		window = SDL_CreateWindow("p7screen",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			w * zoom, h * zoom, 0);
		if (!window) {
			fprintf(stderr, "Couldn't create the window: %s\n",
				SDL_GetError());
			return ;
		}

		/* Then let's create the renderer. */

		rendr = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
		if (!rendr) {
			SDL_DestroyWindow(window);
			window = NULL;

			fprintf(stderr, "Couldn't create the renderer: %s\n",
				SDL_GetError());
			return ;
		}

		/* Finally, create the texture we're gonna use for drawing
		 * the picture as a classic ARGB pixel matric (8 bits per
		 * component). */

		texture = SDL_CreateTexture(rendr, SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_STREAMING, w * zoom, h * zoom);
		if (!texture) {
			SDL_DestroyRenderer(rendr);
			rendr = NULL;

			SDL_DestroyWindow(window);
			window = NULL;

			fprintf(stderr, "Couldn't create the texture: %s\n",
				SDL_GetError());
			return ;
		}

		/* Save data and display message. */

		saved_w = w;
		saved_h = h;

		puts("Turn off your calculator (SHIFT+AC) when you have finished.");
	} else if (saved_w != w || saved_h != h) {
		/* The dimensions have changed somehow, we're gonna manage it!
		 * FIXME: one day. */

		return ;
	}

	/* Copy the data. */

	{
		Uint32 *px;
		int pitch;
		int linesize = w * zoom;

		SDL_LockTexture(texture, NULL, (void **)&px, &pitch);

		for (int y = 0; y < h; y++) {
			Uint32 *refline = px;

			for (int x = 0; x < w; x++) {
				Uint32 pixel = pixels[y][x];

				for (int zx = 0; zx < zoom; zx++)
					*px++ = pixel;
			}
			for (int zy = 1; zy < zoom; zy++) {
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
/* ************************************************************************** */
/*  Main function                                                             */
/* ************************************************************************** */
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

	/* parse args */
	if (parse_args(ac, av, &zoom))
		return (0);

	/* Make the libcasio link handle. */
	if ((err = casio_open_usb(&handle, 0))) {
		/* display error */
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

	/* Initialize SDL */
	if (SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "Failed to initialize SDL: %s\n", SDL_GetError());
		return (3);
	}
	atexit(SDL_Quit);

	/* receive screen */
	if ((err = casio_getscreen(handle, &display_callback, NULL))
	 && err != casio_error_nocalc) {
		switch (err) {
			case casio_error_timeout:
				fprintf(stderr, error_noconnexion);
				break;
			default:
				fprintf(stderr, error_unplanned, casio_strerror(err));
				break;
		}
		return (1);
	}

	/* close */
	casio_close_link(handle);

	/* everything went well */
	return (0);
}
