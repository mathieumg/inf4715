/*

Copyright 2010 Autodesk, Inc.  All rights reserved. 

Use of this software is subject to the terms of the Autodesk license agreement provided at 
the time of installation or download, or which otherwise accompanies this software in either 
electronic or hard copy form. 

*/

#include "unwrap.h"
#include "modsres.h"

BOOL UnwrapMod::AddActionToToolbar(ToolBarFrame *toolBar, int id)
{
	int buttonWidth = 28;
	int buttonHeight = 28;

	int buttonImageWidth = 24;
	int buttonImageHeight = 24;


	int vertexOffset = 40;
	int optionsOffset = 68;
	int viewOffset = 74;
	switch (id)
	{
//SEPARATORS
	case ID_SEPARATOR4:
		{
			toolBar->GetToolBar()->AddTool(ToolOtherItem(_M("static"), 4*mToolBarHeight,	mToolBarHeight, id,WS_CHILD|WS_VISIBLE, 4, _M(" "), 0));
			break;
		}
	case ID_SEPARATOR3:
		{
			toolBar->GetToolBar()->AddTool(ToolOtherItem(_M("static"), 3*mToolBarHeight,	mToolBarHeight, id,WS_CHILD|WS_VISIBLE, 4, _M(" "), 0));
			break;
		}
	case ID_SEPARATOR2:
		{
			toolBar->GetToolBar()->AddTool(ToolOtherItem(_M("static"), 2*mToolBarHeight,	mToolBarHeight, id,WS_CHILD|WS_VISIBLE, 4, _M(" "), 0));
			break;
		}
	case ID_SEPARATOR1:
		{
			toolBar->GetToolBar()->AddTool(ToolOtherItem(_M("static"), 1*mToolBarHeight,	mToolBarHeight, id,WS_CHILD|WS_VISIBLE, 4, _M(" "), 0));
			break;
		}


	case ID_SEPARATORHALF:
		{
			toolBar->GetToolBar()->AddTool(ToolOtherItem(_M("static"), mToolBarHeight/2,mToolBarHeight, id,WS_CHILD|WS_VISIBLE, 4, _M(" "), 0));
			break;
		}

	case ID_SEPARATORBAR:
		{
			toolBar->GetToolBar()->AddTool(ToolOtherItem(_M("static"), 10,	mToolBarHeight, id,WS_CHILD|WS_VISIBLE, 4, _M(" |"), 0));
			break;
		}

//TVSELECTIONS
	case ID_TV_VERTMODE:
		{
			static MaxBmpFileIcon vertOut(_M("UVWUnwrapSelection"),1);
			static MaxBmpFileIcon vertIn(_M("UVWUnwrapSelection"),6);	
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &vertOut, &vertIn, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
								GetString(IDS_TOOLTIP_TV_VERT_MODE),
								NULL,0,0);
			break;
		}
	case ID_TV_EDGEMODE:
		{
			static MaxBmpFileIcon vertOut(_M("UVWUnwrapSelection"),2);
			static MaxBmpFileIcon vertIn(_M("UVWUnwrapSelection"),7);	
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &vertOut, &vertIn, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TV_EDGE_MODE),
				NULL,0,0);
			break;
		}
	case ID_TV_FACEMODE:
		{
			static MaxBmpFileIcon vertOut(_M("UVWUnwrapSelection"),4);
			static MaxBmpFileIcon vertIn(_M("UVWUnwrapSelection"),9);	
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &vertOut, &vertIn, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TV_FACE_MODE),
				NULL,0,0);
			break;
		}
	case ID_TV_ELEMENTMODE:
		{
			static MaxBmpFileIcon iconOut(_M("UVWUnwrapSelection"),24);
			static MaxBmpFileIcon iconIn(_M("UVWUnwrapSelection"),25);
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &iconIn, &iconOut, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TV_ELEMENT_MODE),
				NULL,0,0);
			break;
		}
	case ID_TV_INCSEL:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapSelection"),13);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapSelection"),13);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TV_INCSEL),
				NULL,0,0);
			break;
		}
	case ID_TV_DECSEL:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapSelection"),14);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapSelection"),14);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TV_DECSEL),
				NULL,0,0);
			break;
		}
	case ID_TV_LOOP:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapSelection"),15);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapSelection"),15);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TV_LOOP),
				NULL,0,0);

			break;
		}
	case ID_TV_LOOPGROW:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapSelection"),16);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapSelection"),16);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TV_LOOP_GROW),
				NULL,0,0);

			break;
		}
	case ID_TV_LOOPSHRINK:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapSelection"),17);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapSelection"),17);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TV_LOOP_SHRINK),
				NULL,0,0);
			break;
		}

	case ID_TV_RING:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapSelection"),18);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapSelection"),18);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TV_RING),
				NULL,0,0);
			break;
			break;
		}

	case ID_TV_RINGGROW:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapSelection"),19);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapSelection"),19);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TV_RING_GROW),
				NULL,0,0);
			break;
		}

	case ID_TV_RINGSHRINK:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapSelection"),20);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapSelection"),20);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TV_RING_SHRINK),
				NULL,0,0);
			break;
		}

	case ID_TV_PAINTSELECTMODE:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapSelection"),21);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapSelection"),21);
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TV_PAINTSELECTMODE),
				NULL,0,0);
			break;
		}
	case ID_TV_PAINTSELECTINC:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapSelection"),22);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapSelection"),22);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TV_PAINTSELECTINC),
				NULL,0,0);
			break;
		}
	case ID_TV_PAINTSELECTDEC:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapSelection"),23);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapSelection"),23);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TV_PAINTSELECTDEC),
				NULL,0,0);
			break;
		}


//SOFT SELECTION
	case ID_SOFTSELECTION:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapSoftSelection"),1);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapSoftSelection"),1);
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_SOFTSELECTION),
				NULL,0,0);
			break;
		}
	case ID_SOFTSELECTIONSTR:
	case ID_SOFTSELECTIONSTR_TEXT:					//this ID comes when we load since this is item is 9 elements
		{
			ICustToolbar* itoolBar = toolBar->GetToolBar();
			itoolBar->AddTool(ToolOtherItem(_M("static"), 4,	mToolBarHeight, ID_SOFTSELECTIONSTR_TEXT,WS_CHILD|WS_VISIBLE, 4, GetString(IDS_EMPTY), 0));
			itoolBar->AddTool(ToolOtherItem(CUSTEDITWINDOWCLASS, 38,	16, ID_SOFTSELECTIONSTR_EDIT,WS_CHILD|WS_VISIBLE, 4, NULL, 0));
			itoolBar->AddTool(ToolOtherItem(SPINNERWINDOWCLASS, 18,	16, ID_SOFTSELECTIONSTR_SPINNER,WS_CHILD|WS_VISIBLE, 4, NULL, 0));
			ISpinnerControl* uSpin = GetISpinner(itoolBar->GetItemHwnd(ID_SOFTSELECTIONSTR_SPINNER));
			uSpin->LinkToEdit(itoolBar->GetItemHwnd(ID_SOFTSELECTIONSTR_EDIT),EDITTYPE_FLOAT);
			uSpin->SetLimits(-9999999, 9999999, FALSE);
			uSpin->SetAutoScale();
			ReleaseISpinner(uSpin);

			break;
		}

	case ID_FALLOFF_SPACE:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapSoftSelection"),11);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapSoftSelection"),11);
			static MaxBmpFileIcon iIconF1(_M("UVWUnwrapSoftSelection"),12);
			static MaxBmpFileIcon iIconF2(_M("UVWUnwrapSoftSelection"),12);
			FlyOffData fdata2b[] = {{-1, -1, -1, -1, &outIcon, &outIcon},
									{-1, -1, -1, -1, &iIconF1, &iIconF1}	};
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_FALLOFF_SPACE),
				fdata2b,2,falloffSpace);
			break;
		}
	case ID_FALLOFF:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapSoftSelection"),3);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapSoftSelection"),4);

			static MaxBmpFileIcon iIconF1(_M("UVWUnwrapSoftSelection"),3);
			static MaxBmpFileIcon iIconF2(_M("UVWUnwrapSoftSelection"),4);

			static MaxBmpFileIcon iIconF3(_M("UVWUnwrapSoftSelection"),5);
			static MaxBmpFileIcon iIconF4(_M("UVWUnwrapSoftSelection"),6);

			static MaxBmpFileIcon iIconF5(_M("UVWUnwrapSoftSelection"),7);
			static MaxBmpFileIcon iIconF6(_M("UVWUnwrapSoftSelection"),8);

			static MaxBmpFileIcon iIconF7(_M("UVWUnwrapSoftSelection"),9);
			static MaxBmpFileIcon iIconF8(_M("UVWUnwrapSoftSelection"),10);


			FlyOffData fdata2a[] = 	{	{-1, -1, -1, -1,&iIconF1,&iIconF1},
										{-1, -1, -1, -1,&iIconF3,&iIconF3},
										{-1, -1, -1, -1,&iIconF5,&iIconF5},
										{-1, -1, -1, -1,&iIconF7,&iIconF7} };
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_FALLOFF),
				fdata2a,4,0);
			break;
		}


	case ID_LIMITSOFTSEL:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapSoftSelection"),2);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapSoftSelection"),13);
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_LIMITSOFTSEL),
				NULL,0,0);
			break;
		}
	case ID_SOFTSELECTIONLIMIT:
	case ID_SOFTSELECTIONLIMIT_TEXT:					//this ID comes when we load since this is item is 9 elements
		{
			ICustToolbar* itoolBar = toolBar->GetToolBar();
			itoolBar->AddTool(ToolOtherItem(_M("static"), 4,	mToolBarHeight, ID_SOFTSELECTIONLIMIT_TEXT,WS_CHILD|WS_VISIBLE, 4, GetString(IDS_EMPTY), 0));
			itoolBar->AddTool(ToolOtherItem(CUSTEDITWINDOWCLASS, 22,	16, ID_SOFTSELECTIONLIMIT_EDIT,WS_CHILD|WS_VISIBLE, 4, NULL, 0));
			itoolBar->AddTool(ToolOtherItem(SPINNERWINDOWCLASS, 18,	16, ID_SOFTSELECTIONLIMIT_SPINNER,WS_CHILD|WS_VISIBLE, 4, NULL, 0));
			ISpinnerControl* uSpin = GetISpinner(itoolBar->GetItemHwnd(ID_SOFTSELECTIONLIMIT_SPINNER));
			uSpin->LinkToEdit(itoolBar->GetItemHwnd(ID_SOFTSELECTIONLIMIT_EDIT),EDITTYPE_INT);
			uSpin->SetLimits(-9999999, 9999999, FALSE);
			uSpin->SetAutoScale();
			ReleaseISpinner(uSpin);

			break;
		}
//GEOM SELECTION
	case ID_GEOM_ELEMENT:
		{
			static MaxBmpFileIcon iconOut(_M("UVWUnwrapSelection"),24);
			static MaxBmpFileIcon iconIn(_M("UVWUnwrapSelection"),25);
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &iconIn, &iconOut, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_GEOM_ELEMENT_MODE),
				NULL,0,0);
			break;
		}
	case ID_GEOMEXPANDFACESEL:
		{
			static MaxBmpFileIcon iconOut(_M("UVWUnwrapSelection"),13);
			static MaxBmpFileIcon iconIn(_M("UVWUnwrapSelection"),13);
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &iconIn, &iconOut, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_GEOM_EXPANDSEL),
				NULL,0,0);
			break;
		}
	case ID_GEOMCONTRACTFACESEL:
		{
			static MaxBmpFileIcon iconOut(_M("UVWUnwrapSelection"),14);
			static MaxBmpFileIcon iconIn(_M("UVWUnwrapSelection"),14);
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &iconIn, &iconOut, buttonImageWidth, buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_GEOM_CONTRACTSEL),
				NULL,0,0);
			break;
		}
	case ID_EDGELOOPSELECTION:
		{
			static MaxBmpFileIcon iconOut(_M("UVWUnwrapSelection"),15);
			static MaxBmpFileIcon iconIn(_M("UVWUnwrapSelection"),15);
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &iconIn, &iconOut, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_GEOM_LOOP),
				NULL,0,0);
			break;
		}
	case ID_EDGERINGSELECTION:
		{
			static MaxBmpFileIcon iconOut(_M("UVWUnwrapSelection"),18);
			static MaxBmpFileIcon iconIn(_M("UVWUnwrapSelection"),18);
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &iconIn, &iconOut, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_GEOM_RING),
				NULL,0,0);
			break;
		}
	case ID_IGNOREBACKFACE:
		{
			static MaxBmpFileIcon iconOut(_M("UVWUnwrapSelection"),12);
			static MaxBmpFileIcon iconIn(_M("UVWUnwrapSelection"),11);
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &iconIn, &iconOut, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_GEOM_IGNOREBACKFACE),
				NULL,0,0);
			break;
		}
	case ID_POINT_TO_POINT_SEL:
		{
			static MaxBmpFileIcon iconOut(_M("UVWUnwrapSelection"),26);
			static MaxBmpFileIcon iconIn(_M("UVWUnwrapSelection"),26);
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &iconIn, &iconOut, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_GEOM_POINT_TO_POINT_SEL),
				NULL,0,0);
			break;
		}
	case ID_PLANARMODE:
		{
			static MaxBmpFileIcon iconOut(_M("UVWUnwrapSelection"),29);
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &iconOut, &iconOut, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_GEOM_PLANARMODE),
				NULL,0,0);

			ICustToolbar* itoolBar = toolBar->GetToolBar();
			itoolBar->AddTool(ToolOtherItem(CUSTEDITWINDOWCLASS, 30,	16, ID_PLANAREDIT,WS_CHILD|WS_VISIBLE, 4, NULL, 0));
			itoolBar->AddTool(ToolOtherItem(SPINNERWINDOWCLASS, 18,	16, ID_PLANARSPIN,WS_CHILD|WS_VISIBLE, 4, NULL, 0));
			ISpinnerControl* uSpin = GetISpinner(itoolBar->GetItemHwnd(ID_PLANARSPIN));
			uSpin->LinkToEdit(itoolBar->GetItemHwnd(ID_PLANAREDIT),EDITTYPE_FLOAT);
			uSpin->SetLimits(0, 180, FALSE);
			uSpin->SetAutoScale();
			ReleaseISpinner(uSpin);

			break;
		}


	case ID_SELECTBY_MATID:
		{
			static MaxBmpFileIcon iconOut(_M("UVWUnwrapSelection"),27);
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &iconOut, &iconOut, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_GEOM_SELECTBYMATID),
				NULL,0,0);

			ICustToolbar* itoolBar = toolBar->GetToolBar();
			itoolBar->AddTool(ToolOtherItem(CUSTEDITWINDOWCLASS, 27,	16, ID_MATIDEDIT,WS_CHILD|WS_VISIBLE, 4, NULL, 0));
			itoolBar->AddTool(ToolOtherItem(SPINNERWINDOWCLASS, 18,	16, ID_MATIDSPIN,WS_CHILD|WS_VISIBLE, 4, NULL, 0));
			ISpinnerControl* uSpin = GetISpinner(itoolBar->GetItemHwnd(ID_MATIDSPIN));
			uSpin->LinkToEdit(itoolBar->GetItemHwnd(ID_MATIDEDIT),EDITTYPE_INT);
			uSpin->SetLimits(1, 256, FALSE);
			uSpin->SetAutoScale();
			ReleaseISpinner(uSpin);

			break;
		}			

	case ID_SELECTBY_SMGRP:
		{
			static MaxBmpFileIcon iconOut(_M("UVWUnwrapSelection"),28);
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &iconOut, &iconOut, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_GEOM_SELECTBYSMGRP),
				NULL,0,0);

			ICustToolbar* itoolBar = toolBar->GetToolBar();
			itoolBar->AddTool(ToolOtherItem(CUSTEDITWINDOWCLASS, 27,	16, ID_SMGRPEDIT,WS_CHILD|WS_VISIBLE, 4, NULL, 0));
			itoolBar->AddTool(ToolOtherItem(SPINNERWINDOWCLASS, 18,	16, ID_SMGRPSPIN,WS_CHILD|WS_VISIBLE, 4, NULL, 0));
			ISpinnerControl* uSpin = GetISpinner(itoolBar->GetItemHwnd(ID_SMGRPSPIN));
			uSpin->LinkToEdit(itoolBar->GetItemHwnd(ID_SMGRPEDIT),EDITTYPE_INT);
			uSpin->SetLimits(1, 32, FALSE);
			uSpin->SetAutoScale();
			ReleaseISpinner(uSpin);

			break;
		}			



//TRANSFORM MODES
	case ID_MOVE:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapModes"),1);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapModes"),1);
			static MaxBmpFileIcon outIcon1(_M("UVWUnwrapModes"),11);
			static MaxBmpFileIcon outIcon2(_M("UVWUnwrapModes"),13);
			FlyOffData fdata2[] = {	{ -1,  -1,  -1,  -1, &outIcon, &outIcon},
									{ -1,  -1,  -1,  -1, &outIcon1, &outIcon1},
									{ -1,  -1,  -1,  -1, &outIcon2, &outIcon2} };
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_MODE_MOVE),
				fdata2,3,move);

			break;
		}
	case ID_ROTATE:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapModes"),3);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapModes"),3);
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_MODE_ROTATE),
				NULL,0,0);
			break;
		}
	case ID_SCALE:
		{

			static MaxBmpFileIcon outIcon(_M("UVWUnwrapModes"),5);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapModes"),5);
			static MaxBmpFileIcon outIcon1(_M("UVWUnwrapModes"),7);
			static MaxBmpFileIcon outIcon2(_M("UVWUnwrapModes"),9);
			FlyOffData fdata2[] = {	{ -1,  -1,  -1,  -1, &outIcon, &outIcon},
									{ -1,  -1,  -1,  -1, &outIcon1, &outIcon1},
									{ -1,  -1,  -1,  -1, &outIcon2, &outIcon2} };
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_MODE_SCALE),
				fdata2,3,scale);

			break;
		}
	case ID_FREEFORMMODE:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapModes"),23);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapModes"),23);
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_MODE_FREEFORM),
				NULL,0,0);
			break;
		}
	case ID_WELD:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools"),3);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapTools"),3);
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_MODE_WELDTARGET),
				NULL,0,0);
			break;
		}


//TOOLS
	case ID_MIRROR:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapModes"),15);
			static MaxBmpFileIcon outIcon1(_M("UVWUnwrapModes"),17);
			static MaxBmpFileIcon outIcon2(_M("UVWUnwrapModes"),19);
			static MaxBmpFileIcon outIcon3(_M("UVWUnwrapModes"),21);
			FlyOffData fdata2[] = {	{ -1,  -1,  -1,  -1, &outIcon, &outIcon},
									{ -1,  -1,  -1,  -1, &outIcon1, &outIcon1},
									{ -1,  -1,  -1,  -1, &outIcon2, &outIcon2},
									{ -1,  -1,  -1,  -1, &outIcon3, &outIcon3} };
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &outIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TOOL_MIRROR),
				fdata2,4,mirror);

			break;
		}
	case ID_BREAK:
	case ID_BREAKBYSUBOBJECT:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools"),1);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapTools"),2);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TOOL_BREAK),
				NULL,0,0);
			break;

		}


	case ID_WELD_SELECTED:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools"),4);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapTools"),4);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TOOL_WELDSELECTED),
				NULL,0,0);
			break;

		}
	case ID_WELDALLSHARED:
		{

			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools"),6);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapTools"),6);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TOOL_WELDALLSHARED),
				NULL,0,0);
			break;
		}
	case ID_WELDSELECTEDSHARED:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools"),5);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapTools"),5);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TOOL_WELDSELECTEDSHARED),
				NULL,0,0);
			break;
		}
	case ID_TOOL_WELDDROPDOWN:
		{
			static MaxBmpFileIcon outIcon1(_M("UVWUnwrapTools"),4);
			static MaxBmpFileIcon outIcon2(_M("UVWUnwrapTools"),5);
			static MaxBmpFileIcon outIcon3(_M("UVWUnwrapTools"),6);
			FlyOffData fdata2[] = {	{ -1,  -1,  -1,  -1, &outIcon1, &outIcon1},
			{ -1,  -1,  -1,  -1, &outIcon2, &outIcon2},
			{ -1,  -1,  -1,  -1, &outIcon3, &outIcon3} };
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon1, &outIcon1, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TOOL_WELDROPDOWN),
				fdata2,3,0);
			break;
		}
	case ID_ROTATE_90_CCW:
		{			
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools"),7);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapTools"),7);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TOOL_ROTATE_90_CCW),
				NULL,0,0);
			break;
		}
	case ID_ROTATE_90_CW:
		{			
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools"),8);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapTools"),8);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TOOL_ROTATE_90_CW),
				NULL,0,0);
			break;
		}

	case ID_FREEFORMSNAP:
		{
			static MaxBmpFileIcon outIcon1(_M("UVWUnwrapTools"),9);
			static MaxBmpFileIcon outIcon2(_M("UVWUnwrapTools"),10);
			static MaxBmpFileIcon outIcon3(_M("UVWUnwrapTools"),11);
			static MaxBmpFileIcon outIcon4(_M("UVWUnwrapTools"),12);
			static MaxBmpFileIcon outIcon5(_M("UVWUnwrapTools"),13);
			FlyOffData fdata2[] = {	{ -1,  -1,  -1,  -1, &outIcon1, &outIcon1},
			{ -1,  -1,  -1,  -1, &outIcon2, &outIcon2},
			{ -1,  -1,  -1,  -1, &outIcon3, &outIcon3}, 
			{ -1,  -1,  -1,  -1, &outIcon4, &outIcon4}, 
			{ -1,  -1,  -1,  -1, &outIcon5, &outIcon5}, 
			};
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon1, &outIcon1, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TOOL_FREEFORMSNAP),
				fdata2,5,4);

			break;
		}
	case ID_ALIGN_SELECTION_VERTICAL:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools"),15);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapTools"),15);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TOOL_ALIGN_SELECTION_VERTICAL),
				NULL,0,0);
			break;
		}
	case ID_ALIGN_SELECTION_HORIZONTAL:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools"),14);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapTools"),14);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TOOL_ALIGN_SELECTION_HORIZONTAL),
				NULL,0,0);
			break;
		}
	case ID_TOOL_ALIGN_LINEAR:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools"),18);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapTools"),18);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TOOL_ALIGN_LINEAR),
				NULL,0,0);
			break;
		}
	case ID_TOOL_SPACE_VERTICAL:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools"),17);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapTools"),17);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TOOL_SPACE_VERTICAL),
				NULL,0,0);
			break;
		}
	case ID_TOOL_SPACE_HORIZONTAL:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools"),16);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapTools"),16);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TOOL_SPACE_HORIZONTAL),
				NULL,0,0);
			break;
		}
	case ID_TOOL_ALIGN_ELEMENT:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools"),19);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapTools"),19);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TOOL_ALIGN_ELEMENT),
				NULL,0,0);
			break;
		}

	case ID_STRAIGHTEN:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools"),20);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapTools"),20);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TOOL_STRAIGHTEN),
				NULL,0,0);
			break;
		}

	case ID_ALIGNBYPIVOTV:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools"),15);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapTools"),15);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_ALIGNBYPIVOTV),
				NULL,0,0);
			break;
		}
	case ID_ALIGNBYPIVOTH:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools"),14);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapTools"),14);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_ALIGNBYPIVOTH),
				NULL,0,0);
			break;
		}

	case ID_ALIGNH_BUTTONS:
		{
			static MaxBmpFileIcon outIcon1(_M("UVWUnwrapTools"),14);
			static MaxBmpFileIcon outIcon2(_M("UVWUnwrapTools"),42);
			FlyOffData fdata2[] = {	{ -1,  -1,  -1,  -1, &outIcon1, &outIcon1},
			{ -1,  -1,  -1,  -1, &outIcon2, &outIcon2}
			};
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon1, &outIcon1, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TOOL_ALIGNH),
				fdata2,2,0);			
			break;
			break;
		}
	case ID_ALIGNV_BUTTONS:
		{
			static MaxBmpFileIcon outIcon1(_M("UVWUnwrapTools"),15);
			static MaxBmpFileIcon outIcon2(_M("UVWUnwrapTools"),43);
			FlyOffData fdata2[] = {	{ -1,  -1,  -1,  -1, &outIcon1, &outIcon1},
			{ -1,  -1,  -1,  -1, &outIcon2, &outIcon2}
			};
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon1, &outIcon1, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TOOL_ALIGNV),
				fdata2,2,0);			
			break;
		}

	case ID_RELAXONECLICK:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools"),21);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapTools"),21);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TOOL_RELAXONECLICK),
				NULL,0,0);
			break;
		}
	case ID_RELAXBUTTONS:
		{
			static MaxBmpFileIcon outIcon1(_M("UVWUnwrapTools"),22);
			static MaxBmpFileIcon outIcon2(_M("UVWUnwrapTools"),23);
			FlyOffData fdata2[] = {	{ -1,  -1,  -1,  -1, &outIcon1, &outIcon1},
			{ -1,  -1,  -1,  -1, &outIcon2, &outIcon2}
			};
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon1, &outIcon1, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TOOL_RELAXBUTTONS),
				fdata2,2,0);
			break;
		}
	case ID_PASTESYMMETRICAL:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools"),24);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapTools"),24);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TOOL_PASTESYMMETRICAL),
				NULL,0,0);
			break;

		}
	case ID_STITCHBUTTONS:
		{
			static MaxBmpFileIcon outIcon1(_M("UVWUnwrapTools"),40);
			static MaxBmpFileIcon outIcon2(_M("UVWUnwrapTools"),25);
			FlyOffData fdata2[] = {	{ -1,  -1,  -1,  -1, &outIcon1, &outIcon1},
			{ -1,  -1,  -1,  -1, &outIcon2, &outIcon2}
			};
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon1, &outIcon1, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TOOL_STITCHBUTTONS),
				fdata2,2,0);
			break;
		}
	case ID_STITCHDIALOG:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools"),25);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapTools"),25);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TOOL_STITCHDIALOG),
				NULL,0,0);
			break;
		}
	case ID_STITCHSOURCE:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools"),26);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapTools"),26);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TOOL_STITCHSOURCE),
				NULL,0,0);
			break;

		}
	case ID_STITCHAVERAGE:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools"),27);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapTools"),27);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TOOL_STITCHAVERAGE),
				NULL,0,0);
			break;
		}
	case ID_STITCHTARGET:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools"),28);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapTools"),28);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TOOL_STITCHTARGET),
				NULL,0,0);
			break;
		}	
	case ID_FLATTENMAP:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools"),29);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapTools"),29);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TOOL_FLATTENMAP),
				NULL,0,0);
			break;
		}	
	case ID_FLATTENBYMATID:   //this is our old by material thta flattens all the material IDs into there own quadrants 
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools"),30);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapTools"),30);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
				GetString(IDS_TOOLTIP_TOOL_FLATTENMAPBYMATID),
				NULL,0,0);
			break;
		}	
	case ID_FLATTEN_BYMATID:  //this is the new one where the mat id is used to define cluster
	{
		static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools"),30);
		static MaxBmpFileIcon inIcon(_M("UVWUnwrapTools"),30);
		toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight,id),
			GetString(IDS_TOOLTIP_TOOL_FLATTENMAPBYMATID),
			NULL,0,0);
		break;
	}	
	case ID_FLATTEN_BYSMOOTHINGGROUP:  //this is the new one where the mat id is used to define cluster
	{
		static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools"),31);
		static MaxBmpFileIcon inIcon(_M("UVWUnwrapTools"),31);
		toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
			GetString(IDS_TOOLTIP_TOOL_FLATTENMAPBYSMOOTHINGGROUP),
			NULL,0,0);
		break;
	}	

	case ID_FLATTENBUTTONS:
		{
			static MaxBmpFileIcon outIcon1(_M("UVWUnwrapTools"),41);
			static MaxBmpFileIcon outIcon2(_M("UVWUnwrapTools"),32);
			FlyOffData fdata2[] = {	{ -1,  -1,  -1,  -1, &outIcon1, &outIcon1},
			{ -1,  -1,  -1,  -1, &outIcon2, &outIcon2}
			};
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon1, &outIcon1, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_FLATTENBUTTONS),
				fdata2,2,0);
			break;	
		}

	case ID_FLATTENMAPDIALOG:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools"),32);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapTools"),32);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_FLATTENMAPDIALOG),
				NULL,0,0);
			break;
		}
	case ID_LSCM_INTERACTIVE:
	case ID_LSCM_MAP:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools"),35);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapTools"),35);
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_LSCM_INTERACTIVE),
				NULL,0,0);
			break;
		}
	case ID_LSCM_SOLVE:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools"),34);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapTools"),34);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_LSCM_SOLVE),
				NULL,0,0);
			break;
		}
	case ID_LSCM_RESET:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools"),33);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapTools"),33);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_LSCM_RESET),
				NULL,0,0);
			break;
		}
	case ID_PIN:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools"),36);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapTools"),36);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_PIN),
				NULL,0,0);
			break;
		}
	case ID_UNPIN:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools"),37);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapTools"),37);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_UNPIN),
				NULL,0,0);
			break;

		}
	case ID_TOOL_FILTERPIN:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools"),38);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapTools"),38);
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_FILTERPIN),
				NULL,0,0);
			break;
		}
	case ID_TOOL_AUTOPIN:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools"),39);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapTools"),39);
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_AUTOPIN),
				NULL,0,0);
			break;
		}

	case ID_PACK_TIGHT:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools2"),3);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &outIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_PACK_TIGHT),
				NULL,0,0);
			break;			
		}
	case ID_PACK_REGION:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools2"),4);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &outIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_PACK_REGION),
				NULL,0,0);
			break;		
		}
	case ID_PACK_FULL:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools2"),5);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &outIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_PACK_FULL),
				NULL,0,0);
			break;				
		}

	case ID_PACKBUTTONS:
		{
			static MaxBmpFileIcon outIcon1(_M("UVWUnwrapTools2"),30);
			static MaxBmpFileIcon outIcon2(_M("UVWUnwrapTools2"),6);
			FlyOffData fdata2[] = {	{ -1,  -1,  -1,  -1, &outIcon1, &outIcon1},
			{ -1,  -1,  -1,  -1, &outIcon2, &outIcon2}
			};
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon1, &outIcon1, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_PACK_BUTTONS),
				fdata2,2,0);
			break;	
		}
	case ID_RESCALECLUSTER:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools2"),1);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &outIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_PACK_RESCALECLUSTER),
				NULL,0,0);
			break;	
		}
	case ID_GROUP:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools2"),7);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &outIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_GROUP),
				NULL,0,0);
			break;	
		}
	case ID_UNGROUP:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools2"),8);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &outIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_UNGROUP),
				NULL,0,0);
			break;	
		}
	case ID_GROUPSELECT:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools2"),9);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &outIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_GROUPSELECT),
				NULL,0,0);
			break;	
		}
	case ID_GROUPSETDENSITY_EDIT:
		{

			ICustToolbar *itoolBar = toolBar->GetToolBar();
/*			itoolBar->AddTool(ToolButtonItem(CTB_PUSHBUTTON,0+68, 0+68, 0+68, 0+68, 16, 15, 60, 22, id));
			ICustButton *but  = itoolBar->GetICustButton(id);
			but->SetHighlightColor(GREEN_WASH);
			but->SetTooltip(TRUE,GetString(IDS_RESCALEPRIORITY));
			but->SetImage(NULL,0,0,0,0,0,0);
			but->SetText(GetString(IDS_RESCALEPRIORITY));
			ReleaseICustButton(but);
*/

			
//			itoolBar->AddTool(ToolOtherItem(_M("static"), 50,	mToolBarHeight,ID_GROUPSETDENSITY_TEXT,WS_CHILD|WS_VISIBLE, 4, _M(GetString(IDS_RESCALEPRIORITY)), 0));
			itoolBar->AddTool(ToolOtherItem(CUSTEDITWINDOWCLASS, 30,	16, ID_GROUPSETDENSITY_EDIT,WS_CHILD|WS_VISIBLE, 4, NULL, 0));
			itoolBar->AddTool(ToolOtherItem(SPINNERWINDOWCLASS, 18,	16, ID_GROUPSETDENSITY_SPINNER,WS_CHILD|WS_VISIBLE, 4, NULL, 0));
			ISpinnerControl* uSpin = GetISpinner(itoolBar->GetItemHwnd(ID_GROUPSETDENSITY_SPINNER));
			uSpin->LinkToEdit(itoolBar->GetItemHwnd(ID_GROUPSETDENSITY_EDIT),EDITTYPE_FLOAT);
			uSpin->SetLimits(0.0001f, 1.0f, FALSE);
			uSpin->SetAutoScale();
			ReleaseISpinner(uSpin);

			break;	
		}
//OPTIONS
	case ID_UPDATEMAP:
		{
			ICustToolbar *itoolBar = toolBar->GetToolBar();
			itoolBar->AddTool(ToolButtonItem(CTB_PUSHBUTTON,0+68, 0+68, 0+68, 0+68, 16, 15, 70, 22, id));
			ICustButton *but  = itoolBar->GetICustButton(id);
			but->SetHighlightColor(GREEN_WASH);
			but->SetTooltip(TRUE,GetString(IDS_TOOLTIP_TOOL_UPDATEMAP));
			but->SetImage(NULL,0,0,0,0,0,0);
			but->SetText(GetString(IDS_RB_UPDATE));
			ReleaseICustButton(but);
			break;
		}

	case ID_UVW:
		{
			static MaxBmpFileIcon outIcon1(_M("UVWUnwrapOption"),3);
			static MaxBmpFileIcon outIcon2(_M("UVWUnwrapOption"),4);
			static MaxBmpFileIcon outIcon3(_M("UVWUnwrapOption"),5);
			FlyOffData fdata2[] = {	{ -1,  -1,  -1,  -1, &outIcon1, &outIcon1},
			{ -1,  -1,  -1,  -1, &outIcon2, &outIcon2} ,
			{ -1,  -1,  -1,  -1, &outIcon3, &outIcon3} 
			};
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon1, &outIcon1, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_UVWSPACE),
				fdata2,3,0);
			break;

		}

	case ID_SHOWMAP:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapOption"),1);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapOption"),1);
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_SHOWMAP),
				NULL,0,0);
			break;
		}

	case ID_PROPERTIES:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapOption"),6);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapOption"),6);
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_PROPERTIES),
				NULL,0,0);
			break;
		}
	case ID_TEXTURE_COMBO:
		{
			ICustToolbar *itoolBar = toolBar->GetToolBar();
			itoolBar->AddTool(ToolOtherItem(_T("combobox"), 170,	430, id,CBS, 2, NULL, 0));
			hTextures = itoolBar->GetItemHwnd(ID_TEXTURE_COMBO);

			HFONT hFont;
			hFont = CreateFont(GetUIFontHeight(),0,0,0,FW_LIGHT,0,0,0,GetUIFontCharset(),0,0,0, VARIABLE_PITCH | FF_SWISS, _T(""));
			SendMessage(hTextures, WM_SETFONT, (WPARAM)hFont, MAKELONG(0, 0));
			break;
		}



//TYPEIN
	case ID_ABSOLUTETYPEIN:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTypeIn"),1);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapTypeIn"),2);
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_ABSOLUTETYPEIN),
				NULL,0,0);
			break;
		}
	case ID_ABSOLUTETYPEIN_SPINNERS:
	case ID_STATICU:					//this ID comes when we load since this is item is 9 elements
		{

			ICustToolbar *itoolBar = toolBar->GetToolBar();
			itoolBar->AddTool(ToolOtherItem(_M("static"), 18,	mToolBarHeight, ID_STATICU,WS_CHILD|WS_VISIBLE, 4, _M(" U:"), 0));
			itoolBar->AddTool(ToolOtherItem(CUSTEDITWINDOWCLASS, 30,	16, ID_EDITU,WS_CHILD|WS_VISIBLE, 4, NULL, 0));
			itoolBar->AddTool(ToolOtherItem(SPINNERWINDOWCLASS, 18,	16, ID_SPINNERU,WS_CHILD|WS_VISIBLE, 4, NULL, 0));
			ISpinnerControl* uSpin = GetISpinner(itoolBar->GetItemHwnd(ID_SPINNERU));
			uSpin->LinkToEdit(itoolBar->GetItemHwnd(ID_EDITU),EDITTYPE_FLOAT);
			uSpin->SetLimits(-9999999, 9999999, FALSE);
			uSpin->SetAutoScale();
			ReleaseISpinner(uSpin);

			itoolBar->AddTool(ToolOtherItem(_M("static"), 18,	mToolBarHeight, ID_STATICV,WS_CHILD|WS_VISIBLE, 4, _M("V:"), 0));
			itoolBar->AddTool(ToolOtherItem(CUSTEDITWINDOWCLASS, 30,	16, ID_EDITV,WS_CHILD|WS_VISIBLE, 4, NULL, 0));
			itoolBar->AddTool(ToolOtherItem(SPINNERWINDOWCLASS, 18,	16, ID_SPINNERV,WS_CHILD|WS_VISIBLE, 4, NULL, 0));
			ISpinnerControl* vSpin = GetISpinner(itoolBar->GetItemHwnd(ID_SPINNERV));
			vSpin->LinkToEdit(itoolBar->GetItemHwnd(ID_EDITV),EDITTYPE_FLOAT);
			vSpin->SetLimits(-9999999, 9999999, FALSE);
			vSpin->SetAutoScale();
			ReleaseISpinner(vSpin);


			itoolBar->AddTool(ToolOtherItem(_M("static"), 18,	mToolBarHeight, ID_STATICW,WS_CHILD|WS_VISIBLE, 4, _M("W:"), 0));
			itoolBar->AddTool(ToolOtherItem(CUSTEDITWINDOWCLASS, 30,	16, ID_EDITW,WS_CHILD|WS_VISIBLE, 4, NULL, 0));
			itoolBar->AddTool(ToolOtherItem(SPINNERWINDOWCLASS, 18,	16, ID_SPINNERW,WS_CHILD|WS_VISIBLE, 4, NULL, 0));
			ISpinnerControl* wSpin = GetISpinner(itoolBar->GetItemHwnd(ID_SPINNERW));
			wSpin->LinkToEdit(itoolBar->GetItemHwnd(ID_EDITW),EDITTYPE_FLOAT);
			wSpin->SetLimits(-9999999, 9999999, FALSE);
			wSpin->SetAutoScale();
			ReleaseISpinner(wSpin);

			break;

		}
//VIEW


	case ID_LOCKSELECTED:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapView"),23);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapView"),23);
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_LOCKEDSELECTED),
				NULL,0,0);
			break;
		}

	case ID_HIDE:
		{
			static MaxBmpFileIcon outIcon1(_M("UVWUnwrapView"),13);
			static MaxBmpFileIcon outIcon2(_M("UVWUnwrapView"),15);
			FlyOffData fdata2[] = {	{ -1,  -1,  -1,  -1, &outIcon1, &outIcon1},
			{ -1,  -1,  -1,  -1, &outIcon2, &outIcon2} 
			};

			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon1, &outIcon1, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_HIDE),
				fdata2,2,hide);
			break;
		}

	case ID_FREEZE:
		{
			static MaxBmpFileIcon outIcon1(_M("UVWUnwrapView"),17);
			static MaxBmpFileIcon outIcon2(_M("UVWUnwrapView"),19);
			FlyOffData fdata2[] = {	{ -1,  -1,  -1,  -1, &outIcon1, &outIcon1},
			{ -1,  -1,  -1,  -1, &outIcon2, &outIcon2} 
			};

			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon1, &outIcon1, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_FREEZE),
				fdata2,2,freeze);
			break;
		}

	case ID_FILTER_SELECTEDFACES:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapView"),25);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapView"),26);
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_FILTERSELECTEDFACES),
				NULL,0,0);
			break;
		}
	case ID_FILTER_MATID:
		{
			ICustToolbar* itoolBar = toolBar->GetToolBar();
			itoolBar->AddTool(ToolOtherItem(_T("combobox"), 140,	280, ID_FILTER_MATID, CBS, 2, NULL, 0));

			hMatIDs = itoolBar->GetItemHwnd(ID_FILTER_MATID);
			HFONT hFont;			// Add for Japanese version
			hFont = CreateFont(GetUIFontHeight(),0,0,0,FW_LIGHT,0,0,0,GetUIFontCharset(),0,0,0, VARIABLE_PITCH | FF_SWISS, _T(""));
			SendMessage(hMatIDs, WM_SETFONT, (WPARAM)hFont, MAKELONG(0, 0));
			SendMessage(hMatIDs, CB_ADDSTRING, 0, (LPARAM)GetString(IDS_PW_ID_ALLID));	
			SendMessage(hMatIDs,CB_SETCURSEL, (WPARAM)0, (LPARAM)0 );			
			break;
		}
	case ID_PAN:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapView"),1);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapView"),1);
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_PAN),
				NULL,0,0);
			break;
		}

	case ID_ZOOMTOOL:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapView"),3);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapView"),3);
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_ZOOMTOOL),
				NULL,0,0);
			break;
		}

	case ID_ZOOMREGION:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapView"),5);
			static MaxBmpFileIcon inIcon(_M("UVWUnwrapView"),5);
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &outIcon, &inIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_ZOOMREGION),
				NULL,0,0);
			break;
		}

	case ID_ZOOMEXTENT:
		{
			static MaxBmpFileIcon outIcon1(_M("UVWUnwrapView"),7);
			static MaxBmpFileIcon outIcon2(_M("UVWUnwrapView"),9);
			static MaxBmpFileIcon outIcon3(_M("UVWUnwrapView"),10);
			FlyOffData fdata2[] = {	{ -1,  -1,  -1,  -1, &outIcon1, &outIcon1},
			{ -1,  -1,  -1,  -1, &outIcon2, &outIcon2} ,
			{ -1,  -1,  -1,  -1, &outIcon3, &outIcon3} 
			};

			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon1, &outIcon1, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_ZOOMEXTENT),
				fdata2,3,zoomext);
			break;
		}

	case ID_SNAP:
		{
			static MaxBmpFileIcon outIcon1(_M("UVWUnwrapView"),12);
			static MaxBmpFileIcon outIcon2(_M("UVWUnwrapView"),11);
			FlyOffData fdata2[] = {	{ -1,  -1,  -1,  -1, &outIcon1, &outIcon1},
			{ -1,  -1,  -1,  -1, &outIcon2, &outIcon2} 
			};

			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &outIcon1, &outIcon1, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_SNAP),
				fdata2,2,0);
			break;

		}
//EDIT UVS
	case ID_EDIT:
		{
			ICustToolbar* itoolBar = toolBar->GetToolBar();
			itoolBar->AddTool(ToolButtonItem(CTB_PUSHBUTTON,0, 0, 0, 0, 16, 15, 140, 22, id));
			ICustButton *but  = itoolBar->GetICustButton(id);
			but->SetHighlightColor(GREEN_WASH);
			but->SetTooltip(TRUE,GetString(IDS_EDIT));
			but->SetImage(NULL,0,0,0,0,0,0);
			but->SetText(GetString(IDS_EDIT));
			break;
		}	

	case ID_TWEAKUVW:
		{
			ICustToolbar* itoolBar = toolBar->GetToolBar();
			itoolBar->AddTool(ToolButtonItem(CTB_CHECKBUTTON,0, 0, 0, 0, 16, 15, 140, 22, id));
			ICustButton *but  = itoolBar->GetICustButton(id);
			but->SetHighlightColor(GREEN_WASH);
			but->SetTooltip(TRUE,GetString(IDS_TWEAKUVW));
			but->SetImage(NULL,0,0,0,0,0,0);
			but->SetText(GetString(IDS_TWEAKUVW));

			break;
		}	
	case ID_QMAP:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools2"),14);			
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &outIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_QMAP),
				NULL,0,0);
			break;
		}
	case ID_QUICKMAP_DISPLAY:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools2"),15);			
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &outIcon, &outIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_QMAP_DISPLAY),
				NULL,0,0);
			break;
		}
	case ID_QUICKMAP_ALIGN:
		{
			static MaxBmpFileIcon outIcon1(_M("UVWUnwrapTools2"),10);			
			static MaxBmpFileIcon outIcon2(_M("UVWUnwrapTools2"),11);			
			static MaxBmpFileIcon outIcon3(_M("UVWUnwrapTools2"),12);			
			static MaxBmpFileIcon outIcon4(_M("UVWUnwrapTools2"),31);		
			FlyOffData fdata2[] = {	{ -1,  -1,  -1,  -1, &outIcon1, &outIcon1},
			{ -1,  -1,  -1,  -1, &outIcon2, &outIcon2},
			{ -1,  -1,  -1,  -1, &outIcon3, &outIcon3},
			{ -1,  -1,  -1,  -1, &outIcon4, &outIcon4}
			};

			int align =  0;
			pblock->GetValue(unwrap_qmap_align,0,align,FOREVER);

			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &outIcon1, &outIcon1, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_QMAP_ALIGN),
				fdata2,4,align);
			break;
		}

	case ID_PELT_EDITSEAMS:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools2"),17);			
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &outIcon, &outIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_EDIT_SEAMS),
				NULL,0,0);
			break;
		}
	case ID_PELT_POINTTOPOINTSEAMS:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools2"),18);			
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &outIcon, &outIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_POINTTOPOINTSEAMS),
				NULL,0,0);
			break;
		}
	case ID_PW_SELTOSEAM2:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools2"),19);			
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &outIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_SELTOSEAMS),
				NULL,0,0);
			break;
		}

	case ID_PELT_EXPANDSELTOSEAM:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools2"),20);			
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &outIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_EXPANDSELTOSEAM),
				NULL,0,0);
			break;
		}
	case ID_PLANAR_MAP:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools2"),21);			
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &outIcon, &outIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_PLANAR_MAP),
				NULL,0,0);
			break;
			
		}			
	case ID_CYLINDRICAL_MAP:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools2"),22);			
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &outIcon, &outIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_CYLINDRICAL_MAP),
				NULL,0,0);
			break;
		}			
	case ID_SPHERICAL_MAP:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools2"),23);			
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &outIcon, &outIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_SPHERICAL_MAP),
				NULL,0,0);
			break;
		}			
	case ID_BOX_MAP:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools2"),24);			
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &outIcon, &outIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_BOX_MAP),
				NULL,0,0);
			break;
		}	
	case ID_PELT_MAP:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools2"),16);			
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &outIcon, &outIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_PELT_MAP),
				NULL,0,0);
			break;
		}	
	case ID_SPLINE_MAP:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools2"),25);			
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &outIcon, &outIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_SPLINE_MAP),
				NULL,0,0);
			break;
		}
	case ID_UNFOLD_MAP:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools2"),26);			
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &outIcon, &outIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_UNFOLD_MAP),
				NULL,0,0);
			break;
		}
	case ID_UNFOLD_EDGE:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools2"),26);			
			toolBar->AddButton(id,ToolButtonItem(CTB_CHECKBUTTON, &outIcon, &outIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_UNFOLD_EDGE),
				NULL,0,0);
			break;
		}
	case ID_MAPPING_ALIGNX:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools2"),10);			
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &outIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_ALIGNX),
				NULL,0,0);
			break;
		}			

	case ID_MAPPING_ALIGNY:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools2"),11);			
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &outIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_ALIGNY),
				NULL,0,0);
			break;
		}			
	case ID_MAPPING_ALIGNZ:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools2"),12);			
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &outIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_ALIGNZ),
				NULL,0,0);
			break;
		}			
	case ID_MAPPING_NORMALALIGN:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools2"),13);			
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &outIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_NORMALIGN),
				NULL,0,0);
			break;

		}			
	case ID_MAPPING_ALIGNTOVIEW:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapTools2"),29);			
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &outIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_ALIGNVIEW),
				NULL,0,0);
			break;
		}	
	case ID_MAPPING_FIT:
		{
			ICustToolbar *itoolBar = toolBar->GetToolBar();
			itoolBar->AddTool(ToolButtonItem(CTB_PUSHBUTTON,0+68, 0+68, 0+68, 0+68, 16, 15, 28, 22, id));
			ICustButton *but  = itoolBar->GetICustButton(id);
			but->SetHighlightColor(GREEN_WASH);
			but->SetTooltip(TRUE,GetString(IDS_TOOLTIP_TOOL_MAPPING_FIT));
			but->SetImage(NULL,0,0,0,0,0,0);
			but->SetText(GetString(IDS_FIT));
			ReleaseICustButton(but);
			break;
		}
	case ID_MAPPING_CENTER:
		{
			ICustToolbar *itoolBar = toolBar->GetToolBar();
			itoolBar->AddTool(ToolButtonItem(CTB_PUSHBUTTON,0+68, 0+68, 0+68, 0+68, 16, 15, 40, 22, id));
			ICustButton *but  = itoolBar->GetICustButton(id);
			but->SetHighlightColor(GREEN_WASH);
			but->SetTooltip(TRUE,GetString(IDS_TOOLTIP_TOOL_MAPPING_CENTER));
			but->SetImage(NULL,0,0,0,0,0,0);
			but->SetText(GetString(IDS_CENTER));
			ReleaseICustButton(but);
			break;
		}
	case ID_MAPPING_RESET:
		{
			static MaxBmpFileIcon outIcon(_M("UVWUnwrapView"),27);					
			toolBar->AddButton(id,ToolButtonItem(CTB_PUSHBUTTON, &outIcon, &outIcon, buttonImageWidth,buttonImageHeight,buttonWidth,buttonHeight, id),
				GetString(IDS_TOOLTIP_TOOL_MAPPING_RESET),
				NULL,0,0);
			break;
		}	

	case ID_PACK_RESCALE:
		{
			ICustToolbar* itoolBar = toolBar->GetToolBar();
			itoolBar->AddTool(ToolOtherItem(_T("BUTTON"), 62,	mToolBarHeight, ID_PACK_RESCALE,WS_CHILD|WS_VISIBLE|BS_CHECKBOX|BS_VCENTER, 0, GetString(IDS_EMPTY), 0));						
			HWND hwnd = itoolBar->GetItemHwnd(ID_PACK_RESCALE);
			if (mPackTempRescale)
				SendMessage(hwnd,BM_SETCHECK,BST_CHECKED,0);
			else
				SendMessage(hwnd,BM_SETCHECK,BST_UNCHECKED,0);
			break;
		}
	case ID_PACK_ROTATE:
		{
			ICustToolbar* itoolBar = toolBar->GetToolBar();
			itoolBar->AddTool(ToolOtherItem(_T("BUTTON"), 60,	mToolBarHeight, ID_PACK_ROTATE,WS_CHILD|WS_VISIBLE|BS_CHECKBOX|BS_VCENTER, 0, GetString(IDS_EMPTY), 0));			
			HWND hwnd = itoolBar->GetItemHwnd(ID_PACK_ROTATE);
			if (mPackTempRotate)
				SendMessage(hwnd,BM_SETCHECK,BST_CHECKED,0);
			else
				SendMessage(hwnd,BM_SETCHECK,BST_UNCHECKED,0);
			break;
		}
//	case ID_PACK_PADDINGTEXT:
	case ID_PACK_PADDINGEDIT:
		//this ID comes when we load since this is item is 9 elements
		{
			ICustToolbar* itoolBar = toolBar->GetToolBar();
//			itoolBar->AddTool(ToolOtherItem(_M("static"), 50,	mToolBarHeight, ID_PACK_PADDINGTEXT,WS_CHILD|WS_VISIBLE, 4, _M(GetString(IDS_PACK_PADDINGEDIT)), 0));
			itoolBar->AddTool(ToolOtherItem(CUSTEDITWINDOWCLASS, 38,	16, ID_PACK_PADDINGEDIT,WS_CHILD|WS_VISIBLE, 4, NULL, 0));
			itoolBar->AddTool(ToolOtherItem(SPINNERWINDOWCLASS, 18,	16, ID_PACK_PADDINGSPINNER,WS_CHILD|WS_VISIBLE, 4, NULL, 0));
			ISpinnerControl* uSpin = GetISpinner(itoolBar->GetItemHwnd(ID_PACK_PADDINGSPINNER));
			uSpin->LinkToEdit(itoolBar->GetItemHwnd(ID_PACK_PADDINGEDIT),EDITTYPE_FLOAT);
			uSpin->SetLimits(0.0f, 1.0f, FALSE);
			uSpin->SetValue(mPackTempPadding,FALSE);
			uSpin->SetAutoScale();
			ReleaseISpinner(uSpin);

			break;
		}

	default:   //no custom implementation so just do a simple text button
		{
			return AddDefaultActionToBar(toolBar->GetToolBar(), id);
			break;
		}
	}

	return TRUE;
}

