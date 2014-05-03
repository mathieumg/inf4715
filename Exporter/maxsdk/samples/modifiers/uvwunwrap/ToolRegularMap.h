
/*

Copyright [2010] Autodesk, Inc.  All rights reserved.

Use of this software is subject to the terms of the Autodesk license agreement 
provided at the time of installation or download, or which otherwise accompanies 
this software in either electronic or hard copy form.   

*/


#pragma once

#include "3dsmaxport.h"

#include "TVData.h"
#include "Tab.h"
#include "MeshTopoData.h"
#include "IUnwrapMax8.h"


//just a temporary helper data struct to hold all our UVW verts for a polygon and data to separate UVWs that are on the edge or internal
class UVWInfo
{
public:
	int mGeomVertexIndex;
	int mFaceIndex;
	int mIthFaceEdge;
	int mIthPolyEdge;		//this is the index of the border edge attached to this UVW vertex if it is internal it will be -1
	Point3 mUVW;	//this is the actual UVW value 
};


class EdgeData
{
public:
	int mPolyIndex;
	int mIthPolyEdge;
	bool mSelectEdge;
	bool mNewEdge;

	int mPrevEdge;
	int mNextEdge;
};

enum EdgeType
{
	 kVNegative = 0,kUNegative,kVPositive, kUPositive ,kUknown
};

enum FaceType
{
	kRegular, //4 sides, loop and ring
	kPartial, //4 side but not a ring or loop
	kAbnormal //none of the above
};



class PolygonEdgeInfo
{
public:	
	
	int mVertexIndex;
	int mEdgeIndex;
	int mInnerFace;
	int mInnerFaceIthEdge;
	int mOppositeFace;
	float mU;
	float mV;
	EdgeType mEdgeType;

	bool mTempFlag;
};

class PolygonInfo
{
public:

	void Draw(GraphicsWindow *gw, MeshTopoData*	md);
	int Degree() { return mBorderEdge.Count(); }

	Tab<int>				mFace;				//faces that make up this polygon including internal ones
	Tab<PolygonEdgeInfo>	mBorderEdge;		//the border edges that surround the polygon
	Point3 mGeoNormal;							//the normal of this polygon

	Point3 mCenter;	

	int		mCusterID;
	int		mU,mV;
	int		mUnfoldID;
};



class RegularMap
{
public:
	RegularMap();
	~RegularMap();
	//returns false if the mesh does not conform
	bool Init(UnwrapMod *mod, MeshTopoData *md);
	void Free();


	void Display(GraphicsWindow *gw);

	void AdvanceSelected(BOOL singleStep);
	
	void AdvanceUV(BOOL uPos, BOOL vPos, BOOL uNeg, BOOL vNeg, BOOL singleStep);

	bool StartNewCluster(int faceIndex);
	//commands

	bool GetNormalize();
	void SetNormalize(bool normalize);

	bool GetAutoWeld();
	void SetAutoWeld(bool autoWeld);

	bool GetSingleStep();
	void SetSingleStep(bool singleStep);

	float GetAutoWeldLimit();
	void SetAutoWeldLimit(float autoWeldLimit);

	float GetIconSize();
	void SetIconSize(float iconSize);

	FaceType GetLimit();
	void SetLimit(FaceType limit);

	int GetAutoFit();
	void SetAutoFit(int autofit);

	void ResetFaces();

	void Advance(BOOL uPos, BOOL vPos, BOOL uNeg, BOOL vNeg, 
				 BOOL selected, BOOL singleStep);

	void Expand(RegularMapExpansions expandBy) ;


	HWND GetHWND();
	void SetHWND(HWND hwnd);

	void Hold(bool recomputeLocalData = false);
	void HoldParameters();
	
	
	void GetData(Tab<EdgeData> &edges, BitArray &processedFaces);
	void SetData(Tab<EdgeData> &edges, BitArray &processedFaces);

	void FillOutUI();

	UnwrapMod*		GetMod();
	MeshTopoData*	GetLocalData();

	bool GetPickStartFace();
	void SetPickStartFace(bool start);

	void FitView(int level);

	void GetCurrentCluster(Tab<int> &cluster);
	void SetCurrentCluster(Tab<int> &cluster);

	void SetPos();
	void SavePos();

	void SetPreviewFace(int index);
	


	void MoveCurrentClusterToFreeSpace();

	void MapPolygon(Matrix3 alignTM, int edgeIndex, int polygonIndex, Tab<UVWInfo> &uvws);


private:

	void				BuildBorders( Tab<int> &edges);  //this returns a list of outer border edges for the current cluster

	void				BuildEdgeConnectionInfo();
	void				ClearEdgeFlags();

	void				NormalizeEdges();


	bool				CheckEdge(int polyIndex, int ithEdge, FaceType level);

	Matrix3				GetEdgeTransform(int polyIndex, int edgeIndex);


	// this arranges the edge list to get the best results.
	// we want to order our edges so we unfold certain edges first 
	// for example if a face has 3 edges that are going to be unfolded 
	// that share the same face we want the middle one to unfold first.	
	void				ArrangeEdges(Tab<int> &edges);

	void				UnfoldEdge(int edgeIndex);

	void				NormalizeUVWs(int ithEdge, Tab<UVWInfo> &uvws);
	

	void				RemoveUnusedBorderEdges();

	void				GetFaceEdge(int polyIndex, int ithPolyEdge, int &faceIndex, int &ithFaceEdge);

	UnwrapMod*			mMod;
	MeshTopoData*		mMd;

	FaceType			mAdvanceLimit;
	bool				mAutoWeld;
	float				mAutoWeldLimit;
	bool				mNormalize;
	bool				mSingleStep;
	float				mIconSize;
	bool				mPickStartFace;


	Tab<int>			mCurrentCluster;
	Tab<EdgeData>		mBorderEdges;
	BitArray			mProcessedFaces;
	BitArray			mSelectedPolys;

	Tab<float>			mUSize;
	Tab<float>			mVSize;

	Tab<int>			mFaceToPolyIndex;
	Tab<int>			mVertexConnectionCount;
	Tab<int>			mHiddenEdgeConnectionCount;
	Tab<int>			mOpenEdgeConnectionCount;

	Tab<PolygonInfo*>	mPolygon;
	unsigned int		mCurrentClusterID;  //this is the id of the current cluster/element that is being unfolded

	int					mCurrentUnfoldID;	//this is the id of the last faces that were unfolded

	HWND				mHwnd;
	static bool						mWindowInit;
	static WINDOWPLACEMENT		mWindowPos;

	int					mAutoFit;

	int					mPreviewFace;


};


class RegularMapRestore : public RestoreObj {
public:

	RegularMap			*map;
	Tab<EdgeData>		mUBorderEdges;
	BitArray			mUProcessedFaces;

	Tab<EdgeData>		mRBorderEdges;
	BitArray			mRProcessedFaces;

	BitArray			mUGeomEdge;
	BitArray			mRGeomEdge;
	BitArray			mUTVEdge;
	BitArray			mRTVEdge;

	Tab<int>			mUCurrentCluster;
	Tab<int>			mRCurrentCluster;

	bool				mRecomputeLocalData;
	MeshTopoData		*mRMd;
	MeshTopoData		*mUMd;

	RegularMapRestore(RegularMap *m, bool recomputeLocalData, MeshTopoData *md);
	
	void Restore(int isUndo);
	void Redo();
	int Size() { return 20; }
	void EndHold() {}
	TSTR Description();
	
};

class RegularMapParametersRestore : public RestoreObj {
public:
	FaceType			mUAdvanceLimit;
	bool				mUAutoWeld;
	float				mUAutoWeldLimit;
	bool				mUNormalize;
	bool				mUSingleStep;
	float				mUIconSize;

	FaceType			mRAdvanceLimit;
	bool				mRAutoWeld;
	float				mRAutoWeldLimit;
	bool				mRNormalize;
	bool				mRSingleStep;
	float				mRIconSize;


	RegularMapParametersRestore(RegularMap *m);
	
	void Restore(int isUndo);
	void Redo();
	int Size() { return 20; }
	void EndHold() {}
	TSTR Description();
	
};

