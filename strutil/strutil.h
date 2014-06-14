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

#ifndef STRUTIL_H
#define STRUTIL_H

#if _MSC_VER > 1000
#pragma once
#endif

#include <DriverSpecs.h>
__user_code

#include <windows.h>
#include <tchar.h>
#include <strsafe.h>

LPTSTR StringAClone(_In_ HANDLE hHeap, _In_ LPTSTR pszString);

LPSTR StringWToA(_In_ HANDLE hHeap, _In_ LPWSTR pszString);
LPWSTR StringAToW(_In_ HANDLE hHeap, _In_ LPSTR pszString);

HRESULT StringCbAPrintfA(_In_ HANDLE hHeap, _Outptr_result_maybenull_ LPSTR *ppszDest, _Out_ size_t *pcbDest, _In_ LPCSTR pszFormat, ...);
HRESULT StringCbAPrintfW(_In_ HANDLE hHeap, _Outptr_result_maybenull_ LPWSTR *ppszDest, _Out_ size_t *pcbDest, _In_ LPCWSTR pszFormat, ...);

HRESULT StringCchAPrintfA(_In_ HANDLE hHeap, _Outptr_result_maybenull_ LPSTR *ppszDest, _Out_ size_t *pcbDest, _In_ LPCSTR pszFormat, ...);
HRESULT StringCchAPrintfW(_In_ HANDLE hHeap, _Outptr_result_maybenull_ LPWSTR *ppszDest, _Out_ size_t *pcbDest, _In_ LPCWSTR pszFormat, ...);

HRESULT StringCbAVPrintfA(_In_ HANDLE hHeap, _Outptr_result_maybenull_ LPSTR *ppszDest, _Out_ size_t *pcbDest, _In_ LPCSTR pszFormat, _In_ va_list argList);
HRESULT StringCbAVPrintfW(_In_ HANDLE hHeap, _Outptr_result_maybenull_ LPWSTR *ppszDest, _Out_ size_t *pcbDest, _In_ LPCWSTR pszFormat, _In_ va_list argList);

HRESULT StringCchAVPrintfA(_In_ HANDLE hHeap, _Outptr_result_maybenull_ LPSTR *ppszDest, _Out_ size_t *pcbDest, _In_ LPCSTR pszFormat, _In_ va_list argList);
HRESULT StringCchAVPrintfW( _In_ HANDLE hHeap, _Outptr_result_maybenull_ LPWSTR *ppszDest, _Out_ size_t *pcbDest, _In_ LPCWSTR pszFormat, _In_ va_list argList);

#ifdef UNICODE
#define StringToA StringWToA
#define StringToW StringAClone
#define StringTo StringToW
#define StringCbAPrintf StringCbAPrintfW
#define StringCchAPrintf StringCchAPrintfW
#define StringCbAVPrintf StringCbAVPrintfW
#define StringCchAVPrintf StringCchAVPrintfW
#else
#define StringToA StringAClone
#define StringToW StringAToW
#define StringTo StringToA
#define StringCbAPrintf StringCbAPrintfA
#define StringCchAPrintf StringCchAPrintfA
#define StringCbAVPrintf StringCbAVPrintfA
#define StringCchAVPrintf StringCchAVPrintfA
#endif

#endif
