
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

#include "Unwrap.h"
#include "modsres.h"

void UnwrapMod::UpdatePivot()
{
	if (freeFormMode)
		freeFormMode->dragging = TRUE;
	RebuildFreeFormData();
	if (freeFormMode)
		freeFormMode->dragging = FALSE;


	selCenter = freeFormBounds.Center();

	Point2 prect[2];
	float xzoom, yzoom;
	int width,height;
	ComputeZooms(hView,xzoom,yzoom,width,height);

	int i1, i2;
	GetUVWIndices(i1, i2);

	//draw gizmo bounds
	prect[0] = UVWToScreen(freeFormBounds.pmin,xzoom,yzoom,width,height);
	prect[1] = UVWToScreen(freeFormBounds.pmax,xzoom,yzoom,width,height);

	int corners[4];
	if ((i1 == 0) && (i2 == 1))
	{
		corners[0] = 0;
		corners[1] = 1;
		corners[2] = 2;
		corners[3] = 3;
	}
	else if ((i1 == 1) && (i2 == 2)) 
	{
		corners[0] = 1;//1,2,5,6
		corners[1] = 3;
		corners[2] = 5;
		corners[3] = 7;
	}
	else if ((i1 == 0) && (i2 == 2))
	{
		corners[0] = 0;
		corners[1] = 1;
		corners[2] = 4;
		corners[3] = 5;
	}

	for (int i = 0; i < 4; i++)
	{
		int index = corners[i];
		freeFormCorners[i] = freeFormBounds[index];
	}		

	freeFormPivotScreenSpace = UVWToScreen(selCenter,xzoom,yzoom,width,height);
	
}

BOOL UnwrapMod::WtExecute(int id, int highWord, BOOL override, BOOL emmitMacroScript)
{
	BOOL iret = FALSE;


	//we only execute these when in the dialog is active since 
	//both share the same 
	if (floaterWindowActive)
	{
	switch (id)
	{


		//these are our dialog command modes
		case ID_MOVE:
		case ID_ROTATE:
		case ID_SCALE:
		case ID_WELD:
		case ID_PAN:
		case ID_ZOOMTOOL:
		case ID_ZOOMREGION:
		case ID_FREEFORMMODE:
		case ID_SKETCHMODE:
		case ID_TV_PAINTSELECTMODE:
			SetMode(id);
			iret = TRUE;
			break;				

		case ID_TV_INCSEL:
			fnExpandSelection();
			iret = TRUE;
			break;
		case ID_TV_DECSEL:
			fnContractSelection();
			iret = TRUE;
			break;


		case ID_LOCK:
			fnLock();
			iret = TRUE;
			break;			

		case ID_FIT:
			fnFit();
			iret = TRUE;
			break;			
		case ID_FITSELECTED:
			fnFitSelected();
			iret = TRUE;
			break;			
		case ID_FITSELECTEDELEMENT:
			fnFrameSelectedElement();
			iret = TRUE;
			break;

		}
	}
	

	{


	switch (id)
	{
	case ID_TOOL_WELDDROPDOWN:
		{
			int weldType = mUIManager.GetFlyOut(ID_TOOL_WELDDROPDOWN);
			if (weldType == 0)
				fnWeldSelected();
			if (weldType == 1)
				fnWeldAllShared();
			if (weldType == 2)
				fnWeldSelectedShared();

			iret = TRUE;
		}
		break;
	case ID_FREEFORMMODE:
		fnSetFreeFormMode(TRUE);
		iret = TRUE;
		break;

	case ID_MOVEH:
		fnMoveH();
		iret = TRUE;
		break;
	case ID_MOVEV:
		fnMoveV();
		iret = TRUE;
		break;

	case ID_SCALEH:
		fnScaleH();
		iret = TRUE;
		break;
	case ID_SCALEV:
		fnScaleV();
		iret = TRUE;
		break;

	case ID_MIRROR:
		if (mUIManager.GetFlyOut(ID_MIRROR) == 0)
			fnMirrorH();
		else if (mUIManager.GetFlyOut(ID_MIRROR) == 1)
			fnMirrorV();
		else if (mUIManager.GetFlyOut(ID_MIRROR) == 2)
			fnFlipH();
		else if (mUIManager.GetFlyOut(ID_MIRROR) == 3)
			fnFlipV();
		iret = TRUE;
		break;

	case ID_MIRRORH:
		fnMirrorH();
		iret = TRUE;
		break;
	case ID_MIRRORV:
		fnMirrorV();
		iret = TRUE;
		break;

	case ID_FLIPH:
		fnFlipH();
		iret = TRUE;
		break;
	case ID_FLIPV:
		fnFlipV();
		iret = TRUE;
		break;
	case ID_BREAK:
		fnBreakSelected();
		iret = TRUE;
		break;
/*
	case ID_WELD:
		fnWeld();
		iret = TRUE;
		break;
*/
	case ID_WELD_SELECTED:
		fnWeldSelected();
		iret = TRUE;
		break;
	case ID_BREAKBYSUBOBJECT:
		fnBreakSelected();
		iret = TRUE;
		break;
	case ID_FALLOFF:
		{
			falloff = mUIManager.GetFlyOut(ID_FALLOFF);
			TSTR mstr = GetMacroStr(_T("modifiers[#unwrap_uvw].unwrap.setFalloffType"));
			macroRecorder->FunctionCall(mstr, 1, 0,
				mr_int,falloff+1);
			macroRecorder->EmitScript();
			RebuildDistCache();
			InvalidateView();
			iret = TRUE;
			break;
		}
	case ID_FALLOFF_SPACE:
		{
			falloffSpace = mUIManager.GetFlyOut(ID_FALLOFF_SPACE);
			TSTR mstr = GetMacroStr(_T("modifiers[#unwrap_uvw].unwrap.setFalloffSpace"));
			macroRecorder->FunctionCall(mstr, 1, 0,
				mr_int,falloffSpace+1);
			macroRecorder->EmitScript();
			RebuildDistCache();
			InvalidateView();
			iret = TRUE;
			break;
		}



	case ID_UPDATEMAP:
		fnUpdatemap();
		iret = TRUE;
		break;			
	case ID_OPTIONS:
		fnOptions();
		iret = TRUE;
		break;	
	case ID_UVW:
		{
			uvw = mUIManager.GetFlyOut(ID_UVW);
			TSTR mstr = GetMacroStr(_T("modifiers[#unwrap_uvw].unwrap.setUVSpace"));
			macroRecorder->FunctionCall(mstr, 1, 0,
				mr_int, uvw+1);
			macroRecorder->EmitScript();

			InvalidateView();
			iret = TRUE;
			break;
		}
	case ID_PROPERTIES:
		{
			SetFocus(hDialogWnd);
			TSTR mstr = GetMacroStr(_T("modifiers[#unwrap_uvw].unwrap.options"));
			macroRecorder->FunctionCall(mstr, 0, 0);
			macroRecorder->EmitScript();

			PropDialog();
			iret = TRUE;
			break;
		}



	case ID_SHOWMAP:
		{
			fnShowMap();

			TSTR mstr = GetMacroStr(_T("modifiers[#unwrap_uvw].unwrap.DisplayMap"));
			macroRecorder->FunctionCall(mstr, 1, 0,
				mr_bool, showMap);
			macroRecorder->EmitScript();			
			InvalidateView();

			iret = TRUE;
			break;
		}

	case ID_TEXTURE_COMBO:
		{
			if ( highWord == CBN_SELCHANGE ) 
			{
				//get count
				SetFocus(hDialogWnd); //kinda hack, once the user has selected something we immediatetly change focus so he middle mouse scroll does not cycle the drop list
				int ct = SendMessage( hTextures, CB_GETCOUNT, 0, 0 );
				int res = SendMessage( hTextures, CB_GETCURSEL, 0, 0 );
				//pick a new map
				if (res == (ct -4))
				{
					PickMap();
					SetupImage();
					//			mod->UpdateListBox();			
					SendMessage( hTextures, CB_SETCURSEL, CurrentMap, 0 );
				}
				if (res == (ct -3))
				{
					DeleteFromMaterialList(CurrentMap);
					SetupImage();
					UpdateListBox();
					SendMessage( hTextures, CB_SETCURSEL, CurrentMap, 0 );
				}
				if (res == (ct -2))
				{
					ResetMaterialList();
					UpdateListBox();
					SetupImage();
					SendMessage( hTextures, CB_SETCURSEL, CurrentMap, 0 );
				}
				else if (res < (ct-4))
					//select a current
				{
					//			mod->CurrentMap = res;
					if ((res >= 0) && (res < dropDownListIDs.Count()))
					{
						CurrentMap = dropDownListIDs[res];

						if (CurrentMap == 0)
							ShowCheckerMaterial(TRUE);
						else ShowCheckerMaterial(FALSE);

						SetupImage();
					}


					TSTR mstr = GetMacroStr(_T("modifiers[#unwrap_uvw].unwrap.setCurrentMap"));
					macroRecorder->FunctionCall(mstr, 1, 0,
						mr_int, CurrentMap+1);
					macroRecorder->EmitScript();

				}
			}
			iret = TRUE;
		}
		break;

	case ID_ABSOLUTETYPEIN:
		{
			absoluteTypeIn = !absoluteTypeIn;
			if (emmitMacroScript)
			{
				TSTR mstr = GetMacroStr(_T("modifiers[#unwrap_uvw].unwrap4.SetRelativeTypeIn"));
				macroRecorder->FunctionCall(mstr, 1, 0,
					mr_bool, absoluteTypeIn);
				macroRecorder->EmitScript();
			}
			iret = TRUE;
			break;
		}

	case ID_LOCKSELECTED:
		{
			lockSelected = !lockSelected;
			if (emmitMacroScript)
			{
				TSTR mstr = GetMacroStr(_T("modifiers[#unwrap_uvw].unwrap.lock"));
				macroRecorder->FunctionCall(mstr, 0, 0);
				macroRecorder->EmitScript();
			}
			iret = TRUE;
			break;
		}
	case ID_FILTER_SELECTEDFACES:
		{
			
			filterSelectedFaces = !filterSelectedFaces;
			BuildFilterSelectedFacesData();
			InvalidateView();
			UpdateWindow(hDialogWnd);
			if (emmitMacroScript)
			{
				TSTR mstr = GetMacroStr(_T("modifiers[#unwrap_uvw].unwrap.filterselected"));
				macroRecorder->FunctionCall(mstr, 0, 0);
				macroRecorder->EmitScript();
			}
			iret = TRUE;
			break;
		}
	case ID_FILTER_MATID:
		if ( highWord == CBN_SELCHANGE ) {
			//get count
			matid = SendMessage( hMatIDs, CB_GETCURSEL, 0, 0 )-1;
			SetMatFilters();

			if (emmitMacroScript)
			{
				TSTR mstr = GetMacroStr(_T("modifiers[#unwrap_uvw].unwrap.setMatID"));
				macroRecorder->FunctionCall(mstr, 1, 0,
					mr_int, matid+2);
				macroRecorder->EmitScript();

			}

			UpdateListBox();
			if (dropDownListIDs.Count() >= 2)
			{
				CurrentMap = dropDownListIDs[1];
				SendMessage( hTextures, CB_SETCURSEL, 1, 0 );
			}
			else
			{
				SendMessage( hTextures, CB_SETCURSEL, 0, 0 );
				CurrentMap = dropDownListIDs[0];
			}
			SetupImage();
			InvalidateView();
			SetFocus(hDialogWnd);  //kinda hack, once the user has selected something we immediatetly change focus so he middle mouse scroll does not cycle the drop list
		}
		iret = TRUE;
		break;

	case ID_HIDE:
		if (mUIManager.GetFlyOut(ID_HIDE) == 0)
			fnHide();
		else
			fnUnhide();
		iret = TRUE;
		break;			
	case ID_UNHIDE:
		fnUnhide();
		iret = TRUE;
		break;			
	case ID_FREEZE:
		if (mUIManager.GetFlyOut(ID_FREEZE) == 0)
			fnFreeze();
		else
			fnThaw();
		iret = TRUE;
		break;			
	case ID_UNFREEZE:
		fnThaw();
		iret = TRUE;
		break;			
	case ID_FILTERSELECTED:
		fnFilterSelected();
		iret = TRUE;
		break;			

	case ID_ZOOMEXTENT:
		{
			zoomext = mUIManager.GetFlyOut(ID_ZOOMEXTENT);
			//watje tile
			tileValid = FALSE;

			if (zoomext == 0)
			{
				ZoomExtents();
				if (emmitMacroScript)
				{				
				TSTR mstr = GetMacroStr(_T("modifiers[#unwrap_uvw].unwrap.fit"));
				macroRecorder->FunctionCall(mstr, 0, 0);
				macroRecorder->EmitScript();
				}
			}
			else if (zoomext == 1)
			{
				ZoomSelected();
				if (emmitMacroScript)
				{
					TSTR mstr = GetMacroStr(_T("modifiers[#unwrap_uvw].unwrap.fitselected"));
					macroRecorder->FunctionCall(mstr, 0, 0);
					macroRecorder->EmitScript();
				}
			}
			else if (zoomext == 2)
			{
				FrameSelectedElement();
				if (emmitMacroScript)
				{
					TSTR mstr = GetMacroStr(_T("modifiers[#unwrap_uvw].unwrap5.fitSelectedElement"));
					macroRecorder->FunctionCall(mstr, 0, 0);
					macroRecorder->EmitScript();
				}
			}


			
			iret = TRUE;
			break;
		}

	case ID_SNAP:
		{
			if (mUIManager.GetFlyOut(ID_SNAP) == 1)
			{
				
				pixelSnap = !pixelSnap;
				gridSnap = FALSE;
			}
			else
			{
				pixelSnap = FALSE;
				gridSnap = !gridSnap;
			}

			if (emmitMacroScript)
			{			
				TSTR mstr = GetMacroStr(_T("modifiers[#unwrap_uvw].unwrap.snap"));
				macroRecorder->FunctionCall(mstr, 0, 0);
				macroRecorder->EmitScript();
			}			
		}
		iret = TRUE;
		break;			

	case ID_SOFTSELECTION:
		{
			BOOL softSel = !fnGetEnableSoftSelection();
			fnSetEnableSoftSelection(softSel);
			iret = TRUE;
			break;
		}
	case ID_FACETOVERTEX:
		iret = TRUE;
		fnGetSelectionFromFace();
		break;			

	case ID_DETACH:
		fnDetachEdgeVerts();
		iret = TRUE;
		break;


	case ID_GETFACESELFROMSTACK:
		fnGetFaceSelFromStack();
		iret = TRUE;
		break;
	case ID_STITCH:
		fnStitchVertsNoParams();
		iret = TRUE;
		break;
	case ID_STITCHDIALOG:
		fnStitchVertsDialog();
		iret = TRUE;
		break;
	case ID_STITCHSOURCE:
		{
		float bias = fStitchBias;
		fStitchBias = 0.0f;
		BOOL align = bStitchAlign;
		bStitchAlign = false;
		fnStitchVertsNoParams();
		fStitchBias = bias;
		bStitchAlign = align;
		}
		iret = TRUE;
		break;
	case ID_STITCHTARGET:
		{
		float bias = fStitchBias;
		fStitchBias = 1.0f;
		BOOL align = bStitchAlign;
		bStitchAlign = false;
		fnStitchVertsNoParams();
		fStitchBias = bias;
		bStitchAlign = align;
		iret = TRUE;
		}
		break;
	case ID_STITCHAVERAGE:
		{
			float bias = fStitchBias;
			BOOL align = bStitchAlign;
			bStitchAlign = false;
			fStitchBias = 0.5f;
			fnStitchVertsNoParams();
			fStitchBias = bias;
			bStitchAlign = align;
		}
		iret = TRUE;
		break;
	case ID_STITCHBUTTONS:
		{
			int flyOut = mUIManager.GetFlyOut(ID_STITCHBUTTONS);
			if (flyOut == 0)
				fnStitchVertsNoParams();
			else
			{
				fnStitchVertsDialog();
				mUIManager.SetFlyOut(ID_STITCHBUTTONS,0,FALSE);
			}
			iret = TRUE;
		}

		iret = TRUE;
		break;


	case ID_NORMALMAP:
		fnNormalMapNoParams();
		iret = TRUE;
		break;
	case ID_NORMALMAPDIALOG:
		fnNormalMapDialog();
		iret = TRUE;
		break;

	case ID_FLATTENMAP:
		{
			BOOL holdNormalize = flattenNormalize;
			float holdAngle = flattenAngleThreshold;
			float holdSpacing = flattenSpacing;
			BOOL holdRotate = flattenRotate;
			BOOL holdCollapse = flattenCollapse;
			

			flattenAngleThreshold = 60.0f;
			flattenNormalize = TRUE;
			flattenSpacing = 0.02f;
			flattenRotate = TRUE;
			flattenCollapse = FALSE;

			fnFlattenMapNoParams();

			flattenNormalize = holdNormalize;
			flattenAngleThreshold = holdAngle;
			flattenSpacing = holdSpacing;
			flattenRotate = holdRotate;
			flattenCollapse = holdCollapse;

			iret = TRUE;
		}
		break;
	case ID_FLATTENMAPDIALOG:
		fnFlattenMapDialog();
		iret = TRUE;
		break;
	case ID_FLATTENBYMATID:
		fnFlattenMapByMatIDNoParams();
		iret = TRUE;
		break;
	case ID_UNFOLDMAP:
		fnUnfoldSelectedPolygonsNoParams();
		iret = TRUE;
		break;
	case ID_UNFOLDMAPDIALOG:
		fnUnfoldSelectedPolygonsDialog();
		iret = TRUE;
		break;

	case ID_FLATTENBUTTONS:
		{
			int flyOut = mUIManager.GetFlyOut(ID_FLATTENBUTTONS);
			if (flyOut == 0)
				fnFlattenMapNoParams();
			else
			{
				fnFlattenMapDialog();
				mUIManager.SetFlyOut(ID_FLATTENBUTTONS,0,FALSE);
			}
			iret = TRUE;
		}

		iret = TRUE;
		break;



	case ID_COPY:
		fnCopy();
		iret = TRUE;
		break;

	case ID_PASTE:
		fnPaste(TRUE);
		iret = TRUE;
		break;
	case ID_PASTEINSTANCE:
		fnPasteInstance();
		iret = TRUE;
		break;

	case ID_LIMITSOFTSEL:
		{
			BOOL limit = fnGetLimitSoftSel();

			if (limit)
				fnSetLimitSoftSel(FALSE);
			else fnSetLimitSoftSel(TRUE);
			iret = TRUE;
			break;
		}

	case ID_LIMITSOFTSEL1:
		fnSetLimitSoftSelRange(1);
		iret = TRUE;
		break;
	case ID_LIMITSOFTSEL2:
		fnSetLimitSoftSelRange(2);
		iret = TRUE;
		break;
	case ID_LIMITSOFTSEL3:
		fnSetLimitSoftSelRange(3);
		iret = TRUE;
		break;
	case ID_LIMITSOFTSEL4:
		fnSetLimitSoftSelRange(4);
		iret = TRUE;
		break;
	case ID_LIMITSOFTSEL5:
		fnSetLimitSoftSelRange(5);
		iret = TRUE;
		break;
	case ID_LIMITSOFTSEL6:
		fnSetLimitSoftSelRange(6);
		iret = TRUE;
		break;
	case ID_LIMITSOFTSEL7:
		fnSetLimitSoftSelRange(7);
		iret = TRUE;
		break;
	case ID_LIMITSOFTSEL8:
		fnSetLimitSoftSelRange(8);
		iret = TRUE;
		break;
	case ID_GEOMELEMMODE:
		{
			BOOL mode = fnGetGeomElemMode();
			if (mode)
				fnSetGeomElemMode(FALSE);
			else fnSetGeomElemMode(TRUE);
			iret = TRUE;
			break;
		}
	case ID_PLANARMODE:
		{
			BOOL pmode = fnGetGeomPlanarMode();

			if (pmode)
				fnSetGeomPlanarMode(FALSE);
			else fnSetGeomPlanarMode(TRUE);
			iret = TRUE;
			break;
		}
	case ID_IGNOREBACKFACE:
		{
			BOOL backface = fnGetBackFaceCull();

			if (backface)
				fnSetBackFaceCull(FALSE);
			else fnSetBackFaceCull(TRUE);
			iret = TRUE;
			break;
		}
	case ID_ELEMENTMODE:
		{
			BOOL mode = fnGetTVElementMode();

			if (mode)
				fnSetTVElementMode(FALSE);
			else fnSetTVElementMode(TRUE);
			iret = TRUE;
			break;
		}
	case ID_GEOM_ELEMENT:
		{
			BOOL mode = fnGetGeomElemMode();

			if (mode)
				fnSetGeomElemMode(FALSE);
			else fnSetGeomElemMode(TRUE);
			iret = TRUE;
			break;
		}
	case ID_GEOMEXPANDFACESEL:
		if (ip)
		{
			if (ip->GetSubObjectLevel() == 1)
				fnGeomExpandVertexSel();
			else if  (ip->GetSubObjectLevel() == 2)
				fnGeomExpandEdgeSel();
			else if  (ip->GetSubObjectLevel() == 3)
				fnGeomExpandFaceSel();

			iret = TRUE;
		}
		break;
	case ID_GEOMCONTRACTFACESEL:
		if (ip)
		{
			if (ip->GetSubObjectLevel() == 1)
				fnGeomContractVertexSel();
			else if  (ip->GetSubObjectLevel() == 2)
				fnGeomContractEdgeSel();
			else if  (ip->GetSubObjectLevel() == 3)
				fnGeomContractFaceSel();

			iret = TRUE;
		}
		break;
		//	 		fnGeomContractFaceSel();
		//			iret = TRUE;
		break;

	case ID_SHOWVERTCONNECT:
		{
			BOOL show = fnGetShowConnection();
			if (show)
				fnSetShowConnection(FALSE);
			else fnSetShowConnection(TRUE);
			iret = TRUE;
			break;
		}
	case ID_TV_VERTMODE:
		{
			if (fnGetTVSubMode() == TVVERTMODE)
				fnSetTVSubMode(TVOBJECTMODE);
			else
				fnSetTVSubMode(TVVERTMODE);
			iret = TRUE;
			break;
		}
	case ID_TV_EDGEMODE:
		{
			if (fnGetTVSubMode() == TVEDGEMODE)
				fnSetTVSubMode(TVOBJECTMODE);
			else
				fnSetTVSubMode(TVEDGEMODE);
			iret = TRUE;
			break;
		}
	case ID_TV_FACEMODE:
		{
			if (fnGetTVSubMode() == TVFACEMODE)
				fnSetTVSubMode(TVOBJECTMODE);
			else
				fnSetTVSubMode(TVFACEMODE);
			iret = TRUE;
			break;
		}
	case ID_PACK:
		fnPackNoParams();
		iret = TRUE;
		break;
	case ID_PACKDIALOG:
		fnPackDialog();
		iret = TRUE;
		break;
	case ID_PACK_TIGHT:
		{	
		BOOL normalize = packNormalize;
		BOOL rotate = packRotate;
		BOOL fill = packFillHoles;
		float padding = packSpacing;
		mPackTempPadding = GetUIManager()->GetSpinFValue(ID_PACK_PADDINGSPINNER);
		BOOL rescale = packRescaleCluster;
		
		packNormalize = FALSE;
		packRotate = mPackTempRotate;
		packSpacing = mPackTempPadding;		
		packFillHoles = TRUE;
		packRescaleCluster = mPackTempRescale;

		fnPackNoParams();

		packNormalize = normalize;
		packRotate = rotate;
		packFillHoles = fill;
		packSpacing = padding;
		packRescaleCluster = rescale;
		iret = TRUE;
		}
		break;
	case ID_PACK_REGION:
//FIX
		iret = TRUE;
		break;
	case ID_PACK_FULL:
		{	
		BOOL normalize = packNormalize;
		BOOL rotate = packRotate;
		BOOL fill = packFillHoles;
		float padding = packSpacing;
		mPackTempPadding = GetUIManager()->GetSpinFValue(ID_PACK_PADDINGSPINNER);
		BOOL rescale = packRescaleCluster;

		packNormalize = TRUE;
		packRotate = mPackTempRotate;
		packFillHoles = TRUE;
		packSpacing = mPackTempPadding;
		packRescaleCluster = mPackTempRescale;

		fnPackNoParams();

		packNormalize = normalize;
		packRotate = rotate;
		packFillHoles = fill;
		packSpacing = padding;
		packRescaleCluster = rescale;
		iret = TRUE;
		}
		break;
	case ID_PACKBUTTONS:
		{
		int flyOut = mUIManager.GetFlyOut(ID_PACKBUTTONS);
		if (flyOut == 0)
			fnPackNoParams();
		else
		{
			fnPackDialog();
			mUIManager.SetFlyOut(ID_PACKBUTTONS,0,FALSE);
		}
		iret = TRUE;
		}
		break;


	case ID_UVEDGEMODE:
		{
			if (fnGetUVEdgeMode())
				fnSetUVEdgeMode(FALSE);
			else fnSetUVEdgeMode(TRUE);
			iret = TRUE;
			break;
		}
	case ID_OPENEDGEMODE:
		{
			if (fnGetOpenEdgeMode())
				fnSetOpenEdgeMode(FALSE);
			else fnSetOpenEdgeMode(TRUE);
			iret = TRUE;
			break;
		}
	case ID_UVEDGESELECT:
		{
			fnUVEdgeSelect();
			iret = TRUE;
			break;
		}
	case ID_OPENEDGESELECT:
		{
			fnOpenEdgeSelect();
			iret = TRUE;
			break;
		}

	case ID_VERTTOEDGESELECT:
		{
			fnVertToEdgeSelect();
			fnSetTVSubMode(2);
			MoveScriptUI();
			iret = TRUE;
			break;
		}
	case ID_VERTTOFACESELECT:
		{
			fnVertToFaceSelect();
			fnSetTVSubMode(3);
			MoveScriptUI();
			iret = TRUE;
			break;
		}

	case ID_EDGETOVERTSELECT:
		{
			fnEdgeToVertSelect();
			fnSetTVSubMode(1);
			RebuildDistCache();
			MoveScriptUI();
			iret = TRUE;
			break;
		}

	case ID_EDGETOFACESELECT:
		{
			fnEdgeToFaceSelect();
			fnSetTVSubMode(3);
			MoveScriptUI();
			iret = TRUE;
			break;
		}

	case ID_FACETOVERTSELECT:
		{
			fnFaceToVertSelect();
			fnSetTVSubMode(1);
			RebuildDistCache();
			MoveScriptUI();
			iret = TRUE;
			break;
		}
	case ID_FACETOEDGESELECT:
		{
			fnFaceToEdgeSelect();
			fnSetTVSubMode(2);
			MoveScriptUI();
			iret = TRUE;
			break;
		}
	case ID_DISPLAYHIDDENEDGES:
		{
			if (fnGetDisplayHiddenEdges())
				fnSetDisplayHiddenEdges(FALSE);
			else fnSetDisplayHiddenEdges(TRUE);
			iret = TRUE;
			break;
		}
	case ID_SNAPCENTER:
		{
			fnSnapPivot(1);
			iret = TRUE;
			break;
		}
	case ID_SNAPLOWERLEFT:
		{
			fnSnapPivot(2);
			iret = TRUE;
			break;
		}
	case ID_SNAPLOWERCENTER:
		{
			fnSnapPivot(3);
			iret = TRUE;
			break;
		}
	case ID_SNAPLOWERRIGHT:
		{
			fnSnapPivot(4);
			iret = TRUE;
			break;
		}
	case ID_SNAPRIGHTCENTER:
		{
			fnSnapPivot(5);
			iret = TRUE;
			break;
		}

	case ID_SNAPUPPERLEFT:
		{
			fnSnapPivot(8);
			iret = TRUE;
			break;
		}
	case ID_SNAPUPPERCENTER:
		{
			fnSnapPivot(7);
			iret = TRUE;
			break;
		}
	case ID_SNAPUPPERRIGHT:
		{
			fnSnapPivot(6);
			iret = TRUE;
			break;
		}
	case ID_SNAPLEFTCENTER:
		{
			fnSnapPivot(9);
			iret = TRUE;
			break;
		}

	case ID_FREEFORMSNAP:
		{
			UpdatePivot();


			int id = mUIManager.GetFlyOut(ID_FREEFORMSNAP);
			if (id == 0)
				fnSnapPivot(8);
			else if (id == 1)
				fnSnapPivot(6);
			else if (id == 2)
				fnSnapPivot(2);
			else if (id == 3)
				fnSnapPivot(4);
			else if (id == 4)
				fnSnapPivot(1);
			iret = TRUE;
			break;
		}
	case ID_ALIGN_SELECTION_VERTICAL:
		{
			fnAlign(FALSE);
			iret = TRUE;
			break;
		}
	case ID_ALIGN_SELECTION_HORIZONTAL:
		{
			fnAlign(TRUE);
			iret = TRUE;
			break;
		}
	case ID_TOOL_ALIGN_LINEAR:
		{
			fnAlignLinear();
			iret = TRUE;
			break;
		}
	case ID_TOOL_SPACE_HORIZONTAL:
		{
			fnSpace(TRUE);
			iret = TRUE;
			break;
		}
	case ID_TOOL_SPACE_VERTICAL:
		{
			fnSpace(FALSE);
			iret = TRUE;
			break;
		}
	case ID_TOOL_ALIGN_ELEMENT:
		{
			fnAlignElementToEdge();
			iret = TRUE;
			break;
		}
		case ID_ALIGNBYPIVOTH:
			{
				fnAlignByPivotHorizontal();
				iret = TRUE;
				break;
			}
		case ID_ALIGNBYPIVOTV:
			{
				fnAlignByPivotVertical();
				iret = TRUE;
				break;
			}
	case ID_ALIGNH_BUTTONS:
		{
			int flyOut = GetUIManager()->GetFlyOut(ID_ALIGNH_BUTTONS);
			if (flyOut ==  0)
				fnAlignByPivotHorizontal();
			else
				fnAlign(TRUE);
			break;
		}
	case ID_ALIGNV_BUTTONS:
		{
			int flyOut = GetUIManager()->GetFlyOut(ID_ALIGNV_BUTTONS);
			if (flyOut ==  0)
				fnAlignByPivotVertical();
			else
				fnAlign(FALSE);
			break;
		}


	case ID_RELAXBUTTONS:
		{
			int flyOut = mUIManager.GetFlyOut(ID_RELAXBUTTONS);
			if (flyOut == 0)
				fnRelax2();
			else
			{
				fnRelax2Dialog();
				mUIManager.SetFlyOut(ID_RELAXBUTTONS,0,FALSE);
			}
				
			iret = TRUE;
			break;
		}


	case ID_SKETCHREVERSE:
		{
			fnSketchReverse();
			iret = TRUE;
			break;
		}
	case ID_SKETCHDIALOG:
		{
			fnSketchDialog();
			iret = TRUE;
			break;
		}
	case ID_SKETCH:
		{
			fnSketchNoParams();
			iret = TRUE;
			break;
		}
	case ID_RESETPIVOTONSEL:
		{
			if (fnGetResetPivotOnSel())
				fnSetResetPivotOnSel(FALSE);
			else fnSetResetPivotOnSel(TRUE);
			iret = TRUE;
			break;
		}

	case ID_SHOWHIDDENEDGES:
		{
			if (fnGetDisplayHiddenEdges())
				fnSetDisplayHiddenEdges(FALSE);
			else fnSetDisplayHiddenEdges(TRUE);		
			iret = TRUE;
			break;
		}
	case ID_POLYGONMODE:
		{
			if (fnGetPolyMode())
				fnSetPolyMode(FALSE);
			else fnSetPolyMode(TRUE);		
			iret = TRUE;
			break;
		}
	case ID_POLYGONSELECT:
		{
			HoldSelection();
			fnPolySelect();
			iret = TRUE;
			break;
		}
	case ID_ALLOWSELECTIONINSIDEGIZMO:
		{
			if (fnGetAllowSelectionInsideGizmo())
				fnSetAllowSelectionInsideGizmo(FALSE);
			else fnSetAllowSelectionInsideGizmo(TRUE);		
			iret = TRUE;
			break;
		}
	case ID_SAVEASDEFAULT:
		{
			fnSetAsDefaults();
			iret = TRUE;
			break;
		}
	case ID_LOADDEFAULT:
		{
			fnLoadDefaults();
			iret = TRUE;
			break;
		}
	case ID_SHOWSHARED:
		{
			if (fnGetShowShared())
				fnSetShowShared(FALSE);
			else fnSetShowShared(TRUE);		
			iret = TRUE;
			break;
		}
	case ID_ALWAYSEDIT:
		{
			if (fnGetAlwaysEdit())
				fnSetAlwaysEdit(FALSE);
			else fnSetAlwaysEdit(TRUE);	
			iret = TRUE;
			break;
		}
	case ID_SYNCSELMODE:
		{
			if (fnGetSyncSelectionMode())
				fnSetSyncSelectionMode(FALSE);
			else fnSetSyncSelectionMode(TRUE);			
			iret = TRUE;
			break;
		}
	case ID_SYNCTVSELECTION:
		{
			fnSyncTVSelection();
			iret = TRUE;
			break;
		}
	case ID_SYNCGEOMSELECTION:
		{
			fnSyncGeomSelection();
			iret = TRUE;
			break;
		}
	case ID_SHOWOPENEDGES:
		{
			if (fnGetDisplayOpenEdges())
				fnSetDisplayOpenEdges(FALSE);
			else fnSetDisplayOpenEdges(TRUE);			
			iret = TRUE;
			break;
		}

	case ID_BRIGHTCENTERTILE:
		{
			if (fnGetBrightCenterTile())
				fnSetBrightCenterTile(FALSE);
			else fnSetBrightCenterTile(TRUE);			
			iret = TRUE;
			break;
		}
	case ID_BLENDTOBACK:
		{
			if (fnGetBlendToBack())
				fnSetBlendToBack(FALSE);
			else fnSetBlendToBack(TRUE);
			iret = TRUE;
			break;
		}

	case ID_TV_PAINTSELECTINC:
		{
			fnIncPaintSize();
			iret = TRUE;
			break;
		}
	case ID_TV_PAINTSELECTDEC:
		{
			fnDecPaintSize();			
			iret = TRUE;
			break;
		}
	case ID_GRIDSNAP:
		{
			if (fnGetGridSnap())
				fnSetGridSnap(FALSE);
			else fnSetGridSnap(TRUE);			
			iret = TRUE;
			break;
		}

	case ID_GRIDVISIBLE:
		{
			if (fnGetGridVisible())
				fnSetGridVisible(FALSE);
			else fnSetGridVisible(TRUE);			
			iret = TRUE;
			break;
		}

	case ID_RELAX:
		{
			this->fnRelax2();
			iret = TRUE;
			break;
		}

	case ID_RELAXDIALOG:
		{
			this->fnRelax2Dialog();
			iret = TRUE;
			break;
		}

	case ID_RELAXBYSPRING:
		{
			float stretch = relaxBySpringStretch;
			stretch= 1.0f-stretch*0.01f;
			this->fnRelaxBySprings(relaxBySpringIteration,relaxBySpringStretch,relaxBySpringUseOnlyVEdges);
			iret = TRUE;
			break;
		}
	case ID_RELAXBYSPRINGDIALOG:
		{
			this->fnRelaxBySpringsDialog();
			iret = TRUE;
			break;
		}

	case ID_SELECTINVERTEDFACES:
		{
			this->fnSelectInvertedFaces();
			iret = TRUE;
			break;
		}
	case ID_TV_RING:
		{
			fnUVRing(0);
			iret = TRUE;
			break;
		}
	case ID_TV_RINGGROW:
		{
			fnUVRing(1);
			iret = TRUE;
			break;
		}
	case ID_TV_RINGSHRINK:
		{
			fnUVRing(-1);
			iret = TRUE;
			break;
		}
	case ID_TV_LOOP:
		{
			fnUVLoop(0);
			iret = TRUE;
			break;
		}
	case ID_TV_LOOPGROW:
		{
			fnUVLoop(1);
			iret = TRUE;
			break;
		}
	case ID_TV_LOOPSHRINK:
		{
			fnUVLoop(-1);
			iret = TRUE;
			break;
		}
	case ID_RESCALECLUSTER:
		{
			RescaleSelectedCluster();
			iret = TRUE;
			break;
		}
	case ID_WELDALLSHARED:
		{
			fnWeldAllShared();
			iret = TRUE;
			break;
		}
	case ID_WELDSELECTEDSHARED:
		{
			fnWeldSelectedShared();
			iret = TRUE;
			break;
		}


	case ID_QMAP:
		{
			fnQMap();
			iret = TRUE;

			break;
		}

	case ID_RENDERUV:
		{

			fnRenderUVDialog();
			iret = TRUE;
			break;
		}

	case ID_SNAPGRID:
		{
			BOOL snap; 
			pblock->GetValue(unwrap_gridsnap,0,snap,FOREVER);
			if (snap)
				snap = FALSE;
			else snap = TRUE;
			pblock->SetValue(unwrap_gridsnap,0,snap);
			iret = TRUE;
			break;
		}
	case ID_SNAPVERTEX:
		{
			BOOL snap; 
			pblock->GetValue(unwrap_vertexsnap,0,snap,FOREVER);
			if (snap)
				snap = FALSE;
			else snap = TRUE;
			pblock->SetValue(unwrap_vertexsnap,0,snap);
			iret = TRUE;
			break;
		}

	case ID_SNAPEDGE:
		{
			BOOL snap; 
			pblock->GetValue(unwrap_edgesnap,0,snap,FOREVER);
			if (snap)
				snap = FALSE;
			else snap = TRUE;
			pblock->SetValue(unwrap_edgesnap,0,snap);
			iret = TRUE;
			break;
		}
	case ID_SHOWCOUNTER:
		{
			if (fnGetShowCounter())
				fnSetShowCounter(FALSE);
			else fnSetShowCounter(TRUE);
			iret = TRUE;
			break;
		}

	case ID_SELECT_OVERLAP:
		{
			fnSelectOverlap();
			iret = TRUE;
			break;
		}
	case ID_PW_SHOWEDGEDISTORTION:
		if (fnGetShowEdgeDistortion())
			fnSetShowEdgeDistortion(FALSE);
		else fnSetShowEdgeDistortion(TRUE);
		iret = TRUE;
		break;

	case ID_SHOWLOCALDISTORTION:
		{
			BOOL showLocalDistortion;
			TimeValue t = GetCOREInterface()->GetTime();
			pblock->GetValue(unwrap_localDistorion,t,showLocalDistortion,FOREVER);

			if (showLocalDistortion)
				pblock->SetValue(unwrap_localDistorion,t,FALSE);
			else pblock->SetValue(unwrap_localDistorion,t,TRUE);

			iret = TRUE;
		}
		break;


	case ID_PELT_ALWAYSSHOWSEAMS:
		if  (fnGetAlwayShowPeltSeams())
			fnSetAlwayShowPeltSeams(FALSE);
		else fnSetAlwayShowPeltSeams(TRUE);
		iret = TRUE;

		break;
	case ID_EDGERINGSELECTION:
		fnGeomRingSelect();
		iret = TRUE;
		break;
	case ID_EDGELOOPSELECTION:
		fnGeomLoopSelect();
		iret = TRUE;
		break;
	case ID_PELT_MAP:
		fnSetMapMode(PELTMAP);
		iret = TRUE;
		break;
	case ID_PLANAR_MAP:
		fnSetMapMode(PLANARMAP);
		iret = TRUE;
		break;
	case ID_CYLINDRICAL_MAP:
		fnSetMapMode(CYLINDRICALMAP);
		iret = TRUE;
		break;
	case ID_SPHERICAL_MAP:
		fnSetMapMode(SPHERICALMAP);
		iret = TRUE;
		break;

	case ID_BOX_MAP:
		fnSetMapMode(BOXMAP);
		iret = TRUE;
		break;
	case ID_SPLINE_MAP:
		fnSetMapMode(SPLINEMAP);
		iret = TRUE;
		break;

	case ID_UNFOLD_MAP:
		fnSetMapMode(UNFOLDMAP);
		iret = TRUE;
		break;
	case ID_UNFOLD_EDGE:
		fnRegularMapFromEdge();
		iret = TRUE;
		break;
	case ID_LSCM_INTERACTIVE:
	case ID_LSCM_MAP:
		fnSetMapMode(LSCMMAP);
		iret = TRUE;
		break;

	case ID_LSCM_SOLVE:
		fnLSCMSolve();
		iret = TRUE;
		break;
	case ID_LSCM_RESET:
		fnLSCMReset();
		iret = TRUE;
		break;


	case ID_PIN:
		for (int i = 0; i < GetMeshTopoDataCount(); i++)
		{
			fnPinSelected(GetMeshTopoDataNode(i));
		}		
		iret = TRUE;
		break;
	case ID_UNPIN:
		for (int i = 0; i < GetMeshTopoDataCount(); i++)
		{
			fnUnpinSelected(GetMeshTopoDataNode(i));
		}		
		iret = TRUE;
		break;
			



	case ID_MAPPING_ALIGNX:
		fnAlignAndFit(0);
		iret = TRUE;
		break;				
	case ID_MAPPING_ALIGNY:
		fnAlignAndFit(1);
		iret = TRUE;
		break;				
	case ID_MAPPING_ALIGNZ:
		fnAlignAndFit(2);
		iret = TRUE;
		break;				
	case ID_MAPPING_NORMALALIGN:
		fnAlignAndFit(3);
		iret = TRUE;
		break;
	case ID_MAPPING_FIT:
		fnGizmoFit();
		iret = TRUE;
		break;
	case ID_MAPPING_CENTER:
		fnGizmoCenter();
		iret = TRUE;
		break;
	case ID_MAPPING_ALIGNTOVIEW:
		fnGizmoAlignToView();
		iret = TRUE;
		break;

	case ID_MAPPING_RESET:
		fnGizmoReset();
		iret = TRUE;
		break;

	case ID_TWEAKUVW:
		if (fnGetTweakMode())
			fnSetTweakMode(FALSE);
		else fnSetTweakMode(TRUE);
		iret = TRUE;
		break;

	case ID_PELT_EDITSEAMS:
		if (fnGetPeltEditSeamsMode())
			fnSetPeltEditSeamsMode(FALSE);
		else fnSetPeltEditSeamsMode(TRUE);
		iret = TRUE;

		break;
	case ID_PELT_POINTTOPOINTSEAMS:	
		if (GetCOREInterface()->GetCommandMode() == peltPointToPointMode)
			fnSetPeltPointToPointSeamsMode(FALSE);
		else fnSetPeltPointToPointSeamsMode(TRUE);
		iret = TRUE;
		break;
	case ID_POINT_TO_POINT_SEL:
		if (GetCOREInterface()->GetCommandMode() == peltPointToPointMode)
			peltData.SetPointToPointSeamsMode(this, FALSE,FALSE);
		else 
			peltData.SetPointToPointSeamsMode(this, TRUE,FALSE);
		iret = TRUE;
		break;


	case ID_PELT_EXPANDSELTOSEAM:	
		fnPeltExpandSelectionToSeams();
		iret = TRUE;
		break;

	case ID_PW_SELTOSEAM:	
		fnPeltSelToSeams(TRUE);
		iret = TRUE;
		break;
	case ID_PW_SELTOSEAM2:	
		fnPeltSelToSeams(FALSE);
		iret = TRUE;
		break;

	case ID_PW_SEAMTOSEL:	
		fnPeltSeamsToSel(TRUE);
		iret = TRUE;
		break;

	case ID_PW_SEAMTOSEL2:			
		fnPeltSeamsToSel(FALSE);
		iret = TRUE;
		break;

	case ID_PELTDIALOG:
		fnPeltDialog();
		iret = TRUE;
		break;
	case ID_PELTDIALOG_RESETRIG:
		fnPeltDialogResetRig();
		iret = TRUE;
		break;
	case ID_PELTDIALOG_SELECTRIG:
		fnPeltDialogSelectRig();
		iret = TRUE;
		break;
	case ID_PELTDIALOG_SELECTPELT:
		fnPeltDialogSelectPelt();
		iret = TRUE;
		break;

	case ID_PELTDIALOG_SNAPRIG:
		fnPeltDialogSnapRig();
		iret = TRUE;
		break;

	case ID_PELTDIALOG_STRAIGHTENSEAMS:
		if (fnGetPeltDialogStraightenSeamsMode())
			fnSetPeltDialogStraightenSeamsMode(FALSE);
		else fnSetPeltDialogStraightenSeamsMode(TRUE);
		iret = TRUE;

		break;
	case ID_PELTDIALOG_MIRRORRIG:
		fnPeltDialogMirrorRig();
		iret = TRUE;
		break;
	case ID_PELTDIALOG_RUN:
		fnPeltDialogRun();
		iret = TRUE;
		break;
	case ID_PELTDIALOG_RELAX1:
		fnPeltDialogRelax1();
		iret = TRUE;
		break;
	case ID_PELTDIALOG_RELAX2:
		fnPeltDialogRelax2();
		iret = TRUE;
		break;
	case ID_SHOWOPENEDGESINVIEWPORT:
		if (fnGetViewportOpenEdges())
			fnSetViewportOpenEdges(FALSE);
		else fnSetViewportOpenEdges(TRUE);
		InvalidateView();
		NotifyDependents(FOREVER,PART_GEOM,REFMSG_CHANGE);
		GetCOREInterface()->RedrawViews(GetCOREInterface()->GetTime());
		iret = TRUE;
		break;

	case ID_SAVE:
		fnSave();
		iret = TRUE;
		break;
	case ID_LOAD:
		fnLoad();
		iret = TRUE;
		break;
	case ID_RESET:
		fnReset();
		iret = TRUE;
		break;
	case ID_EDIT:
		fnEdit();
		iret = TRUE;
		break;
	case ID_PREVENTREFLATTENING:
		{
			if (fnGetPreventFlattening())
				fnSetPreventFlattening(FALSE);
			else fnSetPreventFlattening(TRUE);	
			iret = TRUE;
			break;
		}
	case ID_ZOOMTOGIZMO:
		fnZoomToGizmo(FALSE);
		iret = TRUE;
		break;
	case ID_RELAXONECLICK:
		fnRelaxOneClick();
		iret = TRUE;
		break;
	case ID_ROTATE_90_CW:
		{
			RotateAroundPivot(-PI*0.5);

			UpdatePivot();
			
			iret = TRUE;
			break;
		}
	case ID_ROTATE_90_CCW:
		{
			RotateAroundPivot(PI*0.5);
			UpdatePivot();

			iret = TRUE;
		}
		break;


	case ID_STRAIGHTEN:
		{
			fnStraighten();
			iret = TRUE;
			break;
		}

	case ID_PASTESYMMETRICAL:
		{
			//FIX
			iret = TRUE;
			break;
		}
	case ID_GROUP:
		{
			fnGroupCreateBySelection();
			iret = TRUE;
			break;
		}
	case ID_UNGROUP:
		{
			fnGroupDeleteBySelection();
			iret = TRUE;
			break;
		}
	case ID_GROUPSELECT:
		{
			fnGroupSelectBySelection();
			iret = TRUE;
			break;
		}
	case ID_GROUPSETDENSITY:
		{
			float scale = mUIManager.GetSpinFValue(ID_GROUPSETDENSITY_SPINNER);
			fnGroupSetTexelDensity(scale);
			iret = TRUE;
			break;
		}
	case ID_SELECTBY_MATID:
		{
			int id = mUIManager.GetSpinIValue(ID_MATIDSPIN);
			fnSelectByMatID(id);
			//send macro message
			if (emmitMacroScript)
			{
				TSTR mstr = GetMacroStr(_T("modifiers[#unwrap_uvw].unwrap2.selectBySG"));
				macroRecorder->FunctionCall(mstr, 1, 0,
					mr_int,id);
				macroRecorder->EmitScript();
			}
			iret = TRUE;
			break;
		}

	case ID_SELECTBY_SMGRP:
		{
			int smgrp = mUIManager.GetSpinIValue(ID_SMGRPSPIN);
			fnSelectBySG(smgrp);
			//send macro message
			if (emmitMacroScript)
			{

				TSTR mstr = GetMacroStr(_T("modifiers[#unwrap_uvw].unwrap2.selectByMatID"));
				macroRecorder->FunctionCall(mstr, 1, 0,
					mr_int,id);
				macroRecorder->EmitScript();				
			}
			iret = TRUE;
			break;
		}
	case ID_QUICKMAP_DISPLAY:
		{
			BOOL preview = GetQMapPreview();
			preview = !preview;
			SetQMapPreview(preview);
			UpdateViewAndModifier();
			iret = TRUE;
			break;
		}
	case ID_QUICKMAP_ALIGN:
		{
			int align = mUIManager.GetFlyOut(ID_QUICKMAP_ALIGN);
			SetQMapAlign(align);
			UpdateViewAndModifier();
			iret = TRUE;
			break;
		}
	case ID_TOOL_AUTOPIN:
		{
			pblock->GetValue(unwrap_autopin,0,iret,FOREVER);
			iret = !iret;
			pblock->SetValue(unwrap_autopin,0,iret);
			iret = TRUE;
			break;
		}
	case ID_TOOL_FILTERPIN:
		{
			pblock->GetValue(unwrap_filterpin,0,iret,FOREVER);
			iret = !iret;
			pblock->SetValue(unwrap_filterpin,0,iret);
			iret = TRUE;
			break;
		}
	case ID_FLATTEN_BYMATID:
		{
			fnFlattenByMaterialID(FALSE,TRUE,flattenSpacing);
			iret = TRUE;
			break;

		}
	case ID_FLATTEN_BYSMOOTHINGGROUP:
		{
			fnFlattenBySmoothingGroup(FALSE,TRUE,flattenSpacing);
			iret = TRUE;
			break;

		}
	case ID_PACK_RESCALE:
		{
			if (mPackTempRescale)
				mPackTempRescale = false;
			else
				mPackTempRescale = true;
			iret = TRUE;
			break;
		}
	case ID_PACK_ROTATE:
		{
			if (mPackTempRotate)
				mPackTempRotate = false;
			else
				mPackTempRotate = true;
			iret = TRUE;
			break;
		}

	}
	}




	if ( (ip) &&(hDialogWnd) && iret)
	{	
		IMenuBarContext* pContext = (IMenuBarContext*) GetCOREInterface()->GetMenuManager()->GetContext(kUnwrapMenuBar);
		if (pContext)
			pContext->UpdateWindowsMenu();
	}

	mUIManager.UpdateCheckButtons();

	return iret;
}

