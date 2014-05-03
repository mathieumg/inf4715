
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
#include "hold.h"

class UnwrapMod;
class MeshTopoData;


/*
this is all the grouping info stored in the modifier.  Data is split between the modifier and the local data.
The modifier stores the group ID list, names and texel density.
The local data stores the array of Face IDs 
*/
class ToolGrouping
{

public:
	ToolGrouping();

	~ToolGrouping();

	void Init(UnwrapMod *mod);

	//releases allour clusting info
	void Free();

	//creates a new cluster
	void CreateGroupFromSelection(TSTR name);

	//deletes a group
	void DeleteGroup(int clusterID);
	void DeleteGroup(TSTR name);
	
	//selects a cluster
	void SelectGroup(int clusterID);
	void SelectGroup(TSTR &clusterName);
	
	//gets texel density assined to this cluster
	float GetGroupTexelDensity(int clusterID);
	float GetGroupTexelDensity(TSTR clusterName);

	//sets texel density assined to this cluster
	void SetGroupTexelDensity(int clusterID, float v);
	void SetGroupTexelDensity(TSTR clusterName, float v);

	//gets a group name (normally names are created internally)
	void GetGroupName(int clusterID, TSTR &name);
	void SetGroupName(int clusterID, TSTR &name);

	//gets a group ID from a name
	void FindGroupID(const TSTR &name, int &id);
	//gets a name from a group ID
	void FindGroupName(int id, TSTR &name);

	void Dump();

	//this holds our grouping data
	void Hold();

protected:

	void Init();

	UnwrapMod *mMod;
	const static TSTR mEmptyString;
	

};


//The undo class for a group
class ToolGroupingRestore : public RestoreObj 
{
public:

	MeshTopoData*		mMd;

	Tab<int>			mUGroupID;
	Tab<int>			mRGroupID;

	ToolGroupingRestore(MeshTopoData *md);

	void Restore(int isUndo);
	void Redo();
	int Size();
	void EndHold();
	TSTR Description();

};
