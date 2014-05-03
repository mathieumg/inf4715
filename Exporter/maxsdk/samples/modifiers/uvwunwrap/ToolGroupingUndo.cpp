
/*

Copyright [2010] Autodesk, Inc.  All rights reserved.

Use of this software is subject to the terms of the Autodesk license agreement 
provided at the time of installation or download, or which otherwise accompanies 
this software in either electronic or hard copy form.   
*/


#include "ToolGrouping.h"
#include "unwrap.h"
#include "modsres.h"

ToolGroupingRestore::ToolGroupingRestore(MeshTopoData *md)
{
	mMd = md;
	mMd->GetToolGroupingData()->GetFaceGroupIDs(mUGroupID);
}

void ToolGroupingRestore::Restore(int isUndo)
{
	if (isUndo)
	{
		mMd->GetToolGroupingData()->GetFaceGroupIDs(mRGroupID);
	}


	mMd->GetToolGroupingData()->SetFaceGroupIDs(mUGroupID);

}
void ToolGroupingRestore::Redo()
{
	mMd->GetToolGroupingData()->SetFaceGroupIDs(mRGroupID);
}
int ToolGroupingRestore::Size()
{
	return (mUGroupID.Count()+mRGroupID.Count()*4);
}
void ToolGroupingRestore::EndHold()
{

}
TSTR ToolGroupingRestore::Description()
{
	return TSTR(GetString(IDS_GROUPSELECTIONL));
}

