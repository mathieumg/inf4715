/**********************************************************************
 *<
	FILE: le_tclasses.cpp

	DESCRIPTION: Contains the Implementation of the various types and the root class

	CREATED BY: Luis Estrada

	HISTORY:

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

//#include "pch.h"
#include <maxscript/maxscript.h>
#include <maxscript/foundation/numbers.h>
#include <maxscript/foundation/3dmath.h>
#include <maxscript/maxwrapper/mxsobjects.h>

#include "meshadj.h"
#include <patchobj.h>
#include <euler.h>
#include <trig.h>
#include "resource.h"
#include "agnidefs.h"

#include "MXSAgni.h"
#include "ExtClass.h"

#ifdef ScripterExport
	#undef ScripterExport
#endif
#define ScripterExport __declspec( dllexport )

#include <maxscript\macros\define_external_functions.h>
#	include "namedefs.h"

#include <maxscript\macros\define_instantiation_functions.h>
#	include	"le_wraps.h"

/*--------------------------------- TestInterface-----------------------------------*/

/*--------------------------------- Interface Class ------------------------------*/

Value*
ConfigureBitmapPaths_cf(Value** arg_list, int count)
{
	
	check_arg_count (ConfigureBitmapPaths,0,count);

	return MAXScript_interface->ConfigureBitmapPaths() ? &true_value : &false_value;
}


Value*
EditAtmosphere_cf(Value** arg_list, int count)
{
	if (count == 0)
		MAXScript_interface->EditAtmosphere(NULL);
	else
	{
		check_arg_count (EditAtmosphere,2,count);
		MAXScript_interface->EditAtmosphere(arg_list[0]->to_atmospheric(), arg_list[1]->to_node());
	}

	return &ok;
}


Value*
CheckForSave_cf(Value** arg_list, int count)
{
	//bool checkforsave
	check_arg_count (CheckForSave, 0, count);

	return MAXScript_interface->CheckForSave () ? &true_value : &false_value;

}

Value*
MatrixFromNormal_cf(Value** arg_list, int count)
{
	Matrix3 result_matrix;
	check_arg_count (MatrixFromNormal, 1, count);

	Point3 the_normal= arg_list[0]->to_point3();
	
	MatrixFromNormal (the_normal, result_matrix);

	return new Matrix3Value (result_matrix);
	
}



/*-------------------------------------------------- Object Class ---------------*/

/* Parameter needed are the time and the object) */
Value*
GetPolygonCount_cf(Value** arg_list, int count)
{
	int faces=0,vertnum=0;
	one_typed_value_local (Array* total) 

	check_arg_count (GetPolygonCount, 1, count);
	INode* node = arg_list[0]->to_node();

	ObjectState os = node->EvalWorldState(MAXScript_time());
	Object* refobj = os.obj;

	GetPolygonCount (MAXScript_time(), refobj, faces, vertnum);
	
	vl.total = new Array (2);
	vl.total->append (Integer::intern(faces));
	vl.total->append (Integer::intern(vertnum));

	return_value (vl.total);	
}



Value*
GetTriMeshFaceCount_cf(Value** arg_list, int count)
{
	int faces=0,vertnum=0;
	one_typed_value_local (Array* total) 

	check_arg_count (GetTriMeshFaceCount, 1, count);
	INode* node = arg_list[0]->to_node();

	ObjectState os = node->EvalWorldState(MAXScript_time());
	Object* refobj = os.obj;

	GetTriMeshFaceCount (MAXScript_time(), refobj, faces, vertnum);
	
	vl.total = new Array (2);
	vl.total->append (Integer::intern(faces));
	vl.total->append (Integer::intern(vertnum));

	return_value (vl.total);	
}


Value*
NumMapsUsed_cf(Value** arg_list, int count)
{
	check_arg_count (NumMapsUsed, 1, count);
	INode* node = arg_list[0]->to_node();
	ObjectState os = node->EvalWorldState(MAXScript_time());
	Object* refobj = os.obj;

	return Integer::intern (refobj->NumMapsUsed());
}


Value*
IsPointSelected_cf(Value** arg_list, int count)
{
	int pointindex;
	check_arg_count (IsPointSelected, 2, count);

	INode* node = arg_list[0]->to_node();
	ObjectState os = node->EvalWorldState(MAXScript_time());
	Object* refobj = os.obj;

	pointindex = arg_list[1]->to_int()-1;
	if (pointindex < 0)
		throw RuntimeError(MaxSDK::GetResourceStringAsMSTR(IDS_RK_ISPOINTSELECTED_INDEX_OUT_OF_RANGE));

	return refobj->IsPointSelected(pointindex) ? &true_value : &false_value;
}


Value*
PointSelection_cf(Value** arg_list, int count)
{
	int pointindex;
	check_arg_count (PointSelection, 2, count);

	INode* node = arg_list[0]->to_node();
	ObjectState os = node->EvalWorldState(MAXScript_time());
	Object* refobj = os.obj;

	pointindex = arg_list[1]->to_int()-1;
	if (pointindex < 0)
		throw RuntimeError(MaxSDK::GetResourceStringAsMSTR(IDS_RK_POINTSELECTION_INDEX_OUT_OF_RANGE));
	return Float::intern(refobj->PointSelection(pointindex));
}


Value*
IsShapeObject_cf(Value** arg_list, int count)
{
	check_arg_count (IsShapeObject, 1, count);
	
	INode* node = arg_list[0]->to_node();
	ObjectState os = node->EvalWorldState(MAXScript_time());
	Object* refobj = os.obj;

	return refobj->IsShapeObject () ? &true_value : &false_value;
}

Value* 
NumSurfaces_cf(Value** arg_list, int count)
{
	check_arg_count (NumSurfaces, 1, count);
	
	INode* node = arg_list[0]->to_node();
	ObjectState os = node->EvalWorldState(MAXScript_time());
	Object* refobj = os.obj;

	return Integer::intern(refobj->NumSurfaces (MAXScript_time()));

}

Value*
IsSurfaceUVClosed_cf (Value** arg_list, int count)
{
	check_arg_count (IsSurfaceUVClosed, 2, count);
	one_typed_value_local (Array* total);

	INode* node = arg_list[0]->to_node();
	ObjectState os = node->EvalWorldState(MAXScript_time());
	Object* refobj = os.obj;
	int inu;
	int inv;

	int surfnum = arg_list[1]->to_int();
	if (surfnum < 1 || surfnum > refobj->NumSurfaces(MAXScript_time())) 
		throw RuntimeError(MaxSDK::GetResourceStringAsMSTR(IDS_RK_ISSURFACEUVCLOSED_INDEX_OUT_OF_RANGE));

	refobj->SurfaceClosed (MAXScript_time(), surfnum-1, inu, inv);
	
	vl.total = new Array (2);
	vl.total->append (inu ? &true_value : &false_value);
	vl.total->append (inv ? &true_value : &false_value);

	return_value (vl.total);	
}

/*------------------------------ Miscellaneous Global Functions -------------*/

Value*
DeselectHiddenEdges_cf(Value **arg_list, int count)
{
	Object *obj;

	check_arg_count (DeselectHiddenEdges, 1, count);

	INode* node = arg_list[0]->to_node();
	
	// Get the object from the node
	ObjectState os = node->EvalWorldState(MAXScript_time());
	if (os.obj->SuperClassID()==GEOMOBJECT_CLASS_ID) {
		obj = (GeomObject*)os.obj;
		if (obj->ClassID() != GetEditTriObjDesc()->ClassID() &&
			obj->ClassID() != Class_ID(TRIOBJ_CLASS_ID, 0))		
			return &undefined;
	}
	else
		return &undefined;
	
	
	/* get the associated mesh and deselect hidden edges */
	Mesh &themesh = ((TriObject *) obj)->GetMesh();
	DeselectHiddenEdges ( themesh );

	return &ok;
}
	

Value*
DeselectHiddenFaces_cf(Value **arg_list, int count)
{
	Object *obj;
	check_arg_count (DeselectHiddenFaces, 1, count);

	INode* node = arg_list[0]->to_node();
	
	// Get the object from the node
	ObjectState os = node->EvalWorldState(MAXScript_time());
	if (os.obj->SuperClassID()==GEOMOBJECT_CLASS_ID) {
		obj = (GeomObject*)os.obj;
		if (obj->ClassID() != GetEditTriObjDesc()->ClassID() &&
			obj->ClassID() != Class_ID(TRIOBJ_CLASS_ID, 0))		
			return &undefined;
	}
	else
		return &undefined;
	
	
	/* get the associated mesh and deselect hidden edges */
	Mesh &themesh = ((TriObject*) obj)->GetMesh();
	DeselectHiddenFaces ( themesh );

	return &ok;
}


Value*
AverageSelVertCenter_cf(Value** arg_list, int count)
{
	Object *obj;

	check_arg_count (AverageSelVertCenter, 1, count);

	INode* node = arg_list[0]->to_node();
	
	// Get the object from the node
	ObjectState os = node->EvalWorldState(MAXScript_time());
	if (os.obj->SuperClassID()==GEOMOBJECT_CLASS_ID) {
		obj = (GeomObject*)os.obj;
		if (obj->ClassID() != GetEditTriObjDesc()->ClassID() &&
			obj->ClassID() != Class_ID(TRIOBJ_CLASS_ID, 0))		
			return &undefined;
	}
	else
		return &undefined;
	
	
	/* get the associated mesh and return the average center of the selected vertices */
	Mesh &themesh = ((TriObject *) obj)->GetMesh();
	return new Point3Value(AverageSelVertCenter ( themesh ) );

}



Value*
AverageSelVertNormal_cf(Value** arg_list, int count)
{
	Object *obj;

	check_arg_count (AverageSelVertNormal, 1, count);

	INode* node = arg_list[0]->to_node();
	
	// Get the object from the node
	ObjectState os = node->EvalWorldState(MAXScript_time());
	if (os.obj->SuperClassID()==GEOMOBJECT_CLASS_ID) {
		obj = (GeomObject*)os.obj;
		if (obj->ClassID() != GetEditTriObjDesc()->ClassID() &&
			obj->ClassID() != Class_ID(TRIOBJ_CLASS_ID, 0))		
			return &undefined;
	}
	else
		return &undefined;
	
	
	/* get the associated mesh and return the average center of the selected vertices */
	Mesh &themesh = ((TriObject *) obj)->GetMesh();
	return new Point3Value(AverageSelVertNormal ( themesh ) );

}


/* -----------------------------------   Patch Properties ------------------------------- */

#ifndef NO_PATCHES
Value* GetPatchSteps_cf(Value** arg_list, int count)
{
	check_arg_count (GetPatchSteps, 1, count);

	// Get the object from the node
	INode* node = arg_list[0]->to_node();
	Object* obj = Get_Object_Or_XRef_BaseObject(node->GetObjectRef());

	if (obj->ClassID() != Class_ID(PATCHOBJ_CLASS_ID,0)) 
		throw RuntimeError (MaxSDK::GetResourceStringAsMSTR(IDS_RK_PATCH_OPERATION_ON_NONMESH), obj->GetObjectName());

		//throw RuntimeError (_T("Patch operation on non-Patch: "), obj->GetObjectName());

	return Integer::intern( ((PatchObject*)obj)->GetMeshSteps());
}


Value* SetPatchSteps_cf (Value** arg_list, int count)
{
	check_arg_count (SetPatchSteps, 2, count);

	// Get the object from the node
	INode* node = arg_list[0]->to_node();
	Object* obj = Get_Object_Or_XRef_BaseObject(node->GetObjectRef());

	if (obj->ClassID() != Class_ID(PATCHOBJ_CLASS_ID,0))
		throw RuntimeError (MaxSDK::GetResourceStringAsMSTR(IDS_RK_PATCH_OPERATION_ON_NONMESH), obj->GetObjectName());
	if (arg_list[1]->to_int() < 0 ) 
		throw RuntimeError (MaxSDK::GetResourceStringAsMSTR(IDS_RK_SETPATCHSTEPS_INDEX_OUT_OF_RANGE));

	((PatchObject*)obj)->SetMeshSteps (arg_list[1]->to_int());
	needs_complete_redraw_set();

	if (MAXScript_interface->GetCommandPanelTaskMode() == TASK_MODE_MODIFY)
		if ((BaseObject*)obj == MAXScript_interface->GetCurEditObject())
			obj->NotifyDependents (FOREVER, PART_ALL, REFMSG_CHANGE);
			//MAXScript_interface->SetCommandPanelTaskMode (TASK_MODE_MODIFY);

	return &ok;
}
#endif // NO_PATCHES

/* --------------------------- Euler Angles -------------------------------- */

Value* GetEulerQuatAngleRatio_cf (Value** arg_list, int count)
{
	// first 2 args are quats (input), second 2 args are euler angles (output)
	// <float> GetEulerQuatAngleRatio <quat1> <quat2> <array1> <array2> -- euler angle components stored in array elements 1 to 3
	// <float> GetEulerQuatAngleRatio <quat1> <quat2> <eulerAngle1> <eulerAngle2> -- euler angles stored in eulerAngles (modifying eulerAngles passed)
	// <float> GetEulerQuatAngleRatio <quat1> <quat2> &<var1> &<var2> -- eulerAngles written back to by-reference args

	check_arg_count_with_keys(GetEulerQuatAngleRatio, 4, count);
	int angle;

	def_euler_angles();
	Value* theangle = key_arg(angle)->eval();
	if (theangle == &unsupplied)
		angle = EULERTYPE_XYZ;
	else
		angle = GetID(eulerAngles, elements(eulerAngles), theangle); 

	Quat quat1 = arg_list[0]->to_quat();
	Quat quat2 = arg_list[1]->to_quat();

	float eulerarray1[3];
	float eulerarray2[3];
	float res = GetEulerQuatAngleRatio (quat1, quat2, eulerarray1, eulerarray2, angle);

	// fun and games here because we don't want to break existing scripts. 
	// If arg 2 and 3 are arrays, store euler angle component values in them.
	// If arg 2 and 3 are EulerAngle values, store euler angle component values in them.
	// If arg 2 and 3 are Thunks, store EulerAngle values to them.
	Value* arg2 = arg_list[2];
	Value* arg3 = arg_list[3];
	if (is_array(arg2))
	{
		type_check(arg3, Array, _T("GetEulerQuatAngleRatio"));
		Array *euler1 = (Array*)arg2;
		Array *euler2 = (Array*)arg3;
		for (int i = 0; i < 3; i++)
		{
			float v = RadToDeg(eulerarray1[i]);
			if (euler1->size < i+1)
				euler1->append(Float::intern(v));
			else
				euler1->data[i] = heap_ptr(Float::intern(v));
			v = RadToDeg(eulerarray2[i]);
			if (euler2->size < i+1)
				euler2->append(Float::intern(v));
			else
				euler2->data[i] = heap_ptr(Float::intern(v));
		}
	}
	else if (is_eulerangles(arg2))
	{
		type_check(arg3, EulerAnglesValue, _T("GetEulerQuatAngleRatio"));
		EulerAnglesValue *euler1 = (EulerAnglesValue*)arg2;
		EulerAnglesValue *euler2 = (EulerAnglesValue*)arg3;
		for (int i = 0; i < 3; i++)
		{
			euler1->angles[i] = eulerarray1[i];
			euler2->angles[i] = eulerarray2[i];
		}
	}
	else if (is_thunk(arg2))
	{
		if (!is_thunk(arg2))
			type_check(arg3, Thunk, _T("GetEulerQuatAngleRatio"));
		Thunk *euler1 = arg2->to_thunk();
		Thunk *euler2 = arg3->to_thunk();
		one_typed_value_local(EulerAnglesValue* eulerValue);
		vl.eulerValue = new EulerAnglesValue(eulerarray1[0], eulerarray1[1], eulerarray1[2]);
		euler1->assign(vl.eulerValue);
		vl.eulerValue = new EulerAnglesValue(eulerarray2[0], eulerarray2[1], eulerarray2[2]);
		euler2->assign(vl.eulerValue);
		pop_value_locals();
	}
	else
	{
		type_check(arg2, EulerAnglesValue, _T("GetEulerQuatAngleRatio"));
	}

	return Float::intern (res);
}

Value* GetEulerMatAngleRatio_cf (Value** arg_list, int count)
{
	// first 2 args are matrix3 (input), second 2 args are euler angles (output)
	// <float> GetEulerMatAngleRatio <mat1> <mat2> <array1> <array2> -- euler angle components stored in array elements 1 to 3
	// <float> GetEulerMatAngleRatio <mat1> <mat1> <eulerAngle1> <eulerAngle2> -- euler angles stored in eulerAngles (modifying eulerAngles passed)
	// <float> GetEulerMatAngleRatio <mat1> <mat1> &<var1> &<var2> -- eulerAngles written back to by-reference args

	check_arg_count_with_keys(GetEulerMatAngleRatio, 4, count);
	int angle;

	def_euler_angles();
	Value* theangle = key_arg(angle)->eval(); 
	if (theangle == &unsupplied)
		angle = EULERTYPE_XYZ;
	else
		angle = GetID(eulerAngles, elements(eulerAngles), theangle); 

	Matrix3 mat1 = arg_list[0]->to_matrix3();
	Matrix3 mat2 = arg_list[1]->to_matrix3();

	float eulerarray1[3];
	float eulerarray2[3];
	float res = GetEulerMatAngleRatio (mat1, mat2, eulerarray1, eulerarray2, angle);

	// fun and games here because we don't want to break existing scripts. 
	// If arg 2 and 3 are arrays, store euler angle component values in them.
	// If arg 2 and 3 are EulerAngle values, store euler angle component values in them.
	// If arg 2 and 3 are Thunks, store EulerAngle values to them.
	Value* arg2 = arg_list[2];
	Value* arg3 = arg_list[3];
	if (is_array(arg2))
	{
		type_check(arg3, Array, _T("GetEulerMatAngleRatio"));
		Array *euler1 = (Array*)arg2;
		Array *euler2 = (Array*)arg3;
		for (int i = 0; i < 3; i++)
		{
			float v = RadToDeg(eulerarray1[i]);
			if (euler1->size < i+1)
				euler1->append(Float::intern(v));
			else
				euler1->data[i] = heap_ptr(Float::intern(v));
			v = RadToDeg(eulerarray2[i]);
			if (euler2->size < i+1)
				euler2->append(Float::intern(v));
			else
				euler2->data[i] = heap_ptr(Float::intern(v));
		}
	}
	else if (is_eulerangles(arg2))
	{
		type_check(arg3, EulerAnglesValue, _T("GetEulerMatAngleRatio"));
		EulerAnglesValue *euler1 = (EulerAnglesValue*)arg2;
		EulerAnglesValue *euler2 = (EulerAnglesValue*)arg3;
		for (int i = 0; i < 3; i++)
		{
			euler1->angles[i] = eulerarray1[i];
			euler2->angles[i] = eulerarray2[i];
		}
	}
	else if (is_thunk(arg2))
	{
		if (!is_thunk(arg2))
			type_check(arg3, Thunk, _T("GetEulerMatAngleRatio"));
		Thunk *euler1 = arg2->to_thunk();
		Thunk *euler2 = arg3->to_thunk();
		one_typed_value_local(EulerAnglesValue* eulerValue);
		vl.eulerValue = new EulerAnglesValue(eulerarray1[0], eulerarray1[1], eulerarray1[2]);
		euler1->assign(vl.eulerValue);
		vl.eulerValue = new EulerAnglesValue(eulerarray2[0], eulerarray2[1], eulerarray2[2]);
		euler2->assign(vl.eulerValue);
		pop_value_locals();
	}
	else
	{
		type_check(arg2, EulerAnglesValue, _T("GetEulerMatAngleRatio"))
	}

	return Float::intern (res);
}

/* --------------------- plug-in init --------------------------------- */
// this is called by the dlx initializer, register the global vars here
void le_init()
{
//	#include "le_glbls.h"
}

