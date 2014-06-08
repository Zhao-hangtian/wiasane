/***************************************************************************
 *                  _       ___       _____
 *  Project        | |     / (_)___ _/ ___/____ _____  ___
 *                 | | /| / / / __ `/\__ \/ __ `/ __ \/ _ \
 *                 | |/ |/ / / /_/ /___/ / /_/ / / / /  __/
 *                 |__/|__/_/\__,_//____/\__,_/_/ /_/\___/
 *
 * Copyright (C) 2012 - 2014, Marc Hoersken, <info@marc-hoersken.de>
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

#ifndef WINSANE_SCAN_H
#define WINSANE_SCAN_H

#if _MSC_VER > 1000
#pragma once
#endif

#include "winsane.h"
#include "winsane_socket.h"
#include "winsane_device.h"

enum WINSANE_Scan_State {
	NEW,
	CONNECTED,
	SCANNING,
	COMPLETED,
	DISCONNECTED
};

class WINSANE_API WINSANE_Scan {
public:
	/* Constructer & Deconstructer */
	WINSANE_Scan(_In_ PWINSANE_Device device, _In_ PWINSANE_Socket sock, _In_ SANE_Word port, _In_ SANE_Word byte_order);
	~WINSANE_Scan();


	/* Public API */
	SANE_Status Connect();
	SANE_Status AquireImage(_Inout_ PBYTE buffer, _Inout_ PDWORD length);
	SANE_Status Disconnect();


protected:
	SANE_Status Receive(_Inout_ PBYTE buffer, _Inout_ PDWORD length);


private:
	WINSANE_Scan_State state;
	PWINSANE_Device device;
	PWINSANE_Params params;
	PWINSANE_Socket sock, scan;
	SANE_Word port;
	SANE_Word byte_order;
	PBYTE buf;
	DWORD buflen;
	DWORD bufoff;
	DWORD bufpos;
};

#endif
