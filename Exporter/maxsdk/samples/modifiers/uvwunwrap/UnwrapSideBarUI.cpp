/*

Copyright 2010 Autodesk, Inc.  All rights reserved. 

Use of this software is subject to the terms of the Autodesk license agreement provided at 
the time of installation or download, or which otherwise accompanies this software in either 
electronic or hard copy form. 

*/

#include "UnwrapSideBarUI.h"
#include "unwrap.h"
#include "modsres.h"

INT_PTR CALLBACK SideBar1RollupDialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{

	UnwrapMod *mod = DLGetWindowLongPtr<UnwrapMod*>(hWnd);
	if ( !mod && message != WM_INITDIALOG ) 
		return FALSE;

	BOOL processed = mod->GetUIManager()->MessageProc(hWnd,message,wParam,lParam);
	if (processed)
		return TRUE;

	switch ( message ) 
	{
	case WM_INITDIALOG:
		{
			DLSetWindowLongPtr(hWnd, lParam);
			mod = (UnwrapMod*)lParam;
		}

		return TRUE;

	case WM_DESTROY:
	case WM_MOUSEACTIVATE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MOUSEMOVE:            
		return FALSE;
/*
	case WM_COMMAND:
		switch (LOWORD(wParam)) 
		{

		}
*/
	}
	return FALSE;
}


SideBarUI::SideBarUI()
{
	mMod = NULL;
}
SideBarUI::~SideBarUI()
{

}

void SideBarUI::Init(UnwrapMod *mod)
{
	mMod = mod;
}


void SideBarUI::SetDefaults(int index, HWND parentWindow)
{
	if (index == 100) //quick transform
	{		
		mMod->GetUIManager()->AppendToolBar(GetDlgItem(parentWindow,IDC_S1_1_TOOLBAR),index,_M("__InternalQuickTransform1"));
		ToolBarFrame* bar1 = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalQuickTransform1"));

		
		mMod->AddActionToToolbar(bar1,ID_ALIGNH_BUTTONS);//ID_ALIGNBYPIVOTH);//ID_ALIGN_SELECTION_HORIZONTAL);
		mMod->AddActionToToolbar(bar1,ID_TOOL_ALIGN_LINEAR);
		mMod->AddActionToToolbar(bar1,ID_ROTATE_90_CCW);		
	}
	else if (index == 101)
	{
		mMod->GetUIManager()->AppendToolBar(GetDlgItem(parentWindow,IDC_S1_2_TOOLBAR),index,_M("__InternalQuickTransform2"));

		ToolBarFrame* bar2 = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalQuickTransform2"));

		mMod->AddActionToToolbar(bar2,ID_ALIGNV_BUTTONS);//ID_ALIGNBYPIVOTV);//ID_ALIGN_SELECTION_VERTICAL);
		mMod->AddActionToToolbar(bar2,ID_TOOL_ALIGN_ELEMENT);
		mMod->AddActionToToolbar(bar2,ID_ROTATE_90_CW);				
		
		
	}
	else if (index == 102)
	{
		mMod->GetUIManager()->AppendToolBar(GetDlgItem(parentWindow,IDC_S1_3_TOOLBAR),index,_M("__InternalQuickTransform3"));

		ToolBarFrame* bar1 = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalQuickTransform3"));

		mMod->AddActionToToolbar(bar1,ID_FREEFORMSNAP);
	}
	else if (index == 103)
	{
		mMod->GetUIManager()->AppendToolBar(GetDlgItem(parentWindow,IDC_S1_4_TOOLBAR),index,_M("__InternalQuickTransform4"));

		ToolBarFrame* bar1 = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalQuickTransform4"));

		mMod->AddActionToToolbar(bar1,ID_TOOL_SPACE_HORIZONTAL);
	}
	else if (index == 104)
	{
		mMod->GetUIManager()->AppendToolBar(GetDlgItem(parentWindow,IDC_S1_5_TOOLBAR),index,_M("__InternalQuickTransform5"));

		ToolBarFrame* bar1 = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalQuickTransform5"));

		mMod->AddActionToToolbar(bar1,ID_TOOL_SPACE_VERTICAL);
		
		
	}
	else if (index == 110)
	{
		mMod->GetUIManager()->AppendToolBar(GetDlgItem(parentWindow,IDC_S2_1_TOOLBAR),index,_M("__InternalReshapeElements1"));

		ToolBarFrame* bar2 = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalReshapeElements1"));

		
		mMod->AddActionToToolbar(bar2,ID_SEPARATOR1);
		mMod->AddActionToToolbar(bar2,ID_STRAIGHTEN);
		mMod->AddActionToToolbar(bar2,ID_RELAXONECLICK);
		mMod->AddActionToToolbar(bar2,ID_RELAXBUTTONS);
//		mMod->AddActionToToolbar(bar2,ID_PASTESYMMETRICAL);
	}
	else if (index == 120)
	{
		mMod->GetUIManager()->AppendToolBar(GetDlgItem(parentWindow,IDC_S3_1_TOOLBAR),index,_M("__InternalStitch1"));

		ToolBarFrame* bar2 = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalStitch1"));

		mMod->AddActionToToolbar(bar2,ID_SEPARATORHALF);
		mMod->AddActionToToolbar(bar2,ID_STITCHTARGET);
		mMod->AddActionToToolbar(bar2,ID_STITCHAVERAGE);
		mMod->AddActionToToolbar(bar2,ID_STITCHSOURCE);
		mMod->AddActionToToolbar(bar2,ID_STITCHBUTTONS);
		
	}

	else if (index == 130)
	{
		mMod->GetUIManager()->AppendToolBar(GetDlgItem(parentWindow,IDC_S4_1_TOOLBAR),index,_M("__InternalExplode1"));

		ToolBarFrame* bar2 = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalExplode1"));

		mMod->AddActionToToolbar(bar2,ID_BREAKBYSUBOBJECT);
		
		mMod->AddActionToToolbar(bar2,ID_FLATTENMAP);
		mMod->AddActionToToolbar(bar2,ID_FLATTEN_BYSMOOTHINGGROUP);
		mMod->AddActionToToolbar(bar2,ID_FLATTEN_BYMATID);		
		mMod->AddActionToToolbar(bar2,ID_FLATTENBUTTONS);
	}
	else if (index == 131)
	{
		mMod->GetUIManager()->AppendToolBar(GetDlgItem(parentWindow,IDC_S4_2_TOOLBAR),index,_M("__InternalExplode2"));

		ToolBarFrame* bar2 = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalExplode2"));

		mMod->AddActionToToolbar(bar2,ID_WELD);
		mMod->AddActionToToolbar(bar2,ID_WELD_SELECTED);
		mMod->AddActionToToolbar(bar2,ID_WELDSELECTEDSHARED);
		mMod->AddActionToToolbar(bar2,ID_WELDALLSHARED);
//		mMod->AddActionToToolbar(bar2,ID_TOOL_WELDDROPDOWN);
	}
	else if (index == 140)
	{
		mMod->GetUIManager()->AppendToolBar(GetDlgItem(parentWindow,IDC_S5_1_TOOLBAR),index,_M("__InternalPeel1"));

		ToolBarFrame* bar2 = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalPeel1"));
		
		mMod->AddActionToToolbar(bar2,ID_SEPARATOR1);
		mMod->AddActionToToolbar(bar2,ID_LSCM_SOLVE);
		mMod->AddActionToToolbar(bar2,ID_LSCM_INTERACTIVE);
		mMod->AddActionToToolbar(bar2,ID_LSCM_RESET);
	}
	else if (index == 141)
	{
		mMod->GetUIManager()->AppendToolBar(GetDlgItem(parentWindow,IDC_S5_2_TOOLBAR),index,_M("__InternalPeel2"));

		ToolBarFrame* bar2 = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalPeel2"));

		mMod->AddActionToToolbar(bar2,ID_PIN);
		mMod->AddActionToToolbar(bar2,ID_UNPIN);
		mMod->AddActionToToolbar(bar2,ID_TOOL_AUTOPIN);
		mMod->AddActionToToolbar(bar2,ID_SEPARATORBAR);
		mMod->AddActionToToolbar(bar2,ID_TOOL_FILTERPIN);
		
	}
	else if (index == 150)
	{
		mMod->GetUIManager()->AppendToolBar(GetDlgItem(parentWindow,IDC_S6_1_TOOLBAR),index,_M("__InternalArrangeElements1"));

		ToolBarFrame* bar2 = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalArrangeElements1"));

//		mMod->AddActionToToolbar(bar2,ID_RESCALECLUSTER);
//		mMod->AddActionToToolbar(bar2,ID_PACK_TIGHT);
//		mMod->AddActionToToolbar(bar2,ID_PACK_FULL);

		mMod->AddActionToToolbar(bar2,ID_PACKBUTTONS);
	}
	else if (index == 151)
	{
		mMod->GetUIManager()->AppendToolBar(GetDlgItem(parentWindow,IDC_S6_2_TOOLBAR),index,_M("__InternalArrangeElements2"));

		ToolBarFrame* bar2 = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalArrangeElements2"));		
		mMod->AddActionToToolbar(bar2,ID_RESCALECLUSTER);
//		mMod->AddActionToToolbar(bar2,ID_PACK_RESCALE);
//		mMod->AddActionToToolbar(bar2,ID_PACK_ROTATE);
	}
	else if (index == 152)
	{
		mMod->GetUIManager()->AppendToolBar(GetDlgItem(parentWindow,IDC_S6_3_TOOLBAR),index,_M("__InternalArrangeElements3"));

		ToolBarFrame* bar2 = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalArrangeElements3"));	
		mMod->AddActionToToolbar(bar2,ID_PACK_TIGHT);
		

//		mMod->AddActionToToolbar(bar2,ID_PACK_PADDINGTEXT);
		
	}

	else if (index == 153)
	{
		mMod->GetUIManager()->AppendToolBar(GetDlgItem(parentWindow,IDC_S6_4_TOOLBAR),index,_M("__InternalArrangeElements4"));

		ToolBarFrame* bar2 = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalArrangeElements4"));		
		mMod->AddActionToToolbar(bar2,ID_PACK_FULL);
	}
	else if (index == 154)
	{
		mMod->GetUIManager()->AppendToolBar(GetDlgItem(parentWindow,IDC_S6_5_TOOLBAR),index,_M("__InternalArrangeElements5"));

		ToolBarFrame* bar2 = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalArrangeElements5"));		
		mMod->AddActionToToolbar(bar2,ID_PACK_RESCALE);
	}
	else if (index == 155)
	{
		mMod->GetUIManager()->AppendToolBar(GetDlgItem(parentWindow,IDC_S6_6_TOOLBAR),index,_M("__InternalArrangeElements6"));

		ToolBarFrame* bar2 = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalArrangeElements6"));		
		mMod->AddActionToToolbar(bar2,ID_PACK_ROTATE);
	}
	else if (index == 156)
	{
		mMod->GetUIManager()->AppendToolBar(GetDlgItem(parentWindow,IDC_S6_7_TOOLBAR),index,_M("__InternalArrangeElements7"));

		ToolBarFrame* bar2 = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalArrangeElements7"));		
		mMod->AddActionToToolbar(bar2,ID_PACK_PADDINGEDIT);
	}

	else if (index == 160)
	{
		mMod->GetUIManager()->AppendToolBar(GetDlgItem(parentWindow,IDC_S7_1_TOOLBAR),index,_M("__InternalElementProperties1"));

		ToolBarFrame* bar2 = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalElementProperties1"));

		mMod->AddActionToToolbar(bar2,ID_SEPARATORHALF);
		mMod->AddActionToToolbar(bar2,ID_GROUP);
		mMod->AddActionToToolbar(bar2,ID_UNGROUP);
		mMod->AddActionToToolbar(bar2,ID_GROUPSELECT);
	}
	else if (index == 161)
	{
		mMod->GetUIManager()->AppendToolBar(GetDlgItem(parentWindow,IDC_S7_2_TOOLBAR),index,_M("__InternalElementProperties2"));

		ToolBarFrame* bar2 = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalElementProperties2"));
		mMod->AddActionToToolbar(bar2,ID_GROUPSETDENSITY_EDIT);
	}


}

void SideBarUI::LoadInActions(int index)
{
	ToolBarFrame *bar = NULL;
	if (index == 100)
		bar = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalQuickTransform1"));
	else if (index == 101)
		bar = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalQuickTransform2"));
	else if (index == 102)
		bar = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalQuickTransform3"));
	else if (index == 103)
		bar = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalQuickTransform4"));
	else if (index == 104)
		bar = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalQuickTransform5"));

	else if (index == 110)
		bar = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalReshapeElements1"));
	else if (index == 120)
		bar = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalStitch1"));
	else if (index == 130)
		bar = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalExplode1"));
	else if (index == 131)
		bar = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalExplode2"));
	else if (index == 140)
		bar = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalPeel1"));
	else if (index == 141)
		bar = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalPeel2"));
	else if (index == 150)
		bar = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalArrangeElements1"));
	else if (index == 151)
		bar = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalArrangeElements2"));
	else if (index == 152)
		bar = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalArrangeElements3"));
	else if (index == 153)
		bar = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalArrangeElements4"));
	else if (index == 153)
		bar = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalArrangeElements4"));
	else if (index == 154)
		bar = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalArrangeElements5"));
	else if (index == 155)
		bar = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalArrangeElements6"));
	else if (index == 156)
		bar = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalArrangeElements7"));

	else if (index == 160)
		bar = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalElementProperties1"));
	else if (index == 161)
		bar = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalElementProperties2"));
	if (bar)
	{
		Tab<int> ids;
		ids = bar->LoadIDs();
		for (int j = 0; j < ids.Count(); j++)
		{
			int id = ids[j];
			mMod->AddActionToToolbar(bar,id);
		}
		bar->LoadIDsClear();
	}
}




void SideBarUI::Setup(HINSTANCE hInstance, HWND rollupHWND, const MCHAR *iniFile)
{
	//setup rollup
	mRollupHwnd = rollupHWND;

	GetCOREInterface()->RegisterDlgWnd ( mRollupHwnd );

	IRollupWindow *irollup = GetIRollup(rollupHWND);

	irollup->AppendRollup(hInstance,MAKEINTRESOURCE(IDD_SIDEBAR_ROLLUP1),SideBar1RollupDialogProc, GetString(IDS_QUICKTRANSFORM),(LPARAM)mMod );
	irollup->AppendRollup(hInstance,MAKEINTRESOURCE(IDD_SIDEBAR_ROLLUP2),SideBar1RollupDialogProc, GetString(IDS_RESHAPEELEMENTS),(LPARAM)mMod );
	irollup->AppendRollup(hInstance,MAKEINTRESOURCE(IDD_SIDEBAR_ROLLUP3),SideBar1RollupDialogProc, GetString(IDS_STITCH),(LPARAM)mMod );
	irollup->AppendRollup(hInstance,MAKEINTRESOURCE(IDD_SIDEBAR_ROLLUP4),SideBar1RollupDialogProc, GetString(IDS_EXPLODE),(LPARAM)mMod );
	irollup->AppendRollup(hInstance,MAKEINTRESOURCE(IDD_SIDEBAR_ROLLUP5),SideBar1RollupDialogProc, GetString(IDS_PEEL),(LPARAM)mMod );
	irollup->AppendRollup(hInstance,MAKEINTRESOURCE(IDD_SIDEBAR_ROLLUP6),SideBar1RollupDialogProc, GetString(IDS_ARRANGEELEMENTS),(LPARAM)mMod );
	irollup->AppendRollup(hInstance,MAKEINTRESOURCE(IDD_SIDEBAR_ROLLUP7),SideBar1RollupDialogProc, GetString(IDS_ELEMENTPROPERTIES),(LPARAM)mMod );


	for (int i = 0; i < 7; i++)
		irollup->Show(i);

	//quick transform
	if (mMod->GetUIManager()->LoadSingle(iniFile,100,irollup->GetPanelDlg(0),GetDlgItem(irollup->GetPanelDlg(0),IDC_S1_1_TOOLBAR)) == 0)
		SetDefaults(100,irollup->GetPanelDlg(0));
	else
		LoadInActions(100);
	if (mMod->GetUIManager()->LoadSingle(iniFile,101,irollup->GetPanelDlg(0),GetDlgItem(irollup->GetPanelDlg(0),IDC_S1_2_TOOLBAR)) == 0)
		SetDefaults(101,irollup->GetPanelDlg(0));
	else
		LoadInActions(101);
	if (mMod->GetUIManager()->LoadSingle(iniFile,102,irollup->GetPanelDlg(0),GetDlgItem(irollup->GetPanelDlg(0),IDC_S1_3_TOOLBAR)) == 0)
		SetDefaults(102,irollup->GetPanelDlg(0));
	else
		LoadInActions(102);
	if (mMod->GetUIManager()->LoadSingle(iniFile,103,irollup->GetPanelDlg(0),GetDlgItem(irollup->GetPanelDlg(0),IDC_S1_4_TOOLBAR)) == 0)
		SetDefaults(103,irollup->GetPanelDlg(0));
	else
		LoadInActions(103);
	if (mMod->GetUIManager()->LoadSingle(iniFile,104,irollup->GetPanelDlg(0),GetDlgItem(irollup->GetPanelDlg(0),IDC_S1_5_TOOLBAR)) == 0)
		SetDefaults(104,irollup->GetPanelDlg(0));
	else
		LoadInActions(104);


	if (mMod->GetUIManager()->LoadSingle(iniFile,110,irollup->GetPanelDlg(1),GetDlgItem(irollup->GetPanelDlg(1),IDC_S2_1_TOOLBAR)) == 0)
		SetDefaults(110,irollup->GetPanelDlg(1));
	else
		LoadInActions(110);

	if (mMod->GetUIManager()->LoadSingle(iniFile,120,irollup->GetPanelDlg(2),GetDlgItem(irollup->GetPanelDlg(2),IDC_S3_1_TOOLBAR)) == 0)
		SetDefaults(120,irollup->GetPanelDlg(2));
	else
		LoadInActions(120);

	if (mMod->GetUIManager()->LoadSingle(iniFile,130,irollup->GetPanelDlg(3),GetDlgItem(irollup->GetPanelDlg(3),IDC_S4_1_TOOLBAR)) == 0)
		SetDefaults(130,irollup->GetPanelDlg(3));
	else
		LoadInActions(130);

	if (mMod->GetUIManager()->LoadSingle(iniFile,131,irollup->GetPanelDlg(3),GetDlgItem(irollup->GetPanelDlg(3),IDC_S4_2_TOOLBAR)) == 0)
		SetDefaults(131,irollup->GetPanelDlg(3));
	else
		LoadInActions(131);

	if (mMod->GetUIManager()->LoadSingle(iniFile,140,irollup->GetPanelDlg(4),GetDlgItem(irollup->GetPanelDlg(4),IDC_S5_1_TOOLBAR)) == 0)
		SetDefaults(140,irollup->GetPanelDlg(4));
	else
		LoadInActions(140);
	if (mMod->GetUIManager()->LoadSingle(iniFile,141,irollup->GetPanelDlg(4),GetDlgItem(irollup->GetPanelDlg(4),IDC_S5_2_TOOLBAR)) == 0)
		SetDefaults(141,irollup->GetPanelDlg(4));
	else
		LoadInActions(141);

	if (mMod->GetUIManager()->LoadSingle(iniFile,150,irollup->GetPanelDlg(5),GetDlgItem(irollup->GetPanelDlg(5),IDC_S6_1_TOOLBAR)) == 0)
		SetDefaults(150,irollup->GetPanelDlg(5));
	else
		LoadInActions(150);
	if (mMod->GetUIManager()->LoadSingle(iniFile,151,irollup->GetPanelDlg(5),GetDlgItem(irollup->GetPanelDlg(5),IDC_S6_2_TOOLBAR)) == 0)
		SetDefaults(151,irollup->GetPanelDlg(5));
	else
		LoadInActions(151);
	if (mMod->GetUIManager()->LoadSingle(iniFile,152,irollup->GetPanelDlg(5),GetDlgItem(irollup->GetPanelDlg(5),IDC_S6_3_TOOLBAR)) == 0)
		SetDefaults(152,irollup->GetPanelDlg(5));
	else
		LoadInActions(152);
	if (mMod->GetUIManager()->LoadSingle(iniFile,153,irollup->GetPanelDlg(5),GetDlgItem(irollup->GetPanelDlg(5),IDC_S6_4_TOOLBAR)) == 0)
		SetDefaults(153,irollup->GetPanelDlg(5));
	else
		LoadInActions(153);
	if (mMod->GetUIManager()->LoadSingle(iniFile,154,irollup->GetPanelDlg(5),GetDlgItem(irollup->GetPanelDlg(5),IDC_S6_5_TOOLBAR)) == 0)
		SetDefaults(154,irollup->GetPanelDlg(5));
	else
		LoadInActions(154);
	if (mMod->GetUIManager()->LoadSingle(iniFile,155,irollup->GetPanelDlg(5),GetDlgItem(irollup->GetPanelDlg(5),IDC_S6_6_TOOLBAR)) == 0)
		SetDefaults(155,irollup->GetPanelDlg(5));
	else
		LoadInActions(155);
	if (mMod->GetUIManager()->LoadSingle(iniFile,156,irollup->GetPanelDlg(5),GetDlgItem(irollup->GetPanelDlg(5),IDC_S6_7_TOOLBAR)) == 0)
		SetDefaults(156,irollup->GetPanelDlg(5));
	else
		LoadInActions(156);


	if (mMod->GetUIManager()->LoadSingle(iniFile,160,irollup->GetPanelDlg(6),GetDlgItem(irollup->GetPanelDlg(6),IDC_S7_1_TOOLBAR)) == 0)
		SetDefaults(160,irollup->GetPanelDlg(6));
	else
		LoadInActions(160);
	if (mMod->GetUIManager()->LoadSingle(iniFile,161,irollup->GetPanelDlg(6),GetDlgItem(irollup->GetPanelDlg(6),IDC_S7_2_TOOLBAR)) == 0)
		SetDefaults(161,irollup->GetPanelDlg(6));
	else
		LoadInActions(161);

	ReleaseIRollup(irollup);

	//add the toolbars
}
void SideBarUI::TearDown()
{
	mMod->mPackTempPadding = mMod->GetUIManager()->GetSpinFValue(ID_PACK_PADDINGSPINNER);
	if (mMod->GetUIManager()->IsChecked(ID_PACK_RESCALE))
		mMod->mPackTempRescale = TRUE;
	else
		mMod->mPackTempRescale = FALSE;
	if (mMod->GetUIManager()->IsChecked(ID_PACK_ROTATE))
		mMod->mPackTempRotate = TRUE;
	else
		mMod->mPackTempRotate = FALSE;


	GetCOREInterface()->UnRegisterDlgWnd ( mRollupHwnd );
	IRollupWindow *irollup = GetIRollup(mRollupHwnd);

	for (int i = 0; i < 7; i++)
	{
		//load defaults
		HWND hwnd = irollup->GetPanelDlg(i);
		mMod->GetUIManager()->Free(hwnd);
	}

	ReleaseIRollup(irollup);
}

void SideBarUI::CreateDefaultToolBars()
{
	
}

HWND SideBarUI::GetRollupHWND(int rollupIndex)
{


	IRollupWindow *irollup = GetIRollup(mRollupHwnd);
	HWND hWnd =  irollup->GetPanelDlg(rollupIndex);
	ReleaseIRollup(irollup);
	return hWnd;
		

}