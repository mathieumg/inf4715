#include "EggShape.h"

extern ClassDesc2* GetEggShapeDesc();
extern EggShapeCreateCallBack* GetEggShapeCreateCallBack();
extern ParamBlockDesc2* GetParamBlockDesc(int param_block_desc_index);


/************************************************************************************/
// EggShape Class
/************************************************************************************/
IObjParam *EggShape::ip = NULL;

EggShape::EggShape() : SimpleSpline()
{
	myNumRefs = egg_shape_num_param_blocks;
	myNumParentRefs = SimpleSpline::NumRefs();
	myParmBlock = NULL;
	myWidthParamLock = false;
	myLengthParamLock = false;

	// Build the interpolations parameter block in SimpleSpline
	ReadyInterpParameterBlock();

	// The parent class have reserve some spots for their references, the ipblock for the interpolation, and the pblock for user parameters.
	// The parents access their reference by position, so we want to make sure that
	// our reference starts after the parents' references.
	GetParamBlockDesc(egg_shape_params)->ref_no = myNumParentRefs + egg_shape_params;
	GetEggShapeDesc()->MakeAutoParamBlocks(this);
	DbgAssert(myParmBlock);
}

EggShape::~EggShape()
{
	DeleteAllRefsFromMe();
	myParmBlock = NULL;
	UnReadyInterpParameterBlock();
}

void EggShape::BeginEditParams(IObjParam *ip,ULONG flags,Animatable *prev)
{
	this->ip = ip;

	SimpleSpline::BeginEditParams(ip,flags,prev);
	GetEggShapeDesc()->BeginEditParams(ip, this, flags, prev);

	GetParamBlockDesc(egg_shape_keyboard)->SetUserDlgProc(new EggShapeTypeInParmDialog(this));
	GetParamBlockDesc(egg_shape_params)->SetUserDlgProc(new EggShapeParmDialog(this));
}

void EggShape::EndEditParams( IObjParam *ip, ULONG flags,Animatable *next )
{
	//TODO: Save plugin parameter values into class variables, if they are not hosted in ParamBlocks. 
	GetEggShapeDesc()->EndEditParams(ip, this, flags, next);
	SimpleSpline::EndEditParams(ip,flags,next);

	this->ip = NULL;
}

//From BaseObject
CreateMouseCallBack* EggShape::GetCreateMouseCallBack() 
{
	GetEggShapeCreateCallBack()->SetObj(this);
	return GetEggShapeCreateCallBack();
}

void EggShape::InvalidateUI() 
{
	// Hey! Update the param blocks
	myParmBlock->GetDesc()->InvalidateUI();
}

void EggShape::UpdateUI(HWND hWnd)
{
	Interval valid;
	BOOL should_make_donut;
	TimeValue t = ip ? ip->GetTime() : 0;

	myParmBlock->GetValue(egg_shape_make_donut, t, should_make_donut, valid);
	ICustEdit *thickness_edit = GetICustEdit(GetDlgItem(hWnd, IDC_THICK_EDIT));
	ISpinnerControl *thickness_spin = GetISpinner(GetDlgItem(hWnd, IDC_THICK_SPIN));

	if(!thickness_edit || !thickness_spin)
		return;

	if(should_make_donut)
	{
		EnableWindow(GetDlgItem(hWnd,IDC_THICKNESS_TEXT),TRUE);
		thickness_edit->Enable();
		thickness_spin->Enable();
	}
	else
	{
		EnableWindow(GetDlgItem(hWnd,IDC_THICKNESS_TEXT),FALSE);
		thickness_edit->Disable();
		thickness_spin->Disable();
	}

	ReleaseICustEdit(thickness_edit);
	ReleaseISpinner(thickness_spin);
}

RefTargetHandle EggShape::Clone(RemapDir& remap) 
{
	EggShape* newob = new EggShape();	
	//Copying the class data members
	newob->myNumRefs = myNumRefs;
	newob->myNumParentRefs = myNumParentRefs;
	newob->myWidthParamLock = myWidthParamLock;
	newob->myLengthParamLock = myLengthParamLock;

	newob->SimpleSplineClone(this, remap);
	newob->ReplaceReference(myNumParentRefs + egg_shape_params, remap.CloneRef(myParmBlock));
	newob->ivalid.SetEmpty();
	BaseClone(this, newob, remap);
	return(newob);
}

void EggShape::LockParam(int param_index)
{
	switch(param_index)
	{
	case egg_shape_length :
		myLengthParamLock = true;
		break;
	case egg_shape_width :
		myWidthParamLock = true;
		break;
	}
}

void EggShape::UnlockParam(int param_index)
{
	switch(param_index)
	{
	case egg_shape_length :
		myLengthParamLock = false;
		break;
	case egg_shape_width :
		myWidthParamLock = false;
		break;
	}
}

bool EggShape::GetParamIsLocked(int param_index) const
{
	switch(param_index)
	{
	case egg_shape_length :
		return myLengthParamLock;
	case egg_shape_width :
		return myWidthParamLock;
	}

	return false;
}

int	EggShape::NumRefs()
{
	// The number of references that we have is the sum of our own stuffs plus whatever our parent have.
	return myNumRefs + myNumParentRefs;
}

RefTargetHandle EggShape::GetReference(int i)
{
	if(i < myNumParentRefs)
	{
		return SimpleSpline::GetReference(i);
	}
	else if(i == (myNumParentRefs + egg_shape_params))
	{
		return myParmBlock;
	}

	DbgAssert(false && _M("Reference Index Does Not Match to Any Reference"));
	return NULL;
}

void EggShape::SetReference(int i, RefTargetHandle rtarg)
{
	if(i < myNumParentRefs)
	{
		SimpleSpline::SetReference(i, rtarg);
	}
	else if(i == (myNumParentRefs + egg_shape_params))
	{
		myParmBlock = (IParamBlock2*)rtarg;
	}
	else
	{
		DbgAssert(false && _M("Reference Index Does Not Match to Any Reference"));
	}
}

int	EggShape::NumParamBlocks()
{
	return egg_shape_num_param_blocks;
}

IParamBlock2* EggShape::GetParamBlock(int id) 
{
	// We do not need to take the parent param block into account here. The parent param blocks were not included in the NumParamBlocks.
	if(id == egg_shape_params)
		return myParmBlock;

	return NULL;
}

IParamBlock2* EggShape::GetParamBlockByID(BlockID id) 
{
	if(myParmBlock->ID() == id)
		return myParmBlock; 

	return NULL;
}	

RefResult EggShape::NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, PartID& partID,  RefMessage message) 
{
	if(hTarget != myParmBlock)
		return SimpleSpline::NotifyRefChanged(changeInt, hTarget, partID, message);

	switch (message) 
	{
	case REFMSG_CHANGE:
		//invalidate the shape to reset the ivalid
		SimpleSpline::ShapeInvalid();
		ShapeObject::InvalidateGeomCache();

		InvalidateUI();

		break;
	}
	return REF_SUCCEED;
}

int EggShape::NumSubs()
{
	return myNumRefs + myNumParentRefs;
}

Animatable* EggShape::SubAnim(int i)
{
	if(i < myNumParentRefs)
	{
		return SimpleSpline::SubAnim(i);
	}
	else if(i == (myNumParentRefs + egg_shape_params))
	{
		return myParmBlock;
	}

	DbgAssert(false && _M("SubAnim Index Does Not Match"));
	return NULL;
}

TSTR EggShape::SubAnimName(int i)
{
	if(i==(myNumParentRefs + egg_shape_params))
	{
		return TSTR(GetString(IDS_PARAMS));	
	}

	return SimpleSpline::SubAnimName(i);
}

void EggShape::InitNodeName(TSTR& s) 
{ 
	s = GetString(IDS_CLASS_NAME); 
}

Class_ID EggShape::ClassID() 
{ 
	return EggShape_CLASS_ID; 
}

void EggShape::GetClassName(TSTR& s) 
{ 
	s = GetString(IDS_CLASS_NAME); 
}

const TCHAR *EggShape::GetObjectName() 
{ 
	return GetString(IDS_CLASS_NAME); 
}

BOOL EggShape::ValidForDisplay(TimeValue t)
{
	float size1;
	myParmBlock->GetValue(egg_shape_length, t, size1, ivalid);

	return (size1>0.0);
}

void EggShape::BuildShape(TimeValue t, BezierShape& ashape)
{
	ivalid = FOREVER;
	ashape.NewShape();
	
	// setup some parameters before hand
	int steps;
	BOOL optimize,adaptive, should_make_donut;	
	float length, thickness, rotation;

	ipblock->GetValue(IPB_STEPS, t, steps, ivalid);
	ipblock->GetValue(IPB_OPTIMIZE, t, optimize, ivalid);
	ipblock->GetValue(IPB_ADAPTIVE, t, adaptive, ivalid);
	myParmBlock->GetValue(egg_shape_length, t, length, ivalid);
	myParmBlock->GetValue(egg_shape_thickness, t, thickness, ivalid);
	myParmBlock->GetValue(egg_shape_make_donut, t, should_make_donut, ivalid);
	myParmBlock->GetValue(egg_shape_rotation, t, rotation, ivalid);
	ashape.steps = adaptive ? -1 : steps;
	ashape.optimize = optimize;

	if(thickness < EPSILON && fabs(thickness) > length/2)
		thickness = -1* (length/2);

	// actually making our lovely shape
	// The MakeEggShape method takes the radius of the bottom circle of the egg to construct the shape, 
	// so divide the length by 3 to get the radius
	length /= 3;
	MakeEggShape(should_make_donut, ashape, length, thickness, rotation, (length*CENTER_OF_MASS_OFFSET/2));

	ashape.UpdateSels();	// Make sure it readies the selection set info
	ashape.InvalidateGeomCache();
}

void EggShape::MakeEggShape(BOOL should_make_donut, BezierShape& ashape, float size, float size2, float rotation, float offset)
{
	//Calculate the offset position
	float sin_factor = sin(rotation*(-1) * TO_RAD);
	float cos_factor = cos(rotation*(-1) * TO_RAD);
	float off_x = offset * sin_factor;
	float off_y = offset * cos_factor;

	int num_knot_cnt = 0;
	Spline3D *spline = NULL;
	float big_size = size*3;
	float small_size = size/2;
	Point3 small_circle_location((size+small_size)*sin_factor - off_x, (size+small_size)*cos_factor - off_y, 0.0f);
	Point3 big_circle_location(-off_x, -off_y, (float)0.0);

	// Construct the bottom of the egg
	spline = ashape.NewSpline();

	float angle = M_PI;
	float angle_inc = M_PI/2;
	for(int i=0; i<3; i++, angle+=angle_inc) 
	{
		SplineKnot one_knot = MakeAnchor(angle, rotation*TO_RAD, size, CIRCLE_VECTOR_LENGTH_90, big_circle_location);
		spline->AddKnot(one_knot);
		num_knot_cnt++;
	}	

	// Construct the right side of the egg
	angle = 0;
	angle_inc = (EGG_SIDE_ANGLE_SPAN/3)*TO_RAD;
	for(int i=0; i<4; i++, angle+=angle_inc)
	{ 
		// we already have the first knot, just need to update the out vector of this knot.
		if(i==0)
		{
			spline->SetRelOutVec(num_knot_cnt-1, (spline->GetRelOutVec(num_knot_cnt-1))*3*CIRCLE_VECTOR_LENGTH_12/CIRCLE_VECTOR_LENGTH_90);
			continue;
		}

		// other wise we actually make the knot
		Point3 local_offset(-1*(big_size*2/3)*cos_factor - off_x, (big_size*2/3)*sin_factor - off_y, 0.0f);
		SplineKnot one_knot = MakeAnchor(angle, rotation*TO_RAD, big_size, CIRCLE_VECTOR_LENGTH_12, local_offset);
		spline->AddKnot(one_knot);
		num_knot_cnt++;
	}

	// Construct the tip of the egg
	angle = EGG_SIDE_ANGLE_SPAN*TO_RAD;
	angle_inc = (EGG_TIP_ANGLE_SPAN/3)*TO_RAD;
	for(int i=0; i<4; i++, angle+=angle_inc)
	{ 
		if(i==0)
		{
			spline->SetRelOutVec(num_knot_cnt-1, (spline->GetRelOutVec(num_knot_cnt-1))/6*CIRCLE_VECTOR_LENGTH_35/CIRCLE_VECTOR_LENGTH_12);
			continue;
		}

		SplineKnot one_knot = MakeAnchor(angle, rotation*TO_RAD, small_size, CIRCLE_VECTOR_LENGTH_35, small_circle_location);
		spline->AddKnot(one_knot);	
		num_knot_cnt++;
	}

	// Construct the left side of the egg
	angle = (180-EGG_SIDE_ANGLE_SPAN)*TO_RAD;
	angle_inc = (EGG_SIDE_ANGLE_SPAN/3)*TO_RAD;
	for(int i=0; i<4; i++, angle+=angle_inc)
	{ 
		// We have the first one already
		if(i==0)
		{
			spline->SetRelOutVec(num_knot_cnt-1, (spline->GetRelOutVec(num_knot_cnt-1))*6*CIRCLE_VECTOR_LENGTH_12/CIRCLE_VECTOR_LENGTH_35);
			continue;
		}

		// We also have the last one :)
		if(i==3)
		{
			spline->SetRelInVec(0, (spline->GetRelInVec(0))*3*CIRCLE_VECTOR_LENGTH_12/CIRCLE_VECTOR_LENGTH_90);
			continue;
		}

		Point3 local_offset((big_size*2/3)*cos_factor - off_x, -1*(big_size*2/3)*sin_factor - off_y, 0.0f);
		SplineKnot one_knot = MakeAnchor(angle, rotation*TO_RAD, big_size, CIRCLE_VECTOR_LENGTH_12, local_offset);
		spline->AddKnot(one_knot);
		num_knot_cnt++;
	}

	spline->SetClosed();
	spline->ComputeBezPoints();

	if(should_make_donut)
	{
		MakeDonut(ashape, size, size2, rotation, offset);
	}
}

void EggShape::MakeDonut(BezierShape& ashape, float size, float size2, float rotation, float offset)
{	//Calculate the offset position
	float sin_factor = sin(rotation*(-1) * TO_RAD);
	float cos_factor = cos(rotation*(-1) * TO_RAD);
	float off_x = offset * sin_factor;
	float off_y = offset * cos_factor;

	int closing_index = -1;
	int num_knot_cnt = 0;
	Spline3D *wall_spline = NULL;
	float big_size = size*3;
	float small_size = size/2;
	float limit_from_tip = small_size+size2 < 0.0 ? fabs(small_size+size2) : 0.0;
	float limit_from_bottom = size+size2 < 0.0 ? fabs(size+size2) : 0.0;
	float new_limit_from_tip = limit_from_tip;
	bool check_anchor_only = false;

	Point3 small_circle_location((size+small_size)*sin_factor - off_x, (size+small_size)*cos_factor - off_y, 0.0f);
	Point3 big_circle_location(-off_x, -off_y, 0.0f);

	wall_spline = ashape.NewSpline();

	// Construct the bottom of the egg
	float angle = M_PI;
	float angle_inc = M_PI/2;
	for(int i=0; i<3; i++, angle+=angle_inc) 
	{
		SplineKnot one_knot = MakeAnchor(angle, rotation*TO_RAD, size+size2, CIRCLE_VECTOR_LENGTH_90, big_circle_location);

		check_anchor_only = !(size+size2 < 0);
		LimitAnchorLocation(one_knot, big_circle_location, small_circle_location, limit_from_tip, limit_from_bottom, check_anchor_only);

		wall_spline->AddKnot(one_knot);
		num_knot_cnt++;
	}	

	// Construct the right side of the egg
	angle = 0;
	angle_inc = (EGG_SIDE_ANGLE_SPAN/3)*TO_RAD;
	for(int i=0; i<4; i++, angle+=angle_inc)
	{ 
		float divided_by = (size+size2) < 1.0 ? 1.0 : (size+size2);

		// we already have the first knot, just need to update the out vector of this knot.
		if(i==0)
		{
			wall_spline->SetRelOutVec(num_knot_cnt-1, (wall_spline->GetRelOutVec(num_knot_cnt-1))*(CIRCLE_VECTOR_LENGTH_12/CIRCLE_VECTOR_LENGTH_90)*((big_size+size2)/(divided_by)));
			continue;
		}

		// other wise we actually make the knot
		Point3 local_offset(-1*(big_size*2/3)*cos_factor - off_x, (big_size*2/3)*sin_factor - off_y, 0.0f);
		SplineKnot one_knot = MakeAnchor(angle, rotation*TO_RAD, big_size+size2, CIRCLE_VECTOR_LENGTH_12, local_offset);

		float length_to_axis = fabs(2*size/cos(angle));
		check_anchor_only = !(big_size+size2 < length_to_axis);
		LimitAnchorLocation(one_knot, big_circle_location, small_circle_location, limit_from_tip, limit_from_bottom, check_anchor_only);

		wall_spline->AddKnot(one_knot);
		num_knot_cnt++;
	}

	// Construct the tip of the egg
	angle = EGG_SIDE_ANGLE_SPAN*TO_RAD;
	angle_inc = (EGG_TIP_ANGLE_SPAN/3)*TO_RAD;
	for(int i=0; i<4; i++, angle+=angle_inc)
	{ 
		float divided_by = (big_size+size2) < 1.0 ? 1.0 : (big_size+size2);

		if(i==0)
		{
			wall_spline->SetRelOutVec(num_knot_cnt-1, (wall_spline->GetRelOutVec(num_knot_cnt-1))*(CIRCLE_VECTOR_LENGTH_35/CIRCLE_VECTOR_LENGTH_12)*((small_size+size2)/(divided_by)));
			continue;
		}

		SplineKnot one_knot = MakeAnchor(angle, rotation*TO_RAD, small_size+size2, CIRCLE_VECTOR_LENGTH_35, small_circle_location);
		check_anchor_only = !(small_size+size2 < 0);
		LimitAnchorLocation(one_knot, big_circle_location, small_circle_location, limit_from_tip, limit_from_bottom, check_anchor_only);

		wall_spline->AddKnot(one_knot);
		num_knot_cnt++;
	}

	// Construct the left side of the egg
	angle = (180-EGG_SIDE_ANGLE_SPAN)*TO_RAD;
	angle_inc = (EGG_SIDE_ANGLE_SPAN/3)*TO_RAD;
	for(int i=0; i<4; i++, angle+=angle_inc)
	{ 
		float divided_by = (small_size+size2) < 1.0 ? 1.0 : (small_size+size2);

		// We have the first one already
		if(i==0)
		{
			wall_spline->SetRelOutVec(num_knot_cnt-1, (wall_spline->GetRelOutVec(num_knot_cnt-1))*(CIRCLE_VECTOR_LENGTH_12/CIRCLE_VECTOR_LENGTH_35)*((big_size+size2)/(divided_by)));
			continue;
		}

		divided_by = (size+size2) < 1.0 ? 1.0 : (size+size2);
		// We also have the last one :)
		if(i==3)
		{
			wall_spline->SetRelInVec(0, (wall_spline->GetRelInVec(0))*(CIRCLE_VECTOR_LENGTH_12/CIRCLE_VECTOR_LENGTH_90)*((big_size+size2)/(divided_by)));
			continue;
		}

		Point3 local_offset((big_size*2/3)*cos_factor - off_x, -1*(big_size*2/3)*sin_factor - off_y, 0.0f);
		SplineKnot one_knot = MakeAnchor(angle, rotation*TO_RAD, big_size+size2, CIRCLE_VECTOR_LENGTH_12, local_offset);

		float length_to_axis = fabs(2*size/cos(angle));
		check_anchor_only = !(big_size+size2 < length_to_axis);
		LimitAnchorLocation(one_knot, big_circle_location, small_circle_location, limit_from_tip, limit_from_bottom, check_anchor_only);
		if(!check_anchor_only && (i==1 || i==2))
		{
			new_limit_from_tip = (one_knot.Knot()-small_circle_location).Length();
			closing_index = num_knot_cnt;
		}

		wall_spline->AddKnot(one_knot);
		num_knot_cnt++;
	}

	int begining_index = -1;
	if(closing_index == 10)
		begining_index = 3;
	else if(closing_index == 9)
		begining_index = 4;
	// go through the points to update the limit, but we can ignore the bottom semi circle
	for(int i=3; i<wall_spline->KnotCount(); i++)
	{
		
		if(i >= begining_index && i <= closing_index)
		{
			SplineKnot one_knot = wall_spline->GetKnot(i);
			Point3 knot_pos = one_knot.Knot() - big_circle_location;
			Point3 a_vec = small_circle_location - big_circle_location;
			float a_vec_lenght = a_vec.Length();

			knot_pos = (a_vec/a_vec_lenght)*(a_vec_lenght-new_limit_from_tip) + big_circle_location;
			one_knot.SetKnot(knot_pos);
			wall_spline->SetKnot(i, one_knot);
		}
		
	}

	wall_spline->SetClosed();
	wall_spline->ComputeBezPoints();
}

void EggShape::LimitAnchorLocation(SplineKnot& one_knot, const Point3& big_location, const Point3& small_location, float limit_from_tip, float limit_from_bottom, bool check_anchor_only)
{
	Point3 knot_pos = one_knot.Knot();
	Point3 in_vec = one_knot.InVec();
	Point3 out_vec = one_knot.OutVec();
	Point3 vector_a = small_location - big_location;
	Point3 vector_b = knot_pos - big_location;
	float a_length = vector_a.Length();

	if(a_length < EPSILON)
		return;

	if(check_anchor_only)
	{
		bool intersected;
		Point3 new_vec = CheckAnchorCrossOver(knot_pos, in_vec, big_location, small_location, intersected);
		if(intersected)
		{
			one_knot.SetInVec(new_vec);
		}

		new_vec = CheckAnchorCrossOver(knot_pos, out_vec, big_location, small_location, intersected);
		if(intersected)
		{
			one_knot.SetOutVec(new_vec);
		}
	}
	else
	{
		// projection of b onto a
		float comp_a_b = fabs((vector_a%vector_b) / a_length);

		if(comp_a_b < limit_from_bottom)
			comp_a_b = limit_from_bottom;
		else if(comp_a_b + limit_from_tip > a_length)
			comp_a_b = a_length - limit_from_tip;

		knot_pos = comp_a_b * (vector_a/a_length) + big_location;
		one_knot.SetKnot(knot_pos);
		one_knot.SetInVec(knot_pos);
		one_knot.SetOutVec(knot_pos);
	}
}

Point3 EggShape::CheckAnchorCrossOver(const Point3& pos, const Point3& vec, const Point3& line_pt_1, const Point3& line_pt_2, bool& interseted)
{
	float denominator = ((vec.y-pos.y)*(line_pt_2.x-line_pt_1.x) - (vec.x-pos.x)*(line_pt_2.y-line_pt_1.y));

	if(fabs(denominator) < EPSILON)
	{
		interseted = false;
		return Point3(0, 0, 0);
	}

	float t1 = ((vec.x-pos.x)*(line_pt_1.y-pos.y) - (vec.y-pos.y)*(line_pt_1.x-pos.x) ) / denominator; 
	float t2 = ((line_pt_2.x-line_pt_1.x)*(line_pt_1.y-pos.y) - (line_pt_2.y-line_pt_1.y)*(line_pt_1.x-pos.x)) / denominator;

	interseted = (t1 < 1 && t1 > 0) && (t2 < 1 && t2 > 0);
	return Point3(line_pt_1.x + t1*(line_pt_2.x-line_pt_1.x), line_pt_1.y + t1*(line_pt_2.y-line_pt_1.y), 0.0f);
}

void EggShape::MakeCircle(BezierShape& ashape, float radius, const Point3& position)
{
	Spline3D *spline = ashape.NewSpline();

	// A circle has 4 points, each point has 2 anchors
	for(int i=0; i<4; i++) 
	{
		float angle = (2*M_PI*(float)i)/4.f;

		SplineKnot one_knot = MakeAnchor(angle, 0, radius, CIRCLE_VECTOR_LENGTH_90, position);
		spline->AddKnot(one_knot);
	}
	spline->SetClosed();
	spline->ComputeBezPoints();
}

SplineKnot EggShape::MakeAnchor(float angle, float rot, float size, float unit_vector_length, const Point3& off_set)
{
	float sin_fac = (float)sin(angle+rot);
	float cos_fac = (float)cos(angle+rot);

	Point3 p(cos_fac*size+off_set.x, sin_fac*size+off_set.y, 0.0f+off_set.z);
	Point3 rotation(sin_fac*unit_vector_length*size, -cos_fac*unit_vector_length*size, 0.0f);

	return SplineKnot(KTYPE_BEZIER, LTYPE_CURVE, p, p+rotation, p-rotation);
}