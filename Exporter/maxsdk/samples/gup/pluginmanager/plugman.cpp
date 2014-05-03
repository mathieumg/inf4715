// Copyright 2008 Autodesk, Inc.  All rights reserved.
//
// Use of this software is subject to the terms of the Autodesk license
// agreement provided at the time of installation or download, or which
// otherwise accompanies this software in either electronic or hard copy form.  
//
//

#include "Plugman.h"
#include "iparamm2.h"
#include "notify.h"
#include "3dsmaxport.h"
#include <Path.h>

extern HINSTANCE hInstance;

extern bool rebuildCache( Interface *ip, PlugMgrUtility *pmu, HWND hWnd );

/*===========================================================================*\
 | Function Descriptor
\*===========================================================================*/
#define PLUGINMANAGER_FN_INTERFACE	Interface_ID(0x5e3b07f6, 0x273f7de7)

PlugMgrUtility PlugMgrUtility::mInstance
(
	PLUGINMANAGER_FN_INTERFACE, _T("pluginManager"), 0, NULL, FP_CORE,
	PlugMgrUtility::pm_pluginDllName, _T("pluginDllName"), 0, TYPE_STRING, FP_NO_REDRAW, 1,
		_T("index"), 0, TYPE_INDEX,
	PlugMgrUtility::pm_pluginDllDirectory, _T("pluginDllDirectory"), 0, TYPE_STRING, FP_NO_REDRAW, 1,
		_T("index"), 0, TYPE_INDEX,
	PlugMgrUtility::pm_pluginDllFullPath, _T("pluginDllFullPath"), 0, TYPE_TSTR_BV, FP_NO_REDRAW, 1,
		_T("index"), 0, TYPE_INDEX,
	PlugMgrUtility::pm_pluginDllSize, _T("pluginDllSize"), 0, TYPE_INT, FP_NO_REDRAW, 1,
		_T("index"), 0, TYPE_INDEX,
	PlugMgrUtility::pm_isPluginDllLoaded, _T("isPluginDllLoaded"), 0, TYPE_BOOL, FP_NO_REDRAW, 1,
		_T("index"), 0, TYPE_INDEX,
	PlugMgrUtility::pm_loadPluginDll, _T("loadPluginDll"), 0, TYPE_BOOL, FP_NO_REDRAW, 1,
		_T("index"), 0, TYPE_INDEX,
	PlugMgrUtility::pm_loadClass, _T("loadClass"), 0, TYPE_VOID, 0, FP_NO_REDRAW,
		_T("class"), 0, TYPE_CLASS,

	properties,	
		PlugMgrUtility::pm_isVisible, PlugMgrUtility::pm_setVisible, _T("visible"), 0, TYPE_BOOL,
		PlugMgrUtility::pm_pluginDllCount, FP_NO_FUNCTION, _T("pluginDllCount"), 0, TYPE_INT,
		// Since the performance of loadedPluginDllSize would be very slow we provide a C++ implementation 
		PlugMgrUtility::pm_loadedPluginDllSize, FP_NO_FUNCTION, _T("loadedPluginDllSize"), 0, TYPE_INT, 

	p_end);

PlugMgrUtility& PlugMgrUtility::GetInstance()
{
	return mInstance;
}

/*===========================================================================*\
 | Class Descriptor
\*===========================================================================*/

class PlugMgrClassDesc : public ClassDesc2 {
   public:
   int         IsPublic()              { return TRUE; }
	 void*       Create( BOOL loading )     { return &(PlugMgrUtility::GetInstance()); }
   const TCHAR*   ClassName()             { return GetString(IDS_CLASSNAME); }
   SClass_ID      SuperClassID()          { return GUP_CLASS_ID; }
   Class_ID       ClassID()               { return PLUGIN_MANAGER_CLASSID; }
   const TCHAR*   Category()              { return _T("");  }  
   HINSTANCE      HInstance()             { return hInstance; }
};

static PlugMgrClassDesc PlugMgrCD;
ClassDesc* GetPlugMgrDesc() {return &PlugMgrCD;}

/*===========================================================================*\
 | Action Items Descriptor
\*===========================================================================*/

class PluginManagerActions: public FPStaticInterface {
   public:     
		 FPStatus ShowPluginManager() 
		 { 
			 PlugMgrUtility::GetInstance().SetVisible(); 
			 return FPS_OK; 
		 }
      
      // action dispatch
      DECLARE_DESCRIPTOR(PluginManagerActions)
      enum ActionID { pm_show };
      BEGIN_FUNCTION_MAP
         FN_ACTION(pm_show, ShowPluginManager);
      END_FUNCTION_MAP
};

#define PLUGINMANAGER_ACT_INTERFACE Interface_ID(0x5e3b07f6, 0x273f7de8)

static PluginManagerActions sPluginManagerActions(
   PLUGINMANAGER_ACT_INTERFACE, _T("PluginMgrAction"), IDS_CLASSNAME, &PlugMgrCD, FP_ACTIONS, kActionMainUIContext, 
      PluginManagerActions::pm_show, _T("show"), IDS_CLASSNAME, 0,
      p_end,
   p_end);

void PlugMgrUtility::init()
{
	popup1 = NULL;
	ip = NULL;
	pmImages = NULL;
	UI_active = FALSE;
	columnActive = 0;
	pdrec = NULL;
}

DWORD PlugMgrUtility::Start( ) 
{
	popup1 = NULL;
	ip = Max();

	recentArray.SetSize(0);

	UI_active = FALSE;
	columnActive = 1;


	return GUPRESULT_KEEP;
}

void PlugMgrUtility::Stop( ) 
{
}

DWORD_PTR PlugMgrUtility::Control( DWORD parameter ) 
{
	if (parameter==1)
		SetVisible(TRUE);
	else if(parameter==0) 
		SetVisible(FALSE);
	return 0;
}

void PlugMgrUtility::SetVisible(BOOL show)
{
	if (show && !popup1) // make visible
	{
		popup1 = CreateDialogParam(
			hInstance,
			MAKEINTRESOURCE(IDD_MAIN),
			GetCOREInterface()->GetMAXHWnd(),
			PMDefaultDlgProc,
			(LPARAM)this);
		DLSetWindowLongPtr(popup1, this);
	}
	else if(!show && popup1) // hide it
	{
		DestroyWindow(popup1);
		popup1 = NULL;
	}
}

BOOL PlugMgrUtility::IsVisible()
{
	return (popup1) ? TRUE : FALSE;
}

void PlugMgrUtility::LoadClass(ClassDesc* cd)         // ensure deferred plugin class is loaded - JBW 7.4.00
{
   ClassDirectory& cdir = GetCOREInterface()->GetDllDir().ClassDir();
   cdir.FindClassEntry(cd->SuperClassID(), cd->ClassID())->FullCD();
}

int	PlugMgrUtility::GetPluginDllCount()	
{
	const int countTotal = CountAllPlugins();
	DbgAssert( countTotal == (GetDeferredPluginDllCount() + GetLoadedPluginDllCount()));
	return countTotal;
}

int PlugMgrUtility::GetDeferredPluginDllCount()	
{
	return CountPlugins(false);	
}

int PlugMgrUtility::GetLoadedPluginDllCount()	
{
	return CountPlugins(true);
}

int PlugMgrUtility::CountPlugins(bool isLoaded) 
{
	int result = 0;
	const int pluginCount = CountAllPlugins();
	for (int i=0; i < pluginCount; i++)	
	{
		if (isLoaded == GetCOREInterface()->GetDllDir()[i].IsLoaded())	
		{
			result++;
		}
	}
	return result;
}

const MCHAR* PlugMgrUtility::GetPluginDllName(int index)	
{
	if (!PluginIndexIsValid(index))	
	{
		return NULL;
	}
	return GetCOREInterface()->GetDllDir()[index].GetFileName();
}

const MCHAR* PlugMgrUtility::GetPluginDllDirectory(int index)	
{
	if (!PluginIndexIsValid(index))	
	{
		return NULL;
	}
	return GetCOREInterface()->GetDllDir()[index].GetFullPath();
}

MaxSDK::Util::Path PlugMgrUtility::GetPluginDllFullPath(int index)	
{
	if (!PluginIndexIsValid(index))	
	{
		return NULL;
	}
	return GetCOREInterface()->GetDllDir()[index].GetFullPath();
}

MSTR PlugMgrUtility::fpGetPluginDllFullPath(int index)	
{
	return MSTR(GetPluginDllFullPath(index).GetCStr());
}

int PlugMgrUtility::GetPluginDllSize(int index) 
{
	if (!PluginIndexIsValid(index))	
	{
		return 0;
	}
	// A bit brute force but needed in case mxs exposure of pluginmanager 
	// is invoked before its UI is ever opened
	rebuildCache(GetCOREInterface(), this, NULL);
	// Because the plugin's name and loaded status are retrived from the DllDir, 
	// in order to keep methods about a plug-in consistent in behaviour, 
	// but the size of the plugin is kept in PlugMgrUtility::ddList, 
	// we need to look for the item in PlugMgrUtility::ddList whose realIdx matches 
	// the one passed in as parameter
	for (int i = 0; i< ddList.Count(); i++)
	{
		if (index == ddList[i].mDllDirIndex)
		{
			return ddList[i].mPluginSizeKb;
		}
	}

	DbgAssert(false && _M("Plugin not found in PlugMgrUtility::ddList"));
	return 0;
}

bool PlugMgrUtility::LoadPluginDll(int index)
{
	DllDir& dllDir = GetCOREInterface()->GetDllDir();
	const DllDesc& dllDesc = dllDir[index];
	if (!dllDesc.IsLoaded())
	{
		return dllDir.LoadADll(dllDesc.GetFullPath(), true);
	}

	return true;
}

bool PlugMgrUtility::IsPluginDllLoaded(int index)	
{
	if (!PluginIndexIsValid(index))	
	{
		return NULL;
	}
	return GetCOREInterface()->GetDllDir()[index].IsLoaded();
}

int PlugMgrUtility::GetLoadedPluginDllSize() 
{
	int sizeKb = 0;
	// CountAllPlugins updates ddList
	const int pluginCount = CountAllPlugins();
	// The pluginManager is not in ddList
	for (int i=0; i < pluginCount; i++) 
	{
		if (ddList[i].mDllDesc->IsLoaded())
		{
			sizeKb += ddList[i].mPluginSizeKb;
		}
	}
	return sizeKb;
}

bool PlugMgrUtility::PluginIndexIsValid(int index)	const 
{
	return (0 <= index && index < GetCOREInterface()->GetDllDir().Count());
}

int PlugMgrUtility::CountAllPlugins() 
{
	// A bit brute force but needed in case mxs exposure of pluginmanager 
	// is invoked before its UI is ever opened
	rebuildCache(GetCOREInterface(), this, NULL);
	int dllDirCount = GetCOREInterface()->GetDllDir().Count();

	DbgAssert(ddList.Count() == dllDirCount); 
	return dllDirCount;
}

