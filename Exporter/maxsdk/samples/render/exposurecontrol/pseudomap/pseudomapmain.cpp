/*==============================================================================

  file:	        plugin.cpp

  author:       Daniel Levesque
  
  created:      13 August 2001
  
  description:  Entry point for this plugin
  
  modified:	


© 2001 Autodesk
==============================================================================*/


#include <plugapi.h>
#include "resource.h"
#include "RenElem.h"
#include "pseudomapmain.h"
#include "PseudoMap.h"

HINSTANCE hInstance;


TCHAR *GetString( int id );

//------------------------------------------------------------------------------
//

BOOL WINAPI DllMain(
                    HINSTANCE   hinstDLL,
                    ULONG       fdwReason,
                    LPVOID      lpvReserved
                    )
{
    switch(fdwReason) {
    case DLL_PROCESS_ATTACH: 
        MaxSDK::Util::UseLanguagePackLocale();
        hInstance = hinstDLL;
        DisableThreadLibraryCalls( hinstDLL );
        
        break;
    }
    
    return true;
}


// This is the interface to Jaguar:

//------------------------------------------------------------------------------
//

extern "C" __declspec( dllexport ) const TCHAR* LibDescription()
{
    return GetString(IDS_LIBDESCRIPTION);
}


//------------------------------------------------------------------------------
//

extern "C" __declspec( dllexport ) int LibNumberClasses()
{
    return 3;
}

//------------------------------------------------------------------------------
//

extern "C" __declspec( dllexport ) ClassDesc* LibClassDesc( int i )
{
    switch(i) {
    case 0:
        return PseudoMap::GetClassDesc();
    case 1:
        return GetLuminationClassDesc();
    case 2:
        return GetIlluminationClassDesc();
    default:
        return NULL;
    }
}

//------------------------------------------------------------------------------
//

// Return version so can detect obsolete DLLs
extern "C" __declspec( dllexport ) ULONG  LibVersion()
{
    return VERSION_3DSMAX;
}

//------------------------------------------------------------------------------
//

TCHAR *GetString( int id )
{
    static TCHAR buf[256];
    
    if (hInstance) {
        return LoadString(hInstance, id, buf, _countof(buf)) ? buf : NULL;
    }
    
    return NULL;
}
