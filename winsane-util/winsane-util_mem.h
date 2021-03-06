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

#ifndef WINSANE_UTIL_MEM_H
#define WINSANE_UTIL_MEM_H

#if _MSC_VER > 1000
#pragma once
#endif

#include <DriverSpecs.h>
__user_code

#include <windows.h>

BOOL WINAPI HeapSafeFree(_Inout_ HANDLE hHeap, _In_ DWORD dwFlags, _In_opt_ _Post_ptr_invalid_ LPVOID lpMem);

HLOCAL WINAPI LocalSafeFree(_Pre_opt_valid_ HLOCAL hMem);
HGLOBAL WINAPI GlobalSafeFree(_Pre_opt_valid_ HGLOBAL hMem);

#endif
