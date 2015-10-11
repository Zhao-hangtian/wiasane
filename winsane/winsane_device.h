/***************************************************************************
 *                  _       ___       _____
 *  Project        | |     / (_)___ _/ ___/____ _____  ___
 *                 | | /| / / / __ `/\__ \/ __ `/ __ \/ _ \
 *                 | |/ |/ / / /_/ /___/ / /_/ / / / /  __/
 *                 |__/|__/_/\__,_//____/\__,_/_/ /_/\___/
 *
 * Copyright (C) 2012 - 2015, Marc Hoersken, <info@marc-hoersken.de>
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

#ifndef WINSANE_DEVICE_H
#define WINSANE_DEVICE_H

#if _MSC_VER > 1000
#pragma once
#endif

#include "winsane.h"
#include "winsane_session.h"
#include "winsane_socket.h"
#include "winsane_option.h"
#include "winsane_params.h"
#include "winsane_scan.h"

class WINSANE_API WINSANE_Device {
public:
	/* Constructer & Deconstructer */
	WINSANE_Device(_In_ PWINSANE_Session session, _In_ PWINSANE_Socket sock, _In_ PSANE_Device device);
	~WINSANE_Device();

	static PWINSANE_Device ByName(_In_ PWINSANE_Session session, _In_ PWINSANE_Socket sock, _In_ SANE_String_Const name);


	/* Public API */
	SANE_String_Const GetName();
	SANE_String_Const GetVendor();
	SANE_String_Const GetModel();
	SANE_String_Const GetType();

	SANE_Status Open();
	SANE_Status Close();
	BOOL IsOpen();

	SANE_Status FetchOptions();
	LONG GetOptions();
	PWINSANE_Option GetOption(_In_ LONG index);
	PWINSANE_Option GetOption(_In_ SANE_String_Const name);
	PWINSANE_Option GetOption(_In_ PTSTR pszName);
	VOID ClearOptions();

	SANE_Status GetParams(_Outptr_result_maybenull_ PWINSANE_Params *params);

	SANE_Status Start(_Outptr_result_maybenull_ PWINSANE_Scan *scan);
	SANE_Status Cancel();

private:
	int num_options;
	PWINSANE_Option *options;

	PWINSANE_Session session;
	PWINSANE_Socket sock;
	PSANE_Device sane_device;
	SANE_Handle sane_handle;
};

#endif
