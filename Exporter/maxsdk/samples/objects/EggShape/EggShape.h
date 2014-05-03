#pragma once

//**************************************************************************/
// Copyright (c) 2011 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// DESCRIPTION: Includes for Plugins
// AUTHOR: 
//***************************************************************************/

#define _USE_MATH_DEFINES

//#include "3dsmaxsdk_preinclude.h"
#include <Max.h>
#include <istdplug.h>
#include <simpspl.h>
#include <splshape.h>
#include <iparamb2.h>
#include <iparamm2.h>
#include <assert1.h>
#include "resource.h"
#include "EggShapeParam.h"
//SIMPLE TYPE

extern TCHAR *GetString(int id);
extern HINSTANCE hInstance;

// Constant value relating to the bezier circle
#define EGG_SIDE_ANGLE_SPAN 36.87
#define EGG_TIP_ANGLE_SPAN 106.26
// These angle vector lenght are constant as the angle itself is constant, there is no need to perform the expensive calculation all the time
#define CIRCLE_VECTOR_LENGTH_12 0.071571350f	// vector length for unit circle for an angle of 12 degree
#define CIRCLE_VECTOR_LENGTH_35 0.20771742f		// vector length for unit circle for an angle of 35 degree
#define CIRCLE_VECTOR_LENGTH_90 0.5517861843f	// vector length for unit circle for an angle of 90 degree
#define CENTER_OF_MASS_OFFSET 0.6				// center of mass of the egg shape from the center of the bigger circle as a factor of radius of the small circle

#define EggShape_CLASS_ID	Class_ID(EGG_CLASS_ID, 0)

#define TO_WIDTH (2.f/3.f)
#define TO_LENGTH (3.f/2.f)
#define TO_RAD (M_PI/180.0)
#define TO_DEG (180.0/M_PI)
#define EPSILON 0.00001

#define MIN_LENGTH		float(0)
#define MAX_LENGTH		float( 1.0E30)
#define MAX_NEG_LENGTH  float(-1.0E30)
#define EGG_LENGTH_DEFAULT	60.f
#define EGG_WIDTH_DEFAULT	40.f
#define EGG_THICKNESS_DEFAULT	5.f
#define EGG_ANGLE_DEFAULT 0.f

/********************************************* 
// Class Definitions
**********************************************/
class EggShape : public SimpleSpline 
{
public:
	EggShape();
	~EggShape();
	void DeleteThis() { delete this; }

	// Class variable
	static IObjParam* ip;

	//Event Callback method
	CreateMouseCallBack* GetCreateMouseCallBack();

	// Parameter Accessing
	void BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev);
	void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);

	int	NumParamBlocks();
	IParamBlock2* GetParamBlock(int id);
	IParamBlock2* GetParamBlockByID(BlockID id);

	RefTargetHandle GetReference(int i);
	int NumRefs();
	RefResult NotifyRefChanged( Interval changeInt,RefTargetHandle hTarget, PartID& partID, RefMessage message);

	int NumSubs();  
	Animatable* SubAnim(int i);
	TSTR SubAnimName(int i);

	// Object Related
	RefTargetHandle Clone(RemapDir& remap);

	// Display Related
	BOOL ValidForDisplay(TimeValue t);
	void BuildShape(TimeValue t,BezierShape& ashape);
	void InvalidateUI();
	void UpdateUI(HWND hWnd);

	// Class Info Accessing
	void InitNodeName(TSTR& s);	
	Class_ID ClassID();
	void GetClassName(TSTR& s);
	const TCHAR *GetObjectName();

	// Some ugly hack
	void LockParam(int param_index);
	void UnlockParam(int param_index);
	bool GetParamIsLocked(int param_index) const;

protected:
	virtual void SetReference(int i, RefTargetHandle rtarg);

private:
	// The number of references that we have.
	int myNumRefs;
	int myNumParentRefs;

	// This hack is used to prevent cycling when setting width and length param
	bool myWidthParamLock;
	bool myLengthParamLock;

	IParamBlock2* myParmBlock;

	// Shape Helper Functions
	static void MakeCircle(BezierShape& ashape, float radius, const Point3& position);
	static void MakeEggShape(BOOL make_wall, BezierShape& ashape, float size, float size2, float rotation, float offset=0.0);
	static void MakeDonut(BezierShape& ashape, float size, float size2, float rotation, float offset);
	static void LimitAnchorLocation(SplineKnot& one_knot, const Point3& big_location, const Point3& small_location, float limit_from_tip, float limit_from_bottom, bool check_anchor_only);
	static Point3 CheckAnchorCrossOver(const Point3& pos, const Point3& vec, const Point3& line_pt_1, const Point3& line_pt_2, bool& interseted);
	static SplineKnot MakeAnchor(float angle, float rot, float size, float vector_length, const Point3& off_set);
};

class EggShapeClassDesc : public ClassDesc2 
{
public:
	virtual void*			Create(BOOL /*loading = FALSE*/)	{ return new EggShape(); }
	virtual int				IsPublic() 							{ return TRUE; }
	virtual const TCHAR *	ClassName() 						{ return GetString(IDS_CLASS_NAME); }
	virtual SClass_ID		SuperClassID() 						{ return SHAPE_CLASS_ID; }
	virtual Class_ID		ClassID() 							{ return EggShape_CLASS_ID; }
	virtual const TCHAR*	Category() 							{ return GetString(IDS_CATEGORY); }
	const TCHAR*			InternalName()						{ return _T("Egg"); }
	HINSTANCE				HInstance()							{ return hInstance; }
};

//Class for interactive creation of the object using the mouse
class EggShapeCreateCallBack : public CreateMouseCallBack {
	IPoint2 sp0;		//First point in screen coordinates
	EggShape *ob;		//Pointer to the object 
	Point3 p0;			//First point in world coordinates
	Point3 p1;			//Second point in the world coordinates
	Point3 p2;			//Third point for DONUT! option

	float size1;
	float size2;
public:	
	EggShapeCreateCallBack();
	int proc( ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat);
	void SetObj(EggShape *obj) {ob = obj;}
};


// User Dialog Class; dialog associated with typein parameter block
class EggShapeTypeInParmDialog : public ParamMap2UserDlgProc
{
public:
	EggShape *myObject;

	EggShapeTypeInParmDialog(EggShape *obj) { myObject = obj;}
	INT_PTR DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void  	DeleteThis() { delete this; }
};

// User Dialog Class; Dialog associated with parameters block
class EggShapeParmDialog : public ParamMap2UserDlgProc
{
public:
	EggShape *myObject;

	EggShapeParmDialog(EggShape *obj) { myObject = obj;}
	INT_PTR DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void  	DeleteThis() { delete this; }
};

// Accessor class for parameter
class EggAccessor : public PBAccessor
{
	//void Get(PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t, Interval &valid);
	void Set(PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t);
	void DeleteThis(){ delete this;}
};