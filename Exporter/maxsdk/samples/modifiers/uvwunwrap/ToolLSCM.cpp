
/*

Copyright [2010] Autodesk, Inc.  All rights reserved.

Use of this software is subject to the terms of the Autodesk license agreement 
provided at the time of installation or download, or which otherwise accompanies 
this software in either electronic or hard copy form.   

	

*/



#include "ToolLSCM.h"
#include "unwrap.h"
#include "PerformanceTools.h"
#include "modsres.h"

#include "SuperLu_SRC/slu_sdefs.h"
#include "SuperLu_SRC/slu_util.h"


extern float AreaOfTriangle(Point3 a, Point3 b, Point3 c);


InteractiveStartRestoreObj::InteractiveStartRestoreObj(UnwrapMod *m)
{
	mMod = m;
}
void InteractiveStartRestoreObj::Restore(int isUndo)
{
	if (mMod->fnGetMapMode() == LSCMMAP)								
	{
		mMod->fnSetMapMode(NOMAP);
	}
}
void InteractiveStartRestoreObj::Redo()
{
	if (mMod->fnGetMapMode() != LSCMMAP)								
	{
		mMod->fnSetMapMode(LSCMMAP);
	}

}
void InteractiveStartRestoreObj::EndHold()
{

}
TSTR InteractiveStartRestoreObj::Description()
{
	return TSTR(GetString(IDS_PW_LSCM));
}

void UnwrapMod::LSCMForceResolve()
{
	if (fnGetMapMode() == LSCMMAP)
		mLSCMTool.Solve(true);
}


void UnwrapMod::fnLSCMInteractive(BOOL start)
{


	Tab<MeshTopoData*> mdList;
	mdList.SetCount(mMeshTopoData.Count());
	for (int i = 0; i < mMeshTopoData.Count(); i++)
	{
		mdList[i] = mMeshTopoData[i];
	}
	if (start)
	{
		if (theHold.IsSuspended() == FALSE)
		{
			theHold.Begin();
			ClearAFlag(A_HELD);
			HoldPointsAndFaces();
			theHold.Put(new InteractiveStartRestoreObj(this));
			theHold.Accept(GetString(IDS_PW_LSCM));
		}
		BOOL autoEdit = FALSE;
		pblock->GetValue(unwrap_peel_autoedit,0,autoEdit,FOREVER);
		if (hDialogWnd == NULL && autoEdit)
			fnEdit();
		mLSCMTool.Start(true,this,mdList);
	}
	else
		mLSCMTool.End();
}
void UnwrapMod::fnLSCMSolve()
{
	BOOL autoEdit = FALSE;
	pblock->GetValue(unwrap_peel_autoedit,0,autoEdit,FOREVER);
	if (hDialogWnd == NULL && autoEdit)
		fnEdit();

	theHold.Begin();
	ClearAFlag(A_HELD);
	HoldPointsAndFaces();
	theHold.Accept(GetString(IDS_PW_LSCM));

	Tab<MeshTopoData*> mdList;
	mdList.SetCount(mMeshTopoData.Count());
	for (int i = 0; i < mMeshTopoData.Count(); i++)
	{
		mdList[i] = mMeshTopoData[i];
	}
	mLSCMTool.Start(true,this,mdList);
	mLSCMTool.End();
}
void UnwrapMod::fnLSCMReset()
{
	BOOL autoEdit = FALSE;
	pblock->GetValue(unwrap_peel_autoedit,0,autoEdit,FOREVER);
	if (hDialogWnd == NULL && autoEdit)
		fnEdit();

	theHold.Begin();
	ClearAFlag(A_HELD);
	HoldPointsAndFaces();
	theHold.Accept(GetString(IDS_PW_LSCM));
	Tab<MeshTopoData*> mdList;
	mdList.SetCount(mMeshTopoData.Count());
	for (int i = 0; i < mMeshTopoData.Count(); i++)
	{
		mdList[i] = mMeshTopoData[i];
	}
	mLSCMTool.Start(false,this,mdList);
	mLSCMTool.End();	
}

void UnwrapMod::fnLSCMInvalidateTopo(MeshTopoData *md)
{
	mLSCMTool.InvalidateTopo(md);
}

LSCMFace::LSCMFace()
{
	mEdge[0].mIthEdge = -1;
	mEdge[1].mIthEdge = -1;
	mEdge[2].mIthEdge = -1;


	mEdge[0].mGeoAngle = 0.0f;
	mEdge[1].mGeoAngle = 0.0f;
	mEdge[2].mGeoAngle = 0.0f;

	mEdge[0].mPinned = false;
	mEdge[1].mPinned = false;
	mEdge[2].mPinned = false;

	mPolyIndex = -1;
}

LSCMClusterData::LSCMClusterData()
{
	mNumberCoefficients = 0;
	mValidGeom = true;
	mSuperLuLandUAllocated = false;
	mNeedToResolve = true;
}

LSCMClusterData::~LSCMClusterData()
{
	if (mSuperLuLandUAllocated)
	{
		Destroy_SuperNode_Matrix(&(mL));
		Destroy_CompCol_Matrix(&(mU));

		StatFree(&(mSuperLuStats));
	}
	mSuperLuLandUAllocated = false;
}

void LSCMClusterData::Reset()
{
	mFaceData.SetCount(0);
}

int		LSCMClusterData::NumberFaces()
{
	return mFaceData.Count();
}


LSCMFace *LSCMClusterData::GetFace(int index)
{
	if ( (index < 0) || (index >= mFaceData.Count()))
		return NULL;
	return mFaceData.Addr(index);
}

void LSCMClusterData::Invalidate(BitArray changedTVVerts, MeshTopoData *md)
{
	BitArray usedPoly;
	usedPoly.SetSize(md->GetNumberFaces());
	usedPoly.ClearAll();

	//Loop through this cluster face list and see if shares any of the changed
	//tvverts.  If so the cluster needs to be invalidated
	for (int i = 0; i < mFaceData.Count(); i++)
	{
		int polyIndex = mFaceData[i].mPolyIndex;
		if (usedPoly[polyIndex] == false)
		{
			usedPoly.Set(polyIndex);
			int degree = md->GetFaceDegree(polyIndex);
			for (int j = 0; j < degree; j++)
			{
				int tvIndex = md->GetFaceTVVert(polyIndex,j);
				if (tvIndex < changedTVVerts.GetSize())
				{
					if (changedTVVerts[tvIndex])
					{
						mNeedToResolve = true;
						return;
					}
				}
			}
		}
	}
	mNeedToResolve = false;
}

//forces the system to invalidate
void LSCMClusterData::Invalidate()
{
	mNeedToResolve = true;
}

int LSCMClusterData::TempPinCount()
{
	return mTempPins.Count();
}


bool LSCMClusterData::CheckPoly(int polyIndex, MeshTopoData *md)
{
	//degenerate and dead faces should be skipped
	if (md->GetFaceDead(polyIndex))
		return false;

	//we dont process hidden faces
	if (md->GetFaceHidden(polyIndex) == TRUE)
		return false;

	//make sure no shared vertices or zero length edges
	BitArray verts;
	verts.SetSize(md->GetNumberGeomVerts());
	verts.ClearAll();
	int degree = md->GetFaceDegree(polyIndex);
	for (int i = 0; i < degree; i++)
	{
		int index = md->GetFaceGeomVert(polyIndex,i);
		if (verts[index])
			return false;
		verts.Set(index,TRUE);
		int nextIndex =  md->GetFaceGeomVert(polyIndex,(i+1)%degree);
		Point3 a = md->GetGeomVert(index);
		Point3 b = md->GetGeomVert(nextIndex);

		if (Length(a-b) == 0.0f)
			return false;
	}

	return true;
}

void LSCMClusterData::Add(int polyIndex, MeshTopoData *md)
{

	//degenerate and dead faces should be skipped
	if (CheckPoly(polyIndex,md) == false)
		return;



	//if mesh we can just add the triangle
	if (md->GetMesh())
	{
		
		LSCMFace face;
		face.mPolyIndex = polyIndex;
		face.mEdge[0].mIthEdge = 0;
		face.mEdge[1].mIthEdge = 1;
		face.mEdge[2].mIthEdge = 2;
		mFaceData.Append(1,&face,md->GetNumberFaces()/10);
		
	}
	//if poly we need to triangulate
	else if (md->GetMNMesh())
	{
		
		MNMesh *mesh = md->GetMNMesh();
		int degree = md->GetFaceDegree(polyIndex);
		int numberFaces = degree - 2;
		MNFace *mnFace = &mesh->f[polyIndex];
		Tab<int> tri;
		mnFace->GetTriangles (tri);
		for (int i = 0; i < numberFaces; i++)
		{
			int *triv = tri.Addr(i*3);
				
			LSCMFace face;
			face.mPolyIndex = polyIndex;
			face.mEdge[0].mIthEdge = triv[0];
			face.mEdge[1].mIthEdge = triv[1];
			face.mEdge[2].mIthEdge = triv[2];
			mFaceData.Append(1,&face,md->GetNumberFaces()/10);
		}
	}
	//if patch we need create triangulation
	else if (md->GetPatch())
	{
		int degree = md->GetFaceDegree(polyIndex);
		if (degree == 3)
		{
				LSCMFace face;
				face.mPolyIndex = polyIndex;
				face.mEdge[0].mIthEdge = 0;
				face.mEdge[1].mIthEdge = 1;
				face.mEdge[2].mIthEdge = 2;
				mFaceData.Append(1,&face,md->GetNumberFaces()/10);
		}
		else
		{
				LSCMFace face;
				face.mPolyIndex = polyIndex;
				face.mEdge[0].mIthEdge = 0;
				face.mEdge[1].mIthEdge = 1;
				face.mEdge[2].mIthEdge = 3;
				mFaceData.Append(1,&face,md->GetNumberFaces()/10);

				face.mEdge[0].mIthEdge = 2;
				face.mEdge[1].mIthEdge = 3;
				face.mEdge[2].mIthEdge = 1;
				mFaceData.Append(1,&face,md->GetNumberFaces()/10);
		
		}
	}
}

void LSCMClusterData::ApplyMapping(MeshTopoData *md, UnwrapMod *mod)
{

	//mark our faces for this cluster
	BitArray mapFaces;
	mapFaces.SetSize(md->GetNumberFaces());
	mapFaces.ClearAll();
	for (int i = 0; i < mFaceData.Count(); i++)
	{
		int polyIndex = mFaceData[i].mPolyIndex;
		mapFaces.Set(polyIndex,TRUE);
	}

	BitArray borderVerts;
	borderVerts.SetSize(md->GetNumberGeomVerts());
	borderVerts.ClearAll();

	int numGeoEdges = md->GetNumberGeomEdges();

	//mark vertices that are on the border of the cluster
	for (int i = 0; i < numGeoEdges; i++)
	{
		int numberConnectedFaces = md->GetGeomEdgeNumberOfConnectedFaces(i);
		if (numberConnectedFaces >1)
		{
			int numSelected = 0;
			
			for (int j = 0; j < numberConnectedFaces; j++)
			{
				int fa = md->GetGeomEdgeConnectedFace(i,j);
				if (mapFaces[fa])
					numSelected++;
			}
			if ((numSelected > 0) && (numSelected != numberConnectedFaces))
			{
				int va = md->GetGeomEdgeVert(i,0);
				borderVerts.Set(va,TRUE);
				va = md->GetGeomEdgeVert(i,1);
				borderVerts.Set(va,TRUE);

			}
		}
	}

	TimeValue t = GetCOREInterface()->GetTime();

	//find our dead verts ( just an optimization when adding tvs) 
	Tab<int> deadVerts;
	for (int m = 0; m < md->GetNumberTVVerts();m++)
	{
		if (md->GetTVVertDead(m))
		{
			deadVerts.Append(1,&m,1000);						
		}
	}



	Tab<int> geomToTV; //maps a geom vert to a tv 
	geomToTV.SetCount(md->GetNumberTVVerts());
	for (int i = 0; i < geomToTV.Count(); i++)
		geomToTV[i] = -1;

	for (int i = 0; i < mFaceData.Count(); i++)
	{
		int polyIndex = mFaceData[i].mPolyIndex;
		if (mapFaces[polyIndex])
		{
			mapFaces.Clear(polyIndex);
			int degree = md->GetFaceDegree(polyIndex);

			for (int j = 0; j < degree; j++)
			{
				int geomIndex = md->GetFaceGeomVert(polyIndex,j);
				int tvIndex = md->GetFaceTVVert(polyIndex,j);

				Point3 xyz = md->GetGeomVert(geomIndex);

				//border verts need new verts added and then assigned
				if (geomToTV[geomIndex] != -1)
				{
					md->SetFaceTVVert(polyIndex,j,geomToTV[geomIndex]);
				}
				else if (borderVerts[geomIndex])
				{
					int newTVIndex = md->AddTVVert(t,xyz,polyIndex,j,mod,0,&deadVerts);
					geomToTV[geomIndex] = newTVIndex;
					
				}
				//interior verts can just be assigned a pos
				else
				{
					md->SetTVVert(t,tvIndex,xyz,mod);
					geomToTV[geomIndex] = tvIndex;
				}	

			}
		}
	}
}

void LSCMClusterData::UnmarkTempPins(MeshTopoData *md)
{
	for (int i = 0; i < mTempPins.Count(); i++)
	{
		md->TVVertUnpin(mTempPins[i]);
	}
	mTempPins.SetCount(0);
}

class TempPinData
{
public:

	TempPinData();
	void AddTV(int index);
	int mGeoIndex;
	int mTVIndex[2];
	int mTVCount;
};

TempPinData::TempPinData()
{
	mGeoIndex = -1;
	mTVCount = 0;
	mTVIndex[0] = -1;
	mTVIndex[1] = -1;
}
void TempPinData::AddTV(int index)
{


	if (mTVIndex[0] == -1) 
	{
		mTVIndex[0] = index;
		mTVCount++;
		return;
	}
	if ( (mTVIndex[1] == -1) && (mTVIndex[0] != index) )
	{
		mTVIndex[1] = index;
		mTVCount++;
		return;
	}

	if (mTVIndex[0] == index)
		return;
	if (mTVIndex[1] == index)
		return;

	//if we have more than 2 we are not interested in this pin
	if ( (mTVIndex[0] != index) && (mTVIndex[1] != index) )
		mTVCount = 3;
}



class TempPinFinder
{
public:
	TempPinFinder(MeshTopoData *md);

	void AddVert(int geoIndex, int tvIndex);

	void ComputePins(int &pin1, int &pin2);

	int FindPinVert(int tvIndex);

	MeshTopoData* mMd;
	Tab<int> mGeoToPinIndex;
	Tab<TempPinData> mPinData;
};


TempPinFinder::TempPinFinder(MeshTopoData *md)
{
	mMd = md;
	mGeoToPinIndex.SetCount(mMd->GetNumberGeomVerts());
	for (int i = 0; i < mGeoToPinIndex.Count(); i++)
		mGeoToPinIndex[i] = -1;
}

void TempPinFinder::AddVert(int geoIndex, int tvIndex)
{
	int pinIndex = mGeoToPinIndex[geoIndex];
	if (pinIndex == -1)
	{
		TempPinData pinData;
		pinData.mGeoIndex = geoIndex;
		pinData.AddTV(tvIndex);
		mPinData.Append(1,&pinData,10000);
		mGeoToPinIndex[geoIndex] = mPinData.Count()-1;
	}
	else
	{		
		mPinData[pinIndex].AddTV(tvIndex);
	}	
}

int TempPinFinder::FindPinVert(int tvIndex)
{
	for (int i = 0; i < mPinData.Count(); i++)
	{
		int tvCount = mPinData[i].mTVCount;		
		if (tvCount > 2)
			tvCount = 2;
		for (int j = 0; j < tvCount; j++)
		{
			if (mPinData[i].mTVIndex[j] == tvIndex)
			{
				//we don't want pin verts with more than 2 connections
				if (mPinData[i].mTVCount > 2)
					return -1;
				else
					return i;
			}
		}
	}
	return -1;
}

void TempPinFinder::ComputePins(int &pin1, int &pin2)
{
	//if we only need to compute 1
	if (pin1 != -1)
	{
		//find our geo vert attached to this pin
		int pinIndex = FindPinVert(pin1);
		float dist = 0.0f;
		//if we share a common geometry vert find the distance
		if ( (pinIndex != -1) && (mPinData[pinIndex].mTVCount == 2))
		{			
			if (mPinData[pinIndex].mTVIndex[0] == pin1)
				pin2 = mPinData[pinIndex].mTVIndex[1];
			else
				pin2 = mPinData[pinIndex].mTVIndex[0];
			dist = LengthSquared(mMd->GetTVVert(pin1)-mMd->GetTVVert(pin2));
		}

		//now find all the other distances
		Point3 p = mMd->GetTVVert(pin1);
		BitArray usedVerts;
		usedVerts.SetSize(mMd->GetNumberTVVerts());
		usedVerts.ClearAll();
		usedVerts.Set(pin1,TRUE);
		int farthestID = -1;
		float farthestD = 0.0f;
		for (int i = 0; i < mPinData.Count(); i++)
		{
			int tvCount = mPinData[i].mTVCount;						
			if (tvCount <= 2)
			{
				for (int j = 0; j < tvCount; j++)
				{
					int tvIndex = mPinData[i].mTVIndex[j];
					if (usedVerts[tvIndex] == false)
					{
						Point3 p1 = mMd->GetTVVert(tvIndex);
						float d = LengthSquared(p-p1);
						if ((farthestID == -1) || (d > farthestD))
						{
							farthestID = tvIndex;
							farthestD = d;
						}
						usedVerts.Set(tvIndex,TRUE);
					}
				}

			}
		}

		// if the non shared distance is greater by 2x use it instead (if the object is a long rect we want to use that )
		if (farthestD > (dist*2))
			pin2 = farthestID;
		return;

	}
	else
	{
		//loop through looking for pins that share a geom vert
		//look for the farthest distance, if there is one
		//use that one
		int farthestID = -1;
		float farthestD = 0.0f;		
		int ct = 0;

		Box3 pinBounds;
		pinBounds.Init();

		int sealedTV = 0;

		for (int i = 0; i < mPinData.Count(); i++)
		{
			if (mPinData[i].mTVCount == 2)
			{
				Point3 tva = mMd->GetTVVert(mPinData[i].mTVIndex[0]);
				Point3 tvb = mMd->GetTVVert(mPinData[i].mTVIndex[1]);
				tva.z = 0.0f;
				tvb.z = 0.0f;
				pinBounds += tva;
				pinBounds += tvb;

				float d = LengthSquared(tva-tvb);
				ct++;
				if (d < 0.001f)
					sealedTV++;
				if (d > farthestD)
				{
					farthestID = i;
					farthestD = d;
				}
			}
		}

		//this is our first potential pin
		if ((farthestID != -1) && (farthestD > 0.001f))
		{
			pin1 = mPinData[farthestID].mTVIndex[0];
			pin2 = mPinData[farthestID].mTVIndex[1];			
		}

		//we did not find a pair with distance between them, that means the seam is not yet split
		//so find one near the center and the system will push them apart
		else if (sealedTV == mPinData.Count())
		{
			Point3 center = pinBounds.Center();
			center.z = 0.0f;
			int closestID = -1;
			float closestD = 0.0f;
			for (int i = 0; i < mPinData.Count(); i++)
			{
				if (mPinData[i].mTVCount == 2)
				{
					Point3 tva = mMd->GetTVVert(mPinData[i].mTVIndex[0]);	
					tva.z = 0.0f;
					float d = LengthSquared(center-tva);				
					if ((closestID == -1) || (d < closestD))
					{
						closestID = i;
						closestD = d;
					}
				}
			}
			if (closestID != -1) 
			{
				pin1 = mPinData[closestID].mTVIndex[0];
				pin2 = mPinData[closestID].mTVIndex[1];	
				return;
			}
		}

		//now look pins that dont share geom and find the closest one to the extremes
		pinBounds.Init();

		for (int i = 0; i < mPinData.Count(); i++)
		{
			if (mPinData[i].mTVCount == 1)
			{
				Point3 tva = mMd->GetTVVert(mPinData[i].mTVIndex[0]);
				tva.z = 0.0f;
				pinBounds += tva;
			}
		}

		//find the longest axis
		int axis = 0;
		if (pinBounds.Width().y > pinBounds.Width().x)
			axis = 1;
		Point3 centerA = pinBounds.Center();
		Point3 centerB = pinBounds.Center();
		centerA[axis] = pinBounds.pmax[axis];
		centerB[axis] = pinBounds.pmin[axis];
		centerA.z = 0.0f;
		centerB.z = 0.0f;

		int aID = -1;
		int bID = -1;
		float aDist = 0.0f;
		float bDist = 0.0f;

		for (int i = 0; i < mPinData.Count(); i++)
		{
			if (mPinData[i].mTVCount == 1)
			{
				Point3 tva = mMd->GetTVVert(mPinData[i].mTVIndex[0]);	
				tva.z = 0.0f;
				float da = LengthSquared(centerA-tva);
				float db = LengthSquared(centerB-tva);
				if ((aID == -1) || (da < aDist))
				{
					aDist = da;
					aID = i;
				}
				if ((bID == -1) || (db < bDist))
				{
					bDist = db;
					bID = i;
				}
			}
		}

		int tempPin1 = -1;
		int tempPin2 = -1;
		if (aID != -1)
			tempPin1 = mPinData[aID].mTVIndex[0];
		if (bID != -1)
			tempPin2 = mPinData[bID].mTVIndex[0];
		
		if ( (tempPin1 != -1) && (tempPin2 != -1) ) 
		{
			Point3 tva = mMd->GetTVVert(tempPin1);
			Point3 tvb = mMd->GetTVVert(tempPin2);
			tva.z = 0.0f;
			tvb.z = 0.0f;
			float tempDist = LengthSquared( tva - tvb );
			// if the non shared distance is greater by 2x use it instead (if the object is a long rect we want to use that )
			if (tempDist > (farthestD*2))
			{
				pin1 = tempPin1;
				pin2 = tempPin2;
			}
		}
		

		return;

	}
}


void LSCMClusterData::ComputePins(MeshTopoData *md, UnwrapMod *mod, bool useExistingMapping)
{
	
	if (mNeedToResolve == false)
		return;
	

	mTempPins.SetCount(0);
	mPinVerts.SetCount(0);

	//just create a bitarray to prevent the same pin getting added twice
	BitArray mapFaces;
	mapFaces.SetSize(md->GetNumberFaces());
	mapFaces.ClearAll();

	
	mClusterVerts.SetSize(md->GetNumberTVVerts());
	mClusterVerts.ClearAll();
	for (int i = 0; i < mFaceData.Count(); i++)
	{
		int polyIndex = mFaceData[i].mPolyIndex;
		for (int j = 0; j < 3; j++)	
		{			
			int tvIndex = md->GetFaceTVVert(polyIndex,mFaceData[i].mEdge[j].mIthEdge);
			mClusterVerts.Set(tvIndex,TRUE);
		}
	
	}


	for (int i = 0; i < mFaceData.Count(); i++)
	{
		int polyIndex = mFaceData[i].mPolyIndex;
		mapFaces.Set(polyIndex,TRUE);
	}

	BitArray holdMapFaces = mapFaces;

	//loop through our verts looking for pin verts
	BitArray usedVerts;
	usedVerts.SetSize(md->GetNumberTVVerts());
	usedVerts.ClearAll();
	for (int i = 0; i < mFaceData.Count(); i++)
	{
		int polyIndex = mFaceData[i].mPolyIndex;
		if (mapFaces[polyIndex])
		{
			mapFaces.Clear(polyIndex);
			int degree = md->GetFaceDegree(polyIndex);
			

			for (int j = 0; j < degree; j++)
			{
				int tvIndex = md->GetFaceTVVert(polyIndex,j);
				if (md->IsTVVertPinned(tvIndex) && (!usedVerts[tvIndex]))
				{
					mPinVerts.Append(1,&tvIndex,200);
					usedVerts.Set(tvIndex,TRUE);
				}
			}
		}
	}
	mapFaces = holdMapFaces;


	//we need at least 2 pin verts if not we need to create 2
	if (mPinVerts.Count() < 2)
	{	

		
		bool onePinVert = false;
		int pVert1 = -1;
		int pVert2 = -1;

		if (mPinVerts.Count() == 1)
		{
			onePinVert = true;
			pVert1 = mPinVerts[0];
		}

		mPinVerts.SetCount(2);

		TempPinFinder pinFinder(md);
		
		//pin verts are best if they are on open edges we actually need open edges to do a solve
		int  potentialPinVertsCount = 0;

		Box3 potentialBounds;
		potentialBounds.Init();

		Box3 tvBounds;
		tvBounds.Init();

		//find our potential verts
		for (int i = 0; i < md->GetNumberTVEdges(); i++)
		{
			int numSelected = 0;
			int numberConnectedFaces = md->GetTVEdgeNumberTVFaces(i);


			//it is an open edge so could be a pin vert		
			if (numberConnectedFaces == 1)  
			{
				int fa = md->GetTVEdgeConnectedTVFace(i,0);
				if (mapFaces[fa])
				{
					int va = md->GetTVEdgeVert(i,0);
					potentialBounds += md->GetTVVert(va);
					tvBounds += md->GetTVVert(va);	
					int geoIndex = -1;
					int degree = md->GetFaceDegree(fa);
					for (int k = 0; k < degree; k++)
					{
						if (va == md->GetFaceTVVert(fa,k))
						{
							geoIndex = md->GetFaceGeomVert(fa,k);
							 k = degree;
						}
					}
					pinFinder.AddVert(geoIndex,va);
					potentialPinVertsCount++;

					geoIndex = -1;
					va = md->GetTVEdgeVert(i,1);
					potentialBounds += md->GetTVVert(va);
					tvBounds += md->GetTVVert(va);
					for (int k = 0; k < degree; k++)
					{
						if (va == md->GetFaceTVVert(fa,k))
						{
							geoIndex = md->GetFaceGeomVert(fa,k);
							k = degree;
						}
					}
					pinFinder.AddVert(geoIndex,va);
					potentialPinVertsCount++;

				}
			}
			//just add to our bounding box so we can find the center of the tvs later
			else
			{
				int numberFaces = md->GetTVEdgeNumberTVFaces(i);
				for (int j = 0; j < numberFaces; j++)
				{
					int fa = md->GetTVEdgeConnectedTVFace(i,0);
					if (mapFaces[fa])
					{
						int va = md->GetTVEdgeVert(i,0);
						tvBounds += md->GetTVVert(va);
						va = md->GetTVEdgeVert(i,1);
						tvBounds += md->GetTVVert(va);
						j = numberFaces;
					}
				}
			}
		}

		mValidGeom = true;
		if (potentialPinVertsCount == 0)
			mValidGeom = false;
		pinFinder.ComputePins(pVert1,pVert2);

		//find the geom verts attached to the tv pin verts
		int pVertGeo1 = -1;
		int pVertGeo2 = -1;
		int ct = 0;
		for (int i = 0; i < md->GetNumberFaces(); i++)
		{
			if (mapFaces[i])
			{

				int degree = md->GetFaceDegree(i);
				for (int j = 0; j < degree; j++)
				{
					int tvIndex = md->GetFaceTVVert(i,j);
					int geomIndex = md->GetFaceGeomVert(i,j);
					if ((tvIndex == pVert1) && (pVertGeo1 == -1))
					{
						pVertGeo1 = geomIndex;

						//add them to our list

						ct++;
					}
					else if ((tvIndex == pVert2)  && (pVertGeo2 == -1))
					{
						pVertGeo2 = geomIndex;

						//add them to our list

						ct++;
					}
					if (ct >= 2)
					{
						j = degree;
						i = md->GetNumberFaces();
					}
				}
			}
		}

		//if we have on defined pin vert just use one of our computed pins
		if (onePinVert)
		{			
			if (mPinVerts[0] != pVert2)
				pVert1 = mPinVerts[0];
			else
			{
				pVert2 = pVert1;
				pVert1 = mPinVerts[0];
			}
		}

		//see if there is a matching TV vert sharing same geovert, is use that instead
		//this helps with symmetry
		if (onePinVert == false)
		{
			for (int i = 0; i < md->GetNumberFaces(); i++)
			{
				if (mapFaces[i])
				{
					int degree = md->GetFaceDegree(i);
					for (int j = 0; j < degree; j++)
					{
						int tvIndex = md->GetFaceTVVert(i,j);
						int geomIndex = md->GetFaceGeomVert(i,j);
						if (geomIndex == pVertGeo1)
						{
							if (tvIndex != pVert1)
							{
								Point3 pa = md->GetTVVert(pVert1);
								Point3 pb = md->GetTVVert(tvIndex);
								float d = Length(pa-pb);
								//cannot of coincident verts
								if (d > 0.001)
								{
									pVert2 = tvIndex;
									pVertGeo2 = geomIndex;
									j = degree;
									i = md->GetNumberFaces();
								}
								//they are on top of each other so spread them apart
								//along the shortest axis
								else
								{
									 float d = potentialBounds.Width().x;
									 int axis = 0;
									 if (d == 0.0f)
									 {
										 d = potentialBounds.Width().y;
										 axis = 1;
									 }
									 else
									 {
										 if (d < potentialBounds.Width().y)
										 {
											d = potentialBounds.Width().y;
											axis = 1;
										}
									 }
									 if (d > 0.0f)
									 {
										pVert2 = tvIndex;
										pVertGeo2 = geomIndex;
										float expandDist = tvBounds.Width()[axis] * 0.5f;

										TimeValue t = GetCOREInterface()->GetTime();
										Point3 tva = md->GetTVVert(pVert1);
										Point3 tvb = md->GetTVVert(pVert2);
										tva[axis] -= expandDist;
										tvb[axis] += expandDist;
										md->SetTVVert(t,pVert1,tva,mod);
										md->SetTVVert(t,pVert2,tvb,mod);
										i = md->GetNumberFaces();
										j = degree;
									 }
								}
							}
						}				
					}
				}
			}
		}
		

		float pathLen = 0.0f;

		//since we are using default mapping we need spread the pin verts to get some area to work with
		if (useExistingMapping == false)
		{
			if (pVertGeo1 == pVertGeo2)
			{
				if (tvBounds.Width().x != 0.0f)
					pathLen = tvBounds.Width().x;  //just a guesstimate
			}
			else
			{
				Tab<int> path;
				md->EdgeListFromPoints(path, pVertGeo1, pVertGeo2, Point3(0,0,1));

				for (int i = 0; i < path.Count(); i++)
				{
					int edgeIndex = path[i];
					int a = md->GetGeomEdgeVert(edgeIndex,0);
					int b = md->GetGeomEdgeVert(edgeIndex,1);

					Point3 pa = md->GetGeomVert(a);
					Point3 pb = md->GetGeomVert(b);
					pathLen += Length(pa-pb);
				}
			}
		}

		//move our pin verts
		


		TimeValue t = GetCOREInterface()->GetTime();
		
		mPinVerts[0] = pVert1;
		mPinVerts[1] = pVert2;


		//if we are using existing mapping don't move the pins
		if ((pVert1 != -1) && (pVert2 != -1))
		{
			if (useExistingMapping == false)
			{
				Point3 tva(0,0,0);
				Point3 tvb(0,0,0);
				tva = tvBounds.Center();
				tvb = tvBounds.Center();

				tva.x -= pathLen/2;
				tvb.x += pathLen/2;

				md->SetTVVert(t,pVert1,tva,mod);
				md->SetTVVert(t,pVert2,tvb,mod);
			}
			else
			{
				//if they share the same geo vert align them so the object is symmetrical
				if ((pVertGeo2 == pVertGeo1)  && (onePinVert == false))
				{
					Point3 a = md->GetTVVert(pVert1);
					Point3 b = md->GetTVVert(pVert2);
					if (fabs(a.x-b.x) > fabs(a.y-b.y))
					{
						float y = (a.y  + b.y ) *0.5f;
						a.y = y;
						b.y = y;
						md->SetTVVert(t,pVert1,a,mod);
						md->SetTVVert(t,pVert2,b,mod);
					}
					else
					{
						float x = (a.x  + b.x ) *0.5f;
						a.x = x;
						b.x = x;
						md->SetTVVert(t,pVert1,a,mod);
						md->SetTVVert(t,pVert2,b,mod);
					}
				}
			}

			md->TVVertPin(pVert1);
			md->TVVertPin(pVert2);

			//if we have one defined pin vert we only want to store off our computed so we dont
			//erase it when remove the temp pins
			mTempPins.SetCount(0);
			if (onePinVert)
			{
				mTempPins.Append(1,&pVert2);
			}
			else
				mTempPins = mPinVerts;
		}
		else
		{
			mValidGeom = false;
		}

	}


	mTVIndexToMatrixIndex.SetCount(md->GetNumberTVVerts());
	int current = 0;
	for (int i = 0; i < md->GetNumberTVVerts(); i++)
	{
		if (md->IsTVVertPinned(i))
		{
			mTVIndexToMatrixIndex[i] = -1;			
		}
		else if (mClusterVerts[i])
		{
			mTVIndexToMatrixIndex[i] = current++;
		}
		
	}


	BitArray pinnedVerts;
	pinnedVerts.SetSize(md->GetNumberTVVerts());
	pinnedVerts.ClearAll();
	for (int k = 0; k < mPinVerts.Count(); k++)
	{
		if (mPinVerts[k] != -1)
			pinnedVerts.Set(mPinVerts[k],TRUE);
	}

	BitArray otherVerts;
	otherVerts.SetSize(md->GetNumberTVVerts());
	otherVerts.ClearAll();

	for (int i = 0; i < mFaceData.Count(); i++)
	{

		int tvIndex[3];
		tvIndex[0] = md->GetFaceTVVert(mFaceData[i].mPolyIndex,mFaceData[i].mEdge[0].mIthEdge);
		tvIndex[1] = md->GetFaceTVVert(mFaceData[i].mPolyIndex,mFaceData[i].mEdge[1].mIthEdge);
		tvIndex[2] = md->GetFaceTVVert(mFaceData[i].mPolyIndex,mFaceData[i].mEdge[2].mIthEdge);

		mFaceData[i].mEdge[0].mPinned = false;
		mFaceData[i].mEdge[1].mPinned = false;
		mFaceData[i].mEdge[2].mPinned = false;

		if (pinnedVerts[tvIndex[0]])
		{
			mFaceData[i].mEdge[0].mPinned = true;
		}
		else
			otherVerts.Set(tvIndex[0]);

		if (pinnedVerts[tvIndex[1]])
		{
			mFaceData[i].mEdge[1].mPinned = true;
		}
		else
			otherVerts.Set(tvIndex[1]);


		if (pinnedVerts[tvIndex[2]])
		{
			mFaceData[i].mEdge[2].mPinned = true;
		}
		else
			otherVerts.Set(tvIndex[2]);

	}

	mNumberCoefficients = otherVerts.NumberSet() * 2;

}



//this just fills out the angles which is needed for the solve
void LSCMClusterData::ComputeFaceAngles(MeshTopoData *md)
{
	if (mNeedToResolve == false)
		return;

	for (int i = 0; i < mFaceData.Count(); i++)
	{
		int index[3];
		index[0] = md->GetFaceGeomVert(mFaceData[i].mPolyIndex,mFaceData[i].mEdge[0].mIthEdge);
		index[1] = md->GetFaceGeomVert(mFaceData[i].mPolyIndex,mFaceData[i].mEdge[1].mIthEdge);
		index[2] = md->GetFaceGeomVert(mFaceData[i].mPolyIndex,mFaceData[i].mEdge[2].mIthEdge);

		int tvIndex[3];
		tvIndex[0] = md->GetFaceTVVert(mFaceData[i].mPolyIndex,mFaceData[i].mEdge[0].mIthEdge);
		tvIndex[1] = md->GetFaceTVVert(mFaceData[i].mPolyIndex,mFaceData[i].mEdge[1].mIthEdge);
		tvIndex[2] = md->GetFaceTVVert(mFaceData[i].mPolyIndex,mFaceData[i].mEdge[2].mIthEdge);

		Point3 p[3];
		p[0] = md->GetGeomVert(index[0]);
		p[1] = md->GetGeomVert(index[1]);
		p[2] = md->GetGeomVert(index[2]);

		Point3 vec[3];
		vec[0] = Normalize(p[1] - p[0]);
		vec[1] = Normalize(p[2] - p[1]);
		vec[2] = Normalize(p[0] - p[2]);

		mFaceData[i].mEdge[0].mGeoAngle = fabs(md->AngleFromVectors(vec[0],vec[2]*-1.0f));
		mFaceData[i].mEdge[1].mGeoAngle = fabs(md->AngleFromVectors(vec[1],vec[0]*-1.0f));
		mFaceData[i].mEdge[2].mGeoAngle = fabs(md->AngleFromVectors(vec[2],vec[1]*-1.0f));
		float ang = mFaceData[i].mEdge[0].mGeoAngle+mFaceData[i].mEdge[1].mGeoAngle+mFaceData[i].mEdge[2].mGeoAngle;

		mFaceData[i].mEdge[0].mSin = sin(mFaceData[i].mEdge[0].mGeoAngle);
		mFaceData[i].mEdge[1].mSin = sin(mFaceData[i].mEdge[1].mGeoAngle);
		mFaceData[i].mEdge[2].mSin = sin(mFaceData[i].mEdge[2].mGeoAngle);







		//for best result we want the largest angle in the last slot
		for (int j = 0; j < 2; j++)
		{		
			//shift until we get the largest angle in the last slot
			if ( ( mFaceData[i].mEdge[2].mSin < mFaceData[i].mEdge[1].mSin)  || ( mFaceData[i].mEdge[2].mSin < mFaceData[i].mEdge[0].mSin) )
			{				
				LSCMFaceEdge temp = mFaceData[i].mEdge[0];
				mFaceData[i].mEdge[0] = mFaceData[i].mEdge[1];
				mFaceData[i].mEdge[1] = mFaceData[i].mEdge[2];
				mFaceData[i].mEdge[2] = temp;
			}
		}

	}


}



void LSCMClusterData::AddToTempTable(int index, float val, float uv)
{
	
	TempData data;
	
	if (index < 0)
	{
		data.mIndex = 0;
		data.mValue = val * uv;
//		DebugPrint("Pin Add to Matirx %d %f\n",index,val*uv);
		mPinnedVertsList.Append(1,&data,10000);
	}
	else
	{
		data.mIndex = index;
		data.mValue = val;
//		DebugPrint("Add to Matirx %d %f\n",index,val);
		mNonPinnedVertsList.Append(1,&data,10000);
	}
}

int LSCMClusterData::ComputeMatrixIndex(int faceIndex, int ithEdge, int tvIndex)
{
	int matrixIndex = -1;
	if (mFaceData[faceIndex].mEdge[ithEdge].mPinned)
		matrixIndex = -1;
	else
		matrixIndex = mTVIndexToMatrixIndex[tvIndex];
	return matrixIndex;
}

void LSCMClusterData::ComputeMatrices(MeshTopoData *md)
{
	if (mValidGeom ==  false) return;

	if (mNeedToResolve == false)
		return;
	if (mNumberCoefficients == 0)
		return;

	mNonPinnedVertsList.SetCount(0,FALSE);
	mPinnedVertsList.SetCount(0,FALSE);

	mAMatrix.SetSize(mNumberCoefficients);

	mBMatrix.SetCount(mNumberCoefficients);
	mXMatrix.SetCount(mNumberCoefficients);

	for (int i = 0; i < mBMatrix.Count(); i++)
	{
		mBMatrix[i] = 0.0f;
		mXMatrix[i] = 0.0f;
	}

	for (int i = 0; i < mFaceData.Count(); i++)
	{

		float ratio = 1.0f;
		if (mFaceData[i].mEdge[2].mSin != 0.0f)
		{
			ratio =  mFaceData[i].mEdge[1].mSin/mFaceData[i].mEdge[2].mSin;
		}

		float c = cos(mFaceData[i].mEdge[0].mGeoAngle) *ratio;
		float s = mFaceData[i].mEdge[0].mSin*ratio;

//		DebugPrint("c %f s %f ratio %f \n",c,s,ratio);

		int tvIndex[3];
		tvIndex[0] = md->GetFaceTVVert(mFaceData[i].mPolyIndex,mFaceData[i].mEdge[0].mIthEdge);
		tvIndex[1] = md->GetFaceTVVert(mFaceData[i].mPolyIndex,mFaceData[i].mEdge[1].mIthEdge);
		tvIndex[2] = md->GetFaceTVVert(mFaceData[i].mPolyIndex,mFaceData[i].mEdge[2].mIthEdge);

		Point3 uv[3];
		uv[0] = md->GetTVVert(tvIndex[0]);
		uv[1] = md->GetTVVert(tvIndex[1]);
		uv[2] = md->GetTVVert(tvIndex[2]);

//		DebugPrint("Angles %f %f %f \n",mFaceData[i].mEdge[0].mGeoAngle,mFaceData[i].mEdge[1].mGeoAngle,mFaceData[i].mEdge[2].mGeoAngle);
//		DebugPrint("Sin %f %f %f \n",mFaceData[i].mEdge[0].mSin,mFaceData[i].mEdge[1].mSin,mFaceData[i].mEdge[2].mSin);


//		DebugPrint("Add Row\n");
		mNonPinnedVertsList.SetCount(0,FALSE);
		mPinnedVertsList.SetCount(0,FALSE);

		int matrixIndex1 = ComputeMatrixIndex(i,0,tvIndex[0]);
		AddToTempTable(matrixIndex1*2,	c - 1.0f,	uv[0].x); 
		AddToTempTable(matrixIndex1*2+1,	-s,		uv[0].y); 

		int matrixIndex2 = ComputeMatrixIndex(i,1,tvIndex[1]);
		AddToTempTable(matrixIndex2*2,		-c,		uv[1].x); 
		AddToTempTable(matrixIndex2*2+1,	s,		uv[1].y); 

		int matrixIndex3 = ComputeMatrixIndex(i,2,tvIndex[2]);
		AddToTempTable(matrixIndex3*2,		1.0f,	uv[2].x); 
		
		FillSparseMatrix();


		mNonPinnedVertsList.SetCount(0,FALSE);
		mPinnedVertsList.SetCount(0,FALSE);
//		DebugPrint("Add Row\n");

		AddToTempTable(matrixIndex1*2,		s,			uv[0].x); 
		AddToTempTable(matrixIndex1*2+1,	c - 1.0f,	uv[0].y); 

		AddToTempTable(matrixIndex2*2,		-s,			uv[1].x); 
		AddToTempTable(matrixIndex2*2+1,	-c,			uv[1].y); 

		AddToTempTable(matrixIndex3*2+1,	1.0f,		uv[2].y); 

		FillSparseMatrix();


	}

	int numberEntries = 0;
	for (int i = 0; i < mAMatrix.Size(); i++)
	{
		numberEntries += (int) mAMatrix.GetRow(i)->mData.size();
	}

	Tab<int> rowCounter;
	rowCounter.SetCount(mNumberCoefficients+1);

	Tab<float> val;
	val.SetCount(numberEntries);
	
	Tab<int> valueIndex;
	valueIndex.SetCount(numberEntries);


	SuperMatrix At, AtP;
	int info = 0, panel_size = 0, relax = 0;
	superlu_options_t options;

	// Temporary variables 
	int count = 0;


	// Convert M to compressed column format 
	for(int i = 0, count=0; i < mAMatrix.Size(); i++) 
	{
		 LSCMRow *row = mAMatrix.GetRow(i);
		rowCounter[i] = count;

		SparseData :: iterator mIter;
		mIter = row->mData.begin();

		for (int j = 0; j < row->mData.size(); j++, count++)
		{
			val[count] = mIter->second;
			valueIndex[count] = mIter->first;
//DebugPrint("i %d j %d   a %f asub %d \n",i,j,a[count],asub[count]);
			mIter++;
		}
	}
	rowCounter[mNumberCoefficients] = numberEntries;

	if  ( ( val.Count() > 0 ) && ( valueIndex.Count() > 0 ) && ( rowCounter.Count() > 0 ) )
		sCreate_CompCol_Matrix(	&At, mNumberCoefficients, mNumberCoefficients, numberEntries, val.Addr(0), valueIndex.Addr(0), rowCounter.Addr(0), 
			SLU_NC,	SLU_S,	SLU_GE	);

	// Set superlu options 
	options.Fact = DOFACT;
	options.Equil = YES;
	options.ColPerm = COLAMD;
	options.DiagPivotThresh = 1.0;
	options.Trans = NOTRANS;
	options.IterRefine = NOREFINE;
	options.SymmetricMode = NO;
	options.PivotGrowth = NO;
	options.ConditionNumber = NO;
	options.PrintStat = YES;
	options.ColPerm = MY_PERMC;
	options.Fact = DOFACT;

	StatInit(&mSuperLuStats);

	panel_size = sp_ienv(1); 
	relax = sp_ienv(2);

	// Compute permutation and permuted matrix 
	mPermC.SetCount(mNumberCoefficients);
	mPermR.SetCount(mNumberCoefficients);

	for (int i = 0; i < mNumberCoefficients; i++)
	{
		mPermR[i] = 0;
		mPermC[i] = 0;
	}

	Tab<int> elimationTree;
	elimationTree.SetCount(mNumberCoefficients);
	
	if ( (mPermC.Count() > 0) && (elimationTree.Count() > 0) )
	{
		get_perm_c(3, &At, mPermC.Addr(0));

		sp_preorder(&options, &At, mPermC.Addr(0), elimationTree.Addr(0), &AtP);
	}

	
	if (mSuperLuLandUAllocated)
	{
		Destroy_SuperNode_Matrix(&(mL));
		Destroy_CompCol_Matrix(&(mU));
	}
	
	
	if ( (mPermC.Count() > 0) && (elimationTree.Count() > 0) && (mPermR.Count() > 0) )
	{
		// Decompose into L and U 
		sgstrf(&options, &AtP, 0.0f,
			relax, panel_size,	
			elimationTree.Addr(0), NULL, 0, 
			mPermC.Addr(0), mPermR.Addr(0),	&mL, &mU, 
			&mSuperLuStats, &info);

	}

	mSuperLuLandUAllocated = true;

	Destroy_SuperMatrix_Store(&At);
	Destroy_SuperMatrix_Store(&AtP);


}



void LSCMClusterData::FillSparseMatrix()
{
	int numberOfNonPinned		  = mNonPinnedVertsList.Count();
	int numberOfPinned		  = mPinnedVertsList.Count();
	for (int i = 0; i < numberOfNonPinned; i++)
	{
		for(int j = 0; j < numberOfNonPinned; j++) 
		{
			float val = mNonPinnedVertsList[i].mValue*mNonPinnedVertsList[j].mValue;
			int x = mNonPinnedVertsList[i].mIndex;
			int y = mNonPinnedVertsList[j].mIndex;
			
			float oval = mAMatrix.GetRow(x)->mData[y];
			mAMatrix.GetRow(x)->mData[y] += val;
//			DebugPrint("%d %d  value %f new value %f \n",x,y,oval,mAMatrix.GetRow(x)->mData[y]);

		}	
	}
	float sum = 0.0f;
	for(int i = 0; i < numberOfPinned; i++)
		sum += mPinnedVertsList[i].mValue;

	for(int i = 0; i < numberOfNonPinned; i++)
	{
		mBMatrix[ mNonPinnedVertsList[i].mIndex ] -= mNonPinnedVertsList[i].mValue * sum;
	}

}

void LSCMClusterData::Solve(MeshTopoData *md)
{
	if (mValidGeom ==  false) return;

	if (mNeedToResolve == false)
		return;

	if (mNumberCoefficients == 0)
		return;

	int count = mXMatrix.Count();

	SuperMatrix B;
	int info = 0;

	sCreate_Dense_Matrix(&B, count, 1, mBMatrix.Addr(0), count, SLU_DN, SLU_S,  SLU_GE  );

	sgstrs(TRANS, &(mL), &(mU),	mPermC.Addr(0), mPermR.Addr(0), &B,	&(mSuperLuStats), &info);

	mValidSolve = false;
	if(info == 0)
	{
		memcpy(mXMatrix.Addr(0), ((DNformat*)B.Store)->nzval, sizeof(float)*count);
		mValidSolve = true;
	}

	Destroy_SuperMatrix_Store(&B);

	return;
}


void LSCMClusterData::SendBackUVWs(UnwrapMod *mod, MeshTopoData *md)
{
	if (mNeedToResolve == false)
		return;

	mNeedToResolve = false;

	if (mValidGeom ==  false) return;
	if (mValidSolve == false) return;

	TimeValue t = GetCOREInterface()->GetTime();

	BitArray useVerts;
	useVerts.SetSize(md->GetNumberTVVerts());
	useVerts.ClearAll();
	for (int i = 0; i < mFaceData.Count(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			int polyIndex = mFaceData[i].mPolyIndex;
			int tvIndex = md->GetFaceTVVert(polyIndex,mFaceData[i].mEdge[j].mIthEdge);
			if (md->IsTVVertPinned(tvIndex) == false)
			{
				useVerts.Set(tvIndex,TRUE);
			}
		}
		
	}

	int currentIndex = 0;
	for (int i = 0; i < md->GetNumberTVVerts(); i++)
	{
		if ((md->IsTVVertPinned(i) == false) && useVerts[i])
		{
			Point3 val(0,0,0);

			val.x =  mXMatrix[currentIndex*2];
			val.y =  mXMatrix[currentIndex*2+1];

			if ((_isnan(val.x) == false) && (_isnan(val.y) == false))
				md->SetTVVert(t,i,val,mod);

			currentIndex++;
		}
	}
}



LSCMLocalData::LSCMLocalData(UnwrapMod *mod, MeshTopoData* md, bool useExistingMapping, bool useSelectedFaces)
{
	mMod = mod;
	mMd = md;
	mTopoChange = false;

	mTimeToSolve = 0;

	mUseExistingMapping = useExistingMapping;

	mChangedPin.SetSize(md->GetNumberTVVerts());
	mChangedPin.ClearAll();
	mMovedPin = mChangedPin;

	BitArray usedFaces;
	usedFaces.SetSize(mMd->GetNumberFaces());
	usedFaces.ClearAll();

	if (useSelectedFaces)
	{
		BitArray faceSel = md->GetFaceSelection();		
		usedFaces = ~faceSel; 
	}

	mUsedFaces = usedFaces;

	int numberFaces = mMd->GetNumberFaces();
	int startFace = 0;

	//find our clusters
	Tab<int> faces;
	while (numberFaces != usedFaces.NumberSet())
	{
		faces.SetCount(0);
		GetTVCluster(startFace, usedFaces, faces);
		AddCluster(faces);
	}
}
LSCMLocalData::~LSCMLocalData()
{
	Free();
}

void LSCMLocalData::Free()
{
	for (int i = 0; i < mClusterData.Count(); i++)
	{
		if (mClusterData[i])
			delete mClusterData[i];
		mClusterData[i] = NULL;
	}
	mClusterData.ZeroCount();
}

MeshTopoData* LSCMLocalData::GetLocalData()
{
	return mMd;
}


void LSCMLocalData::AddCluster(Tab<int> &faces)
{
	LSCMClusterData *data = new LSCMClusterData();
	

	for (int i = 0; i < faces.Count(); i++)
	{
		int faceIndex = faces[i];
		data->Add(faceIndex,mMd);
	}
	mClusterData.Append(1,&data,100);
}


void LSCMLocalData::GetGeomCluster(int &startFace, BitArray &usedFaces, Tab<int> &faces)
{
	//get the start index
	int numberFaces = mMd->GetNumberFaces();
	BitArray selFaces = mMd->GetFaceSelection();
	faces.SetCount(0);

	if (usedFaces[startFace])
	{
		for (int i = startFace; i < usedFaces.GetSize(); i++)
		{
			if (usedFaces[i] == false)
			{
				startFace = i;
				i = usedFaces.GetSize();
			}
		}
	}


	//get our geom element from the start face
	mMd->ClearFaceSelection();
	mMd->SetFaceSelected(startFace,TRUE);



	mMod->SelectGeomElement(mMd, FALSE);

	for (int i = 0; i < numberFaces; i++)
	{
		if (mMd->GetFaceSelected(i) && (usedFaces[i] == false))
		{
			faces.Append(1,&i,10000);
		}
	}
	usedFaces |= mMd->GetFaceSelection();
	

	mMd->SetFaceSelection(selFaces);
}

void LSCMLocalData::ApplyMapping()
{
	for (int i = 0; i < mClusterData.Count(); i++)
	{
		mClusterData[i]->ApplyMapping(mMd,mMod);
		mMd->SetTVEdgeInvalid();
		mMd->BuildTVEdges();
	}
}

void LSCMLocalData::ComputePins()
{

	for (int i = 0; i < mClusterData.Count(); i++)
	{
		mClusterData[i]->ComputePins(mMd,mMod,mUseExistingMapping);
	}
}

void LSCMLocalData::ComputeFaceAngles()
{
	for (int i = 0; i < mClusterData.Count(); i++)
	{
		mClusterData[i]->ComputeFaceAngles(mMd);
	}
}


//this just fills out the angles which is needed for the solve

void LSCMLocalData::ComputeMatrices()
{
	for (int i = 0; i < mClusterData.Count(); i++)
	{
		mClusterData[i]->ComputeMatrices(mMd);
	}
}
void LSCMLocalData::Solve()
{
	for (int i = 0; i < mClusterData.Count(); i++)
	{
		mClusterData[i]->Solve(mMd);
	}
}

void LSCMLocalData::SendBackUVWs()
{
	for (int i = 0; i < mClusterData.Count(); i++)
	{
		mClusterData[i]->SendBackUVWs(mMod,mMd);		
	}

}

void LSCMLocalData::GetTVCluster(int &startFace, BitArray &usedFaces, Tab<int> &faces)
{
	//get the start index
	int numberFaces = mMd->GetNumberFaces();
	BitArray selFaces = mMd->GetFaceSelection();
	faces.SetCount(0);

	if (usedFaces[startFace])
	{
		for (int i = startFace; i < usedFaces.GetSize(); i++)
		{
			if (usedFaces[i] == false)
			{
				startFace = i;
				i = usedFaces.GetSize();
			}
		}
	}


	//get our tv element from the start face
	mMd->ClearFaceSelection();
	mMd->SetFaceSelected(startFace,TRUE);

	
	mMd->SelectElement(TVFACEMODE,TRUE);


	for (int i = 0; i < numberFaces; i++)
	{
		if (mMd->GetFaceSelected(i) && (usedFaces[i] == false))
		{
			faces.Append(1,&i,10000);
		}
	}
	usedFaces |= mMd->GetFaceSelection();

	mMd->SetFaceSelection(selFaces);
}

void  LSCMLocalData::InvalidatePin(int index)
{
	if (index >= mMovedPin.GetSize())
	{
		mMovedPin.SetSize(index+1,1);
		mChangedPin.SetSize(index+1,1);
	}

	mMovedPin.Set(index);
}
void LSCMLocalData::InvalidatePinAddDelete(int index)
{
	if (index >= mChangedPin.GetSize())
	{
		mMovedPin.SetSize(index+1,1);
		mChangedPin.SetSize(index+1,1);
	}

	mChangedPin.Set(index);
}
void LSCMLocalData::InvalidateTopo()
{
	mTopoChange = true;
	for (int i = 0; i < mClusterData.Count(); i++)
	{
		mClusterData[i]->Invalidate();
	}
}

double LSCMLocalData::GetSolveTime()
{
	return mTimeToSolve;
}

bool LSCMLocalData::Resolve()
{

	BitArray holdVertSel;
	holdVertSel.SetSize(mMd->GetNumberTVVerts());
	holdVertSel = mMd->GetTVVertSelection();


	bool solveHappened = false;
	double tempTime = 0.0;

	if (mTopoChange )
	{
		for (int i = 0; i < mClusterData.Count(); i++)
		{
			mClusterData[i]->Invalidate();
		}
		mMovedPin.SetSize(mMd->GetNumberTVVerts());
		mChangedPin.SetSize(mMd->GetNumberTVVerts());
		mMovedPin.ClearAll();
		mChangedPin.ClearAll();
	}
	else if ( mMovedPin.NumberSet() || mChangedPin.NumberSet() )
	{		
		for (int i = 0; i < mClusterData.Count(); i++)
		{
			mClusterData[i]->Invalidate(mMovedPin,mMd);
			mClusterData[i]->Invalidate(mChangedPin,mMd);
		}

	}

	
		

	if (mTopoChange )
	{
		solveHappened = true;

		MaxSDK::PerformanceTools::Timer timer;
		timer.StartTimer();



//cut the seams
		theHold.Suspend();
		BitArray seam = mMd->mSeamEdges;
		if (seam.NumberSet())
		{
			BitArray uvEdgeSel = mMd->GetTVEdgeSelection();
			mMd->ConvertGeomEdgeSelectionToTV(seam, uvEdgeSel );
			mMd->BreakEdges(uvEdgeSel);
		}
		theHold.Resume();

		Free();

		BitArray usedFaces = mUsedFaces;

		int numberFaces = mMd->GetNumberFaces();
		int startFace = 0;

		//find our clusters
		Tab<int> faces;
		while (numberFaces != usedFaces.NumberSet())
		{
			faces.SetCount(0);
			GetTVCluster(startFace, usedFaces, faces);
			AddCluster(faces);
		}

		//compute the face angles
		ComputeFaceAngles();
	
		tempTime += timer.EndTimer();
	}

	if (mTopoChange || mMovedPin.NumberSet() || mChangedPin.NumberSet() )
	{

		solveHappened = true;

		MaxSDK::PerformanceTools::Timer timer;
		timer.StartTimer();
	
		//see if add/delete pin
		//these need to be recomputed if in this case
		ComputePins();

		//see if pin moved this 
		//compute our matrices only B matrix needs to be computed
		ComputeMatrices();

		//solve our solution
		Solve();


		SendBackUVWs();		
		RescaleClusters();



		tempTime += timer.EndTimer();

	}

	mTimeToSolve = tempTime;

	mMovedPin.ClearAll();
	mChangedPin.ClearAll();

	mTopoChange = false;

	int nv = mMd->GetNumberTVVerts();
	holdVertSel.SetSize(nv,1);
	mMd->SetTVVertSelection(holdVertSel);
	return solveHappened;
}

void LSCMLocalData::RescaleClusters()
{
	Tab<int> clusterDefinitions;
	clusterDefinitions.SetCount(mMd->GetNumberFaces());
	for (int i = 0; i < clusterDefinitions.Count(); i++)
		clusterDefinitions[i] = -1;
	BitArray processFaces;
	processFaces.SetSize(mMd->GetNumberFaces());
	processFaces.ClearAll();

//get our cluster scale values
	Tab<float> rescale;
	rescale.SetCount(mClusterData.Count());

	Tab<float> groupScales;
	groupScales.SetCount(mMod->pblock->Count(unwrap_group_density));
	for (int i = 0; i < groupScales.Count(); i++)
	{
		groupScales[i] = mMod->pblock->GetFloat(unwrap_group_density,0,i);
	}

	for (int i = 0; i < mClusterData.Count(); i++)
	{
		rescale[i] = 1.0f;
		//DebugPrint("Cluster %d  pin count %d\n",i,(mClusterData[i]->TempPinCount()));
		if (mClusterData[i]->TempPinCount() > 1)
		{
			int ct = mClusterData[i]->NumberFaces();
			//DebugPrint("Faces   ");
			for (int j = 0; j < ct; j++)
			{
				LSCMFace *face = mClusterData[i]->GetFace(j);
//if the cluster is user defined get the rescale value attached to that cluster otherwise it will be 1.0f
				int groupID = mMd->GetToolGroupingData()->GetGroupID(face->mPolyIndex);
				if (groupID != -1)
				{
					rescale[i] = groupScales[groupID];
				}
					
				if (processFaces[face->mPolyIndex] == false)
				{
					clusterDefinitions[face->mPolyIndex] = i;
			//		DebugPrint("%d, ",face->mPolyIndex);
					processFaces.Set(face->mPolyIndex);
				}
			}		
			//DebugPrint("\n");
		}
		mClusterData[i]->UnmarkTempPins(mMd);		

	}


	mMd->RescaleClusters(clusterDefinitions,rescale,mMod);

}


ToolLSCM::ToolLSCM()
{
	mMod = NULL;
	mTimeOverAll = 0.0;
	
}
ToolLSCM::~ToolLSCM()
{
	Free();
}

void ToolLSCM::ApplyMapping(bool useSelection, Tab<MeshTopoData*> &localData, UnwrapMod *mod)
{
	for (int i = 0; i < localData.Count(); i++)
	{
		for (int j = 0; j < localData[i]->GetNumberTVVerts(); j++)
		{
			localData[i]->TVVertUnpin(j);
		}

		BitArray faceSel;
		faceSel = localData[i]->GetFaceSelection();

		if (useSelection == false)
			faceSel.SetAll();

		//detach faces
		BitArray vertSel;
		localData[i]->DetachFromGeoFaces(faceSel,vertSel,mod);
		localData[i]->SetTVEdgeInvalid();
		localData[i]->BuildTVEdges();
		localData[i]->BuildVertexClusterList();
		
	}
}

void ToolLSCM::CutSeams(Tab<MeshTopoData*> &localData, UnwrapMod *mod)
{
	theHold.Suspend();
	for (int i = 0; i < localData.Count(); i++)
	{
		BitArray seam = localData[i]->mSeamEdges;
		if (seam.NumberSet())
		{
			BitArray uvEdgeSel = localData[i]->GetTVEdgeSelection();
			localData[i]->ConvertGeomEdgeSelectionToTV(seam, uvEdgeSel );
			localData[i]->BreakEdges(uvEdgeSel);
			if (localData[i]->GetNumberTVEdges() != localData[i]->GetTVEdgeSelection().GetSize())
			{
				localData[i]->GetTVEdgeSelectionPtr()->SetSize(localData[i]->GetNumberTVEdges());
			}
		}
	}
	theHold.Resume();

}

void ToolLSCM::Pack()
{
	BOOL holdRescale = mMod->fnGetPackRescaleCluster();
	mMod->fnSetPackRescaleCluster(FALSE);
	theHold.Suspend();
	mMod->fnPackNoParams();
	theHold.Resume();
	mMod->fnSetPackRescaleCluster(holdRescale);

}

bool ToolLSCM::Start( bool useExistingMapping, UnwrapMod *mod, Tab<MeshTopoData*> &localData)
{
	mMod = mod;
	MaxSDK::PerformanceTools::Timer timer;

	//clean out data if we ended abnormally from the last time
	//should not be able to happen from the UI but could if it was maxscript driven
	Free();

	for (int ldID = 0; ldID < localData.Count(); ldID++)
	{
		localData[ldID]->HoldSelection();
	}


	int holdSelLevel = mod->fnGetTVSubMode();
	mod->fnSetTVSubMode(TVFACEMODE);


	//see if have any selected faces, if so we only operate on the selected faces
	bool useSelectedFaces = false;
	if (holdSelLevel == TVFACEMODE)
	{
		for (int ldID = 0; ldID < localData.Count(); ldID++)
		{
			if (localData[ldID]->GetFaceSelection().NumberSet() > 0)
			{
				useSelectedFaces = true;
				ldID = localData.Count();
			}
		}
	}

	//split our seams
	if (!useExistingMapping)
	{
		//apply our default mapping
		ApplyMapping(useSelectedFaces, localData, mod);
	}

	//cut our seams
	timer.StartTimer();
	CutSeams(localData, mod);
	mTimeCut =  timer.EndTimer();




	timer.StartTimer();
	mLocalData.SetCount(localData.Count());
	for (int i = 0; i < localData.Count(); i++)
	{
		//initialize each local data
		LSCMLocalData *data = new LSCMLocalData(mod,localData[i],useExistingMapping,useSelectedFaces);
		mLocalData[i] = data;
		//make sure we invalidate so we get a solve
		mLocalData[i]->InvalidateTopo();
	}

	mTimeClusterCreation = timer.EndTimer();

	//fill out the LCSM data
	timer.StartTimer();
	for (int i = 0; i < mLocalData.Count(); i++)
	{

		//compute the face angles
		mLocalData[i]->ComputeFaceAngles();	

		//Compute the pin verts if we need be
		mLocalData[i]->ComputePins();



	}
	mTimePin = timer.EndTimer();

	//compute our matrices
	timer.StartTimer();
	for (int i = 0; i < mLocalData.Count(); i++)
	{
		mLocalData[i]->ComputeMatrices();
	}
	mTimeMatrixConstruction = timer.EndTimer();


	//solve our solution
	timer.StartTimer();
	for (int i = 0; i < mLocalData.Count(); i++)
	{
		mLocalData[i]->Solve();
	}
	mTimeSolve = timer.EndTimer();


	for (int i = 0; i < mLocalData.Count(); i++)
	{
		mLocalData[i]->SendBackUVWs();		
	}

	for (int i = 0; i < mLocalData.Count(); i++)
	{	
		mLocalData[i]->RescaleClusters();		
	}



	//do a solve
	if (!useExistingMapping)
	{
		Pack();
	}


	mod->fnSetTVSubMode(holdSelLevel);

	for (int ldID = 0; ldID < localData.Count(); ldID++)
	{
		localData[ldID]->RestoreSelection();
	}

	TimeValue t = GetCOREInterface()->GetTime();

	mod->NotifyDependents(FOREVER,TEXMAP_CHANNEL,REFMSG_CHANGE);
	if (mod->ip) mod->ip->RedrawViews(t);
	mod->InvalidateView();
	

	mTimeOverAll = mTimeClusterCreation + mTimeCut + mTimeMatrixConstruction + mTimePin + mTimeSolve;

	return true;
}

bool ToolLSCM::Solve(bool hasToSolve, bool sendNotify)
{

	bool forceSolve = false;
	if (hasToSolve)
		forceSolve = true;
	else
	{
		if (mTimeOverAll < 500.0)
		{
			forceSolve = true;
		}
	}
	if (forceSolve)
	{
		double tempTime = 0.0;
		bool somethingSolved = false;
		for (int i = 0; i < mLocalData.Count(); i++)
		{
			mLocalData[i]->GetLocalData()->HoldSelection();
			if (mLocalData[i]->Resolve() == true)
			{			
				somethingSolved = true;
			}
			tempTime += mLocalData[i]->GetSolveTime();
			mLocalData[i]->GetLocalData()->RestoreSelection();
		}

		if (tempTime > 0.0)
			mTimeOverAll = tempTime;

		if (sendNotify && somethingSolved)
		{
			mMod->NotifyDependents(FOREVER,TEXMAP_CHANNEL,REFMSG_CHANGE);
			mMod->InvalidateView();

		}

	}



	return true;
}

bool ToolLSCM::End()
{

	//free our data
	Free();

	return true;
}

void ToolLSCM::Free()
{
	for (int i = 0; i < mLocalData.Count(); i++)
	{
		if (mLocalData[i])
			delete mLocalData[i];
		mLocalData[i] = NULL;
	}
	mLocalData.SetCount(0);
}

LSCMLocalData* ToolLSCM::GetData(MeshTopoData *md)
{
	for (int i = 0; i < mLocalData.Count(); i++)
	{
		if (mLocalData[i]->GetLocalData() == md)
			return mLocalData[i];
	}
	return NULL;
}

void ToolLSCM::InvalidatePin(MeshTopoData *md, int index)
{
	LSCMLocalData *ld = GetData(md);
	if (ld)
		ld->InvalidatePin(index);
}
void ToolLSCM::InvalidatePinAddDelete(MeshTopoData *md, int index)
{
	LSCMLocalData *ld = GetData(md);
	if (ld)
		ld->InvalidatePinAddDelete(index);

}
void ToolLSCM::InvalidateTopo(MeshTopoData *md)
{
	LSCMLocalData *ld = GetData(md);
	if (ld)
		ld->InvalidateTopo();
}
