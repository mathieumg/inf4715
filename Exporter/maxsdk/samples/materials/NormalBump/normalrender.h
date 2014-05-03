/**********************************************************************
 *<
	FILE: normalrender.h

	DESCRIPTION:	Includes for Plugins

	CREATED BY: Ben Lipman

	HISTORY:

 *>	Copyright (c) 2003, All Rights Reserved.
 **********************************************************************/

#ifndef __NORMALRENDER__H
#define __NORMALRENDER__H

#include "normalcommon.h"

//===========================================================================
//
// Class Gnormal
//
//===========================================================================

#define NSUBTEX		2 
#define PBLOCK_REF	0

#define PARAM2_CHUNK 0x2030

// Values for Gnormal::method
#define TANGENTUV_SPACE		0
#define TANGENTLOCAL_SPACE	4
#define SCREEN_SPACE		2
#define WORLDXYZ_SPACE		3
#define LOCALXYZ_SPACE		1


class Gnormal : 
	public Texmap, 
	private imrShaderTranslation 
{
	public:
		Gnormal();
		~Gnormal();		

		void				Reset();
		void				Init();
		void				InvalidateUI();

		inline void			Reorder(AColor &s);
		inline void			Reorder(Point3& p);


		//-- From Animatable
		void				DeleteThis()			{ delete this; }
		void				GetClassName(TSTR& s);
		Class_ID			ClassID()				{ return GNORMAL_CLASS_ID;}		
		SClass_ID			SuperClassID()			{ return TEXMAP_CLASS_ID; }

		int					NumSubs()				{ return 1+NSUBTEX; }
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

		int					NumRefs()				{ return 1+NSUBTEX; }
		RefTargetHandle		GetReference(int i);
private:
		virtual void				SetReference(int i, RefTargetHandle rtarg);
public:

		RefResult			NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
								PartID& partID,  RefMessage message);

		//-- From ReferenceTarget
		RefTargetHandle		Clone( RemapDir &remap );

		//-- From ISubMap
		int					NumSubTexmaps()			{ return NSUBTEX; }
		Texmap*				GetSubTexmap(int i)		{ return subTex[i]; }
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
		Point3				EvalNormalPerturb(ShadeContext& sc);

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
		float				gnMult, gnBMult;
		BOOL				flip_red, flip_green, swap_rg;
		BOOL				map1on, map2on;

		// Parameter block
		IParamBlock2		*pblock;	//ref 0
		Texmap				*subTex[NSUBTEX]; //array of sub-materials
		Interval			ivalid;
		int					method;
};

#endif // __NORMALRENDER__H
