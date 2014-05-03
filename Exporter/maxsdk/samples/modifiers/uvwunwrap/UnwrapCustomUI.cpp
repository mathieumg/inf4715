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

#include "UnwrapCustomUI.h"
#include "unwrap.h"
#include "modsres.h"

UnwrapButton::UnwrapButton(int id, HWND hwnd)
{
	mID = id;
	mButton = GetICustButton(hwnd);
}
UnwrapButton::~UnwrapButton()
{
	if (mButton)
		ReleaseICustButton(mButton);
}
UnwrapButton::UnwrapButton()
{
	mButton = NULL;
}
ICustButton* UnwrapButton::ButtonPtr()
{
	return mButton;
}
int UnwrapButton::ID()
{
	return mID;
}
	 

UnwrapCustomUI::UnwrapCustomUI()
{
	mMod = NULL;
	mhInstance = NULL;
}
UnwrapCustomUI::~UnwrapCustomUI()
{
	Free( NULL );	
	FreeButtons();
}

void UnwrapCustomUI::FreeButtons()
{
	for (int i = 0; i < mButtons.Count(); i++)
	{
		if (mButtons[i])
			delete mButtons[i];
		mButtons[i] = NULL;
	}
	mButtons.SetCount(0);
}

void UnwrapCustomUI::Free( HWND parentWindow)
{
	for (int i = 0; i < mToolbars.Count(); i++)
	{
		if (mToolbars[i] == NULL)
		{
			mToolbars.Delete(i,1);
			mToolbars[i] = NULL;
			i--;
		}
		else
		{
			bool del = false;
			if (parentWindow == NULL)
				del = true;
			else if ( parentWindow == mToolbars[i]->GetParWindow() )
				del = true;
			if (del)
			{
				delete mToolbars[i];
				mToolbars.Delete(i,1);
				i--;
			}
		}
	}


}

void UnwrapCustomUI::SetMod(UnwrapMod *mod, HINSTANCE hinstance)
{
	mMod = mod;
	mhInstance = hinstance;
}


int UnwrapCustomUI::NumberToolBars()
{
	return mToolbars.Count();
}

const MCHAR* UnwrapCustomUI::Name(int index)
{
	if ( (index >= 0) && (index < mToolbars.Count()))
	{
		return mToolbars[index]->Name();
	}
	return NULL;
}

ToolBarFrame* UnwrapCustomUI::GetToolBarFrame(const MCHAR* name)
{
	for (int i = 0; i < mToolbars.Count(); i++)
	{
		if (_tcscmp(name,mToolbars[i]->Name()) == 0)
			return mToolbars[i];
	}
	return NULL;
}

void UnwrapCustomUI::AddButton(int id,HWND hWnd, BOOL checkButton)
{
	UnwrapButton *but = new UnwrapButton(id,hWnd);
	if (but)
	{
		if (checkButton)
			but->ButtonPtr()->SetType(CBT_CHECK);
		mButtons.Append(1,&but,100);
	}
}

void UnwrapCustomUI::AppendToolBar(int owner, HWND parent, const MCHAR* name, int pos,  int x, int y, int width, int height, bool popup)
{
	ToolBarFrame *toolBarFrame = new ToolBarFrame(mhInstance,owner,parent,x,y,width,height,(ToolBarFrame::DockPos)pos,popup,name);	
	toolBarFrame->Show(true);

	mToolbars.Append(1,&toolBarFrame,10);

}

void UnwrapCustomUI::AppendToolBar(HWND hwnd, int owner, const MCHAR* name)
{
	ToolBarFrame *toolBarFrame = new ToolBarFrame(hwnd, owner ,name);	
	toolBarFrame->Show(true);

	mToolbars.Append(1,&toolBarFrame,10);

}

void UnwrapCustomUI::UpdatePositions()
{
	int ct = 0;
	int processed = 0;
	mCornerHeight[0] = 0;
	mCornerHeight[1] = 0;
	mCornerHeight[2] = 0;
	mCornerHeight[3] = 0;

	Tab<int> upLeftW;
	Tab<int> upRightW;
	Tab<int> lowerLeftW;
	Tab<int> lowerRightW;

	for (int j = 0; j < mToolbars.Count(); j++)
	{
		int w = mToolbars[j]->Width();
		if (mToolbars[j]->Location() == ToolBarFrame::kDockUpperLeft)
			upLeftW.Append(1,&w,10);
		else if (mToolbars[j]->Location() == ToolBarFrame::kDockUpperRight)
			upRightW.Append(1,&w,10);
		else if (mToolbars[j]->Location() == ToolBarFrame::kDockLowerLeft)
			lowerLeftW.Append(1,&w,10);
		else if (mToolbars[j]->Location() == ToolBarFrame::kDockLowerRight)
			lowerRightW.Append(1,&w,10);
	}

	
	int topIndex = 0;
	int bottomIndex = 0;
	while (processed != mToolbars.Count())
	{
		HWND parentHWND = mToolbars[ct]->GetParWindow();
		int yPos[4];
		Rect r;
		GetClientRect(parentHWND,&r);
		yPos[0] = 0;
		yPos[1] = 0;
		yPos[2] = r.h() - mToolbars[ct]->Height()-2;
		yPos[3] = r.h() - mToolbars[ct]->Height()-2;
		int clientWidth  = r.w();

		bool ctSet = false;
		for (int j = (ct); j < mToolbars.Count(); j++)
		{
			if (mToolbars[j]->GetParWindow() == parentHWND)
			{
				int w = mToolbars[j]->Width();
				int h = mToolbars[j]->Height();


				if (mToolbars[j]->Location() == ToolBarFrame::kFloat)
					mToolbars[j]->UpdateWindow();
				else if (mToolbars[j]->Location() == ToolBarFrame::kDockUpperLeft)
				{
					if (topIndex < upRightW.Count())
						w = clientWidth - upRightW[topIndex];
					mToolbars[j]->ResizeWindow(0,yPos[0],w,h,FALSE);
					yPos[0] += h;
					mCornerHeight[0] += h;
					topIndex++;
				}
				else if (mToolbars[j]->Location() == ToolBarFrame::kDockUpperRight)
				{
					int x = r.w()-w;
					if (x < 0)
						x = 0;
					int tw = w;
					if (x + tw > clientWidth)
						tw = clientWidth - x;

					mToolbars[j]->ResizeWindow(x,yPos[1],tw,h,FALSE);
					yPos[1] += h;
					mCornerHeight[1] += h;
					topIndex++;
				}
				else if (mToolbars[j]->Location() == ToolBarFrame::kDockLowerLeft)
				{
					if (bottomIndex < lowerRightW.Count())
						w = clientWidth - lowerRightW[bottomIndex];
					mToolbars[j]->ResizeWindow(0,yPos[2],w,h,FALSE);
					yPos[2] -= h;
					mCornerHeight[2] += h;
					bottomIndex++;
				}
				else if (mToolbars[j]->Location() == ToolBarFrame::kDockLowerRight)
				{
					int x = r.w()-w;
					if (x < 0)
						x = 0;
					int tw = w;
					if (x + tw > clientWidth)
						tw = clientWidth - x;


					mToolbars[j]->ResizeWindow(x,yPos[3],tw,h,FALSE);
					yPos[3] -= h;
					mCornerHeight[3] += h;
					bottomIndex++;
				}
				processed++;

				
			}
			else
			{
				if ((ctSet == false) && (j != ct))
				{
					ct = j;
					ctSet = true;
				}				
			}
		}
	}
}

int UnwrapCustomUI::GetFlyOut(int buttonID)
{
	for (int i = 0; i < mToolbars.Count(); i++)
	{
		//get the tool bar
		HWND toolBarHWND = mToolbars[i]->GetToolbarWindow();
		ICustToolbar *toolBar = GetICustToolbar(toolBarHWND); 
		if (toolBar)
		{
			//get the elements
			int numItems = toolBar->GetNumItems();
			// see if it is a check box if so update it
			for (int j = 0; j < numItems; j++)
			{
				int testId = toolBar->GetItemID(j);
				if ( buttonID == testId )
				{

					ICustButton *button = toolBar->GetICustButton(buttonID);
					if (button)
					{
						int v = button->GetCurFlyOff(); 
						ReleaseICustButton(button);
						ReleaseICustToolbar(toolBar);
						return v;
					}
				}
			}
		}
		ReleaseICustToolbar(toolBar);
	}
	return 0;
}

void UnwrapCustomUI::SetFlyOut(int buttonID, int flyOut, BOOL notify)
{
	for (int i = 0; i < mToolbars.Count(); i++)
	{
		//get the tool bar
		HWND toolBarHWND = mToolbars[i]->GetToolbarWindow();
		ICustToolbar *toolBar = GetICustToolbar(toolBarHWND); 
		if (toolBar)
		{
			//get the elements
			int numItems = toolBar->GetNumItems();
			// see if it is a check box if so update it
			for (int j = 0; j < numItems; j++)
			{
				int testId = toolBar->GetItemID(j);
				if ( buttonID == testId )
				{

					ICustButton *button = toolBar->GetICustButton(buttonID);
					if (button)
					{
						button->SetCurFlyOff(flyOut,notify);
						ReleaseICustButton(button);
					}
				}
			}
			//fix mem leak
			ReleaseICustToolbar(toolBar);
		}
	}
}


void UnwrapCustomUI::Enable(int buttonID, BOOL enable)
{
	for (int i = 0; i < mToolbars.Count(); i++)
	{
		//get the tool bar
		HWND toolBarHWND = mToolbars[i]->GetToolbarWindow();
		ICustToolbar *toolBar = GetICustToolbar(toolBarHWND); 
		if (toolBar)
		{
			//get the elements
			int numItems = toolBar->GetNumItems();
			// see if it is a check box if so update it
			for (int j = 0; j < numItems; j++)
			{
				int testId = toolBar->GetItemID(j);
				if ( buttonID == testId )
				{

					ICustButton *button = toolBar->GetICustButton(buttonID);
					if (button)
					{
						if (enable)
							button->Enable();
						else
							button->Disable();
						ReleaseICustButton(button);
					}
					
				}
			}
			//fix mem leak
			ReleaseICustToolbar(toolBar);

		}
	}
	for (int i = 0; i < mButtons.Count(); i++)
	{
		if (mButtons[i] && mButtons[i]->ID() == buttonID)
		{
			if (enable)
				mButtons[i]->ButtonPtr()->Enable();
			else
				mButtons[i]->ButtonPtr()->Disable();
		}
	}
}


void UnwrapCustomUI::SetIndeterminate(int buttonID,BOOL indet)
{
	for (int i = 0; i < mToolbars.Count(); i++)
	{
		//get the tool bar
		HWND toolBarHWND = mToolbars[i]->GetToolbarWindow();
		ICustToolbar *toolBar = GetICustToolbar(toolBarHWND); 
		if (toolBar)
		{
			//get the elements
			int numItems = toolBar->GetNumItems();
			// see if it is a check box if so update it
			for (int j = 0; j < numItems; j++)
			{
				int testId = toolBar->GetItemID(j);
				if ( buttonID == testId )
				{
					ISpinnerControl *spin = GetISpinner(toolBar->GetItemHwnd(buttonID));
					if (spin)
					{
						spin->SetIndeterminate(indet);
						ReleaseISpinner(spin);
					}
				}
			}
			//fix mem leak
			ReleaseICustToolbar(toolBar);
		}
	}
}

void UnwrapCustomUI::SetSpinFValue(int buttonID, float v)
{
	for (int i = 0; i < mToolbars.Count(); i++)
	{
		//get the tool bar
		HWND toolBarHWND = mToolbars[i]->GetToolbarWindow();
		ICustToolbar *toolBar = GetICustToolbar(toolBarHWND); 
		if (toolBar)
		{
			//get the elements
			int numItems = toolBar->GetNumItems();
			// see if it is a check box if so update it
			for (int j = 0; j < numItems; j++)
			{
				int testId = toolBar->GetItemID(j);
				if ( buttonID == testId )
				{
					ISpinnerControl *spin = GetISpinner(toolBar->GetItemHwnd(buttonID));
					if (spin)
					{
						spin->SetValue(v,FALSE);
						ReleaseISpinner(spin);
					}
				}
			}
			//fix mem leak
			ReleaseICustToolbar(toolBar);
		}
	}
}

float UnwrapCustomUI::GetSpinFValue(int buttonID)
{
	for (int i = 0; i < mToolbars.Count(); i++)
	{
		//get the tool bar
		HWND toolBarHWND = mToolbars[i]->GetToolbarWindow();
		ICustToolbar *toolBar = GetICustToolbar(toolBarHWND); 
		if (toolBar)
		{
			//get the elements
			int numItems = toolBar->GetNumItems();
			// see if it is a check box if so update it
			for (int j = 0; j < numItems; j++)
			{
				int testId = toolBar->GetItemID(j);
				if ( buttonID == testId )
				{
					ISpinnerControl *spin = GetISpinner(toolBar->GetItemHwnd(buttonID));
					if (spin)
					{
						float v= spin->GetFVal();
						ReleaseISpinner(spin);
						ReleaseICustToolbar(toolBar);
						return v;
					}
				}
			}
			//fix mem leak
			ReleaseICustToolbar(toolBar);
		}
	}

	return 0.0f;
}

void UnwrapCustomUI::SetSpinIValue(int buttonID, int v)
{
	for (int i = 0; i < mToolbars.Count(); i++)
	{
		//get the tool bar
		HWND toolBarHWND = mToolbars[i]->GetToolbarWindow();
		ICustToolbar *toolBar = GetICustToolbar(toolBarHWND); 
		if (toolBar)
		{
			//get the elements
			int numItems = toolBar->GetNumItems();
			// see if it is a check box if so update it
			for (int j = 0; j < numItems; j++)
			{
				int testId = toolBar->GetItemID(j);
				if ( buttonID == testId )
				{
					ISpinnerControl *spin = GetISpinner(toolBar->GetItemHwnd(buttonID));
					if (spin)
					{
						spin->SetValue(v,FALSE);
						ReleaseISpinner(spin);
					}
				}
			}
			//fix mem leak
			ReleaseICustToolbar(toolBar);
		}
	}
}

int UnwrapCustomUI::GetSpinIValue(int buttonID)
{
	for (int i = 0; i < mToolbars.Count(); i++)
	{
		//get the tool bar
		HWND toolBarHWND = mToolbars[i]->GetToolbarWindow();
		ICustToolbar *toolBar = GetICustToolbar(toolBarHWND); 
		if (toolBar)
		{
			//get the elements
			int numItems = toolBar->GetNumItems();
			// see if it is a check box if so update it
			for (int j = 0; j < numItems; j++)
			{
				int testId = toolBar->GetItemID(j);
				if ( buttonID == testId )
				{
					ISpinnerControl *spin = GetISpinner(toolBar->GetItemHwnd(buttonID));
					if (spin)
					{
						int v= spin->GetIVal();
						ReleaseISpinner(spin);
						ReleaseICustToolbar(toolBar);
						return v;
					}
				}
			}
			//fix mem leak
			ReleaseICustToolbar(toolBar);
		}
	}

	return 0;
}

BOOL UnwrapCustomUI::IsChecked(int buttonID)
{
	for (int i = 0; i < mToolbars.Count(); i++)
	{
		//get the tool bar
		HWND toolBarHWND = mToolbars[i]->GetToolbarWindow();
		ICustToolbar *toolBar = GetICustToolbar(toolBarHWND); 
		if (toolBar)
		{
			//get the elements
			int numItems = toolBar->GetNumItems();
			// see if it is a check box if so update it
			for (int j = 0; j < numItems; j++)
			{
				int testId = toolBar->GetItemID(j);
				if ( buttonID == testId )
				{
					if (SendMessage(toolBar->GetItemHwnd(buttonID),BM_GETCHECK,0,0) == BST_CHECKED)
					{
						ReleaseICustToolbar(toolBar);
						return TRUE;
					}
					else
					{
						ReleaseICustToolbar(toolBar);
						return FALSE;
					}

				}
			}
			//fix mem leak
			ReleaseICustToolbar(toolBar);
		}
	}

	for (int i = 0; i < mButtons.Count(); i++)
	{
		if (mButtons[i] && mButtons[i]->ID() == buttonID)
		{
			return mButtons[i]->ButtonPtr()->IsChecked();
		}
	}

	return FALSE;
}

//updates all our check boxes UIs
void UnwrapCustomUI::UpdateCheckButtons()
{
	//loop through all the tool bars 
	for (int i = 0; i < mToolbars.Count(); i++)
	{
		//get the tool bar
		HWND toolBarHWND = mToolbars[i]->GetToolbarWindow();
		ICustToolbar *toolBar = GetICustToolbar(toolBarHWND); 
		if (toolBar)
		{
			HWND h = toolBar->GetHwnd();
			//get the elements
			int numItems = toolBar->GetNumItems();
			// see if it is a check box if so update it
			for (int j = 0; j < numItems; j++)
			{
				int id = toolBar->GetItemID(j);

				HWND toolHWND = toolBar->GetItemHwnd(id);

				//this ownly works with items tagged as other
				if (toolHWND)
				{
					BOOL currentlyEnabled = IsWindowEnabled(toolHWND);
					BOOL needsToBe = mMod->WtIsEnabled(id);
					if (currentlyEnabled != needsToBe)
						EnableWindow(toolHWND,needsToBe);
				}

				
				ICustButton *button = toolBar->GetICustButton(id);
				if (button)
				{
					BOOL currentState = mMod->WtIsChecked(id);
					BOOL checked = button->IsChecked();
					if (currentState != checked)
						button->SetCheck(currentState);
					 
					BOOL currentlyEnabled = button->IsEnabled();
					BOOL needsToBe = mMod->WtIsEnabled(id);
					if (currentlyEnabled != needsToBe)
						button->Enable(needsToBe);

					ReleaseICustButton(button);
				}
				else //it may be check box
				{
					HWND hwnd = toolBar->GetItemHwnd(id);
					BOOL currentState = mMod->WtIsChecked(id);
					BOOL checked = FALSE;
					if (SendMessage(hwnd,BM_GETCHECK,0,0) == BST_CHECKED)
						checked = TRUE;
					if (currentState != checked)
					{
						if (currentState)
							SendMessage(hwnd,BM_SETCHECK,BST_CHECKED,0);
						else
							SendMessage(hwnd,BM_SETCHECK,BST_UNCHECKED,0);
							
					}
					
				}
			}
			ReleaseICustToolbar(toolBar);
		}
	}

	for (int i = 0; i < mButtons.Count(); i++)
	{
		ICustButton *button = mButtons[i]->ButtonPtr();
		if (button)
		{
			int id = mButtons[i]->ID();
			BOOL currentState = mMod->WtIsChecked(id);
			BOOL checked = button->IsChecked();
			if (currentState != checked)
				button->SetCheck(currentState);

			BOOL currentlyEnabled = button->IsEnabled();
			BOOL needsToBe = mMod->WtIsEnabled(id);
			if (currentlyEnabled != needsToBe)
				button->Enable(needsToBe);

		}
	}
}


void UnwrapCustomUI::InvalidateToolbar(HWND parent)
{
	for (int i = 0; i < mToolbars.Count(); i++)
	{
		bool paint = false;
		if (parent == NULL)
			paint = true;
		else if (mToolbars[i]->GetParWindow() == parent)
			paint = true;
		if (paint)
			InvalidateRect(mToolbars[i]->GetToolbarWindow(),NULL,TRUE);

	}
}


int UnwrapCustomUI::MessageProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	BOOL iret = FALSE;
	switch (msg) 
	{
		case WM_INITDIALOG:
			{			
				break;
			}
		case CC_SPINNER_BUTTONDOWN:
			if ( (LOWORD(wParam) == ID_SPINNERU)  ||
				 (LOWORD(wParam) == ID_SPINNERV) ||
				 (LOWORD(wParam) == ID_SPINNERW) )
			{
				theHold.SuperBegin();				
				theHold.Begin();
				iret = TRUE;
				if (LOWORD(wParam) == ID_SPINNERU)
					mMod->InitAbsoluteTypeInChangedValue(0);
				else if (LOWORD(wParam) == ID_SPINNERW)
					mMod->InitAbsoluteTypeInChangedValue(1);
				else if (LOWORD(wParam) == ID_SPINNERV)
					mMod->InitAbsoluteTypeInChangedValue(2);

			}
			break;


		case CC_SPINNER_CHANGE:
			if (LOWORD(wParam) == ID_SOFTSELECTIONLIMIT_SPINNER) 
			{
				mMod->fnSetLimitSoftSelRange(mMod->GetUIManager()->GetSpinIValue(ID_SOFTSELECTIONLIMIT_SPINNER));
				mMod->RebuildDistCache();
				UpdateWindow(hWnd);
				mMod->InvalidateView();
				iret = TRUE;
			}			
			else if (LOWORD(wParam) == ID_SOFTSELECTIONSTR_SPINNER) 
			{
				mMod->RebuildDistCache();
				UpdateWindow(hWnd);
				mMod->InvalidateView();
				iret = TRUE;
			}
			else  if ( (LOWORD(wParam) == ID_SPINNERU)  ||
				       (LOWORD(wParam) == ID_SPINNERV) ||
					   (LOWORD(wParam) == ID_SPINNERW) )
			{
				if (!theHold.Holding()) {
					theHold.SuperBegin();
					theHold.Begin();
				}


				switch (LOWORD(wParam)) {
				case ID_SPINNERU: // IDC_UNWRAP_USPIN:
					mMod->tempWhich = 0;
					mMod->TypeInChanged(0);
					break;
				case ID_SPINNERV: // case IDC_UNWRAP_VSPIN:
					mMod->tempWhich = 1;
					mMod->TypeInChanged(1);
					break;
				case ID_SPINNERW: // case IDC_UNWRAP_WSPIN:
					mMod->tempWhich = 2;
					mMod->TypeInChanged(2);
					break;
				}

				UpdateWindow(hWnd);
				iret = TRUE;
			}
			break;

		case WM_CUSTEDIT_ENTER:
		case CC_SPINNER_BUTTONUP:
			if ( (LOWORD(wParam) == ID_GROUPSETDENSITY_SPINNER) ||
				(LOWORD(wParam) == ID_GROUPSETDENSITY_EDIT) )
			{
				float val = mMod->GetUIManager()->GetSpinFValue(ID_GROUPSETDENSITY_SPINNER);
				mMod->fnGroupSetTexelDensity(val);
			}
			else if ( (LOWORD(wParam) == ID_PLANARSPIN) ||
				(LOWORD(wParam) == ID_PLANAREDIT) )
			{
				float angle = mMod->GetUIManager()->GetSpinFValue(ID_PLANARSPIN);
				mMod->fnSetGeomPlanarModeThreshold(angle);
				//send macro message
				TSTR mstr = mMod->GetMacroStr(_T("modifiers[#unwrap_uvw].unwrap2.setGeomPlanarThreshold"));
				macroRecorder->FunctionCall(mstr, 1, 0,
					mr_float,mMod->fnGetGeomPlanarModeThreshold());
				macroRecorder->EmitScript();
			}
			else if ( (LOWORD(wParam) == ID_SOFTSELECTIONSTR_SPINNER)  ||
				      (LOWORD(wParam) == ID_SOFTSELECTIONSTR_EDIT)  )
			{
				mMod->RebuildDistCache();
				UpdateWindow(hWnd);
				mMod->InvalidateView();
				iret = TRUE;
			}
			else if ( (LOWORD(wParam) == ID_SOFTSELECTIONLIMIT_SPINNER) ||
				      (LOWORD(wParam) == ID_SOFTSELECTIONLIMIT_EDIT) )
			{
				float str = mMod->GetUIManager()->GetSpinFValue(ID_SOFTSELECTIONLIMIT_SPINNER);//iStr->GetFVal();
				TSTR mstr = mMod->GetMacroStr(_T("modifiers[#unwrap_uvw].unwrap.setFalloffDist"));
				macroRecorder->FunctionCall(mstr, 1, 0,
					mr_float,str);
				macroRecorder->EmitScript();
				mMod->RebuildDistCache();
				mMod->InvalidateView();
				UpdateWindow(hWnd);
				iret = TRUE;
			}
			else  if ( (LOWORD(wParam) == ID_SPINNERU)  ||
				(LOWORD(wParam) == ID_SPINNERV) ||
				(LOWORD(wParam) == ID_SPINNERW) ||
			 	(LOWORD(wParam) == ID_EDITU) ||
				(LOWORD(wParam) == ID_EDITV) ||
				(LOWORD(wParam) == ID_EDITW) 
				)
			{
				if (HIWORD(wParam) || msg==WM_CUSTEDIT_ENTER) {

					if (theHold.Holding())
					{
						theHold.Accept(GetString(IDS_PW_MOVE_UVW));
						theHold.SuperAccept(GetString(IDS_PW_MOVE_UVW));
					}


					if (mMod->tempWhich ==0)
					{
						TSTR mstr = mMod->GetMacroStr(_T("modifiers[#unwrap_uvw].unwrap.moveX"));
						macroRecorder->FunctionCall(mstr, 1, 0,
							mr_float,mMod->tempAmount);
					}
					else if (mMod->tempWhich ==1)
					{
						TSTR mstr = mMod->GetMacroStr(_T("modifiers[#unwrap_uvw].unwrap.moveY"));
						macroRecorder->FunctionCall(mstr, 1, 0,
							mr_float,mMod->tempAmount);
					}
					else if (mMod->tempWhich ==2)
					{
						TSTR mstr = mMod->GetMacroStr(_T("modifiers[#unwrap_uvw].unwrap.moveZ"));
						macroRecorder->FunctionCall(mstr, 1, 0,
							mr_float,mMod->tempAmount);
					}

					if (mMod->fnGetRelativeTypeInMode())
						mMod->SetupTypeins();


				} else {
					theHold.Cancel();
					theHold.SuperCancel();

					mMod->NotifyDependents(FOREVER,TEXMAP_CHANNEL,REFMSG_CHANGE);
					mMod->InvalidateView();
					UpdateWindow(hWnd);
					GetCOREInterface()->RedrawViews(GetCOREInterface()->GetTime());
					if (mMod->fnGetRelativeTypeInMode())
						mMod->SetupTypeins();

				}
				iret = TRUE;
			}
			break;


		case WM_COMMAND:
			{				
				iret = mMod->WtExecute(LOWORD(wParam),HIWORD(wParam),FALSE,TRUE);
			}
	}


	return iret;
}

int UnwrapCustomUI::LoadSingle(const MCHAR *iniFileName, int owner, HWND parentWindow, HWND toolbarHWND)
{
	//see if key exists if so load it
	bool done = false;
	int currentSection = 0;
	int ct = 0;
	bool found = false;
	TSTR section;
	while (!done)
	{
		
		section.printf(_T("Dialog_ToolBar_%d"),currentSection);
		MCHAR str[MAX_PATH];
		TSTR def(_T("DISCARD"));
		
		int res = GetPrivateProfileString(section,_T("Owner"),def,str,MAX_PATH,iniFileName);
		if ((res) && (_tcscmp(str,def))) 
		{
			int ownerTest;
			_stscanf(str,_T("%d"),&ownerTest);
			if (ownerTest == owner)
			{
				done = true;
				found = true;
			}
			else
				currentSection++;
		}
		else
			done = true;
	}
	if (found)
	{
		ToolBarFrame *bar = new ToolBarFrame(mhInstance,section,iniFileName,parentWindow,toolbarHWND);
		if (bar->Loaded())
		{
			if ( bar->Owner() == owner ) 
			{
				mToolbars.Append(1,&bar,10);
				ct++;
			}
			else
				delete bar;
		}
		else
		{
			delete bar;
		}
	}
	return ct;
}

int UnwrapCustomUI::Load(const MCHAR *iniFileName, int owner, HWND parentWindow)
{
	//see if key exists if so load it
	bool done = false;
	int currentSection = 0;
	int a = 0;
	int b = 99;
	if (owner == 100)
	{
		a = 100;
		b = 199;
	}
	else if (owner == 200)
	{
		a = 200;
		b = 299;
	}
	
	int ct = 0;
	while (!done)
	{
		TSTR section;
		section.printf(_T("Dialog_ToolBar_%d"),currentSection);
		ToolBarFrame *bar = new ToolBarFrame(mhInstance,section,iniFileName,parentWindow,NULL);
		currentSection++;
		if (bar->Loaded())
		{

			if ( (bar->Owner() >= a) && (bar->Owner() <= b) ) 
			{
			mToolbars.Append(1,&bar,10);
				ct++;
			}
			else
				delete bar;
		}
		else
		{
			delete bar;
			done = true;
		}
	}
	return ct;
}
void UnwrapCustomUI::Save(const MCHAR *iniFileName)
{
	for (int i = 0; i < mToolbars.Count(); i++)
	{
		TSTR section;
		section.printf(_T("Dialog_ToolBar_%d"),i);
		mToolbars[i]->Save(section,iniFileName);
	}


}

int  UnwrapCustomUI::GetCornerHeight(int corner)
{
	if (corner >= 0 && corner < 4)
		return mCornerHeight[corner];
	return 0;
}

void  UnwrapCustomUI::DisplayFloaters(BOOL show)
{
	for (int i = 0; i < mToolbars.Count(); i++)
	{
		if (mToolbars[i]->GetParWindow() == mMod->hDialogWnd)
		{
			
			if (show)
			{								
				mToolbars[i]->Show(true);				
			}
			else
			{
				mToolbars[i]->Show(false);				
			}

		}
		
	}
}
