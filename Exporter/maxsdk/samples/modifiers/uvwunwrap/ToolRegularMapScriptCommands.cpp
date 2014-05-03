

/*

Copyright [2010] Autodesk, Inc.  All rights reserved.

Use of this software is subject to the terms of the Autodesk license agreement 
provided at the time of installation or download, or which otherwise accompanies 
this software in either electronic or hard copy form.   

*/

#include "ToolRegularMap.h"
#include "unwrap.h"
#include "modsres.h"



INT_PTR CALLBACK UnwrapUnfoldMapFloaterDlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	UnwrapMod *mod = DLGetWindowLongPtr<UnwrapMod*>(hWnd);

	static ISpinnerControl *spinThreshold = NULL;

	static int bottomHeight = 0;

	switch (msg) {
		case WM_INITDIALOG:
			{

			

			mod = (UnwrapMod*)lParam;
			mod->fnRegularMapSetHWND(hWnd);

			DLSetWindowLongPtr(hWnd, lParam);
			::SetWindowContextHelpId(hWnd, idh_unwrap_peltmap);
			SendMessage(hWnd, WM_SETICON, ICON_SMALL, GetClassLongPtr(mod->ip->GetMAXHWnd(), GCLP_HICONSM)); // mjm - 3.12.99

			HWND hMethod = GetDlgItem(hWnd,IDC_LIMIT_COMBO);
			SendMessage(hMethod, CB_RESETCONTENT, 0, 0);
			SendMessage(hMethod, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) GetString(IDS_LIMIT_FULL));
			SendMessage(hMethod, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) GetString(IDS_LIMIT_PARTIAL));
			SendMessage(hMethod, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) GetString(IDS_LIMIT_ALWAYS));

			HWND hAutoFit = GetDlgItem(hWnd,IDC_AUTOFIT_COMBO);
			SendMessage(hAutoFit, CB_RESETCONTENT, 0, 0);
			SendMessage(hAutoFit, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) GetString(IDS_AUTOFIT_NONE));
			SendMessage(hAutoFit, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) GetString(IDS_AUTOFIT_LAST));
			SendMessage(hAutoFit, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR) GetString(IDS_AUTOFIT_ALL));


			spinThreshold = SetupFloatSpinner(
				hWnd,IDC_UNWRAP_AUTOWELD_THRESHOLD_SPIN,IDC_UNWRAP_AUTOWELD_THRESHOLD,
				0.0f,1.0f,mod->fnRegularMapGetAutoWeldThreshold());		

			ICustButton *iButton = GetICustButton(GetDlgItem(hWnd, IDC_UNWRAP_STARTNEWCLUSTER));
			if (iButton)
			{
				iButton->SetType(CBT_CHECK);
				ReleaseICustButton(iButton);
			}

			mod->fnRegularMapSetPickStartFace(TRUE);
			mod->fnRegularMapUpdateUI();
			break;
			}
		case WM_SYSCOMMAND:
			if ((wParam & 0xfff0) == SC_CONTEXTHELP) 
			{
				MaxSDK::IHelpSystem::GetInstance()->ShowProductHelpForTopic(idh_unwrap_peltmap); 
			}
			return FALSE;
			break;

		case CC_SPINNER_CHANGE:
			{
				mod->fnRegularMapSetAutoWeldThreshold(spinThreshold->GetFVal());

				TSTR mstr = _T("$.modifiers[#unwrap_uvw].unwrap6.RegularMapSetAutoWeldThreshold");
				macroRecorder->FunctionCall(mstr, 1, 0,mr_float,mod->fnRegularMapGetAutoWeldThreshold());
				macroRecorder->EmitScript();

				break;
			}
		case WM_DESTROY:
			mod->fnRegularMapSetHWND(NULL);
			mod->fnRegularMapEnd();
			ReleaseISpinner(spinThreshold);
			spinThreshold = NULL;
			break;
		case WM_CLOSE:
			{
				EndDialog(hWnd,1);
				break;
			}

		case WM_COMMAND:
			switch (LOWORD(wParam)) 
			{
			case IDC_UNWRAP_STARTNEWCLUSTER:
				{
					ICustButton *iButton = GetICustButton(GetDlgItem(hWnd, IDC_UNWRAP_STARTNEWCLUSTER));
					if (iButton)
					{						
						if (mod->fnRegularMapGetPickStartFace())
						{
							iButton->SetCheck(FALSE);
							mod->fnRegularMapSetPickStartFace(FALSE);
						}
						else
						{
							iButton->SetCheck(TRUE);
							mod->fnRegularMapSetPickStartFace(TRUE);

						}
						ReleaseICustButton(iButton);
					}
				break;
				}
			case IDC_LIMIT_COMBO:
				{
					if (HIWORD(wParam)== CBN_SELCHANGE)
					{	
						HWND hMethod = GetDlgItem(hWnd,IDC_LIMIT_COMBO);
						int limit = SendMessage(hMethod, CB_GETCURSEL, 0L, 0);

						TSTR mstr = _T("$.modifiers[#unwrap_uvw].unwrap6.RegularMapSetLimit");
						macroRecorder->FunctionCall(mstr, 1, 0,mr_int,limit);
						macroRecorder->EmitScript();

						if (limit == 0)
							mod->fnRegularMapSetLimit(kRegular);
						else if (limit == 1)
							mod->fnRegularMapSetLimit(kPartial);
						else if (limit == 2)
							mod->fnRegularMapSetLimit(kAbnormal);

					}
				break;
				}
			case IDC_AUTOFIT_COMBO: 				
				{
					if (HIWORD(wParam)== CBN_SELCHANGE)
					{	
						HWND hAutofit = GetDlgItem(hWnd,IDC_AUTOFIT_COMBO);
						int autofit = SendMessage(hAutofit, CB_GETCURSEL, 0L, 0);

						TSTR mstr = _T("$.modifiers[#unwrap_uvw].unwrap6.RegularMapSetAutoFit");
						macroRecorder->FunctionCall(mstr, 1, 0,mr_int,autofit);
						macroRecorder->EmitScript();

						mod->fnRegularMapSetAutoFit(autofit);
					}
					break;
				}
			case IDC_AUTOWELD_CHECK: 				
				{
					BOOL value = GetCheckBox(hWnd,IDC_AUTOWELD_CHECK);

					TSTR mstr = _T("$.modifiers[#unwrap_uvw].unwrap6.RegularMapSetAutoWeld");
					macroRecorder->FunctionCall(mstr, 1, 0,mr_bool,value);
					macroRecorder->EmitScript();


					mod->fnRegularMapSetAutoWeld(value);
					break;
				}
			case IDC_SINGLESTEP_CHECK: 				
				{
					BOOL value = GetCheckBox(hWnd,IDC_SINGLESTEP_CHECK);

					TSTR mstr = _T("$.modifiers[#unwrap_uvw].unwrap6.RegularMapSetSingleStep");
					macroRecorder->FunctionCall(mstr, 1, 0,mr_bool,value);
					macroRecorder->EmitScript();

					mod->fnRegularMapSetSingleStep(value);
					break;
				}
			case IDC_NORMALIZE_CHECK: 				
				{
					BOOL value = GetCheckBox(hWnd,IDC_NORMALIZE_CHECK);

					TSTR mstr = _T("$.modifiers[#unwrap_uvw].unwrap6.RegularMapSetNormalize");
					macroRecorder->FunctionCall(mstr, 1, 0,mr_bool,value);
					macroRecorder->EmitScript();					

					mod->fnRegularMapSetNormalize(value);
					break;
				}

			case IDC_RESETFACES:
				{
					TSTR mstr = _T("$.modifiers[#unwrap_uvw].unwrap6.RegularMapResetFaces");
					macroRecorder->FunctionCall(mstr, 0, 0);
					macroRecorder->EmitScript();


					mod->fnRegularMapResetFaces();
					break;
				}
			case IDC_ADVANCESELECTED:
				{

					TSTR mstr = _T("$.modifiers[#unwrap_uvw].unwrap6.RegularMapAdvanceSelected");
					macroRecorder->FunctionCall(mstr, 1, 0,mr_bool,mod->fnRegularMapGetSingleStep());
					macroRecorder->EmitScript();
					

					mod->fnRegularMapAdvanceSelected(mod->fnRegularMapGetSingleStep());
					mod->fnRegularMapFitView();
					break;
				}
			case IDC_ADVANCEU:
				{
					TSTR mstr = _T("$.modifiers[#unwrap_uvw].unwrap6.RegularMapAdvanceUV");
					macroRecorder->FunctionCall(mstr, 5, 0,
												mr_bool,true,
												mr_bool,false,
												mr_bool,true,
												mr_bool,false,
												mr_bool,mod->fnRegularMapGetSingleStep());
					macroRecorder->EmitScript();

					mod->fnRegularMapAdvanceUV(true,false,true,false,mod->fnRegularMapGetSingleStep());
					mod->fnRegularMapFitView();
					break;
				}
			case IDC_ADVANCEV:
				{
					TSTR mstr = _T("$.modifiers[#unwrap_uvw].unwrap6.RegularMapAdvanceUV");
					macroRecorder->FunctionCall(mstr, 5, 0,
						mr_bool,false,
						mr_bool,true,
						mr_bool,false,
						mr_bool,true,						
						mr_bool,mod->fnRegularMapGetSingleStep());
					mod->fnRegularMapAdvanceUV(false,true,false,true,mod->fnRegularMapGetSingleStep());
					mod->fnRegularMapFitView();
					break;
				}
			case IDC_COMMIT:
				{
					mod->fnRegularMapEnd();
					mod->fnSetMapMode(UNFOLDMAP);
					break;
				}
			}

		default:
			return FALSE;
		}
	return TRUE;
	}



void UnwrapMod::fnRegularMapExpand(int expandBy) 
{
	theHold.Begin();
	mRegularMap.Hold();

	ClearAFlag(A_HELD);
	HoldPointsAndFaces();


	theHold.Accept(GetString(IDS_UNFOLDMAP));

	mRegularMap.Expand((RegularMapExpansions)expandBy);
}


void UnwrapMod::fnRegularMapStart(INode* node, BOOL bringUpUI)
{

	if (bringUpUI)
	{
		HWND hwnd = CreateDialogParam(	hInstance,
							MAKEINTRESOURCE(IDD_UNFOLDMAP_DIALOG),
							GetCOREInterface()->GetMAXHWnd(),
							UnwrapUnfoldMapFloaterDlgProc,
							(LPARAM)this );
		mRegularMap.SetPos();
		mRegularMap.FillOutUI();

		ShowWindow(hwnd ,SW_SHOW);
		
	}
	MeshTopoData *md = GetMeshTopoData(node);
	mRegularMap.Init(this,md);

}

void UnwrapMod::fnRegularMapEnd()
{
	mRegularMap.Free();
	if (mRegularMap.GetHWND())
	{
		mRegularMap.SavePos();
		DestroyWindow(mRegularMap.GetHWND());
	}
	mRegularMap.SetHWND(NULL);
}
void UnwrapMod::fnRegularMapReset()
{
	mRegularMap.Free();
}

void UnwrapMod::fnRegularMapUpdateUI()
{

	mRegularMap.FillOutUI();
}

BOOL UnwrapMod::fnRegularMapGetNormalize()
{
	if (mRegularMap.GetNormalize())
		return TRUE;
	return FALSE;

}
void UnwrapMod::fnRegularMapSetNormalize(BOOL normalize)
{
	if (normalize)
		mRegularMap.SetNormalize(true);
	else
		mRegularMap.SetNormalize(false);
}


BOOL UnwrapMod::fnRegularMapGetSingleStep()
{
	if (mRegularMap.GetSingleStep())
		return TRUE;
	return FALSE;
}
void UnwrapMod::fnRegularMapSetSingleStep(BOOL singleStep)
{
	if (singleStep)
		mRegularMap.SetSingleStep(true);
	else
		mRegularMap.SetSingleStep(false);
}


BOOL UnwrapMod::fnRegularMapGetAutoWeld()
{
	if (mRegularMap.GetAutoWeld())
		return TRUE;
	return FALSE;
}
void UnwrapMod::fnRegularMapSetAutoWeld(BOOL autoWeld)
{
	if (autoWeld)
		mRegularMap.SetAutoWeld(true);
	else
		mRegularMap.SetAutoWeld(false);

}

float UnwrapMod::fnRegularMapGetAutoWeldThreshold()
{
	return mRegularMap.GetAutoWeldLimit();
}
void UnwrapMod::fnRegularMapSetAutoWeldThreshold(float autoWeldThreshold)
{
	mRegularMap.SetAutoWeldLimit(autoWeldThreshold);
}

void UnwrapMod::fnRegularMapSetHWND(HWND hwnd)
{
	mRegularMap.SetHWND(hwnd);
}
HWND UnwrapMod::fnRegularMapGetHWND()
{
	return mRegularMap.GetHWND();
}


float UnwrapMod::fnRegularMapGetIconSize()
{
	return mRegularMap.GetIconSize();
}
void UnwrapMod::fnRegularMapSetIconSize(float iconSize)
{
	mRegularMap.SetIconSize(iconSize);
}


int UnwrapMod::fnRegularMapGetLimit()
{
	if (mRegularMap.GetLimit() == kRegular)
		return 0;
	else if (mRegularMap.GetLimit() == kPartial)
		return 1;
	else if (mRegularMap.GetLimit() == kAbnormal)
		return 2;
	return 0;
}
void UnwrapMod::fnRegularMapSetLimit(int limit)
{
	if (limit == 0)
		mRegularMap.SetLimit(kRegular);
	else if (limit == 1)
		mRegularMap.SetLimit(kPartial);
	else if (limit == 2)
		mRegularMap.SetLimit(kAbnormal);
}

int UnwrapMod::fnRegularMapGetAutoFit()
{
	return mRegularMap.GetAutoFit();
}
void UnwrapMod::fnRegularMapSetAutoFit(int autofit)
{

	mRegularMap.SetAutoFit(autofit);
}

void UnwrapMod::fnRegularMapResetFaces()
{
	theHold.Begin();
	mRegularMap.Hold();
	theHold.Accept(GetString(IDS_UNFOLDMAP));

	mRegularMap.ResetFaces();
}


void UnwrapMod::fnRegularMapStartNewCluster(INode *node, int faceIndex)
{

	MeshTopoData *md = NULL;
	for (int ldID = 0; ldID < mMeshTopoData.Count(); ldID++)
	{
		if (node == mMeshTopoData.GetNode(ldID))
			md = mMeshTopoData[ldID];
	}


	bool recomputeLocalData = false;
	if (md != mRegularMap.GetLocalData())
		recomputeLocalData = true;


	theHold.Begin();
	mRegularMap.Hold(recomputeLocalData);
	ClearAFlag(A_HELD);
	HoldPointsAndFaces();	
	theHold.Accept(GetString(IDS_UNFOLDMAP));

	if (recomputeLocalData)
		mRegularMap.Init(this,md);

	mRegularMap.StartNewCluster(faceIndex);
	fnFit();
}

void UnwrapMod::fnRegularMapAdvanceUV(BOOL uPosDir, BOOL vPosDir, BOOL uNegDir, BOOL vNegDir, BOOL singleStep)
{
	
	theHold.Begin();
	mRegularMap.Hold();

	ClearAFlag(A_HELD);
	HoldPointsAndFaces();


	theHold.Accept(GetString(IDS_UNFOLDMAP));

	mRegularMap.Advance(uPosDir, vPosDir, uNegDir, vNegDir, FALSE, singleStep);
}
void UnwrapMod::fnRegularMapAdvanceSelected(BOOL singleStep)
{

	
	theHold.Begin();
	mRegularMap.Hold();

	ClearAFlag(A_HELD);
	HoldPointsAndFaces();

	theHold.Accept(GetString(IDS_UNFOLDMAP));

	mRegularMap.Advance(FALSE, FALSE, FALSE, FALSE, TRUE, singleStep);
}

MeshTopoData* UnwrapMod::fnRegularMapGetLocalData()
{
	return mRegularMap.GetLocalData();
}


BOOL UnwrapMod::fnRegularMapGetPickStartFace()
{
	if (mRegularMap.GetPickStartFace())
		return TRUE;
	return FALSE;
}
void UnwrapMod::fnRegularMapSetPickStartFace(BOOL start)
{
	


	if (start)
		mRegularMap.SetPickStartFace(true);
	else
		mRegularMap.SetPickStartFace(false);
	
	fnRegularMapUpdateUI();


}

void UnwrapMod::fnRegularMapFitView()
{
	mRegularMap.FitView(fnRegularMapGetAutoFit());
}

