/**********************************************************************
 *<
	FILE: stdmtl2.h

	DESCRIPTION:

	CREATED BY: Dan Silva modified for shiva by Kells Elmquist

	HISTORY: modified for shiva by Kells Elmquist
	         modified to use ParamBlock2, John Wainwright 11/16/98

 *>	Copyright (c) 1998, All Rights Reserved.
 **********************************************************************/

#ifndef __STDMTL2__H
#define __STDMTL2__H

//JH 5/24/03 Adding support for global supersampler
#define GLOBAL_SUPERSAMPLER

#include "shaders.h"
#include "samplers.h"
#include "iparamm2.h"
#include "texmaps.h"
#include "expmtlControl.h"

#include "stdmtl2helper.h"
#include "../../include/Graphics/ITextureDisplay.h"
#include "../../include/Graphics/ISimpleMaterial.h"
#include <IColorCorrectionMgr.h>

#ifndef NO_ASHLI // MSW 5/14/2004
#include "DxStdMtl2.h"
#include "IViewportManager.h"
#include "StdMaterialViewportShading.h"
#include <IRTShaderParameterBinding.h>
#endif


// StdMtl2 flags values
#ifndef USE_LIMITED_STDMTL // orb 01-14-2002
#define STDMTL_ADD_TRANSP   (1<<0)
#endif // USE_LIMITED_STDMTL

#define STDMTL_FALLOFF_OUT  (1<<1)
#define STDMTL_WIRE		  	(1<<2)
#define STDMTL_2SIDE		(1<<3)
#define STDMTL_SOFTEN       (1<<4)
#define STDMTL_FILT_TRANSP 	(1<<5)
#define STDMTL_WIRE_UNITS	(1<<6)
#define STDMTL_LOCK_AD      (1<<8)
#define STDMTL_LOCK_DS      (1<<9)
#define STDMTL_UNUSED1		(1<<10)
#define STDMTL_LOCK_ADTEX   (1<<11)
#define STDMTL_FACEMAP		(1<<12)
#define STDMTL_OLDSPEC      (1<<13)
#define STDMTL_SSAMP_ON		(1<<14)
#define STDMTL_COLOR_SI		(1<<15)
#define STDMTL_FACETED		(1<<16)

#define STDMTL_ROLLUP0_OPEN  (1<<27)	// shader
#define STDMTL_ROLLUP1_OPEN  (1<<28)	// basic
#define STDMTL_ROLLUP2_OPEN  (1<<29)	// extra
#define STDMTL_ROLLUP3_OPEN  (1<<30)	// maps
#define STDMTL_ROLLUP4_OPEN  (1<<26)	// sampling
#define STDMTL_ROLLUP5_OPEN  (1<<25)	// dynamics
#define STDMTL_ROLLUP6_OPEN  (1<<24)	// effects

// extended standard id's for translucency
#define ID_TL		12		// translucent color
#define ID_TLB		13		// translucent blur

// only needed if the constant shader is included in shaders
#define  CONSTClassID (STDSHADERS_CLASS_ID+1)

#define STDMTL_ROLLUP_FLAGS (STDMTL_ROLLUP0_OPEN|STDMTL_ROLLUP1_OPEN|STDMTL_ROLLUP2_OPEN|STDMTL_ROLLUP3_OPEN \
							|STDMTL_ROLLUP4_OPEN|STDMTL_ROLLUP5_OPEN|STDMTL_ROLLUP6_OPEN)

class StdMtl2Dlg;


#define NUM_REFS		9

// refs
#define OLD_PBLOCK_REF	0		// reference number assignments
#define TEXMAPS_REF		1
#define SHADER_REF		2
#define SHADER_PB_REF	3
#define EXTENDED_PB_REF	4
#define SAMPLING_PB_REF	5
#define MAPS_PB_REF		6
#define DYNMAICS_PB_REF	7
#define SAMPLER_REF		8

// sub anims
#if !defined(NO_OUTPUTRENDERER)	&& !defined(USE_LIMITED_STDMTL)	// russom - 04/19/01
#if !defined( NO_MTL_DYNAMICS_PARAMETERS )
  #define NUM_SUB_ANIMS	5
#else
  #define NUM_SUB_ANIMS	4 // aszabo|Sep.28.01
#endif // !NO_MTL_DYNAMICS_PARAMETERS
#else
  #define NUM_SUB_ANIMS	3
#endif	// NO_OUTPUTRENDERER

//#define OLD_PARAMS_SUB		0
#define TEXMAPS_SUB			0
#define SHADER_SUB			1
#define EXTRA_PB_SUB		2
#if  !defined(NO_OUTPUTRENDERER) && !defined(USE_LIMITED_STDMTL)	// russom - 04/19/01
  #define SAMPLING_PB_SUB		3
#if !defined( NO_MTL_DYNAMICS_PARAMETERS ) // aszabo|Sep.28.01
  #define DYNAMICS_PB_SUB		4
#endif // !NO_MTL_DYNAMICS_PARAMETERS
#endif	// NO_OUTPUTRENDERER

// these define the evalType parameter for the private
// EvalReflection & EvalRefraction calls
#define EVAL_CHANNEL	0
#define RAY_QUERY		1

#ifndef NO_ASHLI // MSW 5/14/2004
#include <notify.h>
static void ShaderUpdate(void *param, NotifyInfo *info);
#endif 

class RefmsgKillCounter {
private:
	friend class KillRefmsg;
	LONG	counter;

public:
	RefmsgKillCounter() : counter(-1) {}

	bool DistributeRefmsg() { return counter < 0; }
};

class KillRefmsg {
private:
	LONG&	counter;

public:
	KillRefmsg(RefmsgKillCounter& c) : counter(c.counter) { ++counter; }
	~KillRefmsg() { --counter; }
};

class StdMtl2Notification
	: public INewSubTexmapNotification
{
public:
	StdMtl2Notification () 	{};

		// From INewSubTexmapNotification
		void NewSubTexmapAdded(int i, Texmap *m);

};

class StdMtl2 : public ExposureMaterialControlImp<StdMtl2,
							AddExposureMaterialControl<StdMat2> >
                , public IReshading
#ifndef NO_ASHLI // MSW 5/14/2004
                , public IDX9DataBridge
                , public StdMaterialViewportShading
				, public IRTShaderParameterBinding 
				, public MaxSDK::Graphics::ITextureDisplay
#endif
{
	typedef ExposureMaterialControlImp<StdMtl2,
		AddExposureMaterialControl<StdMat2> > BaseClass;

	// Animatable parameters
	public:
		// current UI if open
		static ShaderParamDlg* pShaderDlg;
		static IAutoMParamDlg* masterDlg;
		static IAutoMParamDlg* texmapDlg;
		static IAutoMParamDlg* extendedDlg;
		static IAutoMParamDlg* samplingDlg;
		static HWND			   curHwmEdit;
		static IMtlParams*	   curImp;
		static Tab<ClassDesc*> shaderList;
		static Tab<ClassDesc*> samplerList;

		IParamBlock *old_pblock;    // ref 0, for old version loading
		Texmaps* maps;				// ref 1
		Interval ivalid;
		ReshadeRequirements mReshadeRQ; // mjm - 06.02.00
		ReshadeRequirements mInRQ;		// ca - 12/7/00
		ULONG flags;
		int shaderId;
		Shader *pShader;			// ref 2
		// new PB2 paramblocks, one per rollout
		IParamBlock2 *pb_shader;	// ref 3, 4, ...
		IParamBlock2 *pb_extended;	
		IParamBlock2 *pb_sampling;	
		IParamBlock2 *pb_maps;	
		IParamBlock2 *pb_dynamics;	

#ifndef NO_ASHLI // MSW 5/14/2004
		DxStdMtl2 * dxStdMtl2;
#endif
		Color filter;
		float opacity;	
		float opfall;
		float wireSize;
		float ioRefract;
#ifndef USE_LIMITED_STDMTL // orb 01-14-2002
		float dimIntens;
		float dimMult;
		BOOL dimReflect;
#endif

        Color translucentColor;
        float translucentBlur;

		// sampling 
		int samplerId;
		Sampler* pixelSampler;	// ref 8

		// composite of shader/mtl channel types
		int channelTypes[ STD2_NMAX_TEXMAPS ];
		int stdIDToChannel[ N_ID_CHANNELS ];

		// experiment: override filter, >>>>>>>>>> remove this next api change
//		BOOL	filterOverrideOn;
//		float	filterSz;

		// Kill REFMSG_CHANGE messages. This counter is used to
		// prevent these messages when things really aren't changing.
		// Use the class KillRefmsg
		RefmsgKillCounter	killRefmsg;

		static ExposureMaterialControlDesc msExpMtlControlDesc;

		void SetFlag(ULONG f, ULONG val);
		void EnableMap(int i, BOOL onoff);
		BOOL IsMapEnabled(int i) { return (*maps)[i].mapOn; }
		BOOL KeyAtTimeByID(ParamID id,TimeValue t) { return (id == OPACITY_PARAM) ? pb_extended->KeyFrameAtTimeByID(std2_opacity, t) : FALSE; }
		BOOL AmtKeyAtTime(int i, TimeValue t);
		int  GetMapState( int indx ); //returns 0 = no map, 1 = disable, 2 = mapon
		TSTR  GetMapName( int indx ); 
		void SyncADTexLock( BOOL lockOn );

		// from StdMat
		// these set Approximate colors into the plug in shader
		BOOL IsSelfIllumColorOn();
		void SetSelfIllumColorOn( BOOL on );
		void SetSelfIllumColor(Color c, TimeValue t);		
		void SetAmbient(Color c, TimeValue t);		
		void SetDiffuse(Color c, TimeValue t);		
		void SetSpecular(Color c, TimeValue t);
		void SetShininess(float v, TimeValue t);		
		void SetShinStr(float v, TimeValue t);		
		void SetSelfIllum(float v, TimeValue t);	
		void SetSoften(BOOL onoff) { SetFlag(STDMTL_SOFTEN,onoff); }
		
		void SetTexmapAmt(int imap, float amt, TimeValue t);
		void LockAmbDiffTex(BOOL onoff) { SetFlag(STDMTL_LOCK_ADTEX,onoff); }

		void SetWire(BOOL onoff){ pb_shader->SetValue(std2_wire,0, (onoff!=0) ); }//SetFlag(STDMTL_WIRE,onoff); }
		void SetWireSize(float s, TimeValue t);
#ifndef NO_OUTPUTRENDERER	// russom - 08/03/01
		void SetWireUnits(BOOL onoff) { pb_extended->SetValue(std2_wire_units,0, (onoff!=0) ); } //SetFlag(STDMTL_WIRE_UNITS,onoff); }
#else
		void SetWireUnits(BOOL onoff) { return; };
#endif
		
		void SetFaceMap(BOOL onoff) { pb_shader->SetValue(std2_face_map,0, (onoff!=0) ); } //SetFlag(STDMTL_FACEMAP,onoff); }
		void SetTwoSided(BOOL onoff) { pb_shader->SetValue(std2_two_sided,0, (onoff!=0) ); } //SetFlag(STDMTL_2SIDE,onoff); }
		void SetFalloffOut(BOOL outOn) { pb_extended->SetValue(std2_falloff_type,0, (outOn!=0) ); } //SetFlag(STDMTL_FALLOFF_OUT,onoff); }

		void SetTransparencyType(int type);

		void SetFilter(Color c, TimeValue t);
		void SetOpacity(float v, TimeValue t);		
		void SetOpacFalloff(float v, TimeValue t);		
		void SetIOR(float v, TimeValue t);

#ifndef USE_LIMITED_STDMTL // orb 01-14-2002
		void SetDimIntens(float v, TimeValue t);
		void SetDimMult(float v, TimeValue t);
#endif // USE_LIMITED_STDMTL
		
        // translucency
	void SetTranslucentColor(Color& c, TimeValue t);
        void SetTranslucentBlur(float v, TimeValue t);
		
	    int GetFlag(ULONG f) { return (flags&f)?1:0; }

		// >>>Shaders

		// these 3 internal only
		void SetShaderIndx( long shaderId, BOOL update=TRUE );
		long GetShaderIndx(){ return shaderId; }
		void SetShader( Shader* pNewShader );
		void ShuffleTexMaps( Shader* newShader, Shader* oldShader );
		void ShuffleShaderParams( Shader* newShader, Shader* oldShader );

		Shader* GetShader(){ return pShader; }
		void SwitchShader(Shader* pNewShader, BOOL loadDlg = FALSE);
		void SwitchShader(ClassDesc* pNewCD);
		BOOL SwitchShader(Class_ID shaderId);
		int FindShader( Class_ID& findId, ClassDesc** ppCD=NULL );
		BOOL IsShaderInUI() { return pb_shader && pb_shader->GetMap() && pShader && pShader->GetParamDlg(); }

		static void StdMtl2::LoadShaderList();
		static int StdMtl2::NumShaders();
		static ClassDesc* StdMtl2::GetShaderCD(int i);
		static void StdMtl2::LoadSamplerList();
		static int StdMtl2::NumSamplers();
		static ClassDesc* StdMtl2::GetSamplerCD(int i);

		BOOL IsFaceted(){ return GetFlag(STDMTL_FACETED); }
		void SetFaceted( BOOL on ){	pb_shader->SetValue(std2_faceted,0, (on!=0) ); }

		// These utilitys provide R2.5 shaders, ONLY used for Translators
		// Does not & will not work for plug-in shaders
		void SetShading(int s);
		int GetShading();

		// from Mtl
		Color GetAmbient(int mtlNum=0, BOOL backFace=FALSE);		
	    Color GetDiffuse(int mtlNum=0, BOOL backFace=FALSE);		
		Color GetSpecular(int mtlNum=0, BOOL backFace=FALSE);
		float GetShininess(int mtlNum=0, BOOL backFace=FALSE);	
		float GetShinStr(int mtlNum=0, BOOL backFace=FALSE) ;
		float GetXParency(int mtlNum=0, BOOL backFace=FALSE);
		float WireSize(int mtlNum=0, BOOL backFace=FALSE) { return wireSize; }
		BOOL GetTransparencyHint(TimeValue t, Interval& valid);

		// >>>> Self Illumination 
		float GetSelfIllum(int mtlNum, BOOL backFace) ;
		BOOL  GetSelfIllumColorOn(int mtlNum, BOOL backFace);
		Color GetSelfIllumColor(int mtlNum, BOOL backFace);
		
		// >>>> sampling
#ifdef GLOBAL_SUPERSAMPLER
		void SetGlobalSamplingOn( BOOL on )
		{	
		#if !defined(NO_OUTPUTRENDERER) && !defined(USE_LIMITED_STDMTL)		// russom - 04/19/01 - remove SuperSampling
			pb_sampling->SetValue(std2_ssampler_useglobal, 0, on!=0 );
		#endif
		}	
		BOOL GetGlobalSamplingOn()
		{	
		#if !defined(NO_OUTPUTRENDERER) && !defined(USE_LIMITED_STDMTL)		// russom - 04/19/01 - remove SuperSampling
			Interval iv; 
			BOOL on;
			pb_sampling->GetValue(std2_ssampler_useglobal, 0, on, iv );
			return on;
		#else
			return FALSE;
		#endif
		}	
#endif //GLOBAL_SUPERSAMPLER
		void SetSamplingOn( BOOL on )
		{	
		#if !defined(NO_OUTPUTRENDERER) && !defined(USE_LIMITED_STDMTL)		// russom - 04/19/01 - remove SuperSampling
			pb_sampling->SetValue(std2_ssampler_enable, 0, on!=0 );
		#endif
		}	
		BOOL GetSamplingOn()
		{	
		#if !defined(NO_OUTPUTRENDERER) && !defined(USE_LIMITED_STDMTL)		// russom - 04/19/01 - remove SuperSampling
			Interval iv; 
			BOOL on;
			pb_sampling->GetValue(std2_ssampler_enable, 0, on, iv );
			return on;
		#else
			return FALSE;
		#endif
		}	
		void SetSamplingQuality( float quality )
		{	 
		#if !defined(NO_OUTPUTRENDERER) && !defined(USE_LIMITED_STDMTL)		// russom - 04/19/01 - remove SuperSampling
			pb_sampling->SetValue(std2_ssampler_qual, 0, quality );
		#endif
		}	
		float GetSamplingQuality()
		{	
		#if !defined(NO_OUTPUTRENDERER) && !defined(USE_LIMITED_STDMTL)		// russom - 04/19/01 - remove SuperSampling
			Interval iv; 
			float q;
			pb_sampling->GetValue(std2_ssampler_qual, 0, q, iv );
			return q;
		#else
			return 0.0f;
		#endif
		}

		void SwitchSampler(ClassDesc* pNewCD);
		void SwitchSampler(Sampler* pNewSampler);
		BOOL SwitchSampler(Class_ID samplerId);
		static int FindSampler( Class_ID findId, ClassDesc** pNewCD=NULL );
		Sampler * GetPixelSampler(int mtlNum=0, BOOL backFace=FALSE){  return pixelSampler; }	

		// these 2 internal only
		void SetSamplerIndx( long indx, BOOL update=TRUE );
		long  GetSamplerIndx(){ return samplerId; }
		void SetPixelSampler( Sampler * sampler );

		// from StdMat
		BOOL GetSoften() { return GetFlag(STDMTL_SOFTEN); }
		BOOL GetFaceMap() { return GetFlag(STDMTL_FACEMAP); }
		BOOL GetTwoSided() { return GetFlag(STDMTL_2SIDE); }
		BOOL GetWire() { return GetFlag(STDMTL_WIRE); }
		BOOL GetWireUnits() { return GetFlag(STDMTL_WIRE_UNITS); }
		BOOL GetFalloffOut() { return GetFlag(STDMTL_FALLOFF_OUT); }  // 1: out, 0: in
		BOOL GetAmbDiffTexLock(){ return GetFlag(STDMTL_LOCK_ADTEX); } 
		int GetTransparencyType() {
#ifndef USE_LIMITED_STDMTL // orb 01-14-2002
			return (flags&STDMTL_FILT_TRANSP)?TRANSP_FILTER:
				flags&STDMTL_ADD_TRANSP ? TRANSP_ADDITIVE: TRANSP_SUBTRACTIVE;
#else
			return TRANSP_FILTER;
#endif // USE_LIMITED_STDMTL

			}
		Color GetFilter(TimeValue t);

		// these are stubs till i figure out scripting
		Color GetAmbient(TimeValue t);		
		Color GetDiffuse(TimeValue t);		
		Color GetSpecular(TimeValue t);
		float GetShininess( TimeValue t);		
		float GetShinStr(TimeValue t);	
		float GetSelfIllum(TimeValue t);
		BOOL  GetSelfIllumColorOn();
		Color GetSelfIllumColor(TimeValue t); 

		float GetOpacity( TimeValue t);		
		float GetOpacFalloff(TimeValue t);		
		float GetWireSize(TimeValue t);
		float GetIOR( TimeValue t);

#ifndef USE_LIMITED_STDMTL // orb 01-14-2002
		float GetDimIntens( TimeValue t);
		float GetDimMult( TimeValue t);
#endif // USE_LIMITED_STDMTL

		float GetSoftenLevel( TimeValue t);
		BOOL MapEnabled(int i);
		float GetTexmapAmt(int imap, TimeValue t);

		// internal
		float GetOpacity() { return opacity; }		
		float GetOpacFalloff() { return opfall; }		
		float GetTexmapAmt(int imap);
		Color GetFilter();
		float GetIOR() { return ioRefract; }

      
        // translucency
	Color GetTranslucentColor(TimeValue t);
        float GetTranslucentBlur(TimeValue t);

		StdMtl2(BOOL loading = FALSE);
		~StdMtl2() {
			DiscardTexHandles();
#ifndef NO_ASHLI // MSW 5/14/2004
			if(dxStdMtl2)
			{
				delete dxStdMtl2;
			}
			dxStdMtl2 = NULL;
			UnRegisterNotification(ShaderUpdate,this,NOTIFY_HW_TEXTURE_CHANGED);
#endif
			}
		BOOL ParamWndProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
		ParamDlg* CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp);
		BOOL SetDlgThing(ParamDlg* dlg);
		void UpdateTexmaps();
		void UpdateMapButtons();
		void UpdateExtendedMapButtons();
		void UpdateMtlDisplay();
		void UpdateLockADTex( BOOL passOn );
		void UpdateExtraParams( ULONG stdParams );
		void UpdateSamplingParams();
#ifdef GLOBAL_SUPERSAMPLER
		void EnableLocalSamplerControls();
#endif //GLOBAL_SUPERSAMPLER


		Color TranspColor(ShadeContext& sc, float opac, Color& diff);
		void Shade(ShadeContext& sc);
		float EvalDisplacement(ShadeContext& sc); 
		Interval DisplacementValidity(TimeValue t); 
		void Update(TimeValue t, Interval& validr);
		void Reset();
		void OldVerFix(int loadVer);
		void BumpFix();
		Interval Validity(TimeValue t);
		void NotifyChanged();

		// Requirements
		int BuildMaps(TimeValue t, RenderMapsContext &rmc);
		ULONG LocalRequirements(int subMtlNum);
		ULONG Requirements(int subMtlNum);
		void MappingsRequired(int subMtlNum, BitArray & mapreq, BitArray &bumpreq);

		// Methods to access texture maps of material
		int NumSubTexmaps() { return STD2_NMAX_TEXMAPS; }
		Texmap* GetSubTexmap(int i) { return (*maps)[i].map; }
		int MapSlotType(int i);
		void SetSubTexmap(int i, Texmap *m);
		TSTR GetSubTexmapSlotName(int i);
		int SubTexmapOn(int i) { return  MAPACTIVE(i); } 
		long StdIDToChannel( long id ){ return stdIDToChannel[id]; }


		Class_ID ClassID();
		SClass_ID SuperClassID() { return MATERIAL_CLASS_ID; }
		void GetClassName(TSTR& s) { s = GetString(IDS_KE_STANDARD2); }  

		void DeleteThis();

		int NumSubs() { return NUM_SUB_ANIMS; }  
	    Animatable* SubAnim(int i);
		TSTR SubAnimName(int i);
		int SubNumToRefNum(int subNum);

		// JBW: add direct ParamBlock access
		// CA - 3/18/04 - BUG 556450 - Don't include the dynamics parameter block if it has a NULL descriptor
		int	NumParamBlocks() { return (pb_dynamics == NULL || pb_dynamics->GetDesc() != NULL) ? 5 : 4; }
		IParamBlock2* GetParamBlock(int i);
		IParamBlock2* GetParamBlockByID(BlockID id);

		// From ref
 		int NumRefs() { return NUM_REFS; }
		RefTargetHandle GetReference(int i);
		void SetReference(int i, RefTargetHandle rtarg);

		RefTargetHandle Clone(RemapDir &remap);
		RefResult NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, 
		   PartID& partID, RefMessage message );

		// IO
		IOResult Save(ISave *isave);
		IOResult Load(ILoad *iload);

// begin - ke/mjm - 03.16.00 - merge reshading code
		BOOL SupportsRenderElements(){ return TRUE; }
//		BOOL SupportsReShading(ShadeContext& sc);
		void UpdateReshadeRequirements(RefTargetHandle hTarget, PartID partID); // mjm - 06.02.00
		ReshadeRequirements GetReshadeRequirements() { return mReshadeRQ; } // mjm - 06.02.00
		void PreShade(ShadeContext& sc, IReshadeFragment* pFrag);
		void PostShade(ShadeContext& sc, IReshadeFragment* pFrag, int& nextTexIndex, IllumParams* ip);
// end - ke/mjm - 03.16.00 - merge reshading code

		// The traditional viewport and XBR viewport may exists in the same time, so we need to add
		// the following 2 members to hold the texture info for XBR viewport.
		TexHandle *mpXBRTexHandle[MaxSDK::Graphics::ISimpleMaterial::MaxUsage];
		Interval mXBRTexValidInterval;
		IColorCorrectionMgr::CorrectionMode mColorCorrectionMode;
// begin - dds- 04.27.00 - multiple map display support
#define NTEXHANDLES 7 //diffuse, opacity, normal, specular color, level, gloss, emissive
		TexHandle *texHandle[NTEXHANDLES];
		short useSubForTex[NTEXHANDLES];
		short texOpsType[NTEXHANDLES];
		MapUsageType	mapsUsage[NTEXHANDLES]; // indicate the usage of maps.
		DWORD borderColor[NTEXHANDLES];
		int numTexHandlesUsed;
		Interval texHandleValid;
		Interval texHandleValidArray[NTEXHANDLES];
		void SetTexOps(Material *mtl, int i, int type);
		void SetHWTexOps(IHardwareMaterial3 *pIHWMat2, int ntx, int type);
		void DiscardTexHandles();
		void DiscardTexHandle(MapUsageType usage);
		void DiscardTexHandle(Texmap* texmap);
		BOOL SupportTexDisplay() { return TRUE; }
		BOOL SupportsMultiMapsInViewport() { return TRUE; }
		void ActivateTexDisplay(BOOL onoff);
		void SetupGfxMultiMaps(TimeValue t, Material *mtl, MtlMakerCallback &cb);
// end - dds- 04.27.00 - multiple map display support

		// --- Material evaluation - from Mtl ---
		bool IsOutputConst( ShadeContext& sc, int stdID );
		bool EvalColorStdChannel( ShadeContext& sc, int stdID, Color& outClr );
		bool EvalMonoStdChannel( ShadeContext& sc, int stdID, float& outVal );

		void* GetInterface(ULONG id);

#ifndef USE_LIMITED_STDMTL // orb 01-14-2002
		float GetReflectionDim(float diffIllumIntensity ){
			if (dimReflect)
				return ((1.0f-dimIntens)*diffIllumIntensity*dimMult + dimIntens);
			else 
				return 1.0f;
		}
#endif
		Color TranspColor( float opac, Color filt, Color diff );

		float GetEffOpacity(ShadeContext& sc, float opac){
			if ( opac != 1.0f || opfall != 0.0f) {
				if (opfall != 0.0f) {	
					Point3 N = (flags & STDMTL_FACETED) ? sc.GNormal() : sc.Normal();
					float d = (float)fabs( DotProd( N, sc.V() ) );
					if (flags & STDMTL_FALLOFF_OUT) d = 1.0f-d;
					return opac * (1.0f - opfall * d);
				} else return opac;
			} else return 1.0f;
		}

		virtual void SetNoExposure(BOOL on) {
			ExposureMaterialControl::SetNoExposure(on);
			ivalid.SetEmpty();
			NotifyChanged(); }
		virtual void SetInvertSelfIllum(BOOL on) {
			ExposureMaterialControl::SetInvertSelfIllum(on);
			ivalid.SetEmpty();
			NotifyChanged(); }
		virtual void SetInvertReflect(BOOL on) {
			ExposureMaterialControl::SetInvertReflect(on);
			ivalid.SetEmpty();
			NotifyChanged(); }
		virtual void SetInvertRefract(BOOL on) {
			ExposureMaterialControl::SetInvertRefract(on);
			ivalid.SetEmpty();
			NotifyChanged(); }

#ifndef NO_ASHLI // MSW 5/14/2004
		//from IDX9DataBridge
		ParamDlg * CreateEffectDlg(HWND hWnd, IMtlParams * imp) {return NULL;}
		void DisableUI(){};
		void SetDXData(IHardwareMaterial * pHWMtl, Mtl * pMtl){};
		const TCHAR * GetName(){return _T("DxStdMat");}
		float GetDXVersion(){return 9.0f;}

		//!IRTShaderParameterBinding.h
		void BindParameter(const TCHAR * paramName, LPVOID value);
		void BindParameter(const TimeValue t, const MCHAR * paramName, LPVOID value);
		void InitializeBinding(){};
#endif
		BaseInterface *GetInterface(Interface_ID id);

		// -- From ITextureDisplay
		virtual void SetupTextures(TimeValue t, MaxSDK::Graphics::DisplayTextureHelper &cb);
		
	private:
		static StdMtl2Notification mMapNotification;

		// --- Material evaluation ---
		bool EvalChannel( ShadeContext& sc, int channelID, Color& outClr);
		bool EvalBump( ShadeContext& sc, Color& outClr );
		bool EvalReflection( ShadeContext& sc, Color& outClr, int evalType = EVAL_CHANNEL );
		bool EvalReflection( ShadeContext& sc, float& outVal, int evalType = EVAL_CHANNEL );
		bool EvalRefraction( ShadeContext& sc, Color& outClr, int evalType = EVAL_CHANNEL );
		bool EvalRefraction( ShadeContext& sc, float& outVal, int evalType = EVAL_CHANNEL );
		bool EvalDisplacement( ShadeContext& sc, float& outVal );
		bool ShouldEvalSubTexmap( ShadeContext& sc, int id );

		void SetupGfxMultiMapsForHW(TimeValue t, Material *mtl, MtlMakerCallback &cb, IHardwareMaterial *pIHWMat, BOOL bShader);
	protected:
		void UpdateBorderColor(BOOL bShader);
		void SetBorderColor(int stage, IHardwareMaterial3* pIHWMat3);
	};

Mtl* CreateStdMtl2();

#endif
