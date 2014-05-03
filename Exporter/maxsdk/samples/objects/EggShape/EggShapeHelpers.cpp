#include "EggShape.h"

/****************************************************
// Global Static Variables
****************************************************/
static EggShapeClassDesc theEggShapeDesc;
static EggShapeCreateCallBack theEggShapeCreateCB;
static EggAccessor theEggAccessor;

static ParamBlockDesc2 theEggShapeKeyboardBlock 
( 
	egg_shape_keyboard,				// block id
	_T("eggKeyboard"),					// internal name string
	0,								// local name string 
	&theEggShapeDesc,				// class description
	P_CLASS_PARAMS + P_AUTO_UI,		// flags					
	IDD_KEY_PANEL, IDS_KEYPARM, BEGIN_EDIT_CREATE, APPENDROLL_CLOSED, NULL,

	egg_shape_keyboard_length, 	_T(""), 	TYPE_FLOAT, 0, 	IDS_SPIN, 
		p_default, 			EGG_LENGTH_DEFAULT, 
		p_range, 			MIN_LENGTH,	MAX_LENGTH, 
		p_ui, 				TYPE_SPINNER,	EDITTYPE_FLOAT,	IDC_LENGTH_EDIT,	IDC_LENGTH_SPIN,	0.1f, 
		p_tooltip,			IDS_PARAM_LENGTH_TOOLTIP,
		p_end,
	egg_shape_keyboard_width, 	_T(""), 	TYPE_FLOAT, 0,	IDS_SPIN, 
		p_default, 			EGG_WIDTH_DEFAULT, 
		p_range, 			MIN_LENGTH,	MAX_LENGTH, 
		p_ui, 				TYPE_SPINNER,	EDITTYPE_FLOAT,	IDC_WIDTH_EDIT,	IDC_WIDTH_SPIN,	0.1f, 
		p_tooltip,			IDS_PARAM_WIDTH_TOOLTIP,
		p_end,
	egg_shape_keyboard_thickness, 	_T(""), 	TYPE_FLOAT, 0, 	IDS_SPIN, 
		p_default, 			EGG_THICKNESS_DEFAULT, 
		p_range, 			MAX_NEG_LENGTH,	MAX_LENGTH, 
		p_ui, 				TYPE_SPINNER,	EDITTYPE_FLOAT,	IDC_THICK_EDIT,	IDC_THICK_SPIN,	0.1f, 
		p_tooltip,			IDS_PARAM_THICK_TOOLTIP,
		p_end,
	egg_shape_keyboard_rotation,	_T(""), 	TYPE_FLOAT, 0, 	IDS_SPIN, 
		p_default, 			EGG_ANGLE_DEFAULT, 
		p_range, 			-180.0f,	180.0f, 
		p_ui, 				TYPE_SPINNER,	EDITTYPE_FLOAT,	IDC_ROT_EDIT,	IDC_ROT_SPIN,	0.1f, 
		p_tooltip,			IDS_PARAM_ANGLE_TOOLTIP,
		p_end,
	egg_shape_keyboard_position, 			_T(""), 		TYPE_POINT3, 		0, 	IDS_RB_POS,
		p_default, 		Point3(0,0,0), 
		p_range, 		-99999999.0, 99999999.0, 
		p_ui, 			TYPE_SPINNER, EDITTYPE_UNIVERSE, IDC_X_EDIT, IDC_X_SPIN, IDC_Y_EDIT, IDC_Y_SPIN, IDC_Z_EDIT, IDC_Z_SPIN, SPIN_AUTOSCALE, 
		p_end, 
	p_end
);

static ParamBlockDesc2 theEggShapeParmBlock 
( 
	egg_shape_params,				// block id
	_T("eggParams"),				// internal name string
	0,								// local name string 
	&theEggShapeDesc,				// class description
	P_AUTO_CONSTRUCT + P_AUTO_UI,	// flags
	egg_shape_params,						
	IDD_PANEL, IDS_PARAMS, 0, 0, NULL,

	//Parameter Specifications ----------------------
	// For each control create a parameter:
	// NOTE: When updating the script visible name, remember to update the tooltips in the string table
	egg_shape_make_donut, 	_T("egg_outline"), 	TYPE_BOOL,	P_ANIMATABLE|P_RESET_DEFAULT, 	IDS_OUTLINE, 
		p_default, 			TRUE, 
		p_ms_default, 		TRUE,
		p_ui, 				TYPE_SINGLECHEKBOX,		IDC_DONUT_CHECK,
		p_tooltip,			IDS_PARAM_OUTLINE_TOOLTIP,
		p_end,
	egg_shape_thickness, 		_T("egg_thickness"), 	TYPE_FLOAT, P_ANIMATABLE|P_RESET_DEFAULT, 	IDS_THICKNESS, 
		p_default, 			0.f, 
		p_ms_default, 		EGG_THICKNESS_DEFAULT,
		p_range, 			MAX_NEG_LENGTH,	MAX_LENGTH, 
		p_ui, 				TYPE_SPINNER,	EDITTYPE_FLOAT,	IDC_THICK_EDIT,	IDC_THICK_SPIN,	0.1f, 
		p_tooltip,			IDS_PARAM_THICK_TOOLTIP,
		p_end,
	egg_shape_rotation,		_T("egg_angle"), TYPE_FLOAT, P_ANIMATABLE|P_RESET_DEFAULT, 	IDS_ANGLE, 
		p_default, 			0.f, 
		p_ms_default, 		EGG_ANGLE_DEFAULT,
		p_range, 			-180.0f,	180.0f, 
		p_ui, 				TYPE_SPINNER,	EDITTYPE_FLOAT,	IDC_ROT_EDIT,	IDC_ROT_SPIN,	0.1f, 
		p_tooltip,			IDS_PARAM_ANGLE_TOOLTIP,
		p_end,
	egg_shape_length, 		_T("egg_length"), 	TYPE_FLOAT, P_ANIMATABLE|P_RESET_DEFAULT, 	IDS_LENGTH, 
		p_default, 			0.f, 
		p_ms_default, 		EGG_LENGTH_DEFAULT,
		p_range, 			MIN_LENGTH,	MAX_LENGTH, 
		p_accessor,			&theEggAccessor,
		p_ui, 				TYPE_SPINNER,	EDITTYPE_FLOAT,	IDC_LENGTH_EDIT,	IDC_LENGTH_SPIN,	0.1f,
		p_tooltip,			IDS_PARAM_LENGTH_TOOLTIP,
		p_end,
	egg_shape_width, 		_T("egg_width"), 	TYPE_FLOAT, P_ANIMATABLE|P_RESET_DEFAULT, 	IDS_WIDTH,  
		p_default, 			0.f, 
		p_ms_default, 		EGG_WIDTH_DEFAULT,
		p_range, 			MIN_LENGTH,	MAX_LENGTH, 
		p_accessor,			&theEggAccessor,
		p_ui, 				TYPE_SPINNER,	EDITTYPE_FLOAT,	IDC_WIDTH_EDIT,	IDC_WIDTH_SPIN,	0.1f,
		p_tooltip,			IDS_PARAM_WIDTH_TOOLTIP,
		p_end,
	p_end
);

/*****************************************************************
// Helper Functions
*****************************************************************/
ClassDesc2* GetEggShapeDesc() 
{ 
	return &theEggShapeDesc;
}

EggShapeCreateCallBack* GetEggShapeCreateCallBack()
{
	return &theEggShapeCreateCB;
}

ParamBlockDesc2* GetParamBlockDesc(int param_block_desc_index)
{
	switch(param_block_desc_index)
	{
		case egg_shape_keyboard	:	return	&theEggShapeKeyboardBlock;
		case egg_shape_params	:	return	&theEggShapeParmBlock;
		default : return NULL;
	}
}
/*****************************************************************
// Mouse Callback Implementations
*****************************************************************/
EggShapeCreateCallBack::EggShapeCreateCallBack() : sp0(0, 0), p0(0.f, 0.f, 0.f), p1(0.f, 0.f, 0.f), p2(0.f, 0.f, 0.f)
{	
	ob = NULL;		//Pointer to the object
	size1 = 0.0;
	size2 = 0.0;
}

int EggShapeCreateCallBack::proc(ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat )
{
	UNUSED_PARAM(flags);

	if (msg==MOUSE_POINT||msg==MOUSE_MOVE) 
	{
		switch(point) 
		{
		case 0:
			{
				ob->suspendSnap = TRUE;
				sp0 = m;
				p0 = vpt->SnapPoint(m,m,NULL,SNAP_IN_PLANE);
				mat.SetTrans(p0);

				ob->GetParamBlock(egg_shape_params)->SetValue(egg_shape_length, ob->ip->GetTime(), 0.0f);
				ob->GetParamBlock(egg_shape_params)->SetValue(egg_shape_width, ob->ip->GetTime(), 0.0f);
				ob->GetParamBlock(egg_shape_params)->SetValue(egg_shape_thickness, ob->ip->GetTime(), 0.0f);
				ob->GetParamBlock(egg_shape_params)->SetValue(egg_shape_rotation, ob->ip->GetTime(), 0.0f);

				break;
			}
		case 1:
			{
				p1 = vpt->SnapPoint(m,m,NULL,SNAP_IN_PLANE);

				// Assumption here is that p1 is on the X-Y plane.  
				Point3 a = p1 - p0;
				Point3 b;
				
				float com_factor = 6/(4-CENTER_OF_MASS_OFFSET);
				size1 = Length(a) * com_factor;
				b = Point3((float)0.0, size1, (float)0.0);

				float angle = acos((a%b)/(a.Length()*b.Length())) * TO_DEG * ((a.x>0) ? -1:1);

				//Set the overall size in parameter block
				ob->GetParamBlock(egg_shape_params)->SetValue(egg_shape_length, ob->ip->GetTime(), size1);
				ob->GetParamBlock(egg_shape_params)->SetValue(egg_shape_width, ob->ip->GetTime(), size1*TO_WIDTH);
				ob->GetParamBlock(egg_shape_params)->SetValue(egg_shape_rotation, ob->ip->GetTime(), angle);
				theEggShapeParmBlock.InvalidateUI();

				if((size1-EPSILON)<0.0 && msg==MOUSE_POINT)
				{
					ob->suspendSnap = FALSE;
					return CREATE_ABORT;
				}
				break;
			}
		case 2:
			{
				BOOL should_make_donut = TRUE;
				ob->GetParamBlock(egg_shape_params)->GetValue(egg_shape_make_donut, ob->ip->GetTime(), should_make_donut, ob->ivalid);

				if(should_make_donut)
				{
					p2 = vpt->SnapPoint(m,m,NULL,SNAP_IN_PLANE);
					size2 = Length(p2-p0) - Length(p1-p0);

					//Set the overall size in parameter block
					ob->GetParamBlock(egg_shape_params)->SetValue(egg_shape_thickness, ob->ip->GetTime(), size2);
					theEggShapeParmBlock.InvalidateUI();

					if(msg == MOUSE_POINT)
					{
						ob->suspendSnap = FALSE;
						return CREATE_STOP;
					}
					
					break;
				}
			}
		case 3:
			{
				return (size1 < EPSILON) ? CREATE_ABORT : CREATE_STOP;
			}
		}
	} 
	else if (msg == MOUSE_ABORT)
	{
		return CREATE_ABORT;
	}

	return TRUE;
}

/*******************************************************************/
// Dialog Callback Implementation (for type in parameters)
/*******************************************************************/
INT_PTR EggShapeTypeInParmDialog::DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	UNUSED_PARAM(lParam);
	UNUSED_PARAM(hWnd);
	UNUSED_PARAM(map);

	if(msg == WM_COMMAND)
	{
		if(LOWORD(wParam) == IDC_CREATE_BUTTON)
		{
			// Identity Matrix
			Matrix3 tm(1);
			float key_length = GetParamBlockDesc(egg_shape_keyboard)->GetFloat(egg_shape_keyboard_length);
			float key_width = GetParamBlockDesc(egg_shape_keyboard)->GetFloat(egg_shape_keyboard_width);
			float thickness = GetParamBlockDesc(egg_shape_keyboard)->GetFloat(egg_shape_keyboard_thickness);
			float angle = GetParamBlockDesc(egg_shape_keyboard)->GetFloat(egg_shape_keyboard_rotation);
			tm.SetTrans(GetParamBlockDesc(egg_shape_keyboard)->GetPoint3(egg_shape_keyboard_position));

			myObject->suspendSnap = FALSE;
			EggShape* new_obj = dynamic_cast<EggShape*>(myObject->ip->NonMouseCreate(tm));
		
			if(new_obj)
			{
				new_obj->GetParamBlock(egg_shape_params)->SetValue(egg_shape_length, t, key_length);
				new_obj->GetParamBlock(egg_shape_params)->SetValue(egg_shape_width, t, key_width);
				new_obj->GetParamBlock(egg_shape_params)->SetValue(egg_shape_thickness, t, thickness);
				new_obj->GetParamBlock(egg_shape_params)->SetValue(egg_shape_rotation, t, angle);
			}

			return TRUE;
		}
		if(LOWORD(wParam) == IDC_RESET_BUTTON)
		{
			Point3 p(0.f, 0.f, 0.f);
			GetParamBlockDesc(egg_shape_keyboard)->SetValue(egg_shape_keyboard_length, t, EGG_LENGTH_DEFAULT);
			GetParamBlockDesc(egg_shape_keyboard)->SetValue(egg_shape_keyboard_width, t, EGG_WIDTH_DEFAULT);
			GetParamBlockDesc(egg_shape_keyboard)->SetValue(egg_shape_keyboard_thickness, t, EGG_THICKNESS_DEFAULT);
			GetParamBlockDesc(egg_shape_keyboard)->SetValue(egg_shape_keyboard_rotation, t, EGG_ANGLE_DEFAULT);
			GetParamBlockDesc(egg_shape_keyboard)->SetValue(egg_shape_keyboard_position, t, p);
		}
	}
	else if( msg==CC_SPINNER_CHANGE)
	{
		if(LOWORD(wParam) == IDC_LENGTH_SPIN)
		{
			float key_length = GetParamBlockDesc(egg_shape_keyboard)->GetFloat(egg_shape_keyboard_length);
			GetParamBlockDesc(egg_shape_keyboard)->SetValue(egg_shape_keyboard_width, t, key_length*TO_WIDTH);
		}
		if(LOWORD(wParam) == IDC_WIDTH_SPIN)
		{
			float key_width = GetParamBlockDesc(egg_shape_keyboard)->GetFloat(egg_shape_keyboard_width);
			GetParamBlockDesc(egg_shape_keyboard)->SetValue(egg_shape_keyboard_length, t, key_width*TO_LENGTH);
		}
	}

	return FALSE;
}

/*******************************************************************/
// Dialog Callback Implementation (for instance parameters)
/*******************************************************************/
INT_PTR EggShapeParmDialog::DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	UNUSED_PARAM(lParam);
	UNUSED_PARAM(t);
	UNUSED_PARAM(map);

	if(msg == WM_INITDIALOG)
	{
		myObject->UpdateUI(hWnd);
	}
	else if(msg == WM_COMMAND)
	{
		if(LOWORD(wParam) == IDC_DONUT_CHECK)
		{
			myObject->UpdateUI(hWnd);
		}
	}

	return FALSE;
}

/*******************************************************************/
// Accessor class implementation
/*******************************************************************/
void EggAccessor::Set(PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t)
{
	UNUSED_PARAM(tabIndex);

	EggShape *an_egg = (EggShape *)owner;
	
	switch(id)
	{
	case egg_shape_length :
		{
			float new_val = v.f;

			if(an_egg->GetParamIsLocked(egg_shape_length))
				break;

			an_egg->LockParam(egg_shape_length);

			if(!an_egg->GetParamIsLocked(egg_shape_width))
				an_egg->GetParamBlock(egg_shape_params)->SetValue(egg_shape_width, t, new_val*TO_WIDTH);

			an_egg->UnlockParam(egg_shape_length);
		}
		break;
	case egg_shape_width :
		{
			float new_val = v.f;

			if(an_egg->GetParamIsLocked(egg_shape_width))
				break;

			an_egg->LockParam(egg_shape_width);

			if(!an_egg->GetParamIsLocked(egg_shape_length))
				an_egg->GetParamBlock(egg_shape_params)->SetValue(egg_shape_length, t, new_val*TO_LENGTH);

			an_egg->UnlockParam(egg_shape_width);
		}
		break;
	}
}