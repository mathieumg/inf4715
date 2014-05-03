/*===========================================================================*\
 | 
 |  FILE:	wM3_main.cpp
 |			Weighted Morpher for MAX R3
 |			Main class and plugin code
 | 
 |  AUTH:   Harry Denholm
 |			Copyright(c) Kinetix 1999
 |			All Rights Reserved.
 |
 |  HIST:	Started 22-5-98
 | 
\*===========================================================================*/


/*===========================================================================*\
 | Includes and global/macro setup
\*===========================================================================*/
#include "wM3.h"

ClassDesc* GetMorphR3Desc();

// Initialize MorphR3::ip to the core interface since it's used everywhere without
// ensuring that is non-NULL. Leaving it NULL will cause crashes when the morpher
// is used via mxs or the Morpher API and the morpher instance being worked with 
// is not displayed (edited) in the Modify panel.
IObjParam *MorphR3::ip		= dynamic_cast<IObjParam*>(GetCOREInterface());
HWND	MorphR3::hMaxWnd		=	NULL;
const float MorphR3::kTensionMin = 0.0f;
const float MorphR3::kTensionMax = 1.0f;
const float MorphR3::kProgressiveTargetWeigthMin = 0.0f;
const float MorphR3::kProgressiveTargetWeigthMax = 100.0f;

/*===========================================================================*\
 | Parameter Blocks
\*===========================================================================*/


// Global parameter description
static ParamBlockDescID GlobalParams[] = {
	{ TYPE_INT, NULL, FALSE, 0 },	// overrides: Use Limits
	{ TYPE_FLOAT, NULL, FALSE, 1 },	// overrides: Spinner Min
	{ TYPE_FLOAT, NULL, FALSE, 2 },	// overrides: Spinner Max
	{ TYPE_INT, NULL, FALSE, 3 },	// overrides: Use Selection
	{ TYPE_INT, NULL, FALSE, 4 },	// advanced:  Value increments
	{ TYPE_INT, NULL, FALSE, 5 },	// clist:	  Auto load
};
#define MR3_SIZE_GLOBALS	6


MorphR3::MorphR3()
{
	DbgAssert(GetCOREInterface() == ip);
	mFileVersion = 0.0f;

	// Load the channels
	chanBank.resize(MR3_NUM_CHANNELS);
	chanNum = 0;
	chanSel = 0;
	morphmaterial = NULL;

	for(int q=0;q<100;q++) 
	{
		chanBank[q].cblock = NULL;
		chanBank[q].mp = this;
	}

	newname = NULL;
	tccI = FALSE;

	// Zero and init the marker system
	markerName.ZeroCount();
	markerIndex.ZeroCount();
	markerSel		= -1;

	hwLegend = hwGlobalParams = hwChannelList = hwChannelParams = hwAdvanced = NULL;

	// Don't record anything yet!
	recordModifications = FALSE;
	recordTarget = 0;

	// Create the parameter block
	pblock = NULL;
	ReplaceReference(0,CreateParameterBlock(GlobalParams,MR3_SIZE_GLOBALS,1));	
	assert(pblock);	


	// Assign some global defaults
	pblock->SetValue(PB_OV_USELIMITS,	0,	1);
	pblock->SetValue(PB_OV_SPINMIN,		0,	0.0f);
	pblock->SetValue(PB_OV_SPINMAX,		0,	100.0f);
	pblock->SetValue(PB_OV_USESEL,		0,	0);
	pblock->SetValue(PB_AD_VALUEINC,	0,	1);
	pblock->SetValue(PB_CL_AUTOLOAD,	0,	0);


	// Build the multiple set of pblocks
	for( int a=0;a<MR3_NUM_CHANNELS;a++){
	
		// Paramblock / morph channel so we have more organized TV support
		ParamBlockDescID* channelParams = new ParamBlockDescID();
		channelParams->type = TYPE_FLOAT;
		channelParams->user = NULL;
		channelParams->animatable = TRUE;
		channelParams->id = 1;

		ReplaceReference(1+a, CreateParameterBlock(channelParams,1,1));	
		assert(chanBank[a].cblock);

		// TCB controller as default
		// This helps with the very erratic curve results you get when
		// animating with normal bezier float controllers

		chanBank[a].SetUpNewController();
		delete channelParams;
		channelParams = NULL;

		cSpinmin = NULL; cSpinmax = NULL; cCurvature = NULL; cTargetPercent = NULL;
	}
}

MorphR3::~MorphR3()
{
	if(morphmaterial ) morphmaterial->morphp = NULL;
	markerName.ZeroCount();
	markerIndex.ZeroCount();
	DeleteAllRefs();
	chanBank.erase(chanBank.begin(), chanBank.end());
	chanBank.resize(0);

	Interface *Cip = GetCOREInterface();
	if(Cip&&tccI) { Cip->UnRegisterTimeChangeCallback(this); tccI=FALSE; }

	cSpinmin = NULL; cSpinmax = NULL; cCurvature = NULL; cTargetPercent = NULL;
}

void MorphR3::NotifyInputChanged(Interval changeInt, PartID partID, RefMessage message, ModContext *mc)
{
	if( (partID&PART_TOPO) || (partID&PART_GEOM) || (partID&PART_SELECT) )
	{
		if(MC_Local.AreWeCached()) MC_Local.NukeCache();
	//	NotifyDependents(FOREVER, PART_OBJ, REFMSG_CHANGE);
	}
}

Interval MorphR3::LocalValidity(TimeValue t)
{
	Interval iv = FOREVER;
	float ftmp=0;
	int itmp=0;

	for(int i=0;i<100;i++)
	{
		chanBank[i].cblock->GetValue(0,t,ftmp,iv);	

		if(chanBank[i].mConnection)
		{
			chanBank[i].mConnection->GetNodeTM(t,&iv);
		}
	}

	int ALoad; 
	pblock->GetValue(PB_CL_AUTOLOAD, 0, ALoad, iv);
	if(ALoad==1) iv = Interval(t,t);

	return iv; 
}

RefTargetHandle MorphR3::Clone(RemapDir& remap)
{
	int x;
	int nrefs = NumRefs();

	MorphR3* newmod = new MorphR3();
	newmod->chanBank = chanBank;

	int chansize = static_cast<int>(newmod->chanBank.size());	// SR DCAST64: Downcast to 2G limit.

	for(int i=0; i<chansize; i++) { 
		newmod->chanBank[i].CopyTargetPercents(chanBank[i]);
	}

	if (pblock) newmod->ReplaceReference (0, remap.CloneRef(pblock));

	for(x=1;x<=100;x++)		
	{
		newmod->ReplaceReference (x, remap.CloneRef(chanBank[x-1].cblock));
		newmod->chanBank[x-1] = chanBank[x-1];
		newmod->chanBank[x-1].mp = newmod;
		newmod->chanBank[x-1].mConnection = NULL;
	}

	nrefs = NumRefs();
	for(x=101; x<=nrefs; x++)	
	{
	
		RefTargetHandle retarg = GetReference(x);
		if ( retarg == NULL ) continue;
		if( remap.FindMapping( retarg )  ) {
			newmod->ReplaceReference (x, remap.CloneRef(retarg) );
		}
		else {
			newmod->ReplaceReference (x, retarg);
		}
	}
	
	newmod->markerName = markerName;
	newmod->markerIndex = markerIndex;

	BaseClone(this, newmod, remap);
	return(newmod);
}



class MorpherCheckCircular : public GeomPipelineEnumProc
	{
public:  
   MorpherCheckCircular(INode *nd) : mNode(NULL) {}
   PipeEnumResult proc(ReferenceTarget *object, IDerivedObject *derObj, int index);
   INode *mNode;
protected:
   MorpherCheckCircular(MorpherCheckCircular& rhs); // disallowed
   MorpherCheckCircular & operator=(const MorpherCheckCircular& rhs); // disallowed
};

PipeEnumResult MorpherCheckCircular::proc(
   ReferenceTarget *object, 
   IDerivedObject *derObj, 
   int index)
{
   
   ModContext* curModContext = NULL;
   if ((derObj != NULL) && (object && object->ClassID() == Class_ID(MR3_CLASS_ID)))
   {
		Modifier* ModifierPtr = dynamic_cast<Modifier*>(object);
	
	   //loop through the references 
		for(int k=101; k<=200; k++)
		{
			if(ModifierPtr->GetReference(k) == mNode)
			{
				// if there is a circular reference delete it 
				ModifierPtr->DeleteReference(k); 
				// and reset the channel and controller
				MorphR3 *mp = static_cast<MorphR3 *>(ModifierPtr);
				if(mp) 
				{
					mp->chanBank[k-101].cblock->RemoveController(0);
					mp->chanBank[k-101].ResetMe();
				}
			}
		}
   }
   return PIPE_ENUM_CONTINUE;
}

void MorphR3::TestMorphReferenceDependencies( const RefTargetHandle hTarget)
{
	if(!hTarget->SuperClassID()==BASENODE_CLASS_ID) return;
	INode *nd = (INode*)hTarget;
	Object* ObjectPtr = nd->GetObjectRef();

	MorpherCheckCircular pipeEnumProc(nd);
    EnumGeomPipeline(&pipeEnumProc, ObjectPtr);
 

}

//From ReferenceMaker 
RefResult MorphR3::NotifyRefChanged(
		Interval changeInt, RefTargetHandle hTarget,
		PartID& partID,  RefMessage message) 
{
	TSTR outputStr;
	
	switch (message) {

		case REFMSG_MODIFIER_ADDED:
		case REFMSG_NODE_LINK:
		case REFMSG_REF_ADDED:
		{
			TestMorphReferenceDependencies(hTarget);
			break;
		}

		case REFMSG_NODE_MATERIAL_CHANGED:{
			CheckMaterialDependency( );
			break;
		}

		case REFMSG_GET_PARAM_DIM: {
			GetParamDim *gpd = (GetParamDim*)partID;
			gpd->dim = defaultDim; 
			return REF_HALT; 
			}

		case REFMSG_GET_PARAM_NAME: {
			GetParamName *gpn = (GetParamName*)partID;

			switch (gpn->index) {

			case 0:	{		
					if(hTarget==pblock) gpn->name = TSTR(GetString(IDS_PBN_USELIMITS+gpn->index));

					for(int k=0;k<100;k++)
					{
						if(hTarget==chanBank[k].cblock&&chanBank[k].mActive!=FALSE) {	
							outputStr.printf(_T("[%d] %s  (%s)"), 
								k+1,
								chanBank[k].mName,
								chanBank[k].mConnection?GetString(IDS_ONCON):GetString(IDS_NOCON)
								);
							gpn->name = outputStr;
						}
					}
					break;}
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				{
				gpn->name = TSTR(GetString(IDS_PBN_USELIMITS+gpn->index));
				break;}


				default:
					{
						
						// 'should' never show up. catch what index it is asking
						// for if it ever does
						TCHAR s[25];
						_stprintf(s,_T("debug_bad_index [%i]"),gpn->index);
						gpn->name = TSTR(s); 

						
					}
				}
			return REF_HALT; 
			}

		// Handle the deletion of a morph target
		case REFMSG_TARGET_DELETED:{
			for(int u=0;u<100;u++){
				if (hTarget==chanBank[u].mConnection) {
					DeleteReference(101+u);
					chanBank[u].mConnection = NULL;
				}
				int ct = static_cast<int>(chanBank[u].mTargetCache.size());
				for(int j=0; j<ct; j++) {
					if (hTarget==chanBank[u].mTargetCache[j].mTargetINode) {
						if (hTarget)
						{
							if (theHold.Holding()) 
								theHold.Put(new Restore_FullChannel(this, u));

							DeleteReference(GetRefNumber(u, j+1));
							chanBank[u].mTargetCache[j].mTargetINode = NULL;
						}
						break;
					}
				}
			}
			Update_channelFULL();
			Update_channelParams();
			NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
			NotifyDependents(FOREVER, PART_ALL, REFMSG_SUBANIM_STRUCTURE_CHANGED);
			break;
		}
		
			// NS: 04/01/01 Bugfix 275831: Don't block the dependency test
		case REFMSG_TEST_DEPENDENCY:
			return REF_SUCCEED;

		case REFMSG_SUBANIM_STRUCTURE_CHANGED:
			Update_channelFULL();

		default:{
			if(hTarget)
			{
				if(hTarget->SuperClassID() == BASENODE_CLASS_ID && pblock)
				{
					int itmp; Interval valid = FOREVER;
					pblock->GetValue(PB_CL_AUTOLOAD, 0, itmp, valid);
					
					if(!itmp)
					{
						return REF_STOP;				
					}
				}
			}

			break;
		}
	}

	return REF_SUCCEED;
}

TSTR MorphR3::SvGetRelTip(IGraphObjectManager *gom, IGraphNode *gNodeTarger, int id, IGraphNode *gNodeMaker)
{
	return SvGetName(gom, gNodeMaker, false) + _T(" -> ") + gNodeTarger->GetAnim()->SvGetName(gom, gNodeTarger, false);
}

SvGraphNodeReference MorphR3::SvTraverseAnimGraph(IGraphObjectManager *gom, Animatable *owner, int id, DWORD flags)
{
	SvGraphNodeReference nodeRef = Modifier::SvTraverseAnimGraph( gom, owner, id, flags );

	if( nodeRef.stat == SVT_PROCEED ) {
		for( int i=0; i<(int)chanBank.size(); i++ ) {
			gom->AddRelationship( nodeRef.gNode, chanBank[i].mConnection, i, RELTYPE_MODIFIER );
		}
	}

	return nodeRef;
}


// =====================================================================================
// SCARY REFERENCE AND SUBANIM STUFF

bool MorphR3::CheckMaterialDependency( void )
{
	Mtl* mat; INode *node;
	for(int k=0; k<100; k++)
	{
		node = chanBank[k].mConnection;
		if(node && node->GetMtl() ){
			mat = node->GetMtl();
			if ( mat == morphmaterial ) goto CANCEL;
			int nrefs = mat->NumSubMtls();
			for( int num=0; num<nrefs; num++ ){
				Mtl* m = mat->GetSubMtl(num);
				if ( m )	// > 8/30/02 - 3:25pm --MQM-- add this line...wasn't checking for NULL
					if ( m == morphmaterial ) goto CANCEL;
			}
		}
	}
	return	CheckSubMaterialDependency( );

CANCEL:
	if(mat) {
		M3Mat * m3m = static_cast<M3Mat * >(mat);
		m3m->DeleteAllRefs();
//		m3m->Reset();
		if(m3m->morphp ) m3m->morphp->morphmaterial = NULL; 
		m3m->morphp = NULL; 
	}
	if(hMaxWnd) 
	{
		TSTR cyclic;
		cyclic = GetString(IDS_CYCLIC);
		MessageBox(hMaxWnd,cyclic,GetString(IDS_CLASS_NAME),MB_OK);
	}
	

	return true; 
}


bool MorphR3::CheckSubMaterialDependency( void )
{
	Mtl* mat; M3Mat *m3m=NULL;
	if(!morphmaterial) return false;
	int nummats = morphmaterial->NumSubMtls();

	for(int i=0; i<nummats; i++)
	{
		mat = morphmaterial->GetSubMtl(i);
		if(mat  ) {
			if( mat->ClassID() == M3MatClassID ) {
				M3Mat *m3m = static_cast<M3Mat *> (mat);
				MorphR3 *mp = m3m->morphp;
				if(mp){
					for(int j=101; j<=200; j++){
						for(int k=101; k<=200; k++){
							if( GetReference(j) && GetReference(j) == mp->GetReference(k) ) {
								goto CANCEL;
							}
						}
					}
				}
			}
		}
	}
	return	false; 

CANCEL:
	if(m3m) {
		m3m->DeleteAllRefs();
//		m3m->Reset();
		if(m3m->morphp ) m3m->morphp->morphmaterial = NULL; 
		m3m->morphp = NULL; 
	}
	if(hMaxWnd) 
	{
		TSTR cyclic;
		cyclic = GetString(IDS_CYCLIC_MATERIAL);
		MessageBox(hMaxWnd,cyclic,GetString(IDS_CLASS_NAME),MB_OK);
	}
	
	return true; 
}


int MorphR3::NumRefs() 
{
	return 1+(MR3_NUM_CHANNELS*(2+MAX_PROGRESSIVE_TARGETS));
}


// TODO: link this to the MR3_ defines

RefTargetHandle MorphR3::GetReference(int i) 
{
	if(i==0) return pblock;
	else if(i>0&&i<=100) return chanBank[i-1].cblock;
	else if(i>100&&i<=200) return chanBank[i-101].mConnection;
	else if(i>200 && i<=200+100*MAX_PROGRESSIVE_TARGETS){
		int chanNum = ((i-201)/MAX_PROGRESSIVE_TARGETS);
		int targNum = ( (i-201)-(chanNum*MAX_PROGRESSIVE_TARGETS) );
		return chanBank[chanNum].mTargetCache[targNum].mTargetINode;
	}

	else return NULL;
}

void MorphR3::SetReference(int i, RefTargetHandle rtarg)
{
	if(i==0) pblock = (IParamBlock*)rtarg;
	if(i>0&&i<=100) {
		chanBank[i-1].cblock = (IParamBlock*)rtarg;
	}
	if(i>100&&i<=200) {
		chanBank[i-101].mConnection = (INode*)rtarg;
	}
	if(i>200 && i<=200+100*MAX_PROGRESSIVE_TARGETS){
		int chanNum = ((i-201)/MAX_PROGRESSIVE_TARGETS);
		int targNum = ((i-201)-(chanNum*MAX_PROGRESSIVE_TARGETS));
		chanBank[chanNum].mTargetCache[targNum].mTargetINode= (INode*)rtarg;
	}
}

int MorphR3::NumSubs() 
{ 
	return 1+(MR3_NUM_CHANNELS);
}  

Animatable* MorphR3::SubAnim(int i) 
{ 
	if(i==0) return pblock;
	else if( chanBank[i-1].mActive == TRUE ) return chanBank[i-1].cblock;
	else return NULL; 
}

TSTR MorphR3::SubAnimName(int i) 
{ 
	if(i==0) return GetString(IDS_SUBANIMPARAM);
	else return chanBank[i-1].mName;
}

void MorphR3PostLoadCallback::proc(ILoad *iload)
{
	if(!mp || mp->mFileVersion>0 ) 
	{
		
	}
	else
	{
		for(int i=0; i<MR3_NUM_CHANNELS; i++) 
		{
			mp->chanBank[i].ReNormalize();
		}
	}
	delete this;
}

void MorphR3::DeleteTarget(void)
{
	morphChannel &bank = CurrentChannel();
	int channum = CurrentChannelIndex();

	if(!&bank || !bank.mNumProgressiveTargs) return;

	int targetnum = bank.iTargetListSelection;

	if(bank.iTargetListSelection<0) return;
	theHold.Begin();
	theHold.Put(new Restore_FullChannel(this, channum) );

	if(targetnum==0)
	{
		//Make sure we get rid of the old reference before replacing it
		ReplaceReference( channum + 101, NULL );
		bank = bank.mTargetCache[0];
		//Rebuild the channel node information
		bank.buildFromNode( bank.mConnection, FALSE, GetCOREInterface()->GetTime(), TRUE );
		for(int i=0; i<bank.mNumProgressiveTargs-1; i++) {
			bank.mTargetCache[i] = bank.mTargetCache[i+1];
		}
		bank.mTargetCache[bank.mNumProgressiveTargs-1].Clear();
	}
	else if(targetnum>0 && bank.mNumProgressiveTargs && targetnum<=bank.mNumProgressiveTargs)
	{
		//Replace the reference properly instead of just deleting it.
		ReplaceReference( ( channum * MAX_PROGRESSIVE_TARGETS ) + 200 + targetnum, NULL );
		for(int i=targetnum; i<bank.mNumProgressiveTargs; i++) {
			bank.mTargetCache[i-1] = bank.mTargetCache[i];
		}
		bank.mTargetCache[bank.mNumProgressiveTargs-1].Clear();
	}

	bank.mNumProgressiveTargs--;

	bank.ReNormalize();
	if(bank.iTargetListSelection) bank.iTargetListSelection--;

	bank.rebuildChannel();
	Update_channelFULL();

	theHold.Accept(GetString(IDS_DELETE_TARGET));
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	ip->RedrawViews(ip->GetTime());
}

void MorphR3::DeleteChannel(const int &cIndex)
{ 	
	chanBank[cIndex].SetUpNewController();
	DeleteReference(101+cIndex);

	int refnum;
	for(int i=1; i<=chanBank[cIndex].mNumProgressiveTargs; i++) {
		refnum = (cIndex * MAX_PROGRESSIVE_TARGETS)+i+200	;
		DeleteReference( refnum );
	}

	// Ask channel to reset itself
	chanBank[cIndex].ResetMe();	
}

void MorphR3::SwapTargets(const int way)
{
	if(!hwChannelParams) return;
	int from = (int)SendDlgItemMessage( hwChannelParams, IDC_TARGETLIST, LB_GETCURSEL, 0, 0);
	int to = from + way;
	SwapTargets(from, to, false);
}


void MorphR3::SwapTargets(const int from, const int to, const bool isundo)
{
	/////////////////////////////////
	int currentChan = CurrentChannelIndex();
	morphChannel &cBank = chanBank[currentChan];
	if(from<0 || to<0 || from>cBank.NumProgressiveTargets() || to>cBank.NumProgressiveTargets()) return;

	if(!isundo) {
		theHold.Begin();
		theHold.Put(new Restore_TargetMove(this, from, to) );
	}

	if(from!=0 && to!=0) 
	{
		TargetCache toCache(cBank.mTargetCache[to-1]);
		cBank.mTargetCache[to-1] = cBank.mTargetCache[from-1]; 
		cBank.mTargetCache[from-1] = toCache; 
		ReplaceReference(GetRefNumber(currentChan, from), cBank.mTargetCache[from-1].RefNode() );
		ReplaceReference(GetRefNumber(currentChan, to), cBank.mTargetCache[to-1].RefNode());
	}
	else //switch channel and first targetcache
	{
		TargetCache tempCache(cBank.mTargetCache[0]);
		cBank.mTargetCache[0] = cBank;
		cBank = tempCache;
		ReplaceReference(101+currentChan, cBank.mConnection );
		int id = GetRefNumber(currentChan, 0);
		ReplaceReference(id, cBank.mTargetCache[0].RefNode() );
		cBank.rebuildChannel();
	}

	if(!isundo) {
		theHold.Accept(GetString (IDS_MOVE_TARGETS));
	}

	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	CurrentChannel().iTargetListSelection = to;
	Update_channelFULL();
	GetCOREInterface()->RedrawViews(GetCOREInterface()->GetTime());	
}


void MorphR3::RescaleWorldUnits (float f)
{
	for( int i=0; i<MR3_NUM_CHANNELS;i++)
	{

		int numPoints = chanBank[i].mNumPoints;
		for (int j = 0; j < numPoints; j++)
		{
			chanBank[i].mDeltas[j] *= f;
			chanBank[i].mPoints[j] *= f;
		}
	}
}
