/*===========================================================================*\
 | 
 |  FILE:   Swirl.cpp
 |       Twirly Tornado-type material
 |       Main shader and plugin file
 | 
 |  AUTH:   Harry Denholm
 |       Copyright(c) Kinetix 1998
 |       All Rights Reserved.
 |
 |  HIST:   Started 31-7-98
 | 
\*===========================================================================*/


/*===========================================================================*\
 | Includes and global/macro setup
\*===========================================================================*/

#include "swirl.h"

#include "3dsmaxport.h"
#include <iparamm2.h>
#include <macrorec.h>

#define NSUBTEX 2
#define NCOLS 2

#define UVGEN_REF	0
#define PBLOCK_REF	1
#define MAP1_REF	2
#define MAP2_REF	3

#define NUM_REFS	4
#define RANDOM( a ) ( ( (float)rand()/(float)RAND_MAX)*(a) )

static Class_ID SwirlClassID(0x72c8577f, 0x39a00a1b);
extern HINSTANCE hInstance;

/*===========================================================================*\
 | Material sampler
\*===========================================================================*/
class Swirl;

class SwirlSampler: public MapSampler {
   // Pointer to main texture class
   Swirl *Swirler;
   public:
      SwirlSampler() { Swirler= NULL; }
      SwirlSampler(Swirl *c) { Swirler= c; }
      void Set(Swirl *c) { Swirler = c; }
      AColor Sample(ShadeContext& sc, float u,float v);
      AColor SampleFilter(ShadeContext& sc, float u,float v, float du, float dv);
      float SampleMono(ShadeContext& sc, float u,float v);
      float SampleMonoFilter(ShadeContext& sc, float u,float v, float du, float dv);
   } ;


/*===========================================================================*\
 | Main texture class -- Swirl
\*===========================================================================*/

class Swirl: public Texmap { 
   friend class SwirlPBAccessor;
   friend class SwirlPB1ToPB2PostLoad;
   friend class SwirlDlgProc;

   AColor col[NCOLS];
   float hs,vs,cv,hg,vg,ls,rs;
   int h,lock;
   
   // map enabled flag
   BOOL mapOn[2];

   UVGen *uvGen;
   static ParamDlg* uvGenDlg;	
   IParamBlock2 *pblock;
   Texmap* subTex[NSUBTEX];
   TexHandle *texHandle;
   Interval texHandleValid;
   Interval ivalid;
   int rollScroll;
   SwirlSampler mysamp;

   public:
      Swirl();
      ~Swirl() {
         DiscardTexHandle();
         }  
      ParamDlg* CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp);
      void Update(TimeValue t, Interval& valid);
      void Init();
      void Reset();
      Interval Validity(TimeValue t) { Interval v; Update(t,v); return ivalid; }

      void SetColor(int i, Color c, TimeValue t);

      void SetHS(float f, TimeValue t);
      void SetVS(float f, TimeValue t);
      void SetCV(float f, TimeValue t);
      void SetHG(float f, TimeValue t);
      void SetVG(float f, TimeValue t);
      void SetLS(float f, TimeValue t);
      void SetLOCK(int f, TimeValue t);

      void SetH(int f, TimeValue t);
      void SetRS(float f, TimeValue t);

      AColor SwirlFunc(float u,float v,ShadeContext& sc,float du,float dv);

      void SwapInputs(); 
      Bitmap *BuildBitmap(int size);

      // Evaluate the color of map for the context.
      AColor EvalColor(ShadeContext& sc);
      float EvalMono(ShadeContext& sc);
      AColor EvalFunction(ShadeContext& sc, float u, float v, float du, float dv);
      float MonoEvalFunction(ShadeContext& sc, float u, float v, float du, float dv);
      AColor DispEvalFunc( float u, float v);

      // For Bump mapping, need a perturbation to apply to a normal.
      // Leave it up to the Texmap to determine how to do this.
      Point3 EvalNormalPerturb(ShadeContext& sc);

      // Methods for interactive display
      void DiscardTexHandle();
      BOOL SupportTexDisplay() { return TRUE; }
      void ActivateTexDisplay(BOOL onoff);
      DWORD_PTR GetActiveTexHandle(TimeValue t, TexHandleMaker& thmaker);
      void GetUVTransform(Matrix3 &uvtrans) { uvGen->GetUVTransform(uvtrans); }
      int GetTextureTiling() { return  uvGen->GetTextureTiling(); }
      int GetUVWSource() { return uvGen->GetUVWSource(); }
      int GetMapChannel () { return uvGen->GetMapChannel(); }
      UVGen *GetTheUVGen() { return uvGen; }

      // Requirements
      ULONG LocalRequirements(int subMtlNum) { return uvGen->Requirements(subMtlNum); }
      
      void LocalMappingsRequired(int subMtlNum, BitArray & mapreq, BitArray &bumpreq) {  
         uvGen->MappingsRequired(subMtlNum,mapreq,bumpreq); 
         }

      // Methods to access texture maps of material
      int NumSubTexmaps() { return NSUBTEX; }
      Texmap* GetSubTexmap(int i) { 
         return subTex[i]; 
         }
      void SetSubTexmap(int i, Texmap *m);
      TSTR GetSubTexmapSlotName(int i);
      void InitSlotType(int sType) { if (uvGen) uvGen->InitSlotType(sType); }
      int SubTexmapOn(int i) { return (mapOn[i] && subTex[i]) ? 1 : 0; } // mjm - 9.30.99

      Class_ID ClassID() { return SwirlClassID; }
      SClass_ID SuperClassID() { return TEXMAP_CLASS_ID; }
      void GetClassName(TSTR& s) { s= GetString(IDS_SWIRL); }  
      void DeleteThis() { delete this; }  

      int NumSubs() { return 2+NSUBTEX; }  
      Animatable* SubAnim(int i);
      TSTR SubAnimName(int i);
      int SubNumToRefNum(int subNum) { return subNum; }

      // From ref
      int NumRefs() { return 2+NSUBTEX; }
      RefTargetHandle GetReference(int i);
private:
      virtual void SetReference(int i, RefTargetHandle rtarg);
public:

	  // direct ParamBlock access is added
      int   NumParamBlocks() { return 1; }               // return number of ParamBlocks in this instance
      IParamBlock2* GetParamBlock(int i) { return pblock; } // return i'th ParamBlock
      IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; } // return id'd ParamBlock
      BOOL SetDlgThing(ParamDlg* dlg);

	  // support for Save To Previous
	  bool SpecifySaveReferences(ReferenceSaveManager& referenceSaveManager);

      RefTargetHandle Clone(RemapDir &remap);
      RefResult NotifyRefChanged( Interval changeInt, RefTargetHandle hTarget, 
         PartID& partID, RefMessage message );

      // IO
      IOResult Save(ISave *isave);
      IOResult Load(ILoad *iload);

      bool IsLocalOutputMeaningful( ShadeContext& sc ) { return true; }

	  void EnableDialogControls();

   };

ParamDlg* Swirl::uvGenDlg;	

#define SWIRL_VERSION 1

enum { swirl_params };

enum {
	swirl_col1 // swirl color
	, swirl_col2 // base color
	, swirl_hs // intensity
	, swirl_vs // twist
	, swirl_cv // color contrast
	, swirl_vg // center y
	, swirl_hg // center x
	, swirl_ls // amount
	, swirl_8 // not used
	, swirl_h // constant detail
	, swirl_rs // random seed
	, swirl_11 // not used
	, swirl_12 // not used
	, swirl_lock // lock BG
	, swirl_14 // not used
	, swirl_15 // not used
	, swirl_num_params_ver1
	, swirl_map1 = swirl_num_params_ver1
	, swirl_map2
	, swirl_map1_on
	, swirl_map2_on
	, swirl_coords
};


class SwirlClassDesc:public ClassDesc2 {
   public:
   int         IsPublic() { return GetAppID() != kAPP_VIZR; }
   void * Create(BOOL loading) {
		Swirl * newSwirl = new Swirl;
	   	if (newSwirl && !loading) {
			MakeAutoParamBlocks(newSwirl);	// make and intialize paramblock2
			newSwirl->Init();
		}

	   return newSwirl;
   }

   const MCHAR* ClassName()
   {
		return GetString(IDS_SWIRL);
   }

   SClass_ID      SuperClassID() { return TEXMAP_CLASS_ID; }
   Class_ID       ClassID() { return SwirlClassID; }
   const MCHAR*   Category() { return TEXMAP_CAT_2D;  }

   const MCHAR* InternalName() { return _M("Swirl"); }	// returns fixed parsable name (scripter-visible name)
   HINSTANCE HInstance() { return hInstance; }			// returns owning module handle

   };

static SwirlClassDesc swirlCD;
ClassDesc* GetSwirlDesc() { return &swirlCD;  }

class SwirlPBAccessor : public PBAccessor
{
public:
	void Set(PB2Value& val, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t);    // set from v
	void Get(PB2Value& val, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t, Interval &valid);    // get into v
};

SwirlPBAccessor swirl_accessor;

static ParamBlockDesc2 swirl_param_blk (swirl_params, _T("parameters"),  0, &swirlCD, P_AUTO_CONSTRUCT + P_AUTO_UI, PBLOCK_REF,
	// rollout
	IDD_SWIRL, IDS_SWIRLPARAMS, 0, 0, NULL,
	// params
	swirl_col1, _M("Swirl"), TYPE_FRGBA, P_ANIMATABLE, IDS_SWIRL_PARAMNAME,
		p_default, AColor(0.0f,0.1f,0.2f), 
		p_ui, TYPE_COLORSWATCH, IDC_SWIRL_COL1, 
		p_end,

	swirl_col2, _M("Base"), TYPE_FRGBA, P_ANIMATABLE, IDS_BASE,
		p_default, AColor(0.9f,0.58f,0.3f), 
		p_ui, TYPE_COLORSWATCH, IDC_SWIRL_COL2, 
		p_end,

	swirl_hs, _M("Swirl_Intensity"), TYPE_FLOAT, P_ANIMATABLE, IDS_SWIRLINTENS,
		p_default, 2.0f,
		p_range, -10.0f, 10.0f,
		p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_HS_EDIT, IDC_HS_SPIN, 0.05f,
		p_end,

	swirl_vs, _M("Twist"), TYPE_FLOAT, P_ANIMATABLE, IDS_TWIST,
		p_default, 1.0f,
		p_range, -20.0f, 20.0f,
		p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_VS_EDIT, IDC_VS_SPIN, 0.05f,
		p_end,

	swirl_cv, _M("Color_Contrast"), TYPE_FLOAT, P_ANIMATABLE, IDS_COLCONTRAST,
		p_default, 0.4f,
		p_range, 0.0f, 4.0f,
		p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_CV_EDIT, IDC_CV_SPIN, 0.01f,
		p_end,

	swirl_vg, _M("Center_Position_Y"), TYPE_FLOAT, P_ANIMATABLE, IDS_CENTERY,
		p_default, -0.5f,
		p_range, -10.0f, 10.0f,
		p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_VG_EDIT, IDC_VG_SPIN, 0.01f,
		p_accessor, &swirl_accessor,
		p_end,

	swirl_hg, _M("Center_Position_X"), TYPE_FLOAT, P_ANIMATABLE, IDS_CENTERX,
		p_default, -0.5f,
		p_range, -10.0f, 10.0f,
		p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_HG_EDIT, IDC_HG_SPIN, 0.1f,
		p_accessor, &swirl_accessor,
		p_end,

	swirl_ls, _M("Swirl_Amount"), TYPE_FLOAT, P_ANIMATABLE, IDS_SWIRLAMT,
		p_default, 1.0f,
		p_range, 0.0f, 3.0f,
		p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_LS_EDIT, IDC_LS_SPIN, 0.1f,
		p_end,

	swirl_h, _M("Constant_Detail"), TYPE_INT, P_ANIMATABLE, IDS_DETAIL, 
		p_default, 4,
		p_range, 0, 10, 
		p_ui, TYPE_SPINNER, EDITTYPE_INT, IDC_H_EDIT, IDC_H_SPIN, 1.0f, 
		p_end,

	swirl_rs, _M("Random_Seed"), TYPE_FLOAT, P_ANIMATABLE, IDS_RANDOMSEED, 
		p_default, 1.0f,
		p_range, 0.0f, 65535.0f, 
		p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_RS_EDIT, IDC_RS_SPIN, 0.10f,
		p_end,

	swirl_lock, _M("Lock_Background"), TYPE_INT, 0, IDS_LOCK, 
		p_default, 1,
		p_ui, TYPE_CHECKBUTTON, IDC_LOCK, 
		p_accessor, &swirl_accessor,
		p_end,

	swirl_map1, _M("swirlMap"), TYPE_TEXMAP, P_OWNERS_REF, IDS_SWIRL_MAP1,
		p_refno, MAP1_REF,
		p_subtexno,	0,		
		p_ui, TYPE_TEXMAPBUTTON, IDC_SWIRL_TEX1,
		p_end,

	swirl_map2, _M("baseMap"), TYPE_TEXMAP, P_OWNERS_REF, IDS_SWIRL_MAP2,
		p_refno, MAP2_REF,
		p_subtexno, 1,		
		p_ui, TYPE_TEXMAPBUTTON, IDC_SWIRL_TEX2,
		p_end,

	swirl_map1_on, _M("swirlMapEnabled"), TYPE_BOOL, 0, IDS_SWIRL_MAP1_ON,
		p_default, TRUE,
		p_ui, TYPE_SINGLECHEKBOX, IDC_CHKMAP1,
		p_end,

	swirl_map2_on, _M("baseMapEnabled"), TYPE_BOOL, 0, IDS_SWIRL_MAP2_ON,
		p_default, TRUE,
		p_ui, TYPE_SINGLECHEKBOX, IDC_CHKMAP2,
		p_end,

	swirl_coords, _T("coords"), TYPE_REFTARG, P_OWNERS_REF, IDS_DS_COORDINATES,
		p_refno, UVGEN_REF, 
		p_end,

   p_end
);


void SwirlPBAccessor::Set(PB2Value& val, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t)    // set from v
{
	Swirl * swirl = (Swirl*)owner;
	if ( swirl ) {
		switch (id)
		{

			case swirl_hg:
				swirl->hg = val.f;
				if(swirl->lock==1 && swirl->vg != val.f) {
					swirl->vg = val.f; 
					swirl_param_blk.InvalidateUI(swirl_vg);
				}
			break;

			case swirl_lock: {
				swirl->lock = val.i;
				swirl->EnableDialogControls();
				if ( swirl->lock != 1 ) {
					Interval v;
					swirl->pblock->GetValue( swirl_vg, t, swirl->vg, v ); 
				}
				swirl_param_blk.InvalidateUI(swirl_vg);
				swirl_param_blk.InvalidateUI(swirl_hg);

			}
			break;
		}
	}
}

void SwirlPBAccessor::Get(PB2Value& val, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t, Interval &valid)    // get into v
{
	Swirl * swirl = (Swirl*)owner;
	if ( swirl ) {
		switch (id)
		{

		case swirl_vg:
			if(swirl->lock==1) {
				Interval v;
				swirl->pblock->GetValue( swirl_hg, t, val.f, v );
			}
			break;
		}
	}
}

static ParamBlockDescID pbdesc[] = {
   { TYPE_RGBA, NULL, TRUE, swirl_col1 },
   { TYPE_RGBA, NULL, TRUE, swirl_col2 },
   { TYPE_FLOAT, NULL, TRUE, swirl_hs }, 
   { TYPE_FLOAT, NULL, TRUE, swirl_vs }, 
   { TYPE_FLOAT, NULL, TRUE, swirl_cv }, 
   { TYPE_FLOAT, NULL, TRUE, swirl_vg }, 
   { TYPE_FLOAT, NULL, TRUE, swirl_hg }, 
   { TYPE_FLOAT, NULL, TRUE, swirl_ls }, 
   { TYPE_FLOAT, NULL, FALSE, swirl_8 }, 
   { TYPE_INT, NULL, TRUE, swirl_h }, 
   { TYPE_FLOAT, NULL, TRUE, swirl_rs },
   { TYPE_FLOAT, NULL, FALSE, swirl_11}, 
   { TYPE_INT, NULL, FALSE, swirl_12 },
   { TYPE_INT, NULL, FALSE, swirl_lock },
   { TYPE_FLOAT, NULL, FALSE, swirl_14 }, 
   { TYPE_FLOAT, NULL, FALSE, swirl_15 }, 
   };

static ParamVersionDesc versions[] = {
	ParamVersionDesc(pbdesc,swirl_num_params_ver1,1)	// Version 1 params
	};


//dialog stuff
class SwirlDlgProc : public ParamMap2UserDlgProc {

private:

    HIMAGELIST hImageLock;
	Swirl * swirl;
	SwirlDlgProc() {}

public:
	SwirlDlgProc(Swirl * m) : hImageLock(NULL), swirl(m)
	{}
	~SwirlDlgProc();
	INT_PTR DlgProc(TimeValue t,IParamMap2 *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);		
	void DeleteThis() {delete this;}
	void SetThing(ReferenceTarget *m) {
		swirl = (Swirl*)m;
		if ( swirl ) swirl->EnableDialogControls();
	}
};

SwirlDlgProc::~SwirlDlgProc()
{
	if ( hImageLock ) {
		ImageList_Destroy(hImageLock);
	}
}

INT_PTR SwirlDlgProc::DlgProc(TimeValue t,IParamMap2 *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch (msg) {

		case WM_INITDIALOG: {
			ICustButton* iLock = GetICustButton(GetDlgItem(hWnd,IDC_LOCK));
			if ( iLock ) {
				hImageLock = ImageList_Create(15, 14, ILC_COLOR24| ILC_MASK , 2, 0);
				HBITMAP hLocked = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_LOCK1));
				HBITMAP hLockedM  = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_LOCK1_M));

				HBITMAP hUnlocked = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_LOCK2));
				HBITMAP hUnlockedM = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_LOCK2_M));
				ImageList_Add(hImageLock, hLocked, hLockedM);
				ImageList_Add(hImageLock, hUnlocked, hUnlockedM);

				iLock->SetImage(hImageLock, 0,1,0,1, 15, 14);
			}
			if (swirl) swirl->EnableDialogControls();

			return TRUE;
		}

		case WM_COMMAND:
			switch (LOWORD(wParam)) 
			{
			case IDC_TEX_SWAP: {
				swirl->SwapInputs(); 
				break;
			}
			break;
		}
	}
		
	return FALSE;
}
   
AColor SwirlSampler::SampleFilter(ShadeContext& sc, float u,float v, float du, float dv) {
   return Swirler->EvalFunction(sc, u, v, du, dv);
   }
AColor SwirlSampler::Sample(ShadeContext& sc, float u,float v) {
   return Swirler->EvalFunction(sc, u, v, 0.0f, 0.0f);
   }

float SwirlSampler::SampleMonoFilter(ShadeContext& sc, float u,float v, float du, float dv) {
   return Swirler->MonoEvalFunction(sc, u, v, du, dv);
   }
float SwirlSampler::SampleMono(ShadeContext& sc, float u,float v) {
   return Swirler->MonoEvalFunction(sc, u, v, 0.0f, 0.0f);
   }

void Swirl::Init() {
	macroRecorder->Disable();  // disable macrorecorder during reset
	if (uvGen) uvGen->Reset();
	else ReplaceReference( 0, GetNewDefaultUVGen());	
	SetRS(RANDOM(65535.0f), TimeValue(0));
	macroRecorder->Enable();
}

void Swirl::Reset() {
	swirlCD.Reset(this, TRUE);	// reset all pb2's
	DeleteReference(2);
	DeleteReference(3);
	Init();
}

Swirl::Swirl()
: lock( 1 ) // value being used in WM_INITDIALOG (use swirl_lock default)
{
   mysamp.Set(this);
   texHandle = NULL;
   subTex[0] = subTex[1] = NULL;
   // mapOn Values are used on loading for filed saved with the PB1 version.
   // default value is expected to be TRUE
   mapOn[0] = mapOn[1] = TRUE;
   pblock = NULL;
   uvGen = NULL;
   rollScroll=0;
}


void Swirl::DiscardTexHandle() {
   if (texHandle) {
      texHandle->DeleteThis();
      texHandle = NULL;
      }
   }

void Swirl::ActivateTexDisplay(BOOL onoff) {
   if (!onoff) 
      DiscardTexHandle();
   }

DWORD_PTR Swirl::GetActiveTexHandle(TimeValue t, TexHandleMaker& thmaker) {
   if (texHandle) {
      if (texHandleValid.InInterval(t))
         return texHandle->GetHandle();
      else DiscardTexHandle();
      }

   Bitmap *bm;
   texHandleValid.SetInfinite();
   Update(t,texHandleValid);
   bm = BuildBitmap(thmaker.Size());
   texHandle = thmaker.CreateHandle(bm,uvGen->SymFlags());
   bm->DeleteThis();

   texHandleValid.SetInfinite();
   pblock->GetValidity(t,texHandleValid);

   return texHandle->GetHandle();
   }

inline UWORD FlToWord(float r) {
   return (UWORD)(65535.0f*r);
   }

Bitmap *Swirl::BuildBitmap(int size) {
   float u,v;
   BitmapInfo bi;
   static MaxSDK::AssetManagement::AssetUser bitMapAssetUser;
   if (bitMapAssetUser.GetId() == MaxSDK::AssetManagement::kInvalidId)
	   bitMapAssetUser = MaxSDK::AssetManagement::IAssetManager::GetInstance()->GetAsset(_T("SwirlTemp"), MaxSDK::AssetManagement::kBitmapAsset);
   bi.SetAsset(bitMapAssetUser);
   bi.SetWidth(size);
   bi.SetHeight(size);
   bi.SetType(BMM_TRUE_32);
   Bitmap *bm = TheManager->Create(&bi);
   if (bm==NULL) return NULL;
   PixelBuf l64(size);
   float d = 1.0f/float(size);
   v = 1.0f - 0.5f*d;

   for (int y=0; y<size; y++) {
      BMM_Color_64 *p64=l64.Ptr();
      u = 0.0f;
      for (int x=0; x<size; x++, p64++) {
         AColor c = DispEvalFunc(u,v);
         p64->r = FlToWord(c.r); 
         p64->g = FlToWord(c.g); 
         p64->b = FlToWord(c.b);
         p64->a = 0xffff; 
         u += d;
         }
      bm->PutPixels(0,y, size, l64.Ptr()); 
      v -= d;
      }
   return bm;
   }


AColor Swirl::SwirlFunc(float u,float v,ShadeContext& sc,float du,float dv ){

   float offset = ls;
   float scale = hs;
   float twist = vs;
   float omega = cv;
   float octaves = (float)h;

   Point3 Ptexture,PtN;
   float rsq;                // Used in calculation of swirl 
   float angle;              // Swirl angle 
   float sine, cosine;       // sin and cos of angle 
   float l, o, a, i;         // Loop control for fractal sum 
   float value;              // Fractal sum is stored here 


   u+=hg; v+=vg;

      rsq = (u)*(u) + (v)*(v); 


     angle = twist * TWOPI * rsq;
     sine = (float)sin (angle);
     cosine = (float)cos (angle);

     Point3 PP (v*cosine - u*sine,
           v*sine + u*cosine,(float)rs);

     /* Compute VLfBm */
     l = 1;  o = 1;  a = 0;
     for (i = 0;  i < octaves;  i += 1) {
        a += o * noise3 (PP * l);
        l *= 2;
        o *= omega;
      }

      value =  ((offset * scale) * a);
 
      AColor r1 = mapOn[0]&&subTex[0] ? subTex[0]->EvalColor(sc): col[0]; 
      AColor r2 = mapOn[1]&&subTex[1] ? subTex[1]->EvalColor(sc): col[1]; 

      AColor rslt = (r1*value)+(r2*(1.0f-value));
      rslt.ClampMinMax();

      return rslt;

}


AColor Swirl::DispEvalFunc( float u, float v) {
   
   float offset = ls;
   float scale = hs;
   float twist = vs;
   float omega = cv;
   float octaves = (float)h;

   Point3 Ptexture,PtN;
   float rsq;                // Used in calculation of twist 
   float angle;              // Twist angle 
   float sine, cosine;       // sin and cos of angle 
   float l, o, a, i;         // Loop control for fractal sum 
   float value;              // Fractal sum is stored here 


   u+=hg; v+=vg;

      rsq = (u)*(u) + (v)*(v); 

     angle = twist * TWOPI * rsq;
     sine = (float)sin (angle);
     cosine = (float)cos (angle);

     Point3 PP (v*cosine - u*sine,
           v*sine + u*cosine,(float)rs);

     /* Compute VLfBm */
     l = 1;  o = 1;  a = 0;
     for (i = 0;  i < octaves;  i += 1) {
        a += o * noise3 (PP * l);
        l *= 2;
        o *= omega;
      }

      value =  ((offset * scale) * a);
 
      AColor r1 = col[0]; 
      AColor r2 = col[1]; 

      AColor k = (r1*value)+(r2*(1.0f-value));
      k.ClampMinMax();

   return k;
   
}


AColor Swirl::EvalFunction(ShadeContext& sc, float u, float v, float du, float dv) {

return SwirlFunc(u,v,sc,du,dv);

}



float Swirl::MonoEvalFunction(ShadeContext& sc, float u, float v, float du, float dv) {

   AColor k = SwirlFunc(u,v,sc,du,dv);
   k.ClampMinMax();
   
   return Intens(k); 
}


AColor Swirl::EvalColor(ShadeContext& sc) {
   if (gbufID) sc.SetGBufferID(gbufID);
   return uvGen->EvalUVMap(sc,&mysamp);
   }

float Swirl::EvalMono(ShadeContext& sc) {
   if (gbufID) sc.SetGBufferID(gbufID);
   return uvGen->EvalUVMapMono(sc,&mysamp);
   }

Point3 Swirl::EvalNormalPerturb(ShadeContext& sc) {
   Point3 dPdu, dPdv;
   if (gbufID) sc.SetGBufferID(gbufID);
   uvGen->GetBumpDP(sc,dPdu,dPdv);
   Point2 dM = (.02f)*uvGen->EvalDeriv(sc,&mysamp);
   return dM.x*dPdu+dM.y*dPdv;
   }

RefTargetHandle Swirl::Clone(RemapDir &remap) {
   Swirl *mnew = new Swirl();
   *((MtlBase*)mnew) = *((MtlBase*)this);
   mnew->ReplaceReference(0,remap.CloneRef(uvGen));
   mnew->ReplaceReference(1,remap.CloneRef(pblock));
   mnew->col[0] = col[0];
   mnew->col[1] = col[1];

   mnew->hs = hs;
   mnew->vs = vs;
   mnew->cv = cv;
   mnew->vg = vg;
   mnew->hg = hg;
   mnew->rs = rs;
   mnew->ls = ls;
   mnew->h = h;
   mnew->lock=lock;
   
   mnew->ivalid.SetEmpty();   
   for (int i = 0; i<NSUBTEX; i++) {
      mnew->subTex[i] = NULL;
      mnew->mapOn[i] = mapOn[i];
      if (subTex[i])
         mnew->ReplaceReference(i+2,remap.CloneRef(subTex[i]));
      }
   BaseClone(this, mnew, remap);
   return (RefTargetHandle)mnew;
   }


BOOL Swirl::SetDlgThing(ParamDlg* dlg)
{
   // set the appropriate 'thing' sub-object for each
   // secondary dialog
	if (dlg == uvGenDlg)
		uvGenDlg->SetThing(uvGen);
	else 
		return FALSE;
	return TRUE;
}

bool Swirl::SpecifySaveReferences(ReferenceSaveManager& referenceSaveManager)
{
	// if saving to previous version that used pb1 instead of pb2...
	DWORD saveVersion = GetSavingVersion();
	if (saveVersion != 0 && saveVersion <= MAX_RELEASE_R13)
	{
		// create the pb1 instance
		IParamBlock* paramBlock1 = CreateParameterBlock( pbdesc,swirl_num_params_ver1,1);
		DbgAssert(paramBlock1 != NULL);
		if (paramBlock1)
		{
			// copy data from the pb2 to the pb1
			int numParamsCopied = CopyParamBlock2ToParamBlock(pblock,paramBlock1,pbdesc,swirl_num_params_ver1);
			DbgAssert(numParamsCopied == (swirl_num_params_ver1-5)); // 5 params are not being used
			// register the reference slot replacement
			referenceSaveManager.ReplaceReferenceSlot(PBLOCK_REF,paramBlock1);
		}
	}
	return Texmap::SpecifySaveReferences(referenceSaveManager);
}

ParamDlg* Swirl::CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp) {
	// create the rollout dialogs
	uvGenDlg = uvGen->CreateParamDlg(hwMtlEdit, imp);	
	IAutoMParamDlg* masterDlg = swirlCD.CreateParamDlgs(hwMtlEdit, imp, this);
	// add the secondary dialogs to the master
	masterDlg->AddDlg(uvGenDlg);

	swirl_param_blk.SetUserDlgProc(new SwirlDlgProc(this));

	return masterDlg;
}

void Swirl::EnableDialogControls()
{
	if (pblock) {
		IParamMap2 *map = pblock->GetMap();
		if (map) map->Enable(swirl_vg, lock!=1);
	}
}


void Swirl::Update(TimeValue t, Interval& valid) {    

	if (!ivalid.InInterval(t)) {
      ivalid.SetInfinite();
      uvGen->Update(t,ivalid);
      pblock->GetValue( swirl_col1, t, col[0], ivalid );
      col[0].ClampMinMax();

      pblock->GetValue( swirl_col2, t, col[1], ivalid );
      col[1].ClampMinMax();

      pblock->GetValue( swirl_hs, t, hs, ivalid );
      pblock->GetValue( swirl_vs, t, vs, ivalid );
      pblock->GetValue( swirl_cv, t, cv, ivalid );

	  // Get lock before getting vg & hg
	  pblock->GetValue( swirl_lock, t, lock, ivalid );
      pblock->GetValue( swirl_vg, t, vg, ivalid );
      pblock->GetValue( swirl_hg, t, hg, ivalid );
      pblock->GetValue( swirl_rs, t, rs, ivalid );
      pblock->GetValue( swirl_ls, t, ls, ivalid );
      pblock->GetValue( swirl_h, t, h, ivalid );

      pblock->GetValue( swirl_map1_on, t, mapOn[0], ivalid);
      pblock->GetValue( swirl_map2_on, t, mapOn[1], ivalid);

      for (int i=0; i<NSUBTEX; i++) {
         if (subTex[i] && mapOn[i] ) 
            subTex[i]->Update(t,ivalid);
         }
      }
   valid &= ivalid;
   }


void Swirl::SetColor(int i, Color c, TimeValue t) {
    col[i] = c;
   pblock->SetValue( i==0?swirl_col1:swirl_col2, t, col[i]);
   }

void Swirl::SwapInputs() {
   theHold.Begin();
   macroRecorder->Disable();

   pblock->SwapControllers(pblock->IDtoIndex(swirl_map1_on),0,pblock->IDtoIndex(swirl_map2_on),0);
   pblock->SwapControllers(pblock->IDtoIndex(swirl_col1),0,pblock->IDtoIndex(swirl_col2),0);

   Texmap *x = subTex[0];
   if ( x ) x->SetAFlag(A_LOCK_TARGET);
   pblock->SetValue( swirl_map1, 0, subTex[1]);
   pblock->SetValue( swirl_map2, 0, x);
   if ( x ) x->ClearAFlag(A_LOCK_TARGET);

   macroRecorder->Enable();
   theHold.Accept( GetString(IDS_SWAP_INPUTS) );

   macroRec->FunctionCall(_T("swap"), 2, 0, mr_prop, _T("base"), mr_reftarg, this, mr_prop, _T("swirl"), mr_reftarg, this);
   macroRec->FunctionCall(_T("swap"), 2, 0, mr_prop, _T("map1"), mr_reftarg, this, mr_prop, _T("map2"), mr_reftarg, this);
   macroRec->FunctionCall(_T("swap"), 2, 0, mr_prop, _T("map1Enabled"), mr_reftarg, this, mr_prop, _T("map2Enabled"), mr_reftarg, this);
}

void Swirl::SetHS(float f, TimeValue t) { 
   hs = f; 
   pblock->SetValue( swirl_hs, t, f);
   }
void Swirl::SetVS(float f, TimeValue t) { 
   vs = f; 
   pblock->SetValue( swirl_vs, t, f);
   }
void Swirl::SetCV(float f, TimeValue t) { 
   cv = f; 
   pblock->SetValue( swirl_cv, t, f);
   }
void Swirl::SetVG(float f, TimeValue t) { 
   vg = f; 
   pblock->SetValue( swirl_vg, t, f);
   }
void Swirl::SetHG(float f, TimeValue t) { 
   hg = f; 
   pblock->SetValue( swirl_hg, t, f);
   }
void Swirl::SetLS(float f, TimeValue t) { 
   ls = f; 
   pblock->SetValue( swirl_ls, t, f);
   }


void Swirl::SetH(int f, TimeValue t) { 
   h = f; 
   pblock->SetValue( swirl_h, t, f);
   }
void Swirl::SetRS(float f, TimeValue t) { 
   rs = f; 
   pblock->SetValue( swirl_rs, t, f);
   }
void Swirl::SetLOCK(int f, TimeValue t) { 
   lock = f; 
   pblock->SetValue( swirl_lock, t, f);
   }



RefTargetHandle Swirl::GetReference(int i) {
   switch(i) {
      case UVGEN_REF: return uvGen;
      case PBLOCK_REF:  return pblock ;
      default:return subTex[i-2];
      }
   }

void Swirl::SetReference(int i, RefTargetHandle rtarg) {
   switch(i) {
      case UVGEN_REF: uvGen = (UVGen *)rtarg; break;
      case PBLOCK_REF:  pblock = (IParamBlock2 *)rtarg; break;
      default: subTex[i-2] = (Texmap *)rtarg; break;
      }
   }

void Swirl::SetSubTexmap(int i, Texmap *m) {
   ReplaceReference(i+2,m);
	if (i==0) {
		swirl_param_blk.InvalidateUI(swirl_col1);
		ivalid.SetEmpty();
	}
	else if (i==1) {
		swirl_param_blk.InvalidateUI(swirl_col2);
		ivalid.SetEmpty();
	}
  }

TSTR Swirl::GetSubTexmapSlotName(int i) {
   switch(i) {
      case 0:  return TSTR(GetString(IDS_SWIRL_PARAMNAME)); 
      case 1:  return TSTR(GetString(IDS_BASE)); 
      default: return TSTR(_T(""));
      }
   }
    
Animatable* Swirl::SubAnim(int i) {
   switch (i) {
      case 0: return uvGen;
      case 1: return pblock;
      default: return subTex[i-2]; 
      }
   }

TSTR Swirl::SubAnimName(int i) {
   switch (i) {
      case 0: return TSTR(GetString(IDS_COORDS));     
      case 1: return TSTR(GetString(IDS_PARAMS));     
      default: return GetSubTexmapTVName(i-2);
      }
   }


/*===========================================================================*\
 | Notification handler
\*===========================================================================*/

RefResult Swirl::NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
   PartID& partID, RefMessage message ) {
   switch (message) {
      case REFMSG_CHANGE:
         ivalid.SetEmpty();
         if (hTarget == pblock) {
			// see if this message came from a changing parameter in the pblock,
			// if so, limit rollout update to the changing item and update any active viewport texture
			ParamID changing_param = pblock->LastNotifyParamID();
			swirl_param_blk.InvalidateUI(changing_param);
			if (changing_param != -1)
				DiscardTexHandle();
         }
         break;

      case REFMSG_UV_SYM_CHANGE:
         DiscardTexHandle();  
         break;

      }
   return(REF_SUCCEED);
   }


/*===========================================================================*\
 | ISave and ILoad stuff
\*===========================================================================*/

#define MTL_HDR_CHUNK 0x4000
#define MAPOFF_CHUNK 0x1000
#define PARAM2_CHUNK 0x1010

IOResult Swirl::Save(ISave *isave) { 
	IOResult res;
	isave->BeginChunk(MTL_HDR_CHUNK);
	res = MtlBase::Save(isave);
	if (res!=IO_OK) return res;
	isave->EndChunk();

	DWORD saveVersion = isave->SavingVersion();
	if ( saveVersion != 0 && saveVersion <= MAX_RELEASE_R13 ) {
		if (!mapOn[0]) {
			isave->BeginChunk(MAPOFF_CHUNK+0);
			isave->EndChunk();
		}
		if (!mapOn[1]) {
			isave->BeginChunk(MAPOFF_CHUNK+1);
			isave->EndChunk();
		}
	}
	else {
		isave->BeginChunk(PARAM2_CHUNK);
		isave->EndChunk();
	}

	return IO_OK;
}  

class SwirlPB1ToPB2PostLoad : public PostLoadCallback {
private:
	Swirl * m_swirl;
	SwirlPB1ToPB2PostLoad() {}
public:
	SwirlPB1ToPB2PostLoad( Swirl * swirl )
		: m_swirl(swirl)
	{}

	virtual void proc(ILoad *iload) {
		if ( m_swirl && m_swirl->pblock ) {
			macroRecorder->Disable();
			m_swirl->pblock->SetValue( swirl_map1_on, 0, m_swirl->mapOn[0]);
			m_swirl->pblock->SetValue( swirl_map2_on, 0, m_swirl->mapOn[1]);
			macroRecorder->Enable();
		}
		// get rid of the callback
		delete this;
	}
};

IOResult Swirl::Load(ILoad *iload) { 
   IOResult res;
   int id;
   bool loadFromParam1 = true;
   while (IO_OK==(res=iload->OpenChunk())) {
      switch(id=iload->CurChunkID())  {
         case MTL_HDR_CHUNK:
            res = MtlBase::Load(iload);
            break;
         case MAPOFF_CHUNK+0:
         case MAPOFF_CHUNK+1:
            mapOn[id-MAPOFF_CHUNK] = 0; 
            break;
         case PARAM2_CHUNK:
         	loadFromParam1 = false;
         	break;
         }
      iload->CloseChunk();
      if (res!=IO_OK) 
         return res;
      }

	if ( loadFromParam1 ) {
   		// register old version ParamBlock to ParamBlock2 converter
		ParamBlock2PLCB* plcb = new ParamBlock2PLCB(versions, 1, &swirl_param_blk, this, PBLOCK_REF);
		iload->RegisterPostLoadCallback(plcb);

		SwirlPB1ToPB2PostLoad* lp = new SwirlPB1ToPB2PostLoad( this );
		iload->RegisterPostLoadCallback( lp );
	}

   return IO_OK;
   }
