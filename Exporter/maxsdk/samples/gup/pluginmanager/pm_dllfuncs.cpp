// Copyright 2008 Autodesk, Inc.  All rights reserved.
//
// Use of this software is subject to the terms of the Autodesk license
// agreement provided at the time of installation or download, or which
// otherwise accompanies this software in either electronic or hard copy form.  
//
//

#include "plugman.h"
#include "utillib.h"
#include "IPathConfigMgr.h"
#include <maxapi.h>
#include <custcont.h>

extern HINSTANCE hInstance;

/*===========================================================================*\
 |	Used for swap in-out at load time to refresh command UI
\*===========================================================================*/

class nullCD:public ClassDesc {
	public:
	int 			IsPublic() { return 0; }
	void *			Create( BOOL loading ) { return NULL; }
	const TCHAR *	ClassName() { return _T("NULL"); }
	SClass_ID		SuperClassID() { return GEOMOBJECT_CLASS_ID; }
	Class_ID 		ClassID() { return Class_ID(0x35591eda, 0x596b602c); }
	const TCHAR* 	Category() { return _T("");  }
};
static nullCD theNullCD;


void DeepRefreshUI(Interface *ip)
{
		// for commandpanel update
		ip->AddClass(&theNullCD);
		ip->DeleteClass(&theNullCD);

		GetCUIFrameMgr()->RecalcLayout(TRUE);
}

/*===========================================================================*\
 |	Load a new DLL with a fileName selector
\*===========================================================================*/

bool LoadNewDLL( HWND hWnd, Interface *ip)
{
		TCHAR fullPath[MAX_PATH] = {0};
		TCHAR fileTitle[100] = {0};
		TCHAR initDir[MAX_PATH] = {0};
		FilterList FileFilter;

		OPENFILENAME ofn;
		memset(&ofn,0,sizeof(ofn));

		ofn.lStructSize = sizeof(OPENFILENAME);  // No OFN_ENABLEHOOK
		ofn.hwndOwner=hWnd;
		ofn.hInstance=hInstance;
#ifdef DESIGN_VER
		FileFilter.Append(GetString(IDS_MAX_PLUGINS_VIZ));
#else
		FileFilter.Append(GetString(IDS_MAX_PLUGINS));
#endif // DESIGN_VER
		FileFilter.Append(_T("*.DL?;*.BM?;*.FLT"));
		FileFilter.Append(GetString(IDS_ALL_FILES));
		FileFilter.Append(_T("*.*"));
		ofn.lpstrFilter = FileFilter;

		ofn.lpstrCustomFilter=NULL;
		ofn.nMaxCustFilter=0;
		ofn.nFilterIndex=1;
		ofn.lpstrFile=fullPath;
		ofn.nMaxFile=500;
		ofn.lpstrFileTitle = fileTitle;
		ofn.nMaxFileTitle=99;
		IPathConfigMgr* manager = IPathConfigMgr::GetPathConfigMgr();
		if(manager->GetPlugInEntryCount() > 0)	{
			_tcsncpy(initDir, manager->GetPlugInDir(0), MAX_PATH);
			ofn.lpstrInitialDir =  initDir;
		}
		else	{
			ofn.lpstrInitialDir = NULL;
		}
		ofn.lpstrTitle = GetString(IDS_CHOOSE_PLUGIN);
		ofn.Flags = OFN_FILEMUSTEXIST;
		ofn.FlagsEx = OFN_EX_NOPLACESBAR;

		fullPath[0]=_T('\0');
		GetOpenFileName(&ofn);

		if(fullPath[0]==_T('\0')){return false;}
		DllDir* dd = ip->GetDllDirectory();
		
		int dllIndex = dd->FindDllDescFromDllPath(fullPath);
		if (dllIndex >= 0)
		{
			return dd->LoadADll((dd->GetDllDescription(dllIndex)).GetFullPath(), TRUE);
		}

		return dd->LoadADll(fullPath, TRUE);
}

/*===========================================================================*\
 |	Load an existing DLL
\*===========================================================================*/

bool LoadExistingDLL(const DllDesc& dllDesc, HWND hWnd)
{
	if (dllDesc.IsLoaded())
	{
		MessageBox(hWnd,GetString(IDS_ALREADY_LOADED),GetString(IDS_LOAD_ERROR),MB_OK);
		return false;
	}
	else
	{
		return DllDir::GetInstance().LoadADll(dllDesc.GetFullPath(), TRUE);
	}	
}


/*===========================================================================*\
 |	Check to see if a plugin has active ties to the superclasslists
\*===========================================================================*/

BOOL pluginHasClassesRegistered( int masterCode, Interface *ip )
{
	DllDir *dd = ip->GetDllDirectory();
	ClassDirectory *cd = &dd->ClassDir();

	for(int i=0;i<cd->Count();i++)
	{
		SubClassList *scl = &(*cd)[i];

		for(int q=0;q<scl->Count(ACC_ALL);q++)
		{
			ClassEntry *ce = &(*scl)[q];
			if(ce->DllNumber() == masterCode) return TRUE;
		}
	}
	return FALSE;
}






/*===========================================================================*\
 |	REDEFER CODE - WORK IN PROGRESS
\*===========================================================================*/


#define VENDORNAMELENGTH 256
#define PLUGINDESCRLENGTH 256
#define LIBDESCLENGTH 256
#define CLASSSTRINGLENGTH 256
#define VALUEDATALENGTH 256

#define USE_UTIL_REG_KEY

#ifdef USE_UTIL_REG_KEY
#define DEFERLOADREGKEY0_3	UtilityInterface::GetRegistryKeyBase()
#define DEFERLOADREGKEY4	GetString(IDS_PM_REG_PLUGINS)

#else	// USE_UTIL_REG_KEY
// Registry place for deferred loading

#define DEFERLOADREGKEY0	_T("SOFTWARE")
#define DEFERLOADREGKEY1	_T("Autodesk")
#ifdef DESIGN_VER
#define DEFERLOADREGKEY2	_T("VIZ")
#else
#define DEFERLOADREGKEY2	_T("3ds max")
#endif
#define DEFERLOADREGKEY3	_T("5.0")
#define DEFERLOADREGKEY4	_T("Plug-ins")

#endif // USE_UTIL_REG_KEY

static HKEY keychain[8];

LONG FindRegPlace() {
    long ok;
#define CLASSSTRINGLENGTH 256

#ifdef USE_UTIL_REG_KEY
    ok = RegOpenKeyEx(HKEY_CURRENT_USER,
                      DEFERLOADREGKEY0_3,
                      0,
                      KEY_READ,
                      &keychain[3]);

#else	// if not USE_UTIL_REG_KEY

    ok = RegOpenKeyEx(HKEY_CURRENT_USER,
                      DEFERLOADREGKEY0,
                      0,
                      KEY_READ,
                      &keychain[0]);
    if (ok == ERROR_SUCCESS) {
        ok = RegOpenKeyEx(keychain[0],
                          DEFERLOADREGKEY1,
                          0,
                          KEY_READ,
                          &keychain[1]);
        if (ok == ERROR_SUCCESS) {
            ok = RegOpenKeyEx(keychain[1],
                              DEFERLOADREGKEY2,
                              0,
                              KEY_READ,
                              &keychain[2]);
            if (ok == ERROR_SUCCESS) {
                ok = RegOpenKeyEx(keychain[2],
                                  DEFERLOADREGKEY3,
                                  0,
                                  KEY_READ,
                                  &keychain[3]);
#endif	// USE_UTIL_REG_KEY
                if (ok == ERROR_SUCCESS) {
                    ok = RegOpenKeyEx(keychain[3],
                                       DEFERLOADREGKEY4,
                                       0,
                                       KEY_READ,
                                       &keychain[4]);
                    if (ok == ERROR_SUCCESS)
                        return ok;
                    RegCloseKey(keychain[3]);
                    }
#ifndef USE_UTIL_REG_KEY
                RegCloseKey(keychain[2]);
                }
            RegCloseKey(keychain[1]);
            }
        RegCloseKey(keychain[0]);
        }
#endif	// USE_UTIL_REG_KEY
    return ok;
    }

void LoseRegPlace() {
    RegCloseKey(keychain[4]);
    RegCloseKey(keychain[3]);
#ifndef USE_UTIL_REG_KEY
    RegCloseKey(keychain[2]);
    RegCloseKey(keychain[1]);
    RegCloseKey(keychain[0]);
#endif	// USE_UTIL_REG_KEY
    }

bool GetRegSZ(TCHAR *key, TSTR &val) {
	DWORD valueDataLength = VALUEDATALENGTH * sizeof( TCHAR );
	TCHAR valueData[VALUEDATALENGTH];
	DWORD valueType;

	if (RegQueryValueEx(keychain[7],
						key,
						NULL,
						&valueType,
						(unsigned char *)&valueData,
						&valueDataLength)
				== ERROR_SUCCESS
			&& valueType == REG_SZ) {
		val = valueData;
		return TRUE;
		}
	return FALSE;
	}

bool GetRegDW(TCHAR *key, DWORD &val) {
	DWORD dwordDataLength = sizeof( DWORD );
	DWORD dwordData;
	DWORD valueType;

	if (RegQueryValueEx(keychain[7],
						key,
						NULL,
						&valueType,
						(unsigned char *)&dwordData,
						&dwordDataLength)
				== ERROR_SUCCESS
			&& valueType == REG_DWORD) {
		val = dwordData;
		return TRUE;
		}
	return FALSE;
	}
