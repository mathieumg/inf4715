/*===========================================================================*\
 | 
 |  FILE:	wM3_dlg-fns.cpp
 |			Weighted Morpher for MAX R3
 |			Functions to update and refresh chunks of the UI
 |			Seperated here to make my job easier to maintain 'em!
 | 
 |  AUTH:   Harry Denholm
 |			Copyright(c) Kinetix 1999
 |			All Rights Reserved.
 |
 |  HIST:	Started 16-9-98
 | 
\*===========================================================================*/

#include "wM3.h"


// Clamp the channel number (0-90)
void MorphR3::Clamp_chanNum()
{
	if(chanNum<0) chanNum = 0;
	if(chanNum>=90) chanNum = 90;
}

// Handle the scroll bar
void MorphR3::VScroll(int code, short int cpos ) {
	switch (code) {
		case SB_LINEUP: 	chanNum--;		break;
		case SB_LINEDOWN:	chanNum++;		break;
		case SB_PAGEUP:		chanNum -= 10;	break;
		case SB_PAGEDOWN:	chanNum += 10;	break;
		
		case SB_THUMBPOSITION: 
		case SB_THUMBTRACK:
			chanNum = cpos;
			break;
		}

	// Check for out-of-bounds values
	Clamp_chanNum();
	
	// Reposition the scrollbar
	HWND vScr = GetDlgItem(hwChannelList,IDC_LISTSCROLL);
	SetScrollPos((HWND)vScr, SB_CTL, chanNum, TRUE); 

	markerSel = -1;

	// Update everything
	Update_channelFULL();
	Update_channelParams();
	}


void MorphR3::Update_colorIndicators()
{
	if(hwChannelList)
	{
		Rect rect;
		RECT out;
		GetClientRectP(GetDlgItem(hwChannelList,IDC_I1),&rect);
		out.left = rect.left-1;
		out.top = rect.top-1;
		GetClientRectP(GetDlgItem(hwChannelList,IDC_I10),&rect);
		out.right = rect.left+rect.w()+1;
		out.bottom = rect.top+rect.h()+1;

		InvalidateRect(hwChannelList,&out,FALSE);
	}
}

// Update the names of all the morph channels
void MorphR3::Update_channelNames()
{
	if(hwChannelList)
	{
		ICustButton *iTmp;
		int i,tIdx = 0;

		for( i=IDC_P1;i<IDC_P10+1;i++){
			iTmp = GetICustButton(GetDlgItem(hwChannelList,i));
			iTmp->SetText(chanBank[chanNum+tIdx].mName);
			iTmp->SetCheck( (i-IDC_P1 == chanSel) );
			ReleaseICustButton(iTmp);
			tIdx++;
		}
		iTmp = NULL;
	}
}



float MorphR3::TrimDown( float value, int decimalpts )
{
	// changed to _stprintf conversion, for consistency
	// my original method was slower and introduced ~0.01 rounding errors
	// which make the values flicker during animation playback
	_stprintf(trimD,_T(" %.1f    "),value);
	return (float)_tstof(trimD);
	/*
	float tempfl;
	float powC = (float)pow(10,decimalpts);
	value*=powC;
	tempfl=(float)floor(value);
	tempfl/=powC;
	return tempfl;
	*/
}

// Update the spinners for each channel
void MorphR3::Update_channelValues()
{
	TimeValue t = GetCOREInterface()->GetTime();
	if(hwChannelList)
	{
		Update_channelLimits();

		float tmp; Interval valid = FOREVER; int i;
		for( i=0; i<10; i++ )
		{
			if(chanBank[chanNum+i].cblock)
			{
				chanBank[chanNum+i].cblock->GetValue(0, t, tmp, valid);
				BOOL kbON = chanBank[chanNum+i].cblock->KeyFrameAtTime(0,t);
				chanSpins[i]->SetKeyBrackets(kbON);
				float td = TrimDown(tmp,1);
				chanSpins[i]->SetValue(td,FALSE);
			}
		}
	}
	if(hwAdvanced) {
		DisplayMemoryUsage();
	}
}

// Update the channel bank range
void MorphR3::Update_channelInfo()
{
	if(hwChannelList)
	{
		TCHAR s[15];
		_stprintf(s, _T("%d - %d"), chanNum+1,chanNum+10);
		SetWindowText(GetDlgItem(hwChannelList,IDC_PAGE),s);

		int tmp; Interval valid = FOREVER;

		pblock->GetValue(PB_CL_AUTOLOAD, 0, tmp, valid);
		SetCheckBox(hwChannelList,IDC_AUTOLOAD,tmp?TRUE:FALSE);
	}
}

void MorphR3::Update_channelLimits()
{
	if(hwChannelList)
	{
		TimeValue t = ip->GetTime();
		float tmp,min,max; Interval valid = FOREVER;
		float gmin,gmax; int i;

		pblock->GetValue(PB_OV_SPINMIN, t, gmin, valid);
		pblock->GetValue(PB_OV_SPINMAX, t, gmax, valid);

		for( i=0; i<10; i++ )
		{
			min = -999.0f;
			max = 999.0f;

			if(chanBank[chanNum+i].mUseLimit)
			{
				min = chanBank[chanNum+i].mSpinmin;
				max = chanBank[chanNum+i].mSpinmax;
			}

			pblock->GetValue(PB_OV_USELIMITS, t, tmp, valid);
			if(tmp)
			{
				// Set to globals
				min = gmin;
				max = gmax;
			}

			if(chanBank[chanNum+i].cblock)
			{
				float f;
				chanBank[chanNum+i].cblock->GetValue(0, t, f, valid);
				if(f<min) {
					min = f-1.0f;
				}
				else if(f>max) {
					max = f + 1.0f;
				}
			}

			chanSpins[i]->SetLimits(min,max,FALSE);
		}
	}
}

// Update the markers list
void MorphR3::Update_channelMarkers()
{
	if(hwChannelList)
	{
		HWND hwMarker	= GetDlgItem(hwChannelList,IDC_MARKERLIST);
		HWND hwSave		= GetDlgItem(hwChannelList,IDC_SAVEMARKER);
		HWND hwDel		= GetDlgItem(hwChannelList,IDC_DELMARKER);

		// Reset the channel marker
		SendMessage(hwMarker,CB_RESETCONTENT,0,0);

		// Add the bookmark names to the dropdown
		for( int i=0; i<markerName.Count(); i++ )
		{
			SendMessage(hwMarker,CB_ADDSTRING,0,(LPARAM) (LPCTSTR) markerName[i]);
		}

		// Set the current selection
		SendMessage(hwMarker,CB_SETCURSEL ,(WPARAM)markerSel,0);

		BOOL sEn = FALSE;
		BOOL dEn = FALSE;

		TCHAR tm[256];
		GetWindowText(hwMarker,tm,255);
		if(_tcsicmp(tm,_T(""))!=0)
		{	
			sEn=TRUE;
			dEn=FALSE;
		}

		if( markerSel != -1 )
		{
			dEn = TRUE;		
			sEn = FALSE;
		}

		EnableWindow(hwSave,sEn);
		EnableWindow(hwDel,dEn);
		if( SendMessage( hwMarker, CB_GETCOUNT , 0, 0) == 0 ) { EnableWindow(hwDel,FALSE); }
	}
}

// Call all the above
void MorphR3::Update_channelFULL()
{
	if(hwChannelList)
	{
		Clamp_chanNum();

		Update_colorIndicators();
		Update_channelNames();
		Update_channelValues();
		Update_channelInfo();
		Update_channelLimits();
		Update_channelMarkers();
		Update_channelParams();
		Update_TargetListBoxNames();

		// Reposition the scrollbar
		HWND vScr = GetDlgItem(hwChannelList,IDC_LISTSCROLL);
		SetScrollPos((HWND)vScr, SB_CTL, chanNum, TRUE); 
	}
}

// Evalulate the increments value to a fp value
float MorphR3::GetIncrements()
{
	int idTmp = 0; 
	float flRes = 0.0f;
	pblock->GetValue(PB_AD_VALUEINC, 0, idTmp, FOREVER);
	if(idTmp==0) flRes = 5.0f;
	if(idTmp==1) flRes = 1.0f;
	if(idTmp==2) flRes = 0.1f;

	return flRes;
}


// Updates the scaling on all relevent spinners
void MorphR3::Update_SpinnerIncrements()
{
	if(hwChannelParams)
	{
		float flRes = GetIncrements();

		cSpinmin->SetScale(flRes);
		cSpinmax->SetScale(flRes);

		for(int i=0;i<10;i++)
			chanSpins[i]->SetScale(flRes);

		glSpinmax->SetScale(flRes);
		glSpinmin->SetScale(flRes);
	}
}


// Update the channel parameters page
void MorphR3::Update_channelParams()
{
	if( hwGlobalParams )
	{
		Interval valid = FOREVER;
		float smin,smax;
		pblock->GetValue(PB_OV_SPINMIN, 0, smin, valid);
		pblock->GetValue(PB_OV_SPINMAX, 0, smax, valid);
		if(glSpinmin) glSpinmin->SetValue(smin, FALSE);
		if(glSpinmax) glSpinmax->SetValue(smax, FALSE);

		int tmp;
		pblock->GetValue(PB_OV_USELIMITS, 0, tmp, valid);
		SetCheckBox(hwGlobalParams,IDC_LIMIT, tmp );

		pblock->GetValue(PB_OV_USESEL, 0, tmp, valid);
		ICustButton *iTmp;		
		iTmp = GetICustButton(GetDlgItem(hwGlobalParams,IDC_USESEL));
		iTmp->SetType(CBT_CHECK);
		iTmp->SetHighlightColor(BLUE_WASH);
						
		if(tmp) iTmp->SetCheck(TRUE);
		else iTmp->SetCheck(FALSE);

		ReleaseICustButton(iTmp);

	}

	if(hwChannelParams)
	{
		Interval valid = FOREVER; int tempV2,tempV3; 
		TimeValue t = ip->GetTime();

		pblock->GetValue(PB_OV_USELIMITS,t,tempV2,valid);
		pblock->GetValue(PB_OV_USESEL,t,tempV3,valid);


		ICustEdit *cName = NULL;

		cName = GetICustEdit(GetDlgItem(hwChannelParams,IDC_CHANNAME));
		cName->SetText( chanBank[chanNum+chanSel].mName );

		TCHAR s[5]; _stprintf(s,_T("%i"),chanNum+chanSel+1);
		SetWindowText( GetDlgItem(hwChannelParams,IDC_CHANNUM), s);

		// load the current channel
		morphChannel *curTmp = &chanBank[chanNum+chanSel];

		// Setup the min/max stuff
		SetCheckBox(hwChannelParams,IDC_LIMIT, curTmp->mUseLimit );
		EnableWindow(GetDlgItem(hwChannelParams,IDC_LIMIT),!tempV2);

		cSpinmin->SetLimits(-999.9f,curTmp->mSpinmax,FALSE);
		cSpinmax->SetLimits(curTmp->mSpinmin,999.9f,FALSE);

		cSpinmin->SetValue(curTmp->mSpinmin,FALSE);
		cSpinmin->SetScale(GetIncrements());

		cSpinmax->SetValue(curTmp->mSpinmax,FALSE);
		cSpinmax->SetScale(GetIncrements());

		if(tempV2) cSpinmax->Disable();
		else cSpinmax->Enable();
		if(tempV2) cSpinmin->Disable();
		else cSpinmin->Enable();


		ReleaseICustEdit(cName);
	
		// Use selection button
		ICustButton *iTmp;

		iTmp = GetICustButton(GetDlgItem(hwChannelParams,IDC_USESEL));

		if(curTmp->mUseSel) iTmp->SetCheck(TRUE);
		else iTmp->SetCheck(FALSE);

		ReleaseICustButton(iTmp);


		// Update target button
		HWND updTmp = GetDlgItem(hwChannelParams,IDC_UPDATETARGET);
		HWND delTmp = GetDlgItem(hwChannelParams,IDC_DELETE);
		HWND makeTmp = GetDlgItem(hwChannelParams,IDC_MAKE);
		HWND vselTmp = GetDlgItem(hwChannelParams,IDC_USESEL);
		HWND xtractTmp = GetDlgItem(hwChannelParams,IDC_EXTRACT);


		SetCheckBox(hwChannelParams,IDC_USECHAN, curTmp->mActiveOverride );



		// Is channel active?
		if( curTmp->mActive ) 
		{
			SetWindowText( delTmp,GetString(IDS_DEL_ON) );
			EnableWindow( delTmp, TRUE);
			EnableWindow( xtractTmp, TRUE);

			iTmp = GetICustButton(vselTmp);
			iTmp->Enable();
			ReleaseICustButton(iTmp);

			iTmp = GetICustButton(makeTmp);
			iTmp->Disable();
			ReleaseICustButton(iTmp);

			cCurvature->Enable();
			cCurvature->SetValue(curTmp->mCurvature, FALSE);

			cTargetPercent->Enable();
			cTargetPercent->SetValue(GetCurrentTargetPercent(), FALSE);
			
		}
		else {
			cCurvature->SetValue(curTmp->mCurvature, FALSE);
			cCurvature->Disable();
			
			cTargetPercent->SetValue( GetCurrentTargetPercent(), FALSE);
			cTargetPercent->Disable();

			SetWindowText( delTmp,GetString(IDS_DEL_OFF) );
			EnableWindow( delTmp, FALSE);
			EnableWindow( xtractTmp, FALSE);

			iTmp = GetICustButton(vselTmp);
			iTmp->Disable();
			ReleaseICustButton(iTmp);

			iTmp = GetICustButton(makeTmp);
			iTmp->Enable();
			ReleaseICustButton(iTmp);
		}

		// Have we got a connection?
		if( curTmp->mConnection != NULL ) 
		{
			SetWindowText( updTmp, GetString(IDS_UPD_ON) );
			EnableWindow( updTmp, TRUE);
			EnableWindow( xtractTmp, FALSE);

			iTmp = GetICustButton(makeTmp);
			iTmp->Disable();
			ReleaseICustButton(iTmp);
		}
		else {
			SetWindowText( updTmp, GetString(IDS_UPD_OFF) );
			EnableWindow( updTmp, FALSE);
		}

		iTmp = GetICustButton(GetDlgItem(hwChannelParams,IDC_USESEL));
		if(tempV3) iTmp->Disable();
		else iTmp->Enable();
		ReleaseICustButton(iTmp);

		Update_TargetListBoxNames();
	}
}

void MorphR3::Update_TargetListBoxNames(void)
{
	if(hwChannelParams) {
		SendDlgItemMessage( hwChannelParams, IDC_TARGETLIST, 
				 LB_RESETCONTENT, 0, 0);
		INode *node; TSTR name; int channum=CurrentChannelIndex();
		morphChannel &bank = CurrentChannel();
		int numtargs = chanBank[chanNum+chanSel].mNumProgressiveTargs;
		int refnum; TSTR emptychannel = GetString(IDS_EMPTY_CHANNEL);
		if(bank.mName != emptychannel)
		{
			for(int i=0; i<=numtargs; i++) {
				if(i==0) refnum = 101+channum;
				else refnum = 200+i+channum*MAX_PROGRESSIVE_TARGETS;
				node = (INode*) GetReference(refnum);
				if(node) {
					name = node->GetName();
					SendDlgItemMessage( hwChannelParams, IDC_TARGETLIST, 
						 LB_ADDSTRING, 0, (LPARAM)(LPCTSTR)name.data());
				}
			}
		}

		if(bank.mNumProgressiveTargs>=0) {
			SendDlgItemMessage( hwChannelParams, IDC_TARGETLIST, 
				 LB_SETCURSEL, (WPARAM)bank.iTargetListSelection, 0);
			cTargetPercent->SetValue(GetCurrentTargetPercent(), FALSE);
		}
		else {
			cTargetPercent->SetValue(0.0f, FALSE);
			cTargetPercent->Disable();
		}

		if ( bank.mName == emptychannel ) {
			cTargetPercent->SetValue(0.0f, FALSE);
			cTargetPercent->Disable();
		}

		BOOL enble = FALSE; if(bank.mNumProgressiveTargs) enble = TRUE;
		EnableWindow( GetDlgItem(hwChannelParams, IDC_DELETE2), enble);
	}
}

// swap/move a channel
void MorphR3::ChannelOp(int targ, int src, int flags)
{
	if(flags==OP_MOVE)
	{
		DeleteChannel( targ );
		chanBank[targ] = chanBank[src];

		// swap the controllers
		//

		Control * srcControl = (Control *)chanBank[src].cblock->GetController(0);
		chanBank[targ].cblock->SetController( 0, srcControl, FALSE);
		chanBank[src].cblock->RemoveController(0);

		// make the proper reference to the source node
		//
		INode * srcNode  = (INode*) GetReference( 101+src );
		SetReference(101+targ,NULL); // null this out so that the replace reference will work. The code: chanBank[targ] = chanBank[src]; copied the pointer we are setting to where it is being set.
		ReplaceReference(101+targ,srcNode);

		DbgAssert( chanBank[targ].mConnection == chanBank[src].mConnection );

		// make the proper references to the progressive targets
		//
		int targRefnum;
		int srcRefnum;
		for(int i=1; i<=chanBank[targ].mNumProgressiveTargs; i++) {
			targRefnum = (targ * MAX_PROGRESSIVE_TARGETS)+i+200	;
			srcRefnum = (src * MAX_PROGRESSIVE_TARGETS)+i+200	;
			SetReference(targRefnum,NULL); // null this out so that the replace reference will work
			ReplaceReference(targRefnum,GetReference( srcRefnum ));
		}

		chanBank[targ].CopyTargetPercents( chanBank[src] );
		DeleteChannel(src);

		// reset src channel to defaults
		//
		chanBank[src].ResetMe();
		chanBank[src].mTargetPercent = 0.0f;

		if(chanBank[targ].mConnection)
		{
			chanBank[targ].buildFromNode(chanBank[targ].mConnection,FALSE,GetCOREInterface()->GetTime());
			for(int i=0; i<chanBank[targ].mNumProgressiveTargs; i++) 
			{ 
				chanBank[targ].mTargetCache[i].Init(chanBank[targ].mTargetCache[i].mTargetINode); 
			}
		}

	}
	else if(flags==OP_SWAP)
	{
		// save off Target info
		//
		IParamBlock * targBlock = (IParamBlock *) CloneRefHierarchy(GetReference( 1+targ ));
		Control * targControl = (Control *)chanBank[targ].cblock->GetController(0);
		morphChannel targBank = chanBank[targ];
		targBank.CopyTargetPercents( chanBank[targ] );
		INode * targNode  = (INode*) GetReference( 101+targ );

		Tab<INode *> progTargs;
		progTargs.ZeroCount();
		progTargs.SetCount( targBank.mNumProgressiveTargs );

		int targRefnum;
		int srcRefnum;
		for(int i=1; i<=targBank.mNumProgressiveTargs; i++) {
			targRefnum = (targ * MAX_PROGRESSIVE_TARGETS)+i+200	;
			progTargs[i-1] = (INode *) GetReference( targRefnum );
		}

		// copy source to target
		//
		DeleteChannel( targ );
		chanBank[targ] = chanBank[src];
		chanBank[targ].CopyTargetPercents( chanBank[src] );

		chanBank[targ].cblock->SetController( 0, chanBank[src].cblock->GetController(0), FALSE);
		
		INode * srcNode  = (INode*) GetReference( 101+src );
		SetReference(101+targ,NULL); // null this out so that the replace reference will work. The code: chanBank[targ] = chanBank[src]; copied the pointer we are setting to where it is being set.
		ReplaceReference(101+targ,srcNode);

		DbgAssert( chanBank[targ].mConnection == chanBank[src].mConnection );
		// make the proper references to the progressive targets
		//
      for(int i=1; i<=chanBank[targ].mNumProgressiveTargs; i++) {
			targRefnum = (targ * MAX_PROGRESSIVE_TARGETS)+i+200	;
			srcRefnum = (src * MAX_PROGRESSIVE_TARGETS)+i+200	;
			SetReference(targRefnum,NULL); // null this out so that the replace reference will work
			ReplaceReference(targRefnum,GetReference( srcRefnum ));
		}


		// copy target to source
		//
		DeleteChannel( src );
		chanBank[src] = targBank;
		chanBank[src].CopyTargetPercents( targBank );

		chanBank[src].cblock->SetController( 0, targControl, FALSE);
		
		SetReference(101+src,NULL); // null this out so that the replace reference will work. The code: chanBank[targ] = chanBank[src]; copied the pointer we are setting to where it is being set.
		ReplaceReference(101+src,targNode);

		DbgAssert( chanBank[src].mConnection == targBank.mConnection );

		// make the proper references to the progressive targets
		//
      for( int i=1; i<=targBank.mNumProgressiveTargs; i++) {
			srcRefnum = (src * MAX_PROGRESSIVE_TARGETS)+i+200	;
			SetReference(srcRefnum,NULL); // null this out so that the replace reference will work
			ReplaceReference(srcRefnum, progTargs[i-1] );
		}

		if(chanBank[src].mConnection)
		{
			chanBank[src].buildFromNode(chanBank[src].mConnection,FALSE,GetCOREInterface()->GetTime());
			for(int i=0; i<chanBank[src].mNumProgressiveTargs; i++) 
			{ 
				chanBank[src].mTargetCache[i].Init(chanBank[src].mTargetCache[i].mTargetINode); 
			}
		}
		if(chanBank[targ].mConnection)
		{
			chanBank[targ].buildFromNode(chanBank[targ].mConnection,FALSE,GetCOREInterface()->GetTime());
			for(int i=0; i<chanBank[targ].mNumProgressiveTargs; i++) 
			{ 
				chanBank[targ].mTargetCache[i].Init(chanBank[targ].mTargetCache[i].mTargetINode); 
			}
		}


	}

	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	NotifyDependents(FOREVER,PART_ALL,REFMSG_SUBANIM_STRUCTURE_CHANGED);

}

float MorphR3::GetCurrentTargetPercent(void) 
{ 
	morphChannel &bank= CurrentChannel();
	if(bank.iTargetListSelection==0) return bank.mTargetPercent;
   return (bank.iTargetListSelection>0 && bank.iTargetListSelection<=bank.mNumProgressiveTargs) ?
      bank.mTargetCache[bank.iTargetListSelection-1].mTargetPercent : 0.0f;
}

void MorphR3::SetCurrentTargetPercent(const float &fval) 
{ 
	morphChannel &bank= CurrentChannel();
	if(bank.iTargetListSelection==0) bank.mTargetPercent = fval;
	else if(bank.iTargetListSelection>0 && bank.iTargetListSelection<=bank.mNumProgressiveTargs )
		bank.mTargetCache[bank.iTargetListSelection-1].mTargetPercent = fval;
}