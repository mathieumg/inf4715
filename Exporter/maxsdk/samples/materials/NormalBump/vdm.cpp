/**********************************************************************
 *<
	FILE: vdm.cpp

	DESCRIPTION:VDM(vectordisplacementmap)

	CREATED BY:  Halfdan Ingvarsson

	HISTORY:  version 1.0

 *>	Copyright (c) 2010, All Rights Reserved.
 **********************************************************************/

#include "vdm.h"
#include "VNormal.h"

//===========================================================================
//
// Class VDM - Class Descriptor
//
//===========================================================================

class VDMClassDesc:
	public ClassDesc2, 
	public IMtlRender_Compatibility_MtlBase  
{
	public:
	VDMClassDesc();

	int						IsPublic() { 
		return TRUE;
	}

	void*					Create(BOOL loading = FALSE) { return new VDM(); }
	const TCHAR*			ClassName() { return GetString(IDS_VDM_CLASS_NAME); }
	SClass_ID				SuperClassID() { return TEXMAP_CLASS_ID; }
	Class_ID				ClassID() { return VDM_CLASS_ID; }
	const TCHAR*			Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*			InternalName() { return _T("VDM"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE				HInstance() { return hInstance; }				// returns owning module handle
	virtual bool			IsCompatibleWithRenderer(ClassDesc& rendererClassDesc);
};

VDMClassDesc::VDMClassDesc() {
	IMtlRender_Compatibility_MtlBase::Init(*this);
}

static VDMClassDesc VDMDesc;
ClassDesc2* GetVDMDesc() { return &VDMDesc; }


//===========================================================================
//
// Class VDM - Parameter Block
//
//===========================================================================

static ParamBlockDesc2 vdm_param_blk (
	vdm_params, _T("params"),  0, &VDMDesc, P_AUTO_CONSTRUCT + P_AUTO_UI, PBLOCK_REF, 
	//rollout
	IDD_VDM_PANEL, IDS_PARAMS, 0, 0, NULL,
	// params
	vdm_mult_spin, 	_T("mult_spin"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_MULT_SPIN, 
		p_default, 		1.0f, 
		p_range, 		-100.0f,1000.0f, 
		p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_VDM_MULT_EDIT,	IDC_VDM_MULT_SPIN, 0.1f, 
		p_end,
	vdm_map_vector_enabled,_T("vector_map_enabled"),		TYPE_BOOL,	0,	IDS_VDM_MAP_VECTOR_ENABLED,
		p_default,		TRUE,
		p_ui,			TYPE_SINGLECHEKBOX,		IDC_VDM_VECTOR_MAP_ENABLED,
		p_end,
	vdm_map_vector,	_T("vector_map"),		TYPE_TEXMAP,  P_OWNERS_REF,  IDS_VDM_MAP_VECTOR,
		p_refno,		1,
		p_subtexno,		0,
		p_ui,			TYPE_TEXMAPBUTTON,   IDC_VDM_VECTOR_MAP,
		p_end,
	vdm_is_hdr,		_T("vector_map_is_hdr"),		TYPE_BOOL,	0,	IDS_VDM_MAP_IS_HDR,
		p_default,		TRUE,
		p_ui,			TYPE_SINGLECHEKBOX,		IDC_VDM_MAP_IS_HDR,
		p_end,
	vdm_method,		_T("method"),		TYPE_INT,	0,		IDS_VDM_METHOD_RADIO,
		p_default,      2,
		p_ui, 			TYPE_RADIO, 3, IDC_VDM_WORLD_RAD, IDC_VDM_OBJECT_RAD, IDC_VDM_TANGENT_RAD,
		p_vals,			0,1,2,
		p_end,
	p_end
	);



//===========================================================================
//
// Class Gnormal - Implementation
//
//===========================================================================

VDM::VDM() :
	vdmTex( NULL ),
	pblock( NULL )
{
	VDMDesc.MakeAutoParamBlocks(this);
	Init();
}

//From MtlBase
void VDM::Reset() 
{
	// Nuke the VDM sub-map
	DeleteReference( 1 );
	Init();
	ivalid.SetEmpty();
}

void VDM::Init()
{
	ivalid.SetEmpty();
	vdmMult = 1.0f;
	vdmIsMapEnabled = true;
	vdmIsHDR = true;
	vdmSpaceType = VDM_ST_TANGENT;
}

void VDM::InvalidateUI()
{
	vdm_param_blk.InvalidateUI(pblock->LastNotifyParamID());
}

inline void VDM::Reorder(AColor &s) { 
}

inline void VDM::Reorder(Point3& p) { 
}



//-----------------------------------------------------------------------------
//-- From Animatable

void VDM::GetClassName(TSTR& s)
{
	s = GetString(IDS_VDM_CLASS_NAME_IMP);
}
	 
Animatable* VDM::SubAnim(int i) 
{
	switch (i) {
	case 0:		return pblock;
	default:	return vdmTex;
	}
}

TSTR VDM::SubAnimName(int i) 
{
	switch (i) {
	case 0:		return GetString(IDS_PARAMS);
	default:	return GetSubTexmapTVName(i-1);
	}
}

int VDM::RenderBegin(TimeValue t, ULONG flags) {
	//free all vnormal data; it will be initialized inside Eval()
	GetVNormalMgr()->Free();
	return 1;
}

int VDM::RenderEnd(TimeValue t)
{
	GetVNormalMgr()->Free(); //free all vnormal data
	return 1;
}

//-----------------------------------------------------------------------------
//-- From ReferenceMaker

#define MTL_HDR_CHUNK 0x4000

IOResult VDM::Save(ISave *isave) 
{
	IOResult res;
	isave->BeginChunk(MTL_HDR_CHUNK);
	res = MtlBase::Save(isave);
	if (res!=IO_OK) return res;
	isave->EndChunk();
	return IO_OK;
}

IOResult VDM::Load(ILoad *iload) 
{ 
	IOResult res;
	int id;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(id=iload->CurChunkID())  {
			case MTL_HDR_CHUNK:
				res = MtlBase::Load(iload);
				break;
			}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
		}
	return IO_OK;
}

RefTargetHandle VDM::GetReference(int i) 
{
	switch (i) {
	case 0:		return pblock;
	default:	return vdmTex;
	}
}

void VDM::SetReference(int i, RefTargetHandle rtarg) 
{
	switch(i) {
	case 0:		pblock = (IParamBlock2 *)rtarg;	break;
	default:	vdmTex = (Texmap *)rtarg;	break;
	}
}

RefResult VDM::NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
   PartID& partID, RefMessage message ) 
{
	switch (message) {
	case REFMSG_CHANGE:
		ivalid.SetEmpty();
		if (hTarget==pblock)
		{
			ParamID changing_param = pblock->LastNotifyParamID();
			vdm_param_blk.InvalidateUI(changing_param);
		}
		break;
	}	
	return(REF_SUCCEED);
}

//-----------------------------------------------------------------------------
//-- From ReferenceTarget

RefTargetHandle VDM::Clone(RemapDir &remap) 
{
	VDM *mnew = new VDM();
	*((MtlBase*)mnew) = *((MtlBase*)this); // copy superclass stuff
    mnew->ReplaceReference(0,remap.CloneRef(pblock));

	mnew->ivalid.SetEmpty();
	mnew->vdmTex = NULL;
	if (vdmTex)
		mnew->ReplaceReference(1,remap.CloneRef(vdmTex));
	mnew->vdmMult = vdmMult;
	mnew->vdmIsHDR = vdmIsHDR;
	mnew->vdmSpaceType = vdmSpaceType;

	BaseClone(this, mnew, remap);
	return (RefTargetHandle)mnew;
}

//-----------------------------------------------------------------------------
//-- From MtlBase

ULONG VDM::LocalRequirements(int subMtlNum) {
	ULONG retval = 0;
	if (vdmTex)
		retval |= vdmTex->LocalRequirements(subMtlNum);
	return retval;
}

void VDM::LocalMappingsRequired(int subMtlNum, BitArray & mapreq, BitArray &bumpreq) {
	if (vdmTex)
		vdmTex->LocalMappingsRequired(subMtlNum,mapreq,bumpreq);
}

ParamDlg* VDM::CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp) 
{
	IAutoMParamDlg* masterDlg = VDMDesc.CreateParamDlgs(hwMtlEdit, imp, this);
	return masterDlg;	
}

void VDM::Update(TimeValue t, Interval& valid) 
{
	BOOL doUpdate = TRUE;
	if( pblock->GetInt( vdm_method, t ) == VDM_ST_TANGENT )
		ivalid.SetInstant(t);
	else if (!ivalid.InInterval(t))
		ivalid.SetInfinite();
	else doUpdate = FALSE;

	if( doUpdate ) {
		int		spaceType, isHDR, isMapEnabled;

		pblock->GetValue(vdm_mult_spin, t, vdmMult, ivalid);
		pblock->GetValue(vdm_method, t, spaceType, ivalid);
		pblock->GetValue(vdm_is_hdr, t, isHDR, ivalid);
		pblock->GetValue(vdm_map_vector_enabled, t, isMapEnabled, ivalid);

		vdmSpaceType = (VDMSpaceType)spaceType;
		vdmIsHDR = isHDR?true:false;
		vdmIsMapEnabled = isMapEnabled?true:false;

		if (vdmTex) vdmTex->Update(t,ivalid);
	}
	valid &= ivalid;
	GetVNormalMgr()->Free(); //free all vnormal data
}

void VDM::SetSubTexmap(int i, Texmap *m) 
{
	ReplaceReference(i+1,m);
	if(i==0)
	{
		vdm_param_blk.InvalidateUI(vdm_map_vector);
		ivalid.SetEmpty();
	}
}

TSTR VDM::GetSubTexmapSlotName(int i) 
{	
	switch (i) 
	{
	case 0: return TSTR(GetString(IDS_VDM_MAP_VECTOR));
	default: return TSTR(_T(""));
	}
}

DWORD_PTR VDM::GetActiveTexHandle(TimeValue t, TexHandleMaker& thmaker)
{
	//TODO: Return the texture handle to this texture map
	return NULL;
}

void VDM::ActivateTexDisplay(BOOL onoff)
{
	//TODO: Implement this only if SupportTexDisplay() returns TRUE
}

//-----------------------------------------------------------------------------
//-- From Texmap

AColor VDM::EvalColor( ShadeContext& sc ) {
	return AColor(0.0f,0.0f,0.0f,1.0f);
}

float VDM::EvalMono(ShadeContext& sc)
{
	return( 0.0f );
}

//===========================================================================
//
// Mental Ray support
//
//===========================================================================


namespace {

	bool GetParamIDByName(ParamID& paramID, const TCHAR* name, IParamBlock2* pBlock) {

		DbgAssert(pBlock != NULL);
		int count = pBlock->NumParams();
		for(int i = 0; i < count; ++i) {
			ParamID id = pBlock->IndextoID(i);
         const ParamDef& paramDef = pBlock->GetParamDef(id);
			if(_tcsicmp(name, paramDef.int_name) == 0) {
				paramID = id;
				return true;
			}
		}

		DbgAssert(false);
		return false;
	}

	template<typename T>
	void TranslateMRShaderParameter(IParamBlock2* paramBlock, TCHAR* paramName, T value) {

		ParamID paramID;
		if(GetParamIDByName(paramID, paramName, paramBlock)) {
			paramBlock->SetValue(paramID, 0, value);
		}
		else {
			DbgAssert(false);
		}
	}
}

bool VDMClassDesc::IsCompatibleWithRenderer(ClassDesc& rendererClassDesc) {

	Class_ID classID = rendererClassDesc.ClassID();

	if((classID == MRRENDERER_CLASSID) /* || (classID == SCANLINERENDERER_CLASS_ID) */ ) {
		return true;
	}
	else {
		// Return 'true' only if the renderer doesn't implement the compatibility interface.
		// This ensures that we are compatible with all renderers unless they specify the contrary.
		IMtlRender_Compatibility_Renderer* rendererCompatibility = Get_IMtlRender_Compatibility_Renderer(rendererClassDesc);
		return (rendererCompatibility == NULL);
	}
}

imrShader* VDM::GetMRShader(imrShaderCreation& shaderCreation) {

	// Create the shader and return it
	imrShader* shader = shaderCreation.CreateShader(_T("max_vdm"), GetFullName());
	return shader;
}

void VDM::ReleaseMRShader() {

	/* Do nothing */
}

bool VDM::NeedsAutomaticParamBlock2Translation() {

	return false;
}

void VDM::TranslateParameters(imrTranslation& translationInterface, imrShader* shader, TimeValue t, Interval& valid) {

	// Get the parameter block in which we need to store the parameter values
	IParamBlock2* paramsPBlock = shader->GetParametersParamBlock();
	if(paramsPBlock != NULL) {

		Interval localValid = FOREVER;

		// Update at the current time 
		Update(t, localValid);

		TranslateMRShaderParameter(paramsPBlock, _T("vector_map"), vdmIsMapEnabled ? vdmTex : static_cast<Texmap *>( NULL ) );
		TranslateMRShaderParameter(paramsPBlock, _T("scale"), vdmMult);
		TranslateMRShaderParameter(paramsPBlock, _T("vector_type"), vdmIsHDR ? 0 : 1);
		TranslateMRShaderParameter(paramsPBlock, _T("space_type"), vdmSpaceType);

		valid &= localValid;
	}
	else {
		// This shouldn't happen
		DbgAssert(false);
	}
}

void VDM::GetAdditionalReferenceDependencies(AdditionalDependencyTable& refTargets) {
	// None
}

bool VDM::HasRequirement(Requirement requirement, TimeValue t, Interval& valid, void* arg) {

	switch(requirement) {
		case kReq_TangentBasisVectors:
			if(pblock != NULL) {
				Update(t, valid);
				// HACK FIXME: There's a difference between the #define's of the mental ray and MAX shaders
				// I don't know why... but that means I have to consider both tangent and tangent local.
				if(vdmSpaceType == VDM_ST_TANGENT) {
					int* mapChannel = static_cast<int*>(arg);
					*mapChannel = vdmTex?vdmTex->GetMapChannel():1;
					return true;
				}
			}
			else {
				DbgAssert(false);
			}
			break;
		default:
			break;
	}

	return false;
}

BaseInterface* VDM::GetInterface(Interface_ID id) {

	if(id == IMRSHADERTRANSLATION_INTERFACE_ID) {
		return static_cast<imrShaderTranslation*>(this);
	}
	else {
		return Texmap::GetInterface(id);
	}
}
