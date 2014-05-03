/**********************************************************************
 *<
   FILE: DllEntry.cpp

   DESCRIPTION:Contains the Dll Entry stuff

   CREATED BY: 

   HISTORY: 

 *>   Copyright (c) 1997, All Rights Reserved.
 **********************************************************************/
#include "Max.h"
#include "resource.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"

extern TCHAR *GetString(int id);
extern ClassDesc2* GetPointCacheOSMDesc();
extern ClassDesc2* GetPointCacheWSMDesc();
extern ClassDesc2* GetOLDPointCacheDesc();
extern ClassDesc2* GetOLDPointCacheWSMDesc();
extern ClassDesc2* GetOLDParticleCacheDesc();


HINSTANCE hInstance;

// This function is called by Windows when the DLL is loaded.  This 
// function may also be called many times during time critical operations
// like rendering.  Therefore developers need to be careful what they
// do inside this function.  In the code below, note how after the DLL is
// loaded the first time only a few statements are executed.

BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved)
{
   if( fdwReason == DLL_PROCESS_ATTACH )
   {
      MaxSDK::Util::UseLanguagePackLocale();
      hInstance = hinstDLL;            // Hang on to this DLL's instance handle.
      DisableThreadLibraryCalls(hInstance);
   }
         
   return (TRUE);
}

// This function returns a string that describes the DLL and where the user
// could purchase the DLL if they don't have it.
__declspec( dllexport ) const TCHAR* LibDescription()
{
   return GetString(IDS_LIBDESCRIPTION);
}

// This function returns the number of plug-in classes this DLL
//TODO: Must change this number when adding a new class
__declspec( dllexport ) int LibNumberClasses()
{
   return 5;
}

// This function returns the number of plug-in classes this DLL
__declspec( dllexport ) ClassDesc* LibClassDesc(int i)
{
   switch(i) {
      case 0: return GetOLDPointCacheDesc();
      case 1: return GetOLDPointCacheWSMDesc();
      case 2: return GetOLDParticleCacheDesc();
      case 3: return GetPointCacheOSMDesc();
      case 4: return GetPointCacheWSMDesc();
      default: return 0;
   }
}

// This function returns a pre-defined constant indicating the version of 
// the system under which it was compiled.  It is used to allow the system
// to catch obsolete DLLs.
__declspec( dllexport ) ULONG LibVersion()
{
   return VERSION_3DSMAX;
}

__declspec( dllexport ) ULONG CanAutoDefer()
{
	// Turning off defer loading until proxy systems are in place 
	return 0;
}

TCHAR *GetString(int id)
{
   static TCHAR buf[256];

   if (hInstance)
      return LoadString(hInstance, id, buf, _countof(buf)) ? buf : NULL;
   return NULL;
}

