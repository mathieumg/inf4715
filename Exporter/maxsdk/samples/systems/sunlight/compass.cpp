//**************************************************************************/
// Copyright (c) 1998-2007 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
/**********************************************************************
*<
FILE: compass.cpp

DESCRIPTION:  A compass rose helper implementation

CREATED BY: John  Hutchinson Derived from the point helper

HISTORY: Oct 22, 1996

*>   Copyright (c) 1995, All Rights Reserved.
**********************************************************************/

#include "compass.h"
#include <3dsmaxport.h>
#include "sunlight.h"
#include <DllUtilities.h>
#include <gfx.h>
#include "verts.h"

//------------------------------------------------------

#define BASESCALE 5.0f
#define ICONSCALE 5.0f
#define MIN_ZOOM 0.0001f

void AxisViewportRect(ViewExp *vpt, const Matrix3 &tm, float length, Rect *rect);
void DrawAxis(ViewExp *vpt,  float length, BOOL sel=FALSE, BOOL frozen=FALSE);
Box3 GetAxisBox(ViewExp *vpt, const Matrix3 &tm,float length,int resetTM);


// class variable for point class.
HWND CompassRoseObject::hParams = NULL;
IObjParam *CompassRoseObject::iObjParams;

int CompassRoseObject::dlgShowAxis = TRUE;
float CompassRoseObject::dlgAxisLength = AXIS_LENGTH;
CompassRoseObjCreateCallBack CompassRoseObject::sCompassRoseCreateCB;

INT_PTR CALLBACK PointParamProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	CompassRoseObject *po = DLGetWindowLongPtr<CompassRoseObject*>(hWnd);
	if (!po && msg!=WM_INITDIALOG) return FALSE;

	switch (msg) {
	case WM_INITDIALOG: {
		po = (CompassRoseObject*)lParam;
		DLSetWindowLongPtr(hWnd, lParam);
		CheckDlgButton(hWnd,IDC_SHOWAXIS,po->showAxis);

		ISpinnerControl *spin = 
			GetISpinner(GetDlgItem(hWnd,IDC_AXISLENGHSPIN));
		//       spin->SetLimits(0,1000,FALSE);
		spin->SetLimits(AXIS_LENGTH,1000.0f,FALSE);
		// alexc | 03.06.09 | increments proportional to the spinner value
		spin->SetAutoScale();
		spin->SetValue(po->axisLength,FALSE);
		// alexc | 03.06.09 | this is a value in world space units (not plain EDITTYPE_FLOAT)
		spin->LinkToEdit(GetDlgItem(hWnd,IDC_AXISLENGTH),EDITTYPE_UNIVERSE);
		ReleaseISpinner(spin);
		return FALSE;
						}

	case CC_SPINNER_CHANGE: {
		ISpinnerControl *spin = (ISpinnerControl*)lParam;
		po->axisLength = spin->GetFVal();
		po->NotifyDependents(FOREVER,PART_OBJ,REFMSG_CHANGE);
		po->iObjParams->RedrawViews(po->iObjParams->GetTime());
		break;
							}

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_SHOWAXIS:
			po->showAxis = IsDlgButtonChecked(hWnd,IDC_SHOWAXIS);
			po->NotifyDependents(FOREVER,PART_OBJ,REFMSG_CHANGE);
			po->iObjParams->RedrawViews(po->iObjParams->GetTime());
			break;
		}
		break;

	case RU_UPDATE:{
		ISpinnerControl *spin = 
			GetISpinner(GetDlgItem(hWnd,IDC_AXISLENGHSPIN));
		if(spin)
		{
			spin->SetValue(po->axisLength,FALSE);
			ReleaseISpinner(spin);
		}
				   }
				   break;

	default:
		return FALSE;
	}

	return TRUE;
} 

MSTR CompassRoseObject::sObjectName = MaxSDK::GetResourceStringAsMSTR(IDS_DB_COMPASS_OBJECT); // static member initialization
const TCHAR* CompassRoseObject::GetObjectName() 
{ 
	return sObjectName.data();
}

void CompassRoseObject::BeginEditParams(
	IObjParam *ip, ULONG flags,Animatable *prev)
{  
	iObjParams = ip;
	if (!hParams) {
		hParams = ip->AddRollupPage( 
			MaxSDK::GetHInstance(), 
			MAKEINTRESOURCE(IDD_COMPASSPARAM),
			PointParamProc, 
			MaxSDK::GetResourceStringAsMSTR(IDS_DB_PARAMETERS).data(), 
			(LPARAM)this );
		ip->RegisterDlgWnd(hParams);
	} else {
		DLSetWindowLongPtr(hParams, this);
		CheckDlgButton(hParams,IDC_SHOWAXIS,showAxis);
		ISpinnerControl *spin = 
			GetISpinner(GetDlgItem(hParams,IDC_AXISLENGHSPIN));
		spin->SetValue(axisLength,FALSE);
		ReleaseISpinner(spin);
	}
}

void CompassRoseObject::EndEditParams(
	IObjParam *ip, ULONG flags,Animatable *next)
{  
	dlgShowAxis = IsDlgButtonChecked(hParams, IDC_SHOWAXIS );
	ISpinnerControl *spin = GetISpinner(GetDlgItem(hParams,IDC_AXISLENGHSPIN));
	dlgAxisLength = spin->GetFVal();
	ReleaseISpinner(spin);
	if (flags&END_EDIT_REMOVEUI) {
		ip->UnRegisterDlgWnd(hParams);
		ip->DeleteRollupPage(hParams);
		hParams = NULL;
	} else {
		DLSetWindowLongPtr(hParams, 0);
	}
	iObjParams = NULL;
}


CompassRoseObject::CompassRoseObject()
{  
	showAxis = dlgShowAxis;
	axisLength = dlgAxisLength;
	suspendSnap = FALSE;
}

CompassRoseObject::~CompassRoseObject()
{  
}

CreateMouseCallBack* CompassRoseObject::GetCreateMouseCallBack() {
	sCompassRoseCreateCB.SetObj(this);
	return(&sCompassRoseCreateCB);
}

void CompassRoseObject::SetExtendedDisplay(int flags)
{
	extDispFlags = flags;
}

void CompassRoseObject::GetLocalBoundBox(
	TimeValue t, INode* inode, ViewExp* vpt, Box3& box ) 
{
	Matrix3 tm(1);
	tm.SetTrans(inode->GetObjectTM(t).GetTrans());
	box = GetAxisBox(vpt,tm,showAxis?axisLength:0.0f, TRUE);
}

void CompassRoseObject::GetWorldBoundBox(
	TimeValue t, INode* inode, ViewExp* vpt, Box3& box )
{
	Matrix3 tm;
	tm = inode->GetObjectTM(t);
	/*
	if(extDispFlags & EXT_DISP_ZOOM_EXT)
	box = Box3(tm.GetTrans(), tm.GetTrans());
	else
	box = GetAxisBox(vpt,tm,showAxis?axisLength:0.0f, FALSE);
	*/
	box = GetAxisBox(vpt,tm,showAxis?axisLength:0.0f, FALSE);
	assert(!box.IsEmpty());
}


// From BaseObject
int CompassRoseObject::HitTest(TimeValue t, INode *inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt) {

	if ( ! vpt || ! vpt->IsAlive() )
	{
		// why are we here
		DbgAssert(!_T("Invalid viewport!"));
		return FALSE;
	}

	HitRegion hitRegion;
	DWORD savedLimits;

	GraphicsWindow *gw = vpt->getGW();  
	gw->setTransform(idTM);
	MakeHitRegion(hitRegion, type, crossing, 8, p);
	savedLimits = gw->getRndLimits();

	gw->setRndLimits((savedLimits|GW_PICK) & ~GW_ILLUM & ~GW_Z_BUFFER);
	gw->setHitRegion(&hitRegion);
	gw->clearHitCode();

	Matrix3 tm = inode->GetObjectTM(t);    
	gw->setTransform(tm);

	if (showAxis) {
		DrawAxis(vpt,axisLength);
	}
	gw->marker(const_cast<Point3 *>(&Point3::Origin), X_MRKR);
	int res = gw->checkHitCode();

	gw->setRndLimits(savedLimits);
	return res;
}

void CompassRoseObject::Snap(TimeValue t, INode* inode, SnapInfo *snap, IPoint2 *p, ViewExp *vpt)
{

	if ( ! vpt || ! vpt->IsAlive() )
	{
		// why are we here
		DbgAssert(!_T("Invalid viewport!"));
		return;
	}

	if(suspendSnap)
		return;

	Matrix3 tm = inode->GetObjectTM(t); 
	GraphicsWindow *gw = vpt->getGW();  
	gw->setTransform(tm);

	Matrix3 invPlane = Inverse(snap->plane);
	// Check for upside-down coordinate system in viewport
	float ybias = (float)gw->getWinSizeY() - 1.0f;

	// Make sure the vertex priority is active and at least as important as the best snap so far
	if(snap->vertPriority > 0 && snap->vertPriority <= snap->priority) {
		Point2 fp = Point2((float)p->x, (float)p->y);
		Point3 screen3;
		Point2 screen2;

		Point3 thePoint(0,0,0);
		// If constrained to the plane, make sure this point is in it!
		if(snap->snapType == SNAP_2D || snap->flags & SNAP_IN_PLANE) {
			Point3 test = thePoint * tm * invPlane;
			if(fabs(test.z) > 0.0001)  // Is it in the plane (within reason)?
				return;
		}
		//gw->transPointFWin(&thePoint,&screen3);  //this is for MAX 1.2
		gw->transPoint(&thePoint, &screen3);
		screen3.y = ybias - screen3.y;
		screen2.x = screen3.x;
		screen2.y = screen3.y;

		// Are we within the snap radius?
		int len = (int)Length(screen2 - fp);
		if(len <= snap->strength) {
			// Is this priority better than the best so far?
			if(snap->vertPriority < snap->priority) {
				snap->priority = snap->vertPriority;
				snap->bestWorld = thePoint * tm;
				snap->bestScreen = screen2;
				snap->bestDist = len;
			}
			else
				if(len < snap->bestDist) {
					snap->priority = snap->vertPriority;
					snap->bestWorld = thePoint * tm;
					snap->bestScreen = screen2;
					snap->bestDist = len;
				}
		}
	}

}


int CompassRoseObject::Display(TimeValue t, INode* inode, ViewExp *vpt, int flags) {
	if ( ! vpt || ! vpt->IsAlive() )
	{
		// why are we here
		DbgAssert(!_T("Invalid viewport!"));
		return FALSE;
	}

	Matrix3 tm = inode->GetObjectTM(t);
	vpt->getGW()->setTransform(tm);
	if (showAxis) {
		DrawAxis(vpt,axisLength,inode->Selected(),inode->IsFrozen());
	}
	if(!inode->IsFrozen())
		vpt->getGW()->setColor(LINE_COLOR,1.0f,0.9f,0.0f);
	vpt->getGW()->marker(const_cast<Point3 *>(&Point3::Origin), X_MRKR);

	return(0);
}



//
// Reference Management:
//

// This is only called if the object MAKES references to other things.
RefResult CompassRoseObject::NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
	PartID& partID, RefMessage message ) 
{
	return(REF_SUCCEED);
}

ObjectState CompassRoseObject::Eval(TimeValue time){
	return ObjectState(this);
}

RefTargetHandle CompassRoseObject::Clone(RemapDir& remap) {
	CompassRoseObject* newob = new CompassRoseObject();   
	newob->showAxis = showAxis;
	newob->axisLength = axisLength;
	BaseClone(this, newob, remap);
	return(newob);
}


#define SHOW_AXIS_CHUNK    0x0100
#define AXIS_LENGTH_CHUNK  0x0110

IOResult CompassRoseObject::Load(ILoad *iload)
{
	ULONG nb;
	IOResult res = IO_OK;

	while (IO_OK==(res=iload->OpenChunk())) {
		switch (iload->CurChunkID()) {
		case SHOW_AXIS_CHUNK:
			res = iload->Read(&showAxis,sizeof(showAxis),&nb);
			break;
		case AXIS_LENGTH_CHUNK:
			res = iload->Read(&axisLength,sizeof(axisLength),&nb);
			break;
		}

		res = iload->CloseChunk();
		if (res!=IO_OK)  return res;
	}

	return IO_OK;
}

IOResult CompassRoseObject::Save(ISave *isave)
{
	ULONG nb;

	isave->BeginChunk(SHOW_AXIS_CHUNK);
	isave->Write(&showAxis,sizeof(showAxis),&nb);
	isave->EndChunk();

	isave->BeginChunk(AXIS_LENGTH_CHUNK);
	isave->Write(&axisLength,sizeof(axisLength),&nb);
	isave->EndChunk();

	return IO_OK;
}

void CompassRoseObject::InitNodeName( TSTR& s )
{
	s = MaxSDK::GetResourceStringAsMSTR(IDS_DB_COMPASS);
}

void CompassRoseObject::GetClassName( TSTR& s )
{
	s = MaxSDK::GetResourceStringAsMSTR(IDS_DB_COMPASS_CLASS);
}


/*--------------------------------------------------------------------*/
// 
// Stole this from scene.cpp
// Probably couldn't hurt to make an API...
//
//


void Text( ViewExp *vpt, const TCHAR *str, Point3 &pt )
{  
	vpt->getGW()->text( &pt, const_cast<TCHAR*>(str) );  
}

static void DrawAStar( ViewExp *vpt, float len )
{
	GraphicsWindow *gw = vpt->getGW();
	Point3 v[NUMVERTS+1];
	//we'll scale the points to fill out the dlg size, almost
	float mult = (len/ICONSCALE)*(float)0.8;

	/* This is actually more correct but due to a bug in HRIGFX hitTestLIne
	we need to break the pline into small chinks so that bounding box culling will be applied to individual
	segments
	for (int k= 0; k<NUMVERTS;k++)
	{
	v[k]=roseverts[k]*mult;
	}
	gw->polyline( NUMVERTS, v, NULL, NULL, TRUE, NULL );
	*/
	//Begin fix for 75058
	for (int k= 0; k<NUMVERTS-1;k++)
	{
		v[0]=roseverts[k]*mult;
		v[1]=roseverts[k+1]*mult;
		gw->polyline( 2, v, NULL, NULL, FALSE, NULL );
	}
	//explicitly handle closure
	v[0]=roseverts[NUMVERTS-1]*mult;
	v[1]=roseverts[0]*mult;
	gw->polyline( 2, v, NULL, NULL, FALSE, NULL );
	//Begin fix for 75058


	int res = gw->checkHitCode();

	//draw an extra line in the north driection
	v[0] = Point3(0.0f,0.0f,0.0f);
	v[1] = mult * roseverts[3];
	gw->polyline( 2, v, NULL, NULL, FALSE, NULL );
}

#define ZFACT (float).005;
#define AXIS_COLOR      0.4f,0.4f,0.4f
#define SELAXIS_COLOR   1.0f,1.0f,1.0f

void DrawAxis( ViewExp *vpt, float length, BOOL sel, BOOL frozen )
{
	Matrix3 tmn = vpt->getGW()->getTransform();
	Point3 origin = tmn.GetTrans();
	float zoom = 1.0f;
	int limits; 

	// Get width of viewport in world units:  --DS
	zoom = vpt->GetScreenScaleFactor(origin)*ZFACT;
	if (zoom < MIN_ZOOM) zoom = 1.0f;

	length *= zoom;

	limits = vpt->getGW()->getRndLimits();
	// vpt->getGW()->setRndLimits( limits & ~GW_Z_BUFFER );  JH

	if (sel) {
		vpt->getGW()->setColor( TEXT_COLOR, SELAXIS_COLOR );
		vpt->getGW()->setColor( LINE_COLOR, SELAXIS_COLOR );
	} else if (!frozen) {
		vpt->getGW()->setColor( TEXT_COLOR, AXIS_COLOR );
		vpt->getGW()->setColor( LINE_COLOR, AXIS_COLOR );
	}
	float corners = .9f * length;
	Text( vpt, MaxSDK::GetResourceStringAsMSTR(IDS_EAST_MARK).data(), Point3(corners,0.0f,0.0f) ); 
	Text( vpt, MaxSDK::GetResourceStringAsMSTR(IDS_WEST_MARK).data(), Point3(-corners,0.0f,0.0f) ); 
	Text( vpt, MaxSDK::GetResourceStringAsMSTR(IDS_NORTH_MARK).data(), Point3(0.0f,corners,0.0f) ); 
	Text( vpt, MaxSDK::GetResourceStringAsMSTR(IDS_SOUTH_MARK).data(), Point3(0.0f,-corners,0.0f) ); 

	DrawAStar( vpt, length);   

	// vpt->getGW()->setRndLimits( limits );
}

Box3 GetAxisBox(ViewExp *vpt, const Matrix3 &tm,float length,int resetTM)
{
	Matrix3 tmn = tm;
	Box3 box;
	float zoom = 1.0f;

	// Get width of viewport in world units:  --DS
	zoom = vpt->GetScreenScaleFactor(tmn.GetTrans())*ZFACT;
	if (zoom < MIN_ZOOM) zoom = 1.0f;
	// tmn.Scale(Point3(zoom,zoom,zoom));
	length *= zoom;
	if(resetTM)
		tmn.IdentityMatrix();

	box += Point3(0.0f,0.0f,0.0f) * tmn;
	box += Point3(length,0.0f,0.0f) * tmn;
	box += Point3(0.0f,length,0.0f) * tmn;
	box += Point3(0.0f,0.0f,length) * tmn; 
	box += Point3(-length,0.0f,0.0f) * tmn;
	box += Point3(0.0f,-length,0.0f) * tmn;
	box += Point3(0.0f,0.0f,-length) * tmn;
	box.EnlargeBy(10.0f/zoom);
	return box;
}


inline void EnlargeRectIPoint3( RECT *rect, IPoint3& pt )
{
	if ( pt.x < rect->left )   rect->left   = pt.x;
	if ( pt.x > rect->right )  rect->right  = pt.x;
	if ( pt.y < rect->top )    rect->top    = pt.y;
	if ( pt.y > rect->bottom ) rect->bottom = pt.y;
}

// This is a guess - need to find real w/h.
#define FONT_HEIGHT  11
#define FONT_WIDTH  9   
