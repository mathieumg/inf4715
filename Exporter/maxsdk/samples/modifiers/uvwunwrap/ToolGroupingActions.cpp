
/*

Copyright [2010] Autodesk, Inc.  All rights reserved.

Use of this software is subject to the terms of the Autodesk license agreement 
provided at the time of installation or download, or which otherwise accompanies 
this software in either electronic or hard copy form.   
*/






#include "ToolGrouping.h"
#include "unwrap.h"
#include "modsres.h"

void UnwrapMod::GetFaceSelectionClusterIDs(BitArray &ids)
{
	
	ids.SetSize(pblock->Count(unwrap_group_name));
	ids.ClearAll();
	for (int i = 0; i < mMeshTopoData.Count(); i++)
	{
		BitArray fsel = mMeshTopoData[i]->GetFaceSelection();
		for (int j = 0; j < fsel.GetSize(); j++)
		{
			if (fsel[j])
			{
				int clusterID = mMeshTopoData[i]->GetToolGroupingData()->GetGroupID(j);
				if ((clusterID != -1) && (ids[clusterID] == FALSE))
				{
					ids.Set(clusterID);
				}				
			}
		}
	}	

}

void UnwrapMod::fnGroupCreate(const MCHAR *name)
{
	theHold.Begin();
	ClearAFlag(A_HELD);

	HoldSelection();
	mToolGrouping.Hold();
	
	theHold.Accept(GetString(IDS_GROUPSELECTIONL));

	mToolGrouping.CreateGroupFromSelection(name);
}

void UnwrapMod::fnGroupCreateBySelection()
{
	theHold.Begin();
	ClearAFlag(A_HELD);

	HoldSelection();
	mToolGrouping.Hold();

	theHold.Accept(GetString(IDS_GROUPSELECTIONL));

	TSTR emptyName;
	mToolGrouping.CreateGroupFromSelection(emptyName);
}

void UnwrapMod::fnGroupDelete(const MCHAR *name)
{
	theHold.Begin();

	mToolGrouping.Hold();

	int id = -1;
	TSTR cName(name);

	mToolGrouping.DeleteGroup(name);	
	theHold.Accept(GetString(IDS_UNGROUPSELECTIONL));
}

void UnwrapMod::fnGroupDeleteBySelection()
{
	theHold.Begin();

	mToolGrouping.Hold();

	BitArray ids;
	GetFaceSelectionClusterIDs(ids);
	for (int i = 0; i < ids.GetSize(); i++)
	{
		if (ids[i])
			mToolGrouping.DeleteGroup(i);
	}


	theHold.Accept(GetString(IDS_UNGROUPSELECTIONL));
}


void UnwrapMod::fnGroupRename(const MCHAR *name,const MCHAR *newName)
{
	theHold.Begin();

	int id = -1;
	TSTR cName(name);
	TSTR nName(newName);
	mToolGrouping.FindGroupID(cName, id);
	mToolGrouping.SetGroupName(id,nName);

	theHold.Accept(GetString(IDS_GROUPSELECTIONL));

}
void UnwrapMod::fnGroupSelect(const MCHAR *name)
{
	theHold.Begin();
	ClearAFlag(A_HELD);
	HoldSelection();
	theHold.Accept(GetString(IDS_SELECTGROUPL));

	TSTR cName(name);
	mToolGrouping.SelectGroup(cName);

	UpdateViewAndModifier();
}

void UnwrapMod::fnGroupSelectBySelection()
{
	theHold.Begin();
	ClearAFlag(A_HELD);
	HoldSelection();
	theHold.Accept(GetString(IDS_PW_SELECT_UVW));

	BitArray ids;
	GetFaceSelectionClusterIDs(ids);
	for (int i = 0; i < ids.GetSize(); i++)
	{
		if (ids[i])
			mToolGrouping.SelectGroup(i);
	}

	UpdateViewAndModifier();
}

void UnwrapMod::fnGroupSetTexelDensity(float val)
{
	theHold.Begin();


	BitArray ids;
	GetFaceSelectionClusterIDs(ids);
	for (int i = 0; i < ids.GetSize(); i++)
	{
		if (ids[i])
			mToolGrouping.SetGroupTexelDensity(i,val);
	}

	theHold.Accept(GetString(IDS_GROUPDENSITY));
}

float UnwrapMod::fnGroupGetTexelDensity()
{


	BitArray ids;
	float v = -1.0f;
	GetFaceSelectionClusterIDs(ids);
	for (int i = 0; i < ids.GetSize(); i++)
	{
		if (ids[i])
		{
			if (v == -1.0f)
				v = mToolGrouping.GetGroupTexelDensity(i);
			else 
			{
				if (v != mToolGrouping.GetGroupTexelDensity(i))
				{
					v = -1.0f;
					return v;
				}
			}
		}
	}

	return v;
}

void UnwrapMod::UpdateGroupUI()
{
    float currentDensity = -1;
	BOOL indet = FALSE;
	int groupCt = 0;
	BitArray hitGroups;
	hitGroups.SetSize(pblock->Count(unwrap_group_name));
	hitGroups.ClearAll();
	for (int i = 0; i < mMeshTopoData.Count(); i++)
	{
		MeshTopoData* ld = mMeshTopoData[i];
		BitArray fsel = ld->GetFaceSelection();
		ToolGroupingData* groupData =  ld->GetToolGroupingData();
		for (int j = 0; j < ld->GetNumberFaces(); j++)
		{
			if (fsel[j])
			{
				int id = groupData->GetGroupID(j);
				if (id >= 0)
				{
					hitGroups.Set(id,TRUE);
					groupCt++;
					float testDensity = mToolGrouping.GetGroupTexelDensity(id);
					if (currentDensity == -1)
					{
						currentDensity = testDensity;
					}
					else if (testDensity != currentDensity)
					{
						indet = TRUE;
						j = ld->GetNumberFaces(); 
						i = mMeshTopoData.Count();
					}
				}
			}
		}
	}

	if (indet || (groupCt == 0))
	{
		mUIManager.SetIndeterminate(ID_GROUPSETDENSITY_SPINNER,TRUE);
	}
	else
	{
		mUIManager.SetIndeterminate(ID_GROUPSETDENSITY_SPINNER,FALSE);
		mUIManager.SetSpinFValue(ID_GROUPSETDENSITY_SPINNER,currentDensity);
	}

	if (hitGroups.NumberSet() == 0)
	{
		TSTR str;
		str.printf(_T("%s"),GetString(IDS_NOGROUP_SELECTED));
		HWND rollupHWND = mSideBarUI.GetRollupHWND(6);
		SetWindowText(GetDlgItem(rollupHWND,IDC_GROUP_TEXT),str);
	}
	else
	{
		TSTR str;
		str.printf(_T("%s "),GetString(IDS_GROUP_SELECTED));
		for (int i = 0; i < hitGroups.GetSize(); i++)
		{
			if (hitGroups[i])
			{
				TSTR idStr;
				idStr.printf(_T("%d "),i);
				str += idStr;
			}
		}
		HWND rollupHWND = mSideBarUI.GetRollupHWND(6);
		SetWindowText(GetDlgItem(rollupHWND,IDC_GROUP_TEXT),str);
	}
}