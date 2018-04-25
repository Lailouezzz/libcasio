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
#include <string.h>
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
	QUOTE(BIN) ".\n"
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
	static SDL_Surface *screen = NULL;
	static int saved_w = 0, saved_h = 0;

	(void)vcookie;
	/* Create screen if there isn't one. */
	if (!screen || saved_w != w || saved_h != h) {
		/* Create the window. */
		if (!(screen = SDL_SetVideoMode(w * zoom, h * zoom, 32,
		  SDL_SWSURFACE | SDL_DOUBLEBUF))) {
			log("Couldn't set video mode: %s\n", SDL_GetError());
			return ;
		}
		SDL_WM_SetCaption("P7screen", NULL);

		/* Save data and display message. */
		saved_w = w; saved_h = h;
		puts("Turn off your calculator (SHIFT+AC) when you have finished.");
	}

	/* Lock the screen. */
	SDL_LockSurface(screen);

	/* Copy the data. */
	uint32_t *px = (uint32_t*)screen->pixels;
	int linesize = w * zoom;
	for (int y = 0; y < h; y++) {
		uint32_t *refline = px;
		for (int x = 0; x < w; x++) {
			uint32_t pixel = pixels[y][x];
			for (int zx = 0; zx < zoom; zx++)
				*px++ = pixel;
		}
		for (int zy = 1; zy < zoom; zy++) {
			memcpy(px, refline, linesize * sizeof(uint32_t));
			px += linesize;
		}
	}

	/* Unlock the screen, and flippin' flip it. */
	SDL_UnlockSurface(screen);
	SDL_Flip(screen);
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
			case casio_error_nocalc: log(error_noconnexion); break;
			case casio_error_noaccess: log(error_noaccess); break;
			default: log(error_unplanned, casio_strerror(err)); break;
		}

		/* return */
		return (1);
	}

	/* Initialize SDL */
	if (SDL_Init(SDL_INIT_VIDEO)) {
		log("Failed to initialize SDL: %s\n", SDL_GetError());
		return (3);
	}
	atexit(SDL_Quit);

	/* receive screen */
	if ((err = casio_getscreen(handle, &display_callback, NULL))
	 && err != casio_error_nocalc) {
		switch (err) {
			case casio_error_timeout: log(error_noconnexion); break;
			default: log(error_unplanned, casio_strerror(err)); break;
		}
		return (1);
	}

	/* close */
	casio_close_link(handle);

	/* everything went well */
	return (0);
}
