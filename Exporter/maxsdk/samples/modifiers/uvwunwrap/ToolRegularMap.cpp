


/*

Copyright [2010] Autodesk, Inc.  All rights reserved.

Use of this software is subject to the terms of the Autodesk license agreement 
provided at the time of installation or download, or which otherwise accompanies 
this software in either electronic or hard copy form.   


arrange edges sort by edge type count

add normal cluster size

continue edge select advance on abnormal faces

fix abnormal face selection and edge type definition

*/


#include "ToolRegularMap.h"
#include "unwrap.h"
#include "meshadj.h"
#include "modsres.h"

//HWND            UnwrapMod::hSelRollup = NULL;
WINDOWPLACEMENT RegularMap::mWindowPos = WINDOWPLACEMENT();
bool RegularMap::mWindowInit = false;

RegularMap::RegularMap()
{
	if (mWindowInit == false)
	{
		mWindowPos.length = 0;
		mWindowInit = true;
	}

	mNormalize = true;
	mMd = NULL;
	mAdvanceLimit = kRegular;

	mAutoWeld = false;
	mSingleStep = true;
	mAutoWeldLimit = 0.2f;
	mIconSize = 0.25f;
	mCurrentClusterID = 0;
	mHwnd = NULL;
	mPickStartFace = true;
	mAutoFit = 1;
	mCurrentUnfoldID = 0;
	mPreviewFace = -1;
	
}
RegularMap::~RegularMap()
{
	mMd = NULL;
	Free();
}

HWND RegularMap::GetHWND()
{
	return mHwnd;
}
void RegularMap::SetHWND(HWND hwnd)
{
	mHwnd = hwnd;
}

UnwrapMod*		RegularMap::GetMod()
{
	return mMod;
}

MeshTopoData*	RegularMap::GetLocalData()
{
	return mMd;
}

bool RegularMap::GetNormalize()
{
	return mNormalize;
}
void RegularMap::SetNormalize(bool normalize)
{
	mNormalize = normalize;
}

bool RegularMap::GetAutoWeld()
{
	return mAutoWeld;
}
void RegularMap::SetAutoWeld(bool autoWeld)
{
	mAutoWeld = autoWeld;
}


float RegularMap::GetAutoWeldLimit()
{
	return mAutoWeldLimit;
}
void RegularMap::SetAutoWeldLimit(float autoWeldLimit)
{
	mAutoWeldLimit = autoWeldLimit;
}

bool RegularMap::GetSingleStep()
{
	return mSingleStep;
}
void RegularMap::SetSingleStep(bool singleStep)
{
	mSingleStep = singleStep;
}


float RegularMap::GetIconSize()
{
	return mIconSize;
}
void RegularMap::SetIconSize(float iconSize)
{
	mIconSize = iconSize;
}

FaceType RegularMap::GetLimit()
{
	return mAdvanceLimit;
}
void RegularMap::SetLimit(FaceType limit)
{
	mAdvanceLimit = limit;
}

bool RegularMap::GetPickStartFace()
{
	if (mHwnd == NULL)
		return false;
	return mPickStartFace;
}

void RegularMap::SetPickStartFace(bool start)
{	
	mPickStartFace = start;
}

void RegularMap::SetPreviewFace(int index)
{

	if (index == -1)
		mPreviewFace = index;
	else
	{
		int testPoly = mFaceToPolyIndex[index];
		if (testPoly == -1)
			mPreviewFace = -1;
		else
		{
			if (mProcessedFaces[testPoly] || (mPolygon[testPoly]->Degree() != 4))
				mPreviewFace = -1;
			else
				mPreviewFace = testPoly;
		}
			
	}
}

void RegularMap::Hold(bool recomputeLocalData)
{
	bool held = false;
	if (theHold.Holding())
		held = true;
	if (!held)
		theHold.Begin();

	theHold.Put(new RegularMapRestore(this,recomputeLocalData,mMd));

	if (!held)
		theHold.Accept(GetString(IDS_UNFOLDMAP));
}

void RegularMap::HoldParameters()
{
	bool held = false;
	if (theHold.Holding())
		held = true;
	if (!held)
		theHold.Begin();

//	theHold.Put(new RegularMapParams());

	if (!held)
		theHold.Accept(GetString(IDS_UNFOLDPARAMETER));
	

}


void RegularMap::GetData(Tab<EdgeData> &edges, BitArray &processedFaces)
{
	edges = mBorderEdges;
	processedFaces = mProcessedFaces;
}
void RegularMap::SetData(Tab<EdgeData> &edges, BitArray &processedFaces)
{
	mBorderEdges = edges; 
	mProcessedFaces = processedFaces;

}


void RegularMap::FillOutUI()
{
	if (mHwnd)
	{
		CheckDlgButton(mHwnd,IDC_AUTOWELD_CHECK,mAutoWeld);
		CheckDlgButton(mHwnd,IDC_SINGLESTEP_CHECK,mSingleStep);
		CheckDlgButton(mHwnd,IDC_NORMALIZE_CHECK,mNormalize);
		SendMessage(GetDlgItem(mHwnd,IDC_AUTOFIT_COMBO), CB_SETCURSEL, mAutoFit, 0L );

		HWND hMethod = GetDlgItem(mHwnd,IDC_LIMIT_COMBO);
		int limit = 0;
		if (GetAutoWeldLimit() == kRegular)
			limit = 0;
		else if (GetAutoWeldLimit() == kPartial)
			limit = 1;
		else if (GetAutoWeldLimit() == kAbnormal)
			limit = 2;

		SendMessage(hMethod, CB_SETCURSEL, limit, 0L);

					
		ICustButton *iButton = GetICustButton(GetDlgItem(mHwnd, IDC_UNWRAP_STARTNEWCLUSTER));
		

		if (iButton)
		{
			if (GetPickStartFace())
			{
				iButton->SetCheck(TRUE);
			}
			else
			{
				iButton->SetCheck(FALSE);
			}
			ReleaseICustButton(iButton);
		}

		BOOL enable = FALSE;
		if (mBorderEdges.Count() > 0)
			enable = TRUE;
		EnableWindow(GetDlgItem(mHwnd, IDC_ADVANCESELECTED),enable);
		EnableWindow(GetDlgItem(mHwnd, IDC_ADVANCEU),enable);
		EnableWindow(GetDlgItem(mHwnd, IDC_ADVANCEV),enable);



//		mod->normalMethod = SendMessage(hMethod, CB_SETCURSEL, 0, 0L);

		
//			IDC_UNWRAP_AUTOWELD_THRESHOLD
//			IDC_UNWRAP__AUTOWELD_THRESHOLD_SPIN

	}
}

bool RegularMap::Init(UnwrapMod *mod, MeshTopoData *md)
{

	Free();

	mMd = md;
	mMod = mod;
	

	

	//find out how many visible edges are connected at each vertex
	mVertexConnectionCount.SetCount(md->GetNumberGeomVerts());
	mHiddenEdgeConnectionCount.SetCount(md->GetNumberGeomVerts());
	mOpenEdgeConnectionCount.SetCount(md->GetNumberGeomVerts());

	for (int i = 0; i < mVertexConnectionCount.Count(); i++)
	{
		mVertexConnectionCount[i] = 0;
		mHiddenEdgeConnectionCount[i] = 0;
		mOpenEdgeConnectionCount[i] = 0;

	}
	for (int i = 0; i < md->GetNumberGeomEdges(); i++)
	{
		int a = md->GetGeomEdgeVert(i,0);
		int b = md->GetGeomEdgeVert(i,1);
		if (md->GetGeomEdgeNumberOfConnectedFaces(i) == 1)
		{
			mOpenEdgeConnectionCount[a] += 1;
			mOpenEdgeConnectionCount[b] += 1;
		}
		if (md->GetGeomEdgeHidden(i) == FALSE)
		{
			mVertexConnectionCount[a] += 1;		
			mVertexConnectionCount[b] += 1;
		}
		else
		{
			mHiddenEdgeConnectionCount[a] += 1;		
			mHiddenEdgeConnectionCount[b] += 1;
		}
	}

	bool useSelection = false;
	if (mMd->GetFaceSelection().NumberSet()  && (mod->fnGetTVSubMode() == TVFACEMODE))
		useSelection = true;

	mFaceToPolyIndex.SetCount(md->GetNumberFaces());
	for (int i = 0; i < mFaceToPolyIndex.Count(); i++)
		mFaceToPolyIndex[i] = -1;


	if (md->GetMesh())
	{
		//we need to create our polygon data
		BitArray usedFaces;
		usedFaces.SetSize(mMd->GetNumberFaces());
		usedFaces.ClearAll();


		
		AdjEdgeList ae(*md->GetMesh());
		AdjFaceList af(*md->GetMesh(),ae);

		BitArray set;
		set.SetSize(md->GetNumberFaces());
		set.ClearAll();
		Tab<int> faces;
		for (int i = 0; i < md->GetNumberFaces(); i++)
		{
			if (!md->GetFaceDead(i))
			{
				if (usedFaces[i] == false)
				{
					//get our cluster
					set.ClearAll();
					mMd->GetMesh()->PolyFromFace (i, set, 0.785f, FALSE, &af);

					//check to see if degenerate
					for (int j = 0; j < set.GetSize(); j++)
					{
						if (set[j])
						{
							bool valid = true;
							for (int k = 0; k < 3; k++)
							{
								int a = md->GetFaceGeomVert(j,k);
								int b = md->GetFaceGeomVert(j,(k+1)%3);
								if (a==b)
									valid = false;
								float l = Length(md->GetGeomVert(a)-md->GetGeomVert(b));
								if (l == 0.0f)
									valid = false;
							}
							if (!valid)
							{
								set.Clear(j);
								usedFaces.Set(j,TRUE);
							}
							else if (usedFaces[j])
							{
								set.Clear(j);
							}
						}
						
					}

					if (set.NumberSet() == 0)
					{
						continue;
					}

					PolygonInfo *poly = new PolygonInfo();


					poly->mFace.SetCount(set.NumberSet());
					int ct = 0;
					Point3 normal(0,0,0);


//					DebugPrint("Poly Index %d face count %d\n",mPolygon.Count(),(int)set.NumberSet());
					for (int j = 0; j < set.GetSize(); j++)
					{
						if (set[j])
						{
							if (!usedFaces[j])
							{
								normal += md->GetGeomFaceNormal(j);					
								poly->mFace[ct++] = j;
								mFaceToPolyIndex[j] = mPolygon.Count();
								usedFaces.Set(j,TRUE);
							}
							else
							{
								set.Clear(j);
							}
//							DebugPrint(" %d ",j);
						}
					}					
//					DebugPrint(" \n ");
					
					poly->mGeoNormal = normal/poly->mFace.Count();
					poly->mCusterID = mCurrentClusterID;
					poly->mUnfoldID = mCurrentUnfoldID;

					//compute our border loop
					Tab<int> borderEdges;
					BitArray borderVerts;
					borderVerts.SetSize(mMd->GetNumberGeomVerts());
					borderVerts.ClearAll();
//					DebugPrint(" BorderVerts ");
					for (int j = 0; j < mMd->GetNumberGeomEdges(); j++)
					{
						int numberFaces = mMd->GetGeomEdgeNumberOfConnectedFaces(j);
						int va = mMd->GetGeomEdgeVert(j,0);
						int vb = mMd->GetGeomEdgeVert(j,1);


						

						if (numberFaces == 1)
						{
							int faceIndex = mMd->GetGeomEdgeConnectedFace(j,0);
							if (set[faceIndex])
							{
								borderEdges.Append(1,&j,1000);
								borderVerts.Set(va,TRUE);
								borderVerts.Set(vb,TRUE);
//								DebugPrint(" %d %d ",va,vb);
							}
						}						
						else if (numberFaces == 2)
						{
							int faceIndex0 = mMd->GetGeomEdgeConnectedFace(j,0);
							int faceIndex1 = mMd->GetGeomEdgeConnectedFace(j,1);
							if ( (set[faceIndex0] == TRUE) && (set[faceIndex1] == FALSE) )
							{
								borderEdges.Append(1,&j,1000);
								borderVerts.Set(va,TRUE);
								borderVerts.Set(vb,TRUE);
//								DebugPrint(" %d %d ",va,vb);
							}
							else if ( (set[faceIndex1] == TRUE) && (set[faceIndex0] == FALSE) )
							{
								borderEdges.Append(1,&j,1000);
								borderVerts.Set(va,TRUE);
								borderVerts.Set(vb,TRUE);
//								DebugPrint(" %d %d ",va,vb);
							}
						}						
					}

//					DebugPrint(" \n ");

					//now create the edges
//					DebugPrint(" edge Verts \n ");
					poly->mBorderEdge.SetCount(borderEdges.Count());				
					for (int j = 0; j < borderEdges.Count(); j ++)
					{						
						poly->mBorderEdge[j].mVertexIndex = -1;
						poly->mBorderEdge[j].mInnerFace = -1;
						poly->mBorderEdge[j].mInnerFaceIthEdge = -1;

						poly->mBorderEdge[j].mEdgeIndex = borderEdges[j];

						int numberFaces = mMd->GetGeomEdgeNumberOfConnectedFaces(borderEdges[j]);
						int faceIndex = -1;
						if (numberFaces == 1)
						{
							faceIndex = mMd->GetGeomEdgeConnectedFace(borderEdges[j],0);
							poly->mBorderEdge[j].mOppositeFace = -1;
						}
						else
						{
							faceIndex = mMd->GetGeomEdgeConnectedFace(borderEdges[j],0);
							poly->mBorderEdge[j].mOppositeFace = mMd->GetGeomEdgeConnectedFace(borderEdges[j],1);
							if (set[faceIndex] == false)
							{
								faceIndex = mMd->GetGeomEdgeConnectedFace(borderEdges[j],1);
								poly->mBorderEdge[j].mOppositeFace = mMd->GetGeomEdgeConnectedFace(borderEdges[j],0);
							}
						}
						
						poly->mBorderEdge[j].mInnerFace = faceIndex;
						int va = mMd->GetGeomEdgeVert(borderEdges[j],0);
						int vb = mMd->GetGeomEdgeVert(borderEdges[j],1);
//						DebugPrint("Find match va %d %d \n\n",va,vb);
						for (int k = 0; k < 3; k++)
						{
							int geomVert1 = mMd->GetFaceGeomVert(faceIndex,k);
							int geomVert2 = mMd->GetFaceGeomVert(faceIndex,(k+1)%3);
//							DebugPrint(" geomvert1 %d ",geomVert);
							if ( (geomVert1 == va) && (geomVert2 == vb) )
//								 borderVerts[va] &&  borderVerts[vb])
							{
								poly->mBorderEdge[j].mInnerFaceIthEdge = k;
								poly->mBorderEdge[j].mVertexIndex = va;
//								borderVerts.Set(va,FALSE);
								k = 3;
							}
							else if ( (geomVert1 == vb) && (geomVert2 == va) )
//								borderVerts[vb])
							{
								poly->mBorderEdge[j].mInnerFaceIthEdge = k;
								poly->mBorderEdge[j].mVertexIndex = vb;
//								borderVerts.Set(vb,FALSE);
								k = 3;
							}
						}
//						DebugPrint(" \n ");
//						DebugPrint(" %d ",poly->mBorderEdge[j].mVertexIndex);
						poly->mBorderEdge[j].mEdgeType = kUknown;
						poly->mBorderEdge[j].mU = 0.0f;
						poly->mBorderEdge[j].mV = 0.0f;
					}
//					DebugPrint(" \n ");


					//now order our poly border edges
//					DebugPrint(" ordered edge Verts \n ");
					int currentBorderEdge = 0;
					for (int j = 0; j < poly->mBorderEdge.Count(); j++)
					{
						int faceIndex = poly->mBorderEdge[j].mInnerFace;
						int currentVert = poly->mBorderEdge[j].mVertexIndex;
						int nextVert = -1;
						for (int k = 0; k < 3; k++)
						{
							if (mMd->GetFaceGeomVert(faceIndex,k) == currentVert)
							{
								nextVert = mMd->GetFaceGeomVert(faceIndex,(k+1)%3);
								k = 3;
							}
						}
//						DebugPrint("faceIndex %d vertex %d ",faceIndex, poly->mBorderEdge[j].mVertexIndex);
						bool swap = false;
						for (int k = (j+1); k < poly->mBorderEdge.Count(); k++)
						{
							//found connection
							if (poly->mBorderEdge[k].mVertexIndex == nextVert)
							{
								//swap
								PolygonEdgeInfo temp = poly->mBorderEdge[j+1];
								poly->mBorderEdge[j+1] =  poly->mBorderEdge[k];
								poly->mBorderEdge[k] = temp;
								k = poly->mBorderEdge.Count();
								swap = true;
							}
						}
					}
/*
					DebugPrint(" PolyInfo %d \n ",mPolygon.Count());
					DebugPrint("	**Face Info\n ",mPolygon.Count());
					for (int j = 0; j < poly->mFace.Count(); j++)
					{
						DebugPrint("		face %d\n ",poly->mFace[j]);
					}

					DebugPrint("	**Border Info\n ",mPolygon.Count());
					for (int j = 0; j < poly->mBorderEdge.Count(); j++)
					{
						DebugPrint("		inner face %d vertex index %d\n ",poly->mBorderEdge[j].mInnerFace,poly->mBorderEdge[j].mVertexIndex);
					}
					DebugPrint(" \n ");
*/

					mPolygon.Append(1,&poly,md->GetNumberFaces());


				}				
			}
		}

	}
	else
	{
		//copy over just the face data, we need a lot of flags and connection info so we are putting in
		//a separate data structure
		mPolygon.SetCount(md->GetNumberFaces());
		for (int i = 0; i < mPolygon.Count(); i++)
		{
			mFaceToPolyIndex[i] = i;
			if (md->GetFaceDead(i))
			{
				mPolygon[i] = NULL;				 
			}
			else
			{
				
				mPolygon[i] = new PolygonInfo();
				mPolygon[i]->mFace.SetCount(1);
				mPolygon[i]->mFace[0] = i;
				mPolygon[i]->mCusterID = mCurrentClusterID;
				mPolygon[i]->mUnfoldID = mCurrentUnfoldID;

				int degree = md->GetFaceDegree(i);
				mPolygon[i]->mBorderEdge.SetCount(degree);				
				for (int j = 0; j < degree; j ++)
				{
					int index = md->GetFaceGeomVert(i,j);
					mPolygon[i]->mBorderEdge[j].mVertexIndex = index;
					mPolygon[i]->mBorderEdge[j].mInnerFace = i;
					mPolygon[i]->mBorderEdge[j].mInnerFaceIthEdge = j;
					mPolygon[i]->mBorderEdge[j].mEdgeIndex = -1;
					mPolygon[i]->mBorderEdge[j].mEdgeType = kUknown;
					mPolygon[i]->mBorderEdge[j].mU = 0.0f;
					mPolygon[i]->mBorderEdge[j].mV = 0.0f;
					mPolygon[i]->mBorderEdge[j].mOppositeFace = -1;
				}
				Point3 normal(0,0,0);
				normal = md->GetGeomFaceNormal(i);
				mPolygon[i]->mGeoNormal = normal;
				
				
			}
		}

		//now fill out the face data
		int numGeoEdges = md->GetNumberGeomEdges();
		for (int i = 0; i < numGeoEdges; i++)
		{
			int a = md->GetGeomEdgeVert(i,0);
			int b = md->GetGeomEdgeVert(i,1);
			int numConnectedFaces = md->GetGeomEdgeNumberOfConnectedFaces(i);
			if (numConnectedFaces > 2) 
			{
				Free();
				return false;
			}
			for (int j = 0; j < numConnectedFaces; j++)
			{
				int faceIndex = md->GetGeomEdgeConnectedFace(i,j);
				int oppositeFaceIndex = md->GetGeomEdgeConnectedFace(i,(j+1)%numConnectedFaces);
				int degree = md->GetFaceDegree(faceIndex);				
				for (int k = 0; k < degree; k++)
				{
					int testA = md->GetFaceGeomVert(faceIndex,k);
					int testB = md->GetFaceGeomVert(faceIndex,(k+1)%degree);

					if ( ((a==testA) && (b==testB)) ||
						 ((b==testA) && (a==testB)) )
					{
						mPolygon[faceIndex]->mBorderEdge[k].mEdgeIndex = i;
						mPolygon[faceIndex]->mBorderEdge[k].mOppositeFace = oppositeFaceIndex;
/*
						if (md->GetGeomEdgeSelected(i))
						{
							mBorderEdges.SetCount(1);
							mBorderEdges[0].mFaceIndex = faceIndex;
							mBorderEdges[0].mIthFaceEdge = k;
						}
*/
					}
				}
			}
		}	
	}

	for (int i = 0; i < mPolygon.Count(); i++)
	{
		Point3 center(0,0,0);
		int degree = mPolygon[i]->Degree();
		for (int j = 0; j < degree; j++)
		{
			int vertexIndex = mPolygon[i]->mBorderEdge[j].mVertexIndex;
			center += mMd->GetGeomVert(vertexIndex);
		}
		center /= degree;
		mPolygon[i]->mCenter = center;
	}

	mMd->SetTVEdgeInvalid();
	
	mProcessedFaces.SetSize(mPolygon.Count());
	mSelectedPolys.SetSize(mPolygon.Count());
	mProcessedFaces.ClearAll();
	mSelectedPolys.SetAll();

	if (useSelection)
	{
		mProcessedFaces.SetAll();
		for (int i = 0; i < mMd->GetNumberFaces(); i++)
		{
			int polyIndex = mFaceToPolyIndex[i];
			if (mMd->GetFaceSelected(i))
			{
				mProcessedFaces.Set(polyIndex,FALSE);
			}
			else
			{
				mSelectedPolys.Set(polyIndex,FALSE);
			}
		}
	}

	return true;
}
void RegularMap::Free()
{
	for (int i = 0; i < mPolygon.Count(); i++)
	{
		if (mPolygon[i])
			delete mPolygon[i];
		mPolygon[i] = NULL;
	}
	mCurrentCluster.SetCount(0);
	mPolygon.SetCount(0);
	mProcessedFaces.ClearAll();
	mBorderEdges.SetCount(0);
	mCurrentUnfoldID = 0;
	mSelectedPolys.ClearAll();

}


Matrix3	RegularMap::GetEdgeTransform(int polyIndex, int ithEdge)
{
	Point3 center(0,0,0);
	Point3 vec(0,0,0);
	Point3 x(0,0,0),y(0,0,0),z(0,0,0);

	Point3 norm(0,0,0);

	Point3 a(0,0,0), b(0,0,0);

	int index = mPolygon[polyIndex]->mBorderEdge[ithEdge].mVertexIndex;
	int degree = mPolygon[polyIndex]->Degree();


	a = mMd->GetGeomVert(index);
	center = a;
	index = mPolygon[polyIndex]->mBorderEdge[(ithEdge+1)%degree].mVertexIndex;
	b = mMd->GetGeomVert(index);
	float len = Length(b-a);
	x = (b-a);
	x = Normalize(x);
	z = mPolygon[polyIndex]->mGeoNormal;// mMd->GetGeomFaceNormal(polyIndex);
	y = Normalize(CrossProd(x,z));//*len;
	x = Normalize(x)*len;
	z = Normalize(z);//*len;
	Matrix3 tm(1);
	tm.SetRow(0,x);
	tm.SetRow(1,y);
	tm.SetRow(2,z);
	tm.SetRow(3,center);

/*
	DebugPrint("Edge Tm **********\n");
	DebugPrint("	%f %f %f\n",x.x,x.y,x.z);
	DebugPrint("	%f %f %f\n",y.x,y.y,y.z);
	DebugPrint("	%f %f %f\n",z.x,z.y,z.z);
	DebugPrint("	%f %f %f\n",center.x,center.y,center.z);
*/

	return tm;
}

void RegularMap::MapPolygon(Matrix3 alignTM, int polygonIndex, int ithEdge,  Tab<UVWInfo> &uvws)
{
	Matrix3 tm = GetEdgeTransform(polygonIndex,ithEdge);
	Matrix3 itm = Inverse(tm);

	BitArray hitVerts;
	hitVerts.SetSize(mMd->GetNumberGeomVerts());
	hitVerts.ClearAll();

	for (int i = 0; i < mPolygon[polygonIndex]->mBorderEdge.Count(); i++)
	{
		int geomIndex = mPolygon[polygonIndex]->mBorderEdge[i].mVertexIndex;

		UVWInfo value;
		value.mGeomVertexIndex = geomIndex;
		value.mIthPolyEdge = i;
		value.mFaceIndex = mPolygon[polygonIndex]->mBorderEdge[i].mInnerFace;
		value.mIthFaceEdge = mPolygon[polygonIndex]->mBorderEdge[i].mInnerFaceIthEdge;
		uvws.Append(1,&value,100);

		hitVerts.Set(geomIndex,TRUE);
	}


	for (int i = 0; i < mPolygon[polygonIndex]->mFace.Count(); i++)
	{
		int faceIndex = mPolygon[polygonIndex]->mFace[i];
		int degree = mMd->GetFaceDegree(faceIndex);
		for (int j = 0; j < degree; j++)
		{
			int geomIndex = mMd->GetFaceGeomVert(faceIndex,j);
			if (!hitVerts[geomIndex])
			{
				hitVerts.Set(geomIndex,TRUE);

				UVWInfo value;
				value.mIthPolyEdge = -1;
				value.mGeomVertexIndex = geomIndex;
				value.mFaceIndex = faceIndex;
				value.mIthFaceEdge = mPolygon[polygonIndex]->mBorderEdge[i].mInnerFaceIthEdge;
				uvws.Append(1,&value,100);
			}
		}		
	}

	int count = uvws.Count();//mPolygon[faceIndex]->Degree();
	mPolygon[polygonIndex]->mUnfoldID = mCurrentUnfoldID;
	for (int i = 0; i < count; i++)
	{
		int index = uvws[i].mGeomVertexIndex;//mPolygon[polygonIndex]->mBorderEdge[i].mVertexIndex;		
		Point3 geom = mMd->GetGeomVert(index);
		Point3 localP = geom * itm;
		uvws[i].mUVW = localP  * alignTM;
	}
/*
	DebugPrint("MapPoly\n");
	for (int i = 0; i < count; i++)
	{
		DebugPrint("%f %f %f \n",uvws[i].mUVW.x,uvws[i].mUVW.y,uvws[i].mUVW.z);
	}
*/
}

void RegularMap::UnfoldEdge(int edgeIndex)
{
	TimeValue t = GetCOREInterface()->GetTime();
	//get the source face
	int polyIndex = mBorderEdges[edgeIndex].mPolyIndex;
	int ithPolyEdge = mBorderEdges[edgeIndex].mIthPolyEdge;
//	int degree = mPolygon[polyIndex]->Degree();
	mProcessedFaces.Set(polyIndex,TRUE);


	int faceIndex = -1;
	int ithFaceEdge = -1;
	GetFaceEdge(polyIndex,ithPolyEdge,faceIndex,ithFaceEdge);
	int degree = mMd->GetFaceDegree(faceIndex);


//	int faceIndex = mPolygon[polyIndex]->mBorderEdge[ithPolyEdge].mInnerFace;
	

	//get the edge transform
	Point3 a(0,0,0);
	Point3 b(0,0,0);


	int vertexIndexA = mMd->GetFaceTVVert(faceIndex,ithFaceEdge);//mMd->GetTVFmPolygon[faceIndex]->mVertex[ithEdge].mVertexIndex;
	int geoVertexIndexA = mMd->GetFaceGeomVert(faceIndex,ithFaceEdge);
	a = mMd->GetTVVert(vertexIndexA);

	int vertexIndexB = mMd->GetFaceTVVert(faceIndex,(ithFaceEdge+1)%degree);
	int geoVertexIndexB = mMd->GetFaceGeomVert(faceIndex,(ithFaceEdge+1)%degree);

	b = mMd->GetTVVert(vertexIndexB);


	//get the target face
	int targetFaceIndex = mPolygon[polyIndex]->mBorderEdge[ithPolyEdge].mOppositeFace;
	if (targetFaceIndex == -1)
	{
		return;
	}
	int targetDegree = mPolygon[mFaceToPolyIndex[targetFaceIndex]]->Degree();
	//find the target ith edge
	int ithTargetEdge = -1;
	for (int i = 0; i < targetDegree; i++)
	{
		int targetA = mPolygon[mFaceToPolyIndex[targetFaceIndex]]->mBorderEdge[i].mVertexIndex;
		int targetB = mPolygon[mFaceToPolyIndex[targetFaceIndex]]->mBorderEdge[(i+1)%targetDegree].mVertexIndex;

		if ((geoVertexIndexA == targetB) && (geoVertexIndexB == targetA))
			ithTargetEdge = i;
	}


	//compute the transform
	Point3 x(0,0,0);
	Point3 y(0,0,0);
	Point3 z(0,0,1);

	float len = Length(a-b);
	x = Normalize(a-b);	
	y = Normalize(CrossProd(x,z) * 1.0f);


	//get the uvws
	//map it	
	Matrix3 tm(1);
	x *= len;

//  if (scale)  //since we are unfolding in object space we dont want to scale the y and z since we dont want to deform the face
				//if we decide to move to another space we should add these back
//	{
		//	y *= len;
		//	z *= len;
//	}

	tm.SetRow(0,x);
	tm.SetRow(1,y);
	tm.SetRow(2,z);
	tm.SetRow(3,b);



	Tab<UVWInfo> uvws;
	MapPolygon(tm,  mFaceToPolyIndex[targetFaceIndex], ithTargetEdge, uvws);

	if (mNormalize && mPolygon[mFaceToPolyIndex[targetFaceIndex]]->Degree() == 4)
		NormalizeUVWs(ithTargetEdge,uvws);

	mPolygon[mFaceToPolyIndex[targetFaceIndex]]->mCusterID = mPolygon[polyIndex]->mCusterID;

	mProcessedFaces.Set(mFaceToPolyIndex[targetFaceIndex],TRUE);
	mCurrentCluster.Append(1,&mFaceToPolyIndex[targetFaceIndex],1000);


	Tab<int> geomToTVLookup;
	geomToTVLookup.SetCount(mMd->GetNumberGeomVerts());
	for (int i = 0; i < geomToTVLookup.Count(); i++)
	{
		geomToTVLookup[i] = -1;
	}

	int aEdge = -1;
	int bEdge = -1;
	for (int i = 0; i < uvws.Count(); i++)
	{
		if (uvws[i].mIthPolyEdge == ithTargetEdge)
		{
			aEdge = uvws[i].mIthFaceEdge;//(ithPolyEdge+1)%degree);
		}
		//		else if (uvws[i].mGeomVertexIndex == geoVertexIndexB)
		else if (uvws[i].mIthPolyEdge == (ithTargetEdge+1)%uvws.Count())
		{
			bEdge = uvws[i].mIthFaceEdge;//ithPolyEdge);
		}
	}
	for (int i = 0; i < uvws.Count(); i++)
	{
		int geomIndex = -1;
		int uvIndex = -1;
		geomIndex = uvws[i].mGeomVertexIndex;

		
		if (uvws[i].mGeomVertexIndex == geoVertexIndexA)
//		if (uvws[i].mIthPolyEdge == ithTargetEdge)
		{
			uvIndex = vertexIndexA;//mMd->GetFaceTVVert(faceIndex,bEdge);//(ithPolyEdge+1)%degree);
		}
		else if (uvws[i].mGeomVertexIndex == geoVertexIndexB)
//		else if (uvws[i].mIthPolyEdge == (ithTargetEdge+1)%uvws.Count())
		{
			uvIndex = vertexIndexB;//mMd->GetFaceTVVert(faceIndex,aEdge);//ithPolyEdge);
		}
		else
		{
			
			uvIndex = mMd->AddTVVert(t,uvws[i].mUVW, mMod);
		}
		
		geomToTVLookup[geomIndex] = uvIndex;
		//		mMd->AddTVVert(t, uvws[i].mUVW, uvws[i].mFaceIndex, uvws[i].mIthFaceEdge/*i*/, mMod);  
	}

	for (int i = 0; i < mPolygon[mFaceToPolyIndex[targetFaceIndex]]->mFace.Count(); i++)
	{
		int faceIndex = mPolygon[mFaceToPolyIndex[targetFaceIndex]]->mFace[i];
		int degree = mMd->GetFaceDegree(faceIndex);
		for (int j = 0; j < degree; j++)
		{
			int geoIndex = mMd->GetFaceGeomVert(faceIndex,j);
			int newTVIndex = geomToTVLookup[geoIndex];
			mMd->SetFaceTVVert(faceIndex,j,newTVIndex);
		}
	}

/*
	//add the new uvws
	for (int i = 0; i < uvws.Count(); i++)
	{
		//see if edge face else
		if (uvws[i].mIthPolyEdge == ithTargetEdge)
		{
			int tvIndex = mMd->GetFaceTVVert(faceIndex,(ithPolyEdge+1)%degree);
			mMd->SetFaceTVVert(targetFaceIndex,uvws[i].mIthFaceEdge,tvIndex);
		}
		else if (uvws[i].mIthPolyEdge == (ithTargetEdge+1)%uvws.Count())
		{
			int tvIndex = mMd->GetFaceTVVert(faceIndex,ithPolyEdge);
			mMd->SetFaceTVVert(targetFaceIndex,uvws[i].mIthFaceEdge,tvIndex);
		}

		else
		{
			mMd->AddTVVert(t, uvws[i].mUVW, uvws[i].mFaceIndex///targetFaceIndex, uvws[i].mIthFaceEdge//i//, mMod);  
		//do handles
		}		 
	}
	*/

	//mark our polygon U/V edges
	EdgeType eSourceType = mPolygon[polyIndex]->mBorderEdge[ithPolyEdge].mEdgeType;
	if (mPolygon[mFaceToPolyIndex[targetFaceIndex]]->Degree() == 4)//uvws.Count() == 4)
	{
		for (int i = 0; i < 4; i++)
		{
			int edge = (i+ithTargetEdge) % 4;
			int e = (eSourceType +i+2)%4;
			EdgeType eType = (EdgeType) e;

			mPolygon[mFaceToPolyIndex[targetFaceIndex]]->mBorderEdge[edge].mEdgeType = eType;

			EdgeData newEdge;
			newEdge.mPolyIndex = mFaceToPolyIndex[targetFaceIndex];
			newEdge.mIthPolyEdge = edge;
			newEdge.mSelectEdge = false;
			newEdge.mNewEdge = true;
			if (edge == (ithTargetEdge+(uvws.Count()/2))% uvws.Count() ) 
				newEdge.mSelectEdge = true;
			mBorderEdges.Append(1,&newEdge);
		}
	}
	else if (mPolygon[mFaceToPolyIndex[targetFaceIndex]]->Degree() == 3)
	{
		//fix edges for irregular polygons
		//it is a triangle
		//find a perp edge if there is one
		//set that one to our edge type

		//select the edge that is most accute
		for (int i = 0; i < 3; i++)
		{
			int edge = (i+ithTargetEdge) % 3;

			mPolygon[mFaceToPolyIndex[targetFaceIndex]]->mBorderEdge[edge].mEdgeType = kUknown;

			EdgeData newEdge;
			newEdge.mPolyIndex = mFaceToPolyIndex[targetFaceIndex];
			newEdge.mIthPolyEdge = edge;
			newEdge.mSelectEdge = false;
			newEdge.mNewEdge = true;
			mBorderEdges.Append(1,&newEdge);
		}
	}
	else
	{
		int degree = mPolygon[mFaceToPolyIndex[targetFaceIndex]]->Degree();
		for (int i = 0; i < degree; i++)
		{
			int edge = (i+ithTargetEdge) % degree;

			mPolygon[mFaceToPolyIndex[targetFaceIndex]]->mBorderEdge[edge].mEdgeType = kUknown;

			EdgeData newEdge;
			newEdge.mPolyIndex = mFaceToPolyIndex[targetFaceIndex];
			newEdge.mIthPolyEdge = edge;
			newEdge.mSelectEdge = false;
			newEdge.mNewEdge = true;
			mBorderEdges.Append(1,&newEdge);
		}

	}
}

void RegularMap::RemoveUnusedBorderEdges()
{
	//remove  duplicate 

	for (int i = 0; i < mBorderEdges.Count(); i++)
	{
		int polyIndex = mBorderEdges[i].mPolyIndex;
		int ithEdge = mBorderEdges[i].mIthPolyEdge;
		mPolygon[polyIndex]->mBorderEdge[ithEdge].mTempFlag = false;
	}
	for (int i = 0; i < mBorderEdges.Count(); i++)
	{
		int polyIndex = mBorderEdges[i].mPolyIndex;
		int ithEdge = mBorderEdges[i].mIthPolyEdge;

		if (mPolygon[polyIndex]->mBorderEdge[ithEdge].mTempFlag == false)
			mPolygon[polyIndex]->mBorderEdge[ithEdge].mTempFlag = true;
		else
		{
			mBorderEdges.Delete(i,1);
			i--;
		}

	}


	for (int i = 0; i < mBorderEdges.Count(); i++)
	{
		int polyA = mBorderEdges[i].mPolyIndex;
		int faceIndex = mPolygon[mBorderEdges[i].mPolyIndex]->mBorderEdge[mBorderEdges[i].mIthPolyEdge].mOppositeFace;
		if (faceIndex == -1)
		{
			mBorderEdges.Delete(i,1);
			i--;
			continue;
		}
		int polyB = mFaceToPolyIndex[mPolygon[mBorderEdges[i].mPolyIndex]->mBorderEdge[mBorderEdges[i].mIthPolyEdge].mOppositeFace];
		//open edges cannot be border edges
		if ( (polyA == -1) || (polyB == -1) )
		{
			mBorderEdges.Delete(i,1);
			i--;
		}
		else
		{
			if (mProcessedFaces[polyA] && mProcessedFaces[polyB])
			{
				mBorderEdges.Delete(i,1);
				i--;
			}
			else if ((mProcessedFaces[polyA]==FALSE) && (mProcessedFaces[polyB]==FALSE))
			{
				mBorderEdges.Delete(i,1);
				i--;
			}
		}
	}
	
}


void RegularMap::NormalizeUVWs(int ithEdge, Tab<UVWInfo> &uvw)
{
		Point3 a(0,0,0);
		Point3 b(0,0,0);
		a = uvw[ithEdge].mUVW;
		b = uvw[(ithEdge+1) % uvw.Count()].mUVW;

		float xLen = fabs(a.x-b.x);
		float yLen = fabs(a.y-b.y);

		if (xLen < yLen)
		{
			float x = uvw[(ithEdge+2) % uvw.Count()].mUVW.x + uvw[(ithEdge+3) % uvw.Count()].mUVW.x;
			x *= 0.5f;

			uvw[(ithEdge+2) % uvw.Count()].mUVW.x = x;
			uvw[(ithEdge+3) % uvw.Count()].mUVW.x = x;

			uvw[(ithEdge+2) % uvw.Count()].mUVW.y = uvw[(ithEdge+1) % uvw.Count()].mUVW.y;
			uvw[(ithEdge+3) % uvw.Count()].mUVW.y = uvw[(ithEdge) % uvw.Count()].mUVW.y;;

		}
		else
		{
			float y = uvw[(ithEdge+2) % uvw.Count()].mUVW.y + uvw[(ithEdge+3) % uvw.Count()].mUVW.y;
			y *= 0.5f;

			uvw[(ithEdge+2) % uvw.Count()].mUVW.y = y;
			uvw[(ithEdge+3) % uvw.Count()].mUVW.y = y;

			uvw[(ithEdge+2) % uvw.Count()].mUVW.x = uvw[(ithEdge+1) % uvw.Count()].mUVW.x;
			uvw[(ithEdge+3) % uvw.Count()].mUVW.x = uvw[(ithEdge) % uvw.Count()].mUVW.x;

		}

}


bool RegularMap::CheckEdge(int polyIndex, int ithEdge, FaceType level)
{
//make sure the opposite face is not already processed
	int faceIndex = mPolygon[polyIndex]->mBorderEdge[ithEdge].mOppositeFace;
	if (faceIndex == -1)
		return false;
	int oppositePolyIndex = mFaceToPolyIndex[faceIndex];

	if (mProcessedFaces[oppositePolyIndex])
		return false;

	int edgeIndex = mPolygon[polyIndex]->mBorderEdge[ithEdge].mEdgeIndex;

	int va = mMd->GetGeomEdgeVert(edgeIndex,0);
	int vb = mMd->GetGeomEdgeVert(edgeIndex,1);

	bool validEdge = true;

	if (level == kRegular)
	{
		//edge a and b vert need only 4 connected edges
		bool vaValid = false;
		bool vbValid = false;

		if ( ( mVertexConnectionCount[va] == 4 ) ||
			 ( ( mVertexConnectionCount[va] == 3 ) && (mOpenEdgeConnectionCount[va] == 2) ) )
			vaValid = true;

		if ( ( mVertexConnectionCount[vb] == 4 ) ||
			( ( mVertexConnectionCount[vb] == 3 ) && (mOpenEdgeConnectionCount[vb] == 2) ) )
			vbValid = true;

		if ((vaValid == false) || (vbValid == false))
			validEdge = false;

/*
		if (mVertexConnectionCount[va] != 4) 				
			validEdge = false;
		if (mVertexConnectionCount[vb] != 4)
			validEdge = false;
*/

		//opposite face must be quad
		if (mPolygon[oppositePolyIndex]->Degree() != 4)
			validEdge = false;

	}
	else if (level == kPartial)
	{
		//opposite face only needs to be a quad
		//opposite face must be quad
		if (mPolygon[oppositePolyIndex]->Degree() != 4)
			validEdge = false;

	}
	else if (level == kAbnormal)
	{
		//everything else
	}


	return validEdge;
}

bool RegularMap::StartNewCluster(int nfaceIndex)
{
	int polyIndex = mFaceToPolyIndex[nfaceIndex];
	//check to make sure face not already processed	
	mCurrentUnfoldID++;

	TimeValue t = GetCOREInterface()->GetTime();
	if (mProcessedFaces[polyIndex])
		return false;
	if ((polyIndex < 0) || (polyIndex > mPolygon.Count()))
		return false;

	mCurrentCluster.SetCount(0);
	
	//add the mprocessed list to the cluster list

	mCurrentClusterID++;

	//get our face
	
	int degree = mPolygon[polyIndex]->Degree();
	
	int ithEdge = 0;
	//see which edge we want to be our U direction
	Box3 bounds;
	bounds.Init();
	for (int i = 0; i < degree; i++)
	{
		int id = mPolygon[polyIndex]->mBorderEdge[i].mVertexIndex;
		bounds += mMd->GetGeomVert(id);
	}

	int zToU = 0;
	if ( (bounds.Width().z > bounds.Width().x) &&
		 (bounds.Width().z > bounds.Width().y) )
		 zToU = 2;

	float largestDist = 0.0f;
	for (int i = 0; i < degree; i++)
	{	
		int id = mPolygon[polyIndex]->mBorderEdge[i].mVertexIndex;
		int id1 = mPolygon[polyIndex]->mBorderEdge[(i+1)%degree].mVertexIndex;
		Point3 p1 = mMd->GetGeomVert(id);
		Point3 p2 = mMd->GetGeomVert(id1);
		float d  = fabs(p1[zToU] - p2[zToU]);
		if (d > largestDist)
		{
			largestDist = d;
			ithEdge = i;
		}
	}
	
	int a = mPolygon[polyIndex]->mBorderEdge[ithEdge].mVertexIndex;
	int b = mPolygon[polyIndex]->mBorderEdge[(ithEdge+1)%degree].mVertexIndex;
	mPolygon[polyIndex]->mUnfoldID = mCurrentUnfoldID;
	float len = Length(mMd->GetGeomVert(a)-mMd->GetGeomVert(b));
	//transform is world
	Matrix3 tm(1);
	tm.SetRow(0, Point3(0.0f,-1.0f*len,0.0f));
	tm.SetRow(1, Point3(-1.0f,0.0f,0.0f));
	tm.SetRow(2, Point3(0.0f,0.0f,1.0f));

	//map it
	Tab<UVWInfo> uvws;
	MapPolygon(tm,  polyIndex, ithEdge, uvws);

	if (mNormalize && (mPolygon[polyIndex]->Degree() == 4))
	{
		float xLen = fabs(uvws[0].mUVW.x - uvws[1].mUVW.x);
		float yLen = fabs(uvws[0].mUVW.y - uvws[1].mUVW.y);
		if (xLen < yLen)
		{
			float x = uvws[(2) % uvws.Count()].mUVW.x + uvws[(3) % uvws.Count()].mUVW.x;
			x *= 0.5f;

			uvws[(2) % uvws.Count()].mUVW.x = x;
			uvws[(3) % uvws.Count()].mUVW.x = x;


			x = uvws[(2+2) % uvws.Count()].mUVW.x + uvws[(3+2) % uvws.Count()].mUVW.x;
			x *= 0.5f;

			uvws[(2+2) % uvws.Count()].mUVW.x = x;
			uvws[(3+2) % uvws.Count()].mUVW.x = x;


			uvws[(2) % uvws.Count()].mUVW.y = uvws[(+1) % uvws.Count()].mUVW.y;
			uvws[(3) % uvws.Count()].mUVW.y = uvws[(0) % uvws.Count()].mUVW.y;

			uvws[(2+2) % uvws.Count()].mUVW.y = uvws[(1+2) % uvws.Count()].mUVW.y;
			uvws[(3+2) % uvws.Count()].mUVW.y = uvws[(2) % uvws.Count()].mUVW.y;


		}
		else
		{
			float y = uvws[(2) % uvws.Count()].mUVW.y + uvws[(3) % uvws.Count()].mUVW.y;
			y *= 0.5f;

			uvws[(2) % uvws.Count()].mUVW.y = y;
			uvws[(3) % uvws.Count()].mUVW.y = y;

			y = uvws[(2+2) % uvws.Count()].mUVW.y + uvws[(3+2) % uvws.Count()].mUVW.y;
			y *= 0.5f;

			uvws[(2+2) % uvws.Count()].mUVW.y = y;
			uvws[(3+2) % uvws.Count()].mUVW.y = y;

			uvws[(2) % uvws.Count()].mUVW.x = uvws[(1) % uvws.Count()].mUVW.x;
			uvws[(3) % uvws.Count()].mUVW.x = uvws[(0) % uvws.Count()].mUVW.x;

			uvws[(2+2) % uvws.Count()].mUVW.x = uvws[(1+2) % uvws.Count()].mUVW.x;
			uvws[(3+2) % uvws.Count()].mUVW.x = uvws[(2) % uvws.Count()].mUVW.x;
		}
	}

	mProcessedFaces.Set(polyIndex,TRUE);
	mCurrentCluster.Append(1,&polyIndex,1000);

	//copy the uvw to the map faces
	//add the tvverts
	//set the face indices
	Tab<int> geomToTVLookup;
	geomToTVLookup.SetCount(mMd->GetNumberGeomVerts());
	for (int i = 0; i < geomToTVLookup.Count(); i++)
	{
		geomToTVLookup[i] = -1;
	}

	for (int i = 0; i < uvws.Count(); i++)
	{
		int geomIndex = uvws[i].mGeomVertexIndex;
		int uvIndex = mMd->AddTVVert(t,uvws[i].mUVW, mMod);
		geomToTVLookup[geomIndex] = uvIndex;
//		mMd->AddTVVert(t, uvws[i].mUVW, uvws[i].mFaceIndex, uvws[i].mIthFaceEdge/*i*/, mMod);  
	}

	for (int i = 0; i < mPolygon[polyIndex]->mFace.Count(); i++)
	{
		int faceIndex = mPolygon[polyIndex]->mFace[i];
		int degree = mMd->GetFaceDegree(faceIndex);
		for (int j = 0; j < degree; j++)
		{
			int geoIndex = mMd->GetFaceGeomVert(faceIndex,j);
			int newTVIndex = geomToTVLookup[geoIndex];
			mMd->SetFaceTVVert(faceIndex,j,newTVIndex);
		}
	}


	mMd->ClearGeomEdgeSelection();
	mBorderEdges.SetCount(0);
	//mark our polygon U/V edges
	mPolygon[polyIndex]->mCusterID = mCurrentClusterID;
	mPolygon[polyIndex]->mU = 0;
	mPolygon[polyIndex]->mV = 0;

	if (mPolygon[polyIndex]->Degree() == 4)
	{
		for (int i = 0; i < 4; i++)
		{
			int edge = (i+ithEdge)%4;
			EdgeType eType = kUPositive;
			if ( i == 0)
				eType = kVNegative;
			else if ( i == 1)
				eType = kUNegative;
			else if ( i == 2)
				eType = kVPositive;
			else if ( i == 3)
				eType = kUPositive;
			mPolygon[polyIndex]->mBorderEdge[i].mEdgeType = eType;
			//add the border edges
			EdgeData newEdge;
			
			newEdge.mPolyIndex = polyIndex;
			newEdge.mIthPolyEdge = edge;

			newEdge.mSelectEdge = false;
			newEdge.mNewEdge = true;
			mBorderEdges.Append(1,&newEdge);
		}
	}

	mMd->BuildTVEdges();
	mMd->BuildVertexClusterList();
	mMod->RebuildDistCache();

	MoveCurrentClusterToFreeSpace();

	mMod->NotifyDependents(FOREVER,TEXMAP_CHANNEL,REFMSG_CHANGE);
	if (mMod->ip) mMod->ip->RedrawViews(t);
	mMod->InvalidateView();

	ClearEdgeFlags();

	return true;


}


void RegularMap::ArrangeEdges(Tab<int> &edges)
{
	int swapID = 0;


	//this is a two pass we first move the U edges and then the V edges
	for (int k = 0; k > 2; k++)
	{
		//clear the mark flag
		for (int i = 0; i < mPolygon.Count(); i++)
		{
			int degree = mPolygon[i]->Degree();
			for (int j = 0; j < degree; j++)
			{
				mPolygon[i]->mBorderEdge[j].mTempFlag = false;
			}
		}

		//mark all the edges that U or V depending on pass
		for (int i = 0; i < edges.Count(); i++)
		{
			int edgeIndex = edges[i];
			int polyIndex = mBorderEdges[edgeIndex].mPolyIndex;
			int ithPolyEdge = mBorderEdges[edgeIndex].mIthPolyEdge;


			bool addEdge = false;
			if (k == 0)
			{
				if ( (mPolygon[polyIndex]->mBorderEdge[ithPolyEdge].mEdgeType == kUNegative) ||
					(mPolygon[polyIndex]->mBorderEdge[ithPolyEdge].mEdgeType == kUPositive) )
					addEdge = true;
			}
			else
			{
				if ( (mPolygon[polyIndex]->mBorderEdge[ithPolyEdge].mEdgeType == kVNegative) ||
					(mPolygon[polyIndex]->mBorderEdge[ithPolyEdge].mEdgeType == kVPositive) )
					addEdge = true;
			}

			if (addEdge)
			{
				mPolygon[polyIndex]->mBorderEdge[ithPolyEdge].mTempFlag = true;
			}			
		}

		
		//loop through the edges and if the edge has neighbors marked also move
		//them to the front of the list
		for (int i = 0; i < edges.Count(); i++)
		{
			int edgeIndex = edges[i];
			int polyIndex = mBorderEdges[edgeIndex].mPolyIndex;
			int ithPolyEdge = mBorderEdges[edgeIndex].mIthPolyEdge;
			if (mPolygon[polyIndex]->mBorderEdge[ithPolyEdge].mTempFlag)
			{
				int degree = mPolygon[polyIndex]->Degree();
				int prevEdgeIndex = (ithPolyEdge-1);
				if (prevEdgeIndex < 0)
					prevEdgeIndex = degree - 1;
				int nextEdgeIndex = (ithPolyEdge+1)%degree;
				if (mPolygon[polyIndex]->mBorderEdge[prevEdgeIndex].mTempFlag && 
					mPolygon[polyIndex]->mBorderEdge[nextEdgeIndex].mTempFlag )
				{
					if (swapID != i)
					{
						int temp = edges[swapID];
						edges[swapID] = edges[i];
						edges[i] = temp;
						swapID++;
					}
				}
			}				
		}
	}

	//now sort by edges type
	EdgeType cEdge = kVNegative;
	for (int k = 0; k < kUknown; k++)
	{
		for (int i = swapID; i < edges.Count(); i++)
		{
			int edgeIndex = edges[i];
			int polyIndex = mBorderEdges[edgeIndex].mPolyIndex;
			int ithPolyEdge = mBorderEdges[edgeIndex].mIthPolyEdge;

			if (mPolygon[polyIndex]->mBorderEdge[ithPolyEdge].mEdgeType == cEdge)
			{
				int temp = edges[swapID];
				edges[swapID] = edges[i];
				edges[i] = temp;
				swapID++;
			}
		}
		cEdge = (EdgeType)((int)cEdge+1);
	}


}



void RegularMap::BuildBorders( Tab<int> &edges)
{
	

	if (mBorderEdges.Count() ==  0)
		return;

	edges.SetCount(mBorderEdges.Count());
	int leftMostEdge = -1;
	float leftMostValue = 0.0f;
	
	//loop through the edges looking for the left most edge. That will be part of the
	//of the outer border loop
	for (int i = 0; i < mBorderEdges.Count(); i++)
	{
		int polyIndex = mBorderEdges[i].mPolyIndex;
		int ithPolyEdge = mBorderEdges[i].mIthPolyEdge;

		int innerFace = mPolygon[polyIndex]->mBorderEdge[ithPolyEdge].mInnerFace;
		int innerIthFaceEdge = mPolygon[polyIndex]->mBorderEdge[ithPolyEdge].mInnerFaceIthEdge;
		int edgeIndex = mPolygon[polyIndex]->mBorderEdge[ithPolyEdge].mEdgeIndex;
		
		edges[i] = edgeIndex;
		int tvIndex = mMd->GetFaceTVVert(innerFace,innerIthFaceEdge);
		Point3 tvA = mMd->GetTVVert(tvIndex);
		tvIndex = mMd->GetFaceTVVert(innerFace,(innerIthFaceEdge+1)%3);
		Point3 tvB = mMd->GetTVVert(tvIndex);
		float d = tvA.x;
		if (tvB.x < d)
			d = tvB.x;

		if ( (leftMostEdge == -1) || (d < leftMostValue) )
		{
			leftMostEdge = i;
			leftMostValue = d;
		}
	}
	

	//this is our start edge
	int temp = edges[0];
	edges[0] = edges[leftMostEdge];
	edges[leftMostEdge] = temp;

	//now walk around the edges grabbing edges that are attached to the outside loop
	int id = 0;
	int anchor = mMd->GetGeomEdgeVert(edges[0],0);
	for (int i = 1; i < edges.Count(); i++)
	{		
		bool found = false;
		for (int j = i; j < edges.Count(); j++)
		{
			int ta = mMd->GetGeomEdgeVert(edges[j],0);
			int tb = mMd->GetGeomEdgeVert(edges[j],1);
			if (ta == anchor)
			{
				anchor = tb;
				found = true;
			}
			else if (tb == anchor)
			{
				anchor = ta;
				found = true;
			}

			if (found)
			{				
				int temp = edges[i];
				edges[i] = edges[j];
				edges[j] = temp;
				j = edges.Count();
			}
		}

		if (!found)
		{
			edges.SetCount(i);
			i = edges.Count();
		}
	}
}


void RegularMap::Expand(RegularMapExpansions expandBy) 
{

	if (expandBy == regularmap_expand_ring)
	{
		Advance(TRUE, FALSE, TRUE,  FALSE, FALSE,FALSE);
		Advance(FALSE,TRUE,  FALSE, TRUE,  FALSE,FALSE);
		return;
	}

	BitArray faceSel;

	if (expandBy == regularmap_expand_loop) 
	{
		SetLimit(kPartial);
		int opposingEdge = -1;
		EdgeType opposingEdgeType = kUknown;
		BitArray esel = mMd->GetGeomEdgeSelection();
		int ct = esel.NumberSet();
		for (int i = 0; i < mBorderEdges.Count(); i++)
		{
			int polyIndex = mBorderEdges[i].mPolyIndex;
			int ithPolyEdge = mBorderEdges[i].mIthPolyEdge;

			int edgeIndex = mPolygon[polyIndex]->mBorderEdge[ithPolyEdge].mEdgeIndex;
			if (esel[edgeIndex])
			{
				int degree = mPolygon[polyIndex]->mBorderEdge.Count();
				int opposingIthPolyEdge = (ithPolyEdge+(degree/2))%degree;
				opposingEdge = mPolygon[polyIndex]->mBorderEdge[opposingIthPolyEdge].mEdgeIndex;
				opposingEdgeType = mPolygon[polyIndex]->mBorderEdge[opposingIthPolyEdge].mEdgeType;
				i = mBorderEdges.Count();
			}
		}
		if ((opposingEdge != -1) && (opposingEdgeType != kUknown))
		{
			//get our +vertical dir
			//get our -vertical dir
			//expand 
			//uPos vPos uNeg vNeg
			if (opposingEdgeType == kUNegative)
			{
				Advance(FALSE, TRUE, FALSE, FALSE, FALSE,FALSE);
				Advance(FALSE, FALSE, FALSE, TRUE, FALSE,FALSE);
				
				Advance(FALSE, FALSE, TRUE, FALSE, FALSE,FALSE);
				Advance(TRUE, FALSE, FALSE, FALSE, FALSE,FALSE);
			}
			else if (opposingEdgeType == kVNegative)
			{
				Advance(TRUE, FALSE, FALSE, FALSE, FALSE,FALSE);
				Advance(TRUE, FALSE, TRUE, FALSE, FALSE,FALSE);

				Advance(FALSE, FALSE, FALSE, TRUE, FALSE,FALSE);
				Advance(FALSE, TRUE, FALSE, FALSE, FALSE,FALSE);
			}
			else if (opposingEdgeType == kUPositive)
			{
				Advance(FALSE, TRUE, FALSE, FALSE, FALSE,FALSE);
				Advance(FALSE, FALSE, FALSE, TRUE, FALSE,FALSE);

				Advance(TRUE, FALSE, FALSE, FALSE, FALSE,FALSE);
				Advance(FALSE, FALSE, TRUE, FALSE, FALSE,FALSE);
			}
			else if (opposingEdgeType == kVPositive)
			{
				Advance(TRUE, FALSE, FALSE, FALSE, FALSE,FALSE);
				Advance(TRUE, FALSE, TRUE, FALSE, FALSE,FALSE);

				Advance(FALSE, TRUE, FALSE, FALSE, FALSE,FALSE);
				Advance(FALSE, FALSE, FALSE, TRUE, FALSE,FALSE);

			}
		}
		expandBy = regularmap_expand_facesel;
		return;
	}
	else if ( (expandBy == regularmap_expand_outerborder) ||
		(expandBy == regularmap_expand_innerborder) )
	{
		Tab<int> edges;

		BuildBorders(edges);
		BitArray edgeSel;
		edgeSel.SetSize(mMd->GetNumberGeomEdges());
		edgeSel.ClearAll();


		BitArray outerBorder;
		outerBorder = edgeSel;
		for (int i = 0; i < edges.Count(); i++)
		{
			outerBorder.Set(edges[i]);
		}


		for (int i = 0; i < mBorderEdges.Count(); i++)
		{
			int polyIndex = mBorderEdges[i].mPolyIndex;
			int ithPolyEdge = mBorderEdges[i].mIthPolyEdge;

			int edgeIndex = mPolygon[polyIndex]->mBorderEdge[ithPolyEdge].mEdgeIndex;

			if (expandBy == regularmap_expand_outerborder)
			{
				if (outerBorder[edgeIndex])
					edgeSel.Set(edgeIndex);
			}
			else if (expandBy == regularmap_expand_innerborder)
			{
				if (outerBorder[edgeIndex] == false)
					edgeSel.Set(edgeIndex);
			}			
		}

		
		mMd->SetGeomEdgeSelection(edgeSel);
	}
	//with these 2 expansion we just mark faces until the end condition is found
	if ( (expandBy == regularmap_expand_smgrp) ||
		(expandBy == regularmap_expand_peltseams) ||
		 (expandBy == regularmap_expand_planar) )
	{

		BitArray processedPoly;
		processedPoly.SetSize(mPolygon.Count());

		processedPoly.ClearAll();

		//get our clusters 
		BitArray polySel = processedPoly;
		polySel.ClearAll();
		
		for (int i = 0; i < mCurrentCluster.Count(); i++)
		{
			int polyIndex = mCurrentCluster[i];
			processedPoly.Set(polyIndex);
			polySel.Set(polyIndex);
		}



		BitArray edgeSkip = mMd->GetGeomEdgeSelection();
		edgeSkip.ClearAll();

		//keep expanding the selection until it stops growing
		int ct = -1;
		while (ct != polySel.NumberSet())
		{
			ct = polySel.NumberSet();
			//loop through the edges
			for (int i = 0; i < mMd->GetNumberGeomEdges(); i++)
			{
				//if the edge has been processed skip it
				//and we only want faces with 2 edges attached to it
				if ( (edgeSkip[i] == false) && (mMd->GetGeomEdgeNumberOfConnectedFaces(i)==2))
				{
					//get our faces connected to the edge
					int fa = mMd->GetGeomEdgeConnectedFace(i,0);
					int fb = mMd->GetGeomEdgeConnectedFace(i,1);

					//transfer them to a poly index
					int polyIndexA = mFaceToPolyIndex[fa];
					int polyIndexB = mFaceToPolyIndex[fb];

					BOOL aProcessed = polySel[polyIndexA];
					BOOL bProcessed = polySel[polyIndexB];

					//if both faces have been processed we can skip em
					if (aProcessed && bProcessed)
					{
						edgeSkip.Set(i);
					}
					else if (aProcessed ||  bProcessed)
					{
						//we want an edge that has a processed and unprocessed face
						int validFace = -1;
						int sourceFace = -1;
						if ( aProcessed && !bProcessed)
						{
							validFace = fb;
							sourceFace = fa;
						}
						if ( bProcessed && !aProcessed)
						{
							validFace = fa;
							sourceFace = fb;
						}

						// do we meet that condition
						if (validFace != -1)
						{
							
							if (expandBy == regularmap_expand_peltseams)
							{
								//if it is not a pelt seam we can use it 
								if (mMd->mSeamEdges[i] == false)
								{
									if (aProcessed)									
										polySel.Set(polyIndexB);
									else
										polySel.Set(polyIndexA);
								}
							}
							//check by smoothing group
							else if (expandBy == regularmap_expand_smgrp)
							{
								DWORD sourceSmgrp = 0;
								DWORD targetSmgrp = 0;
								if (mMd->GetMesh())
								{
									sourceSmgrp = mMd->GetMesh()->faces[sourceFace].getSmGroup() ;
									targetSmgrp = mMd->GetMesh()->faces[validFace].getSmGroup() ;
								}
								else if (mMd->GetMNMesh())
								{
									sourceSmgrp = mMd->GetMNMesh()->f[sourceFace].smGroup ;
									targetSmgrp = mMd->GetMNMesh()->f[validFace].smGroup ;
								}
								else if (mMd->GetPatch())
								{
									sourceSmgrp = mMd->GetPatch()->patches[sourceFace].smGroup ;
									targetSmgrp = mMd->GetPatch()->patches[validFace].smGroup ;
								}
								
								if (sourceSmgrp & targetSmgrp)
								{
									if (aProcessed)									
										polySel.Set(polyIndexB);
									else
										polySel.Set(polyIndexA);
								}
							}
							//check by normals
							else if (expandBy == regularmap_expand_planar)
							{
								Point3 sourceNormal(0,0,0);
								Point3 targetNormal(0,0,0);
								sourceNormal = mPolygon[mFaceToPolyIndex[sourceFace]]->mGeoNormal;
								targetNormal = mPolygon[mFaceToPolyIndex[validFace]]->mGeoNormal;

								float dot = DotProd(sourceNormal,targetNormal);

								dot = fabs(dot-1.0f);
								if (dot < 0.005f)
								{
									if (aProcessed)									
										polySel.Set(polyIndexB);
									else
										polySel.Set(polyIndexA);
								}
							}
							
						}
						//mark edges and faces as used
						edgeSkip.Set(i);
						processedPoly.Set(polyIndexA);
						processedPoly.Set(polyIndexB);

					}
				}
			}
		}

		//now transfer our selection
		faceSel = mMd->GetFaceSelection();
		faceSel.ClearAll();
		for (int i = 0; i < polySel.GetSize(); i++)
		{
			if (polySel[i])
			{
				for (int j = 0; j < mPolygon[i]->mFace.Count(); j++)
				{
					faceSel.Set(mPolygon[i]->mFace[j]);
				}
			}
		}
	}



	if (expandBy == regularmap_expand_facesel)
	{
		//just copy our face selection
		faceSel = mMd->GetFaceSelection();
	}


	if ( (expandBy == regularmap_expand_facesel) ||
		 (expandBy == regularmap_expand_smgrp) ||
		 (expandBy == regularmap_expand_planar) )
	{
		//convert the face selection to edge selection		
		BitArray edgeSel = mMd->GetGeomEdgeSelection();
		edgeSel.ClearAll();

		//copy the initial border edges into our edge list
		for (int i = 0; i < mBorderEdges.Count(); i++)
		{
			int ithPolyEdge = mBorderEdges[i].mIthPolyEdge;
			int polyIndex = mBorderEdges[i].mPolyIndex;
			int oppositeFace = mPolygon[polyIndex]->mBorderEdge[ithPolyEdge].mOppositeFace;
			int edgeIndex = mPolygon[polyIndex]->mBorderEdge[ithPolyEdge].mEdgeIndex;
			if (faceSel[oppositeFace] && (mMd->GetGeomEdgeNumberOfConnectedFaces(edgeIndex)==2))
				edgeSel.Set(edgeIndex,TRUE);
		}

		//now and edge that shares 2 selected faces we mark
		for (int i = 0; i < mMd->GetNumberGeomEdges(); i++)
		{
			int ct = mMd->GetGeomEdgeNumberOfConnectedFaces(i);
			if (ct == 2)
			{
				int fa = mMd->GetGeomEdgeConnectedFace(i,0);
				int fb = mMd->GetGeomEdgeConnectedFace(i,1);
				if  ( faceSel[fa] && faceSel[fb])
				{
					edgeSel.Set(i,TRUE);
				}
			}
		}
		 mMd->SetGeomEdgeSelection(edgeSel);
	}
	else if (expandBy == regularmap_expand_edgesel)
	{
		//dont have to do anything since we use the edge sel by default to advance
	}


	//advance through our selection until nothing is left
	int lastCount = -1;
	

	//do the faces in u/v direction gives better results when alternating 1 at a time
	for (int i = 0; i < 2; i++)  //we do the expansion in 2 passes first the U direction and then the v
	{
		while (mProcessedFaces.NumberSet() != lastCount)
		{
			lastCount = mProcessedFaces.NumberSet();
			BitArray edgeSel = mMd->GetGeomEdgeSelection();
			BitArray holdEdgeSel = edgeSel;
			
			for (int j = 0; j < mBorderEdges.Count(); j++)
			{
				int ithPolyEdge = mBorderEdges[j].mIthPolyEdge;
				int polyIndex = mBorderEdges[j].mPolyIndex;
				EdgeType edgeType = mPolygon[polyIndex]->mBorderEdge[ithPolyEdge].mEdgeType;
				int edgeIndex = mPolygon[polyIndex]->mBorderEdge[ithPolyEdge].mEdgeIndex;
				if (i == 0)
				{
					if ((edgeType == kUNegative) || (edgeType == kUPositive) || (edgeType == kUknown) )
						edgeSel.Clear(edgeIndex);
				}
				else if (i == 1)
				{
					if ((edgeType == kVNegative) || (edgeType == kVPositive) || (edgeType == kUknown) )
						edgeSel.Clear(edgeIndex);
				}
			}
			mMd->SetGeomEdgeSelection(edgeSel);

			Advance(FALSE, FALSE, FALSE, FALSE, TRUE,TRUE);
			mMd->SetGeomEdgeSelection(holdEdgeSel);
		}	
	}

	lastCount = -1;
	//now do the remaining
	while (mProcessedFaces.NumberSet() != lastCount)
	{
		lastCount = mProcessedFaces.NumberSet();
		BitArray edgeSel = mMd->GetGeomEdgeSelection();
		Advance(FALSE, FALSE, FALSE, FALSE, TRUE,TRUE);
		mMd->SetGeomEdgeSelection(edgeSel);
	}
	mMd->ClearGeomEdgeSelection();
}


void RegularMap::Advance(BOOL uPos, BOOL vPos, BOOL uNeg, BOOL vNeg, BOOL selected, BOOL singleStep)
{
	mCurrentUnfoldID++;
//check to make sure we have something to advance
	if (mBorderEdges.Count() == 0)
		return;

	TimeValue t = GetCOREInterface()->GetTime();
	//hold our UVWs
	//special condition when first start since there is no distinct u/v direction yet
	bool advanceSel = false;

	{
		//see if we have selected edges
		bool done = false;
		while (!done)
		{
			Tab<int> edgesToProcess;	
			if (selected)
			{
				
				for (int i = 0; i < mBorderEdges.Count(); i++)
				{
					int polyIndex = mBorderEdges[i].mPolyIndex;
					int ithEdge = mBorderEdges[i].mIthPolyEdge;
					int index = mPolygon[polyIndex]->mBorderEdge[ithEdge].mEdgeIndex;
					if (mMd->GetGeomEdgeSelected(index))
					{
//make sure the opposite face is not 
//check our level
//always advance a single step
						FaceType advanceLimit = mAdvanceLimit;
						if (singleStep)
							advanceLimit = kAbnormal;
						if (CheckEdge(polyIndex,ithEdge,advanceLimit) )
						{
							edgesToProcess.Append(1,&i,100);
							advanceSel = true;
						}
						else
						{
							CheckEdge(polyIndex,ithEdge,advanceLimit);
							edgesToProcess.SetCount(0,FALSE);
							i = mBorderEdges.Count();
						}

					}
				}
			}

			bool continueAdvancing = false;

			if (uPos || vPos || uNeg || vNeg)
			{
				for (int i = 0; i < mBorderEdges.Count(); i++)
				{
					int polyIndex = mBorderEdges[i].mPolyIndex;
					int ithEdge = mBorderEdges[i].mIthPolyEdge;

					int index = mPolygon[polyIndex]->mBorderEdge[ithEdge].mEdgeIndex;
					EdgeType eType = mPolygon[polyIndex]->mBorderEdge[ithEdge].mEdgeType;
					if ( (uPos && (eType == kUPositive)) || (vPos && (eType == kVPositive))  ||
						 (uNeg && (eType == kUNegative)) || (vNeg && (eType == kVNegative))  )
					{
//check our level
						if (CheckEdge(polyIndex,ithEdge,mAdvanceLimit))
						{
							edgesToProcess.Append(1,&i,100);
							continueAdvancing = true;
						}
						else
						{
							edgesToProcess.SetCount(0,FALSE);
							i = mBorderEdges.Count();
							continueAdvancing = false;
						}
					}
				}
			}

			//fix up selected edges

			if (edgesToProcess.Count() == 0)
				done = TRUE;
			else
			{
				BitArray holdFaces = mProcessedFaces;

				//sort our edges
				ArrangeEdges(edgesToProcess);

				for (int i = 0; i < edgesToProcess.Count(); i++)
				{
					//get our edge
					int edgeIndex = edgesToProcess[i];			
					UnfoldEdge(edgeIndex);
				}

				RemoveUnusedBorderEdges();

				for (int i = 0; i < holdFaces.GetSize(); i++)
				{
					if (holdFaces[i] && mProcessedFaces[i])
						holdFaces.Set(i,FALSE);
					else if (mProcessedFaces[i])
						holdFaces.Set(i,TRUE);
				}
//FIX
				BitArray weldFaces;
				weldFaces.SetSize(mMd->GetNumberFaces());
				weldFaces.ClearAll();
				for (int i = 0; i < mPolygon.Count(); i++)
				{
					//only weld faces that are on the current working  cluster
					if (mPolygon[i]->mCusterID == mCurrentClusterID)
					{
						bool addFaces = false;
						if (mAutoWeld && mProcessedFaces[i])
							addFaces = true;
						else if (holdFaces[i])
							addFaces = true;

						if (addFaces)
						{
							for (int j = 0; j < mPolygon[i]->mFace.Count(); j++)
								weldFaces.Set(mPolygon[i]->mFace[j],TRUE);
						}
					}
				}
				mMd->WeldFaces(weldFaces,mAutoWeld,mAutoWeldLimit,mMod);
/*
				if (mAutoWeld)
					mMd->WeldFaces(mProcessedFaces,mAutoWeld,mAutoWeldLimit,mMod);
				else
					mMd->WeldFaces(holdFaces,mAutoWeld,mAutoWeldLimit,mMod);
*/

				
				if (mNormalize)
					NormalizeEdges();



				if (selected && advanceSel)
				{
					mMd->ClearGeomEdgeSelection();

					for (int i = 0; i < mBorderEdges.Count(); i++)
					{

						if (mBorderEdges[i].mSelectEdge)
						{
							int polyIndex = mBorderEdges[i].mPolyIndex;
							int ith = mBorderEdges[i].mIthPolyEdge;
							
							int edgeIndex = mPolygon[polyIndex]->mBorderEdge[ith].mEdgeIndex;

							if (mMd->GetGeomEdgeNumberOfConnectedFaces(edgeIndex) > 1)
							{
								int oppositeFaceIndex = mPolygon[polyIndex]->mBorderEdge[ith].mOppositeFace;
								int oppositePolyIndex = mFaceToPolyIndex[oppositeFaceIndex];
								if (mProcessedFaces[oppositePolyIndex] == FALSE)
								{
									mMd->SetGeomEdgeSelected(edgeIndex,TRUE);
									continueAdvancing = true;
								}
							}
						}

						mBorderEdges[i].mSelectEdge = false;
					}
				}

				if (singleStep)
					done = true;
				if (continueAdvancing == false)
					done = true;
			}

		}
		
	}


	MoveCurrentClusterToFreeSpace();

	mMd->BuildTVEdges();
	mMd->BuildVertexClusterList();
	mMod->RebuildDistCache();
	
	mMod->SyncTVToGeomSelection(mMd);


/*
	DebugPrint("*******************\n");
	for (int i = 0; i < mBorderEdges.Count(); i++)
	{
		DebugPrint("%d  %d %d\n",i,mBorderEdges[i].mFaceIndex,mBorderEdges[i].mIthFaceEdge);
	}
*/


	mMod->NotifyDependents(FOREVER,TEXMAP_CHANNEL,REFMSG_CHANGE);
	if (mMod->ip) mMod->ip->RedrawViews(t);
	mMod->InvalidateView();

	ClearEdgeFlags();
}





void RegularMap::Display(GraphicsWindow *gw)
{
	
	if ( mPolygon.Count() == 0)
		return;

	DWORD limit = gw->getRndLimits();
	gw->setRndLimits(limit & ~GW_WIREFRAME);

	Tab<Point3> selectedEdges;

	for (int m = 0; m < 4; m++)
	{
		if (m == 0) //unknown
		{
			gw->setColor(LINE_COLOR,0.0f,1.0f,1.0f);
			gw->setColor(FILL_COLOR,0.0f,1.0f,1.0f);
			Material m;
			m.Ka = Point3(0.0f,1.0f,1.0f);
			m.Kd = Point3(0.0f,1.0f,1.0f);
			m.Ks = Point3(0.0f,1.0f,1.0f);
			m.selfIllum = 1.0f;
			gw->setMaterial(m);
		}
		else if (m == 1) //U
		{
			gw->setColor(LINE_COLOR,1.0f,1.0f,0.0f);
			gw->setColor(FILL_COLOR,1.0f,1.0f,0.0f);
			Material m;
			m.Ka = Point3(1.0f,1.0f,0.0f);
			m.Kd = Point3(1.0f,1.0f,0.0f);
			m.Ks = Point3(1.0f,1.0f,0.0f);
			m.selfIllum = 1.0f;
			gw->setMaterial(m);

		}
		else if (m == 2) //V
		{
			gw->setColor(LINE_COLOR,0.0f,0.0f,1.0f);
			gw->setColor(FILL_COLOR,0.0f,0.0f,1.0f);
			Material m;
			m.Ka = Point3(0.0f,0.0f,1.0f);
			m.Kd = Point3(0.0f,0.0f,1.0f);
			m.Ks = Point3(0.0f,0.0f,1.0f);
			m.selfIllum = 1.0f;
			gw->setMaterial(m);

		}
		gw->startTriangles();
		for (int i = 0; i < mBorderEdges.Count(); i++)
		{
			int polyIndex = mBorderEdges[i].mPolyIndex;
			int ith = mBorderEdges[i].mIthPolyEdge;
			
			int degree = mPolygon[polyIndex]->Degree();
			

			int a = mPolygon[polyIndex]->mBorderEdge[ith].mVertexIndex;
			int b = mPolygon[polyIndex]->mBorderEdge[(ith+1)%degree].mVertexIndex;
			EdgeType edgeType = mPolygon[polyIndex]->mBorderEdge[ith].mEdgeType;

			int edgeIndex = mPolygon[polyIndex]->mBorderEdge[ith].mEdgeIndex;
			BOOL edgeSelected = mMd->GetGeomEdgeSelected(edgeIndex);

			if  ( ((m == 0) && (edgeType == kUknown)) ||
				  ((m == 1) && (edgeType == kUPositive)) ||
				  ((m == 1) && (edgeType == kUNegative)) ||
				  ((m == 2) && (edgeType == kVPositive)) ||
				  ((m == 2) && (edgeType == kVNegative)))
			{
				Point3 p[4];
				Point3 pa(0,0,0),pb(0,0,0);
				pa = mMd->GetGeomVert(a);
				pb = mMd->GetGeomVert(b);

				


				Point3 mid = (pa+pb)*0.5f;

				float epsilon = Length(pa-pb) * 0.005;
				
				Point3 shrinkVec = (pb-pa) * (1.0f - mIconSize);

				p[0] = mid;

				//draw the arrow of direction
				Point3  aCenter(0,0,0), bCenter(0,0,0);
				if (!mProcessedFaces[polyIndex])
				{
					aCenter = mPolygon[polyIndex]->mCenter;
					Point3 normOffset = mPolygon[polyIndex]->mGeoNormal * epsilon;
					Point3 offset = (aCenter-mid) *0.05f;
					p[0] = pa + offset + normOffset + shrinkVec;
					p[1] = mid + ((aCenter-mid) * mIconSize) + normOffset;
					p[2] = pb + offset + normOffset - shrinkVec;
					gw->triangle(p,NULL);
					if (edgeSelected)
					{
						Point3 pa(0,0,0);
						pa = p[0] + normOffset;
						selectedEdges.Append(1,&pa,1000);
						pa = p[2] + normOffset;
						selectedEdges.Append(1,&pa,1000);
					}
				}

				int oppositeFace = mPolygon[polyIndex]->mBorderEdge[ith].mOppositeFace;
				int oppositePolyIndex = mFaceToPolyIndex[oppositeFace];
				if ((oppositeFace != -1) && (!mProcessedFaces[oppositePolyIndex]) )
				{
					aCenter = mPolygon[oppositePolyIndex]->mCenter;
					Point3 normOffset = mPolygon[oppositePolyIndex]->mGeoNormal * epsilon;
					Point3 offset = (aCenter - mid) *0.05f;
					p[2] = pa + offset + normOffset + shrinkVec;
					p[1] = mid + ((aCenter-mid) * mIconSize) + normOffset;
					p[0] = pb + offset + normOffset - shrinkVec;
					gw->triangle(p,NULL);
					if (edgeSelected)
					{
						Point3 pa(0,0,0);
						pa = p[0] + normOffset;
						selectedEdges.Append(1,&pa,1000);
						pa = p[2] + normOffset;
						selectedEdges.Append(1,&pa,1000);
					}
				}
			}
		}
		gw->endTriangles();
	}
	
	//draw selected markers
	gw->setColor(LINE_COLOR,1.0f,0.0f,0.0f);
	gw->startSegments();
	for (int i = 0; i < selectedEdges.Count()/2; i++)
	{
		gw->segment(selectedEdges.Addr(i*2),1);
	}
	gw->endSegments();

		//draw the U+  labels
		//draw the U-  labels
		//draw the V-  labels
		//draw the V+  labels

	gw->setColor(LINE_COLOR,0.0f,0.0f,1.0f);
	gw->startSegments();
	
	if ((mPreviewFace != -1) && (mPreviewFace < mPolygon.Count()))
	{
		if (!mProcessedFaces[mPreviewFace])
			mPolygon[mPreviewFace]->Draw(gw, mMd);
	}
	gw->endSegments();

	gw->setColor(LINE_COLOR,1.0f,1.0f,0.0f);
	gw->startSegments();

	Point3 seg[3];
	for (int i = 0; i < mPolygon.Count(); i++)
	{
		//draw the processed faces	

		if (mProcessedFaces[i])
		{
			mPolygon[i]->Draw(gw, mMd);
		}
	}
	gw->endSegments();

	//label the faces
	if (0)
	{
		for (int i = 0; i < mBorderEdges.Count(); i++)
			{
				int faceIndex = mBorderEdges[i].mPolyIndex;
				int ith = mBorderEdges[i].mIthPolyEdge;
				
				int degree = mPolygon[faceIndex]->Degree();
				

				int a = mPolygon[faceIndex]->mBorderEdge[ith].mVertexIndex;
				int b = mPolygon[faceIndex]->mBorderEdge[(ith+1)%degree].mVertexIndex;
				EdgeType edgeType = mPolygon[faceIndex]->mBorderEdge[ith].mEdgeType;

				Point3 normOffset = mPolygon[faceIndex]->mGeoNormal * 0.001f;
				Point3 p = (mMd->GetGeomVert(a) + mMd->GetGeomVert(b) ) *0.5f;
				p += normOffset;
				TSTR info;
				info.printf(_T("%d(%d) -> %d"),i,(int) edgeType,  mBorderEdges[i].mNextEdge);

				gw->text(&p,info);
			}
	}

	gw->setRndLimits(limit);
	
}

class  TempFaceData
{
public:
	int mID;
	int mU,mV;
};


void RegularMap::GetCurrentCluster(Tab<int> &cluster)
{
	cluster = mCurrentCluster;
}
void RegularMap::SetCurrentCluster(Tab<int> &cluster)
{
	mCurrentCluster = cluster;
}


void RegularMap::MoveCurrentClusterToFreeSpace()
{

//get all our faces that are not in the current cluster
	BitArray currentClusterBitArray;
	currentClusterBitArray.SetSize(mMd->GetNumberFaces());
	currentClusterBitArray.ClearAll();

	Box3 clusterBounds;
	Box3 everythingElseBounds;

	clusterBounds.Init();
	everythingElseBounds.Init();

	for (int i = 0; i < mCurrentCluster.Count(); i++)
	{
		for (int j = 0; j < mPolygon[mCurrentCluster[i]]->mFace.Count(); j++)
		{
			currentClusterBitArray.Set(mPolygon[mCurrentCluster[i]]->mFace[j],TRUE);
		}
		//currentClusterBitArray.Set(mCurrentCluster[i],TRUE);
	}
	
	BitArray clusterVerts;
	clusterVerts.SetSize(mMd->GetNumberTVVerts());
	clusterVerts.ClearAll();

	for (int i = 0; i < mMd->GetNumberFaces(); i++)
	{
		int degree = mMd->GetFaceDegree(i);
		for (int j = 0; j < degree; j++)
		{
			int tvid = mMd->GetFaceTVVert(i,j);
			Point3 p = mMd->GetTVVert(tvid);
			if (currentClusterBitArray[i])
			{
				clusterBounds += p;
				clusterVerts.Set(tvid,TRUE);
			}
			else
				everythingElseBounds += p;

		}
	}


	if (everythingElseBounds.IsEmpty() || clusterBounds.IsEmpty())
		return;

	float yOffset = 0.0f;

	yOffset = (everythingElseBounds.pmax.y - clusterBounds.pmin.y);

	yOffset += (clusterBounds.Width().y * 0.1f);

	Point3 offsetVec(0.0,0.0,0.0);
	offsetVec.y = yOffset;

	TimeValue t = GetCOREInterface()->GetTime();
	for (int i = 0; i < clusterVerts.GetSize(); i++)
	{
		if (clusterVerts[i])
		{
			Point3 p = mMd->GetTVVert(i);
			p += offsetVec;	
			mMd->SetTVVert(t,i,p,mMod);
		}
	}
}

int RegularMap::GetAutoFit()
{
	return mAutoFit;
}
void RegularMap::SetAutoFit(int autofit)
{
	mAutoFit = autofit;
}

void RegularMap::ResetFaces()
{
	//get the face selection
	BitArray tempFaceSel = mMd->GetFaceSel();

	BitArray polySel;
	polySel.SetSize(mProcessedFaces.GetSize());
	polySel.ClearAll();
	for (int i = 0; i < tempFaceSel.GetSize(); i++)
	{
		if (tempFaceSel[i])
		{
			int polyIndex = mFaceToPolyIndex[i];
			polySel.Set(polyIndex,TRUE);
		}
	}

	//send those faces back to thre process list
	Tab<int> removedFaces;
	for (int i = 0 ; i < mProcessedFaces.GetSize(); i++)
	{
		if (polySel[i] && mProcessedFaces[i] && mSelectedPolys[i])
		{
			removedFaces.Append(1,&i,1000);
			mProcessedFaces.Set(i,FALSE);
		}
	}

	//remove unused edges
	RemoveUnusedBorderEdges();

	for (int i = 0; i < mPolygon.Count(); i++)
	{
		int degree = mPolygon[i]->Degree();
		for (int j = 0; j < degree; j++)
		{
			mPolygon[i]->mBorderEdge[j].mTempFlag = false;
		}
	}

	//add edges that are now open but not flagged
	//loop through existing edges marking those faces
	for (int i = 0; i < mBorderEdges.Count(); i++)
	{
		int polyIndex = mBorderEdges[i].mPolyIndex;
		int ith = mBorderEdges[i].mIthPolyEdge;
		mPolygon[polyIndex]->mBorderEdge[ith].mTempFlag = true;
	}

	for (int i = 0; i < mPolygon.Count(); i++)
	{
		if (mProcessedFaces[i])
		{
			int degree = mPolygon[i]->Degree();
			for (int j = 0; j < degree; j++)
			{
				int oppositePoly = mFaceToPolyIndex[mPolygon[i]->mBorderEdge[j].mOppositeFace];
				if (!mProcessedFaces[oppositePoly])
				{
					if (mPolygon[i]->mBorderEdge[j].mTempFlag == false)
					{
						//add edge
						mPolygon[i]->mBorderEdge[j].mTempFlag = true;

						EdgeData newEdge;
						newEdge.mPolyIndex = i;
						newEdge.mIthPolyEdge = j;
						newEdge.mSelectEdge = false;
						newEdge.mNewEdge = false;
						mBorderEdges.Append(1,&newEdge);
					}
				}
			}
		}
	}

	ClearEdgeFlags();
}

void RegularMap::ClearEdgeFlags()
{
	for (int i = 0; i < mBorderEdges.Count(); i++)
	{
		mBorderEdges[i].mNewEdge = false;
		mBorderEdges[i].mSelectEdge = false;
	}
}

void RegularMap::GetFaceEdge(int polyIndex, int ithPolyEdge, int &faceIndex, int &ithFaceEdge)
{
	//get the inner face
	if (ithPolyEdge >= mPolygon[polyIndex]->mBorderEdge.Count())
		return;
	faceIndex = mPolygon[polyIndex]->mBorderEdge[ithPolyEdge].mInnerFace;
	int geomVertexID = mPolygon[polyIndex]->mBorderEdge[ithPolyEdge].mVertexIndex;
	int degree = mMd->GetFaceDegree(faceIndex);
	for (int i = 0; i < degree; i++)
	{
		int testVertexID = mMd->GetFaceGeomVert(faceIndex,i);
		if (testVertexID == geomVertexID)
		{
			ithFaceEdge = i;
			i = degree;
		}
	}

}

//--FIX Here
void RegularMap::BuildEdgeConnectionInfo()
{
	Tab<int> vertEdgeConnection;
	vertEdgeConnection.SetCount(mMd->GetNumberTVVerts()*2);
	for (int i = 0; i < vertEdgeConnection.Count(); i++)
	{
		vertEdgeConnection[i] = -1;
	}

	Tab<int> countList;
	countList.SetCount(mMd->GetNumberTVVerts());

	for (int i = 0; i < countList.Count(); i++)
	{
		countList[i] = 0;
	}
	for (int i = 0; i < mBorderEdges.Count(); i++)
	{
		int polyIndex = mBorderEdges[i].mPolyIndex;
		int ithEdge = mBorderEdges[i].mIthPolyEdge;
		int faceIndex = -1;
		int ithFaceEdge = -1;
		GetFaceEdge(polyIndex,ithEdge,faceIndex,ithFaceEdge);

		int degree = mMd->GetFaceDegree(faceIndex);
		int a = mMd->GetFaceTVVert(faceIndex,ithFaceEdge);
		int b = mMd->GetFaceTVVert(faceIndex,(ithFaceEdge+1)%degree);
		countList[a] += 1;
		countList[b] += 1;

	}

	for (int i = 0; i < mBorderEdges.Count(); i++)
	{
		int polyIndex = mBorderEdges[i].mPolyIndex;
		int ithPolyEdge = mBorderEdges[i].mIthPolyEdge;

		int faceIndex = -1;
		int ithFaceEdge = -1;
		GetFaceEdge(polyIndex,ithPolyEdge,faceIndex,ithFaceEdge);

		int degree = mMd->GetFaceDegree(faceIndex);
		int a = mMd->GetFaceTVVert(faceIndex,ithFaceEdge);
		int b = mMd->GetFaceTVVert(faceIndex,(ithFaceEdge+1)%degree);
/*
		if ((a == 885) || (b == 885))
			DebugPrint("Error edge face %d ith %d \n",faceIndex,ithEdge);
		if ((a == 933) || (b == 933))
			DebugPrint("Error edge face %d ith %d \n",faceIndex,ithEdge);
*/
	}



	for (int i = 0; i < mBorderEdges.Count(); i++)
	{
		int polyIndex = mBorderEdges[i].mPolyIndex;
		int ithPolyEdge = mBorderEdges[i].mIthPolyEdge;

		int faceIndex = -1;
		int ithFaceEdge = -1;
		GetFaceEdge(polyIndex,ithPolyEdge,faceIndex,ithFaceEdge);

		int degree = mMd->GetFaceDegree(faceIndex);
		int a = mMd->GetFaceTVVert(faceIndex,ithFaceEdge);
		int b = mMd->GetFaceTVVert(faceIndex,(ithFaceEdge+1)%degree);

		if (vertEdgeConnection[a*2] == -1)
			vertEdgeConnection[a*2] = i;
		else if (vertEdgeConnection[a*2+1] == -1)
			vertEdgeConnection[a*2+1] = i;
//		else
//			DebugPrint("Error edge %d\n",i);

		if (vertEdgeConnection[b*2] == -1)
			vertEdgeConnection[b*2] = i;
		else if (vertEdgeConnection[b*2+1] == -1)
			vertEdgeConnection[b*2+1] = i;
//		else
//			DebugPrint("Error edge %d\n",i);
	}

	for (int i = 0; i < vertEdgeConnection.Count()/2; i++)
	{
		int id = vertEdgeConnection[i*2];		
		int id1 = vertEdgeConnection[i*2+1];
/*
		if ( (id != -1) || (id1 != -1) )
		{
			DebugPrint("TV Vert connected to edges %d %d \n",i,id,id1);
		}
*/
	}

	for (int i = 0; i < mBorderEdges.Count(); i++)
	{
		mBorderEdges[i].mPrevEdge = -1;
		mBorderEdges[i].mNextEdge = -1;

	}

	for (int i = 0; i < mBorderEdges.Count(); i++)
	{
		int polyIndex = mBorderEdges[i].mPolyIndex;
		int ithPolyEdge = mBorderEdges[i].mIthPolyEdge;

		int faceIndex = -1;
		int ithFaceEdge = -1;
		GetFaceEdge(polyIndex,ithPolyEdge,faceIndex,ithFaceEdge);

		int degree = mMd->GetFaceDegree(faceIndex);
		int a = mMd->GetFaceTVVert(faceIndex,ithFaceEdge);
		int b = mMd->GetFaceTVVert(faceIndex,(ithFaceEdge+1)%degree);

		int edge = vertEdgeConnection[(a*2)];
		int edge1 = vertEdgeConnection[(a*2)+1];
		if  ( edge != -1)
		{
			if (edge != i)
				mBorderEdges[i].mPrevEdge = edge;
			else  if (edge1 != -1)
				mBorderEdges[i].mPrevEdge = edge1;
		}

		edge = vertEdgeConnection[(b*2)];
		edge1 = vertEdgeConnection[(b*2)+1];
		if  ( edge != -1)
		{
			if (edge != i)
				mBorderEdges[i].mNextEdge = edge;
			else if (edge1 != -1)
				mBorderEdges[i].mNextEdge = edge1;
		}
	}

}

void RegularMap::NormalizeEdges()
{
	TimeValue t = GetCOREInterface()->GetTime();
//make sure our connection info is filled out
	BuildEdgeConnectionInfo();


	//check face attached to edge to make sure it is regular
	//if it is not regular do not normalize it
	for (int i = 0; i < mBorderEdges.Count(); i++)
	{		
		if (mBorderEdges[i].mNewEdge)
		{
			int polyIndex = mBorderEdges[i].mPolyIndex;
			int ithPolyEdge = mBorderEdges[i].mIthPolyEdge;

			int degree = mPolygon[polyIndex]->Degree();

			if (degree != 4)
				mBorderEdges[i].mNewEdge = false;
			else
			{
				EdgeType etype = mPolygon[polyIndex]->mBorderEdge[0].mEdgeType;
				for (int i = 1; i < 4; i++)
				{
					EdgeType checkType = mPolygon[polyIndex]->mBorderEdge[i].mEdgeType;
					EdgeType currentType = EdgeType (((int)etype + i)%degree);
					if (currentType != checkType)
						mBorderEdges[i].mNewEdge = false;
				}
			}			
		}
	}

	{
		int count = 0;
		int current = 0;
		BitArray processedEdges;
		processedEdges.SetSize(mBorderEdges.Count());
		processedEdges.ClearAll();

		while (count != mBorderEdges.Count() && current != -1)
		{
			int polyIndex = mBorderEdges[current].mPolyIndex;
			int ithPolyEdge = mBorderEdges[current].mIthPolyEdge;
			processedEdges.Set(current,TRUE);

			EdgeType etype = mPolygon[polyIndex]->mBorderEdge[ithPolyEdge].mEdgeType;

			current = mBorderEdges[current].mNextEdge;
			if (current == -1)
			{
				for (int i = 0; i < mBorderEdges.Count(); i++)
				{
					if (!processedEdges[i])
					{
						current = i;
						i = mBorderEdges.Count();
					}
				}
			}
			count++;
		}
	}

//now get our clusters
	bool done = false;
	while (!done)
	{
		int startEdge = -1;
		Tab<int> edgeList;
		int ct = 0;

		for (int i = 0; i < mBorderEdges.Count(); i++)
		{
			if (mBorderEdges[i].mNewEdge)
			{
				if (startEdge == -1)
					startEdge = i;
				ct++;
			}
		}

		if (ct == 1) 
		{
			mBorderEdges[startEdge].mNewEdge = false;
			continue;
		}

		if (startEdge != -1)
		{
			int initialEdge = startEdge;
			int faceIndex = mBorderEdges[startEdge].mPolyIndex;
			int ith = mBorderEdges[startEdge].mIthPolyEdge;
			EdgeType edgeType = mPolygon[faceIndex]->mBorderEdge[ith].mEdgeType;
			//walk back to the start
			int prevEdge = mBorderEdges[startEdge].mPrevEdge;
			bool doneGettingStart = false;
			if (prevEdge != -1)
			{
				do
				{
					bool newEdge = mBorderEdges[prevEdge].mNewEdge;
					int prevPolyIndex = mBorderEdges[prevEdge].mPolyIndex;
					int prevIth = mBorderEdges[prevEdge].mIthPolyEdge;
					EdgeType prevEdgeType = mPolygon[prevPolyIndex]->mBorderEdge[prevIth].mEdgeType;

					if (prevEdge == initialEdge)
						doneGettingStart = true;
					else if (newEdge && prevEdgeType == edgeType)
					{
						doneGettingStart = false;
						startEdge = prevEdge;
						prevEdge = mBorderEdges[startEdge].mPrevEdge;
						if (prevEdge == -1)
							doneGettingStart = true;

					}
					else
						doneGettingStart = true;
				}while (doneGettingStart != true);
			}

			Tab<int> edgeList;
			if (startEdge != -1)
			{
				edgeList.Append(1,&startEdge);
				mBorderEdges[startEdge].mNewEdge = false;

				bool doneGatheringEdges = false;
				do 
				{
					int nextEdge = mBorderEdges[startEdge].mNextEdge;
					if (nextEdge == -1)
					{
						doneGatheringEdges = true;
					}
					else
					{
						bool newEdge = mBorderEdges[nextEdge].mNewEdge;
						int nextPolyIndex = mBorderEdges[nextEdge].mPolyIndex;
						int nextIth = mBorderEdges[nextEdge].mIthPolyEdge;
						EdgeType nextEdgeType = mPolygon[nextPolyIndex]->mBorderEdge[nextIth].mEdgeType;
						if (newEdge && nextEdgeType == edgeType)
						{
							doneGatheringEdges = false;
							mBorderEdges[nextEdge].mNewEdge = false;
							startEdge = nextEdge;						
							edgeList.Append(1,&nextEdge,1000);
						}
						else
							doneGatheringEdges = true;
					}
				} while (doneGatheringEdges != true);

				if (edgeList.Count() < 2)
				{
					mBorderEdges[edgeList[0]].mNewEdge = false;
					continue;
				}


				Tab<int> tvVerts;
				Tab<int> geoVerts;
				for (int i = 0; i < edgeList.Count(); i++)
				{					
					int edge = edgeList[i];
					
					int polyIndex = mBorderEdges[edge].mPolyIndex;
					int ithPolyEdge = mBorderEdges[edge].mIthPolyEdge;

					int faceIndex = -1;
					int ithFaceEdge = -1;
					GetFaceEdge(polyIndex,ithPolyEdge,faceIndex,ithFaceEdge);

					int degree = mMd->GetFaceDegree(faceIndex);
					int tva = mMd->GetFaceTVVert(faceIndex,ithFaceEdge);
					int tvb = mMd->GetFaceTVVert(faceIndex,(ithFaceEdge+1)%degree);
					int geoa = mMd->GetFaceGeomVert(faceIndex,ithFaceEdge);
					int geob = mMd->GetFaceGeomVert(faceIndex,(ithFaceEdge+1)%degree);

					if ( i == 0)
					{
						tvVerts.Append(1,&tva,1000);
						tvVerts.Append(1,&tvb,1000);
					}
					else if ( i == 1)
					{
						//see if we need to reorder the start verts
						int nextVert = -1;
						if (tva == tvVerts[0])
						{
							int swap = tvVerts[0];
							tvVerts[0] = tvVerts[1];
							tvVerts[1] =  swap;

							nextVert = tvb;
						}
						else if (tvb == tvVerts[0])
						{
							int swap = tvVerts[0];
							tvVerts[0] = tvVerts[1];
							tvVerts[1] =  swap;

							nextVert = tva;
						}
						else if (tva == tvVerts[1])
						{
							nextVert = tvb;
						}
						else if (tvb == tvVerts[1])
						{
							nextVert = tva;
						}

						tvVerts.Append(1,&nextVert,1000);
					}
					else 
					{
						//see if we need to reorder the start verts
						int nextVert = -1;
						if (tva == tvVerts[tvVerts.Count()-1])
						{
							nextVert = tvb;
						}
						else if (tvb == tvVerts[tvVerts.Count()-1])
						{
							nextVert = tva;
						}

						tvVerts.Append(1,&nextVert,1000);
					}
				}

				//get our geom and tv length
				float geoLen = 0.0f;
				float tvLen = 0.0f;
				Box3 tvBounds;
				tvBounds.Init();

				//get our bounds
				for (int i = 0; i < edgeList.Count(); i++)
				{					
					int edge = edgeList[i];
					
					int polyIndex = mBorderEdges[edge].mPolyIndex;
					int ithPolyEdge = mBorderEdges[edge].mIthPolyEdge;

					int faceIndex = -1;
					int ithFaceEdge = -1;
					GetFaceEdge(polyIndex,ithPolyEdge,faceIndex,ithFaceEdge);

					int degree = mMd->GetFaceDegree(faceIndex);//mPolygon[faceIndex]->Degree();
					int tva = mMd->GetFaceTVVert(faceIndex,ithFaceEdge);
					int tvb = mMd->GetFaceTVVert(faceIndex,(ithFaceEdge+1)%degree);
					int geoa = mMd->GetFaceGeomVert(faceIndex,ithFaceEdge);
					int geob = mMd->GetFaceGeomVert(faceIndex,(ithFaceEdge+1)%degree);
					tvBounds += mMd->GetTVVert(tva);
					tvBounds += mMd->GetTVVert(tvb);
					geoLen += Length(mMd->GetGeomVert(geoa)-mMd->GetGeomVert(geob));
				}

				//now normalize the edge vertices
				float xLen = 0.0f;
				float yLen = 0.0f;
				xLen = tvBounds.Width().x;
				yLen = tvBounds.Width().y;
				int axis = 0;
				if (yLen < xLen)
					axis = 1;


				float val = (tvBounds.pmax[axis] + tvBounds.pmin[axis]) * 0.5f;

				//compute the uV length
				for (int i = 0; i < edgeList.Count(); i++)
				{
					
					int edge = edgeList[i];
					int polyIndex = mBorderEdges[edge].mPolyIndex;
					int ithPolyEdge = mBorderEdges[edge].mIthPolyEdge;


					int faceIndex = -1;
					int ithFaceEdge = -1;
					GetFaceEdge(polyIndex,ithPolyEdge,faceIndex,ithFaceEdge);

					int degree = mMd->GetFaceDegree(faceIndex);// mPolygon[faceIndex]->Degree();

					int tva = mMd->GetFaceTVVert(faceIndex,ithFaceEdge);
					int tvb = mMd->GetFaceTVVert(faceIndex,(ithFaceEdge+1)%degree);
					Point3 pa = mMd->GetTVVert(tva);
					pa[axis] = val;
					mMd->SetTVVert(t,tva,pa,mMod);

					Point3 pb = mMd->GetTVVert(tvb);
					pb[axis] = val;
					mMd->SetTVVert(t,tvb,pb,mMod);
					tvLen += Length(pb-pa);

				}

				//respace		
				Point3 edgeVec(0,0,0);
				Point3 startVert(0,0,0);
				Point3 endVert(0,0,0);
				startVert = mMd->GetTVVert(tvVerts[0]);
				endVert = mMd->GetTVVert(tvVerts[tvVerts.Count()-1]);
				edgeVec = endVert-startVert;
				float runningPer = 0.0;
				for (int i = 0; i < edgeList.Count()-1; i++)
				{					
					int edge = edgeList[i];

					int polyIndex = mBorderEdges[edge].mPolyIndex;
					int ithPolyEdge = mBorderEdges[edge].mIthPolyEdge;

					int faceIndex = -1;
					int ithFaceEdge = -1;
					GetFaceEdge(polyIndex,ithPolyEdge,faceIndex,ithFaceEdge);

					int degree = mMd->GetFaceDegree(faceIndex);//mPolygon[faceIndex]->Degree();
					int geoa = mMd->GetFaceGeomVert(faceIndex,ithFaceEdge);
					int geob = mMd->GetFaceGeomVert(faceIndex,(ithFaceEdge+1)%degree);

					float per = 0.0f;
					float geoDist = Length(mMd->GetGeomVert(geoa)-mMd->GetGeomVert(geob));
					per = geoDist/geoLen;
					runningPer += per;
					Point3 newPoint = startVert + (edgeVec * runningPer);

					mMd->SetTVVert(t,tvVerts[i+1],newPoint,mMod);
				}


		
			}
		}
		if (startEdge == -1)
			done = true;
	}

	//normalize the edge V
	//normalize the edge U

}


void RegularMap::FitView(int level)
{
	if (level == 1)
	{
		BitArray newFaces;
		newFaces.SetSize(mMd->GetNumberFaces());
		newFaces.ClearAll();
		for (int i = 0; i < mPolygon.Count(); i++)
		{
			if (mPolygon[i]->mUnfoldID == mCurrentUnfoldID)
			{
				for (int j = 0; j < mPolygon[i]->mFace.Count(); j++)
				{
					newFaces.Set(mPolygon[i]->mFace[j],TRUE);
				}
				
			}
		}

		if (newFaces.NumberSet() > 0)
		{

			BitArray holdFaces = mMd->GetFaceSelection();
			BitArray holdEdges = mMd->GetGeomEdgeSelection();
			BitArray holdTVEdges = mMd->GetTVEdgeSelection();
			int holdSelLevel = mMod->fnGetTVSubMode();
			mMod->fnSetTVSubMode(TVFACEMODE);
			mMd->SetFaceSel(newFaces);

			mMod->fnFitSelected();

			mMod->fnSetTVSubMode(holdSelLevel);
			mMd->SetFaceSel(holdFaces);
			mMd->SetGeomEdgeSelection(holdEdges);
			mMd->SetTVEdgeSelection(holdTVEdges);
		}


	}
	else if (level == 2)
	{
		mMod->fnFit();
	}

}

void RegularMap::SetPos()
{
	if (mWindowPos.length != 0) 
		SetWindowPlacement(mHwnd,&mWindowPos);
	else
		CenterWindow(mHwnd,GetCOREInterface()->GetMAXHWnd());
}
void RegularMap::SavePos()
{
	mWindowPos.length = sizeof(WINDOWPLACEMENT); 
	GetWindowPlacement(mHwnd,&mWindowPos);

}


void PolygonInfo::Draw(GraphicsWindow *gw, MeshTopoData*	md)
{
	Point3 seg[3];
	int degree = Degree();
	Point3 normOffset = mGeoNormal * 0.001f;
	if (degree == 4)
	{
		int index = md->GetFaceGeomVert(mBorderEdge[0].mInnerFace, mBorderEdge[0].mInnerFaceIthEdge);//mFace[0],0);			
		seg[0] = md->GetGeomVert(index) + normOffset;
		index = md->GetFaceGeomVert(mBorderEdge[2].mInnerFace, mBorderEdge[2].mInnerFaceIthEdge);//mFace[0],2);			
		seg[1] = md->GetGeomVert(index) + normOffset;
		gw->segment(seg,1);

		index = md->GetFaceGeomVert(mBorderEdge[1].mInnerFace, mBorderEdge[1].mInnerFaceIthEdge);//mFace[0],1);			
		seg[0] = md->GetGeomVert(index) + normOffset;
		index = md->GetFaceGeomVert(mBorderEdge[3].mInnerFace, mBorderEdge[3].mInnerFaceIthEdge);//mFace[0],3);			
		seg[1] = md->GetGeomVert(index) + normOffset;
		gw->segment(seg,1);
	}
	else if (degree > 0)
	{
		Point3 center(0,0,0);
		for (int j = 0; j < degree; j++)
		{
			int index = md->GetFaceGeomVert(mBorderEdge[j].mInnerFace, mBorderEdge[j].mInnerFaceIthEdge);//mFace[0],j);			
			center += md->GetGeomVert(index);
		}

		center = center/degree;
		center += normOffset;

		seg[0] = center;

		for (int j = 0; j < degree; j++)
		{
			int index = md->GetFaceGeomVert(mBorderEdge[j].mInnerFace, mBorderEdge[j].mInnerFaceIthEdge);//mFace[0],j);			
			seg[1] = md->GetGeomVert(index) + normOffset;
			gw->segment(seg,1);
		}
	}
}