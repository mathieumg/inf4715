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


#include "UnwrapCustomToolBars.h"


ToolBarFrameFloating::ToolBarFrameFloating(HINSTANCE instance, HWND parentWindow, int x, int y, int w, int h, const TSTR &name, const TSTR &iniFileName)
{
	mhInstance = instance;
	Init();

	mParentWindow = parentWindow;

	mX = x;
	mY = y;
	mW = w;
	mH = h;
	mName.printf(_T("Floater_%s"),name);

	mINIFileName = iniFileName;

	Create();

}

ToolBarFrameFloating::ToolBarFrameFloating()
{
	Init();
}

ToolBarFrameFloating::~ToolBarFrameFloating()
{
	Show(false);
	Free();
}

void ToolBarFrameFloating::Init()
{
	mParentWindow = NULL;
	mCUIFrameWindow = NULL;
	mToolBarWindow = NULL;

	mCUIFrame = NULL;
	mToolBar = NULL;

	mX = 0;
	mY = 0;
	mW = 0;
	mH = 0;

}

void ToolBarFrameFloating::Free()
{
	if (mToolBar)
		ReleaseICustToolbar(mToolBar);
	mToolBar = NULL;
	if (mCUIFrame)
		ReleaseICUIFrame(mCUIFrame);
	mCUIFrame = NULL;
}

void ToolBarFrameFloating::Save()
{
	if (mCUIFrame)
	{
		mCUIFrame->WriteConfig(mINIFileName.data());
	}
}

HWND ToolBarFrameFloating::GetParentWindow()
{
	return mParentWindow;
}
HWND ToolBarFrameFloating::GetCUIFrameWindow()
{
	return mCUIFrameWindow;
}
HWND ToolBarFrameFloating::GetToolbarWindow()
{
	return mToolBarWindow;
}

ICUIFrame* ToolBarFrameFloating::GetFrame()
{
	return mCUIFrame;
}
ICustToolbar* ToolBarFrameFloating::GetToolBar()
{
	return mToolBar;
}

void  ToolBarFrameFloating::Show(bool vis)
{
	if (vis)
	{
		if (mCUIFrame)
		{
			ShowWindow(mCUIFrameWindow,SW_SHOW);
			mCUIFrame->Hide(FALSE);	
		}
	}
	else
	{
		if (mCUIFrame)
		{
			ShowWindow(mCUIFrameWindow,SW_HIDE);
			mCUIFrame->Hide(TRUE);
		}
	}

}

void ToolBarFrameFloating::ResizeWindow(int x, int y, int w, int h)
{
	MoveWindow(mCUIFrameWindow,x,y,w,h,TRUE);		
	MoveWindow(mToolBarWindow,0,0,w,h,TRUE);		
}

void  ToolBarFrameFloating::Create()
{
	//set up toolbar	
	mCUIFrameWindow = CreateCUIFrameWindow(mParentWindow, mName, 0, 0, mW, mH);
	mCUIFrame = ::GetICUIFrame(mCUIFrameWindow);

	mCUIFrame->SetName(mName.data());

	//see if we have an existing toolbar if so use that
	if (!mCUIFrame->ReadConfig(mINIFileName.data(),TRUE))		
	{
		// -- Now create the toolbar window
		mToolBarWindow = CreateWindow(CUSTTOOLBARWINDOWCLASS,NULL,WS_CHILD | WS_VISIBLE, 0, 0, mW, mH,mCUIFrameWindow,NULL,mhInstance,NULL);

		// -- Now link the toolbar to the CUI frame
		mToolBar = GetICustToolbar(mToolBarWindow);
		mToolBar->LinkToCUIFrame(mCUIFrameWindow, NULL);
		mToolBar->SetBottomBorder(FALSE);
		mToolBar->SetTopBorder(FALSE);

		mCUIFrame->SetName(mName.data());
	}
	else //we have one just get the handle
	{
		mToolBar = GetICustToolbar( mCUIFrame->GetContentHandle());
		mToolBarWindow = mToolBar->GetHwnd();
	}
	//set the type of tool bar
	mCUIFrame->SetContentType(CUI_TOOLBAR);	
	mCUIFrame->SetPosType( CUI_FLOATABLE | CUI_SM_HANDLES | CUI_DONT_SAVE);
	mCUIFrame->SetMenuDisplay(CUI_MENU_HIDE);
	mCUIFrame->SetSystemWindow(FALSE);

	SIZE sz; RECT rect;
	mToolBar->GetFloatingCUIFrameSize(&sz);
	rect.top = 200; rect.left = 200;
	rect.right = rect.left+sz.cx; rect.bottom = rect.top+sz.cy;
	GetCUIFrameMgr()->FloatCUIWindow(mCUIFrameWindow, &rect);
	MoveWindow(mCUIFrameWindow, rect.left, rect.right, sz.cx, sz.cy, TRUE);
}
