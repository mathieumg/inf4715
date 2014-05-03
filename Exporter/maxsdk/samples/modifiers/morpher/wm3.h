//**************************************************************************/
// Copyright (c) 1998-2010 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// Weighted Morpher for 3ds Max 
// by Harry Denholm
//**************************************************************************/

#ifndef __MORPHR3__H
#define __MORPHR3__H

#include <Max.h>
#include <istdplug.h>
#include <meshadj.h>
#include <modstack.h>
#include <imtl.h>
#include <texutil.h>
#include <stdmat.h>
#include <macrorec.h>
#include <vector>

#include "buildver.h"
#include "resource.h"
#include "resourceOverride.h"
#include ".\include\MorpherClassID.h"
#include ".\include\MorpherExport.h"

static Class_ID M3MatClassID(0x4b9937e0, 0x3a1c3da4);
#define MR3_NUM_CHANNELS	100
#define MAX_PROGRESSIVE_TARGETS			25

#define MR3_MORPHERVERSION	010


// Save codes for the morphChannel class
#define MR3_POINTCOUNT					0x0100
#define MR3_POINTCOUNT2					0x0101
#define MR3_SELARRAY					0x0110
#define MR3_NAME						0x0120
#define MR3_PARAMS						0x0130
#define MR3_POINTDATA_MP				0x0140
#define MR3_POINTDATA_MW				0x0150
#define MR3_POINTDATA_MD				0x0160
#define MR3_POINTDATA_MO				0x0170 
#define MR3_PROGRESSIVE_PARAMS			0x0180 
#define MR3_PROGRESSIVE_TARGET_PERCENT		0x0181
#define MR3_PROGRESSIVE_CHANNEL_PERCENT		0x0182
#define MR3_PROGRESSIVE_CHANNEL_CURVATURE	0x0183

#define MR3_TARGETCACHE					0x0190
#define MR3_TARGETCACHE_POINTS			0x0192

#define MR3_FILE_VERSION				0x0195

#define MR3_TARGET_CACHE_SUBCHUNK		0x0300

// Save codes for the MorphR3 class
#define MR3_MARKERNAME		0x0180
#define MR3_MARKERINDEX		0x0185
#define MR3_MC_CHUNK		0x0190
#define MR3_MC_SUBCHUNK		0x0200
#define MR3_FLAGS			0x0210



// paramblock index table
#define PB_OV_USELIMITS 0 // whether or not the global 'Use Limits' checkbox is on
#define PB_OV_SPINMIN   1 // low clamp value for global limits
#define PB_OV_SPINMAX   2 // high clamp value for global limits
#define PB_OV_USESEL    3 // whether or not global vertex selection button is on
#define PB_AD_VALUEINC  4 // the spinner increment value (0,1,2)
#define PB_CL_AUTOLOAD  5 // whether or not 'Autoload Targets' is active on channel list


// Channel operation flags
#define OP_MOVE				0
#define OP_SWAP				1


// two handy macros to set cursors for busy or normal operation
#define UI_MAKEBUSY			SetCursor(LoadCursor(NULL, IDC_WAIT));
#define UI_MAKEFREE			SetCursor(LoadCursor(NULL, IDC_ARROW));
			

// Morph Material ui defines
#define NSUBMTL 10

// Updater flags
#define UD_NORM				0
#define UD_LINK				1

#define IDC_TARGET_UP		0x4000
#define IDC_TARGET_DOWN		0x4010

extern ClassDesc* GetMorphR3Desc();

#ifndef NO_MTL_MORPHER
extern ClassDesc* GetM3MatDesc();
#endif // NO_MTL_MORPHER

extern HINSTANCE hInstance;

TCHAR *GetString(int id);

//   SV Integration
// *----------------*
extern HIMAGELIST hIcons32, hIcons16;
extern COLORREF bkColor;
enum IconIndex {II_MORPHER};
void LoadIcons(COLORREF bkColor);
// *----------------*

class MorphR3;
class M3Mat;
class M3MatDlg;
class Restore_FullChannel;

// Dialog handlers
INT_PTR CALLBACK Legend_DlgProc		(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK Globals_DlgProc		(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK Advanced_DlgProc		(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK ChannelParams_DlgProc	(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK ChannelList_DlgProc	(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

INT_PTR CALLBACK IMPORT_DlgProc		(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK EXPORT_DlgProc		(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK ChannelOpDlgProc		(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

INT_PTR CALLBACK BindProc				(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK NameDlgProc			(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static Point3 junkpoint(0,0,0);
class morphChannel;

// Represents one progressive target
class TargetCache
{
public:
	long mNumPoints;
	INode *mTargetINode;
	std::vector<Point3> mTargetPoints;
	float mTargetPercent;

	TargetCache(){
		mNumPoints=0;
		mTargetINode=NULL;
		mTargetPercent = 0.0f;
	}

	TargetCache(const TargetCache &tcache);

	~TargetCache(){
		mTargetPoints.erase(mTargetPoints.begin(), mTargetPoints.end());
		mTargetPoints.resize(0);
	}

	void Clear(void){
		mTargetPoints.erase(mTargetPoints.begin(), mTargetPoints.end());
		mTargetPoints.resize(0);
		mNumPoints=0;
		mTargetINode=NULL;
	}

	void operator=(const TargetCache &tcache); 

	void operator=(const morphChannel &mchan);
	void Init( INode *nd);
	void Reset(void);
	const Point3 &GetPoint(const long &index) { 
		if(index>=0 && index<mNumPoints) return mTargetPoints[index]; 
		return junkpoint;
	}
	
	IOResult Save(ISave* isave);
	IOResult Load(ILoad* iload);
	INode *RefNode(void) { return mTargetINode; }
	
};

/* 
	Each channel in the morpher is made up of a morphChannel object. 
	Each morph channel holds all the information for a morph target the morpher 
	needs in order to be able to create the morph result. It also exposes 
	methods that initialize the morph channel from nodes in the scene, rebuild the
	internal data, delete the channel, etc. Think of this class being the morphic 
	data management system.
*/
class morphChannel
{
public:

	// Construct/Destruct
	~morphChannel();
	morphChannel();
	morphChannel(const morphChannel & from);

	MorphR3		*mp;

	float mCurvature;

	// Number of points in this morph channel
	int			mNumPoints;

	int iTargetListSelection;

	// mPoints and mWeights contain the complete representation of the geometry of the object
	// in point and weight (in terms of NURBS) form. These are used in advanced modifier tasks
	// such as extracting objects from the morpher or creating morph targets from existing morph
	// results. mDeltas are the difference values from the original values to the new target object.
	// The morpher does the job of keeping all of these values up to date.
	std::vector<Point3>		mPoints;
	std::vector<Point3>		mDeltas;
	std::vector<double>		mWeights;
	
	std::vector<TargetCache>	mTargetCache;

	// A bit array of selected points, extracted from the target when processed.
	BitArray	mSel;

	// If this channel has an active connection to a target object in the scene then 
	// this will contain the INode pointer to that target object.
	INode*		mConnection;
	
	// The user visible name of the morphchannel
	TSTR		mName;
	// The number of progressive (intermediate) targets - does not include the (main) target
	int mNumProgressiveTargs;
	// The contribution of this channel's target object to the overall morph solution
	float mTargetPercent;

	// Various, non-animatable stuff
	BOOL mActive; // TRUE if the channel has data in it in some form
	BOOL mModded; // TRUE if the channel has been changed in SOME form, ie, had its name changed or similar
	BOOL mUseLimit; // TRUE if the channel limits are turned on
	BOOL mUseSel; // TRUE if the channel respects vertex/point selection
	float mSpinmin,mSpinmax; // clamp values for channel limits

	// TRUE if the channel cannot be used in the morph result - ie, its point count is wrong, etc.
	// It will appear as GREY STATUS in the channel list UI if set to TRUE.
	BOOL		mInvalid;

	// If FALSE, the channel is not used in the morph result. If TRUE, it is.
	// It will appear as GREY STATUS in the channel list UI if set to FALSE.
	BOOL		mActiveOverride;

	void InitTargetCache(const int &targnum, INode *nd){ mTargetCache[targnum].Init( nd); }

	// A parameter block per channel - holds it's animatable channel value. 
	// It has only one entry, index 0, which holds the floating point value.
	IParamBlock* cblock;
	
	// Delete and reset the channel. NOTE: you still have to wire in a new paramblock into 
	// the morph channel. Check the MorpherView utility for example code.
	MorphExport void ResetMe();

	MorphExport void AllocBuffers( int sizeA, int sizeB );

	// Do some rough calculations about how much space this channel takes up
	// This isn't meant to be fast or terribly accurate!
	MorphExport float getMemSize();

	// The rebuildChannel call will recalculate the optimization data
	// and refill the mSel selection array. This will be called each time a
	// targeted node is changed, or any of the 'Update Target' buttons is
	// pressed on the UI
	MorphExport void rebuildChannel();

	// Takes in a node from the scene, and initialize the channel with it. 
	// This is an important method that is used a lot by the Morpher UI to assign 
	// the channels with actual object data.
	// Most channels will start out by using this function.
	MorphExport void buildFromNode( INode *node , BOOL resetTime=TRUE , TimeValue t=0, BOOL picked = FALSE );

	// Transfer data to another channel
	MorphExport void operator=(const morphChannel& from);
	void operator=(const TargetCache& tcache);

	// Load/Save channel to stream
	MorphExport IOResult Save(ISave* isave);
	MorphExport IOResult Load(ILoad* iload);
	void SetUpNewController();
	int  NumProgressiveTargets(void) { return mNumProgressiveTargs; }
	void ResetRefs(MorphR3 *, const int&);
	float GetTargetPercent(const int &which);
	void ReNormalize();
	void CopyTargetPercents(const morphChannel &chan);
};


// Used for picking a morph target from the scene
class GetMorphNode : 
		public PickModeCallback,
		public PickNodeCallback {
	public:				
		MorphR3 *mp;

		BOOL isPicking;

		GetMorphNode() {
			mp=NULL;
			isPicking=FALSE;
		}

		MorphExport BOOL  HitTest(IObjParam *ip,HWND hWnd,ViewExp *vpt,IPoint2 m,int flags);		
		MorphExport BOOL  Pick(IObjParam *ip,ViewExp *vpt);		
		MorphExport BOOL  Filter(INode *node);
		MorphExport BOOL  RightClick(IObjParam *ip,ViewExp *vpt) {return TRUE;}

		MorphExport void  EnterMode(IObjParam *ip);
		MorphExport void  ExitMode(IObjParam *ip);		

		MorphExport PickNodeCallback *GetFilter() {return this;}
		// Verifies if the specified node's state as evaluated at the specified time,
		// is a valid morph target for the specified modifier
		static bool IsValidMorphTargetType(MorphR3* pMorphModifier, INode* node, TimeValue t);
	};

/* 
	Most developers will not need to use this class, as it is internally managed 
	to keep a persistent cache of the input morphable object.
	Contains some functions to destroy and rebuild the cache.
*/
class morphCache
{

public:

	BOOL CacheValid;

	Point3*		oPoints;
	double*		oWeights;
	BitArray	sel;

	int		Count;

	morphCache ();
	~morphCache () { NukeCache(); }

	MorphExport void MakeCache(Object *obj);
	
	// Removes all cached data and it will be rebuilt the next time the modifier
	// is refreshed (ModifyObject gets called)
	MorphExport void NukeCache();

	// Returns TRUE if the cache is built. FALSE if it has been nuked and not rebuilt yet.
	MorphExport BOOL AreWeCached();
};



/*===========================================================================*\
 | Morph Channel restore object
\*===========================================================================*/

class Restore_FullChannel : public RestoreObj {

public:
	MorphR3 *mp;
	morphChannel undoMC;
	morphChannel redoMC;
	int mcIndex;
	BOOL update;
	float *targpercents_undo, *targpercents_redo;
	int ntargs_undo, ntargs_redo;
	
	
	// Constructor
	Restore_FullChannel(MorphR3 *mpi, const int idx, const BOOL upd = TRUE);
	~Restore_FullChannel();

	// Called when Undo is selected
	void Restore(int isUndo);

	// Called when Redo is selected
	void Redo();

	// Called to return the size in bytes of this RestoreObj
	int Size();
};

/*===========================================================================*\
 | Morph Channel restore object
\*===========================================================================*/

class Restore_Marker : public RestoreObj {

public:
	MorphR3 *mp;

	Tab<int>			mIndex;
	NameTab				mName;
	int					markerSel;

	Tab<int>			rIndex;
	NameTab				rName;
	int					rSel;
	int					chanNum;
	int					rchanNum;
			

	// Constructor
	Restore_Marker(MorphR3 *mpi);

	
	// Called when Undo is selected
	void Restore(int isUndo);

	// Called when Redo is selected
	void Redo();

	// Called to return the size in bytes of this RestoreObj
	int Size();
};

class Restore_Display : public RestoreObj {

public:
	MorphR3 *mp;

	// Constructor
	Restore_Display(MorphR3 *mpi);

	
	// Called when Undo is selected
	void Restore(int isUndo);

	// Called when Redo is selected
	void Redo();

	// Called to return the size in bytes of this RestoreObj
	int Size();
};


class Restore_CompactChannel : public RestoreObj {

public:
	MorphR3 *mp;

	Tab<int>			mtarg;
	Tab<int>			msrc;

	// Constructor
	Restore_CompactChannel(MorphR3 *mpi, Tab<int> &targ, Tab<int> &src);

	
	// Called when Undo is selected
	void Restore(int isUndo);

	// Called when Redo is selected
	void Redo();

	// Called to return the size in bytes of this RestoreObj
	int Size(){ return 0; }
};

class Restore_TargetMove : public RestoreObj {

public:
	MorphR3 *mp;

	int		from, to;

	// Constructor
	Restore_TargetMove(MorphR3 *mpi, const int &fr, const int &t)
	{
		mp = mpi; from =fr; to = t;
	}

	void Restore(int isUndo);
	void Redo();

	int Size(){ return 0; }
};


/*
	The main morpher modifier class. 
	Contains the bank of morphChannel records (in an array called chanBank), 
	as well as the primary parameter block, various global variables and some 
	methods for handling all the updating or changing of the UI panels. 
	Think of this class being the UI management system.
*/
class MorphR3 : public Modifier, TimeChangeCallback {
	public:

		float mFileVersion;

		// Access to the interface
		static IObjParam *ip;
		
		// Pointer to the morph channels
		std::vector<morphChannel>	chanBank;

		// Pointer to the morph material bound to this morpher
		M3Mat *morphmaterial;
		
		// chanSel and chanNum work together to save the position in the UI that the user
		// is looking at. chanSel is the 0-9 clamped number of the button on the UI, and
		// chanNum is the 0-90 clamped value that shifts the list up and down.
		// For instance, if the user is looking at channel 20, using the first button:
		// chanSel = 0 and chanNum = 19
		// Adding the values together gets the final channel being modified:
		// chanBank[chanSel+chanNum].mActive = TRUE; 
		// A developer shouldn't directly change chanNum - use the VScroll function to do this instead
		int					chanSel; // Currently selected channel (0-9)
		int					chanNum; // Currently viewable channel banks (0-99)

		// Spinner controls 
		// The spinners controlling the channel values for the 10 viewable channels
		ISpinnerControl		*chanSpins[10];
		// Spinners from global settings page
		ISpinnerControl		*glSpinmin,*glSpinmax;
		// Spinners from the channel params dlg
		ISpinnerControl		*cSpinmin,*cSpinmax,*cCurvature,*cTargetPercent;

		// Global parameter block
		IParamBlock			*pblock;

		// The window handles for the rollout pages
		HWND hwGlobalParams, hwChannelList,	hwChannelParams, hwAdvanced, hwLegend;
		static HWND hMaxWnd;

		// For the namer dialog
		ICustEdit			*newname;

		// The local cache, and only instance of the morphCache class. 
		morphCache MC_Local;

		BOOL tccI;
		TCHAR trimD[50];

		// 'Save as Current' support
		BOOL recordModifications;
		int recordTarget;

		// Marker Management - used to add or delete items from the marker dropdown list.
		// The markerIndex and markerName must syncronise properly - the Index tab contains
		// the 0-90 value for the marker itself, and Name contains the name to display in the
		// dropdown list. markerSel is the Id of the marker selected in the dropdown list. If
		// this value is -1, no marker is selected. To add a new marker, try:
		// mp is a valid MorphR3 pointer
		// mp->markerName.AddName("Ishani's Marker");
		// int tmp = 35;
		// mp->markerIndex.Append(1,&tmp,0);
		// mp->markerSel = mp->markerName.Count()-1;
		// mp->Update_channelMarkers();
		Tab<int>			markerIndex;
		NameTab				markerName;
		int					markerSel;

		// Channel operation flag for dialog use
		int					cOp;
		int					srcIdx;

		// storage variable for states between dialog procs
		bool hackUI;

		//Constructor/Destructor
		MorphR3();
		~MorphR3();


		// TimeChangeCallback
		void TimeChanged(TimeValue t) {
			if(hwChannelList) Update_channelValues();

			Interval valid=FOREVER;	int itmp; 
			Interface *Cip = GetCOREInterface();

			if(pblock&&Cip)
			{
				pblock->GetValue(PB_CL_AUTOLOAD, 0, itmp, valid);
				if(itmp==1) NotifyDependents(FOREVER, PART_OBJ, REFMSG_CHANGE);
			}

		}


		// From Animatable
		void DeleteThis() { delete this; }
		void GetClassName(TSTR& s) { s= TSTR(GetString(IDS_CLASS_NAME)); }  
		virtual Class_ID ClassID() { return MR3_CLASS_ID;}		
		RefTargetHandle Clone(RemapDir& remap);
		const TCHAR *GetObjectName() { return GetString(IDS_CLASS_NAME); }

		SvGraphNodeReference SvTraverseAnimGraph(IGraphObjectManager *gom, Animatable *owner, int id, DWORD flags);
		TSTR SvGetRelTip(IGraphObjectManager *gom, IGraphNode *gNodeTarger, int id, IGraphNode *gNodeMaker);

		IOResult Load(ILoad *iload);
		IOResult Save(ISave *isave);

		//From Modifier
		ChannelMask ChannelsUsed()  { return PART_GEOM|PART_TOPO|SELECT_CHANNEL; }
		ChannelMask ChannelsChanged() { return PART_GEOM; }

		void Bez3D(Point3 &b, const Point3 *p, const float &u);
		void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);
		Class_ID InputType() {
			// removed the following
			// it was screwing with modstack evaluation severely!
			//Interface *ip = GetCOREInterface();
			//if(ip&&ip->GetSelNodeCount()>1) return Class_ID(0,0);
			return defObjectClassID;
		}
	
		Interval LocalValidity(TimeValue t);
		void NotifyInputChanged(Interval changeInt, PartID partID, RefMessage message, ModContext *mc);

		void DeleteChannel(const int &);
		// From BaseObject
		BOOL ChangeTopology() {return FALSE;}
		int GetParamBlockIndex(int id) {return id;}

		//From ReferenceMaker
		int NumRefs();
		RefTargetHandle GetReference(int i);
private:
		virtual void SetReference(int i, RefTargetHandle rtarg);
public:
		
		int NumSubs();
		Animatable* SubAnim(int i);
		TSTR SubAnimName(int i);
		bool CheckMaterialDependency( void );
		bool CheckSubMaterialDependency( void );
		RefResult NotifyRefChanged( Interval changeInt,RefTargetHandle hTarget, 
		   PartID& partID, RefMessage message);
		void TestMorphReferenceDependencies( const RefTargetHandle hTarget);
		
		CreateMouseCallBack* GetCreateMouseCallBack() {return NULL;}
		void BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev);
		void EndEditParams(IObjParam *ip, ULONG flags,Animatable *next);

		Interval GetValidity(TimeValue t);
		ParamDimension *GetParameterDim(int pbIndex);
		TSTR GetParameterName(int pbIndex);

		// Handles the scroll bar on the channel list UI
		// Handles scrolling, but can be used to drive the UI scroller. 
		// To move to a specific location, VScroll(SB_THUMBPOSITION,35);
		// where 35 is the chanNum offset you require.
		MorphExport void VScroll(int code, short int cpos );

		// Clamps channel number to valid range
		MorphExport void Clamp_chanNum();
		
		// Moves or swaps morphChannels. The flags parameter can be one of the following:
		// OP_MOVE or OP_SWAP, and src and targ being the 0-based channel index numbers you want to use.
		MorphExport void ChannelOp(int src, int targ, int flags);
		
		// The update methods reload and display various chunks of the UI. 
		// Call them to update changes as you make them to the Morpher.
		MorphExport void Update_globalParams();
		MorphExport void Update_advancedParams();	
		MorphExport void Update_channelParams();


		// evaluate the value increments setting
		MorphExport float GetIncrements();
		// SetScale on the channel list spinners
		MorphExport void Update_SpinnerIncrements();

		// Functions to update the channel list dialog box:
		MorphExport void Update_colorIndicators();
		MorphExport void Update_channelNames();
		MorphExport void Update_channelValues();
		MorphExport void Update_channelLimits();
		MorphExport void Update_channelInfo();
		MorphExport void Update_channelMarkers();
		// Seperated cause this function is pretty expensive
		// Lots done, complete update - calls all functions above
		MorphExport void Update_channelFULL();
		
		// Used to trim fp values to a number of decimal points
		MorphExport float TrimDown(float value, int decimalpts);

		BOOL inRender;

		int RenderBegin(TimeValue t, ULONG flags) {	
			inRender = TRUE;
			return 1; 	
			}
		int RenderEnd(TimeValue t) { 	
			inRender = FALSE;	
			return 1; 	
			}

		int CurrentChannelIndex(void) {return chanNum + chanSel; }
		morphChannel &CurrentChannel(void) { return chanBank[chanNum + chanSel]; }

		float GetCurrentTargetPercent(void); 
		void SetCurrentTargetPercent(const float &fval);
	
		void DeleteTarget(void);
		void Update_TargetListBoxNames(void);
		void SwapTargets(const int way);
		void SwapTargets(const int from, const int to, const bool isundo);

		int GetRefNumber(int chanNum, int targNum) { return (200 + targNum + (chanNum * MAX_PROGRESSIVE_TARGETS)); }
		void DisplayMemoryUsage(void );

	public:
		static const float kTensionMin;
		static const float kTensionMax;
		static const float kProgressiveTargetWeigthMin;
		static const float kProgressiveTargetWeigthMax;


		void RescaleWorldUnits(float f);
};

/*
	Class M3MatDlg and class M3Mat manage the Morpher Material. Only small, 
	but they allow the developer to refresh the link to the morpher modifier, 
	and access the various submaterials and settings thereof.
*/
class M3MatDlg : public ParamDlg {
	public:		
		HWND hwmedit;

		IMtlParams *ip;

		M3Mat *theMtl;

		HWND hPanel; 

		ICustButton *iBut[NSUBMTL];
		ICustButton *bBut;
		ICustButton *pickBut;

		MtlDADMgr dadMgr;

		BOOL valid;

		M3MatDlg(HWND hwMtlEdit, IMtlParams *imp, M3Mat *m); 
		~M3MatDlg();
		
		BOOL WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);				
		void Invalidate();		
		void DragAndDrop(int ifrom, int ito);
		void UpdateSubMtlNames();
		void ActivateDlg(BOOL onOff) {}
		
		// methods inherited from ParamDlg:		
		void ReloadDialog();
		Class_ID ClassID() {return M3MatClassID;}
		void SetThing(ReferenceTarget *m);
		ReferenceTarget* GetThing() { 
			return (ReferenceTarget *)theMtl; 
		}
		void DeleteThis() { delete this;  }	
		void SetTime(TimeValue t) {Invalidate();}

		int FindSubMtlFromHWND(HWND hw);

		MorphExport void VScroll(int code, short int cpos );
		MorphExport void Clamp_listSel();

		// Updates the information from Morpher Modifier -> Morpher Material. 
		// upFlag can be one of the following values:
		// UD_NORM - just redisplay the info
		// UD_LINK - reconnect to the modifier, reload all information
		MorphExport void UpdateMorphInfo(int upFlag);
};

class M3Mat : public Mtl, public IReshading  {	
	public:
		M3MatDlg *matDlg;

		// 100 materials for 100 morph channels, plus 1 base material
		Mtl *mTex[101];
		BOOL mapOn[100];

		BOOL inRender;

		// Morph mod pointer
		MorphR3	*morphp;
		TSTR obName;

		// Temp node pointer used in the mtl pick mode
		INode *Wnode;

		IParamBlock *pblockMat;

		Interval ivalid;
		int listSel;

		//- ke - 5.30.02 - merge reshading code
		ReshadeRequirements mReshadeRQ;


		M3Mat(BOOL loading);
		~M3Mat();

		void* GetInterface(ULONG id);

		void NotifyChanged() {
			NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
		}
		

		// From MtlBase and Mtl
		void SetAmbient(Color c, TimeValue t) {}		
		void SetDiffuse(Color c, TimeValue t) {}		
		void SetSpecular(Color c, TimeValue t) {}
		void SetShininess(float v, TimeValue t) {}				
		
		Color GetAmbient(int mtlNum=0, BOOL backFace=FALSE);
	    Color GetDiffuse(int mtlNum=0, BOOL backFace=FALSE);
		Color GetSpecular(int mtlNum=0, BOOL backFace=FALSE);
		float GetXParency(int mtlNum=0, BOOL backFace=FALSE);
		float GetShininess(int mtlNum=0, BOOL backFace=FALSE);		
		float GetShinStr(int mtlNum=0, BOOL backFace=FALSE);
		float WireSize(int mtlNum=0, BOOL backFace=FALSE);
				
		ParamDlg* CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp);
		
		void Shade(ShadeContext& sc);
		float EvalDisplacement(ShadeContext& sc); 
		Interval DisplacementValidity(TimeValue t); 
		void Update(TimeValue t, Interval& valid);
		void Reset(  );
		Interval Validity(TimeValue t);
		
		Class_ID ClassID() {return M3MatClassID; }
		SClass_ID SuperClassID() {return MATERIAL_CLASS_ID;}
		void GetClassName(TSTR& s) {s=GetString(IDS_MORPHMTL);}  

		// begin - ke - 5.30.02 - merge reshading code
		BOOL SupportsRenderElements(){ return FALSE; }
		ReshadeRequirements GetReshadeRequirements() { return mReshadeRQ; } // mjm - 06.02.00
		void PreShade(ShadeContext& sc, IReshadeFragment* pFrag);
		void PostShade(ShadeContext& sc, IReshadeFragment* pFrag, int& nextTexIndex, IllumParams* ip);
		// end - ke - merge reshading code



		void DeleteThis() {
			delete this;
		}	

		// Methods to access sub-materials of meta-materials
	   	int NumSubMtls() {return 101;}
		Mtl* GetSubMtl(int i) {
			return mTex[i];
		}
		void SetSubMtl(int i, Mtl *m);
		TSTR GetSubMtlSlotName(int i) {
			if(i==100) return GetString(IDS_MTL_BASENAME);

			TCHAR s[25];
			if(morphp) _stprintf(s,_T("Mtl %i (%s)"),i+1,morphp->chanBank[i].mName);
			else _stprintf(s,GetString(IDS_MTL_MAPNAME),i+1);
			if(i<101) return s;

			return _T("x");
		}


		int NumSubs() {return 101;} 
		Animatable* SubAnim(int i);
		TSTR SubAnimName(int i);
		int SubNumToRefNum(int subNum) {return subNum;}

		// From ref
 		int NumRefs() {return 103;}
		RefTargetHandle GetReference(int i);
private:
		virtual void SetReference(int i, RefTargetHandle rtarg);
public:

		RefTargetHandle Clone(RemapDir &remap);
		RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
		   PartID& partID, RefMessage message);

		
		int RenderBegin(TimeValue t, ULONG flags) {	
			if(flags!=RENDERBEGIN_IN_MEDIT) inRender = TRUE;
			return 1; 	
			}
		int RenderEnd(TimeValue t) { 	
			inRender = FALSE;	
			return 1; 	
			}


		// IO
		IOResult Save(ISave *isave); 
		IOResult Load(ILoad *iload); 

		// From Mtl
		bool IsOutputConst( ShadeContext& sc, int stdID );
		bool EvalColorStdChannel( ShadeContext& sc, int stdID, Color& outClr );
		bool EvalMonoStdChannel( ShadeContext& sc, int stdID, float& outVal );
	};

// Used for picking a morph modifier from the Morph Material
class GetMorphMod : 
		public PickObjectProc
{
	public:				
		M3Mat *mp;

		BOOL isPicking;

		GetMorphMod() {
			mp=NULL;
			isPicking=FALSE;
		}

		MorphExport BOOL  Pick(INode *node);		
		MorphExport BOOL  Filter(INode *node);

		MorphExport void  EnterMode();
		MorphExport void  ExitMode();		
	};

class MorphR3PostLoadCallback : public PostLoadCallback {
	public:
		MorphR3 *mp;
		MorphR3PostLoadCallback(ParamBlockPLCB *c) {mp=NULL;}
		MorphR3PostLoadCallback(MorphR3 *m) {mp = m;}
		void proc(ILoad *iload) ;
};


/** This class represents the Morpher API
 \deprecated This class has been deprecated as of 3ds Max 2012
 A more complete Morpher API is available in maxsdk\samples\modifiers\morpher\include
*/
class MAX_DEPRECATED IMorphClass
{
public:
	MorphExport BOOL AddProgessiveMorph(MorphR3 *mp, int morphIndex, INode *node);
	MorphExport BOOL DeleteProgessiveMorph(MorphR3 *mp, int morphIndex, int progressiveMorphIndex);
	MorphExport void SwapMorphs(MorphR3 *mp, const int from, const int to, BOOL swap);		
	MorphExport void SortProgressiveTarget(MorphR3 *mp, int morphIndex, int pMorphIndex );
	MorphExport void SwapPTargets(MorphR3 *mp,const int morphIndex, const int from, const int to, const bool isundo);
	MorphExport void SetTension(MorphR3 *mp, int morphIndex, float tension );
	MorphExport void HoldMarkers(MorphR3 *mp) ;
	MorphExport void HoldChannel(MorphR3 *mp, int channel) ;
};


#endif
