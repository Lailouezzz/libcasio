/* ****************************************************************************
 * stream/builtin/windows/setattrs.c -- set attrs of a Windows API stream.
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
#include "windows.h"
#ifndef LIBCASIO_DISABLED_WINDOWS

/**
 *	casio_windows_setattrs:
 *	Set communication status of a Microsoft Windows stream.
 *
 *	This is accomplished by getting, updating and setting the DCB.
 *	Yay, a Windows API thingy.
 *
 *	@arg	cookie		the cookie.
 *	@arg	settings	the settings to apply.
 *	@return				the error (0 if ok)
 */

int CASIO_EXPORT casio_windows_setattrs(win_cookie_t *cookie,
	const casio_streamattrs_t *settings)
{
	DWORD wsuccess, spd; DCB dcb;

	/* get speed */
	switch (settings->casio_streamattrs_speed) {
	case CASIO_B1200:   spd = CBR_1200;   break;
	case CASIO_B2400:   spd = CBR_2400;   break;
	case CASIO_B4800:   spd = CBR_4800;   break;
	case CASIO_B9600:   spd = CBR_9600;   break;
	case CASIO_B19200:  spd = CBR_19200;  break;
	case CASIO_B38400:  spd = CBR_38400;  break;
	case CASIO_B57600:  spd = CBR_57600;  break;
	case CASIO_B115200: spd = CBR_115200; break;
	default:
		msg((ll_info, "Speed was unsupported by the Windows API: %u",
			settings->casio_streamattrs_speed));
		return (casio_error_op);
	}

	/* gather stream properties */
	SecureZeroMemory(&dcb, sizeof(DCB));
	dcb.DCBlength = sizeof(DCB);
	if (!GetCommState(cookie->_handle, &dcb)) {
		msg((ll_warn, "Failed gathering the DCB (0x%08lX), nevermind.",
			GetLastError()));
		return (0);
	}

	/* set normal things */
	dcb.BaudRate = spd;
	dcb.ByteSize = 8;
	dcb.fParity = !!(settings->casio_streamattrs_flags & CASIO_PARENB);
	dcb.Parity = (~settings->casio_streamattrs_flags & CASIO_PARENB)
		? NOPARITY : (settings->casio_streamattrs_flags & CASIO_PARODD)
		? ODDPARITY : EVENPARITY;
	dcb.StopBits = (settings->casio_streamattrs_flags & CASIO_TWOSTOPBITS)
		? TWOSTOPBITS : ONESTOPBIT;

	/* set the DTR control mode */
	switch (settings->casio_streamattrs_flags & CASIO_DTRMASK) {
	case CASIO_DTRCTL_ENABLE:
		dcb.fDtrControl = DTR_CONTROL_ENABLE;
		break;
	case CASIO_DTRCTL_HANDSHAKE:
		dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
		break;
	default:
		dcb.fDtrControl = DTR_CONTROL_DISABLE;
		break;
	}

	/* set the RTS control mode */
	switch (settings->casio_streamattrs_flags & CASIO_RTSMASK) {
	case CASIO_RTSCTL_ENABLE:
		dcb.fRtsControl = RTS_CONTROL_ENABLE;
		break;
	case CASIO_RTSCTL_HANDSHAKE:
		dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
		break;
	default:
		dcb.fRtsControl = RTS_CONTROL_DISABLE;
		break;
	}

	/* set the XON/XOFF control mode on input */
	dcb.fInX  = !!(settings->casio_streamattrs_flags & CASIO_XONCTL_ENABLE);
	dcb.XonChar  = settings->casio_streamattrs_cc[CASIO_XON];

	/* set the XON/XOFF control mode on output */
	dcb.fOutX = !!(settings->casio_streamattrs_flags & CASIO_XOFFCTL_ENABLE);
	dcb.XoffChar = settings->casio_streamattrs_cc[CASIO_XOFF];

	/* set buffer limits (TODO: find out why non-zero values cause
	 * ERROR_INVALID_PARAMETER when using `SetCommState`) */
	dcb.XonLim = 0x0000;
	dcb.XoffLim = 0x0000;

	/* save new state */
	msg((ll_info, "Updating the DCB."));
	wsuccess = SetCommState(cookie->_handle, &dcb);
	if (!wsuccess) {
		msg((ll_warn, "SetCommState: Error 0x%08lX occured.", GetLastError()));
		return (casio_error_unknown);
	}

	/* no error! */
	return (0);
}

#endif
