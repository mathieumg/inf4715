/**********************************************************************
 *<
   FILE: slave.cpp

   DESCRIPTION: A slave controller that is driven by the master and the sub control

   CREATED BY: Peter Watje

   HISTORY: Oct 15, 1998

 *>   Copyright (c) 1998, All Rights Reserved.
 **********************************************************************/
#include "block.h"
#include "units.h"
#include "masterblock.h"
#include "istdplug.h"

#include "3dsmaxport.h"

static SlaveFloatClassDesc slaveFloatCD;
ClassDesc* GetSlaveFloatDesc() {return &slaveFloatCD;}

#ifndef NO_CONTROLLER_SLAVE_POSITION
static SlavePosClassDesc slavePosCD;
ClassDesc* GetSlavePosDesc() {return &slavePosCD;}
#endif

static SlavePoint3ClassDesc slavePoint3CD;
ClassDesc* GetSlavePoint3Desc() {return &slavePoint3CD;}

#ifndef NO_CONTROLLER_SLAVE_ROTATION
static SlaveRotationClassDesc slaveRotationCD;
ClassDesc* GetSlaveRotationDesc() {return &slaveRotationCD;}
#endif

#ifndef NO_CONTROLLER_SLAVE_SCALE
static SlaveScaleClassDesc slaveScaleCD;
ClassDesc* GetSlaveScaleDesc() {return &slaveScaleCD;}
#endif


INT_PTR CALLBACK NewLinkDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK NewMasterDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//------------------------------------------------------------

SlaveControl::SlaveControl() 
   {  
   range     = Interval(GetAnimStart(),GetAnimEnd());
   master = NULL;
   scratchControl = NULL;
   sub = NULL;
   masterPresent = FALSE;
	propBlockID = 0;
	propSubID = 0;
   } 

SlaveControl::~SlaveControl() 
   {  
   int ct = blockID.Count();
   for (int i = 0; i < ct; i++)
      RemoveControl(0);

   if (scratchControl)
      {
      scratchControl->MaybeAutoDelete();
      }
   scratchControl = NULL;
   } 

int SlaveControl::NumSubs() 
   {
   return 0;
   }


void SlaveControl::SetValue(TimeValue t, void *val, int commit, GetSetMethod method)
   {
// sub->SetValue(t,val,commit,method);
   }

void SlaveControl::EnumIKParams(IKEnumCallback &callback)
   {
   if (scratchControl)
      scratchControl->EnumIKParams(callback);
   }

BOOL SlaveControl::CompDeriv(TimeValue t,Matrix3& ptm,IKDeriv& derivs,DWORD flags)
   {
   if (scratchControl)
      return scratchControl->CompDeriv(t,ptm,derivs,flags);
   else return FALSE;
   }

void SlaveControl::MouseCycleCompleted(TimeValue t)
   {
   if (scratchControl)
      scratchControl->MouseCycleCompleted(t);
   }

void SlaveControl::AddNewKey(TimeValue t,DWORD flags)
   {
   if (scratchControl)
      scratchControl->AddNewKey(t,flags);
   }

void SlaveControl::CloneSelectedKeys(BOOL offset)
   {
   if (scratchControl)
      scratchControl->CloneSelectedKeys(offset);
   }

void SlaveControl::DeleteKeys(DWORD flags)
   {
   if (scratchControl)
      scratchControl->DeleteKeys(flags);
   }

void SlaveControl::SelectKeys(TrackHitTab& sel, DWORD flags)
   {
   if (scratchControl)
      scratchControl->SelectKeys(sel,flags);
   }

BOOL SlaveControl::IsKeySelected(int index)
   {
   if (scratchControl)
      return scratchControl->IsKeySelected(index);
   return FALSE;
   }

void SlaveControl::CopyKeysFromTime(TimeValue src,TimeValue dst,DWORD flags)
   {
   if (scratchControl)
      scratchControl->CopyKeysFromTime(src,dst,flags);

   }

void SlaveControl::DeleteKeyAtTime(TimeValue t)
   {
   if (scratchControl)
      scratchControl->DeleteKeyAtTime(t);
   }

BOOL SlaveControl::IsKeyAtTime(TimeValue t,DWORD flags)
   {
   if (scratchControl)
      return scratchControl->IsKeyAtTime(t,flags);
   return FALSE;
   }

BOOL SlaveControl::GetNextKeyTime(TimeValue t,DWORD flags,TimeValue &nt)
   {
   if (scratchControl)
      return scratchControl->GetNextKeyTime(t,flags,nt);
   return FALSE;
   }

int SlaveControl::GetKeyTimes(Tab<TimeValue> &times,Interval range,DWORD flags)
   {
   if (scratchControl)
      return scratchControl->GetKeyTimes(times,range,flags);
   return 0;
   }

int SlaveControl::GetKeySelState(BitArray &sel,Interval range,DWORD flags)
   {
   if (scratchControl)
      return scratchControl->GetKeySelState(sel,range,flags);
   return 0;
   }




Animatable* SlaveControl::SubAnim(int i) 
   {
      return NULL;
   }


TSTR SlaveControl::SubAnimName(int i) 
   {
   return GetString(IDS_PW_SUB);
   }

BOOL SlaveControl::AssignController(Animatable *control,int subAnim) 
   {
   return FALSE;
   }


int SlaveControl::NumRefs() 
   {
   return 2;
   }

RefTargetHandle SlaveControl::GetReference(int i) 
   {
	DbgAssert( i >= 0);
	DbgAssert( i < NumRefs());
	if (i==0) 
		return (RefTargetHandle) sub;
	else if (i==1) 
		return (RefTargetHandle) master;
   else
      {
//    DebugPrint(_T("get reference error occurred\n"));
      return NULL;
      }
   }

void SlaveControl::SetReference(int i, RefTargetHandle rtarg) 
   {
	DbgAssert( i >= 0);
	DbgAssert( i < NumRefs());
	if (i==0) 
		sub = (Control*) rtarg;
   else if (i==1) 
      {
      if ((rtarg == NULL) && (master))
         {
//tell the master that I am being removed
         int ct = blockID.Count();
         for (int i = 0; i < ct; i++)
            RemoveControl(0);
         }

      master = (MasterBlockControl*) rtarg;
		if (master == NULL) 
			masterPresent = FALSE;
		else 
			masterPresent = TRUE;
      }
	else
		DebugPrint(_T("set reference error occurred\n"));
   }


void SlaveControl::Copy(Control *from)
   {
   if ( from->CanCopyTrack(FOREVER,0) )
      ReplaceReference(0,from);
   superID = from->SuperClassID();
   }

RefResult SlaveControl::NotifyRefChanged(
      Interval changeInt, 
      RefTargetHandle hTarget, 
      PartID& partID,  
      RefMessage message)
   {
   switch (message) {

      case REFMSG_TARGET_DELETED:
         if (hTarget == master) {
            masterPresent = FALSE;
            }

         break;
      case REFMSG_CHANGE:
         break;


      }
   return REF_SUCCEED;
   }



class SlaveRangeRestore : public RestoreObj {
   public:
      SlaveControl *cont;
      Interval ur, rr;
      SlaveRangeRestore(SlaveControl *c) 
         {
         cont = c;
         ur   = cont->range;
         }        
      void Restore(int isUndo) 
         {
         rr = cont->range;
         cont->range = ur;
         cont->NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
         }
      void Redo()
         {
         cont->range = rr;
         cont->NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
         }     
      void EndHold() 
         { 
         cont->ClearAFlag(A_HELD);
         }
      TSTR Description() { return _T("Slave control range"); }
   };


void SlaveControl::HoldRange()
   {
   if (theHold.Holding() && !TestAFlag(A_HELD)) {
      SetAFlag(A_HELD);
      theHold.Put(new SlaveRangeRestore(this));
      }
   }

void SlaveControl::EditTimeRange(Interval range,DWORD flags)
   {

   }

void SlaveControl::MapKeys(TimeMap *map,DWORD flags)
   {

   }
void SlaveControl::RemoveControl(int sel)

{
if (master)
   {
   if (blockID[sel] < master->Blocks.Count() )
      {
      for (int i = 0; i <master->Blocks[blockID[sel]]->externalBackPointers.Count(); i++)
         { 
         if ( this == master->Blocks[blockID[sel]]->externalBackPointers[i])
            {
            master->Blocks[blockID[sel]]->externalBackPointers.Delete(i,1);
            i--;
            }
         }
      for (int i = 0; i <master->Blocks[blockID[sel]]->backPointers.Count(); i++)
         { 
         if ( this == master->Blocks[blockID[sel]]->backPointers[i])
            {
            master->Blocks[blockID[sel]]->backPointers[i]= NULL;
            }
         }
      }
   }
blockID.Delete(sel,1);
subID.Delete(sel,1);

}

void SlaveControl::AddControl(int blockid,int subid )
{
	DbgAssert(master);
	if (!master)
		return;
		
blockID.Append(1,&blockid,1);
subID.Append(1,&subid,1);
SlaveControl *sl = this;
master->Blocks[blockid]->externalBackPointers.Append(1,&sl,1);
if (sub == NULL)
   {
   ReplaceReference(0,CloneRefHierarchy(master->Blocks[blockid]->controls[subid]));
   }
}

void SlaveControl::CollapseControl()
{
#define ID_TV_GETSELECTED  680

	MaxSDK::Array<TrackViewPick> res;

SendMessage(trackHWND,WM_COMMAND,ID_TV_GETSELECTED,(LPARAM)&res);
	if (res.length() == 1)
   {
   if (masterPresent)
      {
      Control *mc = (Control *) CloneRefHierarchy(master->blendControl);

      for (int ct = 0; ct < scratchControl->NumMultCurves(); ct++)
         scratchControl->DeleteMultCurve(ct);
      scratchControl->AppendMultCurve(mc);
      }
   int ct = blockID.Count();
   for (int i = 0; i < ct; i++)
      RemoveControl(0);

   res[0].client->AssignController(CloneRefHierarchy(scratchControl),res[0].subNum);

   NotifyDependents(FOREVER,0,REFMSG_CHANGE);
   NotifyDependents(FOREVER,0,REFMSG_SUBANIM_STRUCTURE_CHANGED);
   }
}


#define COUNT_CHUNK     0x01010
#define DATA_CHUNK      0x01020


IOResult SlaveControl::Save(ISave *isave)
   {     
   ULONG nb;   
//count
   int count = blockID.Count();
   isave->BeginChunk(COUNT_CHUNK);
   isave->Write(&count,sizeof(count),&nb);         
   isave->EndChunk();
//id data 
   for (int i =0; i < count; i++)
      {
      isave->BeginChunk(DATA_CHUNK);
      isave->Write(&blockID[i],sizeof(int),&nb);         
      isave->Write(&subID[i],sizeof(int),&nb);        
      isave->EndChunk();
      }
   return IO_OK;
   }

IOResult SlaveControl::Load(ILoad *iload)
   {
   int ID =  0;
   ULONG nb;
   IOResult res = IO_OK;
   int ix = 0;
   while (IO_OK==(res=iload->OpenChunk())) 
      {
      ID = iload->CurChunkID();
      if (ID ==COUNT_CHUNK)
         {
         int ct;
         iload->Read(&ct, sizeof(ct), &nb);
         blockID.SetCount(ct);
         subID.SetCount(ct);
         }
      else if (ID == DATA_CHUNK)
         {
         int bID,sID;
         iload->Read(&bID, sizeof(int), &nb);
         iload->Read(&sID, sizeof(int), &nb);
         blockID[ix] = bID;
         subID[ix++] = sID;
         }
      iload->CloseChunk();
      if (res!=IO_OK)  return res;
      }

//rebuild all tempcontrols 
   return IO_OK;
   }


//--------------------------------------------------------------------


RefTargetHandle SlaveControl::Clone(RemapDir& remap)
   {
   SlaveControl *cont = new SlaveControl;
   cont->sub = NULL;
   cont->master = NULL;
   cont->scratchControl = NULL;

   cont->ReplaceReference(0,sub);
   cont->ReplaceReference(1,master);
   cont->blockID = blockID;
   cont->subID = subID;
   cont->masterPresent = masterPresent;

   CloneControl(cont,remap);
   cont->UpdateSlave();
   BaseClone(this, cont, remap);
   return cont;
   }


void SlaveControl::GetValue(TimeValue t, void *val, Interval &valid, GetSetMethod method)
{
}

//------------------------------------------------------------
//Slave Float Controller
//------------------------------------------------------------

RefTargetHandle SlaveFloatControl::Clone(RemapDir& remap)
   {
   SlaveFloatControl *cont = new SlaveFloatControl;
   cont->sub = NULL;
   cont->master = NULL;
   cont->scratchControl = NULL;

   cont->ReplaceReference(0,sub);
   cont->ReplaceReference(1,master);
   cont->blockID = blockID;
   cont->subID = subID;
   cont->masterPresent = masterPresent;
   if (master)
      {
      for (int i = 0; i < blockID.Count(); i++)
         {
         SlaveControl *c = (SlaveControl *)cont;
         master->Blocks[blockID[i]]->externalBackPointers.Append(1,&c,1);
         }
      }
   CloneControl(cont,remap);
   cont->UpdateSlave();
   BaseClone(this, cont, remap);
   return cont;
   }

SlaveFloatControl::SlaveFloatControl() 
   {  

   } 

void SlaveFloatControl::UpdateSlave()
{
   if (scratchControl == NULL && sub)
      scratchControl = (Control *) CloneRefHierarchy(sub);

   if (scratchControl == NULL)
      return;

   scratchControl->DeleteKeys(TRACK_DOALL);
   float f = 0.0f;
   scratchControl->SetValue(0,&f);

   if (master)
      master->Update(scratchControl,blockID,subID);
}

void SlaveFloatControl::GetValue(
      TimeValue t, void *val, Interval &valid, GetSetMethod method)
   {
   if ( (sub == NULL) || (!masterPresent) || (blockID.Count()==0))
      {
      if (method == CTRL_ABSOLUTE)
         {
         float *v = ((float*)val);
         *v = 0.0f;
         }
      return;
      }

   if (scratchControl == NULL)
      {
      UpdateSlave();
      }
   float *tv = ((float*)val);

   if (master)
	{
		// should we be passing the float tv here? or val?
		master->GetValue3(scratchControl,t,tv,valid,blockID,subID,range,method);
	}
   }


#ifndef NO_CONTROLLER_SLAVE_POSITION
//------------------------------------------------------------
//Slave Pos Controller
//------------------------------------------------------------

RefTargetHandle SlavePosControl::Clone(RemapDir& remap)
   {
   SlavePosControl *cont = new SlavePosControl;
   cont->sub = NULL;
   cont->master = NULL;
   cont->scratchControl = NULL;
   cont->ReplaceReference(0,sub);
   cont->ReplaceReference(1,master);
   cont->blockID = blockID;
   cont->subID = subID;
   cont->masterPresent = masterPresent;
   if (master)
      {
      for (int i = 0; i < blockID.Count(); i++)
         {
         SlaveControl *c = (SlaveControl *)cont;
         master->Blocks[blockID[i]]->externalBackPointers.Append(1,&c,1);
         }
      }
   CloneControl(cont,remap);
   cont->UpdateSlave();
   BaseClone(this, cont, remap);
   return cont;
   }

SlavePosControl::SlavePosControl() 
   {  

   } 


void SlavePosControl::UpdateSlave()
{
   // CAL-06/03/02: sub could be NULL
   if (scratchControl == NULL && sub)
      scratchControl = (Control *) CloneRefHierarchy(sub);

   if (scratchControl == NULL)
      return;

   scratchControl->DeleteKeys(TRACK_DOALL);
   Point3 f(0.0f,0.f,0.0f);
   scratchControl->SetValue(0,&f);

   if (master)
      master->Update(scratchControl,blockID,subID);
}



void SlavePosControl::GetValue(
      TimeValue t, void *val, Interval &valid, GetSetMethod method)
   {
   if ( (sub == NULL) || (!masterPresent) || (blockID.Count()==0))
      {
      if (method == CTRL_ABSOLUTE)
         {
         Point3 *v = ((Point3*)val);
         *v = Point3(0.0f,0.0f,0.0f);
         }
      else
         {
         Point3 f(0.0f,0.0f,0.0f);
         Matrix3 *v = ((Matrix3*)val);
         v->PreTranslate(f);
         
         }

      return;
      }
//copy keys into scratch control
   if (scratchControl == NULL)
      {
      UpdateSlave();
      }

   if (master)
      master->GetValue3(scratchControl,t,val,valid,blockID,subID,range,method);
   }
#endif // NO_CONTROLLER_SLAVE_POSITION

//------------------------------------------------------------
//Slave Point3 Controller
//------------------------------------------------------------

RefTargetHandle SlavePoint3Control::Clone(RemapDir& remap)
   {
   SlavePoint3Control *cont = new SlavePoint3Control;
// *cont = *this;
   cont->sub = NULL;
   cont->master = NULL;
   cont->scratchControl = NULL;

   cont->ReplaceReference(0,sub);
   cont->ReplaceReference(1,master);
   cont->blockID = blockID;
   cont->subID = subID;
   cont->masterPresent = masterPresent;
   if (master)
      {
      for (int i = 0; i < blockID.Count(); i++)
         {
         SlaveControl *c = (SlaveControl *)cont;
         master->Blocks[blockID[i]]->externalBackPointers.Append(1,&c,1);
         }
      }
   CloneControl(cont,remap);
   cont->UpdateSlave();
   BaseClone(this, cont, remap);
   return cont;
   }

SlavePoint3Control::SlavePoint3Control() 
   {  

   } 


void SlavePoint3Control::UpdateSlave()
{
   if (scratchControl == NULL && sub)
      scratchControl = (Control *) CloneRefHierarchy(sub);

   if (scratchControl == NULL)
      return;

   scratchControl->DeleteKeys(TRACK_DOALL);
   Point3 f(0.0f,0.f,0.0f);
   scratchControl->SetValue(0,&f);

   if (master)
      master->Update(scratchControl,blockID,subID);
}

void SlavePoint3Control::GetValue(
      TimeValue t, void *val, Interval &valid, GetSetMethod method)
   {
   if ( (sub == NULL) || (!masterPresent) || (blockID.Count()==0))
      {
      Point3 *v = ((Point3*)val);
      *v = Point3(0.0f,0.0f,0.0f);
      return;
      }
//copy keys into scratch control

   if (scratchControl == NULL)
      {
      UpdateSlave();
      }

   if (master)
      master->GetValue3(scratchControl,t,val,valid,blockID,subID,range,method);
   }



#ifndef NO_CONTROLLER_SLAVE_ROTATION
//------------------------------------------------------------
//Slave rotation Controller
//------------------------------------------------------------

RefTargetHandle SlaveRotationControl::Clone(RemapDir& remap)
   {
   SlaveRotationControl *cont = new SlaveRotationControl;
// *cont = *this;
   cont->sub = NULL;
   cont->master = NULL;
   cont->scratchControl = NULL;

   cont->ReplaceReference(0,sub);
   cont->ReplaceReference(1,master);
   cont->blockID = blockID;
   cont->subID = subID;
   cont->masterPresent = masterPresent;
   if (master)
      {
      for (int i = 0; i < blockID.Count(); i++)
         {
         SlaveControl *c = (SlaveControl *)cont;
         master->Blocks[blockID[i]]->externalBackPointers.Append(1,&c,1);
         }
      }
   CloneControl(cont,remap);
   cont->UpdateSlave();
   BaseClone(this, cont, remap);
   return cont;
   }


SlaveRotationControl::SlaveRotationControl() 
   {  

   } 


void SlaveRotationControl::UpdateSlave()
{
   // CAL-06/03/02: sub could be NULL
   if (scratchControl == NULL && sub)
      scratchControl = (Control *) CloneRefHierarchy(sub);

   if (scratchControl == NULL)
      return;

   scratchControl->DeleteKeys(TRACK_DOALL);
   Quat f;
   f.Identity();
   scratchControl->SetValue(0,&f);

   if (master)
      master->Update(scratchControl,blockID,subID);
}

void SlaveRotationControl::GetValue(TimeValue t, void *val, Interval &valid, GetSetMethod method)
   {
   if ( (sub == NULL) || (!masterPresent) || (blockID.Count()==0))
      {
      Quat f;
      f.Identity();
      if (method == CTRL_ABSOLUTE)
         {
         Quat *v = ((Quat*)val);
         *v = f;
         return;
         }
      else
         {
         Matrix3 *v = ((Matrix3*)val);
         PreRotateMatrix(*v,f);
         return;           
         }
      }

//copy keys into scratch control
   if (scratchControl == NULL)
      {
      UpdateSlave();
      }

   if (master)
      master->GetValue3(scratchControl,t,val,valid,blockID,subID,range,method);
   }
#endif // NO_CONTROLLER_SLAVE_ROTATION

#ifndef NO_CONTROLLER_SLAVE_SCALE
//------------------------------------------------------------
//Slave Scale Controller
//------------------------------------------------------------

RefTargetHandle SlaveScaleControl::Clone(RemapDir& remap)
   {
   SlaveScaleControl *cont = new SlaveScaleControl;
// *cont = *this;
   cont->sub = NULL;
   cont->master = NULL;
   cont->scratchControl = NULL;

   cont->ReplaceReference(0,sub);
   cont->ReplaceReference(1,master);
   cont->blockID = blockID;
   cont->subID = subID;
   cont->masterPresent = masterPresent;
   if (master)
      {
      for (int i = 0; i < blockID.Count(); i++)
         {
         SlaveControl *c = (SlaveControl *)cont;
         master->Blocks[blockID[i]]->externalBackPointers.Append(1,&c,1);
         }
      }
   CloneControl(cont,remap);
   cont->UpdateSlave();
   BaseClone(this, cont, remap);
   return cont;
   }

SlaveScaleControl::SlaveScaleControl() 
   {  

   } 

void SlaveScaleControl::UpdateSlave()
{
   if (scratchControl == NULL && sub)
      scratchControl = (Control *) CloneRefHierarchy(sub);

   if (scratchControl == NULL)
      return;

   scratchControl->DeleteKeys(TRACK_DOALL);

   Quat f;
   f.Identity();
   Point3 p(1.0f,1.0f,1.0f);
   ScaleValue s(p,f); 

   scratchControl->SetValue(0,&s);

   if (master)
      master->Update(scratchControl,blockID,subID);
}

void SlaveScaleControl::GetValue(TimeValue t, void *val, Interval &valid, GetSetMethod method)
   {
   if ( (sub == NULL) || (!masterPresent) || (blockID.Count()==0))
      {
      Quat f;
      f.Identity();
      Point3 p(1.0f,1.0f,1.0f);
      if (method == CTRL_ABSOLUTE)
         {
         ScaleValue s(p,f); 
         ScaleValue *v = ((ScaleValue*)val);
         *v = s;
         return;
         }
      else
         {
         Matrix3 *mat = (Matrix3*)val;
         ScaleValue s(p,f); 
         ApplyScaling(*mat,s);
         return;           
         }

      }

   if (scratchControl == NULL)
      UpdateSlave();


   if (master)
      master->GetValue3(scratchControl,t,val,valid,blockID,subID,range,method);

   }
#endif // NO_CONTROLLER_SLAVE_SCALE


static INT_PTR CALLBACK SlaveDlgProc(
      HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void SlaveControl::EditTrackParams(
      TimeValue t,
      ParamDimensionBase *dim,
      const TCHAR *pname,
      HWND hParent,
      IObjParam *ip,
      DWORD flags)
   {

   if (flags & EDITTRACK_BUTTON)
      {
      trackHWND = hParent;
		// Display the slave dialog.
		// Make this stack based, so as not to confuse everyone about who deletes the memory
		SlaveDlg the_dlg(this,dim,pname,ip,hParent);
      }
   }

SlaveDlg::SlaveDlg(
      SlaveControl *cont,
      ParamDimensionBase *dim,
      const TCHAR *pname,
      IObjParam *ip,
      HWND hParent)
   {
   this->cont = NULL;
   this->ip   = ip;
   this->dim  = dim;
   valid = FALSE;

	HoldSuspend hs;
   ReplaceReference(0,cont);
	hs.Resume();
	// Create a modal dialog box that returns when the dialog is closed
	DialogBoxParam(hInstance,MAKEINTRESOURCE(IDD_SLAVEPARAMS), hParent,SlaveDlgProc,(LPARAM)this);
   }

SlaveDlg::~SlaveDlg()
   {
	HoldSuspend hs;
   DeleteAllRefsFromMe();
   }

void SlaveDlg::Invalidate()
   {
   valid = FALSE;
   }

void SlaveDlg::Update()
   {
   if (!valid && hWnd) {
      valid = TRUE;
      }
   }

void SlaveDlg::SetupUI(HWND hWnd)
   {
   this->hWnd = hWnd;

   SetupList();

   valid = FALSE;
   Update();
   }

void SlaveDlg::SetupList()
   {
//loop through list getting names
//nuke old lis
	SendMessage(GetDlgItem(hWnd,IDC_LIST1), LB_RESETCONTENT,0,0);
	SendMessage(GetDlgItem(hWnd,IDC_LIST1), LB_SETCOUNT,0,0);
   if (cont->masterPresent)
      {
      for (int i=0; i<cont->blockID.Count(); i++) 
         {
         int id = cont->blockID[i];
         int subid = cont->subID[i];
         if (id < cont->master->Blocks.Count())
            {
            TSTR name = cont->master->Blocks[id]->SubAnimName(subid);
            SendMessage(GetDlgItem(hWnd,IDC_LIST1),
               LB_ADDSTRING,0,(LPARAM)(const TCHAR*)name);
            }
      }
      }
   }

void SlaveDlg::SetButtonStates()
   {
   int sel = SendMessage(GetDlgItem(hWnd,IDC_LIST1),
         LB_GETCURSEL,0,0);
   if (sel!=LB_ERR) {
      if ((cont->blockID.Count() == 0) || (cont->subID.Count()==0))
         {
         EnableWindow(GetDlgItem(hWnd,IDC_REMOVE),FALSE);
         EnableWindow(GetDlgItem(hWnd,IDC_COLLAPSE),FALSE);
         }
      else {
         EnableWindow(GetDlgItem(hWnd,IDC_REMOVE),TRUE);
         EnableWindow(GetDlgItem(hWnd,IDC_COLLAPSE),TRUE);
         }
      }
   else
      {
      EnableWindow(GetDlgItem(hWnd,IDC_REMOVE),FALSE);
      EnableWindow(GetDlgItem(hWnd,IDC_COLLAPSE),FALSE);
      }
   }

BOOL MasterTrackViewFilter :: proc(Animatable *anim, Animatable *client,int subNum)
{
//make sure the parent is not a slave or 
if ( anim->ClassID() ==MASTERBLOCK_CONTROL_CLASS_ID)
   return TRUE;
return FALSE;
}


void SlaveDlg::WMCommand(int id, int notify, HWND hCtrl)
   {
	switch (id)
	{
	case IDC_LIST_NAME:
		{

         int sel = SendMessage(GetDlgItem(hWnd,IDC_LIST1),
            LB_GETCURSEL,0,0);
         if (sel!=LB_ERR) {
            if ((cont->blockID.Count() == 0) || (cont->subID.Count()==0))
               {
               EnableWindow(GetDlgItem(hWnd,IDC_REMOVE),FALSE);
               EnableWindow(GetDlgItem(hWnd,IDC_COLLAPSE),FALSE);
               }
            else {
               EnableWindow(GetDlgItem(hWnd,IDC_REMOVE),TRUE);
               EnableWindow(GetDlgItem(hWnd,IDC_COLLAPSE),TRUE);
               }
            }

         break;
         }
      case IDC_LIST1:
         if (notify==LBN_SELCHANGE) {
            SetButtonStates();            
            }

         break;
      case IDC_LINK:
         {
         if (!cont->masterPresent)
            {
            MasterTrackViewFilter filter;
            TrackViewPick res;
            BOOL MasterOK = GetCOREInterface()->TrackViewPickDlg(hWnd,&res,&filter);
            if (MasterOK && (res.anim != NULL))
               {
               cont->ReplaceReference(1,res.anim,FALSE);
               cont->propBlockID = -1;
               cont->propSubID = -1;

               int OK = DialogBoxParam  (hInstance, MAKEINTRESOURCE(IDD_ADDNEWLINK),
                  hWnd, NewLinkDlgProc, (LPARAM)cont);
         
               if ((OK) && (cont->propSubID != -1) && (cont->propSubID != -1))
                  {
                  cont->AddControl(cont->propBlockID,cont->propSubID);
                  SetupList();
                  }

               }
            int sel = SendMessage(GetDlgItem(hWnd,IDC_LIST1),
               LB_GETCURSEL,0,0);

            if ((cont->blockID.Count() == 0) || (cont->subID.Count()==0))
               {
               EnableWindow(GetDlgItem(hWnd,IDC_REMOVE),FALSE);
               EnableWindow(GetDlgItem(hWnd,IDC_COLLAPSE),FALSE);
                  
               }
            else {
               if (sel!=LB_ERR)
                  {
                  EnableWindow(GetDlgItem(hWnd,IDC_REMOVE),TRUE);
                  EnableWindow(GetDlgItem(hWnd,IDC_COLLAPSE),TRUE);
                  }
               }

            }
         else
            {
            int OK = DialogBoxParam  (hInstance, MAKEINTRESOURCE(IDD_ADDNEWLINK),
               hWnd, NewLinkDlgProc, (LPARAM)cont);
         
            if ( (OK)  && (cont->propSubID != -1) && (cont->propSubID != -1))
               {
               cont->AddControl(cont->propBlockID,cont->propSubID);
               SetupList();
               }
            }
         Change(TRUE);
         break;
         }
      case IDC_REMOVE:
         {
         int sel = SendMessage(GetDlgItem(hWnd,IDC_LIST1),
            LB_GETCURSEL,0,0);
         cont->RemoveControl(sel);
         SendMessage(GetDlgItem(hWnd,IDC_LIST1),
            LB_DELETESTRING,sel,0);
         SetupList();
         sel = SendMessage(GetDlgItem(hWnd,IDC_LIST1),
            LB_GETCURSEL,0,0);

         if ((cont->blockID.Count() == 0) || (cont->subID.Count()==0))
            {
            EnableWindow(GetDlgItem(hWnd,IDC_REMOVE),FALSE);
            EnableWindow(GetDlgItem(hWnd,IDC_COLLAPSE),FALSE);
            }
         else {
            if (sel!=LB_ERR)
               {
               EnableWindow(GetDlgItem(hWnd,IDC_REMOVE),TRUE);
               EnableWindow(GetDlgItem(hWnd,IDC_COLLAPSE),TRUE);
               }
            }

         Change(TRUE);

         break;
         }
      case IDC_COLLAPSE:
         {
			SendMessage(GetDlgItem(hWnd,IDC_LIST1), LB_GETCURSEL,0,0);
         cont->CollapseControl();
         EndDialog(hWnd,1);
         break;
         }
      case IDOK:
         EndDialog(hWnd,1);
         break;
      case IDCANCEL:
         EndDialog(hWnd,0);
         break;
      }
   }


void SlaveDlg::Change(BOOL redraw)
   {
   cont->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
   UpdateWindow(GetParent(hWnd));   
	if (redraw) 
		ip->RedrawViews(ip->GetTime());
   }


class CheckForNonSlaveDlg : public DependentEnumProc {
	public:     
	   BOOL non;
	   ReferenceMaker *me;
	CheckForNonSlaveDlg(ReferenceMaker *m)
		: non(FALSE)
		, me(m)
	{ }
	int proc(ReferenceMaker *rmaker)
	{
		   if (rmaker==me) return DEP_ENUM_CONTINUE;
		   if (rmaker->SuperClassID()!=REF_MAKER_CLASS_ID &&
			   rmaker->ClassID()!=Class_ID(SLAVEDLG_CLASS_ID,0)) {
				   non = TRUE;
				   return DEP_ENUM_HALT;
		   }
		   return DEP_ENUM_SKIP; // just look at direct dependents
	   }
   };

void SlaveDlg::MaybeCloseWindow()
   {
   CheckForNonSlaveDlg check(cont);
   cont->DoEnumDependents(&check);
   if (!check.non) {
      PostMessage(hWnd,WM_CLOSE,0,0);
      }
   }



RefResult SlaveDlg::NotifyRefChanged(
	Interval /*changeInt*/, 
      RefTargetHandle hTarget, 
      PartID& partID,  
      RefMessage message)
   {
   switch (message) {
      case REFMSG_CHANGE:
         Invalidate();        
         break;
      
      case REFMSG_REF_DELETED:
         MaybeCloseWindow();
         break;

	case REFMSG_TARGET_DELETED:
		MaybeCloseWindow();
		cont = NULL;
		break;
      }
   return REF_SUCCEED;
   }

static INT_PTR CALLBACK SlaveDlgProc(
      HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
   {
   SlaveDlg *dlg = DLGetWindowLongPtr<SlaveDlg*>(hWnd);

   switch (msg) {
      case WM_INITDIALOG:
         {
         dlg = (SlaveDlg*)lParam;
         DLSetWindowLongPtr(hWnd, lParam);
         dlg->SetupUI(hWnd);
         SendMessage(GetDlgItem(hWnd,IDC_LIST1),
            LB_SETCURSEL,0,0);
         int sel = SendMessage(GetDlgItem(hWnd,IDC_LIST1),
            LB_GETCURSEL,0,0);
         if (sel==-1) 
            {
            EnableWindow(GetDlgItem(hWnd,IDC_REMOVE),FALSE);
            EnableWindow(GetDlgItem(hWnd,IDC_COLLAPSE),FALSE);
            }

         break;

         }
      case WM_COMMAND:
         dlg->WMCommand(LOWORD(wParam),HIWORD(wParam),(HWND)lParam);                
         break;

      case WM_PAINT:
         dlg->Update();
         return 0;         
      
      case WM_CLOSE:
         DestroyWindow(hWnd);       
         break;

      case WM_DESTROY:                 
		// delete dlg; // This is stack based, and there is no reason to destroy it here
         break;
   
      case CC_COLOR_BUTTONDOWN:
         theHold.Begin();
         break;
      case CC_COLOR_BUTTONUP:
         if (HIWORD(wParam)) theHold.Accept(GetString(IDS_DS_PARAMCHG));
         else theHold.Cancel();
         break;
      case CC_COLOR_CHANGE: {
		// Explanation of what wParam and lParam is below
		// int i = LOWORD(wParam);
		// IColorSwatch *cs = (IColorSwatch*)lParam;
		int sel = SendMessage(GetDlgItem(hWnd,IDC_LIST1), LB_GETCURSEL,0,0);
         if (sel != -1)
            {
            if (HIWORD(wParam)) theHold.Begin();
            if (HIWORD(wParam)) {
               theHold.Accept(GetString(IDS_DS_PARAMCHG));
               }
            }
         break;
      }

      
      default:
         return FALSE;
      }
   return TRUE;
   }





INT_PTR CALLBACK NewLinkDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
   SlaveControl *slv = DLGetWindowLongPtr<SlaveControl*>(hWnd);

   static Tab<int> sid,bid;

   switch (msg) {
   case WM_INITDIALOG:
      {
      sid.ZeroCount();
      bid.ZeroCount();
      slv = (SlaveControl*)lParam;
      DLSetWindowLongPtr(hWnd, lParam);

//goto master look at all sub block
      int count = slv->master->Blocks.Count();
			for (int i = 0; i < count;i++)
         {
         TSTR blockName = slv->master->SubAnimName(i+1);
         for (int j = 0;j < slv->master->Blocks[i]->controls.Count();j++)
            {
            TSTR subName = slv->master->Blocks[i]->SubAnimName(j);
            TSTR finalName = blockName + _T(" ") + subName;
//check if control is the same as ours
            if (slv->sub == NULL)
               {
               if (slv->master->Blocks[i]->controls[j]->SuperClassID() == slv->SuperClassID())  
                  {
//add to list box
                  sid.Append(1,&j,1);
                  bid.Append(1,&i,1);
							SendMessage(GetDlgItem(hWnd,IDC_LIST1), LB_ADDSTRING,0,(LPARAM)(const TCHAR*)finalName);
                  }
               }
            else if (slv->master->Blocks[i]->controls[j]->ClassID() == slv->sub->ClassID() )  
               {
//add to list box
               sid.Append(1,&j,1);
               bid.Append(1,&i,1);
               SendMessage(GetDlgItem(hWnd,IDC_LIST1),
                  LB_ADDSTRING,0,(LPARAM)(const TCHAR*)finalName);
               }
            }
         }  

			SendMessage(GetDlgItem(hWnd,IDC_LIST1), LB_SETCURSEL,0,0);
      CenterWindow(hWnd,GetParent(hWnd));
      break;
      }
      


   case WM_COMMAND:
      switch (LOWORD(wParam)) {
      case IDOK:
         {
         int sel = SendMessage(GetDlgItem(hWnd,IDC_LIST1),
                  LB_GETCURSEL,0,0);
         if (sel >=0)
            {
            slv->propBlockID = bid[sel];
            slv->propSubID = sid[sel];
					slv->NotifyDependents(FOREVER, (PartID)PART_ALL, REFMSG_CHANGE);
            }

         EndDialog(hWnd,1);
         break;
         }
      case IDCANCEL:
         EndDialog(hWnd,0);
         break;
      }
      break;

   default:
      return FALSE;
   }
   return TRUE;
}

