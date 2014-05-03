// MXSActiveX.cpp : Implementation of DLL Exports.



#include "stdafx.h"
#include "resource.h"
#include <initguid.h>


HINSTANCE hInstance;

extern void activex_init();

#include <maxscript/UI/rollouts.h>
#include "MSComObject.h"

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)   
END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID /*lpReserved*/)
{
   if (dwReason == DLL_PROCESS_ATTACH)
    {
        MaxSDK::Util::UseLanguagePackLocale();
        hInstance = hinstDLL;          // Hang on to this DLL's instance handle.
        _Module.Init(NULL, hInstance);
        DisableThreadLibraryCalls(hInstance);
    }
    else if (dwReason == DLL_PROCESS_DETACH)
        _Module.Term();
    return TRUE;    // ok
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
    return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    return _Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
    return _Module.UnregisterServer(TRUE);
}

__declspec( dllexport ) const TCHAR* LibDescription()
{
	static TSTR libDescription (MaxSDK::GetResourceStringAsMSTR(IDS_LIBDESCRIPTION));
	return libDescription;
}

__declspec( dllexport ) void LibInit() { 
   // do any setup here
   _Module.Init(ObjectMap, hInstance, &LIBID_MXSActiveX);
   activex_init();
}
__declspec( dllexport ) ULONG LibVersion()
{
   return VERSION_3DSMAX;
}

