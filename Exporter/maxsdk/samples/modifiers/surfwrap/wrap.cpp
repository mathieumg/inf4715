/**********************************************************************
 *<
	FILE: wrap.cpp

	DESCRIPTION: Wrap one surface over another common files

	CREATED BY: Audrey Peterson

	HISTORY: 1/97

 *>	Copyright (c) 1997, All Rights Reserved.
 **********************************************************************/

#include "wrap.h"

#include "3dsmaxport.h"

HINSTANCE hInstance;

//------------------------------------------------------
// This is the interface to Jaguar:
//------------------------------------------------------
float Check1to1(float x)
{ return(x>1.0f?1.0f:(x<-1.0f?-1.0f:x));}

TCHAR *GetString(int id)
	{
	static TCHAR buf[256];

	if (hInstance)
		return LoadString(hInstance, id, buf, _countof(buf)) ? buf : NULL;
	return NULL;
	}
INT_PTR CALLBACK DefaultSOTProc(
		HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
	{
	IObjParam *ip = DLGetWindowLongPtr<IObjParam*>(hWnd);

	switch (msg) {
		case WM_INITDIALOG:
			DLSetWindowLongPtr(hWnd, lParam);
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			if (ip) ip->RollupMouseMessage(hWnd,msg,wParam,lParam);
			return FALSE;

		default:
			return FALSE;
		}
	return TRUE;
	}
__declspec( dllexport ) const TCHAR *
LibDescription() { return GetString(IDS_LIBDESCRIPTION); }

// This function returns the number of plug-in classes this DLL implements
__declspec( dllexport ) int 
LibNumberClasses() { return 2; }

// This function return the ith class descriptor. We have one.
__declspec( dllexport ) ClassDesc* 
LibClassDesc(int i) {
	switch(i){
	case 0:return GetSWrapDesc();
    case 1:return GetSWrapModDesc();
//    case 2:return GetMSWrapModDesc();
    default:return 0;}
 }
// This function returns a pre-defined constant indicating the version of 
// the system under which it was compiled.  It is used to allow the system
// to catch obsolete DLLs.
__declspec( dllexport ) ULONG 
LibVersion() { return VERSION_3DSMAX; }

// Let the plug-in register itself for deferred loading
__declspec( dllexport ) ULONG CanAutoDefer()
{
	return 1;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved) 
	{	
   if( fdwReason == DLL_PROCESS_ATTACH )
   {
      MaxSDK::Util::UseLanguagePackLocale();
      // Hang on to this DLL's instance handle.
      hInstance = hinstDLL;
      DisableThreadLibraryCalls(hInstance);
   }

	return(TRUE);
	}


