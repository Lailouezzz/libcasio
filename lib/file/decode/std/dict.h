/* ****************************************************************************
 * file/decode/std/dict.h -- dictionary decoding.
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
#ifndef  LOCAL_FILE_DECODE_STD_DICT_H
# define LOCAL_FILE_DECODE_STD_DICT_H 1
# include "../decode.h"

typedef int casio_decode_dict_entry_t
	OF((void *casio__cookie, unsigned int casio__id,
		void *casio__mem, size_t casio__size));

CASIO_EXTERN int CASIO_EXPORT casio_decode_std_dict
	OF((casio_stream_t *casio__buffer,
		casio_decode_dict_entry_t *casio__callback, void *casio__cookie,
		unsigned int casio__num, unsigned long casio__size, int casio__is32));

#endif /* LOCAL_FILE_DECODE_STD_DICT_H */
