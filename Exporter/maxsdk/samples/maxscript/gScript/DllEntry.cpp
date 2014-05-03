// ============================================================================
// DllEntry.cpp
//
// History:
//  09.12.00 - Created by Simon Feltman
//
// Copyright ©2006, Autodesk
// ----------------------------------------------------------------------------
#include <maxscript/maxscript.h>
#include "resource.h"

HINSTANCE hInstance;

// ============================================================================
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
   switch(fdwReason)
   {
   case DLL_PROCESS_ATTACH:
      MaxSDK::Util::UseLanguagePackLocale();
      hInstance = hinstDLL;
      DisableThreadLibraryCalls(hInstance);
      break;

   case DLL_PROCESS_DETACH:
      break;
   }
   return TRUE;
}

// ============================================================================
__declspec(dllexport) void LibInit()
{
}
__declspec( dllexport ) const TCHAR* LibDescription()
{
	static TSTR libDescription (MaxSDK::GetResourceStringAsMSTR(IDS_LIBDESCRIPTION));
	return libDescription;
}
__declspec(dllexport) ULONG LibVersion() {  return VERSION_3DSMAX; }

