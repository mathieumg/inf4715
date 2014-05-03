/*===========================================================================*\
 | 
 |  FILE:	wM3_pick.cpp
 |			Weighted Morpher for MAX R3
 |			Pick mode methods
 | 
 |  AUTH:   Harry Denholm
 |			Copyright(c) Kinetix 1999
 |			All Rights Reserved.
 |
 |  HIST:	Started 27-9-98
 | 
\*===========================================================================*/

#include "wM3.h"


bool GetMorphNode::IsValidMorphTargetType( 
	MorphR3* pMorphModifier, 
	INode* node, 
	TimeValue t )
{
	if (NULL != pMorphModifier && NULL != node && NULL != node->GetObjectRef())
	{
		Interval valid; 
		ObjectState os = node->GetObjectRef()->Eval(t);

		// Target object must be deformable and have the same number of points as the 
		// deformed object
		DbgAssert(pMorphModifier->MC_Local.AreWeCached());
		if (NULL != os.obj && 
			 os.obj->IsDeformable() && 
			 os.obj->NumPoints() == pMorphModifier->MC_Local.Count) 
		{
			// Also, morph target should not cause cycles in morpher's reference graph
			node->BeginDependencyTest();
			pMorphModifier->NotifyDependents(FOREVER, 0, REFMSG_TEST_DEPENDENCY);
			if (FALSE == node->EndDependencyTest()) 
			{
				return true;
			}
		}
	}
	return false;
}

BOOL GetMorphNode::Filter(INode *node)
{
	if (!IsValidMorphTargetType(mp, node, mp->ip->GetTime()))
	{
		return FALSE;
	}

	// check to make sure that the max number of progressive targets will not be exceeded
	//
	morphChannel &bank = mp->CurrentChannel();
	if(bank.mConnection ) {
		if ( bank.mNumProgressiveTargs >= MAX_PROGRESSIVE_TARGETS )
			return FALSE;
	}
	
	return TRUE;
}


BOOL  GetMorphNode::HitTest(
		IObjParam *ip,HWND hWnd,ViewExp * /*vpt*/,IPoint2 m,int flags)
{	
	if (ip->PickNode(hWnd,m,this)) {
		return TRUE;
	}
	return FALSE;
}

BOOL  GetMorphNode::Pick(IObjParam *ip,ViewExp *vpt)
	{
	if ( ! vpt || ! vpt->IsAlive() )
	{
		// why are we here
		DbgAssert(!_T("Invalid viewport!"));
		return FALSE;
	}

	INode *node = vpt->GetClosestHit();
	if (node) {

		node->BeginDependencyTest();
		mp->NotifyDependents(FOREVER,0,REFMSG_TEST_DEPENDENCY);
		if (node->EndDependencyTest()) return FALSE;		

		if( mp->CheckMaterialDependency() ) return FALSE;
		// Make the node reference, and then ask the channel to load itself

		UI_MAKEBUSY

		theHold.Begin();
		theHold.Put(new Restore_FullChannel(mp, mp->chanSel+mp->chanNum));

		morphChannel &bank = mp->CurrentChannel();
		if(bank.mConnection ) {
			int refnum = (mp->CurrentChannelIndex()*MAX_PROGRESSIVE_TARGETS) + 201 + bank.mNumProgressiveTargs;
            mp->ReplaceReference(refnum,node);
			bank.InitTargetCache(bank.mNumProgressiveTargs,node);
			bank.mNumProgressiveTargs++;
			assert(bank.mNumProgressiveTargs<=MAX_PROGRESSIVE_TARGETS);
			bank.ReNormalize();
			mp->Update_channelParams();
		}
		else {
			mp->ReplaceReference(101+mp->CurrentChannelIndex(),node);
			bank.buildFromNode(node, TRUE, 0, TRUE);
			bank.mNumProgressiveTargs=0;
			bank.ReNormalize();
		}
		theHold.Accept(GetString(IDS_MENUNAME));

		mp->DisplayMemoryUsage();

		UI_MAKEFREE
	}
	
	return TRUE;
}


void  GetMorphNode::EnterMode(IObjParam *ip)
{
	// FIX: select the currently active viewport so that
	// the user can use the H shortcut
	SetFocus(mp->ip->GetActiveViewExp().GetHWnd());


	// flag that we are infact picking
	isPicking=TRUE;

	ICustButton *iBut;

	iBut = GetICustButton(GetDlgItem(mp->hwChannelParams,IDC_PICK));
	if (iBut) iBut->SetCheck(TRUE);
	ReleaseICustButton(iBut);

	for( int i=IDC_P1;i<IDC_P10+1;i++){
		HWND button = GetDlgItem(mp->hwChannelList,i);
		iBut = GetICustButton(button);
		if (iBut) 
		{
			iBut->SetHighlightColor(GREEN_WASH);
			InvalidateRect(button, NULL, FALSE);
		}
		ReleaseICustButton(iBut);
	}
}

void  GetMorphNode::ExitMode(IObjParam *ip)
{
	isPicking=FALSE;

	ICustButton *iBut;

	iBut = GetICustButton(GetDlgItem(mp->hwChannelParams,IDC_PICK));
		if (iBut) iBut->SetCheck(FALSE);
	ReleaseICustButton(iBut);

	for( int i=IDC_P1;i<IDC_P10+1;i++){
	HWND button = GetDlgItem(mp->hwChannelList,i);
	iBut = GetICustButton(button);
		if (iBut) 
		{
			InvalidateRect(button, NULL, FALSE);
		}
	ReleaseICustButton(iBut);
	}
}