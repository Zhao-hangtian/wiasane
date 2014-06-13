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

#include "wiasane_scan.h"

#include <sti.h>
#include <wia.h>

#include "wiasane_opt.h"
#include "strutil_dbg.h"

HRESULT SetScannerSettings(_Inout_ PSCANINFO pScanInfo, _Inout_ PWIASANE_Context pContext)
{
	PWINSANE_Option oOption;
	HRESULT hr;

	if (!pContext || !pContext->oDevice || !pContext->pValues)
		return E_INVALIDARG;

	oOption = pContext->oDevice->GetOption(WIASANE_OPTION_MODE);
	if (oOption && oOption->GetType() == SANE_TYPE_STRING) {
		switch (pScanInfo->DataType) {
			case WIA_DATA_THRESHOLD:
				hr = oOption->SetValueString(pContext->pValues->pszModeThreshold);
				break;
			case WIA_DATA_GRAYSCALE:
				hr = oOption->SetValueString(pContext->pValues->pszModeGrayscale);
				break;
			case WIA_DATA_COLOR:
				hr = oOption->SetValueString(pContext->pValues->pszModeColor);
				break;
			default:
				hr = E_INVALIDARG;
				break;
		}
		if (FAILED(hr)) {
			Trace(TEXT("Failed to set option 'mode' according to '%d': %08x"),
				pScanInfo->DataType, hr);
			return hr;
		}
	} else {
		Trace(TEXT("Required option 'mode' is not supported."));
		return E_NOTIMPL;
	}

	oOption = pContext->oDevice->GetOption(WIASANE_OPTION_RESOLUTION);
	if (oOption) {
		hr = oOption->SetValue(pScanInfo->Xresolution);
		if (FAILED(hr)) {
			Trace(TEXT("Failed to set option 'resolution' to '%d': %08x"),
				pScanInfo->Xresolution, hr);
			return hr;
		}
	} else {
		Trace(TEXT("Required option 'resolution' is not supported."));
		return E_NOTIMPL;
	}

	oOption = pContext->oDevice->GetOption(WIASANE_OPTION_CONTRAST);
	if (oOption) {
		hr = oOption->SetValue(pScanInfo->Contrast);
		if (FAILED(hr) && hr != E_NOTIMPL) {
			Trace(TEXT("Failed to set option 'contrast' to '%d': %08x"),
				pScanInfo->Contrast, hr);
			return hr;
		}
	}

	oOption = pContext->oDevice->GetOption(WIASANE_OPTION_BRIGHTNESS);
	if (oOption) {
		hr = oOption->SetValue(pScanInfo->Intensity);
		if (FAILED(hr) && hr != E_NOTIMPL) {
			Trace(TEXT("Failed to set option 'brightness' to '%d': %08x"),
				pScanInfo->Intensity, hr);
			return hr;
		}
	}

	return S_OK;
}

HRESULT SetScanWindow(_Inout_ PWIASANE_Context pContext)
{
	PWINSANE_Option oOption;
	HRESULT hr;

	if (!pContext || !pContext->oDevice)
		return E_INVALIDARG;

	oOption = pContext->oDevice->GetOption(WIASANE_OPTION_TL_X);
	if (!oOption) {
		Trace(TEXT("Required option 'tl-x' is not supported."));
		return E_NOTIMPL;
	}

	hr = SetOptionValueInch(oOption, pContext->dblTopLeftX);
	if (FAILED(hr)) {
		Trace(TEXT("Failed to set option 'tl-x' to '%f': %08x"),
			pContext->dblTopLeftX, hr);
		return hr;
	}

	oOption = pContext->oDevice->GetOption(WIASANE_OPTION_TL_Y);
	if (!oOption) {
		Trace(TEXT("Required option 'tl-y' is not supported."));
		return E_NOTIMPL;
	}

	hr = SetOptionValueInch(oOption, pContext->dblTopLeftY);
	if (FAILED(hr)) {
		Trace(TEXT("Failed to set option 'tl-y' to '%f': %08x"),
			pContext->dblTopLeftY, hr);
		return hr;
	}

	oOption = pContext->oDevice->GetOption(WIASANE_OPTION_BR_X);
	if (!oOption) {
		Trace(TEXT("Required option 'br-x' is not supported."));
		return E_NOTIMPL;
	}

	hr = SetOptionValueInch(oOption, pContext->dblBottomRightX);
	if (FAILED(hr)) {
		Trace(TEXT("Failed to set option 'br-x' to '%f': %08x"),
			pContext->dblBottomRightX, hr);
		return hr;
	}

	oOption = pContext->oDevice->GetOption(WIASANE_OPTION_BR_Y);
	if (!oOption) {
		Trace(TEXT("Required option 'br-y' is not supported."));
		return E_NOTIMPL;
	}

	hr = SetOptionValueInch(oOption, pContext->dblBottomRightY);
	if (FAILED(hr)) {
		Trace(TEXT("Failed to set option 'br-y' to '%f': %08x"),
			pContext->dblBottomRightY, hr);
		return hr;
	}

	return hr;
}

HRESULT SetScanMode(_Inout_ PWIASANE_Context pContext)
{
	PSANE_String_Const string_list;
	PWINSANE_Option oOption;
	HRESULT hr;

	if (!pContext || !pContext->oDevice)
		return E_INVALIDARG;

	switch (pContext->lScanMode) {
		case SCANMODE_FINALSCAN:
			Trace(TEXT("Final Scan"));

			oOption = pContext->oDevice->GetOption(WIASANE_OPTION_PREVIEW);
			if (oOption && oOption->GetType() == SANE_TYPE_BOOL) {
				oOption->SetValueBool(SANE_FALSE);
			}

			oOption = pContext->oDevice->GetOption(WIASANE_OPTION_COMPRESSION);
			if (oOption && oOption->GetType() == SANE_TYPE_STRING
				&& oOption->GetConstraintType() == SANE_CONSTRAINT_STRING_LIST) {
				string_list = oOption->GetConstraintStringList();
				if (string_list[0] != NULL) {
					oOption->SetValueString(string_list[0]);
				}
			}

			hr = S_OK;
			break;

		case SCANMODE_PREVIEWSCAN:
			Trace(TEXT("Preview Scan"));

			oOption = pContext->oDevice->GetOption(WIASANE_OPTION_PREVIEW);
			if (oOption && oOption->GetType() == SANE_TYPE_BOOL) {
				oOption->SetValueBool(SANE_TRUE);
			}

			oOption = pContext->oDevice->GetOption(WIASANE_OPTION_COMPRESSION);
			if (oOption && oOption->GetType() == SANE_TYPE_STRING
				&& oOption->GetConstraintType() == SANE_CONSTRAINT_STRING_LIST) {
				string_list = oOption->GetConstraintStringList();
				if (string_list[0] != NULL && string_list[1] != NULL) {
					oOption->SetValueString(string_list[1]);
				}
			}

			hr = S_OK;
			break;

		default:
			Trace(TEXT("Unknown Scan Mode (%d)"), pContext->lScanMode);

			hr = E_INVALIDARG;
			break;
	}

	return hr;
}
