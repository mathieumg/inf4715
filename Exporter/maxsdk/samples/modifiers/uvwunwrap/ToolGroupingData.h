
/*

Copyright [2010] Autodesk, Inc.  All rights reserved.

Use of this software is subject to the terms of the Autodesk license agreement 
provided at the time of installation or download, or which otherwise accompanies 
this software in either electronic or hard copy form.   

*/

#pragma once

#include "tab.h"
#include "bitarray.h"
#include "strclass.h"

class MeshTopoData;

/*
This describes the clusters for a local data
*/
class ToolGroupingData
{
public:
	ToolGroupingData();

	// creates and initializes the cluster IDs to -1
	void Init(MeshTopoData *md);

	//reset all the ids to -1
	void Reset();

	//returns all the faces with that matches the cluster id
	void GetGroup(int clusterID, Tab<int> &cluster);
	//returns all the faces with that matches the cluster id
	void GetGroup(int clusterID, BitArray &cluster);

	//takes the current selection and turns it into a cluster
	void CreateGroupFromSelection(int index);

	//deletes this group
	void DeleteGroup(int index);

	//selects the faces associated with this ID
	void SelectGroup(int index, int subObjectMode);

	//returns the group ID associated with this face
	int GetGroupID(int faceIndex);

	//copies the face group id list into data
	void GetFaceGroupIDs(Tab<int> &data);
	//sets the face group index list to data
	void SetFaceGroupIDs(Tab<int> &data);


	//the sava and load methods
	ULONG Save(ISave *isave);
	ULONG Load(ILoad *iload);


	//dumps the cluster info to the debug window
	void Dump();

protected:
	MeshTopoData *mMd;
	//this is the cluster ids of this mesh data, -1 means that the face has not been assigned
	//need to move it inside the topo data
	Tab<int> mFaceGroupID;
};
