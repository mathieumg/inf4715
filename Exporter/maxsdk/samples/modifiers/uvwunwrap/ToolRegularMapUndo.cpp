

/*

Copyright [2010] Autodesk, Inc.  All rights reserved.

Use of this software is subject to the terms of the Autodesk license agreement 
provided at the time of installation or download, or which otherwise accompanies 
this software in either electronic or hard copy form.   

*/

#include "ToolRegularMap.h"
#include "unwrap.h"
#include "modsres.h"

RegularMapRestore::RegularMapRestore(RegularMap *m, bool recomputeLocalData, MeshTopoData *md)
{
	map = m;
	map->GetData(mUBorderEdges,mUProcessedFaces);
	mUGeomEdge = map->GetLocalData()->GetGeomEdgeSelection();
	mUTVEdge = map->GetLocalData()->GetTVEdgeSelection();
	map->GetCurrentCluster(mUCurrentCluster);
	mRecomputeLocalData = recomputeLocalData;
	mUMd = md;
}
	
void RegularMapRestore::Restore(int isUndo)
{
	if (isUndo)
	{
		map->GetData(mRBorderEdges,mRProcessedFaces);
		mRGeomEdge = map->GetLocalData()->GetGeomEdgeSelection();
		mRTVEdge = map->GetLocalData()->GetTVEdgeSelection();
		map->GetCurrentCluster(mRCurrentCluster);
		mRMd = map->GetLocalData();
	}

	if (mRecomputeLocalData)
		map->Init(map->GetMod(), mUMd);

	map->SetData(mUBorderEdges,mUProcessedFaces);

	map->GetLocalData()->SetTVEdgeSelection(mUTVEdge);
	map->GetLocalData()->SetGeomEdgeSelection(mUGeomEdge);
	map->SetCurrentCluster(mUCurrentCluster);

	map->GetMod()->NotifyDependents(FOREVER,TEXMAP_CHANNEL,REFMSG_CHANGE);
	if (map->GetMod()->hView) map->GetMod()->InvalidateView();

}
void RegularMapRestore::Redo()
{

	if (mRecomputeLocalData)
		map->Init(map->GetMod(), mRMd);

	map->SetData(mRBorderEdges,mRProcessedFaces);

	map->GetLocalData()->SetTVEdgeSelection(mRTVEdge);
	map->GetLocalData()->SetGeomEdgeSelection(mRGeomEdge);

	map->SetCurrentCluster(mRCurrentCluster);

	map->GetMod()->NotifyDependents(FOREVER,TEXMAP_CHANNEL,REFMSG_CHANGE);
	if (map->GetMod()->hView) map->GetMod()->InvalidateView();


}

TSTR RegularMapRestore::Description()
{
	return TSTR(GetString(IDS_UNFOLDMAP));
}