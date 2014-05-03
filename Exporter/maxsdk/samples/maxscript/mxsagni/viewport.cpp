/**********************************************************************
*<
FILE: viewports.cpp

DESCRIPTION: 

CREATED BY: Simon Feltman

HISTORY: Created 9 December 1998

*>	Copyright (c) 1998, All Rights Reserved.
**********************************************************************/

#include <maxscript/maxscript.h>
#include <maxscript/foundation/numbers.h>
#include <maxscript/foundation/3dmath.h>
#include "MXSAgni.h"
#include <trig.h>
#include <maxscript/maxwrapper/bitmaps.h>
#include "IViewPanelManager.h"
#include "IViewPanel.h"
#include "resource.h"

#ifdef ScripterExport
#undef ScripterExport
#endif
#define ScripterExport __declspec( dllexport )

#include <maxscript\macros\define_external_functions.h>
#	include "namedefs.h"

#include <maxscript\macros\define_instantiation_functions.h>
#	include "viewport_wraps.h"

#include "agnidefs.h"

extern Bitmap* CreateBitmapFromBInfo(void **ptr, const int cx, const int cy);

// ============================================================================
static int vpt_lookup[] = {
	MAXCOM_VPT_LEFT,
	MAXCOM_VPT_RIGHT,
	MAXCOM_VPT_TOP,
	MAXCOM_VPT_BOTTOM,
	MAXCOM_VPT_FRONT,
	MAXCOM_VPT_BACK,
	MAXCOM_VPT_ISO_USER,
	MAXCOM_VPT_PERSP_USER,
	MAXCOM_VPT_CAMERA,
	MAXCOM_VPT_GRID,
	-1, // VIEW_NONE
	MAXCOM_VPT_TRACK,
	MAXCOM_VPT_PERSP_USER,
	MAXCOM_VPT_SPOT,
	MAXCOM_VPT_SHAPE,
};


// ============================================================================
Value* VP_SetType_cf(Value **arg_list, int count)
{
	check_arg_count(SetType, 1, count);
	def_view_types();
	// AF -- (04/30/02) Removed the Trackview in a viewport feature ECO #831
#ifndef TRACKVIEW_IN_A_VIEWPORT
	if (arg_list[0] == n_view_track)
		return &false_value;
#endif //TRACKVIEW_IN_A_VIEWPORT
	int cmd = vpt_lookup[GetID(viewTypes, elements(viewTypes), arg_list[0], -1)];
	if(cmd >= 0) {
		MAXScript_interface->ExecuteMAXCommand(cmd);
		return &true_value;
	}

	return &false_value;
}


// ============================================================================
Value* VP_GetType_cf(Value **arg_list, int count)
{
	check_arg_count_with_keys(GetType, 0, count);
	def_view_types();

	Value *indexValue;
	int index = (int_key_arg(index, indexValue, 0))-1;

	if (index<0) index = MAXScript_interface7->getActiveViewportIndex();
	if (index<0) return &undefined;

	int viewPanelIndex = 0;
	Value* panelValue = key_arg(viewPanelIndex);
	if (panelValue != &unsupplied)
	{
		viewPanelIndex = panelValue->to_int();
		range_check(viewPanelIndex, 1, GetViewPanelManager()->GetViewPanelCount(), MaxSDK::GetResourceStringAsMSTR(IDS_VIEW_PANEL_INDEX_OUT_OF_RANGE))
	}
	-- viewPanelIndex;// 1-based to 0-based.
	IViewPanel* pPanel = GetViewPanelManager()->GetViewPanel(viewPanelIndex);
	ViewExp* vpt = NULL;
	if (NULL == pPanel) //use current active viewpanel.
	{
		vpt = MAXScript_interface7->getViewExp(index).ToPointer();
	}
	else
	{
		vpt = pPanel->GetViewExpByIndex(index).ToPointer();
	}

	if ( !vpt || !vpt->IsAlive() )
	{
		// why are we here
		DbgAssert(!_T("Invalid viewport!"));
		return &false_value;
	}
	Value *vptType = GetName(viewTypes, elements(viewTypes), vpt->GetViewType(), &undefined);
	
	return vptType;
}

// ============================================================================
Value* VP_IsPerspView_cf(Value **arg_list, int count)
{
	check_arg_count(IsPerspView, 0, count);

	if (MAXScript_interface7->getActiveViewportIndex() < 0) return &undefined;
	
	return bool_result( MAXScript_interface->GetActiveViewExp().IsPerspView() );
}


ViewExp10& GetActiveViewExp10(bool& isValid)
{
	ViewExp& vp = MAXScript_interface->GetActiveViewExp(); // Active Viewport is always existing
	DbgAssert(vp.IsAlive());

	ViewExp10* vp10 = reinterpret_cast<ViewExp10*>(vp.Execute(ViewExp::kEXECUTE_GET_VIEWEXP_10));
	
	isValid = vp.IsAlive() ? true : false;
	
	return *vp10; // the pointer will be always valid - thanks to the unsafe reinterpret_cast
}

// ============================================================================
Value* VP_GetFPS_cf(Value **arg_list, int count)
{
	check_arg_count(GetFPS, 0, count);

	if (MAXScript_interface7->getActiveViewportIndex() < 0) return &undefined;
	
	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);

	float fps = 0.0f;
	if (isValid) 
	{
		fps = vp10.GetViewportFPS();
	}

	return Float::intern(fps);
}



Value* VP_GetClipScale_cf(Value **arg_list, int count)
{
	check_arg_count(GetClipScale, 0, count);

	if (MAXScript_interface7->getActiveViewportIndex() < 0) return &undefined;
	
	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);

	float clipScale = 0.0f;
	if (isValid)
	{
		clipScale = vp10.GetViewportClipScale();
	}

	return Float::intern(clipScale);
}

Value* VP_SetClipScale_cf(Value **arg_list, int count)
{
	// viewport.setfov <float>
	check_arg_count(SetClipScale, 1, count);
	
	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);
	
	if (isValid && vp10.GetHWnd() != NULL)
	{
		// SetFOV only allows changes to a viewport if VIEW_PERSP_USER or VIEW_CAMERA.
		vp10.SetViewportClipScale( arg_list[0]->to_float() );
		return &true_value;
	}
	
	return &false_value;
}


// ============================================================================
Value* VP_SetTM_cf(Value **arg_list, int count)
{
	check_arg_count(SetTM, 1, count);

	BOOL result = FALSE;
	Matrix3 tm = arg_list[0]->to_matrix3();

	ViewExp& vpt = MAXScript_interface->GetActiveViewExp();
	if(vpt.IsAlive())
	{
		result = vpt.SetAffineTM(tm);
	}

	if(result)
	{
		needs_complete_redraw_set();
		return &true_value;
	}

	return &false_value;
}


// ============================================================================
Value* VP_GetTM_cf(Value **arg_list, int count)
{
	check_arg_count(GetTM, 0, count);

	Matrix3 tm;
	tm.IdentityMatrix();

	ViewExp& vpt = MAXScript_interface->GetActiveViewExp();
	if(vpt.IsAlive())
	{
		vpt.GetAffineTM(tm);
	}

	return new Matrix3Value(tm);
}


// ============================================================================
Value* VP_SetCamera_cf(Value **arg_list, int count)
{
	check_arg_count(SetCamera, 1, count);

	INode *camnode = arg_list[0]->to_node();
	ViewExp& vpt = MAXScript_interface->GetActiveViewExp();
	if(vpt.IsAlive() && camnode)
	{
		ObjectState os = camnode->EvalWorldState(MAXScript_interface->GetTime());
		if(os.obj && os.obj->SuperClassID() == CAMERA_CLASS_ID)
		{
			vpt.SetViewCamera(camnode);
			needs_complete_redraw_set();
		}
		else if(os.obj && os.obj->SuperClassID() == LIGHT_CLASS_ID) // LAM - 9/5/01
		{
			LightState ls;
			Interval iv;

			((LightObject *)os.obj)->EvalLightState(MAXScript_interface->GetTime(),iv,&ls);
			if (ls.type == SPOT_LGT || ls.type == DIRECT_LGT)
			{
				vpt.SetViewSpot(camnode);
				needs_complete_redraw_set();
			}
		}
	}

	return &ok;
}


//  =================================================

Value*
VP_CanSetToViewport_cf(Value** arg_list, int count) 
{
	check_arg_count(canSetToViewport, 1, count);
	Value* ret = &false_value;

	INode *camnode = arg_list[0]->to_node();
	ViewExp& vpt = MAXScript_interface->GetActiveViewExp();
	if( ! vpt.IsAlive() || ! camnode)
		return ret;
	
	ObjectState os = camnode->EvalWorldState(MAXScript_interface->GetTime());
	if(os.obj && os.obj->SuperClassID() == CAMERA_CLASS_ID)
	{
		ret = &true_value;
	}
	else if(os.obj && os.obj->SuperClassID() == LIGHT_CLASS_ID) // LAM - 9/5/01
	{
		LightState ls;
		Interval iv;

		((LightObject *)os.obj)->EvalLightState(MAXScript_interface->GetTime(),iv,&ls);
		if (ls.type == SPOT_LGT || ls.type == DIRECT_LGT)
		{
			ret = &true_value;
		}
	}

	return ret;
}


// ============================================================================
Value* VP_GetCamera_cf(Value **arg_list, int count)
{
	check_arg_count_with_keys(GetCamera, 0, count);

	Value *indexValue;
	int index = (int_key_arg(index, indexValue, 0))-1;

	if (index<0) index = MAXScript_interface7->getActiveViewportIndex();
	if (index<0) return &undefined;

	int viewPanelIndex = 0;
	Value* panelValue = key_arg(viewPanelIndex);
	if (panelValue != &unsupplied)
	{
		viewPanelIndex = panelValue->to_int();
		range_check(viewPanelIndex, 1, GetViewPanelManager()->GetViewPanelCount(), MaxSDK::GetResourceStringAsMSTR(IDS_VIEW_PANEL_INDEX_OUT_OF_RANGE))
	}
	-- viewPanelIndex;// 1-based to 0-based.

	IViewPanel* pPanel = GetViewPanelManager()->GetViewPanel(viewPanelIndex);

	ViewExp* vpt = NULL;
	INode *camnode = NULL;
	if (!pPanel)
	{
		vpt = MAXScript_interface7->getViewExp(index).ToPointer();
	}
	else
	{
		vpt = pPanel->GetViewExpByIndex(index).ToPointer();
	}
	
	
	if( vpt && vpt->IsAlive())
	{
		camnode = vpt->GetViewCamera();
		if (camnode == NULL)	// LAM - 9/5/01
			camnode = vpt->GetViewSpot();
	}

	if(camnode) return MAXNode::intern(camnode);
	return &undefined;
}


// ============================================================================
Value* VP_SetLayout_cf(Value **arg_list, int count)
{
	check_arg_count(SetLayout, 1, count);
	def_vp_layouts();

	int layoutType = GetID(vpLayouts, elements(vpLayouts), arg_list[0], -1);
	if(layoutType != -1)
	{
		MAXScript_interface->SetViewportLayout(layoutType);
		needs_redraw_set();
	}

	return &ok;
}


// ============================================================================
Value* VP_GetLayout_cf(Value **arg_list, int count)
{
	check_arg_count(GetLayout, 0, count);
	def_vp_layouts();

	Value *layoutKey = GetName(vpLayouts,
		elements(vpLayouts),
		MAXScript_interface->GetViewportLayout(),
		&undefined);
	return layoutKey;
}

// ============================================================================
Value* VP_SetRenderLevel_cf(Value **arg_list, int count)
{
	check_arg_count(SetRenderLevel, 1, count);
	def_vp_renderlevel();

	int renderLevel = GetID(vpRenderLevel, elements(vpRenderLevel), arg_list[0], -1);
	if(renderLevel != -1)
	{
		MAXScript_interface7->SetActiveViewportRenderLevel(renderLevel);
		needs_redraw_set();
	}

	return &ok;
}


// ============================================================================
Value* VP_GetRenderLevel_cf(Value **arg_list, int count)
{
	check_arg_count(GetRenderLevel, 0, count);
	def_vp_renderlevel();

	Value *renderLevelKey = GetName(vpRenderLevel,
		elements(vpRenderLevel),
		MAXScript_interface7->GetActiveViewportRenderLevel(),
		&undefined);
	return renderLevelKey;
}

// ============================================================================
Value* VP_SetShowEdgeFaces_cf(Value **arg_list, int count)
{
	check_arg_count(SetShowEdgeFaces, 1, count);

	BOOL show = arg_list[0]->to_bool();

	MAXScript_interface7->SetActiveViewportShowEdgeFaces(show);
	needs_redraw_set();

	return &ok;
}


// ============================================================================
Value* VP_GetShowEdgeFaces_cf(Value **arg_list, int count)
{
	check_arg_count(GetShowEdgeFaces, 0, count);

	BOOL show = MAXScript_interface7->GetActiveViewportShowEdgeFaces();

	return bool_result(show);
}

// ============================================================================
Value* VP_SetTransparencyLevel_cf(Value **arg_list, int count)
{
	check_arg_count(SetTransparencyLevel, 1, count);

	int level = arg_list[0]->to_int()-1;

	MAXScript_interface7->SetActiveViewportTransparencyLevel(level);
	needs_redraw_set();

	return &ok;
}


// ============================================================================
Value* VP_GetTransparencyLevel_cf(Value **arg_list, int count)
{
	check_arg_count(GetTransparencyLevel, 0, count);

	int level = MAXScript_interface7->GetActiveViewportTransparencyLevel()+1;

	return Integer::intern(level);
}


// RK: Start

Value* 
VP_SetActiveViewport(Value *val)
{
	int index = val->to_int();
	if (index < 1 || index > MAXScript_interface7->getNumViewports())
		throw RuntimeError(MaxSDK::GetResourceStringAsMSTR(IDS_RK_ACTIVEVIEWPORT_INDEX_OUT_OF_RANGE));
	MAXScript_interface7->setActiveViewport(index-1);
	return val;
}

Value* 
VP_GetActiveViewport()
{
	return Integer::intern(MAXScript_interface7->getActiveViewportIndex()+1);
}

Value* VP_SetActiveViewportId(Value *val)
{
	// 1 based to 0 based since we add 1 when calling "viewport.GetID..."
	int Id = val->to_int() - 1;
	IViewPanel* pPanel = NULL;
	int viewPanelCount = GetViewPanelManager()->GetViewPanelCount();
	for (int i = 0; i < viewPanelCount; ++i)
	{
		pPanel = GetViewPanelManager()->GetViewPanel(i);
		if (pPanel)
		{
			int viewportNum = (int)pPanel->GetNumberOfViewports();
			for (int j = 0; j < viewportNum; ++j)
			{
				ViewExp& vpt = pPanel->GetViewExpByIndex(j);
				if (vpt.IsAlive() && vpt.GetViewID() == Id)
				{
					pPanel->SetActiveViewport(j);
					GetViewPanelManager()->SetActiveViewPanel(i);
					return val;
				}
			}
		}
	}

	throw RuntimeError(MaxSDK::GetResourceStringAsMSTR(IDS_RK_ACTIVEVIEWPORT_ID_INVALID));
	return val;
}

Value* VP_GetActiveViewportId()
{
	IViewPanel* pPanel = GetViewPanelManager()->GetActiveViewPanel();
	if (pPanel)
	{
		int index = pPanel->GetActiveViewportIndex();
		if (index >= 0)
		{
			ViewExp& vpt = pPanel->GetViewExpByIndex(index);
			if (vpt.IsAlive())
			{
				return Integer::intern(vpt.GetViewID() + 1); 	// 0 based to 1 based
			}
		}
	}
	return &undefined;
}

Value* VP_SetNumViews(Value* val)
{
	throw RuntimeError (MaxSDK::GetResourceStringAsMSTR(IDS_RK_CANNOT_SET_NUMVIEWS));
	return NULL;
}

Value* 
VP_GetNumViews()
{
	return Integer::intern(MAXScript_interface7->getNumViewports());
}

Value* VP_ResetAllViews_cf(Value **arg_list, int count)
{
	check_arg_count(resetAllViews, 0, count);
	MAXScript_interface7->resetAllViews();
	return &ok;
}

// RK: End

Value* 
VP_ZoomToBounds_cf(Value **arg_list, int count)
{
	check_arg_count(ZoomToBounds, 3, count);
	BOOL all = arg_list[0]->to_bool();
	Point3 a = arg_list[1]->to_point3();
	Point3 b = arg_list[2]->to_point3();
	Box3 box(a,b);
	MAXScript_interface->ZoomToBounds(all,box);

	return &ok;
}

// LAM: Start

#ifndef NO_REGIONS

static Rect GetDeviceRect(HWND vptHWND, int bShowSafeFrame)
{
	RECT clientRect;
	Rect rect;
	float as = 0.0;
	float rendAspect = MAXScript_interface->GetRendApect();
	float rendWidth = MAXScript_interface->GetRendWidth();
	float rendHeight = MAXScript_interface->GetRendHeight();

	if(rendHeight)
		as = rendAspect * rendWidth / rendHeight;
	else
		as = 4.0f / 3.0f;

	GetClientRect(vptHWND, &clientRect);
	int hh, hw;
	int cx = clientRect.right / 2;
	int cy = clientRect.bottom / 2;

	if(bShowSafeFrame) {
		if((float)clientRect.right / (float)clientRect.bottom > as) {
			// window is wider than safe region -- clip off sides
			hh = clientRect.bottom / 2;
			hw = (int)((float)hh * as);
		}
		else {
			// window is taller than safe region -- clip off top and bottom
			hw = clientRect.right / 2;
			hh = (int)((float)hw / as);
		}
	}
	else {
		hw = clientRect.right / 2;
		hh = (int)((float)hw / as);
	}
	rect.left	= cx - hw;
	rect.right	= cx + hw;
	rect.top	= cy - hh;
	rect.bottom	= cy + hh;
	return rect;
}

Value* 
VP_SetRegionRect_cf(Value **arg_list, int count)
{
	check_arg_count_with_keys(SetRegionRect, 2, count);
	int index = arg_list[0]->to_int();
	if (index < 1 || index > MAXScript_interface7->getNumViewports())
		throw RuntimeError(MaxSDK::GetResourceStringAsMSTR(IDS_RK_ACTIVEVIEWPORT_INDEX_OUT_OF_RANGE));

	int viewPanelIndex = 0;
	Value* panelValue = key_arg(viewPanelIndex);
	if (panelValue != &unsupplied)
	{
		viewPanelIndex = panelValue->to_int();
		range_check(viewPanelIndex, 1, GetViewPanelManager()->GetViewPanelCount(), MaxSDK::GetResourceStringAsMSTR(IDS_VIEW_PANEL_INDEX_OUT_OF_RANGE))
	}
	-- viewPanelIndex;// 1-based to 0-based.

	IViewPanel* pPanel = GetViewPanelManager()->GetViewPanel(viewPanelIndex);
	Rect region = arg_list[1]->to_box2();
	Value* tmp;
	BOOL byPixel = bool_key_arg(byPixel, tmp, TRUE); 
	if (NULL == pPanel)
	{
		if (byPixel) 
			MAXScript_interface7->SetRegionRect2(index-1,region);
		else
			MAXScript_interface7->SetRegionRect(index-1,region);
	}
	else
	{
		ViewExp& vpt = pPanel->GetViewExpByIndex(index - 1);
		DeviceRect devRect;
		if (byPixel)
		{
			float rendWidth  = GetCOREInterface()->GetRendWidth();
			float rendHeight = GetCOREInterface()->GetRendHeight();
			devRect.left = (float)region.left/rendWidth;
			devRect.top = (float)region.top/rendHeight;
			devRect.bottom = (float)(region.bottom+1)/rendHeight;
			devRect.right = (float)(region.right+1)/rendWidth;
		}
		else
		{
			Rect dr = GetDeviceRect(vpt.getGW()->getHWnd(), vpt.getSFDisplay());

			devRect.left	= (float)(region.left - dr.left) / (float)(dr.right - dr.left);
			devRect.right	= (float)(region.right - dr.left) / (float)(dr.right - dr.left);
			devRect.top	= (float)(region.top - dr.top) / (float)(dr.bottom - dr.top);
			devRect.bottom	= (float)(region.bottom - dr.top) / (float)(dr.bottom - dr.top);
		}
		ViewExp13* vp13 = reinterpret_cast<ViewExp13*>(vpt.Execute(ViewExp::kEXECUTE_GET_VIEWEXP_13));
		DbgAssert(vp13);
		vp13->SetRegionDeviceRect(devRect);
	}
	return &ok;
}

Value* 
VP_GetRegionRect_cf(Value **arg_list, int count)
{
	check_arg_count_with_keys(GetRegionRect, 1, count);
	int index = arg_list[0]->to_int();
	if (index < 1 || index > MAXScript_interface7->getNumViewports())
		throw RuntimeError(MaxSDK::GetResourceStringAsMSTR(IDS_RK_ACTIVEVIEWPORT_INDEX_OUT_OF_RANGE));
	
	int viewPanelIndex = 0;
	Value* panelValue = key_arg(viewPanelIndex);
	if (panelValue != &unsupplied)
	{
		viewPanelIndex = panelValue->to_int();
		range_check(viewPanelIndex, 1, GetViewPanelManager()->GetViewPanelCount(), MaxSDK::GetResourceStringAsMSTR(IDS_VIEW_PANEL_INDEX_OUT_OF_RANGE))
	}
	-- viewPanelIndex;// 1-based to 0-based.

	IViewPanel* pPanel = GetViewPanelManager()->GetViewPanel(viewPanelIndex);

	Rect region;
	Value* tmp;
	BOOL byPixel = bool_key_arg(byPixel, tmp, TRUE); 
	if (NULL == pPanel)
	{
		if (byPixel) 
			region = MAXScript_interface7->GetRegionRect2(index-1);
		else
			region = MAXScript_interface7->GetRegionRect(index-1);
	}
	else
	{
		ViewExp& vpt = pPanel->GetViewExpByIndex(index - 1); // 1 based to 0 based
		ViewExp13* vp13 = reinterpret_cast<ViewExp13*>(vpt.Execute(ViewExp::kEXECUTE_GET_VIEWEXP_13));
		DbgAssert(vp13);
		if (byPixel)
		{
			DeviceRect devRect = vp13->GetRegionDeviceRect();
			float rendWidth  = GetCOREInterface()->GetRendWidth();
			float rendHeight = GetCOREInterface()->GetRendHeight();
			
			region.left = devRect.left * rendWidth + 0.5f;
			region.top = devRect.top * rendHeight + 0.5f;
			region.bottom = devRect.bottom * rendHeight - 0.5f ;
			region.right = devRect.right * rendWidth - 0.5f;
		}
		else
		{
			Rect dr = GetDeviceRect(vpt.getGW()->getHWnd(), vpt.getSFDisplay());
			DeviceRect devRect = vp13->GetRegionDeviceRect();
			region.left	= dr.left + (int)(devRect.left * (dr.right - dr.left));
			region.right	= dr.left + (int)(devRect.right * (dr.right - dr.left));
			region.top	= dr.top  + (int)(devRect.top * (dr.bottom - dr.top));
			region.bottom	= dr.top  + (int)(devRect.bottom * (dr.bottom - dr.top));
		}
	}
	
	return new Box2Value(region);
}

Value* 
VP_SetBlowupRect_cf(Value **arg_list, int count)
{
	check_arg_count_with_keys(SetBlowupRect, 2, count);
	int index = arg_list[0]->to_int();
	if (index < 1 || index > MAXScript_interface7->getNumViewports())
		throw RuntimeError(MaxSDK::GetResourceStringAsMSTR(IDS_RK_ACTIVEVIEWPORT_INDEX_OUT_OF_RANGE));
	
	int viewPanelIndex = 0;
	Value* panelValue = key_arg(viewPanelIndex);
	if (panelValue != &unsupplied)
	{
		viewPanelIndex = panelValue->to_int();
		range_check(viewPanelIndex, 1, GetViewPanelManager()->GetViewPanelCount(), MaxSDK::GetResourceStringAsMSTR(IDS_VIEW_PANEL_INDEX_OUT_OF_RANGE))
	}
	-- viewPanelIndex;// 1-based to 0-based.

	IViewPanel* pPanel = GetViewPanelManager()->GetViewPanel(viewPanelIndex);

	Rect region = arg_list[1]->to_box2();
	Value* tmp;
	BOOL byPixel = bool_key_arg(byPixel, tmp, TRUE); 
	if (NULL == pPanel)
	{
		if (byPixel) 
			MAXScript_interface7->SetBlowupRect2(index-1,region);
		else
			MAXScript_interface7->SetBlowupRect(index-1,region);
	}
	else
	{
		ViewExp& vpt = pPanel->GetViewExpByIndex(index - 1); // 1 based to 0 based.
		DeviceRect devRect;
		if (byPixel)
		{
			float rendWidth  = GetCOREInterface()->GetRendWidth();
			float rendHeight = GetCOREInterface()->GetRendHeight();
			devRect.left = (float)region.left/rendWidth;
			devRect.top = (float)region.top/rendHeight;
			devRect.bottom = (float)(region.bottom+1)/rendHeight;
			region.right = region.left + (int)((region.bottom - region.top + 1) * (rendWidth/rendHeight));
			devRect.right = (float)region.right/rendWidth;
		}
		else
		{
			Rect dr = GetDeviceRect(vpt.getGW()->getHWnd(), vpt.getSFDisplay());

			devRect.left	= (float)(region.left - dr.left) / (float)(dr.right - dr.left);
			devRect.right	= (float)(region.right - dr.left) / (float)(dr.right - dr.left);
			devRect.top	= (float)(region.top - dr.top) / (float)(dr.bottom - dr.top);
			devRect.bottom	= (float)(region.bottom - dr.top) / (float)(dr.bottom - dr.top);
		}
		ViewExp13* vp13 = reinterpret_cast<ViewExp13*>(vpt.Execute(ViewExp::kEXECUTE_GET_VIEWEXP_13));
		DbgAssert(vp13);
		vp13->SetBlowupDeviceRect(devRect);
	}
	return &ok;
}

Value* 
VP_GetBlowupRect_cf(Value **arg_list, int count)
{
	check_arg_count_with_keys(GetBlowupRect, 1, count);
	int index = arg_list[0]->to_int();
	if (index < 1 || index > MAXScript_interface7->getNumViewports())
		throw RuntimeError(MaxSDK::GetResourceStringAsMSTR(IDS_RK_ACTIVEVIEWPORT_INDEX_OUT_OF_RANGE));
	
	int viewPanelIndex = 0;
	Value* panelValue = key_arg(viewPanelIndex);
	if (panelValue != &unsupplied)
	{
		viewPanelIndex = panelValue->to_int();
		range_check(viewPanelIndex, 1, GetViewPanelManager()->GetViewPanelCount(), MaxSDK::GetResourceStringAsMSTR(IDS_VIEW_PANEL_INDEX_OUT_OF_RANGE))
	}
	-- viewPanelIndex;// 1-based to 0-based.

	IViewPanel* pPanel = GetViewPanelManager()->GetViewPanel(viewPanelIndex);

	Rect region;
	Value* tmp;
	BOOL byPixel = bool_key_arg(byPixel, tmp, TRUE); 
	if (NULL == pPanel)
	{
		if (byPixel) 
			region = MAXScript_interface7->GetBlowupRect2(index-1);
		else
			region = MAXScript_interface7->GetBlowupRect(index-1);
	}
	else
	{
		ViewExp& vpt = pPanel->GetViewExpByIndex(index - 1); // 1 based to 0 based
		ViewExp13* vp13 = reinterpret_cast<ViewExp13*>(vpt.Execute(ViewExp::kEXECUTE_GET_VIEWEXP_13));
		DbgAssert(vp13);
		if (byPixel)
		{
			DeviceRect devRect = vp13->GetBlowupDeviceRect();
			float rendWidth  = GetCOREInterface()->GetRendWidth();
			float rendHeight = GetCOREInterface()->GetRendHeight();

			region.left = devRect.left * rendWidth + 0.5f;
			region.top = devRect.top * rendHeight + 0.5f;
			region.bottom = devRect.bottom * rendHeight - 0.5f ;
			region.right = devRect.right * rendWidth - 0.5f;
		}
		else
		{
			Rect dr = GetDeviceRect(vpt.getGW()->getHWnd(), vpt.getSFDisplay());
			DeviceRect devRect = vp13->GetBlowupDeviceRect();
			region.left	= dr.left + (int)(devRect.left * (dr.right - dr.left));
			region.right	= dr.left + (int)(devRect.right * (dr.right - dr.left));
			region.top	= dr.top  + (int)(devRect.top * (dr.bottom - dr.top));
			region.bottom	= dr.top  + (int)(devRect.bottom * (dr.bottom - dr.top));
		}
	}
	
	return new Box2Value(region);
}
#endif //NO_REGIONS

Value* 
VP_SetGridVisibility_cf(Value **arg_list, int count)
{
	check_arg_count_with_keys(setGridVisibility, 2, count);
	BOOL state = arg_list[1]->to_bool();
	if (arg_list[0] == n_all)
	{
		int panelCount = GetViewPanelManager()->GetViewPanelCount();
		for (int i = 0; i < panelCount; ++i)
		{
			IViewPanel* pPanel = GetViewPanelManager()->GetViewPanel(i);
			if (pPanel)
			{
				for (int index = 0; index < pPanel->GetNumberOfViewports(); ++index)
				{
					pPanel->GetViewExpByIndex(index).SetGridVisibility(state);
				}
			}
		}
	}
	else
	{
		int index = arg_list[0]->to_int();
		if (index < 1 || index > MAXScript_interface7->getNumViewports())
			throw RuntimeError(MaxSDK::GetResourceStringAsMSTR(IDS_RK_ACTIVEVIEWPORT_INDEX_OUT_OF_RANGE));

		int viewPanelIndex = 0;
		Value* panelValue = key_arg(viewPanelIndex);
		if (panelValue != &unsupplied)
		{
			viewPanelIndex = panelValue->to_int();
			range_check(viewPanelIndex, 1, GetViewPanelManager()->GetViewPanelCount(), MaxSDK::GetResourceStringAsMSTR(IDS_VIEW_PANEL_INDEX_OUT_OF_RANGE))
		}
		-- viewPanelIndex;// 1-based to 0-based.

		IViewPanel* pPanel = GetViewPanelManager()->GetViewPanel(viewPanelIndex);

		if (NULL == pPanel)
		{
			MAXScript_interface7->SetViewportGridVisible(index-1,state);
		}
		else
		{
			ViewExp& vpt = pPanel->GetViewExpByIndex(index - 1);
			vpt.SetGridVisibility(state);
		}
	}
	needs_redraw_set();
	return &ok;
}

Value* 
VP_GetGridVisibility_cf(Value **arg_list, int count)
{
	check_arg_count_with_keys(getGridVisibility, 1, count);
	int index = arg_list[0]->to_int();
	if (index < 1 || index > MAXScript_interface7->getNumViewports())
		throw RuntimeError(MaxSDK::GetResourceStringAsMSTR(IDS_RK_ACTIVEVIEWPORT_INDEX_OUT_OF_RANGE));

	int viewPanelIndex = 0;
	Value* panelValue = key_arg(viewPanelIndex);
	if (panelValue != &unsupplied)
	{
		viewPanelIndex = panelValue->to_int();
		range_check(viewPanelIndex, 1, GetViewPanelManager()->GetViewPanelCount(), MaxSDK::GetResourceStringAsMSTR(IDS_VIEW_PANEL_INDEX_OUT_OF_RANGE))
	}
	-- viewPanelIndex;// 1-based to 0-based.

	IViewPanel* pPanel = GetViewPanelManager()->GetViewPanel(viewPanelIndex);

	BOOL res = false;
	if (NULL == pPanel)
	{
		res = MAXScript_interface7->GetViewportGridVisible(index-1);
	}
	else
	{
		res = pPanel->GetViewExpByIndex(index - 1).IsGridVisible();
	}
	return bool_result (res);
}

// LAM: End


// ============================================================================
Value* VP_IsEnabled_cf(Value **arg_list, int count)
{
	check_arg_count(IsEnabled, 0, count);

	BOOL isEnabled = MAXScript_interface->GetActiveViewExp().IsEnabled();

	return (isEnabled) ? &true_value : &false_value;
}


// ============================================================================
Value* VP_GetFOV_cf(Value **arg_list, int count)
{
	check_arg_count(VP_GetFOV, 0, count);

	float fov = MAXScript_interface->GetActiveViewExp().GetFOV() * (180.f/PI);

	return Float::intern(fov);
}

Value* VP_SetFOV_cf(Value **arg_list, int count)
{
	// viewport.setfov <float>
	check_arg_count(VP_SetFOV, 1, count);

	const float fov = arg_list[0]->to_float();
	MXS_range_check(fov, 0.0f, 360.0f, _T("Wanted a value between"));
	
	// Field of View is in Degrees. Can only be between zero and 360 degrees.
	if (fov >= 0.00f && fov <= 360.0f)
	{
		bool isValid = false;
		ViewExp10& vp10 = GetActiveViewExp10(isValid);

		if (isValid && vp10.GetHWnd() != NULL)
		{
			// SetFOV only allows changes to a viewport if VIEW_PERSP_USER or VIEW_CAMERA.
			const float radiansFOV = DegToRad(fov);
			const BOOL setFOVResult = vp10.SetFOV(radiansFOV); // Only a ViewExp10 class has this SetFOV method.
			if (setFOVResult) 
			{
				return &true_value;
			}
		}
	}
	
	return &false_value;
}

Value* VP_Pan_cf(Value **arg_list, int count)
{
	// viewport.pan <x> <y>
	check_arg_count(VP_Pan, 2, count);
	
	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);

	if (isValid)
	{
		const Point2 pt( arg_list[0]->to_float(), arg_list[1]->to_float());
		vp10.Pan(pt);
		return &true_value;
	}
	return &false_value;
}

Value* VP_Zoom_cf(Value **arg_list, int count)
{
	// <boolean> viewport.zoom <float>
	// Equivalent to the Zoom Region tool in the UI.
	// This is also equivalent to what the system calls when the interactive Zoom tool
	// is used on an orthographic or Isometric User viewport.
	check_arg_count(VP_Zoom, 1, count);
	
	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);

	if (isValid)
	{
		vp10.Zoom( arg_list[0]->to_float() );
		return &true_value;
	}
	
	return &false_value;
}

Value* VP_ZoomPerspective_cf(Value **arg_list, int count)
{
	// <boolean> viewport.zoomPerspective <float>
	// Exactly Equivalent to the Interactive zoom tool in the UI. This method only works for
	// Perspective views. In the UI when a user zooms in an orthographic or Isometric User view, 
	// internally, a switch is performed on the viewport type, and instead a call to ViewExp::Zoom is made.
	// Doesn't work for camera views. Implemented here to show users how the code
	// works internally.
	// Returns true if successsful or false otherwise.
	check_arg_count(VP_Zoom, 1, count);
	
	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);
	
	const ULONG viewportType = vp10.GetViewType();
	if (isValid && vp10.IsPerspView() && viewportType != VIEW_CAMERA)
	{
		Matrix3 stm;
		vp10.GetAffineTM(stm);
		const float dist = arg_list[0]->to_float();
		float diff = vp10.GetFocalDist() - dist;
		stm.Translate(Point3(0.0f, 0.0f, diff));
		vp10.SetAffineTM(stm);
		vp10.SetFocalDistance(dist);
		return &true_value;
	}
	
	return &false_value;
}

Value* VP_SetFocalDistance_cf(Value **arg_list, int count)
{
	// <boolean> viewport.setfocaldistance <float>
	// Returns true if successsful or false otherwise.
	check_arg_count(VP_Zoom, 1, count);
	
	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);
	
	const ULONG viewportType = vp10.GetViewType();
	if (isValid && vp10.IsPerspView() && viewportType != VIEW_CAMERA)
	{
		vp10.SetFocalDistance( arg_list[0]->to_float() );
		return &true_value;
	}
	
	return &false_value;
}

Value* VP_GetFocalDistance_cf(Value **arg_list, int count)
{
	// Set Focal Distance is equivalent to the Interactive zoom in the UI
	// viewport.getfocaldistance()
	// returns the set focal distance, or undefined otherwise.
	check_arg_count(VP_Zoom, 0, count);
	
	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);

	if (!isValid)
		return &undefined;
	
	return Float::intern( vp10.GetFocalDist() );
}

Value* VP_Rotate_cf(Value **arg_list, int count)
{
	
	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);
	if ( ! isValid )
		return &false_value;
	
	Quat quat = arg_list[0]->to_quat();
	// <boolean> viewport.rotate <quat> [center: point2]
	check_arg_count_with_keys(VP_Rotate, 1, count);

	Value* vCenterPt = key_arg_or_default(center, &unsupplied);
	if (vCenterPt == &unsupplied)
	{
		vp10.Rotate(quat);
	}
	else
	{
		Point3 pt3 =  vCenterPt->to_point3();
		vp10.Rotate(quat, pt3);
	}
	return &true_value;
}

// ============================================================================
Value* VP_GetScreenScaleFactor_cf(Value **arg_list, int count)
{
	check_arg_count(VP_GetScreenScaleFactor, 1, count);

	float screenScaleFactor = MAXScript_interface->GetActiveViewExp().GetScreenScaleFactor(arg_list[0]->to_point3());
	
	return Float::intern(screenScaleFactor);
}



// ============================================================================
Value* VP_IsWire_cf(Value **arg_list, int count)
{
	check_arg_count(IsWire, 0, count);

	BOOL isWire = MAXScript_interface->GetActiveViewExp().IsWire();
	
	return (isWire) ? &true_value : &false_value;
}

Value* 
VP_SetBkgImageDsp(Value *val)
{
	BOOL on = val->to_bool();
	MAXScript_interface->GetActiveViewExp().setBkgImageDsp(on);
	needs_redraw_set();
	return val;
}

Value* 
VP_GetBkgImageDsp()
{
	BOOL on = MAXScript_interface->GetActiveViewExp().getBkgImageDsp();
	return bool_result(on);
}

Value* VP_Getviewportdib_cf(Value** arg_list, int count)
{
	check_arg_count_with_keys(getViewportDib, 0, count);
	ViewExp& vpt = MAXScript_interface->GetActiveViewExp();
	DbgAssert(vpt.IsAlive());
	if (! vpt.IsAlive()) return &undefined;

	GraphicsWindow *gw = vpt.getGW();

	int dibSize;
	gw->getDIB(NULL, &dibSize);
	BITMAPINFO *bmi = (BITMAPINFO *)malloc(dibSize);
	BITMAPINFOHEADER *bmih = (BITMAPINFOHEADER *)bmi;
	gw->getDIB(bmi, &dibSize);

	HWND gw_hwnd = gw->getHWnd();
	HDC hdc = GetDC(gw_hwnd);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc,bmih->biWidth,bmih->biHeight);

	HDC hdcMem = CreateCompatibleDC(hdc);
	HGDIOBJ hOldB = SelectObject(hdcMem,hBitmap);
	BitBlt(hdcMem, 0, 0, bmih->biWidth, bmih->biHeight, hdc, 0, 0, SRCCOPY);

	BYTE *buf = new BYTE[bmih->biWidth * bmih->biHeight * (bmih->biBitCount / 8) + sizeof(BITMAPINFOHEADER)];

	if (buf)
	{
		if (!GetDIBits(hdcMem, hBitmap, 0, bmih->biHeight, &buf[sizeof(BITMAPINFOHEADER)], bmi, DIB_RGB_COLORS))
		{
			delete [] buf;
			free(bmi);
			DeleteDC(hdcMem);
			ReleaseDC(gw_hwnd, hdc);
			return &undefined;
		}			
		memcpy(buf, bmih, sizeof(BITMAPINFOHEADER));
	}

	Bitmap *map = CreateBitmapFromBInfo((void**)&buf, bmih->biWidth, bmih->biHeight);
	delete [] buf;
	free(bmi);
	DeleteDC(hdcMem);
	ReleaseDC(gw_hwnd, hdc);
	one_typed_value_local(MAXBitMap* mbm);
	vl.mbm = new MAXBitMap ();
	vl.mbm->bi.CopyImageInfo(&map->Storage()->bi);
	vl.mbm->bi.SetFirstFrame(0);
	vl.mbm->bi.SetLastFrame(0);
	vl.mbm->bi.SetName(_T(""));
	vl.mbm->bi.SetDevice(_T(""));
	if (vl.mbm->bi.Type() > BMM_TRUE_64)
		vl.mbm->bi.SetType(BMM_TRUE_64);
	vl.mbm->bm = map;
	return_value(vl.mbm);
}

Value* VP_GetAdaptiveDegFPS_cf(Value **arg_list, int count)
{
	check_arg_count(GetAdaptiveDegFPS, 0, count);

	if (MAXScript_interface7->getActiveViewportIndex() < 0) return &undefined;
	
	float fps = MAXScript_interface->GetActiveViewExp().GetFPS();

	return Float::intern(fps);
}


Value* VP_SetAdaptiveDegFPS_cf(Value **arg_list, int count)
{
	check_arg_count(SetAdaptiveDegFPS, 1, count);
	
	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);
	
	if (isValid && vp10.GetHWnd() != NULL)
	{
		vp10.SetAdaptiveDegGoalFPS( arg_list[0]->to_float() );
		return &true_value;
	}

	return &false_value;
}


Value* VP_GetAdaptiveDegDisplayModeCurrent_cf(Value **arg_list, int count)
{
	check_arg_count(GetAdaptiveDegDisplayModeCurrent, 0, count);

	if (MAXScript_interface7->getActiveViewportIndex() < 0) return &undefined;
	
	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);

	BOOL mode = FALSE;
	if (isValid && vp10.GetHWnd() != NULL)
	{
		mode = vp10.GetAdaptiveDegDisplayModeCurrent();
	}

	return bool_result(mode);
}


Value* VP_SetAdaptiveDegDisplayModeCurrent_cf(Value **arg_list, int count)
{
	check_arg_count(SetAdaptiveDegDisplayModeCurrent, 1, count);
	
	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);
	
	if (isValid && vp10.GetHWnd() != NULL)
	{
		vp10.SetAdaptiveDegDisplayModeCurrent( arg_list[0]->to_bool() );
		return &true_value;
	}

	return &false_value;
}


Value* VP_GetAdaptiveDegDisplayModeFastShaded_cf(Value **arg_list, int count)
{
	check_arg_count(GetAdaptiveDegDisplayModeFastShaded, 0, count);

	if (MAXScript_interface7->getActiveViewportIndex() < 0) return &undefined;
	
	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);
	
	BOOL mode = FALSE;
	if (isValid && vp10.GetHWnd() != NULL)
	{
		mode = vp10.GetAdaptiveDegDisplayModeFastShaded();
	}

	return bool_result(mode);
}


Value* VP_SetAdaptiveDegDisplayModeFastShaded_cf(Value **arg_list, int count)
{
	check_arg_count(SetAdaptiveDegDisplayModeFastShaded, 1, count);
	
	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);
	if (isValid && vp10.GetHWnd() != NULL)
	{
		vp10.SetAdaptiveDegDisplayModeFastShaded( arg_list[0]->to_bool() );
		return &true_value;
	}

	return &false_value;
}


Value* VP_GetAdaptiveDegDisplayModeWire_cf(Value **arg_list, int count)
{
	check_arg_count(GetAdaptiveDegDisplayModeWire, 0, count);

	if (MAXScript_interface7->getActiveViewportIndex() < 0) return &undefined;
	
	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);
	
	BOOL mode = FALSE;
	if (isValid && vp10.GetHWnd() != NULL)
	{
		mode = vp10.GetAdaptiveDegDisplayModeWire();
	}

	return bool_result(mode);
}


Value* VP_SetAdaptiveDegDisplayModeWire_cf(Value **arg_list, int count)
{
	check_arg_count(SetAdaptiveDegDisplayModeWire, 1, count);
	
	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);
	
	if (isValid && vp10.GetHWnd() != NULL)
	{
		vp10.SetAdaptiveDegDisplayModeWire( arg_list[0]->to_bool() );
		return &true_value;
	}
	return &false_value;
}


Value* VP_GetAdaptiveDegDisplayModeBox_cf(Value **arg_list, int count)
{
	check_arg_count(GetAdaptiveDegDisplayModeBox, 0, count);

	if (MAXScript_interface7->getActiveViewportIndex() < 0) return &undefined;
	
	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);
	
	BOOL mode = FALSE;
	if (isValid && vp10.GetHWnd() != NULL)
	{
		mode = vp10.GetAdaptiveDegDisplayModeBox();
	}

	return bool_result(mode);
}


Value* VP_SetAdaptiveDegDisplayModeBox_cf(Value **arg_list, int count)
{
	check_arg_count(SetAdaptiveDegDisplayModeBox, 1, count);
	
	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);
	
	if (isValid && vp10.GetHWnd() != NULL)
	{
		vp10.SetAdaptiveDegDisplayModeBox( arg_list[0]->to_bool() );
		return &true_value;
	}

	return &false_value;
}

Value* VP_GetAdaptiveDegDisplayModePoint_cf(Value **arg_list, int count)
{
	check_arg_count(GetAdaptiveDegDisplayModePoint, 0, count);

	if (MAXScript_interface7->getActiveViewportIndex() < 0) return &undefined;
	
	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);
	
	BOOL mode = FALSE;
	if (isValid && vp10.GetHWnd() != NULL)
	{
		mode = vp10.GetAdaptiveDegDisplayModePoint();
	}
	
	return bool_result(mode);
}


Value* VP_SetAdaptiveDegDisplayModePoint_cf(Value **arg_list, int count)
{
	check_arg_count(SetAdaptiveDegDisplayModePoint, 1, count);
	
	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);
	
	if (isValid && vp10.GetHWnd() != NULL)
	{
		vp10.SetAdaptiveDegDisplayModePoint( arg_list[0]->to_bool() );
		return &true_value;
	}
	return &false_value;
}

Value* VP_GetAdaptiveDegDisplayModeHide_cf(Value **arg_list, int count)
{
	check_arg_count(GetAdaptiveDegDisplayModeHide, 0, count);

	if (MAXScript_interface7->getActiveViewportIndex() < 0) return &undefined;
	
	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);
	
	BOOL mode = FALSE;
	if (isValid && vp10.GetHWnd() != NULL)
	{
		mode = vp10.GetAdaptiveDegDisplayModeHide();
	}

	return bool_result(mode);
}


Value* VP_SetAdaptiveDegDisplayModeHide_cf(Value **arg_list, int count)
{
	check_arg_count(SetAdaptiveDegDisplayModeHide, 1, count);
	
	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);
	
	if (isValid && vp10.GetHWnd() != NULL)
	{
		vp10.SetAdaptiveDegDisplayModeHide( arg_list[0]->to_bool() );
		return &true_value;
	}

	return &false_value;
}

 
Value* VP_GetAdaptiveDegDrawBackface_cf(Value **arg_list, int count)
{
	check_arg_count(GetAdaptiveDegDrawBackface, 0, count);

	if (MAXScript_interface7->getActiveViewportIndex() < 0) return &undefined;
	
	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);
	
	BOOL mode = FALSE;
	if (isValid && vp10.GetHWnd() != NULL)
	{
		mode = vp10.GetAdaptiveDegDrawBackface();
	}
	return bool_result(mode);
}


Value* VP_SetAdaptiveDegDrawBackface_cf(Value **arg_list, int count)
{
	check_arg_count(SetAdaptiveDegDrawBackface, 1, count);
	
	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);
	
	if (isValid && vp10.GetHWnd() != NULL)
	{
		vp10.SetAdaptiveDegDrawBackface( arg_list[0]->to_bool() );
		return &true_value;
	}
	return &false_value;
}


Value* VP_GetAdaptiveDegNeverDegradeSelected_cf(Value **arg_list, int count)
{
	check_arg_count(GetAdaptiveDegNeverDegradeSelected, 0, count);

	if (MAXScript_interface7->getActiveViewportIndex() < 0) return &undefined;
	
	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);
	
	BOOL mode = FALSE;
	if (isValid && vp10.GetHWnd() != NULL)
	{
		mode = vp10.GetAdaptiveDegNeverDegradeSelected();
	}

	return bool_result(mode);
}


Value* VP_SetAdaptiveDegNeverDegradeSelected_cf(Value **arg_list, int count)
{
	check_arg_count(SetAdaptiveDegNeverDegradeSelected, 1, count);
	
	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);
	
	if (isValid && vp10.GetHWnd() != NULL)
	{
		vp10.SetAdaptiveDegNeverDegradeSelected( arg_list[0]->to_bool() );
		return &true_value;
	}
	return &false_value;
}

Value* VP_GetAdaptiveDegDegradeLight_cf(Value **arg_list, int count)
{
	check_arg_count(GetAdaptiveDegDegradeLight, 0, count);

	if (MAXScript_interface7->getActiveViewportIndex() < 0) return &undefined;
	
	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);
	
	BOOL mode = FALSE;
	if (isValid && vp10.GetHWnd() != NULL)
	{
		mode = vp10.GetAdaptiveDegDegradeLight();
	}

	return bool_result(mode);
}


Value* VP_SetAdaptiveDegDegradeLight_cf(Value **arg_list, int count)
{
	check_arg_count(SetAdaptiveDegDegradeLight, 1, count);
	
	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);
	
	if (isValid && vp10.GetHWnd() != NULL)
	{
		vp10.SetAdaptiveDegDegradeLight( arg_list[0]->to_bool() );
		return &true_value;
	}

	return &false_value;
}

Value* VP_GetAdaptiveDegNeverRedrawAfterDegrade_cf(Value **arg_list, int count)
{
	check_arg_count(GetAdaptiveDegNeverRedrawAfterDegrade, 0, count);

	if (MAXScript_interface7->getActiveViewportIndex() < 0) return &undefined;
	
	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);
	
	BOOL mode = FALSE;
	if (isValid && vp10.GetHWnd() != NULL)
	{
		mode = vp10.GetAdaptiveDegNeverRedrawAfterDegrade();
	}

	return bool_result(mode);
}


Value* VP_SetAdaptiveDegNeverRedrawAfterDegrade_cf(Value **arg_list, int count)
{
	check_arg_count(SetAdaptiveDegNeverRedrawAfterDegrade, 1, count);
	
	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);
	
	if (isValid && vp10.GetHWnd() != NULL)
	{
		vp10.SetAdaptiveDegNeverRedrawAfterDegrade( arg_list[0]->to_bool() );
		return &true_value;
	}

	return &false_value;
}

Value* VP_GetAdaptiveDegCameraDistancePriority_cf(Value **arg_list, int count)
{
	check_arg_count(GetAdaptiveDegCameraDistancePriority, 0, count);

	if (MAXScript_interface7->getActiveViewportIndex() < 0) return &undefined;

	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);
	
	float dist = 0.0f;
	if (isValid && vp10.GetHWnd() != NULL)
	{
		dist = vp10.GetAdaptiveDegCameraDistancePriority();
	}

	return Float::intern(dist);
}


Value* VP_SetAdaptiveDegCameraDistancePriority_cf(Value **arg_list, int count)
{
	check_arg_count(SetAdaptiveDegCameraDistancePriority, 1, count);
	
	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);
	
	if (isValid && vp10.GetHWnd() != NULL)
	{
		vp10.SetAdaptiveDegCameraDistancePriority( arg_list[0]->to_float() );
		return &true_value;
	}

	return &false_value;
}

Value* VP_GetAdaptiveDegScreenSizePriority_cf(Value **arg_list, int count)
{
	check_arg_count(GetAdaptiveDegScreenSizePriority, 0, count);

	if (MAXScript_interface7->getActiveViewportIndex() < 0) return &undefined;
	
	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);
	
	float size = 0.0f;
	if (isValid && vp10.GetHWnd() != NULL)
	{
		size = vp10.GetAdaptiveDegScreenSizePriority();
	}
		
	return Float::intern(size);
}


Value* VP_SetAdaptiveDegScreenSizePriority_cf(Value **arg_list, int count)
{
	check_arg_count(SetAdaptiveDegScreenSizePriority, 1, count);
	
	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);
	
	if (isValid && vp10.GetHWnd() != NULL)
	{
		vp10.SetAdaptiveDegScreenSizePriority( arg_list[0]->to_float() );
		return &true_value;
	}

	return &false_value;
}

Value* VP_GetAdaptiveDegMinSize_cf(Value **arg_list, int count)
{
	check_arg_count(GetAdaptiveDegMinSize, 0, count);

	if (MAXScript_interface7->getActiveViewportIndex() < 0) return &undefined;

	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);
	
	int size = 0;
	if (isValid && vp10.GetHWnd() != NULL)
	{
		size = vp10.GetAdaptiveDegMinSize();
	}

	return Integer::intern(size);
}


Value* VP_SetAdaptiveDegMinSize_cf(Value **arg_list, int count)
{
	check_arg_count(SetAdaptiveDegMinSize, 1, count);
	
	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);
	
	if (isValid && vp10.GetHWnd() != NULL)
	{
		vp10.SetAdaptiveDegMinSize( arg_list[0]->to_int() );
		return &true_value;
	}
	return &false_value;
}

Value* VP_GetID_cf( Value **arg_list, int count )
{
	check_arg_count_with_keys(GetID, 1, count);
	int index = (arg_list[0]->to_int() - 1);
	
	if (index<0) index = MAXScript_interface7->getActiveViewportIndex();
	if (index<0) return &undefined;

	int viewPanelIndex = 0;
	Value* panelValue = key_arg(viewPanelIndex);
	if (panelValue != &unsupplied)
	{
		viewPanelIndex = panelValue->to_int();
		range_check(viewPanelIndex, 1, GetViewPanelManager()->GetViewPanelCount(), MaxSDK::GetResourceStringAsMSTR(IDS_VIEW_PANEL_INDEX_OUT_OF_RANGE))
	}
	-- viewPanelIndex;// 1-based to 0-based.

	ViewExp* vpt = NULL;
	IViewPanel* pPanel = GetViewPanelManager()->GetViewPanel(viewPanelIndex);
	if (!pPanel)
	{
		vpt = MAXScript_interface7->getViewExp(index).ToPointer();
	}
	else
	{
		vpt = pPanel->GetViewExpByIndex(index).ToPointer();
	}

	int viewID = -1;
	if(vpt && vpt->IsAlive())
	{
		// Increment by 1, for consistency with other functions.
		// Note the function Interface11::GetRendViewID() returns an ID value, not an index.
		// But maxscript's get_rend_viewID() adds one to the value, treating it as an index.
		// The ID is unique while different viewport can have same index if they belong to different view panels.
		viewID = vpt->GetViewID() + 1;
	}
	
	if(viewID>=0) return Integer::intern(viewID);
	return &undefined;
}

Value* VP_GetViewNumEx_cf( Value **arg_list, int count )
{
	check_arg_count_with_keys(GetViewNumEx, 0, count);

	int viewPanelIndex = 0;
	Value* panelValue = key_arg(viewPanelIndex);
	if (panelValue != &unsupplied)
	{
		viewPanelIndex = panelValue->to_int();
		range_check(viewPanelIndex, 1, GetViewPanelManager()->GetViewPanelCount(), MaxSDK::GetResourceStringAsMSTR(IDS_VIEW_PANEL_INDEX_OUT_OF_RANGE))
	}
	-- viewPanelIndex;// 1-based to 0-based.

	IViewPanel* pPanel = GetViewPanelManager()->GetViewPanel(viewPanelIndex);
	if (NULL == pPanel) //use current active viewpanel.
	{
		return Integer::intern( MAXScript_interface7->getNumViewports() );
	}
	else
	{
		return Integer::intern( (int)pPanel->GetNumberOfViewports() );
	}
}

Value* VP_SetActiveViewportEx_cf( Value **arg_list, int count )
{
	check_arg_count_with_keys(SetActiveViewportEx, 1, count);

	int index = arg_list[0]->to_int();

	int viewPanelIndex = 0;
	Value* panelValue = key_arg(viewPanelIndex);
	if (panelValue != &unsupplied)
	{
		viewPanelIndex = panelValue->to_int();
		range_check(viewPanelIndex, 1, GetViewPanelManager()->GetViewPanelCount(), MaxSDK::GetResourceStringAsMSTR(IDS_VIEW_PANEL_INDEX_OUT_OF_RANGE))
	}
	-- viewPanelIndex;// 1-based to 0-based.
	IViewPanel* pPanel = GetViewPanelManager()->GetViewPanel(viewPanelIndex);
	int viewportCount = 0;
	if (pPanel == NULL)
	{
		viewportCount = MAXScript_interface7->getNumViewports();
	}
	else
	{
		viewportCount = (int)pPanel->GetNumberOfViewports();
	}
	if (index < 1 || index > viewportCount)
		throw RuntimeError(MaxSDK::GetResourceStringAsMSTR(IDS_RK_ACTIVEVIEWPORT_INDEX_OUT_OF_RANGE));

	
	if (NULL == pPanel) //use current active viewpanel.
	{
		MAXScript_interface7->setActiveViewport(index-1);
	}
	else
	{
		pPanel->SetActiveViewport(index - 1);
		GetViewPanelManager()->SetActiveViewPanel(viewPanelIndex);
	}
	return Integer::intern(index);
}

Value* VP_IsSolidBackgroundColorMode_cf(Value **arg_list, int count)
{
	check_arg_count(IsSolidBackgroundColorMode, 0, count);

	if (MAXScript_interface7->getActiveViewportIndex() < 0) return &undefined;

	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);

	if (isValid && vp10.GetHWnd() != NULL)
	{
		return bool_result(vp10.IsSolidBackgroundColorMode());
	}

	return &undefined;
}


Value* VP_EnableSolidBackgroundColorMode_cf(Value **arg_list, int count)
{
	check_arg_count(EnableSolidBackgroundColorMode, 1, count);

	bool isValid = false;
	ViewExp10& vp10 = GetActiveViewExp10(isValid);

	if (isValid && vp10.GetHWnd() != NULL)
	{
		vp10.SetSolidBackgroundColorMode( (arg_list[0]->to_bool() != FALSE));
		return &true_value;
	}
	return &false_value;
}

/* --------------------- plug-in init --------------------------------- */
// this is called by the dlx initializer, register the global vars here
void viewport_init()
{
#include "viewport_glbls.h"
}



