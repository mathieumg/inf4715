
/*

Copyright [2010] Autodesk, Inc.  All rights reserved.

Use of this software is subject to the terms of the Autodesk license agreement 
provided at the time of installation or download, or which otherwise accompanies 
this software in either electronic or hard copy form.   
*/

/*

action items
UI
Save/Load

add to pack dialog


undo in saveface and points
simple undo
*/

#include "ToolGroupingData.h"
#include "MeshTopoData.h"
#include "unwrap.h"

/************   CLuster Data Methods *********************/
ToolGroupingData::ToolGroupingData()
{
	mMd = NULL;
}
void ToolGroupingData::Init(MeshTopoData *md)
{
	mMd = md;

	if (mFaceGroupID.Count() != mMd->GetNumberFaces())
	{
		int lastSize = mFaceGroupID.Count();
		mFaceGroupID.SetCount(mMd->GetNumberFaces());
		for (int i = lastSize; i < mFaceGroupID.Count(); i++)
			mFaceGroupID[i] = -1;
	}

}
void ToolGroupingData::Reset()
{
	mFaceGroupID.SetCount(mMd->GetNumberFaces());
	for (int i = 0; i < mFaceGroupID.Count(); i++)
		mFaceGroupID[i] = -1;
}
void ToolGroupingData::GetGroup(int clusterID, Tab<int> &cluster)
{
	cluster.SetCount(0);
	for (int i = 0; i < mFaceGroupID.Count(); i++)
	{
		if (mFaceGroupID[i]==clusterID)
			cluster.Append(1,&i,mFaceGroupID.Count()/10);
	}
}
void ToolGroupingData::GetGroup(int clusterID, BitArray &cluster)
{
	cluster.SetSize(mFaceGroupID.Count());
	cluster.ClearAll();
	for (int i = 0; i < mFaceGroupID.Count(); i++)
	{
		if (mFaceGroupID[i]==clusterID)
		{
			cluster.Set(i,TRUE);
		}
	}
}

void ToolGroupingData::CreateGroupFromSelection(int clusterID)
{
	//expand the selection to elements	
	mMd->SelectElement(TVFACEMODE,TRUE);
	for (int i = 0; i < mFaceGroupID.Count(); i++)
	{
		if (mMd->GetFaceSelected(i))
			mFaceGroupID[i] = clusterID;
	}
}

void ToolGroupingData::DeleteGroup(int clusterID)
{
	for (int i = 0; i < mFaceGroupID.Count(); i++)
	{
		if (mFaceGroupID[i] == clusterID)
			mFaceGroupID[i] = -1;

	}
}

void ToolGroupingData::SelectGroup(int clusterID, int subObjectMode)
{
	mMd->ClearFaceSelection();
	for (int i = 0; i < mFaceGroupID.Count(); i++)
	{
		if (mFaceGroupID[i] == clusterID)
			mMd->SetFaceSelected(i,TRUE);			
	}

	if (subObjectMode != TVFACEMODE)
	{
		if (subObjectMode != TVEDGEMODE) 
			mMd->ConvertFaceToEdgeSel();
		else if (subObjectMode != TVVERTMODE) 
		{
			BitArray vsel;
			vsel.SetSize(mMd->GetNumberTVVerts());
			vsel.ClearAll();
			mMd->GetVertSelFromFace(vsel);	
			mMd->SetTVVertSelection(vsel);
		}
	}

}

int ToolGroupingData::GetGroupID(int faceIndex)
{
	return mFaceGroupID[faceIndex];
}

void ToolGroupingData::GetFaceGroupIDs(Tab<int> &data)
{
	data = mFaceGroupID;
}

void ToolGroupingData::SetFaceGroupIDs(Tab<int> &data)
{
	mFaceGroupID = data;
}

ULONG ToolGroupingData::Save(ISave *isave)
{
	int fct = mFaceGroupID.Count();
	ULONG nb = 0;
	isave->Write(&fct, sizeof(fct), &nb);
	if (fct > 0 )
		return isave->Write(mFaceGroupID.Addr(0), sizeof(int)*fct, &nb);
	return IO_OK;
}
ULONG ToolGroupingData::Load(ILoad *iload)
{
	ULONG nb = 0;
	int ct = 0;
	iload->Read(&ct, sizeof(ct), &nb);
	mFaceGroupID.SetCount(ct);
	if (ct > 0)
		return iload->Read(mFaceGroupID.Addr(0), sizeof(int)*mFaceGroupID.Count(), &nb);
	return IO_OK;
}


void ToolGroupingData::Dump()
{
	DebugPrint(_T("Face IDs\n"));
	for (int i = 0; i < mFaceGroupID.Count(); i++)
	{
		DebugPrint(_T(" %d, "),mFaceGroupID[i]);
	}
}