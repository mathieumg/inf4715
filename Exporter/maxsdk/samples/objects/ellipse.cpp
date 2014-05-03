/**********************************************************************
 *<
	FILE: rectangl.cpp

	DESCRIPTION:  An rectangular spline object implementation

	CREATED BY: Tom Hudson

	HISTORY: created 23 February 1995
			 updated to use ParamBlock2 system (July 14th, 2011)

 *>	Copyright (c) 1995, All Rights Reserved.
 **********************************************************************/

#include "prim.h"

#ifndef NO_OBJECT_SHAPES_SPLINES

#include "ellipse.h"

class EllipseObjCreateCallBack;

class EllipseObject: public SimpleSpline
{			   
friend class EllipseObjCreateCallBack;

public:	
	// Class vars
	static IObjParam *ip;
	void BuildShape(TimeValue t,BezierShape& ashape);

	EllipseObject();
	~EllipseObject();

	//  inherited virtual methods:
	CreateMouseCallBack* GetCreateMouseCallBack();
	void BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev);
	void EndEditParams( IObjParam *ip, ULONG flags,Animatable *next);
	const TCHAR *GetObjectName() { return GetString(IDS_TH_ELLIPSE); }
	void InitNodeName(TSTR& s) { s = GetString(IDS_TH_ELLIPSE); }		
	Class_ID ClassID() { return Class_ID(ELLIPSE_CLASS_ID,0); }  
	void GetClassName(TSTR& s) { s = GetString(IDS_TH_ELLIPSE_CLASS); }
	RefTargetHandle Clone(RemapDir& remap);
	BOOL ValidForDisplay(TimeValue t);

	int NumRefs();
	RefTargetHandle GetReference(int i);
	RefResult NotifyRefChanged( Interval changeInt,RefTargetHandle hTarget, PartID& partID, RefMessage message);

	int NumSubs();
	Animatable* SubAnim(int i);

	void InvalidateUI();
	void UpdateUI(HWND hwnd);

	// parameterBlock Accessor
	int NumParamBlocks() { return ellipse_num_param_blocks; }			// The parent does not have any PB2
	IParamBlock2*	GetParamBlock(int i) { return myParamBlock; }
	IParamBlock2*	GetParamBlockByID(BlockID id) { return (myParamBlock->ID()==id)?(myParamBlock):(NULL); }

	// IO
	bool SpecifySaveReferences(ReferenceSaveManager& referenceSaveManager);
	IOResult Save(ISave *isave);
	IOResult Load(ILoad *iload);

	static BOOL msBuild_outline;

protected:
	virtual void SetReference(int i, RefTargetHandle rtarg);

private:
	IParamBlock2*	myParamBlock;
	int	myNumRefs;
	int	myParentNumRefs;
};				

BOOL EllipseObject::msBuild_outline = FALSE;

//------------------------------------------------------

class EllipseObjClassDesc : public ClassDesc2 
{
public:
	virtual int 			IsPublic()			{ return TRUE; }
	virtual void *			Create(BOOL)		{ return new EllipseObject(); }
	virtual const TCHAR *	ClassName()			{ return GetString(IDS_TH_ELLIPSE_CLASS); }
	virtual SClass_ID		SuperClassID()		{ return SHAPE_CLASS_ID; }
	virtual Class_ID		ClassID()			{ return Class_ID(ELLIPSE_CLASS_ID,0); }
	virtual const TCHAR* 	Category()			{ return GetString(IDS_TH_SPLINES);  }

	// Added for param block 2 system
	const TCHAR*			InternalName()		{ return _T("Ellipse"); }
	HINSTANCE				HInstance()			{ return hInstance; }			// returns owning module handle

	void ResetClassParams(BOOL fileReset)
	{
		EllipseObject::msBuild_outline = FALSE;
		ClassDesc2::ResetClassParams(fileReset);
	}
};


/*************************************************************************************/
// Static Variables
/*************************************************************************************/
static EllipseObjClassDesc ellipseObjDesc;
ClassDesc* GetEllipseDesc() 
{ 
	return &ellipseObjDesc; 
}

// in prim.cpp  - The dll instance handle
extern HINSTANCE hInstance;

// class variable for sphere class.
IObjParam *EllipseObject::ip = NULL;

/*************************************************************************************/
// Parameter Block 2 Mechanism
/*************************************************************************************/
static ParamBlockDesc2 theEllispeCreateBlock 
( 
	ellipse_create_params,			// block id
	_T("ellipseCreate"),			// internal name string
	0,								// local name string
	&ellipseObjDesc,				// class descriptor associated with this guy
	P_CLASS_PARAMS + P_AUTO_UI,		// flags ... indicate that this is a class parameter block
	IDD_ELLIPSEPARAM1, IDS_TH_CREATION_METHOD, BEGIN_EDIT_CREATE, APPENDROLL_CLOSED, NULL,

	// The parameters specifications .... for each of the controls
	PB_CREATEMETHOD,	_T(""),		TYPE_INT, 0, IDS_RB_RADIO,
		p_default,	CREATE_EDGE,
		p_range,	CREATE_EDGE,	CREATE_CENTER,
		p_ui,		TYPE_RADIO,	2,	IDC_CREATEEDGE,	IDC_CREATECENTER,
		p_end,
	p_end
);

static ParamBlockDesc2 theEllispeTypeInBlock 
( 
	ellipse_type_in_params,			// block id
	_T("ellipseTypeIn"),			// internal name string
	0,								// local name string
	&ellipseObjDesc,					// class descriptor associated with this guy
	P_CLASS_PARAMS + P_AUTO_UI,		// flags ... indicate that this is a class parameter block
	IDD_ELLIPSEPARAM3, IDS_TH_KEYBOARD_ENTRY, BEGIN_EDIT_CREATE, APPENDROLL_CLOSED, NULL,

	// The parameters specifications .... for each of the controls
	PB_TI_POS,	_T(""),		TYPE_POINT3, 0, IDS_RB_POS,
		p_default,	Point3(0, 0, 0),
		p_range,	-99999999.0f,	99999999.0f,
		p_ui,		TYPE_SPINNER, EDITTYPE_UNIVERSE, IDC_TI_POSX, IDC_TI_POSXSPIN, IDC_TI_POSY, IDC_TI_POSYSPIN, IDC_TI_POSZ, IDC_TI_POSZSPIN, SPIN_AUTOSCALE, 
		p_end,
	PB_TI_LENGTH,	_T(""),	TYPE_FLOAT,	0,	IDS_RB_SPIN,
		p_default, 			ELLISPE_LENGTH_DEFAULT, 
		p_range, 			MIN_LENGTH,	MAX_LENGTH, 
		p_ui, 				TYPE_SPINNER,	EDITTYPE_UNIVERSE,	IDC_LENGTHEDIT,	IDC_LENSPINNER,	SPIN_AUTOSCALE, 
		p_end,
	PB_TI_WIDTH,	_T(""),	TYPE_FLOAT,	0,	IDS_RB_SPIN,
		p_default, 			ELLISPE_WIDTH_DEFAULT, 
		p_range, 			MIN_WIDTH,	MAX_WIDTH, 
		p_ui, 				TYPE_SPINNER,	EDITTYPE_UNIVERSE,	IDC_WIDTHEDIT,	IDC_WIDTHSPINNER,	SPIN_AUTOSCALE, 
		p_end,
	PB_TI_THICKNESS,	_T(""),	TYPE_FLOAT,	0,	IDS_RB_SPIN,
		p_default, 			ELLISPE_THICKNESS_DEFAULT, 
		p_range, 			MIN_THICK,	MAX_THICK, 
		p_ui, 				TYPE_SPINNER,	EDITTYPE_UNIVERSE,	IDC_THICKNESSEDIT,	IDC_THICKSPINNER,	SPIN_AUTOSCALE, 
		p_end,
	p_end
);

static ParamBlockDesc2 theEllipseParamBlock
(
	ellipse_params,					// block id
	_T("ellipseParam"),				// internal name string
	0,								// local name string
	&ellipseObjDesc,				// class descriptor associated with this guy
	P_AUTO_CONSTRUCT + P_AUTO_UI,	// flags
	USERPBLOCK,						// the reference id, this parameter block take the ID from SimpleSpline::pblock and override.  
									// For any new parameter block, the ID should start at the last index; that is, SimpleSpline::NumRefs()
	IDD_ELLIPSEPARAM2, IDS_TH_PARAMETERS, 0, 0, NULL,

	// The parameters specifications .... for each of the controls
	PB_LENGTH,	_T("length"),	TYPE_FLOAT,	P_ANIMATABLE|P_RESET_DEFAULT,	IDS_RB_LENGTH,
		p_default, 			ELLISPE_LENGTH_DEFAULT, 
		p_ms_default,		25.f,
		p_range, 			MIN_LENGTH,	MAX_LENGTH, 
		p_ui, 				TYPE_SPINNER,	EDITTYPE_UNIVERSE,	IDC_LENGTHEDIT,	IDC_LENSPINNER,	SPIN_AUTOSCALE, 
		p_end,
	PB_WIDTH,	_T("width"),	TYPE_FLOAT,	P_ANIMATABLE|P_RESET_DEFAULT,	IDS_RB_WIDTH,
		p_default, 			ELLISPE_WIDTH_DEFAULT, 
		p_ms_default,		35.f,
		p_range, 			MIN_WIDTH,	MAX_WIDTH, 
		p_ui, 				TYPE_SPINNER,	EDITTYPE_UNIVERSE,	IDC_WIDTHEDIT,	IDC_WIDTHSPINNER,	SPIN_AUTOSCALE, 
		p_end,
	PB_THICKNESS,	_T("ellipse_thickness"),	TYPE_FLOAT,	P_ANIMATABLE|P_RESET_DEFAULT,	IDS_RB_THICKNESS,
		p_default, 			ELLISPE_THICKNESS_DEFAULT, 
		p_ms_default,		ELLISPE_THICKNESS_DEFAULT,
		p_range, 			MIN_THICK,	MAX_THICK, 
		p_ui, 				TYPE_SPINNER,	EDITTYPE_UNIVERSE,	IDC_THICKNESSEDIT,	IDC_THICKSPINNER,	SPIN_AUTOSCALE, 
		p_end,
	PB_OUTLINE,	_T("ellipse_outline"), 	TYPE_BOOL,	P_ANIMATABLE|P_RESET_DEFAULT, 	IDS_RB_OUTLINE, 
		p_default, 			ELLIPSE_OUTLINE_DEFAULT, 
		p_ms_default, 		ELLIPSE_OUTLINE_DEFAULT,
		p_ui, 				TYPE_SINGLECHEKBOX,		IDC_OUTLINE,
		p_end,
	p_end
);


// For the old ParamBlock
static ParamBlockDescID descVer0[] = 
{
	{ TYPE_FLOAT, NULL, TRUE, PB_LENGTH },		
	{ TYPE_FLOAT, NULL, TRUE, PB_WIDTH }
};

#define NUM_PARAM_V0 2

// Array of old versions
static ParamVersionDesc versions[] = 
{
	ParamVersionDesc(descVer0, NUM_PARAM_V0, 0)	
};

/*********************************************************************/
// Callbacks 
/*********************************************************************/
class EllipseTypeInDlgProc : public ParamMap2UserDlgProc 
{
public:
	EllipseObject *ro;

	EllipseTypeInDlgProc(EllipseObject *d) { ro=d; }
	INT_PTR DlgProc(TimeValue t,IParamMap2 *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	void DeleteThis() { delete this; }
};

INT_PTR EllipseTypeInDlgProc::DlgProc(TimeValue t,IParamMap2 *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch (msg) 
	{
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDC_TI_CREATE: 
				{
					Matrix3 tm(1);
					Point3 pos = theEllispeTypeInBlock.GetPoint3(PB_TI_POS);
					float length = theEllispeTypeInBlock.GetFloat(PB_TI_LENGTH);
					float width = theEllispeTypeInBlock.GetFloat(PB_TI_WIDTH);
					float thickness = theEllispeTypeInBlock.GetFloat(PB_TI_THICKNESS);
					tm.SetTrans(pos);

					ro->suspendSnap = FALSE;

					EllipseObject* new_object = dynamic_cast<EllipseObject*>(ro->ip->NonMouseCreate(tm));
					if(new_object)
					{
						new_object->GetParamBlockByID(ellipse_params)->SetValue(PB_LENGTH, t, length);
						new_object->GetParamBlockByID(ellipse_params)->SetValue(PB_WIDTH, t, width);
						new_object->GetParamBlockByID(ellipse_params)->SetValue(PB_THICKNESS, t, thickness);
					}

					return TRUE;	
				}
			}
			break;	
	}
	return FALSE;
}

class EllipseParamDlgProc : public ParamMap2UserDlgProc 
{
public:
	EllipseObject *ro;

	EllipseParamDlgProc(EllipseObject *d) { ro=d; }
	INT_PTR DlgProc(TimeValue t,IParamMap2 *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	void DeleteThis() { delete this; }
};

INT_PTR EllipseParamDlgProc::DlgProc(TimeValue t,IParamMap2 *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch (msg) 
	{
	case WM_INITDIALOG :
		{
			ro->UpdateUI(hWnd);
			break;
		}
	case WM_COMMAND :
		{
			switch (LOWORD(wParam))
			{
			case IDC_OUTLINE: 
				{
					ro->UpdateUI(hWnd);
					break;
				}
			}
			break;
		}
	}
	
	return FALSE;
}

class EllipseObjCreateCallBack: public CreateMouseCallBack
{
	EllipseObject *ob;
	Point3 p0, p1, p2;
	IPoint2 sp0;
	int createType;
public:
	EllipseObjCreateCallBack();
	int proc( ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat );
	void SetObj(EllipseObject *obj) { ob = obj; }
};

EllipseObjCreateCallBack::EllipseObjCreateCallBack() : sp0(0, 0), p0(0.f, 0.f, 0.f), p1(0.f, 0.f, 0.f), p2(0.f, 0.f, 0.f)
{
	ob = NULL;
	createType = CREATE_EDGE;
}

int EllipseObjCreateCallBack::proc(ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat ) 
{
	if ( ! vpt || ! vpt->IsAlive() )
	{
		// why are we here
		DbgAssert(!_T("Invalid viewport!"));
		return FALSE;
	}

	Point3 d;
#ifdef _3D_CREATE
	DWORD snapdim = SNAP_IN_3D;
#else
	DWORD snapdim = SNAP_IN_PLANE;
#endif

	if (msg == MOUSE_FREEMOVE)
	{
#ifdef _OSNAP
		vpt->SnapPreview(m,m,NULL, snapdim);
#endif
	}
	if (msg==MOUSE_POINT||msg==MOUSE_MOVE) 
	{
		switch(point) 
		{
		case 0:
			sp0 = m;
			ob->suspendSnap = TRUE;

			// Get the creation method
			createType = theEllispeCreateBlock.GetInt(PB_CREATEMETHOD);
			p0 = vpt->SnapPoint(m,m,NULL,snapdim);
			p1 = p0 + Point3(.01,.01,.0);

			ob->GetParamBlockByID(ellipse_params)->SetValue(PB_LENGTH,0,0.f);
			ob->GetParamBlockByID(ellipse_params)->SetValue(PB_WIDTH,0,0.f);	
			ob->GetParamBlockByID(ellipse_params)->SetValue(PB_THICKNESS, 0, 0.f);

			if(createType == CREATE_EDGE)
				mat.SetTrans(float(.5)*(p0+p1));
			else
				mat.SetTrans(p0);
			break;
		case 1: 
			{ 
				p1 = vpt->SnapPoint(m,m,NULL,snapdim);
				p1.z = p0.z; 
				d = p1-p0;
				float w = float(fabs(d.x));
				float l = float(fabs(d.y));
				if(flags & MOUSE_CTRL) 
				{
					if(createType == CREATE_EDGE) 
					{
						float ysign = (d.y < 0.0f) ? -1.0f : 1.0f;
						mat.SetTrans(float(.5)*(p0+Point3(p1.x,p0.y+ysign*w,0.0f)));
					}
					else 
					{
						mat.SetTrans(p0);
						w = w * 2.0f;
					}
					ob->GetParamBlockByID(ellipse_params)->SetValue(PB_LENGTH,0,w);
					ob->GetParamBlockByID(ellipse_params)->SetValue(PB_WIDTH,0,w);
				}
				else 
				{
					if(createType == CREATE_EDGE)
					{
						mat.SetTrans(float(.5)*(p0+p1));
					}
					else 
					{
						mat.SetTrans(p0);
						w = w * 2.0f;
						l = l * 2.0f;
					}
					ob->GetParamBlockByID(ellipse_params)->SetValue(PB_WIDTH,0,w);
					ob->GetParamBlockByID(ellipse_params)->SetValue(PB_LENGTH,0,l);
				}

				theEllipseParamBlock.InvalidateUI();
				if (msg == MOUSE_POINT && (Length(m-sp0)<3 || Length(p1-p0)<0.1f)) 
				{
					ob->suspendSnap = FALSE;
					return CREATE_ABORT;
				}

				break;
			}
		case 2 :
			{
				ob->GetParamBlockByID(ellipse_params)->GetValue(PB_OUTLINE, 0, EllipseObject::msBuild_outline, ob->ivalid);
				if(EllipseObject::msBuild_outline)
				{
					p2 = vpt->SnapPoint(m,m,NULL,snapdim);
					float thickness = Length(p2-p0) - Length(p1-p0);
					ob->GetParamBlockByID(ellipse_params)->SetValue(PB_THICKNESS,0,thickness);

					if (msg == MOUSE_POINT) 
					{
						ob->suspendSnap = FALSE;
						return (Length(m-sp0)<3 || Length(p1-p0)<0.1f) ? CREATE_ABORT: CREATE_STOP;
					}
				}
				else
				{
					ob->suspendSnap = FALSE;
					return (Length(m-sp0)<3 || Length(p1-p0)<0.1f) ? CREATE_ABORT: CREATE_STOP;
				}
				break;
			}
		}
	}
	else if (msg == MOUSE_ABORT) 
	{
		return CREATE_ABORT;
	}

	return TRUE;
}

static EllipseObjCreateCallBack ellipseCreateCB;

/*********************************************************************/
// Ellipse Methods
/*********************************************************************/
EllipseObject::EllipseObject() : SimpleSpline() 
{
	myParentNumRefs = SimpleSpline::NumRefs();

	// The ellipse does not have any more reference than its parent, 
	// since the only parameter hosted by ellipse is overriding the parents
	myNumRefs = 0;					
	pblock = NULL;
	myParamBlock = NULL;

	// Build the interpolations parameter block in SimpleSpline
	ReadyInterpParameterBlock();
	ellipseObjDesc.MakeAutoParamBlocks(this);

	DbgAssert(myParamBlock && _M("Fail to Create Parameter Block"));

	// Set class value
	myParamBlock->SetValue(PB_OUTLINE, 0, msBuild_outline);
}

EllipseObject::~EllipseObject()
{
	DeleteAllRefsFromMe();
	myParamBlock = NULL;
	UnReadyInterpParameterBlock();
}

void EllipseObject::BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev)
{
	SimpleSpline::BeginEditParams(ip, flags, prev);
	this->ip = ip;
	
	ellipseObjDesc.BeginEditParams(ip, this, flags, prev);
	theEllipseParamBlock.SetUserDlgProc( new EllipseParamDlgProc(this) );
	theEllispeTypeInBlock.SetUserDlgProc( new EllipseTypeInDlgProc(this) );
}
		
void EllipseObject::EndEditParams( IObjParam *ip, ULONG flags,Animatable *next )
{
	// Get class value
	myParamBlock->GetValue(PB_OUTLINE, 0, msBuild_outline, ivalid);

	ellipseObjDesc.EndEditParams(ip, this, flags, next);
	SimpleSpline::EndEditParams(ip,flags,next);
	this->ip = NULL;

	// Save these values in class variables so the next object created will inherit them.
}

static void MakeCircle(BezierShape& ashape, float radius, float xmult, float ymult) 
{
	float vector = CIRCLE_VECTOR_LENGTH * radius;
	// Delete all points in the existing spline
	Spline3D *spline = ashape.NewSpline();
	// Now add all the necessary points
	Point3 mult = Point3(xmult, ymult, 1.0f);
	for(int ix=0; ix<4; ++ix) 
	{
		float angle = 6.2831853f * (float)ix / 4.0f;
		float sinfac = (float)sin(angle), cosfac = (float)cos(angle);
		Point3 p(cosfac * radius, sinfac * radius, 0.0f);
		Point3 rotvec = Point3(sinfac * vector, -cosfac * vector, 0.0f);
		spline->AddKnot(SplineKnot(KTYPE_BEZIER,LTYPE_CURVE,p * mult,(p + rotvec) * mult,(p - rotvec) * mult));
	}
	spline->SetClosed();
	spline->ComputeBezPoints();
}

void EllipseObject::BuildShape(TimeValue t, BezierShape& ashape) 
{
	// Start the validity interval at forever and whittle it down.
	ivalid = FOREVER;
	float length;
	float width;
	float thickness;
	BOOL build_outine = FALSE;
	myParamBlock->GetValue(PB_LENGTH, t, length, ivalid);
	myParamBlock->GetValue(PB_WIDTH, t, width, ivalid);
	myParamBlock->GetValue(PB_THICKNESS, t, thickness, ivalid);
	myParamBlock->GetValue(PB_OUTLINE, t, build_outine, ivalid);
	LimitValue( length, MIN_LENGTH, MAX_LENGTH );
	LimitValue( width, MIN_WIDTH, MAX_WIDTH );
	LimitValue(thickness, MIN_THICK, MAX_THICK );

	// Delete the existing shape and create a new spline in it
	ashape.NewShape();

	// Get parameters from SimpleSpline and place them in the BezierShape
	int steps;
	BOOL optimize,adaptive;
	ipblock->GetValue(IPB_STEPS, t, steps, ivalid);
	ipblock->GetValue(IPB_OPTIMIZE, t, optimize, ivalid);
	ipblock->GetValue(IPB_ADAPTIVE, t, adaptive, ivalid);
	ashape.steps = adaptive ? -1 : steps;
	ashape.optimize = optimize;

	float radius, xmult, ymult;
	if(length < width) 
	{
		radius = width;
		xmult = 1.0f;
		ymult = length / width;
	}
	else
	if(width < length) 
	{
		radius = length;
		xmult = width / length;
		ymult = 1.0f;
	}
	else 
	{
		radius = length;
		xmult = ymult = 1.0f;
	}
	MakeCircle(ashape, radius / 2.0f, xmult, ymult);

	if(build_outine)
	{
		length += 2*thickness;
		width += 2*thickness;
		LimitValue( length, MIN_LENGTH, MAX_LENGTH );
		LimitValue( width, MIN_WIDTH, MAX_WIDTH );

		if(length < width) 
		{
			radius = width;
			xmult = 1.0f;
			ymult = length / width;
		}
		else if(width < length) 
		{
			radius = length;
			xmult = width / length;
			ymult = 1.0f;
		}
		else 
		{
			radius = length;
			xmult = ymult = 1.0f;
		}
		MakeCircle(ashape, radius / 2.0f, xmult, ymult);
	}

	ashape.UpdateSels();	// Make sure it readies the selection set info
	ashape.InvalidateGeomCache();
}

void EllipseObject::InvalidateUI() 
{
	myParamBlock->GetDesc()->InvalidateUI();
}

void EllipseObject::UpdateUI(HWND hwnd)
{
	Interval valid;
	BOOL build_outine = FALSE;
	TimeValue t = ip? ip->GetTime() : 0;

	myParamBlock->GetValue(PB_OUTLINE, t, build_outine, valid);
	ICustEdit *thickness_edit = GetICustEdit(GetDlgItem(hwnd, IDC_THICKNESSEDIT));
	ISpinnerControl *thickness_spin = GetISpinner(GetDlgItem(hwnd, IDC_THICKSPINNER));

	if(!thickness_edit || !thickness_spin)
		return;

	if(build_outine)
	{
		EnableWindow(GetDlgItem(hwnd,IDC_THICKNESS_TEXT),TRUE);
		thickness_edit->Enable();
		thickness_spin->Enable();
	}
	else
	{
		EnableWindow(GetDlgItem(hwnd,IDC_THICKNESS_TEXT),FALSE);
		thickness_edit->Disable();
		thickness_spin->Disable();
	}

	ReleaseICustEdit(thickness_edit);
	ReleaseISpinner(thickness_spin);
}

CreateMouseCallBack* EllipseObject::GetCreateMouseCallBack() 
{
	ellipseCreateCB.SetObj(this);
	return(&ellipseCreateCB);
}

//
// Reference Managment:
//

int	EllipseObject::NumRefs()
{
	// The number of references that we have is the sum of our own stuffs plus whatever our parent have.
	return myNumRefs + myParentNumRefs;
}

RefTargetHandle EllipseObject::GetReference(int i)
{
	// if we are trying to get the user parameter block, give them the new block, otherwise, proceed as usual
	if(i == USERPBLOCK)
	{
		return myParamBlock;
	}

	return SimpleSpline::GetReference(i);
}

void EllipseObject::SetReference(int i, RefTargetHandle rtarg)
{
	if(i == USERPBLOCK)
	{
		myParamBlock = (IParamBlock2*)rtarg;

		return;
	}

	SimpleSpline::SetReference(i, rtarg);
}

int EllipseObject::NumSubs()
{
	return myNumRefs + myParentNumRefs;
}

Animatable* EllipseObject::SubAnim(int i) 
{
	if(i==USERPBLOCK)
	{
		return myParamBlock;
	}

	return SimpleSpline::SubAnim(i);
}

RefResult EllipseObject::NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, PartID& partID,  RefMessage message) 
{
	if(hTarget != myParamBlock)
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

RefTargetHandle EllipseObject::Clone(RemapDir& remap) 
{
	EllipseObject* newob = new EllipseObject();
	newob->SimpleSplineClone(this, remap);
	newob->ReplaceReference(USERPBLOCK,remap.CloneRef(myParamBlock));	
	newob->ivalid.SetEmpty();	
	BaseClone(this, newob, remap);
	return(newob);
}

BOOL EllipseObject::ValidForDisplay(TimeValue t) 
{
	float length, width;
	myParamBlock->GetValue(PB_LENGTH, t, length, ivalid);
	myParamBlock->GetValue(PB_WIDTH, t, width, ivalid);
	return (length == 0 || width == 0) ? FALSE : TRUE;
}

/******************************************************************************/
// ISave and ILoad stuff
/******************************************************************************/

#define PARAM2_CHUNK 0x1010

bool EllipseObject::SpecifySaveReferences(ReferenceSaveManager& referenceSaveManager)
{
	// if saving to previous version that used pb1 instead of pb2...
	DWORD saveVersion = GetSavingVersion();
	if (saveVersion != 0 && saveVersion <= MAX_RELEASE_R14)
	{
		// create the pb1 instance
		IParamBlock* paramBlock1 = CreateParameterBlock(descVer0, NUM_PARAM_V0, 0);
		DbgAssert(paramBlock1 != NULL);
		if (paramBlock1)
		{
			// copy data from the pb2 to the pb1
			int numParamsCopied = CopyParamBlock2ToParamBlock( myParamBlock, paramBlock1, descVer0, NUM_PARAM_V0);
			
			// register the reference slot replacement
			referenceSaveManager.ReplaceReferenceSlot(USERPBLOCK, paramBlock1);
		}
	}
	return SimpleSpline::SpecifySaveReferences(referenceSaveManager);
}

IOResult EllipseObject::Save(ISave *isave)
{ 
	IOResult res;

	res = SimpleSpline::Save(isave);
	if (res!=IO_OK) return res;

	DWORD saveVersion = isave->SavingVersion();
	if ( !(saveVersion != 0 && saveVersion <= MAX_RELEASE_R14) ) 
	{
		isave->BeginChunk(PARAM2_CHUNK);
		isave->EndChunk();
	}

	return IO_OK;
}  

IOResult EllipseObject::Load(ILoad *iload) 
{ 
	
	IOResult res;
	int id;
	bool loadFromParam1 = true;

	res = SimpleSpline::Load(iload);
	if(res != IO_OK)
		return res;

	while (IO_OK == (res=iload->OpenChunk())) 
	{
		switch(id=iload->CurChunkID())  
		{
		case PARAM2_CHUNK:
			loadFromParam1 = false;
			break;
		}
		iload->CloseChunk();
		if (res != IO_OK) 
			return res;
	}

	if ( loadFromParam1 ) 
	{
   		// register old version ParamBlock to ParamBlock2 converter
		ParamBlock2PLCB* plcb = new ParamBlock2PLCB(versions, 1, &theEllipseParamBlock, this, USERPBLOCK);
		iload->RegisterPostLoadCallback(plcb);
	}

	return IO_OK;
}

#endif // NO_OBJECT_SHAPES_SPLINES

