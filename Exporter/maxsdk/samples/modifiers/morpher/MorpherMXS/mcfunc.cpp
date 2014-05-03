/*===========================================================================*\
 | 
 |  FILE:	MCFunc.cpp
 |			Implimentations of the moprhChannel functions of MorpherMXS
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 5-4-99
 | 
\*===========================================================================*/

#include "MorpherMXS.h"
#include "..\..\morpher\include\MorpherApi.h"

/*===========================================================================*\
 |	This swaps the position of 2 morphs in the channel list
 |  
\*===========================================================================*/
Value*
wm3_SwapMorph_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_SwapMorph_cf, 3, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_SwapMorph [Morpher Modifier] [Morph Index] [MorphIndex]"));
	type_check(arg_list[1], Integer, _T("WM3_SwapMorph [Morpher Modifier] [Morph Index] [MorphIndex]"));
	type_check(arg_list[2], Integer, _T("WM3_SwapMorph [Morpher Modifier] [Morph Index] [MorphIndex]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;


	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();

	int morphIndexA = arg_list[1]->to_int(); 
	morphIndexA--;
	if(morphIndexA>=100) morphIndexA = 99;
	if(morphIndexA<0) morphIndexA = 0;

	int morphIndexB = arg_list[2]->to_int(); 
	morphIndexB--;
	if(morphIndexB>=100) morphIndexB = 99;
	if(morphIndexB<0) morphIndexB = 0;
	
	MaxMorphModifier maxMorpher(mp);
	bool res = maxMorpher.SwapMorphChannels(morphIndexA, morphIndexB);
	maxMorpher.RefreshChannelsUI();
	
	return (res ? &true_value : &false_value);
}

/*===========================================================================*\
 |	This swaps the position of 2 morphs in the channel list
 |  
\*===========================================================================*/
Value*
wm3_MoveMorph_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_MoveMorph_cf, 3, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_MoveMorph [Morpher Modifier] [Morph Source Index] [Morph Destination Index]"));
	type_check(arg_list[1], Integer, _T("WM3_MoveMorph [Morpher Modifier] [Morph Source Index] [Morph Destination Index]"));
	type_check(arg_list[2], Integer, _T("WM3_MoveMorph [Morpher Modifier] [Morph Source Index] [Morph Destination Index]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;

	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();

	int morphIndexA = arg_list[2]->to_int(); 
	morphIndexA--;
	if(morphIndexA>=100) morphIndexA = 99;
	if(morphIndexA<0) morphIndexA = 0;

	int morphIndexB = arg_list[1]->to_int(); 
	morphIndexB--;
	if(morphIndexB>=100) morphIndexB = 99;
	if(morphIndexB<0) morphIndexB = 0;
	
	MaxMorphModifier maxMorpher(mp);
	bool res = maxMorpher.MoveMorphChannel(morphIndexA, morphIndexB);
	maxMorpher.RefreshChannelsUI();

	return (res ? &true_value : &false_value);
}

/*===========================================================================*\
 |	Returns the the  progressive morph tension
 |  
\*===========================================================================*/
Value*
wm3_GetProgressiveMorphTension_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_GetProgressiveMorphTension_cf, 2, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_GetProgressiveMorphTension [Morpher Modifier] [Morph Index]"));
	type_check(arg_list[1], Integer, _T("WM3_GetProgressiveMorphTension [Morpher Modifier] [Morph Index]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;


	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();

	int morphIndex = arg_list[1]->to_int(); 
	morphIndex--;
	if(morphIndex>=100) morphIndex = 99;
	if(morphIndex<0) morphIndex = 0;

	MaxMorphModifier maxMorpher(mp);
	float tension = maxMorpher.GetMorphChannel(morphIndex).GetProgressiveMorphTension();
	return Float::intern(tension);	
}


/*===========================================================================*\
 |	Sets the the  progressive morph tension
 |  
\*===========================================================================*/
Value*
wm3_SetProgressiveMorphTension_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_SetProgressiveMorphTension_cf, 3, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_SetProgressiveMorphTension [Morpher Modifier] [Morph Index] [Tension]"));
	type_check(arg_list[1], Integer, _T("WM3_SetProgressiveMorphTension [Morpher Modifier] [Morph Index] [Tension]"));
	type_check(arg_list[2], Float, _T("WM3_SetProgressiveMorphTension [Morpher Modifier] [Morph Index] [Tension]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;


	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();

	int morphIndex = arg_list[1]->to_int(); 
	morphIndex--;
	if(morphIndex>=100) morphIndex = 99;
	if(morphIndex<0) morphIndex = 0;

	float tension = arg_list[2]->to_float(); 

	INode *node = NULL;
	MaxMorphModifier maxMorpher(mp);
	bool res = maxMorpher.GetMorphChannel(morphIndex).SetProgressiveMorphTension(tension);
	mp->Update_channelParams();

	return (res ? &true_value : &false_value);
}

/*===========================================================================*\
 |	Returns the the  progressive morph Node
 |  
\*===========================================================================*/
Value*
wm3_GetProgressiveMorphNode_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_GetProgressiveMorphNode_cf, 3, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_GetProgressiveMorphNode [Morpher Modifier] [Morph Index] [Progressive Morph Index]"));
	type_check(arg_list[1], Integer, _T("WM3_GetProgressiveMorphNode [Morpher Modifier] [Morph Index]  [Progressive Morph Index]"));
	type_check(arg_list[2], Integer, _T("WM3_GetProgressiveMorphNode [Morpher Modifier] [Morph Index]  [Progressive Morph Index]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;


	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();

	int morphIndex = arg_list[1]->to_int(); 
	morphIndex--;
	if(morphIndex>=100) morphIndex = 99;
	if(morphIndex<0) morphIndex = 0;

	int pMorphIndex = arg_list[2]->to_int(); 
	pMorphIndex--;

	MaxMorphModifier maxMorpher(mp);
	INode *node = maxMorpher.GetMorphChannel(morphIndex).GetProgressiveMorphTarget(pMorphIndex);
	if (node != NULL)
	{
		return MAXNode::intern(node);	
	}
	return &undefined;
}

/*===========================================================================*\
 |	Adds a progressives Morph
 |  
\*===========================================================================*/
Value*
wm3_AddProgressiveMorphNode_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_AddProgressiveMorphNode, 3, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_AddProgressiveMorphNode [Morpher Modifier] [Morph Index] [Node]"));
	type_check(arg_list[1], Integer, _T("WM3_AddProgressiveMorphNode [Morpher Modifier] [Morph Index]  [Node]"));	
	type_check(arg_list[2], MAXNode, _T("WM3_AddProgressiveMorphNode [Morpher Modifier] [Morph Index]  [Node]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;


	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();

	int morphIndex = arg_list[1]->to_int(); 
	morphIndex--;
	if(morphIndex>=100) morphIndex = 99;
	if(morphIndex<0) morphIndex = 0;

	INode *node = NULL;	
	node = arg_list[2]->to_node(); 
	if (node == NULL) return &ok;
	
	MaxMorphModifier maxMorpher(mp);
	bool res = maxMorpher.GetMorphChannel(morphIndex).AddProgressiveMorphTarget(node);
	return (res ? &true_value : &false_value);
}

/*===========================================================================*\
 |	Deletes a progressives Morph
 |  
\*===========================================================================*/
Value*
wm3_DeleteProgressiveMorphNode_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_DeleteProgressiveMorphNode, 3, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_DeleteProgressiveMorphNode [Morpher Modifier] [Morph Index] [Progressive Morph Index]"));
	type_check(arg_list[1], Integer, _T("WM3_DeleteProgressiveMorphNode [Morpher Modifier] [Morph Index] [Progressive Morph Index]"))	
	type_check(arg_list[2], Integer, _T("WM3_DeleteProgressiveMorphNode [Morpher Modifier] [Morph Index] [Progressive Morph Index]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;


	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();

	int morphIndex = arg_list[1]->to_int(); 
	morphIndex--;
	if(morphIndex>=100) morphIndex = 99;
	if(morphIndex<0) morphIndex = 0;

	int pMorphIndex = arg_list[2]->to_int(); 
	pMorphIndex--;

	MaxMorphModifier maxMorpher(mp);
	bool res = maxMorpher.GetMorphChannel(morphIndex).DeleteProgressiveMorphTarget(pMorphIndex);

	return (res ? &true_value : &false_value);
}


/*===========================================================================*\
 |	Returns the the  progressive morph weight
 |  
\*===========================================================================*/
Value*
wm3_GetProgressiveMorphWeight_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_GetProgressiveMorphWeight_cf, 3, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_GetProgressiveMorphWeight [Morpher Modifier] [Morph Index] [Progressive Node]"));
	type_check(arg_list[1], Integer, _T("WM3_GetProgressiveMorphWeight [Morpher Modifier] [Morph Index]  [Progressive Node]"));
	type_check(arg_list[2], MAXNode, _T("WM3_GetProgressiveMorphWeight [Morpher Modifier] [Morph Index]  [Progressive Node]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;


	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();

	int morphIndex = arg_list[1]->to_int(); 
	morphIndex--;
	if(morphIndex>=100) morphIndex = 99;
	if(morphIndex<0) morphIndex = 0;

	INode *node = NULL;	
	node = arg_list[2]->to_node(); 

	float weight = 0.0f;

	if (node == NULL) return Float::intern(weight);	
	
	MaxMorphModifier maxMorpher(mp);
	int pMorphCount = maxMorpher.GetMorphChannel(morphIndex).NumMaxProgressiveTargets();		
		
	int pMorphIndex = -1;
	if (node == (INode*)mp->GetReference(101+morphIndex))
	{
		pMorphIndex = 0;
	}
	else
	{
		for (int i = 1; i < (pMorphCount); i++)
		{
			int refnum = 200+i+morphIndex*MAX_PROGRESSIVE_TARGETS;
			if (node == (INode*)mp->GetReference(refnum))
			{
					pMorphIndex = i ;
					break;
			}
		}
	}
	weight = maxMorpher.GetMorphChannel(morphIndex).GetProgressiveMorphWeight(pMorphIndex);
	return Float::intern(weight);	
}


/*===========================================================================*\
 |	Returns the the  progressive morph weight
 |  
\*===========================================================================*/
Value*
wm3_SetProgressiveMorphWeight_cf(Value** arg_list, int count)
{
	check_arg_count(WM3_SetProgressiveMorphWeight_cf, 4, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_SetProgressiveMorphWeight [Morpher Modifier] [Morph Index] [Progressive Node] [Weight]"));
	type_check(arg_list[1], Integer, _T("WM3_SetProgressiveMorphWeight [Morpher Modifier] [Morph Index]  [Progressive Node] [Weight]"));
	type_check(arg_list[2], MAXNode, _T("WM3_SetProgressiveMorphWeight [Morpher Modifier] [Morph Index]  [Progressive Node] [Weight]"));
	type_check(arg_list[3], Float, _T("WM3_SetProgressiveMorphWeight [Morpher Modifier] [Morph Index]  [Progressive Node] [Weight]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;


	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();

	int morphIndex = arg_list[1]->to_int(); 
	morphIndex--;
	if(morphIndex>=100) morphIndex = 99;
	if(morphIndex<0) morphIndex = 0;

	INode *node = NULL;	
	node = arg_list[2]->to_node(); 

	float weight = arg_list[3]->to_float(); 

	if (node == NULL) return &false_value;
	
	MaxMorphModifier maxMorpher(mp);
	int pMorphCount = maxMorpher.GetMorphChannel(morphIndex).NumProgressiveMorphTargets();	

	int pMorphIndex = -1;
	if (node == (INode*)mp->GetReference(101+morphIndex))
	{
		pMorphIndex = 0;
	}
	else
	{
		for (int i = 1; i < (pMorphCount); i++)
		{
			int refnum = 200+i+morphIndex*MAX_PROGRESSIVE_TARGETS;
			if (node == (INode*)mp->GetReference(refnum))
			{
				pMorphIndex = i ;
				break;
			}
		}
	}

	bool res = maxMorpher.GetMorphChannel(morphIndex).SetProgressiveMorphWeight(pMorphIndex, weight);
	if (res)	
	{
		mp->Update_channelFULL();
		mp->Update_channelParams();
	}
	return (res ? &true_value : &false_value);
}


/*===========================================================================*\
 |	Returns the number of progressive morphs for a particular morph
 |  this includes the initial morph
\*===========================================================================*/
Value*
wm3_NumberOfProgressiveMorphs_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_NumberOfProgressiveMorphs_cf, 2, count);
	type_check(arg_list[0], MAXModifier, _T("wm3_NumberOfProgressiveMorphs [Morpher Modifier] [Morph Index]"));
	type_check(arg_list[1], Integer, _T("wm3_NumberOfProgressiveMorphs [Morpher Modifier] [Morph Index]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;


	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();

	int morphIndex = arg_list[1]->to_int(); 
	morphIndex--;
	if(morphIndex>=100) morphIndex = 99;
	if(morphIndex<0) morphIndex = 0;

	MaxMorphModifier maxMorpher(mp);
	return Integer::intern(maxMorpher.GetMorphChannel(morphIndex).NumProgressiveMorphTargets());
	
}

/*===========================================================================*\
 |	Returns the number of channels
\*===========================================================================*/
Value*
wm3_NumberOfChannels_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_NumberOfChannels_cf, 1, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_NumberOfChannels [Morpher Modifier]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;


	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();
	MaxMorphModifier maxMorpher(mp);
	int ct = maxMorpher.NumMorphChannels();

	return Integer::intern(ct);
}

/*===========================================================================*\
 |	Returns the number of markers for the modifier
\*===========================================================================*/
Value*
wm3_NumberOfMarkers_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_NumberOfMarkers_cf, 1, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_NumberOfMarkers [Morpher Modifier]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;


	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();

	int ct = mp->markerName.Count();

	return Integer::intern(ct);
}


/*===========================================================================*\
 |	Returns the current active marker.  This returns -1 if none are active
\*===========================================================================*/
Value*
wm3_GetCurrentMarker_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_GetCurrentMarker_cf, 1, count);
	type_check(arg_list[0], MAXModifier, _T("wm3_GetCurrentMarker [Morpher Modifier]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;


	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();

	int index = mp->markerSel;

	if (index >= 0) index++;

	return Integer::intern(index);
}



/*===========================================================================*\
 |	Lets you set the active marker
\*===========================================================================*/

Value*
wm3_SetCurrentMarker_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_SetCurrentMarker_cf, 2, count);
	type_check(arg_list[0], MAXModifier, _T("wm3_SetCurrentMarker [Morpher Modifier] [Marker Index]"));
	type_check(arg_list[1], Integer, _T("wm3_SetCurrentMarker [Morpher Modifier] [Marker Index]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;


	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();

	int sel = arg_list[1]->to_int(); 
	sel -= 1;

	if ((sel >= 0) && (sel < mp->markerName.Count()))
	{
#pragma warning (push)
#pragma warning (disable: 4996)
		IMorphClass imp;
		imp.HoldMarkers(mp);
#pragma warning (pop)
		mp->chanSel = 0;
		mp->chanNum = (mp->markerIndex[sel]);
		mp->markerSel = sel;
		mp->Update_channelFULL();
		mp->Update_channelParams();
		return &true_value;
	}

	return &false_value;
}

/*===========================================================================*\
 |	Lets you create a marker marker
\*===========================================================================*/

Value*
wm3_CreateMarker_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_CreateMarker_cf, 3, count);
	type_check(arg_list[0], MAXModifier, _T("wm3_CreateMarker [Morpher Modifier] [Morph Index] [Marker Name]"));
	type_check(arg_list[1], Integer, _T("wm3_CreateMarker [Morpher Modifier] [Morph Index] [Marker Name]"));
	type_check(arg_list[2], String, _T("wm3_CreateMarker [Morpher Modifier] [Morph Index] [Marker Name]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;


	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();

	int sel = arg_list[1]->to_int(); 
	sel -= 1;

	const TCHAR *name = arg_list[2]->to_string();

	if (sel >= 0) 
	{
#pragma warning (push)
#pragma warning (disable: 4996)
		IMorphClass imp;
		imp.HoldMarkers(mp);
#pragma warning (pop)
		mp->markerName.AddName(name);
		mp->markerIndex.Append(1,&sel,0);
		mp->Update_channelMarkers();
		return &true_value;
	}

	return &false_value;
}


/*===========================================================================*\
 |	Lets you delete a marker
\*===========================================================================*/

Value*
wm3_DeleteMarker_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_DeleteMarker_cf, 2, count);
	type_check(arg_list[0], MAXModifier, _T("wm3_DeletMarker [Morpher Modifier] [Morph Index]"));
	type_check(arg_list[1], Integer, _T("wm3_DeletMarker [Morpher Modifier] [Morph Index]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;


	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();

	int sel = arg_list[1]->to_int(); 
	sel -= 1;



	if ( (sel >= 0) && (sel < mp->markerName.Count()))
	{
#pragma warning (push)
#pragma warning (disable: 4996)
		IMorphClass imp;
		imp.HoldMarkers(mp);
#pragma warning (pop)
		mp->markerName.Delete(sel,1);
		mp->markerIndex.Delete(sel,1);
		mp->Update_channelMarkers();
		return &true_value;
	}

	return &false_value;
}

/*===========================================================================*\
 |	Returns the morph index of a marker
\*===========================================================================*/
Value*
wm3_GetMarkerIndex_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_GetMarkerIndex_cf, 2, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_GetMarkerIndex [Morpher Modifier] [Marker Index]"));
	type_check(arg_list[1], Integer, _T("WM3_GetMarkerIndex [Morpher Modifier] [Marker Index]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;


	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();

	int index = arg_list[1]->to_int(); 
	index--;

	if ((index < 0) || (index >= mp->markerName.Count()))
		return &false_value;

	int id = mp->markerIndex[index];
	id++;

	return Integer::intern(id);
}


/*===========================================================================*\
 |	Returns the markers name
\*===========================================================================*/

Value*
wm3_GetMarkerName_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_GetMarkerName_cf, 2, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_GetMarkerName [Morpher Modifier] [Marker Index]"));
	type_check(arg_list[1], Integer, _T("WM3_GetMarkerName [Morpher Modifier] [Marker Index]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;


	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();

	int index = arg_list[1]->to_int(); 
	index--;

	if ((index < 0) || (index >= mp->markerName.Count()))
		return &false_value;


	return new String(mp->markerName[index]);
}

/*===========================================================================*\
 |	Given the <target object> from the scene, this function initializes the 
 |  channel with all necessary data
\*===========================================================================*/

Value*
wm3_mc_bfn_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_mc_bfn, 3, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_MC_BuildFromNode [Morpher Modifier] [Channel Index] [Target]"));
	type_check(arg_list[1], Integer, _T("WM3_MC_BuildFromNode [Morpher Modifier] [Channel Index] [Target]"));
	type_check(arg_list[2], MAXNode, _T("WM3_MC_BuildFromNode [Morpher Modifier] [Channel Index] [Target]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;

	int sel = arg_list[1]->to_int(); sel -= 1;
	if(sel>=100) sel = 99;
	if(sel<0) sel = 0;

	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();

	INode *node = arg_list[2]->to_node();
	mp->ReplaceReference(101+sel,node);
	mp->chanBank[sel].buildFromNode( node );

	return &true_value;
}

/*===========================================================================*\
 |	Lets you edit a marker 
\*===========================================================================*/

Value*
wm3_SetMarkerData_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_CreateMarker_cf, 4, count);
	type_check(arg_list[0], MAXModifier, _T("wm3_SetMarkerData [Morpher Modifier] [Marker Index] [Morph Index] [Marker Name]"));
	type_check(arg_list[1], Integer, _T("wm3_SetMarkerData [Morpher Modifier] [Marker Index]  [Morph Index] [Marker Name]"));
	type_check(arg_list[2], Integer, _T("wm3_SetMarkerData [Morpher Modifier] [Marker Index]  [Morph Index] [Marker Name]"));
	type_check(arg_list[3], String, _T("wm3_SetMarkerData [Morpher Modifier] [Marker Index]  [Morph Index] [Marker Name]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;


	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();

	int markerIndex = arg_list[1]->to_int(); 
	markerIndex -= 1;

	int morphIndex = arg_list[2]->to_int(); 
	morphIndex -= 1;

	const TCHAR *name = arg_list[3]->to_string();

	if ((markerIndex >= 0) && (markerIndex < mp->markerIndex.Count()))
	{
		mp->markerName.SetName(markerIndex,name);
		mp->markerIndex[markerIndex] = morphIndex;
		
		mp->Update_channelMarkers();
		mp->Update_channelFULL();
		mp->Update_channelParams();
		return &true_value;
	}

	return &false_value;
}


/*===========================================================================*\
 |	Rebuilds optimization and morph data in this channel
 |  Use this after changing the channel's target
\*===========================================================================*/

Value*
wm3_mc_rebuild_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_mc_rebuild, 2, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_MC_Rebuild [Morpher Modifier] [Channel Index]"));
	type_check(arg_list[1], Integer, _T("WM3_MC_Rebuild [Morpher Modifier] [Channel Index]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;

	int sel = arg_list[1]->to_int(); sel -= 1;
	if(sel>=100) sel = 99;
	if(sel<0) sel = 0;

	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();
	mp->chanBank[sel].rebuildChannel();

	return &true_value;
}


/*===========================================================================*\
 |	Deletes the channel
\*===========================================================================*/

Value*
wm3_mc_delete_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_mc_delete, 2, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_MC_Delete [Morpher Modifier] [Channel Index]"));
	type_check(arg_list[1], Integer, _T("WM3_MC_Delete [Morpher Modifier] [Channel Index]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;

	int sel = arg_list[1]->to_int(); sel -= 1;
	if(sel>=100) sel = 99;
	if(sel<0) sel = 0;

	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();
	MaxMorphModifier maxMorpher(mp);
	bool res = maxMorpher.GetMorphChannel(sel).DeleteChannel();
	maxMorpher.RefreshChannelsUI();
	needs_redraw_set();

	return (res ? &true_value : &false_value);
}


/*===========================================================================*\
 |	Retrieves the name of the morpher channel
\*===========================================================================*/

Value*
wm3_mc_getname_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_mc_getname, 2, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_MC_GetName [Morpher Modifier] [Channel Index]"));
	type_check(arg_list[1], Integer, _T("WM3_MC_GetName [Morpher Modifier] [Channel Index]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;

	int sel = arg_list[1]->to_int(); sel -= 1;
	if(sel>=100) sel = 99;
	if(sel<0) sel = 0;

	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();
	MaxMorphModifier maxMorpher(mp);
	return new String(maxMorpher.GetMorphChannel(sel).GetName());
}


/*===========================================================================*\
 |	Sets the name of the channel to be <string name>
\*===========================================================================*/

Value*
wm3_mc_setname_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_mc_setname, 3, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_MC_SetName [Morpher Modifier] [Channel Index] [Name String]"));
	type_check(arg_list[1], Integer, _T("WM3_MC_SetName [Morpher Modifier] [Channel Index] [Name String]"));
	type_check(arg_list[2], String, _T("WM3_MC_SetName [Morpher Modifier] [Channel Index] [Name String]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;

	int sel = arg_list[1]->to_int(); sel -= 1;
	if(sel>=100) sel = 99;
	if(sel<0) sel = 0;

	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();
	MaxMorphModifier maxMorpher(mp);
	maxMorpher.GetMorphChannel(sel).SetName(arg_list[2]->to_string());
	return &true_value;
}


/*===========================================================================*\
 |	Returns the weighting value of the channel
\*===========================================================================*/

Value*
wm3_mc_getamount_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_mc_getamount, 2, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_MC_GetValue [Morpher Modifier] [Channel Index]"));
	type_check(arg_list[1], Integer, _T("WM3_MC_GetValue [Morpher Modifier] [Channel Index]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;

	int sel = arg_list[1]->to_int(); sel -= 1;
	if(sel>=100) sel = 99;
	if(sel<0) sel = 0;

	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();
	MaxMorphModifier maxMorpher(mp);
	// The value of the channel - ie, its weighted percentage
	float tmpVal = maxMorpher.GetMorphChannel(sel).GetMorphWeight(MAXScript_time());

	return Float::intern(tmpVal);
}


/*===========================================================================*\
 |	Sets the weighted value of the channel
\*===========================================================================*/

Value*
wm3_mc_setamount_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_mc_setamount, 3, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_MC_SetValue [Morpher Modifier] [Channel Index] [Value]"));
	type_check(arg_list[1], Integer, _T("WM3_MC_SetValue [Morpher Modifier] [Channel Index] [Value]"));
	type_check(arg_list[2], Float, _T("WM3_MC_SetValue [Morpher Modifier] [Channel Index] [Value]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;

	int sel = arg_list[1]->to_int(); sel -= 1;
	if(sel>=100) sel = 99;
	if(sel<0) sel = 0;

	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();
	MaxMorphModifier maxMorpher(mp);
	bool res = maxMorpher.GetMorphChannel(sel).SetMorphWeight(MAXScript_time(), arg_list[2]->to_float());
	needs_redraw_set();

	return (res ? &true_value : &false_value);
}


/*===========================================================================*\
 |	Returns TRUE if the channel has an active connection to a scene object
\*===========================================================================*/

Value*
wm3_mc_hastarget_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_mc_hastarget, 2, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_MC_HasTarget [Morpher Modifier] [Channel Index]"));
	type_check(arg_list[1], Integer, _T("WM3_MC_HasTarget [Morpher Modifier] [Channel Index]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;

	int sel = arg_list[1]->to_int(); sel -= 1;
	if(sel>=100) sel = 99;
	if(sel<0) sel = 0;

	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();
	MaxMorphModifier maxMorpher(mp);

	if(maxMorpher.GetMorphChannel(sel).GetMorphTarget() != NULL) return &true_value;
	else return &false_value;
}


/*===========================================================================*\
 |	Returns a pointer to the object in the scene the channel is connected to
\*===========================================================================*/

Value*
wm3_mc_gettarget_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_mc_gettarget, 2, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_MC_GetTarget [Morpher Modifier] [Channel Index]"));
	type_check(arg_list[1], Integer, _T("WM3_MC_GetTarget [Morpher Modifier] [Channel Index]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;

	int sel = arg_list[1]->to_int(); sel -= 1;
	if(sel>=100) sel = 99;
	if(sel<0) sel = 0;

	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();
	MaxMorphModifier maxMorpher(mp);

	INode* target = maxMorpher.GetMorphChannel(sel).GetMorphTarget();
	if (target != NULL) 
	{
		return MAXNode::intern(target);
	}
	return &undefined;
}

/*===========================================================================*\
 |	Sets a channel's target object if the channel has no target
\*===========================================================================*/

Value*
wm3_mc_settarget_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_mc_settarget, 3, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_MC_SetTarget [Morpher Modifier] [Channel Index] [Node]"));
	type_check(arg_list[1], Integer, _T("WM3_MC_SetTarget [Morpher Modifier] [Channel Index] [Node]"));
	type_check(arg_list[2], MAXNode, _T("WM3_MC_SetTarget [Morpher Modifier] [Channel Index] [Node]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;

	int sel = arg_list[1]->to_int(); sel -= 1;
	if(sel>=100) sel = 99;
	if(sel<0) sel = 0;

	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();

	INode *node = NULL;	
	node = arg_list[2]->to_node(); 
	if (node == NULL) return &false_value;

	MaxMorphModifier maxMorpher(mp);
	MaxMorphChannel maxMorphChannel = maxMorpher.GetMorphChannel(sel);
	if (NULL != maxMorphChannel.GetMorphTarget()) return &false_value;
	
	if (maxMorphChannel.SetMorphTarget(node, MAXScript_time()))
	{
		return &true_value;
	}
	return &false_value;
}

/*===========================================================================*\
 |	Returns TRUE if the channel has not been marked as an invalid channel
\*===========================================================================*/

Value*
wm3_mc_isvalid_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_mc_isvalid, 2, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_MC_IsValid [Morpher Modifier] [Channel Index]"));
	type_check(arg_list[1], Integer, _T("WM3_MC_IsValid [Morpher Modifier] [Channel Index]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;

	int sel = arg_list[1]->to_int(); sel -= 1;
	if(sel>=100) sel = 99;
	if(sel<0) sel = 0;

	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();
	MaxMorphModifier maxMorpher(mp);

	if(maxMorpher.GetMorphChannel(sel).IsValid()) return &true_value;
	else return &false_value;
}


/*===========================================================================*\
 |	Returns TRUE if the channel has some morpher data in it (Indicator: BLUE)
\*===========================================================================*/

Value*
wm3_mc_hasdata_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_mc_hasdata, 2, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_MC_HasData [Morpher Modifier] [Channel Index]"));
	type_check(arg_list[1], Integer, _T("WM3_MC_HasData [Morpher Modifier] [Channel Index]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;

	int sel = arg_list[1]->to_int(); sel -= 1;
	if(sel>=100) sel = 99;
	if(sel<0) sel = 0;

	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();
	MaxMorphModifier maxMorpher(mp);

	if (maxMorpher.GetMorphChannel(sel).HasData()) return &true_value;
	else return &false_value;
}


/*===========================================================================*\
 |	Returns TRUE if the channel is turned on and used in the Morph
\*===========================================================================*/

Value*
wm3_mc_isactive_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_mc_isactive, 2, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_MC_IsActive [Morpher Modifier] [Channel Index]"));
	type_check(arg_list[1], Integer, _T("WM3_MC_IsActive [Morpher Modifier] [Channel Index]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;

	int sel = arg_list[1]->to_int(); sel -= 1;
	if(sel>=100) sel = 99;
	if(sel<0) sel = 0;

	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();

	MaxMorphModifier maxMorpher(mp);
	if (maxMorpher.GetMorphChannel(sel).IsActive()) return &true_value;
	return &false_value;
}



/*===========================================================================*\
 |	Sets whether or not the channel is used in the morph results or not
\*===========================================================================*/

Value*
wm3_mc_setactive_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_mc_setactive, 3, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_MC_SetActive [Morpher Modifier] [Channel Index] [true/false]"));
	type_check(arg_list[1], Integer, _T("WM3_MC_SetActive [Morpher Modifier] [Channel Index] [true/false]"));
	type_check(arg_list[2], Boolean, _T("WM3_MC_SetActive [Morpher Modifier] [Channel Index] [true/false]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;

	int sel = arg_list[1]->to_int(); sel -= 1;
	if(sel>=100) sel = 99;
	if(sel<0) sel = 0;

	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();

	MaxMorphModifier maxMorpher(mp);
	maxMorpher.GetMorphChannel(sel).SetActive((TRUE == arg_list[2]->to_bool()));

	needs_redraw_set();

	return &true_value;
}



/*===========================================================================*\
 |	Returns TRUE if the 'Use Limits' checkbox is on
\*===========================================================================*/

Value*
wm3_mc_getuselimits_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_mc_getuselimits, 2, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_MC_GetUseLimits [Morpher Modifier] [Channel Index]"));
	type_check(arg_list[1], Integer, _T("WM3_MC_GetUseLimits [Morpher Modifier] [Channel Index]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;

	int sel = arg_list[1]->to_int(); sel -= 1;
	if(sel>=100) sel = 99;
	if(sel<0) sel = 0;

	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();

	if(mp->chanBank[sel].mUseLimit) return &true_value;
	return &false_value;
}


/*===========================================================================*\
 |	Turns on and off the 'Use Limits' checkbox
\*===========================================================================*/

Value*
wm3_mc_setuselimits_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_mc_setuselimits, 3, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_MC_SetUseLimits [Morpher Modifier] [Channel Index] [true/false]"));
	type_check(arg_list[1], Integer, _T("WM3_MC_SetUseLimits [Morpher Modifier] [Channel Index] [true/false]"));
	type_check(arg_list[2], Boolean, _T("WM3_MC_SetUseLimits [Morpher Modifier] [Channel Index] [true/false]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;

	int sel = arg_list[1]->to_int(); sel -= 1;
	if(sel>=100) sel = 99;
	if(sel<0) sel = 0;

	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();

	mp->chanBank[sel].mUseLimit = arg_list[2]->to_bool();
	mp->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);

	needs_redraw_set();

	return &true_value;
}


/*===========================================================================*\
 |	Returns the upper limit for the channel values (only used if 'Use Limits' is on)
\*===========================================================================*/

Value*
wm3_mc_getlimitmax_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_mc_getlimitmax, 2, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_MC_GetLimitMAX [Morpher Modifier] [Channel Index]"));
	type_check(arg_list[1], Integer, _T("WM3_MC_GetLimitMAX [Morpher Modifier] [Channel Index]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;

	int sel = arg_list[1]->to_int(); sel -= 1;
	if(sel>=100) sel = 99;
	if(sel<0) sel = 0;

	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();

	return Float::intern(mp->chanBank[sel].mSpinmax);
}


/*===========================================================================*\
 |	Returns the lower limit for the channel values (only used if 'Use Limits' is on)
\*===========================================================================*/

Value*
wm3_mc_getlimitmin_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_mc_getlimitmin, 2, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_MC_GetLimitMIN [Morpher Modifier] [Channel Index]"));
	type_check(arg_list[1], Integer, _T("WM3_MC_GetLimitMIN [Morpher Modifier] [Channel Index]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;

	int sel = arg_list[1]->to_int(); sel -= 1;
	if(sel>=100) sel = 99;
	if(sel<0) sel = 0;

	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();

	return Float::intern(mp->chanBank[sel].mSpinmin);
}


/*===========================================================================*\
 |	Sets the high limit for the channel's value
\*===========================================================================*/

Value*
wm3_mc_setlimitmax_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_mc_setlimitmax, 3, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_MC_SetLimitMAX [Morpher Modifier] [Channel Index] [Float Value]"));
	type_check(arg_list[1], Integer, _T("WM3_MC_SetLimitMAX [Morpher Modifier] [Channel Index] [Float Value]"));
	type_check(arg_list[2], Float, _T("WM3_MC_SetLimitMAX [Morpher Modifier] [Channel Index] [Float Value]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;

	int sel = arg_list[1]->to_int(); sel -= 1;
	if(sel>=100) sel = 99;
	if(sel<0) sel = 0;

	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();

	mp->chanBank[sel].mSpinmax = arg_list[2]->to_float();
	mp->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);

	needs_redraw_set();

	return &true_value;
}


/*===========================================================================*\
 |	Sets the lower limit for the channel's value
\*===========================================================================*/

Value*
wm3_mc_setlimitmin_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_mc_setlimitmin, 3, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_MC_SetLimitMIN [Morpher Modifier] [Channel Index] [Float Value]"));
	type_check(arg_list[1], Integer, _T("WM3_MC_SetLimitMIN [Morpher Modifier] [Channel Index] [Float Value]"));
	type_check(arg_list[2], Float, _T("WM3_MC_SetLimitMIN [Morpher Modifier] [Channel Index] [Float Value]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;

	int sel = arg_list[1]->to_int(); sel -= 1;
	if(sel>=100) sel = 99;
	if(sel<0) sel = 0;

	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();

	mp->chanBank[sel].mSpinmin = arg_list[2]->to_float();
	mp->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);

	needs_redraw_set();

	return &true_value;
}


/*===========================================================================*\
 |	Returns TRUE if the 'Use Vertex Selection' button is on
\*===========================================================================*/

Value*
wm3_mc_getvertsel_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_mc_getvertsel, 2, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_MC_GetUseVertexSel [Morpher Modifier] [Channel Index]"));
	type_check(arg_list[1], Integer, _T("WM3_MC_GetUseVertexSel [Morpher Modifier] [Channel Index]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;

	int sel = arg_list[1]->to_int(); sel -= 1;
	if(sel>=100) sel = 99;
	if(sel<0) sel = 0;

	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();

	if(mp->chanBank[sel].mUseSel) return &true_value;
	return &false_value;
}


/*===========================================================================*\
 |	Sets whether or not to use vertex selection in this channel
\*===========================================================================*/

Value*
wm3_mc_setvertsel_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_mc_setvertsel, 3, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_MC_SetUseVertexSel [Morpher Modifier] [Channel Index] [true/false]"));
	type_check(arg_list[1], Integer, _T("WM3_MC_SetUseVertexSel [Morpher Modifier] [Channel Index] [true/false]"));
	type_check(arg_list[2], Boolean, _T("WM3_MC_SetUseVertexSel [Morpher Modifier] [Channel Index] [true/false]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;

	int sel = arg_list[1]->to_int(); sel -= 1;
	if(sel>=100) sel = 99;
	if(sel<0) sel = 0;

	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();

	mp->chanBank[sel].mUseSel = arg_list[2]->to_bool();
	mp->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);

	needs_redraw_set();

	return &true_value;
}


/*===========================================================================*\
 |	Returns an estimation of how many bytes this channel takes up in memory
\*===========================================================================*/

Value*
wm3_mc_getmemuse_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_mc_getmemuse, 2, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_MC_GetMemUse [Morpher Modifier] [Channel Index]"));
	type_check(arg_list[1], Integer, _T("WM3_MC_GetMemUse [Morpher Modifier] [Channel Index]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;

	int sel = arg_list[1]->to_int(); sel -= 1;
	if(sel>=100) sel = 99;
	if(sel<0) sel = 0;

	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();

	float tmSize = 0.0f;
	tmSize += mp->chanBank[sel].getMemSize();

	return Float::intern(tmSize);
}


/*===========================================================================*\
 |	The actual number of points in this channel
\*===========================================================================*/

Value*
wm3_mc_getnumpts_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_mc_getnumpts, 2, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_MC_NumPts [Morpher Modifier] [Channel Index]"));
	type_check(arg_list[1], Integer, _T("WM3_MC_NumPts [Morpher Modifier] [Channel Index]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;

	int sel = arg_list[1]->to_int(); sel -= 1;
	if(sel>=100) sel = 99;
	if(sel<0) sel = 0;

	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();
	MaxMorphModifier maxMorpher(mp);
	return Integer::intern(maxMorpher.GetMorphChannel(sel).NumMorphPoints());
}


/*===========================================================================*\
 |  The number of 'morphable points' in this channel
 |  'morphable points' are those that are different to the original object
\*===========================================================================*/

Value*
wm3_mc_getnummpts_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_mc_getnummpts, 2, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_MC_NumMPts [Morpher Modifier] [Channel Index]"));
	type_check(arg_list[1], Integer, _T("WM3_MC_NumMPts [Morpher Modifier] [Channel Index]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;

	int sel = arg_list[1]->to_int(); sel -= 1;
	if(sel>=100) sel = 99;
	if(sel<0) sel = 0;

	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();

	return Integer::intern(static_cast<int>(mp->chanBank[sel].mPoints.size()));	// SR DCAST64: Downcast to 2G limit.
}


/*===========================================================================*\
 |	Gets a Point3 value of the <index> point in the channel
\*===========================================================================*/

Value*
wm3_mc_getmorphpoint_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_mc_getmorphpoint, 3, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_MC_GetMorphPoint [Morpher Modifier] [Channel Index] [Index]"));
	type_check(arg_list[1], Integer, _T("WM3_MC_GetMorphPoint [Morpher Modifier] [Channel Index] [Index]"));
	type_check(arg_list[2], Integer, _T("WM3_MC_GetMorphPoint [Morpher Modifier] [Channel Index] [Index]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;

	int sel = arg_list[1]->to_int(); sel -= 1;
	if(sel>=100) sel = 99;
	if(sel<0) sel = 0;

	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();
	MaxMorphModifier maxMorpher(mp);

	return new Point3Value(maxMorpher.GetMorphChannel(sel).GetMorphPoint(arg_list[2]->to_int()));
}


/*===========================================================================*\
 |	Gets a floating point value of the <index> weight in the channel
\*===========================================================================*/

Value*
wm3_mc_getmorphweight_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_mc_getmorphweight, 3, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_MC_GetMorphWeight [Morpher Modifier] [Channel Index] [Index]"));
	type_check(arg_list[1], Integer, _T("WM3_MC_GetMorphWeight [Morpher Modifier] [Channel Index] [Index]"));
	type_check(arg_list[2], Integer, _T("WM3_MC_GetMorphWeight [Morpher Modifier] [Channel Index] [Index]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;

	int sel = arg_list[1]->to_int(); sel -= 1;
	if(sel>=100) sel = 99;
	if(sel<0) sel = 0;

	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();
	MaxMorphModifier maxMorpher(mp);
	return Float::intern((float)maxMorpher.GetMorphChannel(sel).GetMorphPointWeight(arg_list[2]->to_int()));
}


/*===========================================================================*\
 |	Resets the internal object cache, forcing a complete rebuild.
\*===========================================================================*/

Value*
wm3_rebuildIMC_cf(Value** arg_list, int count)
{
	check_arg_count(wm3_rebuildIMC, 1, count);
	type_check(arg_list[0], MAXModifier, _T("WM3_RebuildInternalCache [Morpher Modifier]"));

	ReferenceTarget* obj = arg_list[0]->to_modifier();	
	if( !check_ValidMorpher(obj,arg_list) ) return &false_value;

	MorphR3 *mp = (MorphR3*)arg_list[0]->to_modifier();
	mp->MC_Local.NukeCache();
	mp->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);

	needs_redraw_set();

	return &true_value;
}
