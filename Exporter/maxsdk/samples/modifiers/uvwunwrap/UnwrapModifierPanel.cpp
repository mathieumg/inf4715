/*

Copyright [YEAR*] Autodesk, Inc.  All rights reserved. 

Use of this software is subject to the terms of the Autodesk license agreement provided at 
the time of installation or download, or which otherwise accompanies this software in either 
electronic or hard copy form. 

*/


#include "UnwrapModifierPanelUI.h"
#include "unwrap.h"
#include "modsres.h"

INT_PTR CALLBACK ModifierPanelRollupDialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
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


ModifierPanelUI::ModifierPanelUI()
{
	mMod = NULL;
}
ModifierPanelUI::~ModifierPanelUI()
{

}

void ModifierPanelUI::Init(UnwrapMod *mod)
{
	mMod = mod;
}


void ModifierPanelUI::SetDefaults(int index, HWND parentWindow)
{
	if (index == 200) //select1
	{		
		mMod->GetUIManager()->AppendToolBar(GetDlgItem(parentWindow,IDC_SELPARAM1_TOOLBAR),index,_M("__InternalSelect1"));
		ToolBarFrame* bar1 = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalSelect1"));

		mMod->AddActionToToolbar(bar1,ID_TV_VERTMODE);
		mMod->AddActionToToolbar(bar1,ID_TV_EDGEMODE);
		mMod->AddActionToToolbar(bar1,ID_TV_FACEMODE);
		mMod->AddActionToToolbar(bar1,ID_SEPARATOR1);
		mMod->AddActionToToolbar(bar1,ID_GEOM_ELEMENT);
	}

	if (index == 201) //select2
	{		
		mMod->GetUIManager()->AppendToolBar(GetDlgItem(parentWindow,IDC_SELPARAM2_TOOLBAR),index,_M("__InternalSelect2"));
		ToolBarFrame* bar1 = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalSelect2"));

		mMod->AddActionToToolbar(bar1,ID_GEOMEXPANDFACESEL);
		mMod->AddActionToToolbar(bar1,ID_GEOMCONTRACTFACESEL);
		mMod->AddActionToToolbar(bar1,ID_SEPARATOR1);
		mMod->AddActionToToolbar(bar1,ID_EDGELOOPSELECTION);
		mMod->AddActionToToolbar(bar1,ID_EDGERINGSELECTION);
	}

	if (index == 202) //select2
	{		
		mMod->GetUIManager()->AppendToolBar(GetDlgItem(parentWindow,IDC_SELPARAM3_TOOLBAR),index,_M("__InternalSelect3"));
		ToolBarFrame* bar1 = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalSelect3"));

		mMod->AddActionToToolbar(bar1,ID_IGNOREBACKFACE);
		mMod->AddActionToToolbar(bar1,ID_POINT_TO_POINT_SEL);
		mMod->AddActionToToolbar(bar1,ID_SEPARATORHALF);
		mMod->AddActionToToolbar(bar1,ID_PLANARMODE);
	}
	if (index == 203) //select3
	{		
		mMod->GetUIManager()->AppendToolBar(GetDlgItem(parentWindow,IDC_SELPARAM4_TOOLBAR),index,_M("__InternalSelect4"));
		ToolBarFrame* bar1 = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalSelect4"));

		mMod->AddActionToToolbar(bar1,ID_SELECTBY_MATID);
		mMod->AddActionToToolbar(bar1,ID_SELECTBY_SMGRP);
	}

	if (index == 210) //EditUVs1
	{		
		mMod->GetUIManager()->AppendToolBar(GetDlgItem(parentWindow,IDC_EDITPARAM1_TOOLBAR),index,_M("__InternalEditUVs1"));
		ToolBarFrame* bar1 = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalEditUVs1"));

				
//		mMod->AddActionToToolbar(bar1,ID_EDIT);

	}
	if (index == 211) //EditUVs2
	{		
		mMod->GetUIManager()->AppendToolBar(GetDlgItem(parentWindow,IDC_EDITPARAM2_TOOLBAR),index,_M("__InternalEditUVs2"));
		ToolBarFrame* bar1 = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalEditUVs2"));

//		mMod->AddActionToToolbar(bar1,ID_TWEAKUVW);

	}
	if (index == 212) //EditUVs3
	{		
		mMod->GetUIManager()->AppendToolBar(GetDlgItem(parentWindow,IDC_EDITPARAM3_TOOLBAR),index,_M("__InternalEditUVs3"));
		ToolBarFrame* bar1 = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalEditUVs3"));

		mMod->AddActionToToolbar(bar1,ID_SEPARATOR1);	
		mMod->AddActionToToolbar(bar1,ID_QMAP);		
		mMod->AddActionToToolbar(bar1,ID_QUICKMAP_DISPLAY);
		mMod->AddActionToToolbar(bar1,ID_QUICKMAP_ALIGN);

	}
	if (index == 230) //Peel1
	{		
		mMod->GetUIManager()->AppendToolBar(GetDlgItem(parentWindow,IDC_PEELPARAM1_TOOLBAR),index,_M("__InternalMPeel1"));
		ToolBarFrame* bar1 = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalMPeel1"));
		
		mMod->AddActionToToolbar(bar1,ID_SEPARATORHALF);
		mMod->AddActionToToolbar(bar1,ID_LSCM_SOLVE);
		mMod->AddActionToToolbar(bar1,ID_LSCM_INTERACTIVE);
		mMod->AddActionToToolbar(bar1,ID_LSCM_RESET);		
		mMod->AddActionToToolbar(bar1,ID_PELT_MAP);
	}
	if (index == 231) //Peel2
	{		
		mMod->GetUIManager()->AppendToolBar(GetDlgItem(parentWindow,IDC_PEELPARAM2_TOOLBAR),index,_M("__InternalMPeel2"));
		ToolBarFrame* bar1 = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalMPeel2"));

		mMod->AddActionToToolbar(bar1,ID_SEPARATORHALF);
		mMod->AddActionToToolbar(bar1,ID_PELT_EDITSEAMS);
		mMod->AddActionToToolbar(bar1,ID_PELT_POINTTOPOINTSEAMS);
		mMod->AddActionToToolbar(bar1,ID_PW_SELTOSEAM2);
		mMod->AddActionToToolbar(bar1,ID_PELT_EXPANDSELTOSEAM);
	}
	if (index == 240) //map
	{		
		mMod->GetUIManager()->AppendToolBar(GetDlgItem(parentWindow,IDC_MAPPARAM1_TOOLBAR),index,_M("__InternalProjection1"));
		ToolBarFrame* bar1 = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalProjection1"));
		
		mMod->AddActionToToolbar(bar1,ID_SEPARATORHALF);
		mMod->AddActionToToolbar(bar1,ID_PLANAR_MAP);
		mMod->AddActionToToolbar(bar1,ID_CYLINDRICAL_MAP);
		mMod->AddActionToToolbar(bar1,ID_SPHERICAL_MAP);
		mMod->AddActionToToolbar(bar1,ID_BOX_MAP);
		
	}
	if (index == 241) //map
	{		
		mMod->GetUIManager()->AppendToolBar(GetDlgItem(parentWindow,IDC_MAPPARAM2_TOOLBAR),index,_M("__InternalProjection2"));
		ToolBarFrame* bar1 = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalProjection2"));
		mMod->AddActionToToolbar(bar1,ID_MAPPING_ALIGNX);
		mMod->AddActionToToolbar(bar1,ID_MAPPING_ALIGNY);
		mMod->AddActionToToolbar(bar1,ID_MAPPING_ALIGNZ);
		mMod->AddActionToToolbar(bar1,ID_MAPPING_NORMALALIGN);
		mMod->AddActionToToolbar(bar1,ID_MAPPING_ALIGNTOVIEW);
	}
	if (index == 242) //map
	{		
		mMod->GetUIManager()->AppendToolBar(GetDlgItem(parentWindow,IDC_MAPPARAM3_TOOLBAR),index,_M("__InternalProjection3"));
		ToolBarFrame* bar1 = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalProjection3"));		
//		mMod->AddActionToToolbar(bar1,ID_MAPPING_CENTER);
//		mMod->AddActionToToolbar(bar1,ID_MAPPING_FIT);		
		mMod->AddActionToToolbar(bar1,ID_MAPPING_RESET);
		
	}
	if (index == 250) //wrap
	{		
		mMod->GetUIManager()->AppendToolBar(GetDlgItem(parentWindow,IDC_WRAPPARAM1_TOOLBAR),index,_M("__InternalWrap1"));
		ToolBarFrame* bar1 = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalWrap1"));
				
		mMod->AddActionToToolbar(bar1,ID_SEPARATOR1);
		mMod->AddActionToToolbar(bar1,ID_SPLINE_MAP);
		mMod->AddActionToToolbar(bar1,ID_SEPARATOR1);
		mMod->AddActionToToolbar(bar1,ID_UNFOLD_EDGE);
		
	}

}

void ModifierPanelUI::LoadInActions(int index)
{
	ToolBarFrame *bar = NULL;
	if (index == 200)
		bar = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalSelect1"));
	if (index == 201)
		bar = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalSelect2"));
	if (index == 202)
		bar = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalSelect3"));
	if (index == 203)
		bar = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalSelect4"));
	if (index == 210)
		bar = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalEditUVs1"));
	if (index == 211)
		bar = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalEditUVs2"));
	if (index == 212)
		bar = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalEditUVs3"));
	if (index == 230)
		bar = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalMPeel1"));
	if (index == 231)
		bar = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalMPeel2"));
	if (index == 240)
		bar = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalProjection1"));
	if (index == 241)
		bar = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalProjection2"));
	if (index == 242)
		bar = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalProjection3"));
	if (index == 250)
		bar = mMod->GetUIManager()->GetToolBarFrame(_M("__InternalWrap1"));

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




void ModifierPanelUI::Setup(HINSTANCE hInstance, HWND rollupHWND, const MCHAR *iniFile)
{
	//setup rollup
	mRollupHwnd = rollupHWND;

	

	IRollupWindow *irollup = GetIRollup(rollupHWND);
		
	if (mMod->GetUIManager()->LoadSingle(iniFile,200,mMod->hSelParams,GetDlgItem(mMod->hSelParams,IDC_SELPARAM1_TOOLBAR)) == 0)
		SetDefaults(200,mMod->hSelParams);
	else
		LoadInActions(200);

	if (mMod->GetUIManager()->LoadSingle(iniFile,201,mMod->hSelParams,GetDlgItem(mMod->hSelParams,IDC_SELPARAM2_TOOLBAR)) == 0)
		SetDefaults(201,mMod->hSelParams);
	else
		LoadInActions(201);

	if (mMod->GetUIManager()->LoadSingle(iniFile,202,mMod->hSelParams,GetDlgItem(mMod->hSelParams,IDC_SELPARAM3_TOOLBAR)) == 0)
		SetDefaults(202,mMod->hSelParams);
	else
		LoadInActions(202);

	if (mMod->GetUIManager()->LoadSingle(iniFile,203,mMod->hSelParams,GetDlgItem(mMod->hSelParams,IDC_SELPARAM4_TOOLBAR)) == 0)
		SetDefaults(203,mMod->hSelParams);
	else
		LoadInActions(203);


	if (mMod->GetUIManager()->LoadSingle(iniFile,210,mMod->hEditUVParams,GetDlgItem(mMod->hEditUVParams,IDC_EDITPARAM1_TOOLBAR)) == 0)
		SetDefaults(210,mMod->hEditUVParams);
	else
		LoadInActions(210);

	if (mMod->GetUIManager()->LoadSingle(iniFile,211,mMod->hEditUVParams,GetDlgItem(mMod->hEditUVParams,IDC_EDITPARAM2_TOOLBAR)) == 0)
		SetDefaults(211,mMod->hEditUVParams);
	else
		LoadInActions(211);

	if (mMod->GetUIManager()->LoadSingle(iniFile,212,mMod->hEditUVParams,GetDlgItem(mMod->hEditUVParams,IDC_EDITPARAM3_TOOLBAR)) == 0)
		SetDefaults(212,mMod->hEditUVParams);
	else
		LoadInActions(212);

	if (mMod->GetUIManager()->LoadSingle(iniFile,230,mMod->hPeelParams,GetDlgItem(mMod->hPeelParams,IDC_PEELPARAM1_TOOLBAR)) == 0)
		SetDefaults(230,mMod->hPeelParams);
	else
		LoadInActions(230);

	if (mMod->GetUIManager()->LoadSingle(iniFile,231,mMod->hPeelParams,GetDlgItem(mMod->hPeelParams,IDC_PEELPARAM2_TOOLBAR)) == 0)
		SetDefaults(231,mMod->hPeelParams);
	else
		LoadInActions(231);

	if (mMod->GetUIManager()->LoadSingle(iniFile,240,mMod->hMapParams,GetDlgItem(mMod->hMapParams,IDC_MAPPARAM1_TOOLBAR)) == 0)
		SetDefaults(240,mMod->hMapParams);
	else
		LoadInActions(240);

	if (mMod->GetUIManager()->LoadSingle(iniFile,241,mMod->hMapParams,GetDlgItem(mMod->hMapParams,IDC_MAPPARAM2_TOOLBAR)) == 0)
		SetDefaults(241,mMod->hMapParams);
	else
		LoadInActions(241);

	if (mMod->GetUIManager()->LoadSingle(iniFile,242,mMod->hMapParams,GetDlgItem(mMod->hMapParams,IDC_MAPPARAM3_TOOLBAR)) == 0)
		SetDefaults(242,mMod->hMapParams);
	else
		LoadInActions(242);

	if (mMod->GetUIManager()->LoadSingle(iniFile,250,mMod->hWrapParams,GetDlgItem(mMod->hWrapParams,IDC_WRAPPARAM1_TOOLBAR)) == 0)
		SetDefaults(250,mMod->hWrapParams);
	else
		LoadInActions(250);

	ReleaseIRollup(irollup);

	mMod->GetUIManager()->UpdateCheckButtons();

	//add the toolbars
}
void ModifierPanelUI::TearDown()
{
	
	IRollupWindow *irollup = GetIRollup(mRollupHwnd);

	mMod->GetUIManager()->Free(mMod->hSelParams);
	mMod->GetUIManager()->Free(mMod->hEditUVParams);
	mMod->GetUIManager()->Free(mMod->hPeelParams);
	mMod->GetUIManager()->Free(mMod->hMapParams);
	mMod->GetUIManager()->Free(mMod->hWrapParams);

	ReleaseIRollup(irollup);
}

void ModifierPanelUI::CreateDefaultToolBars()
{
	
}