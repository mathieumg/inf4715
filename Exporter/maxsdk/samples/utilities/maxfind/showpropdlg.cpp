/****************************************************************************
MAX File Finder
Christer Janson
September 20, 1998
ShowPropDlg.cpp - Property dialog implementation
***************************************************************************/
#include "pch.h"

#include "MaxFindApp.h"
#include "resource.h"
#include <maxstring.h>
#include <maxvariant.h>
#include <strclass.h>

INT_PTR CALLBACK	PropDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
void VariantToString(MaxFindApp* pApp, PROPVARIANT* pProp, TCHAR* szString, int bufSize);
void TypeNameFromVariant(MaxFindApp* pApp, PROPVARIANT* pProp, TCHAR* szString, int bufSize);

HINSTANCE		hInstance;

void MaxFindApp::ShowProperties()
{
	hInstance = GetInstance();

	if (!hPropDialog) {
		hPropDialog = CreateDialogParam(
			GetInstance(),
			MAKEINTRESOURCE(IDD_SHOW_PROPERTIES),
			GetHWnd(),
			(DLGPROC)PropDlgProc,
			(LPARAM)this);
	}
	else {
		SetActiveWindow(hPropDialog);
		GetProperties();
	}
}

INT_PTR CALLBACK PropDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	MaxFindApp* pApp = reinterpret_cast<MaxFindApp*>(static_cast<LONG_PTR>(GetWindowLongPtr(hWnd, GWLP_USERDATA)));

	switch (message) {
		case WM_INITDIALOG:
			{
				pApp = (MaxFindApp*)lParam;
				SetWindowLongPtr(hWnd, GWLP_USERDATA, lParam);
				SetCursor(LoadCursor(NULL,IDC_ARROW));
				SetWindowPos(hWnd, HWND_TOP, pApp->regsettings.propdlgx, pApp->regsettings.propdlgy, 0, 0, 
					SWP_NOOWNERZORDER|SWP_NOSIZE);

				pApp->hPropDialog = hWnd;

				// Arrange the columns in the list view
				LV_COLUMN column;
				column.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
				column.fmt = LVCFMT_LEFT;
				column.pszText = pApp->GetString(IDS_CUST_NAME);
				column.cx = 80;
				ListView_InsertColumn(GetDlgItem(hWnd, IDC_CUSTOM), 0, &column);
				column.pszText = pApp->GetString(IDS_CUST_VALUE);
				ListView_InsertColumn(GetDlgItem(hWnd, IDC_CUSTOM), 1, &column);
				column.pszText = pApp->GetString(IDS_CUST_TYPE);
				ListView_InsertColumn(GetDlgItem(hWnd, IDC_CUSTOM), 2, &column);

				pApp->GetProperties();
			}
			return 1;
		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;
		case WM_DESTROY:
			{
				RECT wr;
				GetWindowRect(hWnd, &wr);
				pApp->regsettings.propdlgx = wr.left;
				pApp->regsettings.propdlgy = wr.top;
			}
			pApp->hPropDialog = NULL;
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
		case IDOK:
		case IDCANCEL:
			DestroyWindow(hWnd);
			break;
		case IDC_PREV:
			{
				int cur = SendMessage(pApp->hListBox, LB_GETCURSEL, 0, 0);
				//int tot = SendMessage(pApp->hListBox, LB_GETCOUNT, 0, 0);
				if (cur > 0) {
					SendMessage(pApp->hListBox, LB_SETCURSEL, cur-1, 0);
					pApp->GetProperties();
				}
			}
			break;
		case IDC_NEXT:
			{
				int cur = SendMessage(pApp->hListBox, LB_GETCURSEL, 0, 0);
				int tot = SendMessage(pApp->hListBox, LB_GETCOUNT, 0, 0);
				if (cur < (tot-1)) {
					SendMessage(pApp->hListBox, LB_SETCURSEL, cur+1, 0);
					pApp->GetProperties();
				}
			}
			break;

			}
			return 1;
	}
	return 0;
}

using namespace MaxSDK::Util;

void MaxFindApp::GetProperties()
{
	LPSTORAGE				pStorage = NULL;
	IPropertySetStorage*	pPropertySetStorage = NULL;
	IPropertyStorage*		pSummaryInfoStorage = NULL;
	IPropertyStorage*		pDocumentSummaryInfoStorage = NULL;
	IPropertyStorage*		pUserDefinedPropertyStorage = NULL;
	TCHAR					szBuf[256] = {0};
	TCHAR					filename[MAX_PATH];

	SendMessage(GetDlgItem(hPropDialog, IDC_TITLE), WM_SETTEXT, 0, (LPARAM)_T(""));
	SendMessage(GetDlgItem(hPropDialog, IDC_SUBJECT), WM_SETTEXT, 0, (LPARAM)_T(""));
	SendMessage(GetDlgItem(hPropDialog, IDC_AUTHOR), WM_SETTEXT, 0, (LPARAM)_T(""));
	SendMessage(GetDlgItem(hPropDialog, IDC_MANAGER), WM_SETTEXT, 0, (LPARAM)_T(""));
	SendMessage(GetDlgItem(hPropDialog, IDC_COMPANY), WM_SETTEXT, 0, (LPARAM)_T(""));
	SendMessage(GetDlgItem(hPropDialog, IDC_CATEGORY), WM_SETTEXT, 0, (LPARAM)_T(""));
	SendMessage(GetDlgItem(hPropDialog, IDC_KEYWORDS), WM_SETTEXT, 0, (LPARAM)_T(""));
	SendMessage(GetDlgItem(hPropDialog, IDC_COMMENTS), WM_SETTEXT, 0, (LPARAM)_T(""));
	SendMessage(GetDlgItem(hPropDialog, IDC_CONTENTS), LB_RESETCONTENT, 0, 0);
	ListView_DeleteAllItems(GetDlgItem(hPropDialog, IDC_CUSTOM));

	int idx = SendMessage(hListBox, LB_GETCURSEL, 0, 0);

	SendMessage(hListBox, LB_GETTEXT, idx, (LPARAM)filename);
	SetWindowText(hPropDialog, filename);

	HRESULT	res = StgOpenStorage(TSTR(filename).ToUTF16(), (LPSTORAGE)0, STGM_DIRECT|STGM_READ|STGM_SHARE_DENY_WRITE, NULL,0,&pStorage);
	if (res!=S_OK) {
		return;
	}

	// Get the Storage interface
	if (S_OK != pStorage->QueryInterface(IID_IPropertySetStorage, (void**)&pPropertySetStorage)) {
		pStorage->Release();
		return;
	}

	// Get the SummaryInfo property set interface
	if (S_OK == pPropertySetStorage->Open(FMTID_SummaryInformation, STGM_READ|STGM_SHARE_EXCLUSIVE, &pSummaryInfoStorage)) {
		PROPSPEC	PropSpec[6];
		PROPVARIANT	PropVar[6];
		UINT        CodePage;

		PropSpec[0].ulKind = PRSPEC_PROPID;
		PropSpec[0].propid = PID_TITLE;

		PropSpec[1].ulKind = PRSPEC_PROPID;
		PropSpec[1].propid = PID_SUBJECT;

		PropSpec[2].ulKind = PRSPEC_PROPID;
		PropSpec[2].propid = PID_AUTHOR;

		PropSpec[3].ulKind = PRSPEC_PROPID;
		PropSpec[3].propid = PID_KEYWORDS;

		PropSpec[4].ulKind = PRSPEC_PROPID;
		PropSpec[4].propid = PID_COMMENTS;

		PropSpec[5].ulKind = PRSPEC_PROPID;
		PropSpec[5].propid = PID_CODEPAGE;

		HRESULT hr = pSummaryInfoStorage->ReadMultiple(6, PropSpec, PropVar);
		if (S_OK == hr) {

			if ((PropVar[5].vt    == VT_I2) &&
				(PropVar[5].uiVal != 1200))
			{
				CodePage = PropVar[5].uiVal;
			}
			else 
			{
				// FYI. 1200 means "Unicode". UTF-16 on disk, but unfortunately translated to ACP by the IPropertyStorage
				// when loaded in memory.
				CodePage = CP_ACP;
			}

			if (PropVar[0].vt != VT_EMPTY) {
				SetDlgItemText(hPropDialog, IDC_TITLE, VariantToString(PropVar[0], CodePage).ToMCHAR());
			}
			if (PropVar[1].vt != VT_EMPTY) {
				SetDlgItemText(hPropDialog, IDC_SUBJECT, VariantToString(PropVar[1], CodePage).ToMCHAR());
			}
			if (PropVar[2].vt != VT_EMPTY) {
				SetDlgItemText(hPropDialog, IDC_AUTHOR, VariantToString(PropVar[2], CodePage).ToMCHAR());
			}
			if (PropVar[3].vt != VT_EMPTY) {
				SetDlgItemText(hPropDialog, IDC_KEYWORDS, VariantToString(PropVar[3], CodePage).ToMCHAR());
			}
			if (PropVar[4].vt != VT_EMPTY) {
				SetDlgItemText(hPropDialog, IDC_COMMENTS, VariantToString(PropVar[4], CodePage).ToMCHAR());
			}
		}

		FreePropVariantArray(6, PropVar);
		pSummaryInfoStorage->Release();
	}


	// Get the DocumentSummaryInfo property set interface
	if (S_OK == pPropertySetStorage->Open(FMTID_DocSummaryInformation, STGM_READ|STGM_SHARE_EXCLUSIVE, &pDocumentSummaryInfoStorage)) {
		PROPSPEC	PropSpec[6];
		PROPVARIANT	PropVar[6];
		UINT        CodePage;

		PropSpec[0].ulKind = PRSPEC_PROPID;
		PropSpec[0].propid = PID_MANAGER;

		PropSpec[1].ulKind = PRSPEC_PROPID;
		PropSpec[1].propid = PID_COMPANY;

		PropSpec[2].ulKind = PRSPEC_PROPID;
		PropSpec[2].propid = PID_CATEGORY;

		PropSpec[3].ulKind = PRSPEC_PROPID;
		PropSpec[3].propid = PID_HEADINGPAIR;

		PropSpec[4].ulKind = PRSPEC_PROPID;
		PropSpec[4].propid = PID_DOCPARTS;

		PropSpec[5].ulKind = PRSPEC_PROPID;
		PropSpec[5].propid = PID_CODEPAGE;

		HRESULT hr = pDocumentSummaryInfoStorage->ReadMultiple(6, PropSpec, PropVar);
		if (S_OK == hr) {

			if ((PropVar[5].vt    == VT_I2) &&
				(PropVar[5].uiVal != 1200))
			{
				CodePage = PropVar[5].uiVal;
			}
			else 
			{
				// FYI. 1200 means "Unicode". UTF-16 on disk, but unfortunately translated to ACP by the IPropertyStorage
				// when loaded in memory.
				CodePage = CP_ACP;
			}

			if (PropVar[0].vt != VT_EMPTY) {
				SetDlgItemText(hPropDialog, IDC_MANAGER, VariantToString(PropVar[0], CodePage).ToMCHAR());
			}
			if (PropVar[1].vt != VT_EMPTY) {
				SetDlgItemText(hPropDialog, IDC_COMPANY, VariantToString(PropVar[1], CodePage).ToMCHAR());
			}
			if (PropVar[2].vt != VT_EMPTY) {
				SetDlgItemText(hPropDialog, IDC_CATEGORY, VariantToString(PropVar[2], CodePage).ToMCHAR());
			}
			if ((PropVar[3].vt == (VT_VARIANT | VT_VECTOR)) && VariantIsStringVector(PropVar[4])) {
				CAPROPVARIANT*	pHeading = &PropVar[3].capropvar;
				std::vector<MaxString> pDocPart;

				VariantToStringVector(PropVar[4], pDocPart, CodePage);

				// Headings:
				// =========
				// 0  - General
				// 2  - Mesh Totals
				// 4  - Scene Totals
				// 6  - External Dependencies
				// 8  - Objects
				// 10 - Materials
				// 12 - Plug-Ins

				int nDocPart = 0;
				for (UINT i=0; i<pHeading->cElems; i+=2) {
					SendMessage(GetDlgItem(hPropDialog, IDC_CONTENTS), LB_ADDSTRING, 0, (LPARAM)(LPCTSTR)VariantToString(pHeading->pElems[i], CodePage).ToMCHAR());
					for (int j=0; j<pHeading->pElems[i+1].lVal; j++) {
						MaxString szBuf(MaxString::FromMCHAR(_T("\t")));

						szBuf += pDocPart[nDocPart];
						SendMessage(GetDlgItem(hPropDialog, IDC_CONTENTS), LB_ADDSTRING, 0, (LPARAM)(LPCTSTR)szBuf.ToMCHAR());
						nDocPart++;
					}
				}

			}

		}

		FreePropVariantArray(6, PropVar);
		pDocumentSummaryInfoStorage->Release();
	}

	if (S_OK == pPropertySetStorage->Open(FMTID_UserDefinedProperties, STGM_READ|STGM_SHARE_EXCLUSIVE, &pUserDefinedPropertyStorage)) {
		int		numUserProps = 0;

		// First we need to count the properties
		IEnumSTATPROPSTG*	pIPropertyEnum;
		if (S_OK == pUserDefinedPropertyStorage->Enum(&pIPropertyEnum)) {
			STATPROPSTG prop;
			while (pIPropertyEnum->Next(1, &prop, NULL) == S_OK) {
				if (prop.lpwstrName) {
					CoTaskMemFree(prop.lpwstrName);
					prop.lpwstrName = NULL;
					numUserProps++;
				}
			}

			PROPSPEC* pPropSpec = new PROPSPEC[numUserProps+1];
			PROPVARIANT* pPropVar = new PROPVARIANT[numUserProps+1];

			ZeroMemory(pPropVar, (numUserProps+1)*sizeof(PROPVARIANT));
			ZeroMemory(pPropSpec, (numUserProps+1)*sizeof(PROPSPEC));

			pPropSpec[numUserProps].ulKind = PRSPEC_PROPID;
			pPropSpec[numUserProps].propid = 1;

			pIPropertyEnum->Reset();
			int idx = 0;
			while (pIPropertyEnum->Next(1, &prop, NULL) == S_OK) {
				if (prop.vt) {
					if (prop.lpwstrName) {
						pPropSpec[idx].ulKind = PRSPEC_LPWSTR;
						pPropSpec[idx].lpwstr = prop.lpwstrName;
					} else {
						pPropSpec[idx].ulKind = PRSPEC_PROPID;
						pPropSpec[idx].propid = prop.propid;
					}

					prop.lpwstrName = NULL;
					idx++;
				}
			}
			pIPropertyEnum->Release();

			ListView_DeleteAllItems(GetDlgItem(hPropDialog, IDC_CUSTOM));
			HRESULT hr = pUserDefinedPropertyStorage->ReadMultiple(numUserProps+1, pPropSpec, pPropVar);
			if (S_OK == hr) {

				UINT CodePage = CP_ACP;
				if ((pPropVar[numUserProps].vt    == VT_I2) &&
					(pPropVar[numUserProps].uiVal != 1200))
				{
					CodePage = pPropVar[numUserProps].uiVal;
				}
				else 
				{
					// FYI. 1200 means "Unicode". UTF-16 on disk, but unfortunately translated to ACP by the IPropertyStorage
					// when loaded in memory.
					CodePage = CP_ACP;
				}

				for (int i=0; i<numUserProps; i++) {
					_tcsncpy(szBuf, WStr(pPropSpec[i].lpwstr).ToMCHAR(), 255);
					LV_ITEM item;
					item.mask = LVIF_TEXT;
					item.iItem = i;
					item.iSubItem = 0;
					item.pszText = szBuf;
					item.cchTextMax = static_cast<int>(_tcslen(szBuf));	// SR DCAST64: Downcast to 2G limit.
					ListView_InsertItem(GetDlgItem(hPropDialog, IDC_CUSTOM), &item);

					_tcsncpy(szBuf, VariantToString(&pPropVar[i], CodePage).ToMCHAR(), 255);
					item.iSubItem = 1;
					item.pszText = szBuf;
					item.cchTextMax = static_cast<int>(_tcslen(szBuf));	// SR DCAST64: Downcast to 2G limit.
					ListView_SetItem(GetDlgItem(hPropDialog, IDC_CUSTOM), &item);

					TypeNameFromVariant(this, &pPropVar[i], szBuf, 255);
					item.iSubItem = 2;
					item.pszText = szBuf;
					item.cchTextMax = static_cast<int>(_tcslen(szBuf));	// SR DCAST64: Downcast to 2G limit.
					ListView_SetItem(GetDlgItem(hPropDialog, IDC_CUSTOM), &item);
				}
			}

			for (int i=0; i<idx; i++) {
				CoTaskMemFree(pPropSpec[i].lpwstr);
			}

			FreePropVariantArray(numUserProps, pPropVar);

			delete [] pPropSpec;
			delete [] pPropVar;
		}

		pUserDefinedPropertyStorage->Release();
	}

	pPropertySetStorage->Release();
	pStorage->Release();
}

void TypeNameFromVariant(MaxFindApp* pApp, PROPVARIANT* pProp, TCHAR* szString, int bufSize)
{
	switch (pProp->vt) {
		case VT_LPWSTR:
		case VT_LPSTR:
			_tcscpy_s(szString, bufSize, pApp->GetString(IDS_TYPE_TEXT));
			break;
		case VT_I4:
		case VT_R4:
		case VT_R8:
		case VT_I8:
		case VT_INT:
		case VT_INT_PTR:
			_tcscpy_s(szString, bufSize, pApp->GetString(IDS_TYPE_NUMBER));
			break;
		case VT_BOOL:
			_tcscpy_s(szString, bufSize, pApp->GetString(IDS_TYPE_BOOL));
			break;
		case VT_FILETIME:
			_tcscpy_s(szString, bufSize, pApp->GetString(IDS_TYPE_DATE));
			break;
		default:
			_tcscpy_s(szString, bufSize, _T(""));
			break;
	}
}

void VariantToString(MaxFindApp* pApp, PROPVARIANT* pProp, TCHAR* szString, int bufSize)
{
	switch (pProp->vt) {
		case VT_LPWSTR:
			_tcscpy(szString, WStr(pProp->pwszVal).ToMCHAR());
			break;
		case VT_LPSTR:
			_tcscpy(szString, CStr(pProp->pszVal).ToMCHAR());
			break;
#if !defined( WIN64 )
		case VT_INT_PTR:
#endif
		case VT_I4:
			_stprintf(szString, _T("%ld"), pProp->lVal);
			break;
#if defined( WIN64 )
		case VT_INT_PTR:
#endif
		case VT_I8:
			_stprintf(szString, _T("%I64d"), pProp->hVal.QuadPart);
			break;
		case VT_INT:
			_stprintf(szString, _T("%d"), pProp->intVal);
			break;
		case VT_R4:
			_stprintf(szString, _T("%f"), pProp->fltVal);
			break;
		case VT_R8:
			_stprintf(szString, _T("%lf"), pProp->dblVal);
			break;
		case VT_BOOL:
			_stprintf(szString, _T("%s"), pProp->boolVal ? pApp->GetString(IDS_VAL_YES) : pApp->GetString(IDS_VAL_NO));
			break;
		case VT_FILETIME:
			SYSTEMTIME sysTime;
			FileTimeToSystemTime(&pProp->filetime, &sysTime);
			GetDateFormat(LOCALE_SYSTEM_DEFAULT,
				DATE_SHORTDATE,
				&sysTime,
				NULL,
				szString,
				bufSize);
			break;
		default:
			_tcscpy(szString, _T(""));
			break;
	}
}
