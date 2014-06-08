/***************************************************************************
 *                  _       ___       _____
 *  Project        | |     / (_)___ _/ ___/____ _____  ___
 *                 | | /| / / / __ `/\__ \/ __ `/ __ \/ _ \
 *                 | |/ |/ / / /_/ /___/ / /_/ / / / /  __/
 *                 |__/|__/_/\__,_//____/\__,_/_/ /_/\___/
 *
 * Copyright (C) 2012 - 2013, Marc Hoersken, <info@marc-hoersken.de>
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this software distribution.
 *
 * You may opt to use, copy, modify, and distribute this software for any
 * purpose with or without fee, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either expressed or implied.
 *
 ***************************************************************************/

#ifndef COISANE_UTIL_H
#define COISANE_UTIL_H

#if _MSC_VER > 1000
#pragma once
#endif

#include <DriverSpecs.h>
__user_code

#include <windows.h>
#include <setupapi.h>

#include "coisane.h"
#include "winsane.h"

HINF OpenInfFile(_In_ HDEVINFO hDeviceInfoSet, _In_ PSP_DEVINFO_DATA pDeviceInfoData, _Out_opt_ PUINT ErrorLine);

DWORD UpdateInstallDeviceFlags(_In_ HDEVINFO hDeviceInfoSet, _In_ PSP_DEVINFO_DATA pDeviceInfoData, _In_ DWORD dwFlags);
DWORD ChangeDeviceState(_In_ HDEVINFO hDeviceInfoSet, _In_ PSP_DEVINFO_DATA pDeviceInfoData, _In_ DWORD StateChange, _In_ DWORD Scope);
DWORD UpdateDeviceInfo(_In_ PCOISANE_Data privateData, _In_ PWINSANE_Device device);

DWORD QueryDeviceData(_In_ PCOISANE_Data privateData);
DWORD UpdateDeviceData(_In_ PCOISANE_Data privateData, _In_ PWINSANE_Device device);

size_t CreateResolutionList(_In_ PCOISANE_Data privateData, _In_ PWINSANE_Device device, _Outptr_result_maybenull_ LPTSTR *ppszResolutions);

#endif
