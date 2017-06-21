/* ****************************************************************************
 * comlist/builtin/windows.c -- find out serial devices on MS-Windows.
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
#include "../../internals.h"
#ifdef __WINDOWS__
/* only works because I redefined the version at the beginning
 * of `internals.h`! */
# include <windows.h>
# include <setupapi.h>
# include <usbiodef.h>
# include <winerror.h>

/**
 *	casio_comlist_windows:
 *	List serial devices under MS-Windows.
 *
 *	I'm using the source from Python 3.x's winreg module source and the MSDN
 *	documentation.
 *
 *	@arg	callback	the callback to which to send the path.
 *	@arg	cookie		the cookie to pass to the callback.
 *	@return				the error code (0 if ok).
 */

int CASIO_EXPORT casio_comlist_windows(casio_list_com_t callback, void *cookie)
{
	DWORD werr, i, type, valsize, curval, datasize, curdata;
	HKEY hkey; int hkey_open = 0;
	char *value = NULL, *data = NULL;

	/* read the registry key */
	msg((ll_info,
		"Opening HKEY_LOCAL_MACHINE\\HARDWARE\\DEVICEMAP\\SERIALCOMM"));
	werr = RegOpenKey(HKEY_LOCAL_MACHINE,
		"HARDWARE\\DEVICEMAP\\SERIALCOMM", &hkey);
	if (werr) goto fail;
	hkey_open = 1;

	/* prepare */
	msg((ll_info, "Allocating enough space"));
	valsize = 1024; datasize = 1024; /* I DON'T CARE IT WORKS LALALA */
	if (!(value = casio_alloc(valsize, 1))
	 || !(data  = casio_alloc(datasize, 1)))
		goto fail;

	/* enumerate values */
	msg((ll_info, "Enumerating values"));
	curval = valsize;
	curdata = datasize;
	for (i = 0; (werr = RegEnumValue(hkey, i, value, &curval, NULL,
	  &type, (BYTE*)data, &curdata)) != ERROR_NO_MORE_ITEMS; i++) {
		if (type != REG_SZ)
			continue;
		if (werr == ERROR_MORE_DATA) {
			msg((ll_error, "data not big enough, o shit waddap"));
			goto fail;
		}

		(*callback)(cookie, data);
		curval = valsize;
		curdata = datasize;
	}

	/* error is `ERROR_MORE_DATA`, o ship shaddap */
	werr = 0;

fail:
	ifmsg(werr, (ll_error, "Got error %08lu", werr));
	/* free, close the key and return ! */
	casio_free(value); casio_free(data);
	if (hkey_open) RegCloseKey(hkey);
	return (werr ? casio_error_unknown : 0);
}

#endif /* __WINDOWS__ */
