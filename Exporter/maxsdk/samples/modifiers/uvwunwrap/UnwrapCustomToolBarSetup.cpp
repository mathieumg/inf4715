/*

Copyright 2010 Autodesk, Inc.  All rights reserved. 

Use of this software is subject to the terms of the Autodesk license agreement provided at 
the time of installation or download, or which otherwise accompanies this software in either 
electronic or hard copy form. 

*/

/*

add method to get corner heights
add missing actions
	
*/

#include "Unwrap.h"
#include "modsres.h"


void UnwrapMod::fnAddFloater(const MCHAR* name)
{
	pblock->Append(unwrap_floaters,1,&name);
	BOOL vis = TRUE;
	pblock->Append(unwrap_floaters_visible,1,&vis);

	TSTR floaterName;
	floaterName.printf(_T("%s"),name);
	ToolBarFrameFloating *floater = new ToolBarFrameFloating(hInstance,hDialogWnd,0,0,400,40,floaterName,mToolBarIniFileName);
	floater->Show(true);
	mFloaters.Append(1,&floater);
}

int		UnwrapMod::GetFloaterIndex(const MCHAR* name)
{
	int ct = pblock->Count(unwrap_floaters);
	for (int i = 0; i < ct; i++)
	{
		const MCHAR *floaterName = pblock->GetStr(unwrap_floaters,0,i);
		if (_tcscmp(floaterName,name) == 0)
			return i;
	}
	return -1;
}

void	UnwrapMod::fnDeleteFloater(const MCHAR* name)
{
	int index = GetFloaterIndex(name);


	if (index != -1)
	{
		if (mFloaters[index])
			delete mFloaters[index];
		mFloaters[index]=NULL;
		mFloaters.Delete(index,1);

		pblock->Delete(unwrap_floaters,index,1);
		pblock->Delete(unwrap_floaters_visible,index,1);
	}
}
void	UnwrapMod::fnShowFloater(const MCHAR* name, BOOL show)
{
	int index = GetFloaterIndex(name);
	if (index != -1)
	{
		if (show)
			mFloaters[index]->Show(true);
		else
			mFloaters[index]->Show(false);

	}
}


void UnwrapMod::fnAddToolBar(int owner, const MCHAR* name, int pos, int x, int y,  int width, BOOL popup)
{
	bool found = false;
	int ct = mUIManager.NumberToolBars();
	for (int i = 0; i < ct; i++)
	{
		const MCHAR* testName = mUIManager.Name(i);
		if (_tcscmp(name,testName) == 0)
		{
			found = true;
			i = ct;
		}
	}

	if (!found)
	{
		if (hDialogWnd)
		{
			AppendToolBar(owner, name,pos,x,y,width,popup);
		}
	}


}

void UnwrapMod::AppendToolBar(int owner, const MCHAR* name, int pos, int x, int y,  int width, BOOL popup )
{
	HWND parentHWND = hDialogWnd;
	if ( (owner >= 100) && (owner <= 199) )
	{
		int ownerID = owner-100;
		IRollupWindow *irollup = GetIRollup(GetDlgItem(hDialogWnd,IDC_SIDEBAR_ROLLOUT));
		if (irollup)
		{
			parentHWND = irollup->GetPanelDlg(ownerID);
			ReleaseIRollup(irollup);
		}
		else 
			parentHWND = NULL;
	}
	else if ( (owner >= 200) && (owner <= 299) )
	{
		parentHWND = GetParent(hSelParams);
	}



	if (parentHWND)
{
	if (popup)
			mUIManager.AppendToolBar(owner, parentHWND,name,pos,x,y,width,mToolBarHeight,true);
	else
			mUIManager.AppendToolBar(owner, parentHWND,name,pos,x,y,width,mToolBarHeight,false);

	SetupDefaultWindows();


	if (hDialogWnd)
	{
		SizeDlg();
		InvalidateView();
	}
}
}

void UnwrapMod::fnShowToolBar(BOOL visible)
{

}

void UnwrapMod::UpdateToolBars()
{
	mUIManager.UpdatePositions();

}

void	UnwrapMod::SetupTransformToolBar(ToolBarFrame *toolBarFrame)
{


	AddActionToToolbar(toolBarFrame,ID_MOVE);
	AddActionToToolbar(toolBarFrame,ID_ROTATE);
	AddActionToToolbar(toolBarFrame,ID_SCALE);
	AddActionToToolbar(toolBarFrame,ID_FREEFORMMODE);
	AddActionToToolbar(toolBarFrame,ID_SEPARATORBAR);
	AddActionToToolbar(toolBarFrame,ID_MIRROR);
}

void	UnwrapMod::SetupOptionToolBar(ToolBarFrame *toolBarFrame)
{
	AddActionToToolbar(toolBarFrame,ID_SHOWMAP);
	AddActionToToolbar(toolBarFrame,ID_UVW);
	AddActionToToolbar(toolBarFrame,ID_PROPERTIES);
	AddActionToToolbar(toolBarFrame,ID_TEXTURE_COMBO);
	
}


void	UnwrapMod::SetupTypeInToolBar(ToolBarFrame *toolBarFrame)
{
	AddActionToToolbar(toolBarFrame,ID_ABSOLUTETYPEIN);
	AddActionToToolbar(toolBarFrame,ID_SEPARATORBAR);
	AddActionToToolbar(toolBarFrame,ID_ABSOLUTETYPEIN_SPINNERS);
	AddActionToToolbar(toolBarFrame,ID_SEPARATORBAR);
}

void	UnwrapMod::SetupViewToolBar(ToolBarFrame *toolBarFrame)
{
	AddActionToToolbar(toolBarFrame,ID_LOCKSELECTED);
	AddActionToToolbar(toolBarFrame,ID_FILTER_SELECTEDFACES);
	AddActionToToolbar(toolBarFrame,ID_SEPARATORBAR);
	AddActionToToolbar(toolBarFrame,ID_HIDE);
	AddActionToToolbar(toolBarFrame,ID_FREEZE);
	AddActionToToolbar(toolBarFrame,ID_SEPARATORBAR);

	AddActionToToolbar(toolBarFrame,ID_FILTER_MATID);


	AddActionToToolbar(toolBarFrame,ID_PAN);
	AddActionToToolbar(toolBarFrame,ID_ZOOMTOOL);
	AddActionToToolbar(toolBarFrame,ID_ZOOMREGION);
	AddActionToToolbar(toolBarFrame,ID_ZOOMEXTENT);
	AddActionToToolbar(toolBarFrame,ID_SNAP);
}

void	UnwrapMod::SetupSelectToolBar(ToolBarFrame *toolBarFrame)
{
	AddActionToToolbar(toolBarFrame,ID_TV_VERTMODE);
	AddActionToToolbar(toolBarFrame,ID_TV_EDGEMODE);
	AddActionToToolbar(toolBarFrame,ID_TV_FACEMODE);
	AddActionToToolbar(toolBarFrame,ID_SEPARATORBAR);
	AddActionToToolbar(toolBarFrame,ID_TV_ELEMENTMODE);
	AddActionToToolbar(toolBarFrame,ID_SEPARATORBAR);
	//subobject modes

	AddActionToToolbar(toolBarFrame,ID_TV_INCSEL);
	AddActionToToolbar(toolBarFrame,ID_TV_DECSEL);

	AddActionToToolbar(toolBarFrame,ID_TV_LOOP);
	AddActionToToolbar(toolBarFrame,ID_TV_LOOPGROW);
	AddActionToToolbar(toolBarFrame,ID_TV_LOOPSHRINK);

	AddActionToToolbar(toolBarFrame,ID_TV_RING);
	AddActionToToolbar(toolBarFrame,ID_TV_RINGGROW);
	AddActionToToolbar(toolBarFrame,ID_TV_RINGSHRINK);


	AddActionToToolbar(toolBarFrame,ID_TV_PAINTSELECTMODE);
	AddActionToToolbar(toolBarFrame,ID_TV_PAINTSELECTINC);
	AddActionToToolbar(toolBarFrame,ID_TV_PAINTSELECTDEC);


}
void	UnwrapMod::SetupSoftSelectToolBar(ToolBarFrame *toolBarFrame)
{
	AddActionToToolbar(toolBarFrame,ID_SOFTSELECTION);
	AddActionToToolbar(toolBarFrame,ID_SOFTSELECTIONSTR);
	AddActionToToolbar(toolBarFrame,ID_FALLOFF);
	AddActionToToolbar(toolBarFrame,ID_FALLOFF_SPACE);
//	AddActionToToolbar(toolBarFrame,ID_SEPARATORHALF);
	AddActionToToolbar(toolBarFrame,ID_SEPARATORBAR);
//	AddActionToToolbar(toolBarFrame,ID_SEPARATORHALF);
	AddActionToToolbar(toolBarFrame,ID_LIMITSOFTSEL);
	AddActionToToolbar(toolBarFrame,ID_SOFTSELECTIONLIMIT);
}


void    UnwrapMod::SetupDefaultWindows()
{
	ToolBarFrame *bar = mUIManager.GetToolBarFrame(_T("_InternalTransform"));
	if ( bar)
	{
		if (bar->NumItems() == 0)
			SetupTransformToolBar(bar);
	}
	bar = mUIManager.GetToolBarFrame(_T("_InternalOption"));
	if ( bar)
	{
		if (bar->NumItems() == 0)
			SetupOptionToolBar(bar);
	}

	bar = mUIManager.GetToolBarFrame(_T("_InternalTypeIn"));
	if ( bar )
	{
		if (bar->NumItems() == 0)
			SetupTypeInToolBar(bar);
	}

	bar = mUIManager.GetToolBarFrame(_T("_InternalView"));
	if ( bar )
	{
		if (bar->NumItems() == 0)
			SetupViewToolBar(bar);
	}

	bar = mUIManager.GetToolBarFrame(_T("_InternalSelect"));
	if ( bar )
	{
		if (bar->NumItems() == 0)
			SetupSelectToolBar(bar);
	}

	bar = mUIManager.GetToolBarFrame(_T("_InternalSoftSelect"));
	if ( bar )
	{
		if (bar->NumItems() == 0)
			SetupSoftSelectToolBar(bar);
	}
}

void    UnwrapMod::FillOutToolBars()
{
	int numBars = mUIManager.NumberToolBars();
	if (mUIManager.GetToolBarFrame(_T("_InternalTransform")) == NULL)
	{
		//add our default bars
		AppendToolBar(0,_T("_InternalTransform"), 0, 0, 0, 148, TRUE );
		AppendToolBar(0,_T("_InternalOption"), 1, 0, 0, 255+16, TRUE );

		AppendToolBar(0,_T("_InternalTypeIn"), 2, 0, 0, 232, TRUE );
		AppendToolBar(0,_T("_InternalSelect"), 2, 0, 0, 420, TRUE );				

		AppendToolBar(0,_T("_InternalView"), 3, 0, 0, 400+16, TRUE );
		AppendToolBar(0,_T("_InternalSoftSelect"), 3, 0, 0, 246-16, TRUE );
		
	}
	else
	{
	for (int i = 0; i < numBars; i++)
	{
		const MCHAR *name = mUIManager.Name(i);
		ToolBarFrame *bar = mUIManager.GetToolBarFrame(name);
		Tab<int> ids;
		ids = bar->LoadIDs();
		for (int j = 0; j < ids.Count(); j++)
		{
			int id = ids[j];

			AddActionToToolbar(bar,id);
		}
			 bar->LoadIDsClear();
		}
	}
}

void    UnwrapMod::SetupToolBarUIs()
{
	//set up toolbar	
	mUIManager.Load(mToolBarIniFileName,0,hDialogWnd);
	FillOutToolBars();
	SetupDefaultWindows();


	mSideBarUI.Setup(hInstance,GetDlgItem(hDialogWnd,IDC_SIDEBAR_ROLLOUT),mToolBarIniFileName);



	//setup the floaters
	int ct = pblock->Count(unwrap_floaters);
	for (int i = 0; i < ct; i++)
	{
		const MCHAR *floaterName = pblock->GetStr(unwrap_floaters,0,i);
		BOOL  floaterVisible = TRUE;
		pblock->GetValue(unwrap_floaters_visible,0,floaterVisible,FOREVER,i);

		ToolBarFrameFloating *floater = new ToolBarFrameFloating(hInstance,hDialogWnd,0,0,400,40,floaterName,mToolBarIniFileName);
		floater->Show(true);
		mFloaters.Append(1,&floater);
	}

}
void     UnwrapMod::TearDownToolBarUIs()
{

	//save our toolbar data out to an ini file
	mUIManager.Save(mToolBarIniFileName);
	mUIManager.Free(hDialogWnd);

	mSideBarUI.TearDown();


	//tear down the floaters
	int ct = pblock->Count(unwrap_floaters);
	for (int i = 0; i < ct; i++)
	{
		if (mFloaters[i])
		{
			mFloaters[i]->Save();
			delete mFloaters[i];
		}
		mFloaters[i] = NULL;
	}
	mFloaters.SetCount(0);

}



