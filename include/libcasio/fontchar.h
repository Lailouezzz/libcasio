/* ****************************************************************************
 * libcasio/char.h -- libcasio FONTCHARACTER management.
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
#ifndef  LIBCASIO_FONTCHAR_H
# define LIBCASIO_FONTCHAR_H
# include "cdefs.h"

typedef casio_uint16_t FONTCHARACTER;

/* A multi-character should resolve as a maximum of 16 characters. */
# define CASIO_FONTCHAR_MULTI_MAX 16

CASIO_EXTERN int CASIO_EXPORT casio_is_fontchar_lead
	OF((int casio__code));
CASIO_EXTERN int CASIO_EXPORT casio_fontchar_to_uni
	OF((casio_uint16_t casio__code, casio_uint32_t *casio__uni));
CASIO_EXTERN int CASIO_EXPORT casio_uni_to_fontchar
	OF((casio_uint32_t const *casio__uni, casio_uint16_t casio__code));

#endif /* LIBCASIO_CHAR_H */
