/**********************************************************************
 *<
	FILE: vdm.h

	DESCRIPTION:	Includes for Plugins

	CREATED BY: 

	HISTORY:

 *>	Copyright (c) 2010, All Rights Reserved.
 **********************************************************************/

#ifndef __VDM_H
#define __VDM_H

#include "normalcommon.h"


//===========================================================================
//
// Class VDM
//
//===========================================================================

#define PBLOCK_REF	0

class VDM : 
	public Texmap, 
	private imrShaderTranslation 
{
	public:
		VDM();

		void				Reset();
		void				Init();
		void				InvalidateUI();

		inline void			Reorder(AColor &s);
		inline void			Reorder(Point3& p);


		//-- From Animatable
		void				DeleteThis()			{ delete this; }
		void				GetClassName(TSTR& s);
		Class_ID			ClassID()				{ return VDM_CLASS_ID;}		
		SClass_ID			SuperClassID()			{ return TEXMAP_CLASS_ID; }

		int					NumSubs()				{ return 2; }
		Animatable*			SubAnim(int i); 
		TSTR				SubAnimName(int i);
		int					SubNumToRefNum(int subNum) { return subNum; }

		int					RenderBegin(TimeValue t, ULONG flags=0);
		int					RenderEnd(TimeValue t);

		int					NumParamBlocks()		{ return 1; } // return number of ParamBlocks in this instance
		IParamBlock2*		GetParamBlock(int i)	{ return pblock; } // return i'th ParamBlock
		IParamBlock2*		GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; } // return id'd ParamBlock

		//-- From ReferenceMaker
		IOResult			Load(ILoad *iload);
		IOResult			Save(ISave *isave);

		int					NumRefs()				{ return 2; }
		RefTargetHandle		GetReference(int i);
		void				SetReference(int i, RefTargetHandle rtarg);

		RefResult			NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
								PartID& partID,  RefMessage message);

		//-- From ReferenceTarget
		RefTargetHandle		Clone( RemapDir &remap );

		//-- From ISubMap
		int					NumSubTexmaps()			{ return 1; }
		Texmap*				GetSubTexmap(int i)		{ return vdmTex; }
		void				SetSubTexmap(int i, Texmap *m);
		TSTR				GetSubTexmapSlotName(int i);

		//-- From MtlBase
		ULONG				LocalRequirements(int subMtlNum);
		void				LocalMappingsRequired(int subMtlNum, BitArray & mapreq, BitArray &bumpreq);

		void				Update(TimeValue t, Interval& valid);
		Interval			Validity(TimeValue t)	{Interval v; Update(t,v); return ivalid;}
		ParamDlg*			CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp);

		BOOL				SupportTexDisplay()		{ return FALSE; }
		DWORD_PTR		GetActiveTexHandle(TimeValue t, TexHandleMaker& thmaker);
		void				ActivateTexDisplay(BOOL onoff);

		//-- From Texmap
		AColor				EvalColor(ShadeContext& sc);
		float				EvalMono(ShadeContext& sc);
		Point3				EvalNormalPerturb(ShadeContext& sc)	{ return Point3(0,0,0); }

		//-- Mental Ray support
	private:
		// -- from InterfaceServer
		BaseInterface*		GetInterface(Interface_ID id);
		// -- from imrShaderTranslation
		imrShader*			GetMRShader(imrShaderCreation& shaderCreation);
		void				ReleaseMRShader();
		bool				NeedsAutomaticParamBlock2Translation();
		void				TranslateParameters(imrTranslation& translationInterface, imrShader* shader, TimeValue t, Interval& valid);
		void				GetAdditionalReferenceDependencies(AdditionalDependencyTable& refTargets);
		virtual bool HasRequirement(Requirement, TimeValue t, Interval& valid, void* arg);

	protected:
		//user
		float				vdmMult;
		bool				vdmIsHDR;
		bool				vdmIsMapEnabled;
		enum VDMSpaceType
		{
			VDM_ST_WORLD = 0,
			VDM_ST_OBJECT = 1,
			VDM_ST_TANGENT = 2
		} vdmSpaceType;


		// Parameter block
		IParamBlock2		*pblock;	//ref 0
		Texmap				*vdmTex;	//the VDM texture
		Interval			ivalid;
};



#endif // __VDM_H