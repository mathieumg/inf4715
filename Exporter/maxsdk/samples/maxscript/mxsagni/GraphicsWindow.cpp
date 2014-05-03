/**********************************************************************
*<
FILE: GraphicsWindow.cpp

DESCRIPTION: 

CREATED BY: Larry Minton

HISTORY: Created 15 April 2007

*>	Copyright (c) 2007, All Rights Reserved.
**********************************************************************/

#include <maxscript/maxscript.h>
#include <maxscript/foundation/numbers.h>
#include <maxscript/foundation/strings.h>
#include <maxscript/foundation/3dmath.h>
#include <maxscript/maxwrapper/bitmaps.h>
#include <Graphics/IDisplayManager.h>
#include "MXSAgni.h"

#include "resource.h"

#include "IHardwareMaterial.h"
#include "IHardwareMesh.h"
#include "IHardwareShader.h"
#include "IHardwareRenderer.h"

#include "gamma.h"
#include "IColorCorrectionMgr.h"

#ifdef ScripterExport
#undef ScripterExport
#endif
#define ScripterExport __declspec( dllexport )

#include "extclass.h"

// ============================================================================

#include <maxscript\macros\define_external_functions.h>
#	include "namedefs.h"

#include <maxscript\macros\define_instantiation_functions.h>
#	include "GraphicsWindow_wraps.h"

#include "agnidefs.h"




/*-----------------------------------GraphicsWindow-------------------------------------------*/

Value*
getDriverString_cf(Value** arg_list, int count)
{
	check_arg_count(getDriverString, 0, count);
	GraphicsWindow *gw = MAXScript_interface->GetActiveViewExp().getGW();
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	return new String(gw->getDriverString());
}

Value*
isPerspectiveView_cf(Value** arg_list, int count)
{
	check_arg_count(isPerspectiveView, 0, count);
	GraphicsWindow *gw = MAXScript_interface->GetActiveViewExp().getGW();		
	
	return gw->isPerspectiveView() ? &true_value : &false_value;	
}

Value*
setSkipCount_cf(Value** arg_list, int count)
{
	check_arg_count(setSkipCount, 1, count);
	GraphicsWindow *gw = MAXScript_interface->GetActiveViewExp().getGW();
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	gw->setSkipCount(arg_list[0]->to_int());
	return &ok;
}


Value*
setDirectXDisplayAllTriangle_cf(Value** arg_list, int count)
{
	check_arg_count(setDirectXDisplayAllTriangle, 1, count);
	GraphicsWindow *gw = MAXScript_interface->GetActiveViewExp().getGW();
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	IHardwareRenderer *phr = (IHardwareRenderer *)gw->GetInterface(HARDWARE_RENDERER_INTERFACE_ID);
	if (phr)
	{
		BOOL displayAllEdges = arg_list[0]->to_bool();
		if (displayAllEdges)
			phr->SetDisplayAllTriangleEdges(true);
		else phr->SetDisplayAllTriangleEdges(false);
	}

	return &ok;
}


Value*
getSkipCount_cf(Value** arg_list, int count)
{
	check_arg_count(getSkipCount, 0, count);
	GraphicsWindow *gw = MAXScript_interface->GetActiveViewExp().getGW();	
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	return Integer::intern(gw->getSkipCount());
}

Value*
querySupport_cf(Value** arg_list, int count)
{
	check_arg_count(querySupport, 01, count);
	def_spt_types();
	GraphicsWindow	*gw = MAXScript_interface->GetActiveViewExp().getGW();		
	
	return gw->querySupport(
		GetID(sptTypes, elements(sptTypes), arg_list[0])) ?
		&true_value : &false_value;
}

Value*
setTransform_cf(Value** arg_list, int count)
{
	check_arg_count(setTransform, 1, count);
	GraphicsWindow *gw = MAXScript_interface->GetActiveViewExp().getGW();
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	gw->setTransform(arg_list[0]->to_matrix3());
	return &ok;
}

Value*
setPos_cf(Value** arg_list, int count)
{
	check_arg_count_with_keys(setPos, 4, count);
	GraphicsWindow *gw = MAXScript_interface->GetActiveViewExp().getGW();
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	gw->setPos(
		arg_list[0]->to_int(),
		arg_list[1]->to_int(),
		arg_list[2]->to_int(),
		arg_list[3]->to_int());
	return &ok;
}

Value*
getWinSizeX_cf(Value** arg_list, int count)
{
	check_arg_count(getWinSizeX, 0, count);
	int sizeX = MAXScript_interface->GetActiveViewExp().getGW()->getWinSizeX();	
	return Integer::intern(sizeX);

}

Value*
getWinSizeY_cf(Value** arg_list, int count)
{
	check_arg_count(getWinSizeY, 0, count);
	int sizeY = MAXScript_interface->GetActiveViewExp().getGW()->getWinSizeY();
	return Integer::intern(sizeY);
}


Value*
getWinDepth_cf(Value** arg_list, int count)
{
	check_arg_count(getWinDepth, 0, count);
	GraphicsWindow *gw = MAXScript_interface->GetActiveViewExp().getGW();	
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	return Integer::intern(gw->getWinDepth());
}

Value*
getHitherCoord_cf(Value** arg_list, int count)
{
	check_arg_count(getHitherCoord, 0, count);
	GraphicsWindow *gw = MAXScript_interface->GetActiveViewExp().getGW();	
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	return Integer::intern(gw->getHitherCoord());
}

Value*
getYonCoord_cf(Value** arg_list, int count)
{
	check_arg_count(getYonCoord, 0, count);
	GraphicsWindow *gw = MAXScript_interface->GetActiveViewExp().getGW();	
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	return Integer::intern(gw->getYonCoord());
}

//DIB Methods
Value*
getViewportDib_cf(Value** arg_list, int count)
{
	check_arg_count(getViewportDib, 0, count);
	GraphicsWindow *gw = MAXScript_interface->GetActiveViewExp().getGW();
	BITMAPINFO *bmi = NULL;
	BITMAPINFOHEADER *bmih;
	BitmapInfo bi;
	Bitmap *bmp;
	int size;
	gw->getDIB(NULL, &size);
	bmi  = (BITMAPINFO *)malloc(size);
	bmih = (BITMAPINFOHEADER *)bmi;
	gw->getDIB(bmi, &size);
	bi.SetWidth((WORD)bmih->biWidth);
	bi.SetHeight((WORD)bmih->biHeight);
	bi.SetType(BMM_TRUE_32);

	UWORD  *gammatab = NULL;

	IColorCorrectionMgr* idispGamMgr = (IColorCorrectionMgr*) GetCOREInterface(COLORCORRECTIONMGR_INTERFACE);
	float gamma = 1.0f; // default if off
	if(gammaMgr.IsEnabled() && idispGamMgr)
	{
		gamma = idispGamMgr->GetGamma();
		gammatab = new UWORD[RCOLN];

		// Build gamma correction table
		if (gammatab)
			BuildGammaTab(gammatab, 1.0f/gamma, true);

		// To gamma-correct we need 64 bits resolution
		bi.SetType(BMM_TRUE_64);
	}

	bi.SetGamma(1.0f); // New bitmap will be linear

	bmp = CreateBitmapFromBitmapInfo(bi); // Make new, linear bitmap
	bmp->FromDib(bmi);

	free(bmi);    // JBW 10.7.99: missing free(), elided above I/O, not desired

	// If gamma is on:
	/* EXPLANATION:
		The code that saves a bitmap always assumes the bitmap to be saved comes from the renderer,
		and hence, is linear. Since we are grabbing off the viewport (with an embedded gamma) we
		need to linearize the bitmap first */
	if (gammatab)
	{
		// We still want this to be SAVED with a gamma. What gamma MaxScript will save
		// this with (by default) is defined by the gamma of the BitmapInfo bi's gamma
		// And we intentionally want it to look like it was displayed - hence we use the
		// display gamma!!
		bi.SetGamma(gamma);

		int h = bmp->Height();
		int w = bmp->Width();

		BMM_Color_64 *pixelrow = (BMM_Color_64 *)LocalAlloc(LPTR,w*sizeof(BMM_Color_64));

		if (pixelrow) 
		{
			for (int iy = 0; iy < h; iy++) {
				bmp->GetPixels(0, iy, w, pixelrow);
				for (int ix = 0; ix < w; ix++) {
					
					pixelrow[ix].r = gammatab[UWORD(pixelrow[ix].r) >> RCSH16];
					pixelrow[ix].g = gammatab[UWORD(pixelrow[ix].g) >> RCSH16];
					pixelrow[ix].b = gammatab[UWORD(pixelrow[ix].b) >> RCSH16];
				}
				bmp->PutPixels(0, iy, w, pixelrow);
			}
			LocalFree(pixelrow);
		}

		delete [] gammatab;
	}

	return new MAXBitMap(bi, bmp);
}

Value*
resetUpdateRect_cf(Value** arg_list, int count)
{
	check_arg_count(resetUpdateRect, 0, count);
	GraphicsWindow	*gw		= MAXScript_interface->GetActiveViewExp().getGW();	
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	gw->resetUpdateRect();
	return &ok;
}

Value*
enlargeUpdateRect_cf(Value** arg_list, int count)
{
	check_arg_count(enlargeUpdateRect, 1, count);
	GraphicsWindow	*gw		= MAXScript_interface->GetActiveViewExp().getGW();
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	Value*			val		= arg_list[0];
	gw->enlargeUpdateRect(( val == n_whole) ? NULL : &(val->to_box2()));
	return &ok;
}

Value*
getUpdateRect_cf(Value** arg_list, int count)
{
	check_arg_count(getUpdateRect, 0, count);
	GraphicsWindow	*gw		= MAXScript_interface->GetActiveViewExp().getGW();	
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	Rect			rt;	
	BOOL res = gw->getUpdateRect(&rt);
	if (!res) 	// LAM - 8/19/03 - defect 470189
	{
		rt.SetEmpty();
		rt.SetWH(gw->getWinSizeX(),gw->getWinSizeY());
	}

	return new Box2Value(rt);
}

Value*
updateScreen_cf(Value** arg_list, int count)
{
	check_arg_count(updateScreen, 0, count);
	GraphicsWindow	*gw		= MAXScript_interface->GetActiveViewExp().getGW();		
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	gw->updateScreen();
	
	return &ok;
}

Value*
setRndLimits_cf(Value** arg_list, int count)
{
	check_arg_count(setRndLimits, 1, count);
	def_render_types();
	GraphicsWindow	*gw		= MAXScript_interface->GetActiveViewExp().getGW();
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	Array			*arr	= (Array*)arg_list[0];
	DWORD			lim		=0;

	type_check(arr, Array, _T("setRndLimits"));
	for (int i=0; i < arr->size; i++)
		lim |= GetID(renderTypes, elements(renderTypes), arr->data[i]);
	gw->setRndLimits(lim);
	return &ok;
}

Value*
getRndLimits_cf(Value** arg_list, int count)
{
	check_arg_count(getRndLimits, 0, count);
	def_render_types();
	one_typed_value_local(Array* result);	
	GraphicsWindow	*gw		= MAXScript_interface->GetActiveViewExp().getGW();
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	DWORD			lim		= gw->getRndLimits();

	vl.result = new Array(3);
	for (int i=0; i < elements(renderTypes); i++)
		if ((renderTypes[i].id) & lim) 
			vl.result->append(renderTypes[i].val);	
	
	return_value (vl.result); // LAM - 5/18/01 - was return vl.result
}

Value*
getRndMode_cf(Value** arg_list, int count)
{
	check_arg_count(getRndMode, 0, count);
	def_render_types();
	one_typed_value_local(Array* result);
	GraphicsWindow	*gw		= MAXScript_interface->GetActiveViewExp().getGW();
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	DWORD			mode	= gw->getRndMode();

	vl.result = new Array(3);
	for (int i=0; i < elements(renderTypes); i++)
		if ((renderTypes[i].id) & mode) 
			vl.result->append(renderTypes[i].val);		
	return_value (vl.result); // LAM - 5/18/01 - was return vl.result
}

Value*
getMaxLights_cf(Value** arg_list, int count)
{
	check_arg_count(getMaxLights, 0, count);
	GraphicsWindow	*gw		= MAXScript_interface->GetActiveViewExp().getGW();	
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	int				n		= gw->getMaxLights();	
	return Integer::intern(n);
}

Value*
hTransPoint_cf(Value** arg_list, int count)
{
	check_arg_count(hTransPoint, 1, count);
	GraphicsWindow	*gw		= MAXScript_interface->GetActiveViewExp().getGW();	
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	Point3			in		= arg_list[0]->to_point3();
	IPoint3			out;

	gw->hTransPoint(&in, &out);
	return new Point3Value((float)out.x, (float)out.y, (float)out.z);
}

Value*
wTransPoint_cf(Value** arg_list, int count)
{
	check_arg_count(wTransPoint, 1, count);
	GraphicsWindow	*gw		= MAXScript_interface->GetActiveViewExp().getGW();
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	Point3			in		= arg_list[0]->to_point3();
	IPoint3			out;

	gw->wTransPoint(&in, &out);
	return new Point3Value((float)out.x, (float)out.y, (float)out.z);
}

Value*
transPoint_cf(Value** arg_list, int count)
{
	check_arg_count(transPoint, 1, count);
	GraphicsWindow	*gw		= MAXScript_interface->GetActiveViewExp().getGW();
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	Point3			in		= arg_list[0]->to_point3();
	Point3			out;

	gw->transPoint(&in, &out);
	return new Point3Value((float)out.x, (float)out.y, (float)out.z);
}

Value*
getFlipped_cf(Value** arg_list, int count)
{
	check_arg_count(getFlipped, 0, count);
	GraphicsWindow	*gw		= MAXScript_interface->GetActiveViewExp().getGW();
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	BOOL			res		= gw->getFlipped();	
	return (res) ? &true_value : &false_value;
}

Value*
setColor_cf(Value** arg_list, int count)
{
	check_arg_count(setColor, 2, count);
	def_color_types();
	AColor			col		= arg_list[1]->to_acolor();
	ViewExp&		vpt		= MAXScript_interface->GetActiveViewExp();
	GraphicsWindow	*gw			= vpt.getGW();
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	ColorType clrType = (ColorType)GetID(colorTypes, elements(colorTypes), arg_list[0]);
	gw->setColor(clrType, col.r, col.g, col.b);
	return &ok;
}

Value*
clearScreen_cf(Value** arg_list, int count)
{
	check_arg_count_with_keys(clearScreen, 1, count);
	Box2			rect	= arg_list[0]->to_box2();
	GraphicsWindow	*gw		= MAXScript_interface->GetActiveViewExp().getGW();
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	gw->clearScreen(&rect, key_arg_or_default(useBkg, &false_value)->to_bool());
	return &ok;
}

Value*
hText_cf(Value** arg_list, int count)
{
	check_arg_count_with_keys(hText, 2, count);
	Value*			col_val = key_arg(color);
	GraphicsWindow	*gw		= MAXScript_interface->GetActiveViewExp().getGW();	
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	gw->setColor(TEXT_COLOR, 
		(col_val == &unsupplied) ? Point3(1, 0, 0) : (col_val->to_point3()/255.0f));	
	gw->hText(&(to_ipoint3(arg_list[0])), arg_list[1]->to_string());	
	return &ok;
}

Value*
hMarker_cf(Value** arg_list, int count)
{
	check_arg_count_with_keys(hMarker, 2, count);	
	def_marker_types();

	Value*			col_val = key_arg(color);	
	MarkerType		mt = (MarkerType)GetID(markerTypes, elements(markerTypes), arg_list[1]);
	GraphicsWindow* gw = MAXScript_interface->GetActiveViewExp().getGW();
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	// LAM - 8/19/03 - defect 470189
	DWORD			lim		= gw->getRndLimits();
	BOOL			resetLimit = lim & GW_Z_BUFFER;
	if (resetLimit) gw->setRndLimits(lim & ~GW_Z_BUFFER);

	gw->setColor(LINE_COLOR, 
		(col_val == &unsupplied) ? Point3(1, 0, 0) : col_val->to_point3() / 255.0f );
	gw->hMarker(&(to_ipoint3(arg_list[0])), mt);	

	if (resetLimit) gw->setRndLimits(lim);

	return &ok;
}

Value*
hPolyline_cf(Value** arg_list, int count)
{
	check_arg_count_with_keys(hPolyline, 2, count);
	type_check(arg_list[0], Array, _T("hPolyline"));

	Array*			pts_val	= (Array*)arg_list[0];
	int				ct		= pts_val->size; if (!ct) return &undefined; // Return if an empty array is passed
	GraphicsWindow	*gw		= MAXScript_interface->GetActiveViewExp().getGW();				 
	Point3*			col		= NULL;

	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	IPoint3* pts = new IPoint3[ct]; 
	for (int i=0; i < ct; i++)
		pts[i] = to_ipoint3(pts_val->data[i]);

	if (key_arg(rgb) != &unsupplied) {
		type_check(key_arg(rgb), Array, _T("hPolyline"));
		Array* col_val = (Array*)key_arg(rgb);		 		
		if (ct != col_val->size)
			throw RuntimeError(MaxSDK::GetResourceStringAsMSTR(IDS_RK_INVALID_RGB_ARRAY_SIZE));
		col = new Point3[ct];
		for (int i=0; i < ct; i++)
			col[i] = col_val->data[i]->to_point3()/255.f;
	}

	// LAM - 8/19/03 - defect 470189
	DWORD			lim		= gw->getRndLimits();
	BOOL			resetLimit = lim & GW_Z_BUFFER;
	if (resetLimit) gw->setRndLimits(lim & ~GW_Z_BUFFER);

	gw->hPolyline(ct, pts, col, arg_list[1]->to_bool(), NULL);	

	if (resetLimit) gw->setRndLimits(lim);

	delete [] pts;
	if (col) delete [] col;
	return &ok;
}

Value*
hPolygon_cf(Value** arg_list, int count)
{
	check_arg_count(hPolygon, 3, count);
	for (int i=0; i < count; i++)
		type_check(arg_list[i], Array, _T("hPolygon"));

	Array			*pts_val = (Array*)arg_list[0], 
		*col_val = (Array*)arg_list[1],
		*uvw_val = (Array*)arg_list[2];	
	int				ct		 = pts_val->size; if (!ct) return &undefined; // Return if an empty array is passed
	GraphicsWindow	*gw		 = MAXScript_interface->GetActiveViewExp().getGW();
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	if (col_val->size != ct)
		throw RuntimeError(MaxSDK::GetResourceStringAsMSTR(IDS_RK_INVALID_RGB_ARRAY_SIZE));
	if (uvw_val->size != ct)
		throw RuntimeError(MaxSDK::GetResourceStringAsMSTR(IDS_RK_INVALID_UVW_ARRAY_SIZE));

	IPoint3* pts = new IPoint3[ct]; 
	Point3*	 col = new Point3[ct]; 
	Point3*	 uvw = new Point3[ct]; 
	for (int i=0; i < ct; i++) {
		pts[i] = to_ipoint3(pts_val->data[i]);
		col[i] = col_val->data[i]->to_point3()/255.f;
		uvw[i] = uvw_val->data[i]->to_point3();
	}	

	// LAM - 8/19/03 - defect 470189
	DWORD			lim		= gw->getRndLimits();
	BOOL			resetLimit = lim & GW_Z_BUFFER;
	if (resetLimit) gw->setRndLimits(lim & ~GW_Z_BUFFER);

	gw->hPolygon(ct, pts, col, uvw);	

	if (resetLimit) gw->setRndLimits(lim);

	delete [] pts;
	delete [] col;
	delete [] uvw;
	return &ok;
}

Value*
triStrip_cf(Value** arg_list, int count)
{
	check_arg_count(triStrip, 3, count);
	for (int i=0; i < count; i++)
		type_check(arg_list[i], Array, _T("triStrip"));

	Array			*pts_val = (Array*)arg_list[0], 
		*col_val = (Array*)arg_list[1],
		*uvw_val = (Array*)arg_list[2];	
	int				ct		 = pts_val->size; if (!ct) return &undefined; // Return if an empty array is passed
	GraphicsWindow	*gw		 = MAXScript_interface->GetActiveViewExp().getGW();	
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	if (col_val->size != ct)
		throw RuntimeError(MaxSDK::GetResourceStringAsMSTR(IDS_RK_INVALID_RGB_ARRAY_SIZE));
	if (uvw_val->size != ct)
		throw RuntimeError(MaxSDK::GetResourceStringAsMSTR(IDS_RK_INVALID_UVW_ARRAY_SIZE));

	Point3* pts = new Point3[ct]; 
	Point3*	 col = new Point3[ct]; 
	Point3*	 uvw = new Point3[ct]; 
	for (int i=0; i < ct; i++) {
		pts[i] = pts_val->data[i]->to_point3();
		col[i] = col_val->data[i]->to_point3()/255.f;
		uvw[i] = uvw_val->data[i]->to_point3();
	}	

	gw->triStrip(ct, pts, col, uvw);

	delete [] pts;
	delete [] col;
	delete [] uvw;
	return &ok;
}


Value*
hTriStrip_cf(Value** arg_list, int count)
{
	check_arg_count(hTriStrip, 3, count);
	for (int i=0; i < count; i++)
		type_check(arg_list[i], Array, _T("hTriStrip"));

	Array			*pts_val = (Array*)arg_list[0], 
		*col_val = (Array*)arg_list[1],
		*uvw_val = (Array*)arg_list[2];	
	int				ct		 = pts_val->size; if (!ct) return &undefined; // Return if an empty array is passed
	GraphicsWindow	*gw		 = MAXScript_interface->GetActiveViewExp().getGW();

	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	if (col_val->size != ct)
		throw RuntimeError(MaxSDK::GetResourceStringAsMSTR(IDS_RK_INVALID_RGB_ARRAY_SIZE));
	if (uvw_val->size != ct)
		throw RuntimeError(MaxSDK::GetResourceStringAsMSTR(IDS_RK_INVALID_UVW_ARRAY_SIZE));

	IPoint3* pts = new IPoint3[ct]; 
	Point3*	 col = new Point3[ct]; 
	Point3*	 uvw = new Point3[ct]; 
	for (int i=0; i < ct; i++) {
		pts[i] = to_ipoint3(pts_val->data[i]);
		col[i] = col_val->data[i]->to_point3()/255.f;
		uvw[i] = uvw_val->data[i]->to_point3();
	}	

	// LAM - 8/19/03 - defect 470189
	DWORD			lim		= gw->getRndLimits();
	BOOL			resetLimit = lim & GW_Z_BUFFER;
	if (resetLimit) gw->setRndLimits(lim & ~GW_Z_BUFFER);

	gw->hTriStrip(ct, pts, col, uvw);

	if (resetLimit) gw->setRndLimits(lim);

	delete [] pts;
	delete [] col;
	delete [] uvw;
	return &ok;
}

Value*
wText_cf(Value** arg_list, int count)
{
	check_arg_count_with_keys(wText, 2, count);
	GraphicsWindow	*gw		= MAXScript_interface->GetActiveViewExp().getGW();	
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	Value*			col_val = key_arg(color);

	gw->setColor(TEXT_COLOR, 
		(col_val == &unsupplied) ? Point3(1, 0, 0) : (col_val->to_point3()/255.0f));	
	gw->wText(&(to_ipoint3(arg_list[0])), arg_list[1]->to_string());	
	return &ok;
}

Value*
wMarker_cf(Value** arg_list, int count)
{
	check_arg_count_with_keys(wMarker, 2, count);	
	def_marker_types();

	Value*			col_val = key_arg(color);	
	MarkerType		mt = (MarkerType)GetID(markerTypes, elements(markerTypes), arg_list[1]);
	GraphicsWindow* gw = MAXScript_interface->GetActiveViewExp().getGW();

	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	// LAM - 8/19/03 - defect 470189
	DWORD			lim		= gw->getRndLimits();
	BOOL			resetLimit = lim & GW_Z_BUFFER;
	if (resetLimit) gw->setRndLimits(lim & ~GW_Z_BUFFER);

	gw->setColor(LINE_COLOR, 
		(col_val == &unsupplied) ? Point3(1, 0, 0) : col_val->to_point3()/255.f);
	gw->wMarker(&(to_ipoint3(arg_list[0])), mt);

	if (resetLimit) gw->setRndLimits(lim);

	return &ok;
}

Value*
wPolyline_cf(Value** arg_list, int count)
{
	check_arg_count_with_keys(wPolyline, 2, count);
	type_check(arg_list[0], Array, _T("wPolyline"));

	Array*			pts_val	= (Array*)arg_list[0];
	int				ct		= pts_val->size; if (!ct) return &undefined; // Return if an empty array is passed			 	
	GraphicsWindow	*gw		= MAXScript_interface->GetActiveViewExp().getGW();
	Point3*			col		= NULL;
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	IPoint3* pts = new IPoint3[ct]; 
	for (int i=0; i < ct; i++)
		pts[i] = to_ipoint3(pts_val->data[i]);

	if (key_arg(rgb) != &unsupplied) {
		type_check(key_arg(rgb), Array, _T("wPolyline"));		
		Array* col_val = (Array*)key_arg(rgb);		 		
		if (ct != col_val->size)
			throw RuntimeError(MaxSDK::GetResourceStringAsMSTR(IDS_RK_INVALID_RGB_ARRAY_SIZE));
		col = new Point3[ct];
		for (int i=0; i < ct; i++)
			col[i] = col_val->data[i]->to_point3()/255.f;
	}

	// LAM - 8/19/03 - defect 470189
	DWORD			lim		= gw->getRndLimits();
	BOOL			resetLimit = lim & GW_Z_BUFFER;
	if (resetLimit) gw->setRndLimits(lim & ~GW_Z_BUFFER);

	gw->wPolyline(ct, pts, col, arg_list[1]->to_bool(), NULL);	

	if (resetLimit) gw->setRndLimits(lim);

	delete [] pts;
	if (col) delete [] col;
	return &ok;
}

Value*
wPolygon_cf(Value** arg_list, int count)
{
	check_arg_count(wPolygon, 3, count);
	for (int i=0; i < count; i++)
		type_check(arg_list[i], Array, _T("wPolygon"));

	Array			*pts_val = (Array*)arg_list[0], 
		*col_val = (Array*)arg_list[1],
		*uvw_val = (Array*)arg_list[2];	
	int				ct		 = pts_val->size; if (!ct) return &undefined; // Return if an empty array is passed	
	GraphicsWindow	*gw		 = MAXScript_interface->GetActiveViewExp().getGW();	
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	if (col_val->size != ct)
		throw RuntimeError(MaxSDK::GetResourceStringAsMSTR(IDS_RK_INVALID_RGB_ARRAY_SIZE));
	if (uvw_val->size != ct)
		throw RuntimeError(MaxSDK::GetResourceStringAsMSTR(IDS_RK_INVALID_UVW_ARRAY_SIZE));

	IPoint3* pts = new IPoint3[ct]; 
	Point3*	 col = new Point3[ct]; 
	Point3*	 uvw = new Point3[ct]; 
	for (int i=0; i < ct; i++) {
		pts[i] = to_ipoint3(pts_val->data[i]);
		col[i] = col_val->data[i]->to_point3()/255.f;
		uvw[i] = uvw_val->data[i]->to_point3();
	}	

	// LAM - 8/19/03 - defect 470189
	DWORD			lim		= gw->getRndLimits();
	BOOL			resetLimit = lim & GW_Z_BUFFER;
	if (resetLimit) gw->setRndLimits(lim & ~GW_Z_BUFFER);

	gw->wPolygon(ct, pts, col, uvw);	

	if (resetLimit) gw->setRndLimits(lim);

	delete [] pts;
	delete [] col;
	delete [] uvw;
	return &ok;

}

Value*
wTriStrip_cf(Value** arg_list, int count)
{
	check_arg_count(wTriStrip, 3, count);
	for (int i=0; i < count; i++)
		type_check(arg_list[i], Array, _T("wTriStrip"));

	Array			*pts_val = (Array*)arg_list[0], 
		*col_val = (Array*)arg_list[1],
		*uvw_val = (Array*)arg_list[2];	
	int				ct		 = pts_val->size; if (!ct) return &undefined; // Return if an empty array is passed
	GraphicsWindow	*gw		 = MAXScript_interface->GetActiveViewExp().getGW();	
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	if (col_val->size != ct)
		throw RuntimeError(MaxSDK::GetResourceStringAsMSTR(IDS_RK_INVALID_RGB_ARRAY_SIZE));
	if (uvw_val->size != ct)
		throw RuntimeError(MaxSDK::GetResourceStringAsMSTR(IDS_RK_INVALID_UVW_ARRAY_SIZE));

	IPoint3* pts = new IPoint3[ct]; 
	Point3*	 col = new Point3[ct]; 
	Point3*	 uvw = new Point3[ct]; 
	for (int i=0; i < ct; i++) {
		pts[i] = to_ipoint3(pts_val->data[i]);
		col[i] = col_val->data[i]->to_point3()/255.f;
		uvw[i] = uvw_val->data[i]->to_point3();
	}	

	// LAM - 8/19/03 - defect 470189
	DWORD			lim		= gw->getRndLimits();
	BOOL			resetLimit = lim & GW_Z_BUFFER;
	if (resetLimit) gw->setRndLimits(lim & ~GW_Z_BUFFER);

	gw->wTriStrip(ct, pts, col, uvw);

	if (resetLimit) gw->setRndLimits(lim);

	delete [] pts;
	delete [] col;
	delete [] uvw;
	return &ok;

}

Value*
wRect_cf(Value** arg_list, int count)
{
	check_arg_count(wRect, 2, count);
	Box2 &rect = arg_list[0]->to_box2();
	Point3 color = arg_list[1]->to_point3()/255.f;

	GraphicsWindow	*gw		= MAXScript_interface->GetActiveViewExp().getGW();		
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	Point3*	 col = NULL; // new Point3[2];
	IPoint3* pts = new IPoint3[2]; 

//	col[0]=col[1]=color;
	pts[0]=IPoint3((int)(rect.left),0.f,0.f);
	pts[1]=IPoint3((int)(rect.right),0.f,0.f);

	gw->setColor(LINE_COLOR, color);
	for (int j = rect.top; j <= rect.bottom; j++)
	{
		pts[0].y=pts[1].y=j;
		gw->wPolyline(2, pts, col, FALSE, NULL);	
	}

	delete [] pts;
//	delete [] col;
	return &ok;
}

Value*
hRect_cf(Value** arg_list, int count)
{
	check_arg_count(hRect, 2, count);
	Box2 &rect = arg_list[0]->to_box2();
	Point3 color = arg_list[1]->to_point3()/255.f;

	GraphicsWindow	*gw		= MAXScript_interface->GetActiveViewExp().getGW();		
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	Point3*	 col = NULL; // new Point3[2];
	IPoint3* pts = new IPoint3[2]; 

//	col[0]=col[1]=color;
	pts[0]=IPoint3((int)(rect.left),0.f,0.f);
	pts[1]=IPoint3((int)(rect.right),0.f,0.f);

	gw->setColor(LINE_COLOR, color);
	for (int j = rect.top; j <= rect.bottom; j++)
	{
		pts[0].y=pts[1].y=j;
		gw->hPolyline(2, pts, col, FALSE, NULL);	
	}

	delete [] pts;
//	delete [] col;
	return &ok;
}

Value*
text_cf(Value** arg_list, int count)
{
	check_arg_count_with_keys(text, 2, count);
	GraphicsWindow	*gw		= MAXScript_interface->GetActiveViewExp().getGW();	
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	Value*			col_val = key_arg(color);

	gw->setColor(TEXT_COLOR, 
		(col_val == &unsupplied) ? Point3(1, 0, 0) : (col_val->to_point3()/255.0f));	
	gw->text(&(arg_list[0]->to_point3()), arg_list[1]->to_string());
	/*gw->resetUpdateRect();
	gw->enlargeUpdateRect(NULL);
	gw->updateScreen();*/
	return &ok;
}

Value*
marker_cf(Value** arg_list, int count)
{
	check_arg_count_with_keys(marker, 2, count);	
	def_marker_types();

	Value*			col_val = key_arg(color);	
	MarkerType		mt = (MarkerType)GetID(markerTypes, elements(markerTypes), arg_list[1]);
	GraphicsWindow* gw = MAXScript_interface->GetActiveViewExp().getGW();
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	// LAM - 8/19/03 - defect 470189
	DWORD			lim		= gw->getRndLimits();
	BOOL			resetLimit = lim & GW_Z_BUFFER;
	if (resetLimit) gw->setRndLimits(lim & ~GW_Z_BUFFER);

	gw->setColor(LINE_COLOR, 
		(col_val == &unsupplied) ? Point3(1, 0, 0) : col_val->to_point3()/255.f);
	gw->marker(&(arg_list[0]->to_point3()), mt);

	if (resetLimit) gw->setRndLimits(lim);

	return &ok;
}

Value*
polyline_cf(Value** arg_list, int count)
{
	check_arg_count_with_keys(polyline, 2, count);
	type_check(arg_list[0], Array, _T("Polyline"));

	Array*			pts_val	= (Array*)arg_list[0];
	int				ct		= pts_val->size; if (!ct) return &undefined; // Return if an empty array is passed
	GraphicsWindow	*gw		= MAXScript_interface->GetActiveViewExp().getGW();	
	Point3*			col		= NULL;	
	Point3*			pts		= new Point3[ct+1]; // one extra element per sdk
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	for (int i=0; i < ct; i++)
		pts[i] = pts_val->data[i]->to_point3();

	if (key_arg(rgb) != &unsupplied) {
		type_check(key_arg(rgb), Array, _T("Polyline"));		
		Array* col_val = (Array*)key_arg(rgb);		 		
		if (ct != col_val->size)
			throw RuntimeError(MaxSDK::GetResourceStringAsMSTR(IDS_RK_INVALID_RGB_ARRAY_SIZE));
		col = new Point3[ct+1];
		for (int i=0; i < ct; i++)
			col[i] =  col_val->data[i]->to_point3()/255.f;
	}

	// LAM - 8/19/03 - defect 470189
	DWORD			lim		= gw->getRndLimits();
	BOOL			resetLimit = lim & GW_Z_BUFFER;
	if (resetLimit) gw->setRndLimits(lim & ~GW_Z_BUFFER);

	gw->polyline(ct, pts, col, arg_list[1]->to_bool(), NULL);	

	if (resetLimit) gw->setRndLimits(lim);

	delete [] pts;
	if (col) delete [] col;
	return &ok;
}

Value*
polygon_cf(Value** arg_list, int count)
{
	check_arg_count(polygon, 3, count);
	for (int i=0; i < count; i++)
		type_check(arg_list[i], Array, _T("Polygon"));

	Array			*pts_val = (Array*)arg_list[0], 
		*col_val = (Array*)arg_list[1],
		*uvw_val = (Array*)arg_list[2];	
	int				ct		 = pts_val->size; if (!ct) return &undefined; // Return if an empty array is passed	
	GraphicsWindow	*gw		 = MAXScript_interface->GetActiveViewExp().getGW();
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	if (col_val->size != ct)
		throw RuntimeError(MaxSDK::GetResourceStringAsMSTR(IDS_RK_INVALID_RGB_ARRAY_SIZE));
	if (uvw_val->size != ct)
		throw RuntimeError(MaxSDK::GetResourceStringAsMSTR(IDS_RK_INVALID_UVW_ARRAY_SIZE));

	Point3* pts = new Point3[ct+1];  // one extra element per sdk
	Point3*	col = new Point3[ct+1]; 
	Point3*	uvw = new Point3[ct+1]; 
	for (int i=0; i < ct; i++) {
		pts[i] = pts_val->data[i]->to_point3();
		col[i] = col_val->data[i]->to_point3()/255.f;
		uvw[i] = uvw_val->data[i]->to_point3();
	}	

	// LAM - 8/19/03 - defect 470189
	DWORD			lim		= gw->getRndLimits();
	BOOL			resetLimit = lim & GW_Z_BUFFER;
	if (resetLimit) gw->setRndLimits(lim & ~GW_Z_BUFFER);

	gw->polygon(ct, pts, col, uvw);	

	if (resetLimit) gw->setRndLimits(lim);

	delete [] pts;
	delete [] col;
	delete [] uvw;
	return &ok;
}

Value*
startTriangles_cf(Value** arg_list, int count)
{
	check_arg_count(startTriangles, 0, count);
	GraphicsWindow	*gw		= MAXScript_interface->GetActiveViewExp().getGW();		
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	gw->startTriangles();
	return &ok;
}

Value*
endTriangles_cf(Value** arg_list, int count)
{
	check_arg_count(endTriangles, 0, count);
	GraphicsWindow	*gw		= MAXScript_interface->GetActiveViewExp().getGW();		
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	gw->endTriangles();
	return &ok;
}

Value*
triangle_cf(Value** arg_list, int count)
{
	check_arg_count(triangle, 2, count);
	for (int i=0; i < count; i++)
		type_check(arg_list[i], Array, _T("triangle"));

	Array			*pts_val = (Array*)arg_list[0], 
	*col_val = (Array*)arg_list[1];
	GraphicsWindow	*gw		 = MAXScript_interface->GetActiveViewExp().getGW();	
	
	if (MaxSDK::Graphics::IsRetainedModeEnabled() && gw->querySupport(GW_SPT_NUM_LIGHTS) == 0)
	{
		return &undefined;
	}

	if (col_val->size != 3)
		throw RuntimeError(MaxSDK::GetResourceStringAsMSTR(IDS_RK_RGB_ARRAY_SIZE_NOT_3));
	if (pts_val->size != 3)
		throw RuntimeError(MaxSDK::GetResourceStringAsMSTR(IDS_RK_VERT_ARRAY_SIZE_NOT_3));

	Point3* pts = new Point3[3]; 
	Point3*	 col = new Point3[3]; 
	for (int i=0; i < 3; i++) {
		pts[i] = pts_val->data[i]->to_point3();
		col[i] = col_val->data[i]->to_point3()/255.f;
	}	

	//	virtual void	triangle(Point3 *xyz, Point3 *rgb) = 0;
	gw->triangle(pts, col);

	delete [] pts;
	delete [] col;
	return &ok;
}

Value*
getTextExtent_gw_cf(Value** arg_list, int count)
{
	check_arg_count_with_keys(getTextExtent, 1, count);
	const TCHAR			*text	= arg_list[0]->to_string();
	GraphicsWindow	*gw		= MAXScript_interface->GetActiveViewExp().getGW();	
	SIZE size;
	gw->getTextExtents(text, &size);
	return new Point2Value((float)size.cx, (float)size.cy);
}


/*-----------------------------------ViewExp-------------------------------------------*/

Value*
NonScalingObjectSize_cf(Value** arg_list, int count)
{
	check_arg_count(NonScalingObjectSize, 0, count);
	float ret = MAXScript_interface->GetActiveViewExp().NonScalingObjectSize();
	return Float::intern(ret);
}

Value*
GetPointOnCP_cf(Value** arg_list, int count)
{
	check_arg_count(GetPointOnCP, 1, count);
	IPoint2 val = to_ipoint2(arg_list[0]);
	Point3 ret = MAXScript_interface->GetActiveViewExp().GetPointOnCP(val);
	return new Point3Value(ret);
}

Value*
GetCPDisp_cf(Value** arg_list, int count)
{
	check_arg_count(GetCPDisp, 4, count);
	Point3 v1 = arg_list[0]->to_point3();
	Point3 v2 = arg_list[1]->to_point3();
	IPoint2 v3 = to_ipoint2(arg_list[2]);
	IPoint2 v4 = to_ipoint2(arg_list[3]);
	float ret = MAXScript_interface->GetActiveViewExp().GetCPDisp(v1,v2,v3,v4);		
	return Float::intern(ret);
}

Value*
GetVPWorldWidth_cf(Value** arg_list, int count)
{
	check_arg_count_with_keys(GetVPWorldWidth, 1, count);
	Point3 val = arg_list[0]->to_point3();
	float ret = MAXScript_interface->GetActiveViewExp().GetVPWorldWidth(val);
	return Float::intern(ret);
}

Value*
MapCPToWorld_cf(Value** arg_list, int count)
{
	check_arg_count(MapCPToWorld, 1, count);

	Point3 val = arg_list[0]->to_point3();
	Point3	ret = MAXScript_interface->GetActiveViewExp().MapCPToWorld(val);
	return new Point3Value(ret);
}

Value*
SnapPoint_cf(Value** arg_list, int count)
{
	check_arg_count_with_keys(SnapPoint, 1, count);
	def_snap_types();
	IPoint2 out;
	IPoint2 loc = to_ipoint2(arg_list[0]);

	Value	*val = key_arg(snapType);
	int		flags = (val == &unsupplied) ? 0 : GetID(snapTypes, elements(snapTypes), val);

	Value *snapPlane = key_arg(snapPlane);
	Matrix3* plane = NULL;
	if (snapPlane != &unsupplied) {
		if (!snapPlane->is_kind_of(class_tag(Matrix3Value)))
			throw TypeError (_T("snapPlane requires a Matrix3 value"), snapPlane);

		Matrix3Value* mv = static_cast<Matrix3Value*>(snapPlane);
		plane = new Matrix3(mv->m);
	}
	Point3	ret = MAXScript_interface->GetActiveViewExp().SnapPoint(loc, out, plane, flags);
	if (plane != NULL)
		delete plane;
	return new Point3Value(ret);
}

Value* 
SnapLength_cf(Value** arg_list, int count)
{
	check_arg_count(SnapLength, 1, count);
	float len = arg_list[0]->to_float();
	float ret = MAXScript_interface->GetActiveViewExp().SnapLength(len);
	return Float::intern(ret);
}

Value*
IsPerspView_cf(Value** arg_list, int count)
{
	check_arg_count(IsPerspView, 0, count);
	Value* ret = MAXScript_interface->GetActiveViewExp().IsPerspView() ? &true_value : &false_value;
	return ret;
}

Value*
GetFocalDist_cf(Value** arg_list, int count)
{
	check_arg_count(GetFocalDist, 0, count);
	float ret = MAXScript_interface->GetActiveViewExp().GetFocalDist();
	return Float::intern(ret);
}
