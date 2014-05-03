//-----------------------------------------------------------------------------
// -----------------------
// File	....:	ViewFile.cpp
// -----------------------
// Author...:	Gus J	Grubba
// Date	....:	September 1995
// O.S.	....:	Windows NT 3.51
//
// History	.:	Nov, 02 1995 -	Created
//
// This	is	the "View File" option in MAX's File menu.
//
//-----------------------------------------------------------------------------

#include <Max.h>
#include "bmmlib.h"
#include "resource.h"

#define	 VWFEXPORT __declspec( dllexport	)
#include "ViewFile.h"

//--	Globals ------------------------------------------------------------------

HINSTANCE hInst = NULL;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//--	DLL Declaration

BOOL	WINAPI DllMain(HINSTANCE hDLLInst, DWORD fdwReason, LPVOID lpvReserved)	{
	switch (fdwReason) {
		 case	DLL_PROCESS_ATTACH:
				MaxSDK::Util::UseLanguagePackLocale();
				if	(hInst)
					return(FALSE);
				hInst	= hDLLInst;
				break;
		 case	DLL_PROCESS_DETACH:
				hInst	 =	NULL;
				break;
		 case	DLL_THREAD_ATTACH:
				break;
		 case	DLL_THREAD_DETACH:
				break;
	}
	return TRUE;
}

static bool UseSystemGammaByDefault() 	{
	TCHAR filename[MAX_PATH];
	_tcscpy(filename,TheManager->GetDir(APP_PLUGCFG_LN_DIR));
	int len = static_cast<int>(_tcslen(filename));  // SR DCAST64: Downcast to 2G limit.
	if (len) {
		if (_tcscmp(&filename[len-1],_T("\\")))
			_tcscat(filename,_T("\\"));
	}   
	_tcscat(filename,_T("gamma.ini"));
	// If UseFileGamma == 0 (the default if it is missing), we should use system gamma
	return GetPrivateProfileInt(_T("BitmapPicker"), _T("UseFileGamma"), 0, filename) == 0;
}


//-----------------------------------------------------------------------------
// Interface

VWFEXPORT void *ViewFileCreate	( ) {	
	return new ViewFile;
}	 

VWFEXPORT void  ViewFileDestroy	( ViewFile *v ) {	
	if	(v)
		delete v;
}	 

//-----------------------------------------------------------------------------
// #> ViewFile::ViewFile()
//

ViewFile::ViewFile() {
	hWnd		 =	NULL;
}

//-----------------------------------------------------------------------------
// #> ViewFile::~ViewFile()
//

ViewFile::~ViewFile() {


}

//-----------------------------------------------------------------------------
// #> ViewFile::View()
//

void	ViewFile::View( HWND	hWnd ) {
	
	int idx;
	DWORD caps;
	BitmapInfo bi;
	Bitmap *map = NULL;
	TCHAR buf[256];

	LoadString(hInst, IDS_DB_VIEW_FILE, buf, _countof(buf));
	
	// Do this conditionally now
	if (UseSystemGammaByDefault())
		bi.ResetCustomFlag(BMM_CUSTOM_FILEGAMMA);

	if (!TheManager->SelectFileInputEx(&bi, hWnd, buf, TRUE))
	   return;

	if	(bi.Name()[0])
		idx = TheManager->ioList.ResolveDevice(&bi);
	else
		idx = TheManager->ioList.FindDevice(bi.Device());

	if (idx == -1)
		goto error;

	caps = TheManager->ioList.GetDeviceCapabilities(bi.Device());
	TCHAR title[MAX_PATH];

	if	(caps & BMMIO_EXTENSION)
		_tcscpy(title,bi.Filename());
	else
		_tcscpy(title,bi.Device());

	if	(caps & BMMIO_OWN_VIEWER) {
		BitmapIO	*IO =	TheManager->ioList.CreateDevInstance(bi.Device());
		if(IO) {
			BOOL succeeded = IO->ShowImage(hWnd,&bi);
			delete IO;
			if(!succeeded)
				goto normal_view;
		}
	} else {
		normal_view:
		SetCursor(LoadCursor(NULL,IDC_WAIT));
		map = TheManager->Load(&bi);
		if	(map)	{
			map->Display(title, BMM_CN, TRUE, FALSE);
		} else {
			error:
			TCHAR text[128];
			TCHAR tmp[128];
			LoadString(hInst, IDS_DB_NO_VIEW, tmp, _countof(tmp));
			wsprintf(text,tmp,bi.Name());
			LoadString(hInst, IDS_DB_VIEW_ERROR, tmp, _countof(tmp));
			MessageBox(hWnd,text,tmp,MB_OK);
		}
		SetCursor(LoadCursor(NULL,IDC_ARROW));
	}
	
}

//--	EOF: ViewFile.cpp	--------------------------------------------------------
