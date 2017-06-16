/* ****************************************************************************
 * stream/builtin/windows.c -- built-in Windows API stream.
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
#ifndef LIBCASIO_DISABLED_WINDOWS

/* only works because I redefined the version at the beginning
 * of `internals.h`! */
# include <windows.h>
# include <setupapi.h>
# include <usbiodef.h>
# include <winerror.h>

/* Here is the structure of a cookie, used by the stream callbacks.
 * PSP_COOKIE is just there to take the piss out of Microsoft :p */
# define BUFSIZE 2048
typedef struct {
	HANDLE _handle;

	long _start, _end;
	unsigned char _buf[BUFSIZE];
} win_cookie_t, *PSP_COOKIE;
/* ************************************************************************* */
/*  Find USB devices                                                         */
/* ************************************************************************* */
/**
 *	wfind:
 *	Find the Microsoft Windows device path.
 *
 *	@arg	vid		the vendor ID (0x0000 to 0xffff)
 *	@arg	pid		the product ID (0x0000 to 0xffff)
 *	@return			the allocated path of the device if found, NULL otherwise
 */

CASIO_LOCAL char *wfind(unsigned int vid, unsigned int pid)
{
	int i; DWORD werr;
	char *devpath = NULL, vidpid[20];
	HDEVINFO DeviceInfoSet;
	SP_DEVICE_INTERFACE_DATA DeviceInterfaceData;

	/* make the vid/pid string */
	sprintf(vidpid, "#vid_%04x&pid_%04x", vid, pid);

	/* get the device information set (chained list) */
	msg((ll_info, "Getting the device info set"));
	DeviceInfoSet = SetupDiGetClassDevs(&GUID_DEVINTERFACE_USB_DEVICE,
		NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (DeviceInfoSet == INVALID_HANDLE_VALUE) {
		werr = GetLastError();
		msg((ll_fatal, "Device info gathering failed! Error 0x%08lX", werr));
		return (NULL);
	}


	/* browse this set, setup */
	DeviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	msg((ll_info, "Enumerating interfaces"));
	for (i = 0; SetupDiEnumDeviceInterfaces(DeviceInfoSet, NULL,
	  &GUID_DEVINTERFACE_USB_DEVICE, i, &DeviceInterfaceData); i++) {
		/* make the local variables */
		DWORD RequiredSize = 0; const char *Path;
		PSP_DEVICE_INTERFACE_DETAIL_DATA DeviceInterfaceDetailData;

		/* get the detail size */
		msg((ll_info, "Getting interface information detail size"));
		if (!SetupDiGetDeviceInterfaceDetail(DeviceInfoSet,
		  &DeviceInterfaceData, NULL, 0, &RequiredSize, NULL) &&
		  (werr = GetLastError()) != ERROR_INSUFFICIENT_BUFFER) {
			msg((ll_error, "Error getting this size: 0x%08lX", werr));
			continue;
		}

		/* allocate detail space */
		msg((ll_info,
			"Allocating space for interface information detail (%luo)",
			RequiredSize));
		DeviceInterfaceDetailData = malloc(RequiredSize);
		if (!DeviceInterfaceDetailData) {
			msg((ll_error, "Memory allocation failed. Oh well."));
			break ;
		}
		DeviceInterfaceDetailData->cbSize =
			sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

		/* get the detail */
		msg((ll_info, "Getting interface information detail"));
		if (!SetupDiGetDeviceInterfaceDetail(DeviceInfoSet,
		  &DeviceInterfaceData, DeviceInterfaceDetailData, RequiredSize,
		  NULL, NULL)) {
			werr = GetLastError();
			msg((ll_error, "Error getting the interface information detail: "
				"0x%08lX", werr));
			continue;
		}

		/* check if it corresponds */
		Path = DeviceInterfaceDetailData->DevicePath;
		msg((ll_info, "Stumbled across: %s", Path));
		if (strstr(Path, vidpid)) {
			devpath = malloc(strlen(Path) + 1);
			if (!devpath) break;
			strcpy(devpath, Path);
		}

		/* free the allocated detail */
		free(DeviceInterfaceDetailData);
		if (devpath) break ;
	}

	/* destroy the device information set */
	msg((ll_info, "Destroying the device information set"));
	SetupDiDestroyDeviceInfoList(DeviceInfoSet);
	return (devpath);
}
/* ************************************************************************* */
/*  Settings, close callbacks                                                */
/* ************************************************************************* */
/**
 *	casio_win_setcomm:
 *	Set communication status of a Microsoft Windows stream.
 *
 *	This is accomplished by getting, updating and setting the DCB.
 *	Yay, a Windows API thingy.
 *
 *	@arg	vcookie		the cookie (uncasted).
 *	@arg	settings	the settings to apply.
 *	@return				the error (0 if ok)
 */

CASIO_LOCAL int casio_win_setcomm(void *vcookie,
	const casio_streamattrs_t *settings)
{
	win_cookie_t *cookie = (win_cookie_t*)vcookie;
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

/**
 *	casio_win_settm:
 *	Set timeouts.
 *
 *	@arg	vcookie		the cookie (uncasted).
 *	@arg	timeouts	the timeouts to apply.
 *	@return				the error (0 if ok).
 */

CASIO_LOCAL int casio_win_settm(void *vcookie,
	const casio_timeouts_t *timeouts)
{
	win_cookie_t *cookie = (void*)vcookie;

	/* set the timeouts */
	COMMTIMEOUTS tm;
	tm.ReadIntervalTimeout =       timeouts->casio_timeouts_read_bw;
	tm.ReadTotalTimeoutConstant =  timeouts->casio_timeouts_read;
	tm.WriteTotalTimeoutConstant = timeouts->casio_timeouts_write;
	SetCommTimeouts(cookie->_handle, &tm);

	/* no error */
	return (0);
}

/**
 *	casio_win_close:
 *	Close a MS-Windows stream.
 *
 *	@arg	vcookie		the cookie (uncasted).
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL int casio_win_close(void *vcookie)
{
	win_cookie_t *cookie = (win_cookie_t*)vcookie;
	CloseHandle(cookie->_handle);
	free(cookie);
	return (0);
}
/* ************************************************************************* */
/*  Character stream callbacks                                               */
/* ************************************************************************* */
/**
 *	casio_win_read:
 *	Read from an MS-Windows stream.
 *
 *	@arg	vcookie		the cookie (uncasted).
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL int casio_win_read(void *vcookie, unsigned char *dest, size_t size)
{
	win_cookie_t *cookie = (win_cookie_t*)vcookie;
	DWORD werr, wsuccess, recv; size_t tocopy;

	/* transmit what's already in the buffer */
	if (cookie->_start <= cookie->_end) {
		tocopy = cookie->_end - cookie->_start + 1;
		if (tocopy > size) tocopy = size;

		memcpy(dest, &cookie->_buf[cookie->_start], tocopy);
		cookie->_start += tocopy;
		dest += tocopy;
		size -= tocopy;
	}

	/* main receiving loop */
	while (size) {
		/* receive */
		wsuccess = ReadFile(cookie->_handle,
			cookie->_buf, size, &recv, NULL);

		/* check error */
		if (!wsuccess) switch ((werr = GetLastError())) {
			case ERROR_DEV_NOT_EXIST:
				msg((ll_error, "Device has been disconnected!"));
				return (casio_error_nocalc);

			default:
				msg((ll_fatal, "Encountered error 0x%08lX", werr));
				return (casio_error_unknown);
		}

		/* get the current size to copy */
		tocopy = (size_t)recv;
		if (tocopy > size) tocopy = size;

		/* copy to destination */
		memcpy(dest, cookie->_buf, tocopy);
		dest += tocopy;
		size -= tocopy;

		/* correct start and end points */
		cookie->_start = tocopy;
		cookie->_end = (size_t)recv - 1;
	}

	/* no error */
	return (0);
}

/**
 *	casio_win_seek:
 *	Seek within an MS-Windows stream.
 *
 *	@arg	vcookie		the cookie (uncasted).
 *	@arg
 */

CASIO_LOCAL int casio_win_seek(void *vcookie, casio_off_t *offset,
	casio_whence_t whence)
{
	win_cookie_t *cookie = (win_cookie_t*)vcookie;
	DWORD ret;

	/* get the move method */
	DWORD MoveMethod;
	switch (whence) {
	case CASIO_SEEK_CUR:          MoveMethod = FILE_CURRENT;
	case CASIO_SEEK_END:          MoveMethod = FILE_END;
	default /* CASIO_SEEK_SET */: MoveMethod = FILE_BEGIN; }

	ret = SetFilePointer(cookie->_handle, (LONG)*offset, NULL, MoveMethod);
	if (ret == INVALID_SET_FILE_POINTER)
		return (casio_error_op); /* TODO: check error with `GetLastError`? */

	*offset = (casio_off_t)ret;
	return (0);
}

/**
 *	casio_win_write:
 *	Write to an MS-Windows stream.
 *
 *	@arg	vcookie		the cookie (uncasted).
 *	@arg	data		the source
 *	@arg	size		the source size
 *	@return				the error code (0 if ok).
 */

CASIO_LOCAL int casio_win_write(void *vcookie,
	const unsigned char *data, size_t size)
{
	win_cookie_t *cookie = (win_cookie_t*)vcookie;
	BOOL wsuccess, werr;

	/* make the I/O request */
	wsuccess = TRUE;
	do {
		/* write */
		DWORD wrt;
		wsuccess = WriteFile(cookie->_handle, data, size, &wrt, NULL);
		if (!wsuccess) break;

		/* go forward */
		data += wrt;
		size -= wrt;
	} while (size);

	/* check error */
	if (!wsuccess) switch ((werr = GetLastError())) {
		case ERROR_DEV_NOT_EXIST:
			msg((ll_error, "Device has been disconnected!"));
			return (casio_error_nocalc);

		default:
			msg((ll_fatal, "Encountered error 0x%08lx", werr));
			return (casio_error_unknown);
	}

	/* success! */
	return (0);
}
/* ************************************************************************* */
/*  Initialization function                                                  */
/* ************************************************************************* */
/* Callbacks */
CASIO_LOCAL const casio_streamfuncs_t casio_windows_callbacks = {
	casio_win_close,
	casio_win_setcomm, casio_win_settm,
	casio_win_read, casio_win_write, casio_win_seek,
	NULL
};

/**
 *	casio_openusb_windows:
 *	Open a Microsoft Windows API stream.
 *
 *	@arg	stream		the stream to make.
 *	@return				the error code (0 if you're ugly).
 */

int CASIO_EXPORT casio_openusb_windows(casio_stream_t **stream)
{
	int err; char *p = NULL;

	p = wfind(0x07cf, 0x6101);
	if (!p) return (casio_error_nocalc);

	err = casio_open_stream_windows(stream, path);
	free(p);
	return (err);
}

/**
 *	casio_opencom_windows:
 *	Open a Microsoft Windows API stream.
 *
 *	@arg	stream		the stream to make.
 *	@arg	path		the Windows device path (NULL if we should find it).
 *	@return				the error code (0 if you're a psychopath).
 */

int CASIO_EXPORT casio_opencom_windows(casio_stream_t **stream,
	const char *path)
{
	int err = 0; DWORD werr;
	HANDLE fhandle = INVALID_HANDLE_VALUE;
	win_cookie_t *cookie = NULL;

	/* open the file handle - my god, this function is so complex. */
	msg((ll_info, "Opening the stream"));
	fhandle = CreateFile(path, GENERIC_READ | GENERIC_WRITE, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fhandle == INVALID_HANDLE_VALUE) switch ((werr = GetLastError())) {
		case ERROR_FILE_NOT_FOUND:
		case ERROR_DEV_NOT_EXIST:
			err = casio_error_nocalc; goto fail;
		case ERROR_ACCESS_DENIED:
			err = casio_error_noaccess; goto fail;
		default:
			msg((ll_fatal, "Error 0x%08lx encountered.", werr));
			err = casio_error_noaccess; goto fail;
	}

	/* make cookie */
	msg((ll_info, "Making the cookie"));
	cookie = malloc(sizeof(win_cookie_t));
	err = casio_error_alloc;
	if (!cookie) goto fail;

	/* fill cookie */
	cookie->_handle = fhandle;
	cookie->_start = 0;
	cookie->_end = 1;

	/* initialize for real */
	return (casio_open(stream, CASIO_MODE_READ | CASIO_MODE_WRITE,
		casio_streamtype_serial, cookie, &casio_windows_callbacks));
fail:
	if (fhandle != INVALID_HANDLE_VALUE) CloseHandle(fhandle);
	if (cookie) free(cookie);
	return (err);
}

#endif
