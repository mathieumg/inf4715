/*===========================================================================*\
 | 
 |  FILE:	Tex.cpp
 |			Plugin interface for SWIRL texture
 | 
 |
 |  AUTHOR: Harry Denholm
 |			All Rights Reserved. Copyright(c) Kinetix 1998
 |
 | 
\*===========================================================================*/

#include "swirl.h"


HINSTANCE hInstance;

/*===========================================================================*\
 | Entry point
\*===========================================================================*/

BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved) {
   if( fdwReason == DLL_PROCESS_ATTACH )
   {
      MaxSDK::Util::UseLanguagePackLocale();
      hInstance = hinstDLL;
      DisableThreadLibraryCalls(hInstance);
   }
	return(TRUE);

}


/*===========================================================================*\
 | Interface to MAX
\*===========================================================================*/

__declspec( dllexport ) const TCHAR *
LibDescription() { return GetString(IDS_LIBDESC); }


__declspec( dllexport ) int LibNumberClasses() {return 1;}

__declspec( dllexport ) ClassDesc*
LibClassDesc(int i) {
	switch(i) {
		case  0: return GetSwirlDesc();
		default: return 0;
		}
	}

__declspec( dllexport ) ULONG 
LibVersion() { return VERSION_3DSMAX; }

// Let the plug-in register itself for deferred loading
__declspec( dllexport ) ULONG CanAutoDefer()
{
	return 1;
}

TCHAR *GetString(int id)
	{
	static TCHAR buf[256];
	if(hInstance)
		return LoadString(hInstance, id, buf, _countof(buf)) ? buf : NULL;
	return NULL;
	}
