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
FILE: helpers.h

DESCRIPTION: Helper object header file

CREATED BY: Tom Hudson

HISTORY: Created 31 January 1995

*>	Copyright (c) 1995, All Rights Reserved.
**********************************************************************/
#pragma once

#include <object.h>
#include "CompassRoseObjCreateCallBack.h"

#define COMPASS_CLASS_ID Class_ID(0x69011e82, 0x5622b0d)
#define AXIS_LENGTH 10.0f //used to be 25.0 before creation proc changed
#define RU_UPDATE WM_USER + 666

class CompassRoseObject : public HelperObject {
public:			
	// Class vars
	static HWND hParams;
	static IObjParam *iObjParams;
	static int dlgShowAxis;
	static float dlgAxisLength;

	// Snap suspension flag (TRUE during creation only)
	BOOL suspendSnap;

	// Params
	BOOL showAxis;
	float axisLength;
	int extDispFlags;

	CompassRoseObject();
	~CompassRoseObject();

	// From BaseObject
	const TCHAR* GetObjectName();
	int HitTest(TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt);
	void Snap(TimeValue t, INode* inode, SnapInfo *snap, IPoint2 *p, ViewExp *vpt);
	void SetExtendedDisplay(int flags);
	int Display(TimeValue t, INode* inode, ViewExp *vpt, int flags);
	CreateMouseCallBack* GetCreateMouseCallBack();
	void BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev);
	void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);

	// From Object
	ObjectState Eval(TimeValue time);
	void InitNodeName(TSTR& s);
	ObjectHandle ApplyTransform(Matrix3& matrix) {return this;}
	Interval ObjectValidity(TimeValue t) {return FOREVER;}
	int CanConvertToType(Class_ID obtype) {return FALSE;}
	Object* ConvertToType(TimeValue t, Class_ID obtype) {assert(0);return NULL;}
	void GetWorldBoundBox(TimeValue t, INode *mat, ViewExp *vpt, Box3& box );
	void GetLocalBoundBox(TimeValue t, INode *mat, ViewExp *vpt, Box3& box );
	int DoOwnSelectHilite()	{ return 1; }
	BOOL HasViewDependentBoundingBox() { return TRUE; }

	// Animatable methods
	void DeleteThis() { delete this; }
	Class_ID ClassID() { return COMPASS_CLASS_ID; }  
	void GetClassName(TSTR& s);
	int IsKeyable(){ return 0;}

	//  inherited virtual methods for Reference-management
	RefResult NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message );
	RefTargetHandle Clone(RemapDir& remap);

	IOResult Load(ILoad *iload);
	IOResult Save(ISave *isave);

private:
	static CompassRoseObjCreateCallBack sCompassRoseCreateCB;
	static MSTR sObjectName;

};


