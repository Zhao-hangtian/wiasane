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

#ifndef WINSANE_PARAMS_H
#define WINSANE_PARAMS_H

#if _MSC_VER > 1000
#pragma once
#endif

#include "winsane.h"
#include "winsane_socket.h"
#include "winsane_device.h"

class WINSANE_API WINSANE_Params {
public:
	WINSANE_Params(_In_ PWINSANE_Device device, _In_ PWINSANE_Socket sock, _In_ PSANE_Parameters sane_params);
	~WINSANE_Params();


	/* Public API */
	SANE_Frame GetFormat();
	SANE_Bool IsLastFrame();
	SANE_Int GetBytesPerLine();
	SANE_Int GetPixelsPerLine();
	SANE_Int GetLines();
	SANE_Int GetDepth();


private:
	PWINSANE_Device device;
	PWINSANE_Socket sock;
	PSANE_Parameters sane_params;
};

#endif
