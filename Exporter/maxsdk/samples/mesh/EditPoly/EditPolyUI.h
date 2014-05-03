/**********************************************************************
 *<
	FILE: EditPolyUI.h

	DESCRIPTION: UI-related classes for Edit Poly Modifier

	CREATED BY: Steve Anderson,

	HISTORY: created May 2004

 *>	Copyright (c) 2004 Discreet, All Rights Reserved.
 **********************************************************************/

#include "iEPolyMod.h"

class ConstraintUIHandler
{
public:
	void Initialize (HWND hWnd, EPolyMod *pMod);
	void Update (HWND hWnd, EPolyMod *pMod);
	void Set (HWND hWnd, EPolyMod *pMod, int constraintType);
};

ConstraintUIHandler *TheConstraintUIHandler ();

class EditPolyGeomDlgProc : public ParamMap2UserDlgProc {
	EPolyMod *mpMod;

public:
	EditPolyGeomDlgProc () : mpMod(NULL) { }
	void SetEPoly (EPolyMod *e) { mpMod = e; }
	INT_PTR DlgProc (TimeValue t, IParamMap2 *map, HWND hWnd,
		UINT msg, WPARAM wParam, LPARAM lParam);
	void SetEnables (HWND hGeom);
	void UpdateSliceUI (HWND hGeom);
	void DeleteThis() { }
};

EditPolyGeomDlgProc *TheGeomDlgProc ();

class EditPolySubobjControlDlgProc : public ParamMap2UserDlgProc {
	EPolyMod *mpMod;

public:
	EditPolySubobjControlDlgProc () : mpMod(NULL) { }
	void SetEPoly (EPolyMod *e) { mpMod = e; }
	void SetEnables (HWND hWnd);
	INT_PTR DlgProc (TimeValue t, IParamMap2 *map, HWND hWnd,
		UINT msg, WPARAM wParam, LPARAM lParam);
	void DeleteThis() { }
};

EditPolySubobjControlDlgProc *TheSubobjDlgProc ();

class MaterialUIHandler
{
public:
	void GetMtlIDList (Mtl *mtl, NumList & mtlIDList);
	void GetMtlIDList (EPolyMod *pMod, INode *node, NumList & mtlIDList);
	INode *GetNode (EPolyMod *pMod);
	bool SetupMtlSubNameCombo (HWND hWnd, EPolyMod *pMod);
	void UpdateNameCombo (HWND hWnd, ISpinnerControl *spin);
	void ValidateUINameCombo (HWND hWnd, EPolyMod *pMod);
	void UpdateCurrentMaterial (HWND hWnd, EPolyMod *pMod, TimeValue t, Interval & validity);
	void SelectByName (HWND hWnd, EPolyMod *pMod);
};

class SurfaceDlgProc : public ParamMap2UserDlgProc, public TimeChangeCallback {
protected:

	EPolyMod *mpMod;
	bool klugeToFixWM_CUSTEDIT_ENTEROnEnterFaceLevel;
	bool mSpinningMaterial;
	Interval uiValid;
	HWND mTimeChangeHandle;
	MaterialUIHandler mMaterialUIHandler;

public:
	SurfaceDlgProc () : mpMod(NULL), uiValid(NEVER), klugeToFixWM_CUSTEDIT_ENTEROnEnterFaceLevel(false),
		mSpinningMaterial(false), mTimeChangeHandle(NULL) { }
	INT_PTR DlgProc (TimeValue t, IParamMap2 *map, HWND hWnd,
		UINT msg, WPARAM wParam, LPARAM lParam);
	void DeleteThis() { }
	void Invalidate () { uiValid.SetEmpty(); }
	void SetEPoly (EPolyMod *e) { mpMod=e; }
	void AddTimeChangeInvalidate (HWND hWnd);
	void RemoveTimeChangeInvalidate ();
	void TimeChanged (TimeValue t) { if (!uiValid.InInterval(t)) InvalidateRect (mTimeChangeHandle, NULL, true); }
};

SurfaceDlgProc *TheSurfaceDlgProc();
SurfaceDlgProc *TheSurfaceDlgProcFloater();

class SmoothingGroupUIHandler {
private:
	bool mUIValid;

public:
	SmoothingGroupUIHandler () : mUIValid(false) { }

	void InitializeButtons (HWND hWnd);
	void UpdateSmoothButtons (HWND hWnd,DWORD set,DWORD indeterminate, DWORD unused=0);
	void UpdateSmoothButtons (HWND hWnd, EPolyMod *pMod);
	bool GetSmoothingGroups (EPolyMod *pMod, DWORD *anyFaces, DWORD *allFaces, bool useSel);
	void SetSmoothing (HWND hWnd, int buttonID, EPolyMod *pMod);
	void ClearAll (EPolyMod *pMod);
	bool GetSelectBySmoothParams (EPolyMod *pEPoly);
	bool UIValid () { return mUIValid; }
};


class FaceSmoothDlgProc : public SurfaceDlgProc {
private:
	SmoothingGroupUIHandler mSmoothingGroupUIHandler;
public:
	FaceSmoothDlgProc () : SurfaceDlgProc() { }
	void DeleteThis() { }
	INT_PTR DlgProc (TimeValue t, IParamMap2 *map, HWND hWnd,
		UINT msg, WPARAM wParam, LPARAM lParam);

};

FaceSmoothDlgProc *TheFaceSmoothDlgProc(); //use the same surface dlg proc since it came from there
FaceSmoothDlgProc *TheFaceSmoothDlgProcFloater(); //use the same surface dlg proc since it came from there

// Class used to track the "current" position of the EPoly popup dialogs
class EPolyPopupPosition {
	bool mPositionSet;
	int mPosition[4];
public:
	EPolyPopupPosition() : mPositionSet(false) { }
	bool GetPositionSet () { return mPositionSet; }
	void Scan (HWND hWnd);
	void Set (HWND hWnd);
};

EPolyPopupPosition *ThePopupPosition ();

class EditPolyOperationProc : public ParamMap2UserDlgProc {
	EPolyMod *mpMod;

public:
	EditPolyOperationProc () : mpMod(NULL) { }
	void SetEditPolyMod (EPolyMod*e) { mpMod = e; }
	void UpdateUI (HWND hWnd, TimeValue t, int paramID);
	void UpdateHingeUI (HWND hWnd);
	INT_PTR DlgProc(TimeValue t,IParamMap2 *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	void DeleteThis() { }
};

EditPolyOperationProc *TheOperationDlgProc ();

class EditPolyPaintDeformDlgProc : public ParamMap2UserDlgProc {
	bool uiValid;
	EditPolyMod *mpEPoly;

public:
	EditPolyPaintDeformDlgProc () : mpEPoly(NULL), uiValid(false) { }
	INT_PTR DlgProc (TimeValue t, IParamMap2 *map, HWND hWnd,
		UINT msg, WPARAM wParam, LPARAM lParam);
	void SetEnables (HWND hWnd);
	void DeleteThis() { }
	void InvalidateUI (HWND hWnd) { uiValid = false; if (hWnd) InvalidateRect (hWnd, NULL, false); }
	void SetEPoly (EditPolyMod *e) { mpEPoly=e; }
};

EditPolyPaintDeformDlgProc *ThePaintDeformDlgProc();

class EditPolySoftselDlgProc : public ParamMap2UserDlgProc {
	EPolyMod *mpEPoly;

public:
	EditPolySoftselDlgProc () : mpEPoly(NULL) { }
	INT_PTR DlgProc (TimeValue t, IParamMap2 *map, HWND hWnd,
		UINT msg, WPARAM wParam, LPARAM lParam);
	void DrawCurve (TimeValue t, HWND hWnd, HDC hdc);
	void DeleteThis() { }
	void SetEnables (HWND hSS, TimeValue t);
	void SetEPoly (EPolyMod *e) { mpEPoly=e; }
};

EditPolySoftselDlgProc *TheSoftselDlgProc();


