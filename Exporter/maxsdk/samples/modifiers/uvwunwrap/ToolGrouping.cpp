
/*

Copyright [2010] Autodesk, Inc.  All rights reserved.

Use of this software is subject to the terms of the Autodesk license agreement 
provided at the time of installation or download, or which otherwise accompanies 
this software in either electronic or hard copy form.   
*/


#include "ToolGrouping.h"
#include "unwrap.h"

const TSTR ToolGrouping::mEmptyString = _M("--EMPTYENTRY--");


/************   Tool Clustering Methods *********************/
ToolGrouping::ToolGrouping()
{
	mMod = NULL;
}


ToolGrouping::~ToolGrouping()
{
}


void ToolGrouping::Init(UnwrapMod *mod)
{
	mMod = mod;
}

void ToolGrouping::CreateGroupFromSelection(TSTR name)
{

	//see if we have an existing group and if so delete that one
	//since we are replacing it
	int dupeID = -1;
	FindGroupID(name,dupeID);
	if (dupeID != -1)
		DeleteGroup(dupeID);

	//find a new group ID
	int index = -1;
//	TSTR emptyStr;
//	emptyStr.printf("--EMPTYENTRY--");
	FindGroupID(mEmptyString,index);

	if (index != -1)
	{
		if (name.Length() == 0)
			name.printf(_T("__INTERNALCLUSTER_%d"),index);

		mMod->pblock->SetValue(unwrap_group_name,0,name.data(),index);
		float den = 1.0f;
		mMod->pblock->SetValue(unwrap_group_density,0,den,index);
	}
	else
	{
		index = mMod->pblock->Count(unwrap_group_name);

		if (name.Length() == 0)
			name.printf(_T("__INTERNALCLUSTER_%d"),index);

		const TCHAR *cname = name.data();
		mMod->pblock->Append(unwrap_group_name,1,&cname);
		float den = 1.0f;
		mMod->pblock->Append(unwrap_group_density,1,&den);
	}

	//create the new group
	for (int i = 0; i < mMod->GetMeshTopoDataCount(); i++)
	{		
		mMod->GetMeshTopoData(i)->GetToolGroupingData()->CreateGroupFromSelection(index);
	}
}


void ToolGrouping::DeleteGroup(int clusterID)
{
//	TSTR emptyStr;
//	emptyStr.printf("--EMPTYENTRY--");
	mMod->pblock->SetValue(unwrap_group_name,0,mEmptyString,clusterID);
	for (int i = 0; i < mMod->GetMeshTopoDataCount(); i++)
	{		
		mMod->GetMeshTopoData(i)->GetToolGroupingData()->DeleteGroup(clusterID);
	}
}
void ToolGrouping::DeleteGroup(TSTR name)
{
	int clusterID = -1;
	FindGroupID(name,clusterID);
	if (clusterID != -1)
		DeleteGroup(clusterID);
}

void ToolGrouping::SelectGroup(int clusterID)
{
	for (int i = 0; i < mMod->GetMeshTopoDataCount(); i++)
	{
		mMod->GetMeshTopoData(i)->GetToolGroupingData()->SelectGroup(clusterID,mMod->fnGetTVSubMode());
	}
}
void ToolGrouping::SelectGroup(TSTR &clusterName)
{
	int clusterID = -1;
	FindGroupID(clusterName,clusterID);
	if (clusterID != -1)
		SelectGroup(clusterID);
}

float ToolGrouping::GetGroupTexelDensity(int clusterID)
{
	float den = 1.0f;
	int ct = mMod->pblock->Count(unwrap_group_density);
	if ( (clusterID >= 0) && (clusterID < ct) )	
	{
		den = mMod->pblock->GetFloat(unwrap_group_density,0,clusterID);
	}
	return den;
}
float ToolGrouping::GetGroupTexelDensity(TSTR clusterName)
{
	float den = 1.0f;
	int clusterID = -1;
	FindGroupID(clusterName,clusterID);
	if (clusterID != -1)
		den = GetGroupTexelDensity(clusterID);
	return den;
}

void ToolGrouping::SetGroupTexelDensity(int clusterID, float v)
{
	int ct = mMod->pblock->Count(unwrap_group_density);
	if ( (clusterID >= 0) && (clusterID < ct) )	
	{
		mMod->pblock->SetValue(unwrap_group_density,0,v,clusterID);
	}
}
void ToolGrouping::SetGroupTexelDensity(TSTR clusterName, float v)
{
	int ct = mMod->pblock->Count(unwrap_group_density);
	int clusterID = -1;
	FindGroupID(clusterName,clusterID);
	if (clusterID != -1)
		SetGroupTexelDensity(clusterID,v);
}

void ToolGrouping::GetGroupName(int clusterID, TSTR &name)
{
	FindGroupName(clusterID,name);
}
void ToolGrouping::SetGroupName(int clusterID, TSTR &name)
{
	int ct = mMod->pblock->Count(unwrap_group_name);
	if ( (clusterID >= 0) && (clusterID < ct) )	
	{
		mMod->pblock->SetValue(unwrap_group_name,0,name.data(),clusterID);
	}
}

void ToolGrouping::FindGroupID(const TSTR &name, int &clusterID)
{
	int ct = mMod->pblock->Count(unwrap_group_name);
	for (int i = 0; i < ct; i++)
	{
		const MCHAR *testName = mMod->pblock->GetStr(unwrap_group_name,0,i);
		if (_tcscmp(testName,name.data()) == 0)
		{
			clusterID = i;
			return;
		}
	}
}
void ToolGrouping::FindGroupName(int clusterID, TSTR &name)
{
	name.printf(_T("-NOTFOUND-"));
	int ct = mMod->pblock->Count(unwrap_group_name);
	if ( (clusterID >= 0) && (clusterID < ct) )
	{
		name.printf(_T("%s"),mMod->pblock->GetStr(unwrap_group_name,0,clusterID));
	}
}

void ToolGrouping::Dump()
{
	DebugPrint(_T("**DUMP*****\n"));
	int ct = mMod->pblock->Count(unwrap_group_name);
	for (int i = 0; i < ct; i++)
	{
		const MCHAR *testName = mMod->pblock->GetStr(unwrap_group_name,0,i);
		DebugPrint(_T("Cluster ID %d name %s\n"),i,testName);
	}

	for (int i = 0; i < mMod->GetMeshTopoDataCount(); i++)
	{		
		mMod->GetMeshTopoData(i)->GetToolGroupingData()->Dump();
	}
}

void ToolGrouping::Hold()
{
	if (theHold.Holding())
	{
		for (int i = 0; i < mMod->GetMeshTopoDataCount(); i++)
		{
			theHold.Put(new ToolGroupingRestore(mMod->GetMeshTopoData(i)));
		}
	}
}