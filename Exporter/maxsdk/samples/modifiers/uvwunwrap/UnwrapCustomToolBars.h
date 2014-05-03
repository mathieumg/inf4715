//**************************************************************************/
// Copyright (c) 1998-2010 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// DESCRIPTION: Unwrap UI classes
// AUTHOR: Peter Watje
// DATE: 2010/08/31 
//***************************************************************************/

#pragma once

#include <windowsx.h>
#include "custcont.h"


class UnwrapMod;

/* 
this is a wrapper around the Max ICustToolbar

*/
class ToolBarFrame
{
public:

	enum DockPos {kDockUpperLeft,kDockUpperRight, kDockLowerLeft, kDockLowerRight, kFloat};

	ToolBarFrame( HWND hwnd, int owner, const TSTR &name);

	ToolBarFrame(HINSTANCE instance, int owner, HWND parentWindow, int x, int y, int w, int h, DockPos location, bool popup, const TSTR &name);
	ToolBarFrame(HINSTANCE instance, const MCHAR* section, const MCHAR *iniFile, HWND parent, HWND toolbarHWND);
	~ToolBarFrame();

	void UpdateWindow();
	
	void ResizeWindow(int x, int y, int w, int h, BOOL save = TRUE);

	HWND GetParWindow();
	HWND GetToolbarWindow();

	ICustToolbar* GetToolBar();

	DockPos Location();
	int Width();
	int Height();

	void Show(bool vis);

	void Save(const MCHAR* section, const MCHAR* iniFileName);
	void Load(const MCHAR* section, const MCHAR* iniFileName);

	//Frees the CUI Frame and Toolbar
	void Free();

	bool Loaded();

	const MCHAR* Name();
	int	NumItems();

	int Owner();
	Tab<int> LoadIDs();
	void LoadIDsClear();

	void AddButton(int id, ToolButtonItem item, const MCHAR* toolTip, FlyOffData *flyOff, int flyOffCount, int flyOffValue);

protected:
	ToolBarFrame();

	void Init();

	//creates the toolbar if toolBarHWND is NULL a window will be created in its place
	void Create(HWND toolbarHWND);

	HWND mParentWindow;

	HWND mDummyWindow;
	HWND mPopUpWindow;

	HWND mToolBarWindow;
	ICustToolbar* mToolBar;

	int mOwner;
	TSTR mName;
	DockPos		mLocation;    
	int		mX,mY,mW,mH;
	bool	mPopup;

	bool mCenter;

	HINSTANCE mhInstance;

	bool mLoaded;
	HIMAGELIST mImages;

	Tab<int> mLoadIds;

};

/*
This is a wrapper around the CUIFrame when it is used as floating pallette
*/
class ToolBarFrameFloating
{
public:
	ToolBarFrameFloating(HINSTANCE instance, HWND parentWindow, int x, int y, int w, int h, const TSTR &name, const TSTR &iniFileName);
	~ToolBarFrameFloating();

	void ResizeWindow(int x, int y, int w, int h);

	HWND GetParentWindow();
	HWND GetCUIFrameWindow();
	HWND GetToolbarWindow();

	ICUIFrame* GetFrame();
	ICustToolbar* GetToolBar();

	void Show(bool vis);

	void Save();

	//Frees the CUI Frame and Toolbar
	void Free();

	

protected:
	ToolBarFrameFloating();

	void Init();

	void Create();

	HWND mParentWindow;

	HWND mCUIFrameWindow;
	ICUIFrame* mCUIFrame;

	HWND mToolBarWindow;
	ICustToolbar* mToolBar;

	TSTR	mName;
	TSTR	mINIFileName;
	int		mX,mY,mW,mH;

	HINSTANCE mhInstance;

};