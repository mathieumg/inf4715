/****************************************************************************
 MAX File Finder
 Christer Janson
 September 19, 1998
 MaxFindApp.cpp - Main Application and Search Methods
 ***************************************************************************/
#include "pch.h"

#include "MaxFindApp.h"
#include "resource.h"
#include <utilexp.h>
#include <utilintf.h>
#include <buildver.h>
#include <strbasic.h>
#include <strclass.h>
#include <maxvariant.h>

LRESULT	CALLBACK AppWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT	CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK statPanDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
// WIN64 Cleanup: Martell
BOOL BrowseFolder(HINSTANCE hInstance, HWND hParent);

static char* useFolder;

using namespace MaxSDK::Util;

const TCHAR* GetRegistryKey()	{

#ifndef DESIGN_VER
#define REGISTRY_KEY _T("max Finder")
#else
#define REGISTRY_KEY _T("VIZ Finder")
#endif // !DESIGN_VER

	static TCHAR regKey[MAX_PATH];
	static bool init = true;
	if(init)	{
		_sntprintf(regKey, MAX_PATH, _T("%s\\%s"), UtilityInterface::GetRegistryKeyBase(), REGISTRY_KEY);
		init = false;
	}
	return regKey;
}

MaxFindApp::MaxFindApp(HINSTANCE hInst, int cshow)
{
	hInstance = hInst;
	cmdShow = cshow;
	hPropDialog = NULL;
	InitializeCriticalSection(&cs);
	bSearchActive = FALSE;
	hThreadExitRequest = CreateEvent(NULL, TRUE, FALSE, NULL);
}

MaxFindApp::~MaxFindApp()
	{
	DeleteCriticalSection(&cs);
	CloseHandle(hThreadExitRequest);
	if (hFont) DeleteObject(hFont);
	}

BOOL MaxFindApp::Init()
	{
	WNDCLASSEX	wc;

	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = AppWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
//	wc.hIconSm		 = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAINWND));
//	wc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAINWND));
	wc.hIconSm		 = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL; //(HBRUSH)(COLOR_MENU+1);
	wc.lpszMenuName  = MAKEINTRESOURCE(IDR_MAINMENU);
	wc.lpszClassName = THECLASSNAME;

	if (!RegisterClassEx(&wc))
		return FALSE;

	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = StaticWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIconSm		 = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_MENU+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = _T("CJSTATIC");

	if (!RegisterClassEx(&wc))
		return FALSE;

	LOGFONT lf;
	GetObject(GetStockObject(SYSTEM_FONT), sizeof(lf), &lf);
	lf.lfWeight = 400;
	lf.lfHeight = 14;
	lf.lfWidth = 0;
	_tcscpy((TCHAR*)&lf.lfFaceName, GetString(IDS_APP_FONT)); // mjm - 2.10.99 - for localization
//	strcpy((char *)&lf.lfFaceName, "MS Sans Serif");
	hFont = CreateFontIndirect(&lf);

	GetRegSettings();

	return TRUE;
	}

void MaxFindApp::GetRegSettings()
	{
	HKEY	hKey;

	regsettings.x = CW_USEDEFAULT;
	regsettings.y = CW_USEDEFAULT;
	regsettings.w = 500;
	regsettings.h = 500;
	regsettings.filespec = 0;
	regsettings.propdlgx = 100;
	regsettings.propdlgy = 100;

	if (RegOpenKeyEx(HKEY_CURRENT_USER, GetRegistryKey(), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
		DWORD nSize = sizeof(DWORD);
		RegQueryValueEx(hKey, _T("InitX"), 0, 0, (LPBYTE)&regsettings.x, &nSize);
		nSize = sizeof(DWORD);
		RegQueryValueEx(hKey, _T("InitY"), 0, 0, (LPBYTE)&regsettings.y, &nSize);
		nSize = sizeof(DWORD);
		RegQueryValueEx(hKey, _T("InitW"), 0, 0, (LPBYTE)&regsettings.w, &nSize);
		nSize = sizeof(DWORD);
		RegQueryValueEx(hKey, _T("InitH"), 0, 0, (LPBYTE)&regsettings.h, &nSize);
		nSize = sizeof(DWORD);
		RegQueryValueEx(hKey, _T("InitFileSpec"), 0, 0, (LPBYTE)&regsettings.filespec, &nSize);
		nSize = sizeof(DWORD);
		RegQueryValueEx(hKey, _T("InitPropDlgX"), 0, 0, (LPBYTE)&regsettings.propdlgx, &nSize);
		nSize = sizeof(DWORD);
		RegQueryValueEx(hKey, _T("InitPropDlgY"), 0, 0, (LPBYTE)&regsettings.propdlgy, &nSize);
		}
	}

void MaxFindApp::SetRegSettings()
	{
	HKEY	hKey;
	DWORD	disp;
	if (RegCreateKeyEx(HKEY_CURRENT_USER, GetRegistryKey(), 0, _T(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &disp) == ERROR_SUCCESS) {
		RegSetValueEx(hKey, _T("InitX"), 0, REG_DWORD, (LPBYTE)&regsettings.x, sizeof(DWORD));
		RegSetValueEx(hKey, _T("InitY"), 0, REG_DWORD, (LPBYTE)&regsettings.y, sizeof(DWORD));
		RegSetValueEx(hKey, _T("InitW"), 0, REG_DWORD, (LPBYTE)&regsettings.w, sizeof(DWORD));
		RegSetValueEx(hKey, _T("InitH"), 0, REG_DWORD, (LPBYTE)&regsettings.h, sizeof(DWORD));
		RegSetValueEx(hKey, _T("InitFileSpec"), 0, REG_DWORD, (LPBYTE)&regsettings.filespec, sizeof(DWORD));
		RegSetValueEx(hKey, _T("InitPropDlgX"), 0, REG_DWORD, (LPBYTE)&regsettings.propdlgx, sizeof(DWORD));
		RegSetValueEx(hKey, _T("InitPropDlgY"), 0, REG_DWORD, (LPBYTE)&regsettings.propdlgy, sizeof(DWORD));
		}
	}


void MaxFindApp::PostInit()
	{
	// russom - 06/21/01
	// we don't have access to resmgr - ugh.
	// Product specific code here:
#if defined(GAME_VER)
	// Add .gmax for both free and dev
	SendMessage(hFileSpec, CB_ADDSTRING, 0, (LPARAM)_T("*.gmax"));
	// If this is dev (not free), also add max
  #if !defined(GAME_FREE_VER)
	SendMessage(hFileSpec, CB_ADDSTRING, 0, (LPARAM)_T("*.max"));
  #endif
#else
	SendMessage(hFileSpec, CB_ADDSTRING, 0, (LPARAM)_T("*.max"));
#endif
	SendMessage(hFileSpec, CB_SETCURSEL, regsettings.filespec, 0);
	PostMessage(hFileSpec, CB_SETEDITSEL, 0, MAKELPARAM(-1, 0));

	int idx;
	idx = SendMessage(hProperty, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_PROP_ALL));
		SendMessage(hProperty, CB_SETITEMDATA, idx, (LPARAM)ALL_PROPERTIES);
	idx = SendMessage(hProperty, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_PROP_TITLE));
		SendMessage(hProperty, CB_SETITEMDATA, idx, (LPARAM)TITLE_PROP);
	idx = SendMessage(hProperty, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_PROP_SUBJECT));
		SendMessage(hProperty, CB_SETITEMDATA, idx, (LPARAM)SUBJECT_PROP);
	idx = SendMessage(hProperty, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_PROP_AUTHOR));
		SendMessage(hProperty, CB_SETITEMDATA, idx, (LPARAM)AUTHOR_PROP);
	idx = SendMessage(hProperty, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_PROP_MANAGER));
		SendMessage(hProperty, CB_SETITEMDATA, idx, (LPARAM)MANAGER_PROP);
	idx = SendMessage(hProperty, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_PROP_COMPANY));
		SendMessage(hProperty, CB_SETITEMDATA, idx, (LPARAM)COMPANY_PROP);
	idx = SendMessage(hProperty, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_PROP_CATEGORY));
		SendMessage(hProperty, CB_SETITEMDATA, idx, (LPARAM)CATEGORY_PROP);
	idx = SendMessage(hProperty, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_PROP_KEYWORDS));
		SendMessage(hProperty, CB_SETITEMDATA, idx, (LPARAM)KEYWORDS_PROP);
	idx = SendMessage(hProperty, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_PROP_COMMENTS));
		SendMessage(hProperty, CB_SETITEMDATA, idx, (LPARAM)COMMENTS_PROP);
	idx = SendMessage(hProperty, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_PROP_EXTDEP));
		SendMessage(hProperty, CB_SETITEMDATA, idx, (LPARAM)EXT_DEPEND_PROP);
	idx = SendMessage(hProperty, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_PROP_PLUGINS));
		SendMessage(hProperty, CB_SETITEMDATA, idx, (LPARAM)PLUGINS_PROP);
	idx = SendMessage(hProperty, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_PROP_OBJECTS));
		SendMessage(hProperty, CB_SETITEMDATA, idx, (LPARAM)OBJECTS_PROP);
	idx = SendMessage(hProperty, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_PROP_MATERIALS));
		SendMessage(hProperty, CB_SETITEMDATA, idx, (LPARAM)MATERIALS_PROP);
	idx = SendMessage(hProperty, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_PROP_CUSTOM));
		SendMessage(hProperty, CB_SETITEMDATA, idx, (LPARAM)USER_PROP);
	
	SendMessage(hProperty, CB_SETCURSEL, 0, 0);

	SendMessage(hCheckSubdirs, BM_SETCHECK, BST_CHECKED, 0);

	SetFocus(hSearchText);
	DoStatusDirectory();
	}

void MaxFindApp::CreateControls()
	{
	hControlPane = CreateDialogParam(hInstance,
				MAKEINTRESOURCE(IDD_PANEL),
				hMainWnd,
				statPanDlgProc,
				(LPARAM)this);

	hTxtSearch		= GetDlgItem(hControlPane, IDC_TXTSEARCH);
	hTxtFileSpec	= GetDlgItem(hControlPane, IDC_TXTFILESPEC);
	hTxtProperty	= GetDlgItem(hControlPane, IDC_TXTPROPERTY);
	hFindButton		= GetDlgItem(hControlPane, IDC_START);
	hCDButton		= GetDlgItem(hControlPane, IDC_BROWSE);
	hFileSpec		= GetDlgItem(hControlPane, IDC_FILESPEC);
	hProperty		= GetDlgItem(hControlPane, IDC_PROPERTY);
	hSearchText		= GetDlgItem(hControlPane, IDC_SEARCHEDIT);
	hCheckSubdirs	= GetDlgItem(hControlPane, IDC_SUBDIRS);
	hStatusPanel	= GetDlgItem(hControlPane, IDC_STATUSPANEL);
	hDlgFrame		= GetDlgItem(hControlPane, IDC_BORDER);

	hListBox = CreateWindow(_T("LISTBOX"),
				 _T(""),
				 WS_VISIBLE | WS_CHILD | WS_BORDER | LBS_NOINTEGRALHEIGHT | WS_VSCROLL | WS_TABSTOP | LBS_NOTIFY,
				 0, 0,
				 0, 0,
				 hMainWnd,
				 (HMENU)IDC_LISTBOX,
				 hInstance,
				 NULL);

	SendMessage(hListBox, WM_SETFONT, (WPARAM)GetAppFont(), 0);

	}

void MaxFindApp::Move()
	{
	RECT	wr;
	GetWindowRect(hMainWnd, &wr);

	regsettings.x = wr.left;
	regsettings.y = wr.top;
	regsettings.w = wr.right-wr.left;
	regsettings.h = wr.bottom-wr.top;
	}

void MaxFindApp::Resize(int /*flags*/, int /*width*/, int /*height*/)
	{
	RECT	r, wr;
	GetClientRect(hMainWnd, &r);
	GetWindowRect(hMainWnd, &wr);

	regsettings.x = wr.left;
	regsettings.y = wr.top;
	regsettings.w = wr.right-wr.left;
	regsettings.h = wr.bottom-wr.top;

	int		editWidth = (r.right)/3;


	int		btnHeight = 20;
	int		editHeight = 23;
	int		txtHeight = 15;
	int		statusHeight = 16;

	int		toolbarHeight = 91;

	int		startFindBtn = r.left+5;
	int		widthFindBtn = 70;
	int		startCDBtn = startFindBtn+widthFindBtn+15;
	int		widthCDBtn = 70;
	int		startSubdir = startCDBtn+widthCDBtn+15;

	MoveWindow(hControlPane, r.left, 0, r.right, toolbarHeight, TRUE);
	MoveWindow(hDlgFrame, r.left, 0, r.right, toolbarHeight, TRUE);

	MoveWindow(hTxtSearch, r.left+5, 5, editWidth-10, txtHeight, TRUE);
	MoveWindow(hTxtFileSpec, editWidth, 5, editWidth-10, txtHeight, TRUE);
	MoveWindow(hTxtProperty, 2*editWidth, 5, editWidth-10, txtHeight, TRUE);

	MoveWindow(hSearchText, r.left+5, txtHeight+4, editWidth-10, editHeight-1, TRUE);
	MoveWindow(hFileSpec, editWidth, txtHeight+5, editWidth-10, editHeight+200, TRUE);
	MoveWindow(hProperty, 2*editWidth, txtHeight+5, editWidth-10, editHeight+200, TRUE);


	MoveWindow(hFindButton, startFindBtn, editHeight+txtHeight+10, widthFindBtn, btnHeight, TRUE);
	MoveWindow(hCDButton, startCDBtn, editHeight+txtHeight+10, widthCDBtn, btnHeight, TRUE);
	MoveWindow(hCheckSubdirs, startSubdir, editHeight+txtHeight+10, 120, 18, TRUE);
	
	MoveWindow(hStatusPanel, r.left+5, toolbarHeight-statusHeight-4, r.right-10, statusHeight, TRUE);

	MoveWindow(hListBox, r.left, r.top+toolbarHeight, r.right, r.bottom-toolbarHeight, TRUE);
	}

void MaxFindApp::Reset()
	{
	SendMessage(hListBox, LB_RESETCONTENT, 0, 0);
	}

void MaxFindApp::AppIsIdle()
	{
	WaitMessage();
	}

HINSTANCE MaxFindApp::GetInstance()
	{
	return hInstance;
	}

void MaxFindApp::SetHWnd(HWND wnd)
	{
	hMainWnd = wnd;
	}

HWND MaxFindApp::GetHWnd()
	{
	return hMainWnd;
	}

HDC MaxFindApp::GetDC()
	{
	return hMainWndDC;
	}

void MaxFindApp::SetDC(HDC dc)
	{
	hMainWndDC = dc;
	}

void MaxFindApp::SetHMenu(HMENU menu)
	{
	hMenu = menu;
	}

HMENU MaxFindApp::GetHMenu()
	{
	return hMenu;
	}

HFONT MaxFindApp::GetAppFont()
	{
	return hFont;
	}


void MaxFindApp::DoCD()
	{
	BROWSEINFO	browseInfo;
	TCHAR		dir[_MAX_PATH];
	int			image = 0;

	browseInfo.hwndOwner = GetHWnd();
    browseInfo.pidlRoot = NULL;
    browseInfo.pszDisplayName = dir;
    browseInfo.lpszTitle = GetString(IDS_SELFOLDERTITLE);
    browseInfo.ulFlags = 0;
    browseInfo.lpfn = NULL;
    browseInfo.lParam = NULL;
    browseInfo.iImage = image;

	LPITEMIDLIST item = SHBrowseForFolder(&browseInfo);
	if (item) {
		SHGetPathFromIDList(item, dir);
		if (_tcscmp(dir, _T(""))) {
			SetCurrentDirectory(dir);
			DoStatusDirectory();
			}
		}
	}


// From MSJ, August 94, p. 70
BOOL CenterWindow(HWND hWndChild, HWND hWndParent)
{
	RECT	rChild, rParent;
	int		wChild, hChild, wParent, hParent;
	int		wScreen, hScreen, xNew, yNew;
	HDC		hdc = GetDC(hWndChild);

	// get the display limits
	wScreen = GetDeviceCaps(hdc, HORZRES);
	hScreen = GetDeviceCaps(hdc, VERTRES);

	// Get the Height and Width of the parent window
	if(hWndParent)
		GetWindowRect(hWndParent, &rParent);
	else {
		rParent.left = 0;
		rParent.right = wScreen;
		rParent.top = 0;
		rParent.bottom = hScreen;
	}
	wParent = rParent.right - rParent.left;
	hParent = rParent.bottom - rParent.top;

	// get the Height and Width of the child window
	GetWindowRect(hWndChild, &rChild);
	wChild = rChild.right - rChild.left;
	hChild = rChild.bottom - rChild.top;

	// calculate new X position, then adjust for screen
	xNew = rParent.left + ((wParent - wChild) / 2);
	if(xNew < 0)
		xNew = 0;
	else if ((xNew + wChild) > wScreen)
		xNew = wScreen - wChild;

	// calculate new Y position, then adjust for screen
	yNew = rParent.top + ((hParent - hChild) / 2);
	if(yNew < 0)
		yNew = 0;
	else if((yNew + hChild) > hScreen)
		yNew = hScreen - hChild;

	ReleaseDC(hWndChild, hdc);
	// set it, and return
	return SetWindowPos(hWndChild, NULL, xNew, yNew, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

void MaxFindApp::DoStatusDirectory()
	{
	TCHAR dir[_MAX_PATH];
	TCHAR status[_MAX_PATH+32];

	GetCurrentDirectory(_MAX_PATH, dir);
	_tcscpy(status, GetString(IDS_CURDIR));
	_tcscat(status, dir);
	SendMessage(hStatusPanel, WM_SETTEXT, 0, (LPARAM)status);
	}


void MaxFindApp::EnableUI(BOOL status)
	{
	EnableWindow(hCDButton, status);
	EnableWindow(hSearchText, status);
	EnableWindow(hFileSpec, status);
	EnableWindow(hProperty, status);
	EnableWindow(hCheckSubdirs, status);

	SendMessage(hFindButton, WM_SETTEXT, 0, (LPARAM)(status ? GetString(IDS_BTNSTART) : GetString(IDS_BTNCANCEL)));
	}


BOOL MaxFindApp::ScanDirectory(TCHAR* path, TCHAR* filespec, HWND hList)
	{
	WIN32_FIND_DATA	findData;
	TCHAR*	dirSpec = _T("*");
	TCHAR	buf[_MAX_PATH];
	TCHAR	filespecbuf[_MAX_PATH];
	TCHAR	curDir[_MAX_PATH+32];
	BOOL	bQuitRequest = FALSE;
	HANDLE	hFind;
	TCHAR*	pathPtr;
	TCHAR*	pathToken;


	GetCurrentDirectory(_MAX_PATH, curDir);

	_tcscpy(buf, GetString(IDS_SEARCHDIR));
	_tcscat(buf, path);

	SendMessage(hStatusPanel, WM_SETTEXT, 0, (LPARAM)buf);

	_tcscpy(filespecbuf, filespec);
	pathPtr = filespecbuf;

	while (_tcstok(pathPtr, _T(";, ")) != NULL) {
		pathToken = _tcstok(pathPtr, _T(";, "));
		pathPtr = NULL;

		_tcscpy(buf, path);
		if (path[_tcslen(path)-1] != _T('\\'))
			_tcscat(buf, _T("\\"));

		_tcscat(buf, pathToken);

		hFind = FindFirstFile(buf, &findData);
		if (hFind != INVALID_HANDLE_VALUE) {
			do {
				if (WaitForSingleObject(hThreadExitRequest, 0) == WAIT_OBJECT_0) {
					bQuitRequest = TRUE;
					break;
					}
				if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
					!(findData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) &&
					!(findData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) &&
					!(findData.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY)) {

					_tcscpy(buf, path);
					if (path[_tcslen(path)-1] != _T('\\'))
						_tcscat(buf, _T("\\"));
					_tcscat(buf, findData.cFileName);
					if (Qualify(buf)) {
						SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)buf);
						}
					}
				}while (FindNextFile(hFind, &findData));
			}

		FindClose(hFind);

		if (bQuitRequest) {
			return FALSE;
			}
		}

	if (SendMessage(hCheckSubdirs, BM_GETCHECK, 0, 0) == BST_CHECKED) {
		_tcscpy(buf, path);
		if (path[_tcslen(path)-1] != _T('\\'))
			_tcscat(buf, _T("\\"));

		_tcscat(buf, dirSpec);

		hFind = FindFirstFile(buf, &findData);
		if (hFind == INVALID_HANDLE_VALUE) return TRUE;
		do {
			if (WaitForSingleObject(hThreadExitRequest, 0) == WAIT_OBJECT_0) {
				bQuitRequest = TRUE;
				break;
				}

			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				if (_tcscmp(findData.cFileName, _T(".")) &&
					_tcscmp(findData.cFileName, _T(".."))) {
					_tcscpy(buf, path);
					if (path[_tcslen(path)-1] != _T('\\'))
						_tcscat(buf, _T("\\"));
					_tcscat(buf, findData.cFileName);
					if (!ScanDirectory(buf, filespec, hList)) {
						bQuitRequest = TRUE;
						break;
						}
					}
				}
			}while (!bQuitRequest && FindNextFile(hFind, &findData));
		FindClose(hFind);
		}

	return !bQuitRequest;
	}

DWORD WINAPI StartFinder(void* lparg)
	{
	TCHAR path[_MAX_PATH];
	TCHAR filespec[_MAX_PATH];
	MaxFindApp*	pApp = (MaxFindApp*)lparg;

	pApp->bSearchActive = TRUE;

	pApp->EnableUI(FALSE);
	pApp->Reset();
	GetCurrentDirectory(_MAX_PATH, path);
	SendMessage(pApp->GetFileSpecDropDown(), WM_GETTEXT, _MAX_PATH, (LPARAM)filespec);

	pApp->ScanDirectory(path, filespec, pApp->GetMainListBox());

	pApp->DoStatusDirectory();
	pApp->EnableUI(TRUE);
	pApp->bSearchActive = FALSE;
	return 0;
	}

void MaxFindApp::DoFind()
	{
	if (!bSearchActive) {
		ResetEvent(hThreadExitRequest);
		CreateThread(NULL, 0, StartFinder, this, 0, &wWorkerThread);
		}
	else {
		SetEvent(hThreadExitRequest);
		}
	}

void MaxFindApp::ViewFile()
	{
	ShowProperties();
	}

BOOL MaxFindApp::Qualify(TCHAR* filename)
	{
	LPSTORAGE				pStorage = NULL;
	IPropertySetStorage*	pPropertySetStorage = NULL;
	IPropertyStorage*		pSummaryInfoStorage = NULL;
	IPropertyStorage*		pDocumentSummaryInfoStorage = NULL;
	IPropertyStorage*		pUserDefinedPropertyStorage = NULL;
	TCHAR					szSearchString[_MAX_PATH];

	int						propFlags = 0;

	int idx = SendMessage(hProperty, CB_GETCURSEL, 0, 0);
	if (idx == CB_ERR) {
		return FALSE;
		}

	propFlags = SendMessage(hProperty, CB_GETITEMDATA, idx, 0);

	GetWindowText(hSearchText, szSearchString, _MAX_PATH);

	if ((propFlags == ALL_PROPERTIES) && (_tcscmp(szSearchString, _T("")) == 0)) {
		// Qualify all files if we don't search for anything
		return TRUE;
		}
	HRESULT	res = StgOpenStorage(TSTR(filename).ToUTF16(), (LPSTORAGE)0, STGM_DIRECT|STGM_READ|STGM_SHARE_DENY_WRITE, NULL,0,&pStorage);
	if (res!=S_OK) {
		return FALSE;
		}


	// Get the Storage interface
	if (S_OK != pStorage->QueryInterface(IID_IPropertySetStorage, (void**)&pPropertySetStorage)) {
		pStorage->Release();
		return FALSE;
	}

	// Get the SummaryInfo property set interface
	if (propFlags & PROPSET_SUMINFO) {
		if (S_OK == pPropertySetStorage->Open(FMTID_SummaryInformation, STGM_READ|STGM_SHARE_EXCLUSIVE, &pSummaryInfoStorage)) {
			BOOL bFound = FALSE;

			PROPSPEC	PropSpec[6];
			PROPVARIANT	PropVar[6];

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
			PropSpec[5].propid = 1; // Read the property storage's codepage.

			HRESULT hr = pSummaryInfoStorage->ReadMultiple(6, PropSpec, PropVar);
			if (S_OK == hr) {
				if (PropVar[5].vt == VT_I2) {
					if (PropVar[5].uiVal == 1200) {
						// 1200 is a special codepage. Means that, on-disk, data is UTF-16,
						// but was translated to ACP when loaded.
					}
					else if (PropVar[5].uiVal != GetACP()) {
						// The string's codepage doesn't match our. We must convert it to something neutral.
						VariantConvertToUTF16(&PropVar[0], &PropVar[0], PropVar[5].uiVal);
						VariantConvertToUTF16(&PropVar[1], &PropVar[1], PropVar[5].uiVal);
						VariantConvertToUTF16(&PropVar[2], &PropVar[2], PropVar[5].uiVal);
						VariantConvertToUTF16(&PropVar[3], &PropVar[3], PropVar[5].uiVal);
						VariantConvertToUTF16(&PropVar[4], &PropVar[4], PropVar[5].uiVal);
					}
				}

				if (propFlags & TITLE_PROP) {
					if (Compare(PropVar[0], szSearchString)) {
						bFound = TRUE;
						}
					}

				if (!bFound && (propFlags & SUBJECT_PROP)) {
					if (Compare(PropVar[1], szSearchString)) {
						bFound = TRUE;
						}
					}

				if (!bFound && (propFlags & AUTHOR_PROP)) {
					if (Compare(PropVar[2], szSearchString)) {
						bFound = TRUE;
						}
					}

				if (!bFound && (propFlags & KEYWORDS_PROP)) {
					if (Compare(PropVar[3], szSearchString)) {
						bFound = TRUE;
						}
					}

				if (!bFound && (propFlags & COMMENTS_PROP)) {
					if (Compare(PropVar[4], szSearchString)) {
						bFound = TRUE;
						}
					}
				}

			FreePropVariantArray(6, PropVar);

			pSummaryInfoStorage->Release();
			if (bFound) {

				pPropertySetStorage->Release();
				pStorage->Release();
				return TRUE;
				}
			}
		}

	// Get the DocumentSummaryInfo property set interface
	if (propFlags & PROPSET_DOCSUMINFO) {
		if (S_OK == pPropertySetStorage->Open(FMTID_DocSummaryInformation, STGM_READ|STGM_SHARE_EXCLUSIVE, &pDocumentSummaryInfoStorage)) {
			BOOL bFound = FALSE;

			PROPSPEC	PropSpec[6];
			PROPVARIANT	PropVar[6];
			UINT        CodePage = CP_ACP;

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
			PropSpec[5].propid = 1; // Read the property storage's codepage.

			HRESULT hr = pDocumentSummaryInfoStorage->ReadMultiple(6, PropSpec, PropVar);
			if (S_OK == hr) {

				if (PropVar[5].vt == VT_I2) {
					if (PropVar[5].uiVal == 1200) {
						// 1200 is a special codepage. Means that, on-disk, data is UTF-16,
						// but was translated to ACP when loaded.
					}
					else if (PropVar[5].uiVal != GetACP()) {
						CodePage = PropVar[5].uiVal;
						// The string's codepage doesn't match our. We must convert it to something neutral.
						VariantConvertToUTF16(&PropVar[0], &PropVar[0], CodePage);
						VariantConvertToUTF16(&PropVar[1], &PropVar[1], CodePage);
						VariantConvertToUTF16(&PropVar[2], &PropVar[2], CodePage);
					}
				}

				if (propFlags & MANAGER_PROP) {
					if (Compare(PropVar[0], szSearchString)) {
						bFound = TRUE;
						}
					}

				if (!bFound && (propFlags & COMPANY_PROP)) {
					if (Compare(PropVar[1], szSearchString)) {
						bFound = TRUE;
						}
					}

				if (!bFound && (propFlags & CATEGORY_PROP)) {
					if (Compare(PropVar[2], szSearchString)) {
						bFound = TRUE;
						}
					}

				// Scan through document contents
				if (!bFound && (PropVar[3].vt == (VT_VARIANT | VT_VECTOR)) && VariantIsStringVector(PropVar[4])) {
					CAPROPVARIANT*	  pHeading = &PropVar[3].capropvar;
					std::vector<TSTR> pDocPart;

					VariantToStringVector(&PropVar[4], pDocPart, CodePage);

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
						BOOL bCompare = FALSE;

						if ((i==6) && (propFlags & EXT_DEPEND_PROP)) bCompare = TRUE;
						if ((i==8) && (propFlags & OBJECTS_PROP)) bCompare = TRUE;
						if ((i==10) && (propFlags & MATERIALS_PROP)) bCompare = TRUE;
						if ((i==12) && (propFlags & PLUGINS_PROP)) bCompare = TRUE;

						for (int j=0; j<pHeading->pElems[i+1].lVal; j++) {
							if (!bFound && bCompare) {
								if (Compare(pDocPart[nDocPart].data(), szSearchString)) {
									bFound = TRUE;
									}
								}
							nDocPart++;
							}
						}
					}
				}

			FreePropVariantArray(6, PropVar);

			pDocumentSummaryInfoStorage->Release();
			if (bFound) {
				pPropertySetStorage->Release();
				pStorage->Release();
				return TRUE;
				}
			}
		}

	// Get the User Defined property set interface
	if (propFlags & PROPSET_USERDEF) {
		if (S_OK == pPropertySetStorage->Open(FMTID_UserDefinedProperties, STGM_READ|STGM_SHARE_EXCLUSIVE, &pUserDefinedPropertyStorage)) {
			BOOL	bFound = FALSE;
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

				HRESULT hr = pUserDefinedPropertyStorage->ReadMultiple(numUserProps + 1, pPropSpec, pPropVar);
				if (S_OK == hr) {

					bool    convertToUTF16 = false;

					if (pPropVar[numUserProps].vt == VT_I2) {
						if (pPropVar[numUserProps].uiVal == 1200) {
							// 1200 is a special codepage. Means that, on-disk, data is UTF-16,
							// but was translated to ACP when loaded.
						}
						else if (pPropVar[numUserProps].uiVal != GetACP()) {
							// The string's codepage doesn't match our. We must convert it to something neutral.
							convertToUTF16 = true;
						}
					}

					for (int i=0; i < numUserProps; i++) {
						if (convertToUTF16) {
							VariantConvertToUTF16(&pPropVar[i], &pPropVar[i], pPropVar[numUserProps].uiVal);
						}

						if (Compare(pPropVar[i], szSearchString)) {
							bFound = TRUE;
							break;
						}
					}
				}

				for (int i=0; i<numUserProps; i++) {
					if (pPropSpec[i].ulKind == PRSPEC_LPWSTR) {
						CoTaskMemFree(pPropSpec[i].lpwstr);
					}
				}

				FreePropVariantArray(numUserProps+1, pPropVar);

				delete [] pPropSpec;
				delete [] pPropVar;
				}

			pUserDefinedPropertyStorage->Release();
			if (bFound) {
				pPropertySetStorage->Release();
				pStorage->Release();
				return TRUE;
				}
			}
		}

	pPropertySetStorage->Release();
	pStorage->Release();

	return FALSE;
	}


BOOL MaxFindApp::Compare(const TCHAR* s1, const TCHAR* s2)
{
	if (s1[0] == 0) return FALSE;

	TCHAR* lcs1 = _tcsdup(s1);
	TCHAR* lcs2 = _tcsdup(s2);

	if (!lcs1 && !lcs2) return FALSE;

	_tcslwr(lcs1);
	_tcslwr(lcs2);

	BOOL bFound = _tcsstr(lcs1, lcs2) != NULL;

	free(lcs1);
	free(lcs2);

	if (bFound)
		return TRUE;

	return FALSE;
}

BOOL MaxFindApp::Compare(const PROPVARIANT& s1, const TCHAR* s2)
{
	if (VariantIsString(&s1))
	{
		return Compare(VariantToString(s1).ToMCHAR(), s2);
	}

	return false;
}
