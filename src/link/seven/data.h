/* ****************************************************************************
 * link/seven/data.h -- data various functions.
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
 *
 * These functions are about sending and receiving data buffers.
 * ************************************************************************* */
#ifndef  LOCAL_LINK_SEVEN_DATA_H
# define LOCAL_LINK_SEVEN_DATA_H 1
# include "../link.h"

extern int casio_seven_send_data_packet OF((casio_link_t *casio__handle,
	unsigned int casio__total, unsigned int casio__id,
	const void *casio__data, unsigned int casio__datasize,
	int casio__resp));

extern int casio_seven_send_quick_data_packet OF((casio_link_t *casio__handle,
	unsigned int casio__total, unsigned int casio__id,
	void *casio__buf, unsigned int casio__datasize, int casio__resp));

extern int casio_seven_unshift OF((casio_link_t *casio__handle));

#endif /* LOCAL_LINK_SEVEN_DATA_H */
