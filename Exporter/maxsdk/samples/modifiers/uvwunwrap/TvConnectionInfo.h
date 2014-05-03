
/*

Copyright 2010 Autodesk, Inc.  All rights reserved. 

Use of this software is subject to the terms of the Autodesk license agreement provided at 
the time of installation or download, or which otherwise accompanies this software in either 
electronic or hard copy form. 

*/

#pragma once

#include "tab.h"

class MeshTopoData;

//This is what is connected to a vertex along an edge
class VertexConnectedTo
{
public:
	//  v  vertex index
	//  e  edge index
	//  faceCount the number of faces attached to this edge
	//  faceA. faceB the indices of the face attached to the edge
	// hidden Edge whether the edge is hidden
	VertexConnectedTo(int v, int e, int faceCount, int faceA, int faceB, BOOL hiddenEdge);
	int mVert;
	int mEdge;
	int mFaceCount;
	int mFace[2];
	BOOL mHiddenEdge;
	BOOL mBorderEdge;  //Border edge is computed by TVConnectionInfo constructor is whether the edge shares an open edge or share selection
	int mIthOpposingEdge;

protected:
	VertexConnectedTo();
};

//This is the class that contains all the vertex info and everything connected to that vertex
class Vertex
{
public:
	Vertex();

	//this is the grid location of the vertex
	int mX,mY;
	
	//The list of all edges that connect to this vertex and all the stuff attched to those edges
	//this is in order list CW around the vertex
	Tab<VertexConnectedTo> mConnectedTo;
	//The list of all faces connected to this vertex
	//this is in order list CW around the vertex
	Tab<int> mConnectedFaces;


	int mVisibleEdgeCount;  // how many visible edges touch the vertex
	int mHiddenEdgeCount;	//how many hidden edges touch the vertex
	int mNumRealEdges;		//this is the number of real edges ie non hidden and really connected to a vetrex

	bool mBorder;			//whether this vertex sits on an open or selection border
	bool mUset, mVset;      //flags used to determine is the mU/V have been set
	float mU,mV;             // UV location of the vertex
	int mDir;               // if a border edge this is the direction of that edge 0 -x, 1 +y, 2 +x, 3-y

	//returns if this vertex is regular which means that you can run a valid edge loop this vertex
	//BOOL IsGood();
	//adds a face to the connection list
	void AddFace(int faceIndex);
	//returns the opposing vertex to VertexIndex, returns -1 if there is none
	int GetOpposingVert(int vertexIndex);
	//returns ith visible edge, used to walk around hidden edges
	int GetVisibleEdge(int ith);

	//this will return the next clock wise vert not attached to a hidden edge
	//you also get the ith edge from the start
	int GetNextVert(int vertexIndex,int &ith);


	//this will loop find the edge thta goes the vertexIndex and return if the edge is a border
	bool IsBorderEdge(int vertexIndex);

};


//This is class that holds all the vertex connection info
class TVConnectionInfo
{
public:

	TVConnectionInfo(MeshTopoData *ld);
	~TVConnectionInfo();
	//builds the border info based on a face selection
	void BuildBorders(BitArray fSel);
	
	//list of all the vertices
	Tab<Vertex*>   mVertex;	


protected:
	MeshTopoData *mMd;
	//orders the vertex connnection info
	void OrderLists();

	void DumpVert(int index);
};
