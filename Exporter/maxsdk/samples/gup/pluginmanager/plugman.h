// Copyright 2008 Autodesk, Inc.  All rights reserved.
//
// Use of this software is subject to the terms of the Autodesk license
// agreement provided at the time of installation or download, or which
// otherwise accompanies this software in either electronic or hard copy form.  
//
//
/*===========================================================================*\
 | 
 |	NOTE:	The plugin manager also has minimal support for unloading plugins. 
 |			You can enable it by uncommenting the following lines in from Plugman.rc
 |			MENUITEM "Unload",                      ID_DLLMENU_UNLOAD
 |			MENUITEM "Unload",                      ID_TAG_UNLOAD
 |
 |			Unloading plugins is an untested feature and might lead to crashes,
 |			if you try to unload a plugin that is currently being used. 
 |			USE IT AT YOUR OWN RISK
 |			
\*===========================================================================*/

#ifndef __GUPSKEL__H
#define __GUPSKEL__H


#include <ifnpub.h>
#include <bmmlib.h>
#include <guplib.h>
#include <Noncopyable.h>
#include <bitarray.h>
#include <CommCtrl.h>
#include <path.h>
#include "resource.h"

// IMPORTANT:
// The ClassID must be changed whenever a new project
// is created using this skeleton
static const Class_ID PLUGIN_MANAGER_CLASSID = Class_ID(0x61c2789d, 0x6dc3025f);

TCHAR *GetString(int id);
extern ClassDesc* GetPlugMgrDesc();

class DllDesc;
class Interface;
class BitArrayCallback;
class PlugMgrUtility;


/*===========================================================================*\
 |	DLL Management functions
\*===========================================================================*/

// recalc the class usage in the scene
void ComputeClassUse(Interface *ip);

// open file selector and load in chosen plugin
bool LoadNewDLL( HWND hWnd, Interface *ip);

// load in given plugin
bool LoadExistingDLL( const DllDesc& dllDesc, HWND hWnd);

// check to see if a plugin has entries in the class lists (ie, deferred)
BOOL pluginHasClassesRegistered( int masterCode, Interface *ip );

// refresh subsections of the UI after a DLL addition
void DeepRefreshUI(Interface *ip);

// fill the listview
void PVRefresh( HWND hWnd, PlugMgrUtility *pmu, BOOL rebuild );

// enumerate the selection and call BAC's proc
void PVSelectionEnumerator( HWND hWnd, BitArrayCallback &bac);

// rebuild our internal dll dir representation
bool rebuildCache( Interface *ip, PlugMgrUtility *pmu, HWND hWnd );


struct PluginData
{
	PluginData(const DllDesc* dllDesc, int dllDirIndex, int pluginSizeKb) : mDllDesc(dllDesc), mDllDirIndex(dllDirIndex), mPluginSizeKb(pluginSizeKb) { } 
	const DllDesc* mDllDesc;
	int mDllDirIndex;
	int mPluginSizeKb;
	PluginData(); // disallowed
};

// This class adds user interface and maxscript exposure for class DllDir
class PlugMgrUtility : public GUP, public FPStaticInterface, public MaxSDK::Util::Noncopyable 
{
	public:
		HWND			popup1;
		BitArray		recentArray;
		Tab<PluginData>	ddList;
		Tab<DllDesc*>	newlyLoaded;
		Interface		*ip;
		HIMAGELIST		pmImages;
		BOOL			UI_active;

		int				columnActive;
		BOOL*			pdrec;

		~PlugMgrUtility	( ) {}
		
		// GUP Methods
		void	DeleteThis		( )	{ }
		DWORD	Start			( );
		void	Stop			( );
		DWORD_PTR	Control			( DWORD parameter );

		// PlugMgrUtility functionality

		// Opens or Closes the pluginManager UI
		void	SetVisible(BOOL show=TRUE);
		BOOL	IsVisible();
		
		// Loads a plug-in dll that exposes the specified class descriptor
		void	LoadClass(ClassDesc* cd);
		
		// Returns the total number of plug-in dlls registered with the system
		int GetPluginDllCount();
		
		// Returns the file name of the i-th plug-in dll
		// \param index - the index into DllDir's plug-in registry
		const MCHAR* GetPluginDllName(int index);
		
		// Returns the directory (folder) of the i-th plug-in dll
		// \param index - the index into DllDir's plug-in registry
		const MCHAR* GetPluginDllDirectory(int index);

		// Returns the full path (folder + file name) of the i-th plug-in dll
		// \param index - the index into DllDir's plug-in registry
		MaxSDK::Util::Path GetPluginDllFullPath(int index);

		// Returns whether the i-th plug-in dll is loaded or not.
		// A plug-in dll that is not loaded is deferred
		// \param index - the index into DllDir's plug-in registry
		bool IsPluginDllLoaded(int index);
		
		// Returns the size of the memory occupied by the binary image of the i-th plug-in dll
		// Note that this size can be different than the size of the plug-in dll file on disk.
		// \param index - the index into DllDir's plug-in registry
		// return - The size in kilobtes of the memory occupied by the binary image of the plug-in dll
		// If the plug-in is deferred, the memory its image occupies is zero.
		int GetPluginDllSize(int index);
		
		// Loads the i-th plug-in dll
		// \param index - the index into DllDir's plug-in registry
		// return - true if the operation succeeded, false otherwise
		bool LoadPluginDll(int index);

		// Returns the size of the memory occupied by all loaded plug-ins.
		int GetLoadedPluginDllSize();

		// singleton access
		static PlugMgrUtility& GetInstance();

	protected:
		// A wrapper for GetPluginDllFullPath since the function publishing system does not
		// undertsand Path objects
		// \param index - the index into DllDir's plug-in registry
		MSTR fpGetPluginDllFullPath(int index);

		// function IDs 
		enum { 
			pm_setVisible, 
			pm_isVisible, 
			pm_loadClass,  
			pm_pluginDllCount, 
			pm_pluginDllSize, 
			pm_pluginDllName,
			pm_pluginDllDirectory,
			pm_pluginDllFullPath,
			pm_isPluginDllLoaded,
			pm_loadPluginDll,
			pm_loadedPluginDllSize,
		}; 

		DECLARE_DESCRIPTOR_INIT(PlugMgrUtility)
		// dispatch map
		BEGIN_FUNCTION_MAP
			VFN_1(pm_loadClass,    LoadClass, TYPE_CLASS);
			FN_1(pm_pluginDllName, TYPE_STRING, GetPluginDllName, TYPE_INDEX);
			FN_1(pm_pluginDllDirectory, TYPE_STRING, GetPluginDllDirectory, TYPE_INDEX);
			FN_1(pm_pluginDllFullPath, TYPE_TSTR_BV, fpGetPluginDllFullPath, TYPE_INDEX);
			FN_1(pm_pluginDllSize, TYPE_INT, GetPluginDllSize, TYPE_INDEX);
			FN_1(pm_isPluginDllLoaded, TYPE_BOOL, IsPluginDllLoaded, TYPE_INDEX);
			FN_1(pm_loadPluginDll, TYPE_BOOL, LoadPluginDll, TYPE_INDEX);
			PROP_FNS(pm_isVisible, IsVisible, pm_setVisible, SetVisible, TYPE_BOOL);			
			RO_PROP_FN(pm_pluginDllCount, GetPluginDllCount, TYPE_INT);
			RO_PROP_FN(pm_loadedPluginDllSize, GetLoadedPluginDllSize, TYPE_INT);
		END_FUNCTION_MAP 

		static PlugMgrUtility mInstance;

	private:
		bool PluginIndexIsValid(int index) const;
		int CountPlugins(bool isLoaded) ;
		int CountAllPlugins();
		int GetDeferredPluginDllCount();
		int GetLoadedPluginDllCount();
		
		// UI related methods
		static INT_PTR CALLBACK PMDefaultDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void	Init(HWND hWnd);
		void	Destroy(HWND hWnd);
};

#endif