
/*

Copyright 2010 Autodesk, Inc.  All rights reserved. 

Use of this software is subject to the terms of the Autodesk license agreement provided at 
the time of installation or download, or which otherwise accompanies this software in either 
electronic or hard copy form. 

*/

#include "unwrap.h"
#include "TvConnectionInfo.h"
#include "modsres.h"

VertexConnectedTo::VertexConnectedTo(int v, int e, int faceCount, int faceA,int faceB, BOOL hiddenEdge)
{
	mVert = v;
	mEdge = e;
	mFaceCount = faceCount;
	mFace[0] = faceA;
	mFace[1] = faceB;
	mHiddenEdge = hiddenEdge;
	mBorderEdge = FALSE;
	mIthOpposingEdge = -1;
	

}

Vertex::Vertex()
{
	mX = 0XFFFF;
	mY = 0XFFFF;
	mDir = 0;

	mVisibleEdgeCount = 0;
	mHiddenEdgeCount = 0;
	mUset = false;
	mVset = false;

	mBorder = false;

	mU = 0.0f;
	mV = 0.0f;

}


void Vertex::AddFace(int faceIndex)
{
	if (faceIndex < 0)
		return;
	BOOL found = FALSE;
	for (int i = 0; i < mConnectedFaces.Count(); i++)
	{
		if (faceIndex == mConnectedFaces[i])
		{
			found = TRUE;
			i = mConnectedFaces.Count();
		}
	}
	if (found == FALSE)
		mConnectedFaces.Append(1,&faceIndex,8);
}

int Vertex::GetVisibleEdge(int ith)
{
	int ct = 0;
	for (int i = 0; i < mConnectedTo.Count(); i++)
	{
		if (mConnectedTo[i].mHiddenEdge == FALSE)
		{
			if (ct == ith)
				return i;
			ct++;			
		}
	}
	return -1;
}




int Vertex::GetOpposingVert(int vertexIndex)
{
	int offSet = mVisibleEdgeCount/2;
	int start = -1;
	int ithOpposing = -1;
	for (int i = 0; i < mConnectedTo.Count(); i++)
	{
		if (mConnectedTo[i].mVert == vertexIndex)
		{
			start = i;
			ithOpposing = mConnectedTo[i].mIthOpposingEdge;
			i = mConnectedTo.Count();
		}
	}

	if (ithOpposing != -1)
		return  mConnectedTo[ithOpposing].mVert;


	return -1;

}

bool Vertex::IsBorderEdge(int vertexIndex)
{
	for (int i = 0; i < mConnectedTo.Count(); i++)
	{
		if (mConnectedTo[i].mVert == vertexIndex)
		{
			if (mConnectedTo[i].mBorderEdge)
				return true;
			else
				return false;
		}
	}
	return false;
}

int Vertex::GetNextVert(int vertexIndex,int &ith)
{
	ith = -1;
	for (int i = 0; i < mConnectedTo.Count(); i++)
	{
		if (mConnectedTo[i].mVert == vertexIndex)
		{
			bool done = false;
			int j = i;
			int bailID = j;
			while (!done)
			{
				j++;
				if (j >= mConnectedTo.Count())
					j = 0;
				if ( !mConnectedTo[j].mHiddenEdge )
				{
					ith = j;
					return mConnectedTo[j].mVert;
				}
				if (j == bailID)
					done = true;
			}

		}
	}
	return -1;
}

TVConnectionInfo::TVConnectionInfo(MeshTopoData *ld)
{
	mMd = ld;
	{
		mVertex.SetCount(ld->GetNumberTVVerts());
		for (int vertexIndex = 0; vertexIndex < ld->GetNumberTVVerts(); vertexIndex++)
		{
			mVertex[vertexIndex] = new Vertex();
		}
		for (int edgeIndex = 0; edgeIndex < ld->GetNumberTVEdges(); edgeIndex++)
		{
			BOOL hiddenEdge = ld->GetTVEdgeHidden(edgeIndex);
			int a = ld->GetTVEdgeVert(edgeIndex,0);
			int b = ld->GetTVEdgeVert(edgeIndex,1);
			int fct = ld->GetTVEdgeNumberTVFaces(edgeIndex);
			int fa = -1;
			int fb = -1;
			if (fct == 1)
			{
				fa = ld->GetTVEdgeConnectedTVFace(edgeIndex,0);
			}
			else if (fct == 2)
			{
				fa = ld->GetTVEdgeConnectedTVFace(edgeIndex,0);
				fb = ld->GetTVEdgeConnectedTVFace(edgeIndex,1);
			}
			//else bad edge skip it

			VertexConnectedTo dataA(a,edgeIndex,fct,fa,fb,hiddenEdge);
			VertexConnectedTo dataB(b,edgeIndex,fct,fa,fb,hiddenEdge);
			mVertex[a]->mConnectedTo.Append(1,&dataB,4);
			mVertex[b]->mConnectedTo.Append(1,&dataA,4);
			mVertex[a]->AddFace(fa);
			mVertex[a]->AddFace(fb);
			mVertex[b]->AddFace(fa);
			mVertex[b]->AddFace(fb);
		}
	}
	OrderLists();
}

TVConnectionInfo::~TVConnectionInfo()
{
	for (int i = 0; i < mVertex.Count(); i++)
	{
		if (mVertex[i])
			delete mVertex[i];
	}
}


void TVConnectionInfo::DumpVert(int index)
{
	DebugPrint(_T("VERTEX %d\n"),index);
	int ct = mVertex[index]->mConnectedTo.Count();
	DebugPrint(_T("    num Connected Verts %d\n"),ct);
	DebugPrint(_T("		verts "));
	for (int i = 0; i < ct; i++ )
	{
		DebugPrint(_T(" %d"),mVertex[index]->mConnectedTo[i].mVert);
	}
	DebugPrint(_T("\n"));
	DebugPrint(_T("		edges "));
	for (int i = 0; i < ct; i++ )
	{
		DebugPrint(_T(" %d(%d)"),mVertex[index]->mConnectedTo[i].mEdge,mVertex[index]->mConnectedTo[i].mHiddenEdge);
	}
	DebugPrint(_T("\n"));

	ct = mVertex[index]->mConnectedFaces.Count();
	DebugPrint(_T("    num Connected Faces %d\n"),mVertex[index]->mConnectedFaces.Count());
	DebugPrint(_T("		Faces "));
	for (int i = 0; i < ct; i++ )
	{
		DebugPrint(_T(" %d"),mVertex[index]->mConnectedFaces[i]);
	}
	DebugPrint(_T("\n"));

	
	ct = mVertex[index]->mConnectedTo.Count();
	DebugPrint(_T("  Opposing Edge Data \n"));
	for (int i = 0; i < ct; i++ )
	{
		int opposing = mVertex[index]->mConnectedTo[i].mIthOpposingEdge;
		if (opposing != -1)
			DebugPrint(_T(" %d(%d ith) "),mVertex[index]->mConnectedTo[opposing].mVert,opposing);
		else
			DebugPrint(_T(" -1(-1 ith) "),mVertex[index]->mConnectedTo[opposing].mVert,opposing);
	}
	DebugPrint(_T("\n"));

}

void TVConnectionInfo::OrderLists()
{

	int hiddenVertID = -1;
	for (int i = 0; i < mVertex.Count(); i++)
	{




		if (mVertex[i]->mConnectedFaces.Count() > 0)
		{
		//	DumpVert(i);

			//order our faces
			Tab<int> faceStack;
			Tab<int> finalFaceStack;
			Tab<int> remainingFaces = mVertex[i]->mConnectedFaces;
			faceStack.Append(1,&remainingFaces[0]);
			remainingFaces.Delete(0,1);
			int vert = i;
			
			//gather all the faces that are together and ordered them
			while (remainingFaces.Count() > 0)
			{
				int currentFace = faceStack[0];
				int prevVert = -1;
				int nextVert = -1;
				int deg = mMd->GetFaceDegree(currentFace);
				for (int j = 0; j < deg; j++)
				{
					int a = mMd->GetFaceTVVert(currentFace,j);
					if (a == vert)
					{
						nextVert = mMd->GetFaceTVVert(currentFace,(j+1)%deg);
						prevVert = mMd->GetFaceTVVert(currentFace,(j+deg-1)%deg);
						j = deg;
					}
				}

				int  hit = FALSE;
				for (int j = 0; j < remainingFaces.Count(); j++)
				{
					int faceIndex = remainingFaces[j];
					//see if we have any matching faces
					int ithEdge = mMd->FindUVEdge(faceIndex,vert,nextVert);
					if (ithEdge != -1) //we have a match
					{
						faceStack.Append(1,&faceIndex,8);
						remainingFaces.Delete(j,1);					 
						j = -1;
						hit = TRUE;

						//find the new nextvert
						int deg = mMd->GetFaceDegree(faceIndex);						
						for (int k = 0; k < deg; k++)
						{
							int a = mMd->GetFaceTVVert(faceIndex,k);
							if (a == vert)
							{
								nextVert = mMd->GetFaceTVVert(faceIndex,(k+1)%deg);
								k = deg;
							}
						}
					}
					else
					{
						ithEdge = mMd->FindUVEdge(faceIndex,vert,prevVert);
						if (ithEdge != -1) //we have a match
						{
							faceStack.Insert(0,1,&faceIndex);
							remainingFaces.Delete(j,1);
							j = -1;
							hit = TRUE;

							//find the new prevvert
							int deg = mMd->GetFaceDegree(faceIndex);
							for (int k = 0; k < deg; k++)
							{
								int a = mMd->GetFaceTVVert(faceIndex,k);
								if (a == vert)
								{									
									prevVert = mMd->GetFaceTVVert(faceIndex,(k+deg-1)%deg);
									k = deg;
								}
							}
						}						
					}
				}

				//see if we hit an open edge if so add all the faces we have and add a -1 to mark the gap
				int remainingFacesCount = remainingFaces.Count();
				if (hit == FALSE || (remainingFacesCount == 0))
				{
					for (int j = 0; j < faceStack.Count(); j++)
						finalFaceStack.Append(1,&faceStack[j],8);
					int neg = -1;
					if (remainingFaces.Count() != 0)
						finalFaceStack.Append(1,&neg);

					if (remainingFaces.Count() > 0)
					{											
						faceStack.SetCount(0);
						faceStack.Append(1,&remainingFaces[0],8);
						remainingFaces.Delete(0,1);
					}
				}

			}


			//special condition with 1 face
			if (mVertex[i]->mConnectedFaces.Count() == 1)
			{
				int neg = -1;
				finalFaceStack = mVertex[i]->mConnectedFaces;
				finalFaceStack.Append(1,&neg);
			}		
			//else see if the last and end faces dont connect and if mark it with -1 
			else if (mVertex[i]->mConnectedFaces.Count() > 1)
			{

				int firstFace = finalFaceStack[0];
				int lastFace = finalFaceStack[finalFaceStack.Count()-1];
				int pVert = -1;
				int nVert = -1;
				if ((firstFace != -1) && (lastFace != -1))
				{
					int deg = mMd->GetFaceDegree(firstFace);
					for (int j = 0; j < deg; j++)
					{
						int a = mMd->GetFaceTVVert(firstFace,j);
						if (a == vert)
						{						
							pVert = mMd->GetFaceTVVert(firstFace,(j+deg-1)%deg);
							j = deg;
						}
					}
					deg = mMd->GetFaceDegree(lastFace);
					for (int j = 0; j < deg; j++)
					{
						int a = mMd->GetFaceTVVert(lastFace,j);
						if (a == vert)
						{
							nVert = mMd->GetFaceTVVert(lastFace,(j+1)%deg);
							j = deg;
						}
					}
				}

				//there is a gap add a neg -1 to signify this
				if (nVert != pVert)
				{
					int neg = -1;
					finalFaceStack.Append(1,&neg);
				}
			}



/*
			DebugPrint ("	unordered face list ");
			for (int j = 0; j < mVertex[i]->mConnectedFaces.Count(); j++)
			{
				DebugPrint(_T(" %d"),mVertex[i]->mConnectedFaces[j]);
			}
			DebugPrint (_T("\n"));

*/
			mVertex[i]->mConnectedFaces = finalFaceStack;
/*
			DebugPrint ("	ordered face list ");
			for (int j = 0; j < mVertex[i]->mConnectedFaces.Count(); j++)
			{
				DebugPrint(_T(" %d"),mVertex[i]->mConnectedFaces[j]);
			}
			DebugPrint (_T("\n"));
*/
			
			
			//now order the edges
			//just a temp list to store the verts as we sort them
			Tab<VertexConnectedTo> sortedtVerts;
			
			if (mVertex[i]->mConnectedFaces.Count() == 2)  //special case for corners since they have both the same face IDs
			{
				int faceIndex1 = mVertex[i]->mConnectedFaces[0];
				if (faceIndex1 == -1)
					faceIndex1 = mVertex[i]->mConnectedFaces[1];
								
				int prevVert = -1;
				int nextVert = -1;
				int deg = mMd->GetFaceDegree(faceIndex1);
				for (int j = 0; j < deg; j++)
				{
					int a = mMd->GetFaceTVVert(faceIndex1,j);
					if (a == vert)
					{
						nextVert = mMd->GetFaceTVVert(faceIndex1,(j+1)%deg);
						prevVert = mMd->GetFaceTVVert(faceIndex1,(j+deg-1)%deg);
						j = deg;
					}
				}

				//see if we need to swap edge order
				if (mVertex[i]->mConnectedTo[0].mVert == nextVert)
				{
					
					sortedtVerts.Append(1,&mVertex[i]->mConnectedTo[1],8);
					sortedtVerts.Append(1,&mVertex[i]->mConnectedTo[0],8);
				}
				else
				{
					sortedtVerts.Append(1,&mVertex[i]->mConnectedTo[0],8);
					sortedtVerts.Append(1,&mVertex[i]->mConnectedTo[1],8);
				}
				VertexConnectedTo data(hiddenVertID--,-1,0,-1,-1,FALSE);
				sortedtVerts.Append(1,&data,8);

			}
			else
			{
				int currentEdge = 0;
				//we can just walk around looking for matching faces at the edge
				for (int faceIndex = 0; faceIndex < mVertex[i]->mConnectedFaces.Count(); faceIndex++)
				{
					int fa = finalFaceStack[faceIndex];
					int fb = finalFaceStack[(faceIndex+1)%mVertex[i]->mConnectedFaces.Count()];
					//make sure to add the gap if there is no face 
					if (fa == -1)
					{
						VertexConnectedTo data(hiddenVertID--,-1,0,-1,-1,FALSE);
						sortedtVerts.Append(1,&data,8);  //open edges are not hidden since we want to count them for purpuses of rings and loops
						currentEdge++;
					}
					//find a matching edge
					for (int k = 0; k < mVertex[i]->mConnectedTo.Count(); k++)
					{
						int testa = mVertex[i]->mConnectedTo[k].mFace[0];
						int testb = mVertex[i]->mConnectedTo[k].mFace[1];
						if ( ((testa == fa)  && (testb == fb)) ||
							((testa == fb)  && (testb == fa)) )
						{
							sortedtVerts.Append(1,&mVertex[i]->mConnectedTo[k],8);
	//						sortedtVerts[currentEdge] = mVertex[i]->mConnectedTo[k];
							currentEdge++;
							k =  mVertex[i]->mConnectedTo.Count();
						}
					}
				}
			}
/*
			DebugPrint ("	unordered edge list ");
			for (int j = 0; j < mVertex[i]->mConnectedTo.Count(); j++)
			{
				DebugPrint(_T(" %d"),mVertex[i]->mConnectedTo[j].mVert);
			}
			DebugPrint (_T("\n"));
*/

			mVertex[i]->mConnectedTo = sortedtVerts;

/*
			DebugPrint ("	ordered edge list ");
			for (int j = 0; j < mVertex[i]->mConnectedTo.Count(); j++)
			{
				DebugPrint(_T(" %d"),mVertex[i]->mConnectedTo[j].mVert);
			}
			DebugPrint (_T("\n"));
*/

			int ct = mVertex[i]->mConnectedTo.Count();
			for (int j = 0; j < ct; j++)
			{
				if (mVertex[i]->mConnectedTo[j].mHiddenEdge)
					mVertex[i]->mHiddenEdgeCount++;
				else 
					mVertex[i]->mVisibleEdgeCount++;
			}
		}
	}

//build our opposing edges now
	for (int i = 0; i < mVertex.Count(); i++)
	{
		
		Vertex *v = mVertex[i];
		v->mNumRealEdges = 0;
		int deg = v->mConnectedTo.Count();
		for (int j = 0; j < deg; j++)
		{
			if ( (v->mConnectedTo[j].mVert >= 0) && (v->mConnectedTo[j].mHiddenEdge == FALSE) )
				v->mNumRealEdges++;
		}


		if (v->mNumRealEdges == 3)
		{
			int center = -1;
			for (int j = 0; j < deg; j++)
			{
				if (v->mConnectedTo[j].mVert < 0) 
					center = j;
					
			}

			bool done = false;
			int prevIth = center;
			int nextIth = center;
			int nIndex = -1;
			int pIndex = -1;
			while (!done)
			{
				nextIth++;
				prevIth--;
				nextIth = (nextIth+deg)%deg;
				prevIth = (prevIth+deg)%deg;
				if ((v->mConnectedTo[nextIth].mVert >= 0) && (v->mConnectedTo[nextIth].mHiddenEdge == FALSE) )
				{
					if (nIndex == -1)
						nIndex = nextIth;
				}
				if ((v->mConnectedTo[prevIth].mVert >= 0) && (v->mConnectedTo[prevIth].mHiddenEdge == FALSE) )
				{
					if (pIndex == -1)
						pIndex = prevIth;
				}
				if ((pIndex != -1) && (nIndex != -1))
					done = true;
			}
			//find our -1 edge go left and right of it til hit a real edge
			v->mConnectedTo[pIndex].mIthOpposingEdge = nIndex;
			v->mConnectedTo[nIndex].mIthOpposingEdge = pIndex;

		}
		else if (v->mNumRealEdges > 3)
		{
			BOOL valid = FALSE;
			if ((v->mNumRealEdges%2) == 0)
				valid = TRUE;
			if (valid)
			{
				int halfIth = v->mNumRealEdges/2;
				for (int j = 0; j < deg; j++)
				{
					int startIndex = j;
					BOOL isEdgeHidden = v->mConnectedTo[startIndex].mHiddenEdge;
					
					if ( (v->mConnectedTo[startIndex].mVert < 0) || isEdgeHidden )
						v->mConnectedTo[j].mIthOpposingEdge = -1;
					else
					{
						int ct = 0;
						while (ct != halfIth)
						{
							startIndex++;
							if (startIndex >= deg)
								startIndex = 0;
							if ( (v->mConnectedTo[startIndex].mVert >= 0) && (v->mConnectedTo[startIndex].mHiddenEdge == FALSE) )
								ct++;
						}
						v->mConnectedTo[j].mIthOpposingEdge = startIndex;
					}
				}

			}
			else
				for (int j = 0; j < deg; j++)
					v->mConnectedTo[j].mIthOpposingEdge = -1;
		}
//		DumpVert(i);
	}

}

void TVConnectionInfo::BuildBorders(BitArray fSel)
{
	//fillout border info
	for (int i = 0; i < mVertex.Count(); i++)
	{

		//see if we have an open edge
		int ct = mVertex[i]->mVisibleEdgeCount;
		bool openEdge = false;
		for (int j = 0; j < ct; j++ )
		{
			int neighbor = mVertex[i]->GetVisibleEdge(j);
			if (mVertex[i]->mConnectedTo[neighbor].mFaceCount == 1)
			{
				openEdge = true;
				j = ct;
			}
		}

		//see if we have an edge with on face selected and another not
		bool faceSel = false;
		bool faceUnSel = false;

		for (int j = 0; j < mVertex[i]->mConnectedFaces.Count(); j++)
		{
			int faceIndex = mVertex[i]->mConnectedFaces[j];
			if (faceIndex != -1)
			{
				if (fSel[faceIndex] == TRUE)
					faceSel = true;
				else
					faceUnSel = true;
			}
		}	
		mVertex[i]->mBorder = false;


		//now do the connecting edges
		if (faceSel && faceUnSel)
			mVertex[i]->mBorder = true;
		else if (faceSel && openEdge)
			mVertex[i]->mBorder = true;

		for (int j = 0; j < ct; j++ )
		{
			int neighbor = mVertex[i]->GetVisibleEdge(j);
			int fa = mVertex[i]->mConnectedTo[neighbor].mFace[0];
			int fb = mVertex[i]->mConnectedTo[neighbor].mFace[1];
			int fct = mVertex[i]->mConnectedTo[neighbor].mFaceCount;
			if (fct == 1 && fSel[fa])
				mVertex[i]->mConnectedTo[neighbor].mBorderEdge = TRUE;
			else if (fct == 2)
			{
				if ( (fSel[fa] && !fSel[fb])  ||
					 (fSel[fb] && !fSel[fa])  )
					 mVertex[i]->mConnectedTo[neighbor].mBorderEdge = TRUE;
			}		
		}
	}
}


