/*****************************************************************************

	FILE: layerctrl.h

	DESCRIPTION: 
	
	CREATED BY:	Michael Zyracki

	HISTORY:	January, 2005	Creation

 	Copyright (c) 2005, All Rights Reserved.
 *****************************************************************************/

#ifndef __LAYERCTRL__H
#define __LAYERCTRL__H


#include "iparamb2.h"
#include "istdplug.h"
#include "notify.h"
#include "ILayerControl.h"
#include <ILockedTracks.h>

extern TCHAR *GetString(int id);
extern HINSTANCE hInstance;


#define MASTERLAYERCONTROLMANAGER_CLASS_ID	Class_ID(0xf4871a5, 0x781f1430)
#define MASTERLAYERCONTROLMANAGER_CNAME	GetString(IDS_MASTERLAYERCONTROLMANAGER)



// Forward Declarations
class MasterLayerControl;
class LayerControl;
class AnimLayerToolbarControl;
class MasterLayerControlManagerPostLoadCallback;

#pragma region class MasterLayerControlManager

class MasterLayerControlManager  : public Control
{
	friend class LayerControl;
	friend class MasterLayerControlManagerPostLoadCallback;
public:
	typedef enum {masterlayers} Enums;
	
	//structure used when finding where we should go.
	struct LCInfo
	{
		INode *node; //the node it's on
		LayerControl *lC; 
		Animatable * client;
		int subNum;
	};

	//Constructor/deconstructor
	MasterLayerControlManager ();
	virtual ~MasterLayerControlManager ();


	//if selction changes, used for CUI button/enabling disabling..
	//we store what layers are 'active' based upon the selection set!
	void NotifySelectionChanged();
	bool ShowEnableAnimLayersBtn();
	bool ShowAnimLayerPropertiesBtn();
	bool ShowAddAnimLayerBtn();
	bool ShowDeleteAnimLayerBtn();
	bool ShowCopyAnimLayerBtn();
	bool ShowPasteActiveAnimLayerBtn();
	bool ShowPasteNewLayerBtn();
	bool ShowCollapseAnimLayerBtn();
	bool ShowDisableAnimLayerBtn();
	bool ShowSelectNodesFromActive();

	//if locked we disable and prevent certain things, like pasting ,collapsing and disabling
	bool AreAnyActiveLayersLocked();

	//used by the copy and paste operations....
	// no Control *copyClip;			//LayerControl
	TSTR copyNameClip;
	float copyWeightClip;

	//combo control
	static AnimLayerToolbarControl* toolbarCtrl;

	Class_ID ClassID() { return MASTERLAYERCONTROLMANAGER_CLASS_ID; }  
	SClass_ID SuperClassID() { return CTRL_USERTYPE_CLASS_ID; } 
	void GetClassName(TSTR& s) {s = _T("MASTER_LAYER_MANAGER");}//MASTERLAYERCONTROLMANAGER_CNAME;}

	// Animatable methods		
	void DeleteThis() {delete this;}
	void *GetInterface(ULONG IFaceID);
	void GetValue(TimeValue t, void *val, Interval &valid, GetSetMethod method=CTRL_ABSOLUTE){};
	void SetValue(TimeValue t, void *val, int commit, GetSetMethod method){};
	int IsKeyable() {return 0;}		
	BOOL IsAnimated() {return FALSE;}
	int NumSubs();
	Animatable* SubAnim(int i);
	TSTR SubAnimName(int i);
	int NumRefs();
	RefTargetHandle GetReference(int i);
private:
	virtual void SetReference(int i, RefTargetHandle rtarg);
public:
	BOOL CanAssignController(int subAnim){return FALSE;}
	BOOL CanCopyAnim() {return FALSE;}
	BOOL CanMakeUnique(){return FALSE;}
	BOOL IsReplaceable() { return FALSE;}	
	BOOL CanApplyEaseMultCurves() { return FALSE;}
	
	RefResult NotifyRefChanged(
			Interval changeInt, 
			RefTargetHandle hTarget, 
     		PartID& partID,  
			RefMessage message);
	virtual IOResult Save(ISave *isave);
	virtual IOResult Load(ILoad *iload);


	// Control methods				
	void Copy(Control *from) {}
	BOOL IsLeaf() {return TRUE;}
	void CommitValue(TimeValue t) {}
	void RestoreValue(TimeValue t) {}		
	void EditTimeRange(Interval range,DWORD flags){};

	int NumParamBlocks();
	IParamBlock2 *GetParamBlock(int i);
	IParamBlock2 *GetParamBlockByID(BlockID id);

	void InvalidateUI();
	RefTargetHandle Clone(RemapDir& remap);

	IParamBlock2 *pblock;

	int  GetNumMLCs();
	MasterLayerControl *GetNthMLC(int which);
	int GetNthMLC(TSTR &name);
	int GetMLCIndex(TSTR &name);
	int GetMLCIndex(MasterLayerControl *mlc);
	void AppendMLC(MasterLayerControl *mLC);
	void InsertMLC(int location,MasterLayerControl *mLC);
	void SetLayerActive(int index);
	void SetLayerActiveNodes(int index, Tab<INode *> &nodeTab);	
	void GetActiveLayersNodes(Tab<INode *> &nodeTab,Tab<int> &layers);
	void GetNodesActiveLayer(Tab<INode *> &nodeTab);
	bool ContinueDelete();
	int GetLayerActive();
	void GetLayerControls(Tab<INode*> &nodes, Tab<LCInfo> &layerControls);
	BOOL EnableAnimLayer(ReferenceTarget *anim, ReferenceTarget *client, int subNum);
	BOOL CanEnableAnimLayer(ReferenceTarget *anim, ReferenceTarget *client,int subNum);
	BOOL CanEnableAnimLayer(ReferenceTarget *anim, ReferenceTarget *client,int subNum,INode *node,XMLAnimTreeEntryList *list);
	void GetNodesLayers(Tab<INode *> &nodeTab, Tab<int> &layers,Tab<bool> &layersOnAll);
	void SetLayerName(int index, TSTR name);
	TSTR GetLayerName(int index);
	float GetLayerWeight(int index,TimeValue t);
	void SetLayerWeight(int index, TimeValue t, float weight);
	Control* GetLayerWeightControl(int index);
	bool SetLayerWeightControl(int index, Control *c);
	bool GetLayerMute(int index);
	void SetLayerMute(int index, bool mute);
	bool GetLayerOutputMute(int index);
	void SetLayerOutputMute(int index, bool mute);
	bool GetLayerLocked(int index);
	void SetLayerLocked(int index, bool mute);
	void SetUpCustUI(ICustToolbar *toolbar,int id,HWND hWnd, HWND hParent);
	void SelectNodesFromActive();
	void GetAllNodes(Tab <INode *> &allNodes);
	TCHAR *GetComboToolTip();

	Control *GetKeyableController(Control *control);
	void DeleteNthLayer(int index);
	void DeleteNthLayerNodes(int index, Tab<INode *> &nodes);

	void CopyLayerNodes(int index,Tab<INode *> &nodeTab);
	void PasteLayerNodes(int index,Tab<INode *> &nodeTab);

	void CollapseLayerNodes(int index,Tab<INode *> &nodeTab);
	void DisableLayerNodes (Tab<INode *> &nodeTab);
	void SetFilterActiveOnlyTrackView(bool val);
	bool GetFilterActiveOnlyTrackView();
	void SetJustUpToActive(bool v);
	bool GetJustUpToActive();
	void SetCollapseControllerType(IAnimLayerControlManager::ControllerType type);
	IAnimLayerControlManager::ControllerType GetCollapseControllerType();
	void SetCollapsePerFrame(bool v);
	bool GetCollapsePerFrame();
	void SetCollapsePerFrameActiveRange(bool v);
	bool GetCollapsePerFrameActiveRange();
	void SetCollapseRange(Interval range);
	Interval GetCollapseRange();

	void DeleteMe();

	//functions for hanlding merging in scenes that already have some layer controls on them.
	void SaveExistingMLCs();
	void RevertExistingMLCs();

	TSTR MakeLayerNameUnique(TSTR &name); //need to expose to sdk
	LayerControl *BuildLayerControl(TrackViewPick res);
	void MakeLayerControl(TrackViewPick res,LayerControl *existingLayer,INode *node);


	//UI for changing a layer name
	INT_PTR RenameLayer(HWND hWnd, int which);
	static INT_PTR CALLBACK RenameLayerProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


	static void LoadFromIni();
//Current Active Layers.  Will change based upon selection set.
	Tab<int> activeLayers;
	Tab<int> currentLayers; //current total layers 
	Tab<bool> isLayerOnAll; //true if layer is on all nodes, false if it isn't.
	//we have copy layers... again based on selection, used to drive UI.
	bool copyLayers; 
	//only one layer on the selected nodes, which means we can disable
	bool canDisable;

	
	//Layer properties saved out per scene
	bool justUpToActive; //mute all greater than active!
	IAnimLayerControlManager::ControllerType controllerType;
	bool collapseAllPerFrame;
	bool collapsePerFrameActiveRange;
	Interval collapseRange;
	bool askBeforeDeleting;

private:	//this is actually set and stored from trackview.
	bool filterActive;

	Tab<MasterLayerControl*> existingMasterLayerControls; //new for handling merging.. the clip assocations in there get deleted.. need to reset them!


	//something has changed, ususally a property and we need update all layer controllers
	void UpdateLayerControls();
	//just delete it,with no external messages.
	void DeleteNthMLC(int index);

	//after we delete or collapse a layer we need to reset the index of the layers that point to those after it,
	//so index is the value of the layer that was just deleted(collapsed).
	void ResetLayerMLCs(int index);

};

#pragma endregion
class LayerControlMotionDlg;

#define MASTERLAYER_CONTROL_CLASS_ID	Class_ID(0x225546f7, 0x75e02662)
#define MASTERLAYER_CNAME	GetString(IDS_MASTERLAYER)

#pragma region class MasterLayerControl
/* 	Master Layer Control contains
	1) references to the list that make it up.
	2) references to the nodes that have the layers...
	layers can only be set active, or be copy, or have weights set on master. */
class MasterLayerControl : public Control {		
	public:
		Tab<LayerControl*> layers;	//Pointer to all of the layers controllers that have this layer
		Tab<ReferenceTarget *> monitorNodes; //INodeMonitor reference
		IParamBlock2* pblock;
		bool addDefault; //or add current;
		bool mute;
		bool outputMute;
		DWORD paramFlags;
		
		

		MasterLayerControl(BOOL loading=FALSE);
		MasterLayerControl(const MasterLayerControl& ctrl);
		virtual ~MasterLayerControl();	
		void Init();
		
		void GetClassName(TSTR& s) {s = MASTERLAYER_CNAME;;}
		SClass_ID		SuperClassID() {return  CTRL_USERTYPE_CLASS_ID;}
		Class_ID		ClassID() {return  MASTERLAYER_CONTROL_CLASS_ID;}

		MasterLayerControl& operator=(const MasterLayerControl& ctrl);
		void Resize(int numLayers);
		float AverageWeight(float weight);	

		Control *GetDefaultControl(SClass_ID sclassID);
		Control *GetWeightControl();
		void SetWeightControl(Control *weight);


		TSTR GetName();
		void SetName(TSTR &name);
		float GetWeight(TimeValue t,Interval &valid);
		void SetWeight(TimeValue t, float weight);
		void AppendLayerControl(LayerControl *lC);
		void AddNode(INode *newNode);

		// From ReferenceTarget

		int NumSubs();
		Animatable* SubAnim(int i);
		TSTR SubAnimName(int i);
		int SubNumToRefNum(int subNum);

		int	NumParamBlocks() { return 1; }					// return number of ParamBlocks in this instance
		IParamBlock2* GetParamBlock(int i) { return pblock; } // return i'th ParamBlock
		IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; } // return id'd ParamBlock

		int NumRefs() {return layers.Count()+monitorNodes.Count()+1;};	//numControllers+monitorNodes +pblock 
		RefTargetHandle GetReference(int i);
private:
		virtual void SetReference(int i, RefTargetHandle rtarg);		
public:
		//RefTargetHandle Clone(RemapDir& remap);
		RefResult NotifyRefChanged(Interval, RefTargetHandle, PartID&, RefMessage);
		void NotifyForeground(TimeValue t);
		IOResult Save(ISave *isave);
		IOResult Load(ILoad *iload);
		void DeleteThis() {delete this;}
		//From Control
		void Control::Copy(Control *){};
		void GetValue(TimeValue,void *,Interval &,GetSetMethod){};
		void SetValue(TimeValue,void *,int,GetSetMethod){};
		BOOL CanAssignController(int subAnim){return FALSE;}
		BOOL CanCopyAnim() {return FALSE;}
		BOOL CanMakeUnique(){return FALSE;}
		BOOL IsReplaceable() { return FALSE;}	
		BOOL CanApplyEaseMultCurves() { return FALSE;}
		void NotifyLayers(); //notify all layers when something has changed, in particularly we've been muted.	

		bool GetLocked(){return locked;}
		void SetLocked(bool val, bool setAll = true);  //if setAll is true we set all of the controllers in each layer also, if false, only the locked value.
		//when we get unlocked we need to make sure all the layer controllers in that layer in the scene are also unlocked
        //since the default unlocking rules won't unlock them. 
		void UnlockChildren(ReferenceTarget *anim); 
private:
		bool locked; //this is private so the locked state is set via the get/set funcgtions
		bool IsHardLocked();

};

#pragma endregion

#pragma region class LayerControl


class LayerControl : public Control, public ILayerControl, public ILockedTrackImp
{

	public:
		//the next 2 tabs should always have a 1-1 correspondance!
		Tab<int> mLCs; //the masters this guy belongs to.
		Tab<Control*> conts;	//actually controls that make it up

		IParamBlock2* pblock;

		int active; //contained individually!
		Control *copyClip;	//copy n paste.
		bool needToSetUpParentNode;		//whether nor not it was just cloned, if so we need to set up it's node

		LayerControlMotionDlg *dlg;
		DWORD paramFlags;

		LayerControl(BOOL loading=FALSE);
		LayerControl(const LayerControl& ctrl);
		virtual ~LayerControl();	
		
		LayerControl& operator=(const LayerControl& ctrl);
		void Resize(int c);
		float AverageWeight(float weight,TimeValue t,Interval &valid);	
		void CreateController(int i);//used for output controls
		TSTR ControllerName(int i);


		//callsed by the MasterLayerControl
		virtual void AddLayer(Control *control,int newMLC);
		void SetLayerActiveInternal(int active);  //need this to set up the dialog correctly for the rollouts for the active control
		virtual void DeleteLayerInternal(int active);
		virtual void CopyLayerInternal(int active);
		virtual Control* PasteLayerInternal(int active);
		virtual void CollapseLayerInternal(int index);
		void DisableLayer(Animatable *client,int subNum);



		//UI calls that do UNDO/REDO's
		void DisableLayerUI();
		void DeleteLayerUI(int index);
		void PasteLayerUI(int index);

		//virtual ClassDesc2 *GetListDesc()=0;
		virtual LayerControl *DerivedClone()=0;

		//From ILayerControl
		int	 GetLayerCount();// {return conts.Count();} //same as mLC->GetLayerCount
		void SetLayerActive(int index);//{mLC->SetLayerActive(index);}
		int  GetLayerActive();// { return mLC->GetLayerActive(); }
		void DeleteLayer(int index);//{mLC->DeleteLayer(index);}
		void CopyLayer(int index);//{mLC->CopyLayer(index);}
		void PasteLayer(int index);//{mLC->PasteLayer(index);}
		void SetLayerName(int index, TSTR name);//{mLC->SetLayerName(index,name);}
		TSTR GetLayerName(int index);//{return mLC->GetLayerName(index);}
		Control*	GetSubCtrl(int in_index) const;
		float GetLayerWeight(int index,TimeValue t);//{return mLC->GetLayerWeight(index,t);}
		void SetLayerWeight(int index,TimeValue t,float weight);//{mLC->SetLayerWeight(index,t,weight);}
		void DisableLayer();	
		void SetLayerMute(int index, bool mute);
		bool GetLayerMute(int index);
		bool GetLayerLocked(int index);
		void SetLayerLocked(int index, bool mute);

		//From FPMixinInterface
		BaseInterface* GetInterface(Interface_ID id) 
		{ 
			if (id == LAYER_CONTROLLER_INTERFACE) 
				return static_cast<ILayerControl*>(this); 
			else 
				return FPMixinInterface::GetInterface(id);
		} 

		//For LayerOutputControl
		virtual void GetOutputValue(TimeValue t, void *val, Interval &valid)=0;

		// From Control
		void Copy(Control *from);
		void CommitValue(TimeValue t);
		void RestoreValue(TimeValue t);
		BOOL IsLeaf() {return FALSE;}
		int IsKeyable();
		void SetValue(TimeValue t, void *val, int commit=1, GetSetMethod method=CTRL_ABSOLUTE);
		void EnumIKParams(IKEnumCallback &callback);
		BOOL CompDeriv(TimeValue t,Matrix3& ptm,IKDeriv& derivs,DWORD flags);
		void MouseCycleCompleted(TimeValue t);
		BOOL InheritsParentTransform();



		// From Animatable
		int NumSubs();
		Animatable* SubAnim(int i);
		TSTR SubAnimName(int i);	
		int SubNumToRefNum(int subNum);
		void DeleteThis() {delete this;}		
		void AddNewKey(TimeValue t,DWORD flags);
		void CloneSelectedKeys(BOOL offset);
		void DeleteKeys(DWORD flags);
		void SelectKeys(TrackHitTab& sel, DWORD flags);
		BOOL IsKeySelected(int index);
		void BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev);
		void EndEditParams(IObjParam *ip, ULONG flags,Animatable *next);
		ParamDimension* GetParamDimension(int i);
		BOOL AssignController(Animatable *control,int subAnim);
		void CopyKeysFromTime(TimeValue src,TimeValue dst,DWORD flags);
		void DeleteKeyAtTime(TimeValue t);
		BOOL IsKeyAtTime(TimeValue t,DWORD flags);
		BOOL GetNextKeyTime(TimeValue t,DWORD flags,TimeValue &nt);
		int GetKeyTimes(Tab<TimeValue> &times,Interval range,DWORD flags);
		int GetKeySelState(BitArray &sel,Interval range,DWORD flags);
		void EditTrackParams(
			TimeValue t,
			ParamDimensionBase *dim,
			const TCHAR *pname,
			HWND hParent,
			IObjParam *ip,
			DWORD flags);
		int TrackParamsType() {if(GetLocked()==false) return TRACKPARAMS_WHOLE; else return TRACKPARAMS_NONE;}		
	
		Interval GetTimeRange(DWORD flags);

		BOOL CanAssignController(int subAnim){ if(GetLocked()==false)return TRUE; else return FALSE;}
		BOOL CanCopyAnim() {return FALSE;}
		BOOL CanMakeUnique(){return FALSE;}
		BOOL IsReplaceable() { return FALSE;}	
		BOOL CanApplyEaseMultCurves(){ if(GetLocked()==false) return TRUE; else return FALSE;}
		void* GetInterface(ULONG id)
		{
			switch (id) {
				case I_LOCKED:
						return dynamic_cast<ILockedTrack*>( this);
				}
			return Control::GetInterface(id);
		}

		// From ReferenceTarget
		int	NumParamBlocks() { return 1; }					// return number of ParamBlocks in this instance
		IParamBlock2* GetParamBlock(int i) { return pblock; } // return i'th ParamBlock
		IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; } // return id'd ParamBlock
		RefResult AutoDelete();


		int NumRefs() {return conts.Count()+2;};	//numControllers+copyClip+pblock
		RefTargetHandle GetReference(int i);
private:
		virtual void SetReference(int i, RefTargetHandle rtarg);		
public:
		RefTargetHandle Clone(RemapDir& remap);
		RefResult NotifyRefChanged(Interval, RefTargetHandle, PartID&, RefMessage);
		void NotifyForeground(TimeValue t);
		IOResult Save(ISave *isave);
		IOResult Load(ILoad *iload);


		//functions for handling the LayerOutputControls that are created on the pblock
		void CollapseLayer(int index);
		BOOL CollapseXYZController(int index, Control *onto, Control *from);

		INode * GetNodeAndClientSubAnim(TrackViewPick &res);
		void SetParentNodesOnMLC();  //finds the node it belongs too if it's cloned.

	private:

		BOOL CollapseKeys(int index,Control *onto, Control *from);
		BOOL CollapseFramePerKey(int index,Control *onto, Interval range,bool ontoKeys);
		void *GetControllerValues(int index, Interval range, BOOL getRotAsEuler=FALSE);

		void CheckForLock(RefTargetHandle hTarget,bool lockVal); //maybe set up lock on ourselves
		int GetEndOfOutputControls();
};

#pragma endregion

#pragma region class AnimLayerToolbarControl

class AnimLayerToolbarControl :  public TimeChangeCallback
{
private:
	static HWND hSpinHWND;
	static HWND hEditHWND;
	static HWND m_hwnd;
	HWND m_parent;
	static HWND m_list;
	WNDPROC m_defWndProc;
	static WNDPROC m_defListProc;
	static WNDPROC m_defToolProc;

	// The sum of the width of the icons that are drawn in the drop down
	// This is used to set the width of the
	// drop down according to the length of its content.
	static int m_iconColWidth;

	static int m_col;
	enum Columns {
			      _kMuteCol,
				  _kOutputMuteCol,
				  _kLockedCol,
				  _kNameCol
	};

public:
	AnimLayerToolbarControl();
	~AnimLayerToolbarControl();

	void SetHWND(HWND hwnd);
	void SetParent(HWND hParent);
	void SetListBox(HWND hList);
	bool OnCommand(WPARAM wParam, LPARAM lParam);
	void SetSpinHWND(HWND hwnd){hSpinHWND = hwnd;}
	void SetEditHWND(HWND hwnd){hEditHWND = hwnd;}
	static void Refresh();

	static void SetSelected(int which);
	static HWND GetEditHWND(){return hEditHWND;}
	// from TimeChangeCallback
	void TimeChanged(TimeValue t) {EnableDisableWeight();}

public:
	static TSTR FormatName(TSTR & name, int nLen2, HWND hDlg);
protected:
	static void SetComboSize(HWND hCombo);
	static POINT HitTest(POINT p);
	static void DrawNameColumn(int which, HDC hDC, RECT r);
	static void DrawOutputMuteColumn(int which, HDC hDC, RECT r);
	static void DrawMuteColumn(int which, HDC hDC, RECT r);
	static void DrawLockedColumn(int which, HDC hdc, RECT r);
	static void OnDraw(HWND hDlg, WPARAM wParam, LPARAM lParam);
	static bool OnLButtonDown(HWND hDlg, WPARAM wParam, LPARAM lParam);
	static bool OnLButtonUp(HWND hDlg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK LayerComboControlProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK ToolbarMsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam); 
	static LRESULT CALLBACK ListBoxControlProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	static bool OnRButtonUp(HWND hDlg, WPARAM wParam, LPARAM lParam);

	static void EnableDisableWeight();
};

#pragma endregion


#endif 
