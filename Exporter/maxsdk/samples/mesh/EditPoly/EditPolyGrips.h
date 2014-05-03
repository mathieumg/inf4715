/*==============================================================================
Copyright 2009 Autodesk, Inc.  All rights reserved. 

This computer source code and related instructions and comments are the unpublished
confidential and proprietary information of Autodesk, Inc. and are protected under 
applicable copyright and trade secret law.  They may not be disclosed to, copied
or used by any third party without the prior written consent of Autodesk, Inc.

//**************************************************************************/

#include "IGrip.h"

#ifndef __EDIT_POLY_GRIPS_H
#define __EDIT_POLY_GRIPS_H

class EditPolyMod;
class PolyOperation;

//icon defines

#define STR_ICON_DIR					_T("Grips\\")
#define BEVEL_TYPE_PNG					_T("Bevel Type.png")
#define BEVEL_GROUP_PNG					_T("Group.png")
#define BEVEL_NORMAL_PNG				_T("Normal.png")
#define BEVEL_POLYGON_PNG				_T("Polygon.png")
#define BEVEL_HEIGHT_PNG				_T("Height.png")
#define BEVEL_OUTLINE_PNG				_T("Outline Amount.png")
#define EXTRUDE_TYPE_PNG				_T("Extrude Type.png")
#define EXTRUDE_GROUP_PNG				_T("Group.png")
#define EXTRUDE_NORMAL_PNG				_T("Normal.png")
#define EXTRUDE_POLYGON_PNG				_T("Polygon.png")
#define EXTRUDE_HEIGHT_PNG				_T("Height.png")
#define EXTRUDE_WIDTH_PNG				_T("Width.png")
#define OUTLINE_AMOUNT_PNG				_T("Outline Amount.png")
#define INSET_TYPE_PNG                  _T("Inset Type.png")
#define INSET_AMOUNT_PNG				_T("Inset Amount.png")
#define INSET_GROUP_PNG					_T("Group.png")
#define INSET_POLYGON_PNG				_T("Polygon.png")
#define CONNECT_EDGE_SEGMENTS_PNG  _T("segments.png")
#define CONNECT_EDGE_PINCH_PNG        _T("segmentPinch.png")
#define CONNECT_EDGE_SLIDE_PNG         _T("segmentSlide.png")
#define RELAX_AMOUNT_PNG                  _T("Value.png")
#define RELAX_ITERATIONS_PNG             _T("relaxIteration.png")
#define RELAX_BOUNDARY_POINTS_PNG             _T("holdBoundary.png")
#define RELAX_OUTER_POINTS_PNG             _T("holdOuterPoints.png")
#define CHAMFER_VERTICES_AMOUNT_PNG   _T("Value.png")
#define CHAMFER_VERTICES_OPEN_PNG   _T("hole.png")
#define CHAMFER_EDGE_AMOUNT_PNG         _T("Value.png")
#define CHAMFER_EDGE_SEGMENT_PNG        _T("segments.png")
#define CHAMFER_EDGE_OPEN_PNG        _T("hole.png")
#define BRIDGE_BORDER_TYPE_PNG                _T("byEdge.png")
#define BRIDGE_POLYGON_TYPE_PNG                _T("byPolygon.png")
#define BRIDGE_EDGE_TYPE_PNG                _T("byEdge.png")
#define BRIDGE_SELECTED_PNG          _T("selected.png")
#define BRIDGE_SPECIFIC_PNG           _T("specific.png")
#define BRIDGE_SEGMENTS_PNG        _T("segments.png")
#define BRIDGE_TAPER_PNG              _T("taper.png")
#define BRIDGE_TWIST1_PNG             _T("twist1.png")
#define BRIDGE_TWIST2_PNG             _T("twist2.png")
#define BRIDGE_BIAS_PNG                _T("bias.png")
#define BRIDGE_SMOOTH_PNG          _T("smoothMesh.png")
#define BRIDGE_ADJACENT_PNG        _T("bridgeAdjacent_16.png")
#define BRIDGE_REVERSE_TRI_PNG    _T("reverseTri.png")
#define BRIDGE_EDGE_PICK1_PNG          _T("BridgeEdgePick1.png")
#define BRIDGE_EDGE_PICK2_PNG           _T("BridgeEdgePick2.png")
#define BRIDGE_POLYGON_PICK1_PNG		_T("BridgePolygonPick1.png")
#define BRIDGE_POLYGON_PICK2_PNG		_T("BridgePolygonPick2.png")
#define BRIDGE_BORDER_PICK1_PNG		_T("BridgeBorderPick1.png")
#define BRIDGE_BORDER_PICK2_PNG		_T("BridgeBorderPick2.png")
#define HINGE_FROM_EDGE_ANGLE_PNG    _T("angle_16.png")
#define HINGE_FROM_EDGE_SEGMENTS_PNG    _T("segments.png")
#define HINGE_PICK_PNG		           _T("HingePick.png")
#define EXTRUDE_ALONG_SPLINE_SEGMENTS_PNG   _T("segments.png")
#define EXTRUDE_ALONG_SPLINE_TAPER_AMOUNT_PNG   _T("Value.png")
#define EXTRUDE_ALONG_SPLINE_TAPER_CURVE_PNG    _T("taperCurve.png")
#define EXTRUDE_ALONG_SPLINE_TWIST_PNG              _T("twist.png")
#define EXTRUDE_ALONG_SPLINE_ROTATION_PNG         _T("angle_16.png")
#define EXTRUDE_ALONG_SPLINE_TO_FACE_NORMAL_PNG         _T("localNormal.png")
#define EXTRUDE_ALONG_SPLINE_PICK_PNG           _T("ExtrudeAlongSplinePick.png")
#define WELD_THRESHOLD_PNG                      _T("Value.png")
#define MSMOOTH_SMOOTHNESS_PNG                  _T("smoothMesh.png")
#define MSMOOTH_SMOOTHING_GROUPS_PNG                  _T("smoothGroups.png")
#define MSMOOTH_SMOOTH_MATERIALS_PNG                  _T("smoothMaterials.png")
#define TESSELLATE_EDGE_PNG                           _T("byEdge.png")
#define  TESSELLATE_FACE_PNG                          _T("byPolygon.png")
#define  TESSELLATE_TYPE_PNG                          _T("Group.png")
#define  TESSELLATE_TENSION_PNG                     _T("Value.png")
#define PICK_EMPTY_PNG						_T("PickButton Empty.png")
#define PICK_FULL_PNG						_T("PickButton Full.png")
#define FALLOFF_PNG							_T("falloff.png")
#define BUBBLE_PNG							_T("bubble.png")
#define PINCH_PNG							_T("pinch.png")
#define SETFLOW_PNG							_T("setFlow.png")
#define LOOPSHIFT_PNG							_T("loopshift.png")
#define RINGSHIFT_PNG							_T("ringShift.png")


class EPManipulatorGrip : public IBaseGrip
{
	
public:
	enum OurGrips{eFalloff = 0,  ePinch, eBubble,eSetFlow, eLoopShift,
					eRingShift, eEdgeCrease, eEdgeWeight,  eVertexWeight};

	EPManipulatorGrip():mpEditPoly(NULL){};
	virtual ~EPManipulatorGrip(){};
    void Init(EditPolyMod* editPoly){mpEditPoly = editPoly;}
	EditPolyMod*  GetEditPolyMod() { return mpEditPoly; }
	void SetEditPolyMod( EditPolyMod* pEdPolyMod ){ mpEditPoly = pEdPolyMod; }
	void SetUpUI(){GetIGripManager()->ResetAllUI();}  //called after the grip is enabled to set up init states that are called via IGripManager
	void SetUpVisibility(BitArray &manipMask);  //should be called after it's enabled we will reset everything
	void SetFalloff(bool val){ mSSFalloff = val; ResetGrip();}
	void SetBubble(bool val){ mSSBubble = val; ResetGrip();}
	void SetPinch(bool val){ mSSPinch = val; ResetGrip();}
	void SetSetFlow(bool val) {mSetFlow = val; ResetGrip();}
	void SetLoophift(bool val){mLoopShift = val; ResetGrip();}
	void SetRingShift(bool val) {mRingShift = val; ResetGrip();}

	void ResetGrip();

	//from IBaseGrip
	void Okay(TimeValue t);
	void Cancel();
	void Apply(TimeValue t);
	bool SupportsOkayApplyCancel(){return false;} //from IBaseGrip, we don't support ok,apply, cancel buttons
	bool GetCommandIcon(int which, MSTR &string){return false;}

	int GetNumGripItems();
	IBaseGrip::Type GetType(int which);
	void GetGripName(TSTR &string);
	bool GetText(int which,TSTR &string);
	bool GetResolvedIconName(int which,MSTR &string);
	DWORD GetCustomization(int which);
	bool GetComboOptions(int which, Tab<IBaseGrip::ComboLabel*> &radioOptions);

	bool GetValue(int which,TimeValue t,IBaseGrip::GripValue &value);
	bool SetValue(int which,TimeValue t,IBaseGrip::GripValue &value); 

	bool GetAutoScale(int which);
	bool GetScaleInViewSpace(int which, float &depth);
	bool GetScale(int which, IBaseGrip::GripValue &scaleValue);
	bool GetResetValue(int which,IBaseGrip::GripValue &resetValue);
	bool StartSetValue(int which,TimeValue t);
	bool EndSetValue(int which,TimeValue t,bool accepted);
	bool ResetValue(TimeValue t,int which);
	bool GetRange(int which,IBaseGrip::GripValue &minRange, IBaseGrip::GripValue &maxRange);
	bool ShowKeyBrackets(int which,TimeValue t);

private:
	EditPolyMod* mpEditPoly;
	bool mSSFalloff;
	bool mSSBubble;
	bool mSSPinch;
	bool mSetFlow;
	bool mLoopShift;
	bool mRingShift;
};

class EditPolyModPolyOperationGrip : public IBaseGrip
{
public:
	EditPolyModPolyOperationGrip();
	virtual ~EditPolyModPolyOperationGrip();
    virtual void Init(EditPolyMod* editPoly);
	EditPolyMod*  GetEditPolyMod();
	virtual void SetUpUI();
	bool SupportsOkayApplyCancel();
protected:
	void CommonOK( TimeValue t );
	void CommonApply( TimeValue t );
	void CommonCancel();
private:
	EditPolyMod* mpEditPoly;
	bool		 mReentryLock; // protect against re-entery into CommonOk/CommonApply/CommonCancel
};

class BevelGrip :public EditPolyModPolyOperationGrip
{
public:

	enum OurGrips{eBevelType = 0, eBevelHeight, eBevelOutline};

	BevelGrip(){};
	BevelGrip(EditPolyMod *m, PolyOperation *o);
	~BevelGrip();

	void Okay(TimeValue t);
	void Cancel();
	void Apply(TimeValue t);
	int GetNumGripItems();
	IBaseGrip::Type GetType(int which);
	void GetGripName(TSTR &string);
	bool GetText(int which,TSTR &string);
	bool GetResolvedIconName(int which,MSTR &string);
	DWORD GetCustomization(int which);
	bool GetComboOptions(int which, Tab<IBaseGrip::ComboLabel*> &radioOptions);
	bool GetCommandIcon(int which, MSTR &string); 

	bool GetValue(int which,TimeValue t,IBaseGrip::GripValue &value);
	bool SetValue(int which,TimeValue t,IBaseGrip::GripValue &value); 

	bool GetAutoScale(int which);
	bool GetScaleInViewSpace(int which, float &depth);
	bool GetScale(int which, IBaseGrip::GripValue &scaleValue);
	bool GetResetValue(int which,IBaseGrip::GripValue &resetValue);
	bool StartSetValue(int which,TimeValue t);
	bool EndSetValue(int which,TimeValue t,bool accepted);
	bool ResetValue(TimeValue t,int which);
	bool GetRange(int which,IBaseGrip::GripValue &minRange, IBaseGrip::GripValue &maxRange);
	bool ShowKeyBrackets(int which,TimeValue t);

};

class ExtrudeFaceGrip :public EditPolyModPolyOperationGrip
{
public:

	enum OurGrips{eExtrudeFaceType = 0, eExtrudeFaceHeight};

	ExtrudeFaceGrip(){};
	ExtrudeFaceGrip(EditPolyMod *m, PolyOperation *o);
	~ExtrudeFaceGrip();

	void Okay(TimeValue t);
	void Cancel();
	void Apply(TimeValue t);
	int GetNumGripItems();
	IBaseGrip::Type GetType(int which);
	void GetGripName(TSTR &string);
	bool GetText(int which,TSTR &string);
	bool GetResolvedIconName(int which,MSTR &string);
	DWORD GetCustomization(int which);
	bool GetComboOptions(int which, Tab<IBaseGrip::ComboLabel*> &radioOptions);
	bool GetCommandIcon(int which, MSTR &string); 

	bool GetValue(int which,TimeValue t,IBaseGrip::GripValue &value);
	bool SetValue(int which,TimeValue t,IBaseGrip::GripValue &value); 

	bool GetAutoScale(int which);
	bool GetScaleInViewSpace(int which, float &depth);
	bool GetScale(int which, IBaseGrip::GripValue &scaleValue);
	bool GetResetValue(int which,IBaseGrip::GripValue &resetValue);
	bool StartSetValue(int which,TimeValue t);
	bool EndSetValue(int which,TimeValue t,bool accepted);
	bool ResetValue(TimeValue t,int which);
	bool GetRange(int which,IBaseGrip::GripValue &minRange, IBaseGrip::GripValue &maxRange);
	bool ShowKeyBrackets(int which,TimeValue t);

};

class ExtrudeEdgeGrip :public EditPolyModPolyOperationGrip
{
public:

	enum OurGrips{eExtrudeEdgeHeight = 0, eExtrudeEdgeWidth};

	ExtrudeEdgeGrip(){};
	ExtrudeEdgeGrip(EditPolyMod *m, PolyOperation *o);
	~ExtrudeEdgeGrip();
	
	void Okay(TimeValue t);
	void Cancel();
	void Apply(TimeValue t);
	int GetNumGripItems();
	IBaseGrip::Type GetType(int which);
	void GetGripName(TSTR &string);
	bool GetText(int which,TSTR &string);
	bool GetResolvedIconName(int which,MSTR &string);
	DWORD GetCustomization(int which);
	bool GetComboOptions(int which, Tab<IBaseGrip::ComboLabel*> &radioOptions);
	bool GetCommandIcon(int which, MSTR &string); 

	bool GetValue(int which,TimeValue t,IBaseGrip::GripValue &value);
	bool SetValue(int which,TimeValue t,IBaseGrip::GripValue &value); 

	bool GetAutoScale(int which);
	bool GetScaleInViewSpace(int which, float &depth);
	bool GetScale(int which, IBaseGrip::GripValue &scaleValue);
	bool GetResetValue(int which,IBaseGrip::GripValue &resetValue);
	bool StartSetValue(int which,TimeValue t);
	bool EndSetValue(int which,TimeValue t,bool accepted);
	bool ResetValue(TimeValue t,int which);
	bool GetRange(int which,IBaseGrip::GripValue &minRange, IBaseGrip::GripValue &maxRange);
	bool ShowKeyBrackets(int which,TimeValue t);

};

class ExtrudeVertexGrip :public EditPolyModPolyOperationGrip
{
public:

	enum OurGrips{eExtrudeVertexHeight = 0, eExtrudeVertexWidth};

	ExtrudeVertexGrip(){};
	ExtrudeVertexGrip(EditPolyMod *m, PolyOperation *o);
	~ExtrudeVertexGrip();
	
	void Okay(TimeValue t);
	void Cancel();
	void Apply(TimeValue t);
	int GetNumGripItems();
	IBaseGrip::Type GetType(int which);
	void GetGripName(TSTR &string);
    bool GetText(int which,TSTR &string);
	bool GetResolvedIconName(int which,MSTR &string);
	DWORD GetCustomization(int which);

	bool GetValue(int which,TimeValue t,IBaseGrip::GripValue &value);
	bool SetValue(int which,TimeValue t,IBaseGrip::GripValue &value); 
	bool GetComboOptions(int which, Tab<IBaseGrip::ComboLabel*> &radioOptions);
	bool GetCommandIcon(int which, MSTR &string); 

	bool GetAutoScale(int which);
	bool GetScaleInViewSpace(int which, float &depth);
	bool GetScale(int which, IBaseGrip::GripValue &scaleValue);
	bool GetResetValue(int which,IBaseGrip::GripValue &resetValue);
	bool StartSetValue(int which,TimeValue t);
	bool EndSetValue(int which,TimeValue t,bool accepted);
	bool ResetValue(TimeValue t,int which);
	bool GetRange(int which,IBaseGrip::GripValue &minRange, IBaseGrip::GripValue &maxRange);
	bool ShowKeyBrackets(int which,TimeValue t);

};

class OutlineGrip :public EditPolyModPolyOperationGrip
{
public:

	enum OurGrips{eOutlineAmount = 0};

	OutlineGrip(){};
	OutlineGrip(EditPolyMod *m, PolyOperation *o);
	~OutlineGrip();

	void Okay(TimeValue t);
	void Cancel();
	void Apply(TimeValue t);
	int GetNumGripItems();
	IBaseGrip::Type GetType(int which);
	void GetGripName(TSTR &string);
	bool GetText(int which,TSTR &string);
	bool GetResolvedIconName(int which,MSTR &string);
	DWORD GetCustomization(int which);

	bool GetValue(int which,TimeValue t,IBaseGrip::GripValue &value);
	bool SetValue(int which,TimeValue t,IBaseGrip::GripValue &value); 
	bool GetComboOptions(int which, Tab<IBaseGrip::ComboLabel*> &radioOptions);
	bool GetCommandIcon(int which, MSTR &string); 
	bool GetAutoScale(int which);
	bool GetScaleInViewSpace(int which, float &depth);
	bool GetScale(int which, IBaseGrip::GripValue &scaleValue);
	bool GetResetValue(int which,IBaseGrip::GripValue &resetValue);
	bool StartSetValue(int which,TimeValue t);
	bool EndSetValue(int which,TimeValue t,bool accepted);
	bool ResetValue(TimeValue t,int which);
	bool GetRange(int which,IBaseGrip::GripValue &minRange, IBaseGrip::GripValue &maxRange);
	bool ShowKeyBrackets(int which,TimeValue t);

};

class InsetGrip :public EditPolyModPolyOperationGrip
{
public:

	enum OurGrips{eInsetType = 0, eInsetAmount};

	InsetGrip(){};
	InsetGrip(EditPolyMod *m, PolyOperation *o);
	~InsetGrip();

	void Okay(TimeValue t);
	void Cancel();
	void Apply(TimeValue t);
	int GetNumGripItems();
	IBaseGrip::Type GetType(int which);
	void GetGripName(TSTR &string);
	bool GetText(int which,TSTR &string);
	bool GetResolvedIconName(int which,MSTR &string);
	DWORD GetCustomization(int which);
	bool GetComboOptions(int which, Tab<IBaseGrip::ComboLabel*> &radioOptions);
	bool GetCommandIcon(int which, MSTR &string); 
	bool GetValue(int which,TimeValue t,IBaseGrip::GripValue &value);
	bool SetValue(int which,TimeValue t,IBaseGrip::GripValue &value); 

	bool GetAutoScale(int which);
	bool GetScaleInViewSpace(int which, float &depth);
	bool GetScale(int which, IBaseGrip::GripValue &scaleValue);
	bool GetResetValue(int which,IBaseGrip::GripValue &resetValue);
	bool StartSetValue(int which,TimeValue t);
	bool EndSetValue(int which,TimeValue t,bool accepted);
	bool ResetValue(TimeValue t,int which);
	bool GetRange(int which,IBaseGrip::GripValue &minRange, IBaseGrip::GripValue &maxRange);
	bool ShowKeyBrackets(int which,TimeValue t);

};

class ConnectEdgeGrip :public EditPolyModPolyOperationGrip
{
public:

	enum OurGrips{eConnectEdgeSegments = 0, eConnectEdgePinch, eConnectEdgeSlide};

	ConnectEdgeGrip(){};
	ConnectEdgeGrip(EditPolyMod *m, PolyOperation *o);
	~ConnectEdgeGrip();

	void Okay(TimeValue t);
	void Cancel();
	void Apply(TimeValue t);
	
	int GetNumGripItems();
	IBaseGrip::Type GetType(int which);
	void GetGripName(TSTR &string);
	bool GetText(int which,TSTR &string);
	bool GetResolvedIconName(int which,MSTR &string);
	DWORD GetCustomization(int which);
	bool GetComboOptions(int which, Tab<IBaseGrip::ComboLabel*> &radioOptions);
	bool GetCommandIcon(int which, MSTR &string); 
	bool GetValue(int which,TimeValue t,IBaseGrip::GripValue &value);
	bool SetValue(int which,TimeValue t,IBaseGrip::GripValue &value); 

	bool GetAutoScale(int which);
	bool GetScaleInViewSpace(int which, float &depth);
	bool GetScale(int which, IBaseGrip::GripValue &scaleValue);
	bool GetResetValue(int which,IBaseGrip::GripValue &resetValue);
	bool StartSetValue(int which,TimeValue t);
	bool EndSetValue(int which,TimeValue t,bool accepted);
	bool ResetValue(TimeValue t,int which);
	bool GetRange(int which,IBaseGrip::GripValue &minRange, IBaseGrip::GripValue &maxRange);
	bool ShowKeyBrackets(int which,TimeValue t);

};

class RelaxGrip :public EditPolyModPolyOperationGrip
{
public:

	enum OurGrips{eRelaxAmount = 0, eRelaxIterations, eRelaxBoundaryPoints,eRelaxOuterPoints};

	RelaxGrip(){};
	RelaxGrip(EditPolyMod *m, PolyOperation *o);
	~RelaxGrip();

	void Okay(TimeValue t);
	void Cancel();
	void Apply(TimeValue t);
	int GetNumGripItems();
	IBaseGrip::Type GetType(int which);
	void GetGripName(TSTR &string);
	bool GetText(int which,TSTR &string);
	bool GetResolvedIconName(int which,MSTR &string);
	DWORD GetCustomization(int which);
	bool GetComboOptions(int which, Tab<IBaseGrip::ComboLabel*> &radioOptions);
	bool GetCommandIcon(int which, MSTR &string); 
	bool GetValue(int which,TimeValue t,IBaseGrip::GripValue &value);
	bool SetValue(int which,TimeValue t,IBaseGrip::GripValue &value); 

	bool GetAutoScale(int which);
	bool GetScaleInViewSpace(int which, float &depth);
	bool GetScale(int which, IBaseGrip::GripValue &scaleValue);
	bool GetResetValue(int which,IBaseGrip::GripValue &resetValue);
	bool StartSetValue(int which,TimeValue t);
	bool EndSetValue(int which,TimeValue t,bool accepted);
	bool ResetValue(TimeValue t,int which);
	bool GetRange(int which,IBaseGrip::GripValue &minRange, IBaseGrip::GripValue &maxRange);
	bool ShowKeyBrackets(int which,TimeValue t);

};

class ChamferVerticesGrip :public EditPolyModPolyOperationGrip
{
public:

	enum OurGrips{eChamferVerticesAmount = 0, eChamferVerticesOpen };

	ChamferVerticesGrip(){};
	ChamferVerticesGrip(EditPolyMod *m, PolyOperation *o);
	~ChamferVerticesGrip();

	void Okay(TimeValue t);
	void Cancel();
	void Apply(TimeValue t);
	int GetNumGripItems();
	IBaseGrip::Type GetType(int which);
	void GetGripName(TSTR &string);
	bool GetText(int which,TSTR &string);
	bool GetResolvedIconName(int which,MSTR &string);
	DWORD GetCustomization(int which);
	bool GetComboOptions(int which, Tab<IBaseGrip::ComboLabel*> &radioOptions);
	bool GetCommandIcon(int which, MSTR &string); 
	bool GetValue(int which,TimeValue t,IBaseGrip::GripValue &value);
	bool SetValue(int which,TimeValue t,IBaseGrip::GripValue &value); 

	bool GetAutoScale(int which);
	bool GetScaleInViewSpace(int which, float &depth);
	bool GetScale(int which, IBaseGrip::GripValue &scaleValue);
	bool GetResetValue(int which,IBaseGrip::GripValue &resetValue);
	bool StartSetValue(int which,TimeValue t);
	bool EndSetValue(int which,TimeValue t,bool accepted);
	bool ResetValue(TimeValue t,int which);
	bool GetRange(int which,IBaseGrip::GripValue &minRange, IBaseGrip::GripValue &maxRange);
	bool ShowKeyBrackets(int which,TimeValue t);

};

class ChamferEdgeGrip :public EditPolyModPolyOperationGrip
{
public:

	enum OurGrips{eChamferEdgeAmount = 0, eChamferEdgeSegments, eChamferEdgeOpen};

	ChamferEdgeGrip(){};
	ChamferEdgeGrip(EditPolyMod *m, PolyOperation *o);
	~ChamferEdgeGrip();

	void Okay(TimeValue t);
	void Cancel();
	void Apply(TimeValue t);
	int GetNumGripItems();
	IBaseGrip::Type GetType(int which);
	void GetGripName(TSTR &string);
	bool GetText(int which,TSTR &string);
	bool GetResolvedIconName(int which,MSTR &string);
	DWORD GetCustomization(int which);
	bool GetComboOptions(int which, Tab<IBaseGrip::ComboLabel*> &radioOptions);
	bool GetCommandIcon(int which, MSTR &string); 
	bool GetValue(int which,TimeValue t,IBaseGrip::GripValue &value);
	bool SetValue(int which,TimeValue t,IBaseGrip::GripValue &value); 

	bool GetAutoScale(int which);
	bool GetScaleInViewSpace(int which, float &depth);
	bool GetScale(int which, IBaseGrip::GripValue &scaleValue);
	bool GetResetValue(int which,IBaseGrip::GripValue &resetValue);
	bool StartSetValue(int which,TimeValue t);
	bool EndSetValue(int which,TimeValue t,bool accepted);
	bool ResetValue(TimeValue t,int which);
	bool GetRange(int which,IBaseGrip::GripValue &minRange, IBaseGrip::GripValue &maxRange);
	bool ShowKeyBrackets(int which,TimeValue t);

};

class BridgeEdgeGrip :public EditPolyModPolyOperationGrip
{
public:

	enum OurGrips{
		eBridgeEdgeSegments = 0, 
		eBridgeEdgeSmooth, 
		eBridgeEdgeAdjacent,
		eBridgeEdgeReverseTri,
		eBridgeEdgeType,
		eBridgeEdgePick1,
		eBridgeEdgePick2
	};

	BridgeEdgeGrip(){};
	BridgeEdgeGrip(EditPolyMod *m, PolyOperation *o);
	~BridgeEdgeGrip();

	void Okay(TimeValue t);
	void Cancel();
	void Apply(TimeValue t);
	int GetNumGripItems();
	IBaseGrip::Type GetType(int which);
	void GetGripName(TSTR &string);
	bool GetText(int which,TSTR &string);
	bool GetResolvedIconName(int which,MSTR &string);
	DWORD GetCustomization(int which);
	bool GetComboOptions(int which, Tab<IBaseGrip::ComboLabel*> &radioOptions);
	bool GetCommandIcon(int which, MSTR &string); 
	bool GetValue(int which,TimeValue t,IBaseGrip::GripValue &value);
	bool SetValue(int which,TimeValue t,IBaseGrip::GripValue &value); 

	bool GetAutoScale(int which);
	bool GetScaleInViewSpace(int which, float &depth);
	bool GetScale(int which, IBaseGrip::GripValue &scaleValue);
	bool GetResetValue(int which,IBaseGrip::GripValue &resetValue);
	bool StartSetValue(int which,TimeValue t);
	bool EndSetValue(int which,TimeValue t,bool accepted);
	bool ResetValue(TimeValue t,int which);
	bool GetRange(int which,IBaseGrip::GripValue &minRange, IBaseGrip::GripValue &maxRange);
	bool ShowKeyBrackets(int which,TimeValue t);

	//need to activate grips
	void SetUpUI(); //called after the grip is enabled to set up init states that are called via IGripManager

	void SetEdge1PickModeStarted();
	void SetEdge1Picked(int edge);
	void SetEdge1PickDisabled();
	void SetEdge2PickModeStarted();
	void SetEdge2Picked(int edge);
	void SetEdge2PickDisabled();

private:
	MSTR mEdge1Picked;
	MSTR mEdge2Picked;
	MSTR mPick1Icon;
	MSTR mPick2Icon;

};

class BridgeBorderGrip :public EditPolyModPolyOperationGrip
{
public:

	enum OurGrips{
		eBridgeBorderSegments = 0, 
		eBridgeBorderTaper,
		eBridgeBorderBias,
		eBridgeBorderSmooth, 
		eBridgeBorderTwist1,
		eBridgeBorderTwist2,
		eBridgeBorderType,
		eBridgeBorderPick1,
		eBridgeBorderPick2
	};

	BridgeBorderGrip(){};
	BridgeBorderGrip(EditPolyMod *m, PolyOperation *o);
	~BridgeBorderGrip();

	void Okay(TimeValue t);
	void Cancel();
	void Apply(TimeValue t);
	int GetNumGripItems();
	IBaseGrip::Type GetType(int which);
	void GetGripName(TSTR &string);
	bool GetText(int which,TSTR &string);
	bool GetResolvedIconName(int which,MSTR &string);
	DWORD GetCustomization(int which);
	bool GetComboOptions(int which, Tab<IBaseGrip::ComboLabel*> &radioOptions);
	bool GetCommandIcon(int which, MSTR &string); 
	bool GetValue(int which,TimeValue t,IBaseGrip::GripValue &value);
	bool SetValue(int which,TimeValue t,IBaseGrip::GripValue &value); 

	bool GetAutoScale(int which);
	bool GetScaleInViewSpace(int which, float &depth);
	bool GetScale(int which, IBaseGrip::GripValue &scaleValue);
	bool GetResetValue(int which,IBaseGrip::GripValue &resetValue);
	bool StartSetValue(int which,TimeValue t);
	bool EndSetValue(int which,TimeValue t,bool accepted);
	bool ResetValue(TimeValue t,int which);
	bool GetRange(int which,IBaseGrip::GripValue &minRange, IBaseGrip::GripValue &maxRange);
	bool ShowKeyBrackets(int which,TimeValue t);

	//need to activate grips
	void SetUpUI(); //called after the grip is enabled to set up init states that are called via IGripManager


	void SetEdge1PickModeStarted();
	void SetEdge1Picked(int edge);
	void SetEdge1PickDisabled();
	void SetEdge2PickModeStarted();
	void SetEdge2Picked(int edge);
	void SetEdge2PickDisabled();

private:
	MSTR mEdge1Picked;
	MSTR mEdge2Picked;
	MSTR mPick1Icon;
	MSTR mPick2Icon;

};

class BridgePolygonGrip :public EditPolyModPolyOperationGrip
{
public:

	enum OurGrips{
		eBridgePolygonSegments = 0, 
		eBridgePolygonTaper,
		eBridgePolygonBias,
		eBridgePolygonSmooth, 
		eBridgePolygonTwist1,
		eBridgePolygonTwist2,
		eBridgePolygonType,
		eBridgePolygonPick1,
		eBridgePolygonPick2
	};

	BridgePolygonGrip(){};
	BridgePolygonGrip(EditPolyMod *m, PolyOperation *o);
	~BridgePolygonGrip();

	void Okay(TimeValue t);
	void Cancel();
	void Apply(TimeValue t);
	int GetNumGripItems();
	IBaseGrip::Type GetType(int which);
	void GetGripName(TSTR &string);
	bool GetText(int which,TSTR &string);
	bool GetResolvedIconName(int which,MSTR &string);
	DWORD GetCustomization(int which);
	bool GetComboOptions(int which, Tab<IBaseGrip::ComboLabel*> &radioOptions);
	bool GetCommandIcon(int which, MSTR &string); 
	bool GetValue(int which,TimeValue t,IBaseGrip::GripValue &value);
	bool SetValue(int which,TimeValue t,IBaseGrip::GripValue &value); 

	bool GetAutoScale(int which);
	bool GetScaleInViewSpace(int which, float &depth);
	bool GetScale(int which, IBaseGrip::GripValue &scaleValue);
	bool GetResetValue(int which,IBaseGrip::GripValue &resetValue);
	bool StartSetValue(int which,TimeValue t);
	bool EndSetValue(int which,TimeValue t,bool accepted);
	bool ResetValue(TimeValue t,int which);
	bool GetRange(int which,IBaseGrip::GripValue &minRange, IBaseGrip::GripValue &maxRange);
	bool ShowKeyBrackets(int which,TimeValue t);

	//need to activate grips
	void SetUpUI(); //called after the grip is enabled to set up init states that are called via IGripManager

	void SetPoly1PickModeStarted();
	void SetPoly1Picked(int num);
	void SetPoly1PickDisabled();
	void SetPoly2PickModeStarted();
	void SetPoly2Picked(int num);
	void SetPoly2PickDisabled();

private:
	MSTR mPoly1Picked;
	MSTR mPoly2Picked;
	MSTR mPick1Icon;
	MSTR mPick2Icon;

};

class HingeGrip :public EditPolyModPolyOperationGrip
{
public:

	enum OurGrips{eHingeAngle = 0, eHingeSegments, eHingePick};

	HingeGrip():mEdgePicked(_T("")),mPickIcon(_T("")),i(0){};
	HingeGrip(EditPolyMod *m, PolyOperation *o);
	~HingeGrip();

	void Okay(TimeValue t);
	void Cancel();
	void Apply(TimeValue t);
	int GetNumGripItems();
	IBaseGrip::Type GetType(int which);
	void GetGripName(TSTR &string);
	bool GetText(int which,TSTR &string);
	bool GetResolvedIconName(int which,MSTR &string);
	DWORD GetCustomization(int which);
	bool GetComboOptions(int which, Tab<IBaseGrip::ComboLabel*> &radioOptions);
	bool GetCommandIcon(int which, MSTR &string); 
	bool GetValue(int which,TimeValue t,IBaseGrip::GripValue &value);
	bool SetValue(int which,TimeValue t,IBaseGrip::GripValue &value); 

	bool GetAutoScale(int which);
	bool GetScaleInViewSpace(int which, float &depth);
	bool GetScale(int which, IBaseGrip::GripValue &scaleValue);
	bool GetResetValue(int which,IBaseGrip::GripValue &resetValue);
	bool StartSetValue(int which,TimeValue t);
	bool EndSetValue(int which,TimeValue t,bool accepted);
	bool ResetValue(TimeValue t,int which);
	bool GetRange(int which,IBaseGrip::GripValue &minRange, IBaseGrip::GripValue &maxRange);
	bool ShowKeyBrackets(int which,TimeValue t);

	void SetUpUI();

	void SetEdgePickModeStarted();
	void SetEdgePicked(int edge);
	void SetEdgePickDisabled();

private:
	MSTR mEdgePicked;
	MSTR mPickIcon;
	ModContextList mList;
	INodeTab mNodes;
	int i;

};

class ExtrudeAlongSplineGrip :public EditPolyModPolyOperationGrip
{
public:

	enum OurGrips{eExtrudeAlongSplineSegments = 0, eExtrudeAlongSplineTaperAmount, eExtrudeAlongSplineTaperCurve,
		eExtrudeAlongSplineTwist, eExtrudeAlongSplineAlignToFaceNormal, eExtrudeAlongSplineRotation,eExtrudeAlongSplinePickSpline};

	ExtrudeAlongSplineGrip(){};
	ExtrudeAlongSplineGrip(EditPolyMod *m, PolyOperation *o);
	~ExtrudeAlongSplineGrip();

	void Okay(TimeValue t);
	void Cancel();
	void Apply(TimeValue t);
	int GetNumGripItems();
	IBaseGrip::Type GetType(int which);
	void GetGripName(TSTR &string);
	bool GetText(int which,TSTR &string);
	bool GetResolvedIconName(int which,MSTR &string);
	DWORD GetCustomization(int which);
	bool GetComboOptions(int which, Tab<IBaseGrip::ComboLabel*> &radioOptions);
	bool GetCommandIcon(int which, MSTR &string); 
	bool GetValue(int which,TimeValue t,IBaseGrip::GripValue &value);
	bool SetValue(int which,TimeValue t,IBaseGrip::GripValue &value); 

	bool GetAutoScale(int which);
	bool GetScaleInViewSpace(int which, float &depth);
	bool GetScale(int which, IBaseGrip::GripValue &scaleValue);
	bool GetResetValue(int which,IBaseGrip::GripValue &resetValue);
	bool StartSetValue(int which,TimeValue t);
	bool EndSetValue(int which,TimeValue t,bool accepted);
	bool ResetValue(TimeValue t,int which);
	bool GetRange(int which,IBaseGrip::GripValue &minRange, IBaseGrip::GripValue &maxRange);
	bool ShowKeyBrackets(int which,TimeValue t);

	void SetSplinePickModeStarted();
	void SetSplinePicked(INode *node);
	void SetSplinePickDisabled();

	//need to activate grips
	void SetUpUI(); //called after the grip is enabled to set up init states that are called via IGripManager

private:
	MSTR mSplinePicked;
	MSTR mPickIcon;

};

class WeldVerticesGrip :public EditPolyModPolyOperationGrip
{
public:

	enum OurGrips{eWeldVerticesWeldThreshold = 0, eWeldVerticesNum};

	WeldVerticesGrip::WeldVerticesGrip():mStrBefore(_T(" ")),mStrAfter(_T(" ")){}
	WeldVerticesGrip(EditPolyMod *m, PolyOperation *o);
	~WeldVerticesGrip();

	void Okay(TimeValue t);
	void Cancel();
	void Apply(TimeValue t);
	int GetNumGripItems();
	IBaseGrip::Type GetType(int which);
	void GetGripName(TSTR &string);
	bool GetText(int which,TSTR &string);
	bool GetResolvedIconName(int which,MSTR &string);
	DWORD GetCustomization(int which);
	bool GetComboOptions(int which, Tab<IBaseGrip::ComboLabel*> &radioOptions);
	bool GetCommandIcon(int which, MSTR &string); 
	bool GetValue(int which,TimeValue t,IBaseGrip::GripValue &value);
	bool SetValue(int which,TimeValue t,IBaseGrip::GripValue &value); 

	bool GetAutoScale(int which);
	bool GetScaleInViewSpace(int which, float &depth);
	bool GetScale(int which, IBaseGrip::GripValue &scaleValue);
	bool GetResetValue(int which,IBaseGrip::GripValue &resetValue);
	bool StartSetValue(int which,TimeValue t);
	bool EndSetValue(int which,TimeValue t,bool accepted);
	bool ResetValue(TimeValue t,int which);
	bool GetRange(int which,IBaseGrip::GripValue &minRange, IBaseGrip::GripValue &maxRange);
	bool ShowKeyBrackets(int which,TimeValue t);

	
	void SetNumVerts(int before,int after);
private:
	MSTR mStrBefore;
	MSTR mStrAfter;

};

class WeldEdgesGrip :public EditPolyModPolyOperationGrip
{
public:

	enum OurGrips{eWeldEdgesWeldThreshold = 0, eWeldEdgesNum};

	WeldEdgesGrip::WeldEdgesGrip():mStrBefore(_T("     ")),mStrAfter(_T("     ")){}
	WeldEdgesGrip(EditPolyMod *m, PolyOperation *o);
	~WeldEdgesGrip();

	void Okay(TimeValue t);
	void Cancel();
	void Apply(TimeValue t);
	int GetNumGripItems();
	IBaseGrip::Type GetType(int which);
	void GetGripName(TSTR &string);
	bool GetText(int which,TSTR &string);
	bool GetResolvedIconName(int which,MSTR &string);
	DWORD GetCustomization(int which);
	bool GetComboOptions(int which, Tab<IBaseGrip::ComboLabel*> &radioOptions);
	bool GetCommandIcon(int which, MSTR &string); 
	bool GetValue(int which,TimeValue t,IBaseGrip::GripValue &value);
	bool SetValue(int which,TimeValue t,IBaseGrip::GripValue &value); 

	bool GetAutoScale(int which);
	bool GetScaleInViewSpace(int which, float &depth);
	bool GetScale(int which, IBaseGrip::GripValue &scaleValue);
	bool GetResetValue(int which,IBaseGrip::GripValue &resetValue);
	bool StartSetValue(int which,TimeValue t);
	bool EndSetValue(int which,TimeValue t,bool accepted);
	bool ResetValue(TimeValue t,int which);
	bool GetRange(int which,IBaseGrip::GripValue &minRange, IBaseGrip::GripValue &maxRange);
	bool ShowKeyBrackets(int which,TimeValue t);

	void SetNumVerts(int before,int after);
private:
	MSTR mStrBefore;
	MSTR mStrAfter;

};

class MSmoothGrip :public EditPolyModPolyOperationGrip
{
public:

	enum OurGrips{eMSmoothSmoothness = 0, eMSmoothSmoothingGroups, eMSmoothMaterials};

	MSmoothGrip(){};
	MSmoothGrip(EditPolyMod *m, PolyOperation *o);
	~MSmoothGrip();

	void Okay(TimeValue t);
	void Cancel();
	void Apply(TimeValue t);
	int GetNumGripItems();
	IBaseGrip::Type GetType(int which);
	void GetGripName(TSTR &string);
	bool GetText(int which,TSTR &string);
	bool GetResolvedIconName(int which,MSTR &string);
	DWORD GetCustomization(int which);
	bool GetComboOptions(int which, Tab<IBaseGrip::ComboLabel*> &radioOptions);
	bool GetCommandIcon(int which, MSTR &string); 
	bool GetValue(int which,TimeValue t,IBaseGrip::GripValue &value);
	bool SetValue(int which,TimeValue t,IBaseGrip::GripValue &value); 

	bool GetAutoScale(int which);
	bool GetScaleInViewSpace(int which, float &depth);
	bool GetScale(int which, IBaseGrip::GripValue &scaleValue);
	bool GetResetValue(int which,IBaseGrip::GripValue &resetValue);
	bool StartSetValue(int which,TimeValue t);
	bool EndSetValue(int which,TimeValue t,bool accepted);
	bool ResetValue(TimeValue t,int which);
	bool GetRange(int which,IBaseGrip::GripValue &minRange, IBaseGrip::GripValue &maxRange);
	bool ShowKeyBrackets(int which,TimeValue t);

};

class TessellateGrip :public EditPolyModPolyOperationGrip
{
public:

	enum OurGrips{eTessellateType = 0, eTessellateTension };

	TessellateGrip(){};
	TessellateGrip(EditPolyMod *m, PolyOperation *o);
	~TessellateGrip();

	void Okay(TimeValue t);
	void Cancel();
	void Apply(TimeValue t);
	int GetNumGripItems();
	IBaseGrip::Type GetType(int which);
	void GetGripName(TSTR &string);
	bool GetText(int which,TSTR &string);
	bool GetResolvedIconName(int which,MSTR &string);
	DWORD GetCustomization(int which);
	bool GetComboOptions(int which, Tab<IBaseGrip::ComboLabel*> &radioOptions);
	bool GetCommandIcon(int which, MSTR &string); 
	bool GetValue(int which,TimeValue t,IBaseGrip::GripValue &value);
	bool SetValue(int which,TimeValue t,IBaseGrip::GripValue &value); 

	bool GetAutoScale(int which);
	bool GetScaleInViewSpace(int which, float &depth);
	bool GetScale(int which, IBaseGrip::GripValue &scaleValue);
	bool GetResetValue(int which,IBaseGrip::GripValue &resetValue);
	bool StartSetValue(int which,TimeValue t);
	bool EndSetValue(int which,TimeValue t,bool accepted);
	bool ResetValue(TimeValue t,int which);
	bool GetRange(int which,IBaseGrip::GripValue &minRange, IBaseGrip::GripValue &maxRange);
	bool ShowKeyBrackets(int which,TimeValue t);

	//need to activate grips
	void SetUpUI(); //called after the grip is enabled to set up init states that are called via IGripManager

};


#endif
