/* *****************************************************************************
 * g1a-wrapper/icon.c -- g1a-wrapper icon decoding function.
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
#include <magick/MagickCore.h>

/**
 *	open_icon:
 *	Open the icon.
 *
 *	@arg	path		the image path.
 *	@arg	icon		the image.
 */

int open_icon(const char *path, uint32_t **icon)
{
	int ret = 1;
	Image *images = NULL, *image = NULL;
	ImageInfo *info = NULL;
	ExceptionInfo *exception = NULL;

	/* initialize */
	MagickCoreGenesis("g1a-wrapper", MagickTrue);

	/* read source image and image info */
	exception = AcquireExceptionInfo();
	info = CloneImageInfo((ImageInfo*)NULL);
	strcpy(info->filename, path);
	images = ReadImage(info, exception);
	if (!images) {
		err("unable to open the icon: %s", exception->reason);
		goto fail;
	}

	/* get the first frame */
	image = RemoveFirstImageFromList(&images);
	if (!image) goto fail;

	/* check the dimensions */
	unsigned int width = image->columns, height = image->rows;
	if (width != 30 || height < 17 || height > 19) {
		err("icon must be between 30x17 and 30x19 pixels, is %ux%u",
			width, height);
		goto fail;
	}

	/* get the pixels */
	const Quantum *pixels = GetVirtualPixels(image, 0, 0, width, height,
		exception);
	if (!pixels) {
		err("unable to access icon pixels.");
		goto fail;
	}

	/* skip the first line */
	if (height == 19) pixels += width * 4;

	/* get the pixels */
	for (size_t y = 0; y < 17; y++) for (size_t x = 0; x < 30; x++) {
		/* get the total */
		float r = *pixels++;
		float g = *pixels++;
		float b = *pixels++;
		pixels++;

		/* check the total */
		icon[y][x] = (r + g + b > 0x198) ? 0xFFFFFF : 0x000000;
	}

	ret = 0;
fail:
	if (info) DestroyImageInfo(info);
	if (exception) DestroyExceptionInfo(exception);
	if (image) DestroyImage(image);
	if (images) DestroyImage(images);
	MagickCoreTerminus();
	return (ret);
}
