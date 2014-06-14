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

#include "coisane_wizard.h"

#include <tchar.h>
#include <strsafe.h>
#include <malloc.h>

#include "dllmain.h"
#include "resource.h"
#include "strutil.h"
#include "strutil_dbg.h"
#include "strutil_res.h"
#include "coisane_util.h"


PCOISANE_Data g_pWizardPageData = NULL; // global instance of the COISANE data with device information

DWORD NewDeviceWizardFinishInstall(_In_ DI_FUNCTION InstallFunction, _In_ HDEVINFO hDeviceInfoSet, _In_ PSP_DEVINFO_DATA pDeviceInfoData)
{
	SP_NEWDEVICEWIZARD_DATA newDeviceWizardData;
	HPROPSHEETPAGE hPropSheetPage;
	PROPSHEETPAGE propSheetPage;
	PCOISANE_Data privateData;
	HANDLE hActCtx, hHeap;
	HINSTANCE hInstance;
	BOOL res;

	hActCtx = GetActivationContext();
	hInstance = GetModuleInstance();
	if (!hInstance)
		return ERROR_OUTOFMEMORY;

	hHeap = GetProcessHeap();
	if (!hHeap)
		return ERROR_OUTOFMEMORY;

	ZeroMemory(&newDeviceWizardData, sizeof(newDeviceWizardData));
	newDeviceWizardData.ClassInstallHeader.cbSize = sizeof(newDeviceWizardData.ClassInstallHeader);
	newDeviceWizardData.ClassInstallHeader.InstallFunction = InstallFunction;
	res = SetupDiGetClassInstallParams(hDeviceInfoSet, pDeviceInfoData, &newDeviceWizardData.ClassInstallHeader, sizeof(newDeviceWizardData), NULL);
	if (!res)
		return GetLastError();

	if (newDeviceWizardData.NumDynamicPages >= MAX_INSTALLWIZARD_DYNAPAGES - 1)
		return NO_ERROR;

	privateData = (PCOISANE_Data) HeapAlloc(hHeap, HEAP_ZERO_MEMORY, sizeof(COISANE_Data));
	if (!privateData)
		return ERROR_OUTOFMEMORY;

	privateData->hHeap = hHeap;
	privateData->hInstance = hInstance;
	privateData->hDeviceInfoSet = hDeviceInfoSet;
	privateData->pDeviceInfoData = pDeviceInfoData;

	ZeroMemory(&propSheetPage, sizeof(propSheetPage));
	propSheetPage.dwSize = sizeof(propSheetPage);
	propSheetPage.dwFlags = PSP_USECALLBACK | PSP_USEHEADERTITLE | PSP_USEHEADERSUBTITLE;
	propSheetPage.hActCtx = hActCtx;
	propSheetPage.hInstance = hInstance;
	propSheetPage.pfnDlgProc = &DialogProcWizardPageServer;
	propSheetPage.pfnCallback = &PropSheetPageProcWizardPage;
	propSheetPage.pszTemplate = MAKEINTRESOURCE(IDD_WIZARD_PAGE_SERVER);
	propSheetPage.pszHeaderTitle = MAKEINTRESOURCE(IDS_WIZARD_PAGE_SERVER_HEADER_TITLE);
	propSheetPage.pszHeaderSubTitle = MAKEINTRESOURCE(IDS_WIZARD_PAGE_SERVER_HEADER_SUBTITLE);
	propSheetPage.lParam = (LPARAM) privateData;

	hPropSheetPage = CreatePropertySheetPage(&propSheetPage);
	if (!hPropSheetPage)
		return GetLastError();
	
	newDeviceWizardData.DynamicPages[newDeviceWizardData.NumDynamicPages++] = hPropSheetPage;

	ZeroMemory(&propSheetPage, sizeof(propSheetPage));
	propSheetPage.dwSize = sizeof(propSheetPage);
	propSheetPage.dwFlags = PSP_USECALLBACK | PSP_USEHEADERTITLE | PSP_USEHEADERSUBTITLE;
	propSheetPage.hActCtx = hActCtx;
	propSheetPage.hInstance = hInstance;
	propSheetPage.pfnDlgProc = &DialogProcWizardPageScanner;
	propSheetPage.pfnCallback = &PropSheetPageProcWizardPage;
	propSheetPage.pszTemplate = MAKEINTRESOURCE(IDD_WIZARD_PAGE_SCANNER);
	propSheetPage.pszHeaderTitle = MAKEINTRESOURCE(IDS_WIZARD_PAGE_SCANNER_HEADER_TITLE);
	propSheetPage.pszHeaderSubTitle = MAKEINTRESOURCE(IDS_WIZARD_PAGE_SCANNER_HEADER_SUBTITLE);
	propSheetPage.lParam = (LPARAM) privateData;

	hPropSheetPage = CreatePropertySheetPage(&propSheetPage);
	if (!hPropSheetPage)
		return GetLastError();

	newDeviceWizardData.DynamicPages[newDeviceWizardData.NumDynamicPages++] = hPropSheetPage;

	res = SetupDiSetClassInstallParams(hDeviceInfoSet, pDeviceInfoData, &newDeviceWizardData.ClassInstallHeader, sizeof(newDeviceWizardData));
	if (!res)
		return GetLastError();

	return NO_ERROR;
}

INT_PTR CALLBACK DialogProcWizardPageServer(_In_ HWND hwndDlg, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	LPPROPSHEETPAGE lpPropSheetPage;
	PCOISANE_Data privateData;

	UNREFERENCED_PARAMETER(wParam);

	switch (uMsg) {
		case WM_INITDIALOG:
			Trace(TEXT("WM_INITDIALOG"));
			lpPropSheetPage = (LPPROPSHEETPAGE) lParam;
			privateData = (PCOISANE_Data) lpPropSheetPage->lParam;

			InitWizardPageServer(hwndDlg, privateData);
			SetWindowLongPtr(hwndDlg, GWLP_USERDATA, lParam);
			break;

		case WM_NOTIFY:
			Trace(TEXT("WM_NOTIFY"));
			lpPropSheetPage = (LPPROPSHEETPAGE) GetWindowLongPtr(hwndDlg, GWLP_USERDATA);
			if (!lpPropSheetPage)
				break;

			privateData = (PCOISANE_Data) lpPropSheetPage->lParam;
			if (!privateData)
				break;

			switch (((LPNMHDR) lParam)->code) {
				case PSN_SETACTIVE:
					Trace(TEXT("PSN_SETACTIVE"));
					PropSheet_SetWizButtons(((LPNMHDR) lParam)->hwndFrom, PSWIZB_NEXT);
					break;

				case PSN_WIZBACK:
					Trace(TEXT("PSN_WIZBACK"));
					break;

				case PSN_WIZNEXT:
					Trace(TEXT("PSN_WIZNEXT"));
					if (!NextWizardPageServer(hwndDlg, privateData)) {
						MessageBoxR(privateData->hHeap, privateData->hInstance, hwndDlg, IDS_DEVICE_OPEN_FAILED, IDS_PROPERTIES_SCANNER_DEVICE, MB_ICONERROR | MB_OK);
						SetWindowLongPtr(hwndDlg, DWLP_MSGRESULT, -1);
						return TRUE;
					}
					break;

				case PSN_WIZFINISH:
					Trace(TEXT("PSN_WIZFINISH"));
					break;

				case PSN_QUERYCANCEL:
					Trace(TEXT("PSN_QUERYCANCEL"));
					ChangeDeviceState(privateData->hDeviceInfoSet, privateData->pDeviceInfoData, DICS_DISABLE, DICS_FLAG_GLOBAL);
					break;
			}
			break;
	}

	return FALSE;
}

INT_PTR CALLBACK DialogProcWizardPageScanner(_In_ HWND hwndDlg, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	LPPROPSHEETPAGE lpPropSheetPage;
	PCOISANE_Data privateData;

	UNREFERENCED_PARAMETER(wParam);

	switch (uMsg) {
		case WM_INITDIALOG:
			Trace(TEXT("WM_INITDIALOG"));
			lpPropSheetPage = (LPPROPSHEETPAGE) lParam;
			privateData = (PCOISANE_Data) lpPropSheetPage->lParam;

			InitWizardPageScanner(hwndDlg, privateData);
			SetWindowLongPtr(hwndDlg, GWLP_USERDATA, lParam);
			break;

		case WM_NOTIFY:
			Trace(TEXT("WM_NOTIFY"));
			lpPropSheetPage = (LPPROPSHEETPAGE) GetWindowLongPtr(hwndDlg, GWLP_USERDATA);
			privateData = (PCOISANE_Data) lpPropSheetPage->lParam;

			switch (((LPNMHDR) lParam)->code) {
				case PSN_SETACTIVE:
					Trace(TEXT("PSN_SETACTIVE"));
					PropSheet_SetWizButtons(((LPNMHDR) lParam)->hwndFrom, PSWIZB_BACK | PSWIZB_NEXT);
					break;

				case PSN_WIZBACK:
					Trace(TEXT("PSN_WIZBACK"));
					break;

				case PSN_WIZNEXT:
					Trace(TEXT("PSN_WIZNEXT"));
					if (!NextWizardPageScanner(hwndDlg, privateData)) {
						MessageBoxR(privateData->hHeap, privateData->hInstance, hwndDlg, IDS_DEVICE_OPEN_FAILED, IDS_PROPERTIES_SCANNER_DEVICE, MB_ICONERROR | MB_OK);
						SetWindowLongPtr(hwndDlg, DWLP_MSGRESULT, -1);
						return TRUE;
					}
					break;

				case PSN_WIZFINISH:
					Trace(TEXT("PSN_WIZFINISH"));
					break;

				case PSN_QUERYCANCEL:
					Trace(TEXT("PSN_QUERYCANCEL"));
					ChangeDeviceState(privateData->hDeviceInfoSet, privateData->pDeviceInfoData, DICS_DISABLE, DICS_FLAG_GLOBAL);
					break;
			}
			break;
	}

	return FALSE;
}

UINT CALLBACK PropSheetPageProcWizardPage(_In_ HWND hwnd, _In_ UINT uMsg, _Inout_ LPPROPSHEETPAGE ppsp)
{
	PCOISANE_Data privateData;
	UINT ret;

	UNREFERENCED_PARAMETER(hwnd);

	ret = 0;

	if (ppsp && ppsp->lParam) {
		privateData = (PCOISANE_Data) ppsp->lParam;
	} else {
		privateData = NULL;
	}

	switch (uMsg) {
		case PSPCB_ADDREF:
			Trace(TEXT("PSPCB_ADDREF"));
			if (privateData) {
				privateData->uiReferences++;
			}
			break;

		case PSPCB_CREATE:
			Trace(TEXT("PSPCB_CREATE"));
			if (privateData) {
				ret = 1;
			}
			break;

		case PSPCB_RELEASE:
			Trace(TEXT("PSPCB_RELEASE"));
			if (privateData) {
				privateData->uiReferences--;

				if (privateData->uiReferences == 0) {
					if (privateData->lpHost)
						HeapFree(privateData->hHeap, 0, privateData->lpHost);
					if (privateData->lpName)
						HeapFree(privateData->hHeap, 0, privateData->lpName);
					if (privateData->lpUsername)
						HeapFree(privateData->hHeap, 0, privateData->lpUsername);
					if (privateData->lpPassword)
						HeapFree(privateData->hHeap, 0, privateData->lpPassword);

					HeapFree(privateData->hHeap, 0, privateData);
					ppsp->lParam = NULL;
				}
			}
			break;
	}

	return ret;
}

BOOL InitWizardPageServer(_In_ HWND hwndDlg, _Inout_ PCOISANE_Data privateData)
{
	INFCONTEXT InfContext;
	HINF InfFile;
	LPTSTR strField;
	INT intField;
	DWORD size;
	BOOL res;

	InfFile = OpenInfFile(privateData->hDeviceInfoSet, privateData->pDeviceInfoData, NULL);
	if (InfFile != INVALID_HANDLE_VALUE) {
		res = SetupFindFirstLine(InfFile, TEXT("WIASANE.DeviceData"), TEXT("Host"), &InfContext);
		if (res) {
			res = SetupGetStringField(&InfContext, 1, NULL, 0, &size);
			if (res) {
				strField = (LPTSTR) HeapAlloc(privateData->hHeap, HEAP_ZERO_MEMORY, size * sizeof(TCHAR));
				if (strField) {
					res = SetupGetStringField(&InfContext, 1, strField, size, NULL);
					if (res) {
						res = SetDlgItemText(hwndDlg, IDC_WIZARD_PAGE_SERVER_EDIT_HOST, strField);

						privateData->lpHost = strField;
					} else {
						HeapFree(privateData->hHeap, 0, strField);
					}
				} else {
					res = FALSE;
				}
			}
		}

		if (res) {
			res = SetupFindFirstLine(InfFile, TEXT("WIASANE.DeviceData"), TEXT("Port"), &InfContext);
			if (res) {
				res = SetupGetIntField(&InfContext, 1, &intField);
				if (res) {
					res = SetDlgItemInt(hwndDlg, IDC_WIZARD_PAGE_SERVER_EDIT_PORT, intField, FALSE);

					privateData->usPort = (USHORT) intField;
				}
			}
		}

		SetupCloseInfFile(InfFile);
	} else {
		res = FALSE;
	}

	if (!privateData->lpHost)
		privateData->lpHost = StringAClone(privateData->hHeap, TEXT("localhost"));

	if (!privateData->usPort)
		privateData->usPort = WINSANE_DEFAULT_PORT;

	return res;
}

BOOL NextWizardPageServer(_In_ HWND hwndDlg, _Inout_ PCOISANE_Data privateData)
{
	PWINSANE_Session oSession;
	LPTSTR lpHost;
	USHORT usPort;
	BOOL res;

	lpHost = (LPTSTR) HeapAlloc(privateData->hHeap, HEAP_ZERO_MEMORY, sizeof(TCHAR) * MAX_PATH);
	if (lpHost) {
		res = GetDlgItemText(hwndDlg, IDC_WIZARD_PAGE_SERVER_EDIT_HOST, lpHost, MAX_PATH);
		if (res) {
			if (privateData->lpHost) {
				HeapFree(privateData->hHeap, 0, privateData->lpHost);
			}
			privateData->lpHost = lpHost;
		} else {
			HeapFree(privateData->hHeap, 0, lpHost);
		}
	} else {
		res = FALSE;
	}

	if (res) {
		usPort = (USHORT) GetDlgItemInt(hwndDlg, IDC_WIZARD_PAGE_SERVER_EDIT_PORT, &res, FALSE);
		if (res) {
			privateData->usPort = usPort;

			oSession = WINSANE_Session::Remote(privateData->lpHost, privateData->usPort);
			if (oSession) {
				if (oSession->Init(NULL, NULL) == SANE_STATUS_GOOD) {
					res = oSession->Exit() == SANE_STATUS_GOOD;
				} else {
					res = FALSE;
				}
				delete oSession;
			} else {
				res = FALSE;
			}
		}
	}

	return res;
}

BOOL InitWizardPageScanner(_In_ HWND hwndDlg, _Inout_ PCOISANE_Data privateData)
{
	PWINSANE_Session oSession;
	PWINSANE_Device oDevice;
	LONG index;
	HWND hwnd;
	BOOL res;

	oSession = WINSANE_Session::Remote(privateData->lpHost, privateData->usPort);
	if (oSession) {
		if (oSession->Init(NULL, NULL) == SANE_STATUS_GOOD) {
			hwnd = GetDlgItem(hwndDlg, IDC_WIZARD_PAGE_SCANNER_COMBO_SCANNER);
			if (oSession->FetchDevices() == SANE_STATUS_GOOD) {
				SendMessageA(hwnd, CB_RESETCONTENT, (WPARAM) 0, (LPARAM) 0);
				for (index = 0; index < oSession->GetDevices(); index++) {
					oDevice = oSession->GetDevice(index);
					if (oDevice) {
						SendMessageA(hwnd, CB_ADDSTRING, (WPARAM) 0, (LPARAM) oDevice->GetName());
					}
				}
			}
			res = oSession->Exit() == SANE_STATUS_GOOD;
		} else {
			res = FALSE;
		}
		delete oSession;
	} else {
		res = FALSE;
	}

	return res;
}

BOOL NextWizardPageScanner(_In_ HWND hwndDlg, _Inout_ PCOISANE_Data privateData)
{
	PWINSANE_Session oSession;
	PWINSANE_Device oDevice;
	LPTSTR lpName;
	LPTSTR lpUsername;
	LPTSTR lpPassword;
	BOOL res;

	lpName = (LPTSTR) HeapAlloc(privateData->hHeap, HEAP_ZERO_MEMORY, sizeof(TCHAR) * MAX_PATH);
	if (lpName) {
		res = GetDlgItemText(hwndDlg, IDC_WIZARD_PAGE_SCANNER_COMBO_SCANNER, lpName, MAX_PATH);
		if (res) {
			if (privateData->lpName) {
				HeapFree(privateData->hHeap, 0, privateData->lpName);
			}
			privateData->lpName = lpName;
		} else {
			HeapFree(privateData->hHeap, 0, lpName);
		}
	} else {
		res = FALSE;
	}

	if (res) {
		lpUsername = (LPTSTR) HeapAlloc(privateData->hHeap, HEAP_ZERO_MEMORY, sizeof(TCHAR) * MAX_PATH);
		if (lpUsername) {
			if (GetDlgItemText(hwndDlg, IDC_WIZARD_PAGE_SCANNER_EDIT_USERNAME, lpUsername, MAX_PATH)) {
				if (privateData->lpUsername) {
					HeapFree(privateData->hHeap, 0, privateData->lpUsername);
				}
				privateData->lpUsername = lpUsername;
			} else {
				HeapFree(privateData->hHeap, 0, lpUsername);
			}
		}

		lpPassword = (LPTSTR) HeapAlloc(privateData->hHeap, HEAP_ZERO_MEMORY, sizeof(TCHAR) * MAX_PATH);
		if (lpPassword) {
			if (GetDlgItemText(hwndDlg, IDC_WIZARD_PAGE_SCANNER_EDIT_PASSWORD, lpPassword, MAX_PATH)) {
				if (privateData->lpPassword) {
					HeapFree(privateData->hHeap, 0, privateData->lpPassword);
				}
				privateData->lpPassword = lpPassword;
			} else {
				HeapFree(privateData->hHeap, 0, lpPassword);
			}
		}

		oSession = WINSANE_Session::Remote(privateData->lpHost, privateData->usPort);
		if (oSession) {
			g_pWizardPageData = privateData;
			if (oSession->Init(NULL, &WizardPageAuthCallback) == SANE_STATUS_GOOD) {
				if (oSession->FetchDevices() == SANE_STATUS_GOOD) {
					oDevice = oSession->GetDevice(privateData->lpName);
					if (oDevice) {
						UpdateDeviceInfo(privateData, oDevice);
						UpdateDeviceData(privateData, oDevice);

						ChangeDeviceState(privateData->hDeviceInfoSet, privateData->pDeviceInfoData, DICS_ENABLE, DICS_FLAG_GLOBAL);
						ChangeDeviceState(privateData->hDeviceInfoSet, privateData->pDeviceInfoData, DICS_PROPCHANGE, DICS_FLAG_GLOBAL);

						res = TRUE;
					} else {
						res = FALSE;
					}
				} else {
					res = FALSE;
				}
				if (oSession->Exit() != SANE_STATUS_GOOD) {
					res = FALSE;
				}
			} else {
				res = FALSE;
			}
			g_pWizardPageData = NULL;
			delete oSession;
		} else {
			res = FALSE;
		}
	}

	return res;
}


WINSANE_API_CALLBACK WizardPageAuthCallback(_In_ SANE_String_Const resource, _Inout_ SANE_Char *username, _Inout_ SANE_Char *password)
{
	LPSTR lpUsername, lpPassword;
	HANDLE hHeap;

	if (!g_pWizardPageData || !resource || !strlen(resource) || !username || !password)
		return;

	Trace(TEXT("------ WizardPageAuthCallback(resource='%hs') ------"), resource);

	hHeap = GetProcessHeap();
	if (!hHeap)
		return;

	lpUsername = StringToA(hHeap, g_pWizardPageData->lpUsername);
	if (lpUsername) {
		lpPassword = StringToA(hHeap, g_pWizardPageData->lpPassword);
		if (lpPassword) {
			strcpy_s(username, SANE_MAX_USERNAME_LEN, lpUsername);
			strcpy_s(password, SANE_MAX_PASSWORD_LEN, lpPassword);
			HeapFree(hHeap, 0, lpPassword);
		}
		HeapFree(hHeap, 0, lpUsername);
	}

	Trace(TEXT("Username: %hs (%d)"), username, strlen(username));
	Trace(TEXT("Password: ******** (%d)"), strlen(password));
}
