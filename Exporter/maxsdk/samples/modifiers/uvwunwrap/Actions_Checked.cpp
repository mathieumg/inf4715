
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

#include "Unwrap.h"
#include "modsres.h"


BOOL UnwrapMod::WtIsChecked(int id)
{

	BOOL iret = FALSE;
	switch (id)
	{
		//These are our dialog command modes
	case ID_FREEFORMMODE:
		if (mode ==ID_FREEFORMMODE)
		{
			iret = TRUE; 
		}
		break;
	case ID_MOVE:
		if (mode ==  ID_MOVE)
		{
			iret = TRUE; 
		}
		break;
	case ID_ROTATE:
		if ( (mode ==  ID_ROTATE))
		{
			iret = TRUE; 
		}
		break;
	case ID_SCALE:
		if ((mode ==  ID_SCALE))
		{
			iret = TRUE; 
		}
		break;
	case ID_WELD:
		if ((mode ==  ID_WELD))
		{
			iret = TRUE; 
		}
		break;
	case ID_PAN:
		if ((mode ==  ID_PAN))
		{
			iret = TRUE; 
		}
		break;
	case ID_ZOOMTOOL:
		if ((mode ==  ID_ZOOMTOOL))
		{
			iret = TRUE; 
		}
		break;
	case ID_ZOOMREGION:
		if ((mode ==  ID_ZOOMREGION)) 
		{
			iret = TRUE; 
		}
		break;
	case ID_TOOL_PELTSTRAIGHTEN:	
		if (mode ==ID_TOOL_PELTSTRAIGHTEN)
		{
			iret = TRUE; 
		}
		break;

	case ID_SNAPGRID:
		{
			BOOL snap; 
			pblock->GetValue(unwrap_gridsnap,0,snap,FOREVER);
			iret = snap;
			break;
		}
	case ID_SNAPVERTEX:
		{
			BOOL snap; 
			pblock->GetValue(unwrap_vertexsnap,0,snap,FOREVER);
			iret = snap;
			break;
		}

	case ID_SNAPEDGE:
		{
			BOOL snap; 
			pblock->GetValue(unwrap_edgesnap,0,snap,FOREVER);
			iret = snap;
			break;
		}


	case ID_SHOWCOUNTER:
		iret = fnGetShowCounter();
		break;

	case ID_TWEAKUVW:
		iret = fnGetTweakMode();
		break;


	case ID_SHOWLOCALDISTORTION:
		{
			BOOL showLocalDistortion;
			TimeValue t = GetCOREInterface()->GetTime();
			pblock->GetValue(unwrap_localDistorion,t,showLocalDistortion,FOREVER);
			iret = showLocalDistortion;
		}
		break;

	case ID_PW_SHOWEDGEDISTORTION:
		iret = fnGetShowEdgeDistortion();
		break;

	case ID_PELT_ALWAYSSHOWSEAMS:
		iret = fnGetAlwayShowPeltSeams();
		break;

	case ID_PELT_MAP:
		if (fnGetMapMode()==PELTMAP)
			iret = TRUE;
		break;
	case ID_PLANAR_MAP:
		if (fnGetMapMode()==PLANARMAP)
			iret = TRUE;
		break;
	case ID_CYLINDRICAL_MAP:
		if (fnGetMapMode()==CYLINDRICALMAP)
			iret = TRUE;
		break;
	case ID_SPHERICAL_MAP:
		if (fnGetMapMode()==SPHERICALMAP)
			iret = TRUE;
		break;
	case ID_BOX_MAP:
		if (fnGetMapMode()==BOXMAP)
			iret = TRUE;
		break;
	case ID_SPLINE_MAP:
		if (fnGetMapMode()==SPLINEMAP)
			iret = TRUE;
		break;
	case ID_UNFOLD_MAP:
		if (fnGetMapMode()==UNFOLDMAP)
			iret = TRUE;
		break;
	case ID_LSCM_MAP:
	case ID_LSCM_INTERACTIVE:
		if (fnGetMapMode()==LSCMMAP)
			iret = TRUE;
		break;

	case ID_PELT_EDITSEAMS:
		if (fnGetPeltEditSeamsMode())
			iret = TRUE;
		break;
	case ID_PELT_POINTTOPOINTSEAMS:
		if (fnGetPeltPointToPointSeamsMode() && peltData.PointToPointSelSeams())
			iret = TRUE;
		break;
	case ID_POINT_TO_POINT_SEL:
		if (fnGetPeltPointToPointSeamsMode() && (peltData.PointToPointSelSeams()== FALSE))
			iret = TRUE;
		break;
	case ID_PELTDIALOG_STRAIGHTENSEAMS:
		if (fnGetPeltDialogStraightenSeamsMode())
			iret = TRUE;
		break;


	case ID_SHOWOPENEDGESINVIEWPORT:
		if (fnGetViewportOpenEdges())
			iret = TRUE;
		break;




	case ID_LOCK:
		iret = lockSelected;
		break;
	case ID_FILTERSELECTED:
		iret = filterSelectedFaces;
		break;

	case ID_SHOWMAP:
		iret = showMap;
		break;

	case ID_SNAP:
		if (mUIManager.GetFlyOut(ID_SNAP) == 1)
			iret = pixelSnap;
		else
			iret = gridSnap;
		break;
	case ID_LIMITSOFTSEL:
		iret = limitSoftSel;
		break;
	case ID_GEOMELEMMODE:
		iret = geomElemMode;
		break;
	case ID_PLANARMODE:
		{
			iret = fnGetGeomPlanarMode();
			break;
		}
	case ID_IGNOREBACKFACE:
		{
			iret = fnGetBackFaceCull();
			break;
		}
	case ID_ELEMENTMODE:
		{
			iret = fnGetTVElementMode();
			break;
		}
	case ID_SHOWVERTCONNECT:
		{
			iret = fnGetShowConnection();
			break;
		}
	case ID_TV_VERTMODE:
		{
			if (fnGetTVSubMode() == TVVERTMODE)
				iret = TRUE;
			else iret = FALSE;
			break;
		}
	case ID_TV_EDGEMODE:
		{
			if (fnGetTVSubMode() == TVEDGEMODE)
				iret = TRUE;
			else iret = FALSE;
			break;
		}
	case ID_TV_FACEMODE:
		{
			if (fnGetTVSubMode() == TVFACEMODE)
				iret = TRUE;
			else iret = FALSE;
			break;
		}
	case ID_UVEDGEMODE:
		{
			if (fnGetUVEdgeMode())
				iret = TRUE;
			else iret = FALSE;			
			break;
		}
	case ID_OPENEDGEMODE:
		{
			if (fnGetOpenEdgeMode())
				iret = TRUE;
			else iret = FALSE;			
			break;
		}
	case ID_DISPLAYHIDDENEDGES:
		{
			if (fnGetDisplayHiddenEdges())
				iret = TRUE;
			else iret = FALSE;			
			break;
		}
	case ID_RESETPIVOTONSEL:
		{
			if (fnGetResetPivotOnSel())
				iret = TRUE;
			else iret = FALSE;			
			break;
		}
	case ID_SKETCH:
		{
			if (mode == ID_SKETCHMODE)
				iret = TRUE;
			else iret = FALSE;			
			break;
		}
	case ID_SHOWHIDDENEDGES:
		{
			if (fnGetDisplayHiddenEdges())
				iret = TRUE;
			else iret = FALSE;			
			break;
		}
	case ID_POLYGONMODE:
		{
			if (fnGetPolyMode())
				iret = TRUE;
			else iret = FALSE;			
			break;
		}
	case ID_ALLOWSELECTIONINSIDEGIZMO:
		{
			if (fnGetAllowSelectionInsideGizmo())
				iret = TRUE;
			else iret = FALSE;			
			break;
		}
	case ID_SHOWSHARED:
		{
			if (fnGetShowShared())
				iret = TRUE;
			else iret = FALSE;			
			break;
		}
	case ID_ALWAYSEDIT:
		{
			if (fnGetAlwaysEdit())
				iret = TRUE;
			else iret = FALSE;			
			break;
		}
	case ID_SYNCSELMODE:
		{
			if (fnGetSyncSelectionMode())
				iret = TRUE;
			else iret = FALSE;			
			break;
		}
	case ID_SHOWOPENEDGES:
		{
			if (fnGetDisplayOpenEdges())
				iret = TRUE;
			else iret = FALSE;			
			break;
		}
	case ID_BRIGHTCENTERTILE:
		{
			if (fnGetBrightCenterTile())
				iret = TRUE;
			else iret = FALSE;			
			break;
		}
	case ID_BLENDTOBACK:
		{
			if (fnGetBlendToBack())
				iret = TRUE;
			else iret = FALSE;			
			break;
		}

	case ID_TV_PAINTSELECTMODE:
		{
			if (fnGetPaintMode())
				iret = TRUE;
			else iret = FALSE;			
			break;
		}


	case ID_GRIDSNAP:
		{
			if (fnGetGridSnap())
				iret = TRUE;
			else iret = FALSE;			
			break;
		}

	case ID_GRIDVISIBLE:
		{
			if (fnGetGridVisible())
				iret = TRUE;
			else iret = FALSE;			
			break;
		}
	case ID_PREVENTREFLATTENING:
		{
			if (fnGetPreventFlattening())
				iret = TRUE;
			else iret = FALSE;			
			break;
		}
	case ID_ABSOLUTETYPEIN:
		{
			iret = !absoluteTypeIn;
			break;
		}
	case ID_LOCKSELECTED:
		{
			iret = lockSelected;
			break;
		}

	case ID_FILTER_SELECTEDFACES:
		{

			iret = filterSelectedFaces;
			break;
		}
	case ID_SOFTSELECTION:
		{
			iret = fnGetEnableSoftSelection();
			break;
		}
	case ID_GEOM_ELEMENT:
		{
			iret = fnGetGeomElemMode();
			break;
		}
	case ID_QUICKMAP_DISPLAY:
		{
			iret = GetQMapPreview();
			break;
		}
	case ID_TOOL_AUTOPIN:
		{
			pblock->GetValue(unwrap_autopin,0,iret,FOREVER);
			break;
		}
	case ID_TOOL_FILTERPIN:
		{
			pblock->GetValue(unwrap_filterpin,0,iret,FOREVER);
			break;
		}
	case ID_PACK_RESCALE:
		{
			iret = mPackTempRescale;
			break;
		}
	case ID_PACK_ROTATE:
		{
			iret = mPackTempRotate;
			break;
		}

	}

	return iret;
}
