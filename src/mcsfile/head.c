/* ****************************************************************************
 * mcsfile/head.c -- correct a main memory head.
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
#include "mcsfile.h"
#include "reference/reference.h"

/**
 *	casio_correct_mcshead:
 *	Correct the information.
 *
 *	@arg	head	the MCS head to correct.
 *	@arg	mcsfor	the destination platform.
 *	@return			the error code (0 if ok).
 */

int CASIO_EXPORT casio_correct_mcshead(casio_mcshead_t *head,
	unsigned long mcsfor)
{
	int err; unsigned long inifor;

	/* Prepare. */
	inifor = head->casio_mcshead_flags & casio_mcsfor_mask;
	mcsfor =                    mcsfor & casio_mcsfor_mask;
	msg((ll_info, "Converting head from info type 0x%02X to 0x%02X",
		inifor >> 24, mcsfor >> 24));

	/* Check if has already been converted. */
	if (inifor == mcsfor) return (0);

	/* If we ought to convert from any specific type,
	 * we have to convert to abstract first. */
	switch (inifor) {
	case 0: err = 0; break;
	case casio_mcsfor_mcs:
		err = casio_correct_mcshead_from_mcs(head); break;
	case casio_mcsfor_cas40: /* FALLTHRU */
	case casio_mcsfor_cas50:
		err = casio_correct_mcshead_from_casdata(head); break;
#if 0 /* TODO */
	case casio_mcsfor_cas100:
		err = casio_correct_mcshead_from_cas100(head); break;
#endif
	default: err = casio_error_op;
	}

	/* If we have encountered an error while doing that,
	 * we should tell the user about it. */
	if (err) return (err);
	head->casio_mcshead_flags &= ~casio_mcsfor_mask;

	/* Then we can convert from abstract to specific. */
	switch (mcsfor) {
	case 0: return (0);

	case casio_mcsfor_mcs:
		err = casio_correct_mcshead_to_mcs(head); break;
	case casio_mcsfor_cas40: /* FALLTHRU */
	case casio_mcsfor_cas50:
		err = casio_correct_mcshead_to_casdata(head); break;
#if 0 /* TODO */
	case casio_mcsfor_cas100:
		err = casio_correct_mcshead_to_cas100(head); break;
#endif
	default: err = casio_error_op;
	}

	if (err) return (err);
	head->casio_mcshead_flags |= mcsfor;
	return (err);
}
