
/*

Copyright 2010 Autodesk, Inc.  All rights reserved. 

Use of this software is subject to the terms of the Autodesk license agreement provided at 
the time of installation or download, or which otherwise accompanies this software in either 
electronic or hard copy form. 

*/

//**************************************************************************/
// DESCRIPTION: Unwrap UI classes
// AUTHOR: Peter Watje
// DATE: 2010/08/31 
//***************************************************************************/

/*


*/

#include "UnwrapCustomToolBars.h"
#include "custcont.h"
#include "dbgprint.h"
#include <windowsx.h>
#include "unwrap.h"
#include "modsres.h"
#include <Util\IniUtil.h> // MaxSDK::Util::WritePrivateProfileString

static LRESULT CALLBACK PopUpToolbarProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{	
	
	if (msg == WM_NOTIFY)
	{
		// This is where we provide the tooltip text for the
		// toolbar buttons...
		LPNMHDR hdr = (LPNMHDR)lParam;
		if ( (hdr->code == TTN_POP) ||
			 (hdr->code == TTN_SHOW) )
		{
			SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		}
	}
	else if ( ( msg == WM_COMMAND) ||		 
		 ( msg == WM_CUSTEDIT_ENTER) ||
		 ( msg == CC_SPINNER_BUTTONUP) ||
		 ( msg == CC_SPINNER_BUTTONDOWN) ||
		 ( msg == CC_SPINNER_CHANGE) )
	{
		HWND parent = GetParent(hWnd);
		return SendMessage(parent,msg,wParam,lParam);
	}	
	return DefWindowProc(hWnd,msg,wParam,lParam);;
}


ToolBarFrame::ToolBarFrame(HINSTANCE instance, int owner, HWND parentWindow, int x, int y, int w, int h, DockPos location, bool popup, const TSTR &name)
{
	mOwner = owner;
	mhInstance = instance;
	Init();
	

	mParentWindow = parentWindow;

	mX = x;
	mY = y;
	mW = w;
	mH = h;
	mName = name;

	mLocation = location;
	mPopup = popup;


	Create(NULL);
	if (mToolBar)
		mToolBar->SetImage(mImages);
}

ToolBarFrame::ToolBarFrame(HINSTANCE instance, const MCHAR* section, const MCHAR *iniFile, HWND parent, HWND toolbarHWND)
{
	mhInstance = instance;
	Init();	
	mParentWindow = parent;
	
	Load(section,iniFile);
	if (mLoaded)
		Create(toolbarHWND);
	if (mToolBar)
		mToolBar->SetImage(mImages);

}

ToolBarFrame::ToolBarFrame( HWND hwnd,int owner, const TSTR &name)
{
	mhInstance = NULL;
	Init();
	mToolBarWindow = hwnd;
	mParentWindow = GetParent(hwnd);
	mName = name;
	mOwner = owner;

	Rect r;
	GetClientRect(mToolBarWindow,&r);
	
	WINDOWPLACEMENT p;
	p.length = sizeof(WINDOWPLACEMENT);

	GetWindowPlacement(mToolBarWindow,&p);
//	GetWindowRect(mToolBarWindow,&r);
	mX = p.rcNormalPosition.left;
	mY = p.rcNormalPosition.top;
	mW = p.rcNormalPosition.right-p.rcNormalPosition.left;
	mH = p.rcNormalPosition.bottom- p.rcNormalPosition.top;

	mLocation = kFloat;
	mPopup = FALSE;

	mToolBar = GetICustToolbar(mToolBarWindow);
	mToolBar->SetBottomBorder(FALSE);
	mToolBar->SetTopBorder(FALSE);

	//set the type of tool bar
	mToolBar->SetNumRows(1);
	mToolBar->SetImage(mImages);
	ShowWindow(mToolBarWindow,SW_SHOW);
}

ToolBarFrame::ToolBarFrame()
{
	Init();
}

ToolBarFrame::~ToolBarFrame()
{
	Free();
}

void ToolBarFrame::Init()
{
	mParentWindow = NULL;
	mToolBarWindow = NULL;
	mPopUpWindow = NULL;
	mDummyWindow = NULL;

	mToolBar = NULL;
	mOwner = 0;

	mX = 0;
	mY = 0;
	mW = 0;
	mH = 0;

	mLocation = kFloat;

	mLoaded = false;

	mCenter = false;

	mImages = NULL; 
	

	static BOOL registered = FALSE;
	if (!registered && mhInstance) {
		registered = TRUE;
		WNDCLASS  wc;
		wc.style         = 0;
		wc.hInstance     = mhInstance;
		wc.hIcon         = NULL;
		wc.hCursor       = NULL;
		wc.hbrBackground = NULL; 
		wc.lpszMenuName  = NULL;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.lpfnWndProc   = PopUpToolbarProc;
		wc.lpszClassName = _M("ToolBarFrameWindow");
		RegisterClass(&wc);

	}

	HBITMAP hBitmap, hMask;	
	mImages = ImageList_Create(16, 15, TRUE, 4, 0);
	hBitmap = LoadBitmap(mhInstance,MAKEINTRESOURCE(IDB_UNWRAP_TRANSFORM));
	hMask   = LoadBitmap(mhInstance,MAKEINTRESOURCE(IDB_UNWRAP_TRANSFORM_MASK));
	ImageList_Add(mImages,hBitmap,hMask);
	DeleteObject(hBitmap);
	DeleteObject(hMask);
}

void ToolBarFrame::Free()
{

	
	if (mImages) ImageList_Destroy(mImages);	
	mImages = NULL;
	if (mToolBar)
		ReleaseICustToolbar(mToolBar);
	mToolBar = NULL;

	if (mToolBarWindow)
		DestroyWindow(mToolBarWindow);
	if (mDummyWindow)
		DestroyWindow(mDummyWindow);
	if (mPopUpWindow)
	{
		GetCOREInterface()->UnRegisterDlgWnd ( mPopUpWindow );
		DestroyWindow(mPopUpWindow);
	}

	mParentWindow = NULL;
	mToolBarWindow = NULL;
	mPopUpWindow = NULL;
	mDummyWindow = NULL;
}

void ToolBarFrame::Save(const MCHAR* section, const MCHAR* iniFileName)
{

/*
	[ToolBar_1]
	Name = 
		Owner =
		SubOwner = 
		Location = 
		X = 
		y = 
		W = 
		H = 
		Popup = 
		ItemCount = 
		Items = 
*/

	TSTR str;	
	MaxSDK::Util::WritePrivateProfileString(section,_T("Name"),mName,iniFileName);
	str.printf(_T("%d"),mOwner);
	MaxSDK::Util::WritePrivateProfileString(section,_T("Owner"),str,iniFileName);
	str.printf(_T("%d"),0);
	MaxSDK::Util::WritePrivateProfileString(section,_T("RollupName"),str,iniFileName);
	str.printf(_T("%d"),mLocation);
	MaxSDK::Util::WritePrivateProfileString(section,_T("Location"),str,iniFileName);
	str.printf(_T("%d"),mX);
	MaxSDK::Util::WritePrivateProfileString(section,_T("X"),str,iniFileName);
	str.printf(_T("%d"),mY);
	MaxSDK::Util::WritePrivateProfileString(section,_T("Y"),str,iniFileName);
	str.printf(_T("%d"),mW);
	MaxSDK::Util::WritePrivateProfileString(section,_T("W"),str,iniFileName);
	str.printf(_T("%d"),mH);
	MaxSDK::Util::WritePrivateProfileString(section,_T("H"),str,iniFileName);
	str.printf(_T("%d"),mPopup);
	MaxSDK::Util::WritePrivateProfileString(section,_M("Popup"),str,iniFileName);
	int itemCount = mToolBar->GetNumItems();
	str.printf(_T("%d"),itemCount);
	MaxSDK::Util::WritePrivateProfileString(section,_T("ItemCount"),str,iniFileName);
	
	for (int i = 0; i < itemCount; i++)
	{
		TSTR key;
		key.printf(_T("Item%d"),i);
		int itemID = mToolBar->GetItemID(i);

		str.printf(_T("%d"),itemID);
		MaxSDK::Util::WritePrivateProfileString(section,key,str,iniFileName);
	}
	

}

void ToolBarFrame::Load(const MCHAR* section, const MCHAR* iniFileName)
{

	MCHAR str[MAX_PATH];
	TSTR def(_T("DISCARD"));

	
	int res = GetPrivateProfileString(section,_T("Name"),def,str,MAX_PATH,iniFileName);
	if ((res) && (_tcscmp(str,def))) 
		mName.printf(_T("%s"),str);
	else
	{
		mLoaded = false;
		return;
	}

	res = GetPrivateProfileString(section,_T("Owner"),def,str,MAX_PATH,iniFileName);
	if ((res) && (_tcscmp(str,def))) 
		_stscanf(str,_T("%d"),&mOwner);

	TSTR rollupName;
	res = GetPrivateProfileString(section,_T("RollupName"),def,str,MAX_PATH,iniFileName);
	if ((res) && (_tcscmp(str,def))) 
		rollupName.printf(_T("%s"),str);
	
	res = GetPrivateProfileString(section,_T("Location"),def,str,MAX_PATH,iniFileName);
	if ((res) && (_tcscmp(str,def))) 
		_stscanf(str,_T("%d"),&mLocation);

	res = GetPrivateProfileString(section,_T("X"),def,str,MAX_PATH,iniFileName);
	if ((res) && (_tcscmp(str,def))) 
		_stscanf(str,_T("%d"),&mX);

	res = GetPrivateProfileString(section,_T("Y"),def,str,MAX_PATH,iniFileName);
	if ((res) && (_tcscmp(str,def))) 
		_stscanf(str,_T("%d"),&mY);

	res = GetPrivateProfileString(section,_T("W"),def,str,MAX_PATH,iniFileName);
	if ((res) && (_tcscmp(str,def))) 
		_stscanf(str,_T("%d"),&mW);

	res = GetPrivateProfileString(section,_T("H"),def,str,MAX_PATH,iniFileName);
	if ((res) && (_tcscmp(str,def))) 
		_stscanf(str,_T("%d"),&mH);

	res = GetPrivateProfileString(section,_T("Popup"),def,str,MAX_PATH,iniFileName);
	if ((res) && (_tcscmp(str,def))) 
		_stscanf(str,_T("%d"),&mPopup);

	int itemCount = 0;
	res = GetPrivateProfileString(section,_T("ItemCount"),def,str,MAX_PATH,iniFileName);
	if ((res) && (_tcscmp(str,def))) 
		_stscanf(str,_T("%d"),&itemCount);

	mLoadIds.SetCount(itemCount);
	for (int i = 0; i < itemCount; i++)
	{
		TSTR itemStr;
		itemStr.printf(_T("Item%d"),i);
		int res = GetPrivateProfileString(section,itemStr,def,str,MAX_PATH,iniFileName);
		int id = 0;
		if ((res) && (_tcscmp(str,def))) 
			_stscanf(str,_T("%d"),&id);
		mLoadIds[i]=id;
	}

	mLoaded = true;		
}

Tab<int> ToolBarFrame::LoadIDs()
{
	return mLoadIds;
}

void ToolBarFrame::LoadIDsClear()
{
	mLoadIds.SetCount(0);
}


int ToolBarFrame::Owner()
{
	return mOwner;
}

ToolBarFrame::DockPos ToolBarFrame::Location()
{
	return mLocation;
}


HWND ToolBarFrame::GetParWindow()
{
	return mParentWindow;
}

HWND ToolBarFrame::GetToolbarWindow()
{
	return mToolBarWindow;
}

ICustToolbar* ToolBarFrame::GetToolBar()
{
	return mToolBar;
}

void  ToolBarFrame::Show(bool vis)
{
	if (vis)
	{
		ShowWindow(mToolBarWindow,SW_SHOW);
		if (mPopup)
			ShowWindow(mPopUpWindow,SW_SHOW);
	}
	else
	{
		ShowWindow(mToolBarWindow,SW_HIDE);
		if (mPopup)
			ShowWindow(mPopUpWindow,SW_HIDE);

	}

}

void ToolBarFrame::ResizeWindow(int x, int y, int w, int h, BOOL save)
{

	int sx,sy,sw,sh;

	sx = mX;
	sy = mY;
	sw = mW;
	sh = mH;

	mX = x;
	mY = y;
	mW = w;
	mH = h;

	UpdateWindow();

	if (!save)
	{
		mX = sx;
		mY = sy;
		mW = sw;
		mH = sh;
	}


}

void  ToolBarFrame::UpdateWindow()
{


	if (mPopup)
	{
		MoveWindow(mDummyWindow,mX,mY,1,1,TRUE);

		Rect r;
		GetWindowRect(mDummyWindow,&r);
		MoveWindow(mPopUpWindow,r.x(),r.y(),mW,mH,TRUE);		
		MoveWindow(mToolBarWindow,0,0,mW,mH,TRUE);		

	}
	else
		MoveWindow(mToolBarWindow,mX,mY,mW,mH,TRUE);

}





void  ToolBarFrame::Create(HWND toolbarHWND)
{
	//create our window if need be


	if (toolbarHWND)
	{
		mToolBarWindow = toolbarHWND;
	}
	else
	{
	if (mPopup)
	{

		mPopUpWindow = CreateWindow(_M("ToolBarFrameWindow"),mName.data(),WS_POPUP|WS_BORDER | WS_VISIBLE, mX, mY, mW, mH, mParentWindow,NULL,mhInstance,NULL);
		mDummyWindow = CreateWindow(_M("ToolBarFrameWindow"),_M("DummyWindow"),WS_CHILD | WS_BORDER | WS_VISIBLE, mX, mY, mW, mH, mParentWindow,NULL,mhInstance,NULL);
			GetCOREInterface()->RegisterDlgWnd ( mPopUpWindow );
	}

	//set up toolbar	
	if (mPopup)
		mToolBarWindow = CreateWindow(CUSTTOOLBARWINDOWCLASS,NULL,WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 0, 0, mW, mH,mPopUpWindow,NULL,mhInstance,NULL);
	else
			mToolBarWindow = CreateWindow(CUSTTOOLBARWINDOWCLASS,NULL,WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS , 0, 0, mW, mH,mParentWindow,NULL,mhInstance,NULL);

		
	}


	mToolBar = GetICustToolbar(mToolBarWindow);
	mToolBar->SetBottomBorder(FALSE);
	mToolBar->SetTopBorder(FALSE);

	//set the type of tool bar
	mToolBar->SetNumRows(1);
	mToolBar->SetImage(mImages);
	ShowWindow(mToolBarWindow,SW_SHOW);


}

int ToolBarFrame::Width()
{
	return mW;
}

int ToolBarFrame::Height()
{
	return mH;
}
bool ToolBarFrame::Loaded()
{
	return mLoaded;
}


const MCHAR* ToolBarFrame::Name()
{
	return mName.data();
}

int	ToolBarFrame::NumItems()
{
	return mToolBar->GetNumItems();
}

void ToolBarFrame::AddButton(int id, ToolButtonItem item, const MCHAR* toolTip, FlyOffData *flyOff, int flyOffCount, int flyOffValue)
{
	mToolBar->AddTool(item);
	ICustButton *but  = mToolBar->GetICustButton(id);
	if (but)
	{
		but->SetHighlightColor(GREEN_WASH);
		but->SetTooltip(TRUE,toolTip);
		if (flyOff && (flyOffCount > 0)	)
			but->SetFlyOff(flyOffCount,flyOff,GetCOREInterface()->GetFlyOffTime(),flyOffValue,FLY_VARIABLE);
		ReleaseICustButton(but);
	}
}