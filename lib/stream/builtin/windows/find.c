/* ****************************************************************************
 * stream/builtin/windows/find.c -- find a Windows API USB stream.
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
 *	casio_windows_find_usb:
 *	Find the Microsoft Windows device path.
 *
 *	@arg	path	the path to allocate.
 *	@arg	vid		the vendor ID (0x0000 to 0xffff)
 *	@arg	pid		the product ID (0x0000 to 0xffff)
 *	@return			the error code (0 if ok).
 */

int CASIO_EXPORT casio_windows_find_usb(char **path,
	unsigned int vid, unsigned int pid)
{
	int i; DWORD werr;
	char *devpath = NULL, vidpid[20];
	HDEVINFO DeviceInfoSet;
	SP_DEVICE_INTERFACE_DATA DeviceInterfaceData;

	/* Make the vid/pid string. */

	sprintf(vidpid, "#vid_%04x&pid_%04x", vid, pid);

	/* Get the device information set (chained list). */

	msg((ll_info, "Getting the device info set"));
	DeviceInfoSet = SetupDiGetClassDevs(&GUID_DEVINTERFACE_USB_DEVICE,
		NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (DeviceInfoSet == INVALID_HANDLE_VALUE) {
		werr = GetLastError();
		msg((ll_fatal, "Device info gathering failed! Error 0x%08lX", werr));
		return (casio_error_notfound);
	}

	/* Browse this set, setup. */

	DeviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	msg((ll_info, "Enumerating interfaces"));
	for (i = 0; SetupDiEnumDeviceInterfaces(DeviceInfoSet, NULL,
	  &GUID_DEVINTERFACE_USB_DEVICE, i, &DeviceInterfaceData); i++) {
		DWORD RequiredSize = 0; const char *Path;
		PSP_DEVICE_INTERFACE_DETAIL_DATA DeviceInterfaceDetailData;

		/* Get the detail size. */

		msg((ll_info, "Getting interface information detail size"));
		if (!SetupDiGetDeviceInterfaceDetail(DeviceInfoSet,
		  &DeviceInterfaceData, NULL, 0, &RequiredSize, NULL) &&
		  (werr = GetLastError()) != ERROR_INSUFFICIENT_BUFFER) {
			msg((ll_error, "Error getting this size: 0x%08lX", werr));
			continue;
		}

		/* Allocate detail space. */

		msg((ll_info, "Allocating space for interface "
			"information detail (%luo)", RequiredSize));
		DeviceInterfaceDetailData = casio_alloc(RequiredSize, 1);
		if (!DeviceInterfaceDetailData) {
			msg((ll_error, "Memory allocation failed. Oh well."));
			break;
		}
		DeviceInterfaceDetailData->cbSize =
			sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

		/* Get the detail. */

		msg((ll_info, "Getting interface information detail"));
		if (!SetupDiGetDeviceInterfaceDetail(DeviceInfoSet,
		  &DeviceInterfaceData, DeviceInterfaceDetailData, RequiredSize,
		  NULL, NULL)) {
			werr = GetLastError();
			msg((ll_error, "Error getting the interface information detail: "
				"0x%08lX", werr));
			continue;
		}

		/* Check if it corresponds. */

		Path = DeviceInterfaceDetailData->DevicePath;
		msg((ll_info, "Stumbled across: %s", Path));
		if (strstr(Path, vidpid)) {
			devpath = casio_alloc(strlen(Path) + 1, 1);
			if (!devpath)
				break;
			strcpy(devpath, Path);
		}

		/* Free the allocated detail. */

		casio_free(DeviceInterfaceDetailData);
		if (devpath)
			break ;
	}

	/* Destroy the device information set. */

	msg((ll_info, "Destroying the device information set"));
	SetupDiDestroyDeviceInfoList(DeviceInfoSet);
	if (!devpath)
		return (casio_error_notfound);
	*path = devpath;
	return (0);
}

#endif
