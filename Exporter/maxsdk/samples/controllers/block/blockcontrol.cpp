#include "block.h"
#include "units.h"
#include "masterblock.h"
#include "istdplug.h"

#include "iparamm2.h"

#include "3dsmaxport.h"

static BlockControlClassDesc blockControlCD;
ClassDesc* GetBlockControlDesc() {return &blockControlCD;}

INT_PTR CALLBACK BlockPropDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK TrackPropDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//------------------------------------------------------------

BlockControl::BlockControl() 
   {  
   range     = Interval(GetAnimStart(),GetAnimEnd());
	trackHWND = NULL;
   suspendNotifies = FALSE;
	this->start = 0;
	this->m_end = 0;
	l = 0;
	propStart = 0;
}

BlockControl::~BlockControl() 
   {  
   HoldSuspend hs;
   DeleteAllRefs();
   for (int i = 0; i < tempControls.Count(); i++)
      {
      if (tempControls[i] != NULL)
        {
        RefResult res = tempControls[i]->MaybeAutoDelete();
        DbgAssert(REF_SUCCEED == res);
        tempControls[i] = NULL;
        }
      }
   } 


int BlockControl::NumSubs() 
   {
   return controls.Count();
   }

Animatable* BlockControl::SubAnim(int i) 
   {
   if (i < controls.Count()) return controls[i];
   return NULL;
   }

TSTR BlockControl::SubAnimName(int i) 
   {
   TSTR name;
   if (i < names.Count())
      {
      if (names[i] && names[i]->length()) 
         {
         name = *names[i];
         }
      } 
   else if (controls[i]) 
         {
         controls[i]->GetClassName(name);
         } 

   return name;
   }


BOOL BlockControl::AssignController(Animatable *control,int subAnim) 
   {
   
   MessageBox(  GetCOREInterface()->GetMAXHWnd(),          // handle of owner window
                (LPCTSTR) GetString(IDS_PW_ERROR_MSG),     // address of text in message box
               (LPCTSTR) NULL,  // address of title of message box
                  MB_OK | MB_ICONWARNING | MB_APPLMODAL );         // style of message box);
   return FALSE;
   }

int BlockControl::NumRefs() 
   {
//DebugPrint(_T("Block Num ref %d\n"),controls.Count());
   return controls.Count();
   }

RefTargetHandle BlockControl::GetReference(int i) 
   {
	DbgAssert(i >= 0);
	DbgAssert(i < controls.Count());
   if (i < controls.Count()) 
      {
//    DebugPrint(_T("Getting block control ref %d\n"),i);
      return controls[i];
      }
   return NULL;
   }

void BlockControl::SetReference(int i, RefTargetHandle rtarg) 
   {
	DbgAssert(i >= 0);
	DbgAssert(i <= controls.Count());
// DebugPrint(_T("Setting block control ref %d\n"),i);
   if (i==controls.Count() && rtarg) {
      controls.Resize(controls.Count()+1);
      }
   if (i==controls.Count() && !rtarg) {
      return;
      }

   controls[i] = (Control*)rtarg;

   }

void BlockControl::NotifySlaves()
{
if (!suspendNotifies)
   {

   int i, count;
   count = backPointers.Count();

   for (i=0; i<count; i++) 
      {
      if ( (backPointers[i]) && (backPointers[i]->master))
         {
         backPointers[i]->UpdateSlave();
         backPointers[i]->NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
         }
      }
   count = externalBackPointers.Count();
   for (i=0; i<count; i++) 
      {
      if (externalBackPointers[i])
         {
         externalBackPointers[i]->UpdateSlave();
         externalBackPointers[i]->NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
         }
      }
   }
}

RefResult BlockControl::NotifyRefChanged(
      Interval changeInt, 
      RefTargetHandle hTarget, 
      PartID& partID,  
      RefMessage message)
   {
   switch (message) {
      case REFMSG_CHANGE:
         NotifySlaves();
         break;
      }
   return REF_SUCCEED;
   }


RefTargetHandle BlockControl::Clone(RemapDir& remap)
   {
   BaseClone(this, NULL, remap);
   return NULL;
   }

void BlockControl::GetValue(
      TimeValue t, void *val, Interval &valid, GetSetMethod method)
   {
//DebugPrint(_T("Error occured this getValue should never be called on a block control\n"));
   }

void BlockControl::GetValue(
      TimeValue t, void *val, Interval &valid, int whichSub, GetSetMethod method)
   {
//DebugPrint(_T("Error occured this getValue should never be called on a block control\n"));
   controls[whichSub]->GetValue(t,val,valid,method);
   }

void BlockControl::SetValue(
      TimeValue t, void *val, int commit, GetSetMethod method)
   {
//DebugPrint(_T("Error occured SetValue should never be called on a block control\n"));
   }


void BlockControl::RebuildTempControl()
{
for (int i =0; i < controls.Count();i++)
   {
   if (tempControls[i] == NULL)
      tempControls[i] = (Control *) CloneRefHierarchy(controls[i]);
   tempControls[i]->DeleteKeys(TRACK_DOALL);
   float f = 0.0f;
   Point3 p(0.0f,0.0f,0.0f);
   if (tempControls[i]->SuperClassID() == CTRL_FLOAT_CLASS_ID)
      tempControls[i]->SetValue(0,&f);
   else if ( (tempControls[i]->SuperClassID() == CTRL_POSITION_CLASS_ID) || (tempControls[i]->SuperClassID() == CTRL_POINT3_CLASS_ID))
      tempControls[i]->SetValue(0,&p);
   }
}

void BlockControl::AddKeyToTempControl(TimeValue t,  TimeValue scale, BOOL isRelative)
{
//DebugPrint(_T("key data\n"));
for (int i =0; i < controls.Count();i++)
   {
//copy track in 
//need to add relative controls
		Interval iv(start,m_end);
   TrackClipObject *cpy = controls[i]->CopyTrack(iv, TIME_INCLEFT|TIME_INCRIGHT);
// int sz = sizeof(&cpy);
		iv.Set(t,t + (m_end-start));
   DWORD flags = TIME_INCLEFT|TIME_INCRIGHT;
   if (isRelative)
      {
      flags |= PASTE_RELATIVE;
//look at last key   and get offset
      }

   tempControls[i]->PasteTrack(cpy, iv, flags);
//now loop through and add relative value

//now need to scale those keys
   float s = 1.0f;
		if ((m_end-start) != 0)
			s =  (float)scale/(float)(m_end-start);

   tempControls[i]->ScaleTime(iv, s);

   cpy->DeleteThis();
   }
}


void BlockControl::AddKeyToSub(Control *sub, int whichSub, TimeValue t,  TimeValue scale, Interval mrange,BOOL isRelative)
{
//DebugPrint(_T("key data\n"));
int i = whichSub;
   
//copy track in 
//need to add relative controls
	Interval iv(start,m_end);
   TrackClipObject *cpy = controls[i]->CopyTrack(iv, TIME_INCLEFT|TIME_INCRIGHT);
//now need to scale those keys
   float s = 1.0f;
	if ((m_end-start) != 0)
		s =  (float)scale/(float)(m_end-start);
	iv.Set(t,t + (m_end-start));
   DWORD flags = TIME_INCLEFT|TIME_INCRIGHT;
   if (isRelative)
      {
      flags |= PASTE_RELATIVE;
//look at last key   and get offset
      }

   Interval scaleIV = iv;
   iv = iv & mrange;
   sub->PasteTrack(cpy, iv, flags);
//now loop through and add relative value

   sub->ScaleTime(scaleIV, s);

   cpy->DeleteThis();
      }

#define BLOCKCOUNT_CHUNK      0x01010
#define NAME_CHUNK            0x01020
#define NONAME_CHUNK       0x01030
#define COLOR_CHUNK           0x01040
#define START_CHUNK           0x01050
#define END_CHUNK          0x01060
#define BACKPOINTERS_CHUNK    0x01070
#define EXBACKPOINTERS_COUNT_CHUNK  0x01080
#define EXBACKPOINTERS_CHUNK  0x01090

IOResult BlockControl::Save(ISave *isave)
   {     
   ULONG nb;   
//count
   int count = controls.Count();
   isave->BeginChunk(BLOCKCOUNT_CHUNK);
   isave->Write(&count,sizeof(int),&nb);        
   isave->EndChunk();
//names

   for (int i=0; i<count; i++) {
      if (names[i]) {
         isave->BeginChunk(NAME_CHUNK);
         isave->WriteWString(*names[i]);
         isave->EndChunk();
      } else {
         isave->BeginChunk(NONAME_CHUNK);
         isave->EndChunk();
         }
      }
//color
   Color c = color;
   isave->BeginChunk(COLOR_CHUNK);
   isave->Write(&c,sizeof(c),&nb);        
   isave->EndChunk();

//start
   TimeValue s = start;
   isave->BeginChunk(START_CHUNK);
   isave->Write(&s,sizeof(s),&nb);        
   isave->EndChunk();

//end
	TimeValue e = m_end;
   isave->BeginChunk(END_CHUNK);
   isave->Write(&e,sizeof(e),&nb);        
   isave->EndChunk();

//back pointers
   isave->BeginChunk(BACKPOINTERS_CHUNK);
   for (int i=0; i<count; i++) 
      {
      ULONG id = isave->GetRefID(backPointers[i]);
      isave->Write(&id,sizeof(ULONG), &nb);
      }
   isave->EndChunk();

//external back pointers
   count = externalBackPointers.Count();
   isave->BeginChunk(EXBACKPOINTERS_COUNT_CHUNK);
   isave->Write(&count,sizeof(int),&nb);        
   isave->EndChunk();

//external back pointers
   isave->BeginChunk(EXBACKPOINTERS_CHUNK);
   for (int i=0; i<count; i++) 
      {
      ULONG id = isave->GetRefID(externalBackPointers[i]);
      isave->Write(&id,sizeof(ULONG), &nb);
      }
   isave->EndChunk();
   
   return IO_OK;
   }

IOResult BlockControl::Load(ILoad *iload)
   {
   int ID =  0;
   ULONG nb;
   IOResult res = IO_OK;
   int ix = 0;

   while (IO_OK==(res=iload->OpenChunk())) 
      {
      ID = iload->CurChunkID();
      if (ID ==BLOCKCOUNT_CHUNK)
         {
         int ct;
         iload->Read(&ct, sizeof(ct), &nb);
         controls.SetCount(ct);
         tempControls.SetCount(ct);
         names.SetCount(ct);
         backPointers.SetCount(ct);
         for (int i=0; i<ct; i++) 
            {
            names[i] = NULL;
            controls[i] = NULL;
            tempControls[i] = NULL;
            backPointers[i] = NULL;
            }

         }
      else if (ID == NAME_CHUNK)
         {
         TCHAR *buf;
         iload->ReadWStringChunk(&buf);
         names[ix++] = new TSTR(buf);
         }
      else if (ID == NONAME_CHUNK)
         {
         ix++;
         }
      else if (ID ==COLOR_CHUNK)
         {
         Color c;
         iload->Read(&c, sizeof(Color), &nb);
         color = c;
         }
      else if (ID ==START_CHUNK)
         {
         TimeValue s;
         iload->Read(&s, sizeof(s), &nb);
         start = s;
         }
      else if (ID ==END_CHUNK)
         {
         TimeValue e;
         iload->Read(&e, sizeof(e), &nb);
			m_end = e;
         }
      else if (ID ==BACKPOINTERS_CHUNK)
         {
         for (int i=0; i<backPointers.Count(); i++) 
            {
            ULONG id;
            iload->Read(&id,sizeof(ULONG), &nb);
            if (id!=0xffffffff)
               iload->RecordBackpatch(id,(void**)&backPointers[i]);
            }
         }
      else if (ID ==EXBACKPOINTERS_COUNT_CHUNK)
         {
         int ct;
         iload->Read(&ct, sizeof(ct), &nb);
         externalBackPointers.SetCount(ct);
         for (int i=0; i<ct; i++) 
            {
            externalBackPointers[i] = NULL;
            }

         }
      else if (ID ==EXBACKPOINTERS_CHUNK)
         {
         for (int i=0; i<externalBackPointers.Count(); i++) 
            {
            ULONG id;
            iload->Read(&id,sizeof(ULONG), &nb);
            if (id!=0xffffffff)
               iload->RecordBackpatch(id,(void**)&externalBackPointers[i]);
            }
         }


      iload->CloseChunk();
      if (res!=IO_OK)  return res;
      }

//rebuild all tempcontrols 
   return IO_OK;
   }



void BlockControl::EditTrackParams(
         TimeValue t,
         ParamDimensionBase *dim,
         const TCHAR *pname,
         HWND hParent,
         IObjParam *ip,
         DWORD flags)
{
trackHWND = hParent;

	DialogBoxParam  (hInstance, MAKEINTRESOURCE(IDD_ADDNEWTRACK), hParent, BlockPropDlgProc, (LPARAM)this);
}

int BlockControl::AddBlockName(ReferenceTarget *anim,ReferenceTarget *client, int subNum, NameList &names)

{

MyEnumProc dep;              
anim->DoEnumDependents(&dep);
TSTR nodeName = dep.name;
TSTR np = TSTR(client->SubAnimName(subNum));
TSTR Slash(_T("/"));
nodeName += Slash;
nodeName += np;
      
TSTR *st = new TSTR(nodeName);
names.Append(1,&st,1);
return 1;
}

Control* BlockControl::BuildListControl(TrackViewPick res, BOOL &createdList)
{
Control *list=NULL;
createdList = FALSE;
if ((res.anim->SuperClassID() == CTRL_FLOAT_CLASS_ID) && (res.client->ClassID() != Class_ID(FLOATLIST_CONTROL_CLASS_ID,0)))
   {
   list = (Control*)GetCOREInterface()->CreateInstance(
   CTRL_FLOAT_CLASS_ID,
   Class_ID(FLOATLIST_CONTROL_CLASS_ID,0));
   createdList = TRUE;
   }
else if ((res.anim->SuperClassID() == CTRL_FLOAT_CLASS_ID) && (res.client->ClassID() == Class_ID(FLOATLIST_CONTROL_CLASS_ID,0)))
   {
   list = (Control *)res.client;
   }
else if ((res.anim->SuperClassID() == CTRL_POSITION_CLASS_ID) && (res.client->ClassID() != Class_ID(POSLIST_CONTROL_CLASS_ID,0)))
   {
   list = (Control*)GetCOREInterface()->CreateInstance(
   CTRL_POSITION_CLASS_ID,
   Class_ID(POSLIST_CONTROL_CLASS_ID,0));
   createdList = TRUE;
   }
else if ((res.anim->SuperClassID() == CTRL_POSITION_CLASS_ID) && (res.client->ClassID() == Class_ID(POSLIST_CONTROL_CLASS_ID,0)))
   {
   list =  (Control *)res.client;
   }
else if ((res.anim->SuperClassID() == CTRL_ROTATION_CLASS_ID) && (res.client->ClassID() != Class_ID(ROTLIST_CONTROL_CLASS_ID,0)))
   {
   list = (Control*)GetCOREInterface()->CreateInstance(
   CTRL_ROTATION_CLASS_ID,
   Class_ID(ROTLIST_CONTROL_CLASS_ID,0));
   createdList = TRUE;
   }
else if ((res.anim->SuperClassID() == CTRL_ROTATION_CLASS_ID) && (res.client->ClassID() == Class_ID(ROTLIST_CONTROL_CLASS_ID,0)))
   {
   list =  (Control *)res.client;
   }
else if ((res.anim->SuperClassID() == CTRL_SCALE_CLASS_ID) && (res.client->ClassID() != Class_ID(SCALELIST_CONTROL_CLASS_ID,0)))
   {
   list = (Control*)GetCOREInterface()->CreateInstance(
   CTRL_SCALE_CLASS_ID,
   Class_ID(SCALELIST_CONTROL_CLASS_ID,0));
   createdList = TRUE;
   }
else if ((res.anim->SuperClassID() == CTRL_SCALE_CLASS_ID) && (res.client->ClassID() == Class_ID(SCALELIST_CONTROL_CLASS_ID,0)))
   {
   list =  (Control *)res.client;
   }
return list;
}

Control* BlockControl::BuildSlave(TrackViewPick res,Control* list, BOOL createdList)
{
int count = list->NumSubs()-2;
Control *slave = NULL;
for (int i = 0; i < count; i++)
   {
   if (res.anim->SuperClassID() == CTRL_FLOAT_CLASS_ID) 
      {
      slave = (Control*)list->SubAnim(i);
      if (slave->ClassID() == SLAVEFLOAT_CONTROL_CLASS_ID)
         {
         return slave;
         }
      }
   else if (res.anim->SuperClassID() == CTRL_POSITION_CLASS_ID) 
      {
      slave = (Control*)list->SubAnim(i);
      if (slave->ClassID() == SLAVEPOS_CONTROL_CLASS_ID)
         {
         return slave;
         }
      }
   else if (res.anim->SuperClassID() == CTRL_ROTATION_CLASS_ID) 
      {
      slave = (Control*)list->SubAnim(i);
      if (slave->ClassID() == SLAVEROTATION_CONTROL_CLASS_ID)
         {
         return slave;
         }
      }
   else if (res.anim->SuperClassID() == CTRL_SCALE_CLASS_ID) 
      {
      slave = (Control*)list->SubAnim(i);
      if (slave->ClassID() == SLAVESCALE_CONTROL_CLASS_ID)
         {
         return slave;
         }
      }
   }
BOOL isRotation = FALSE;
if (res.anim->SuperClassID() == CTRL_FLOAT_CLASS_ID) 
   slave = (Control*)new SlaveFloatControl;
#ifndef NO_CONTROLLER_SLAVE_POSITION
else if (res.anim->SuperClassID() == CTRL_POSITION_CLASS_ID) 
   slave = (Control*)new SlavePosControl;
#endif
#ifndef NO_CONTROLLER_SLAVE_ROTATION
else if (res.anim->SuperClassID() == CTRL_ROTATION_CLASS_ID) 
   {
   slave = (Control*)new SlaveRotationControl;
   isRotation = TRUE;
   }
#endif
#ifndef NO_CONTROLLER_SLAVE_SCALE
else if (res.anim->SuperClassID() == CTRL_SCALE_CLASS_ID) 
   slave = (Control*)new SlaveScaleControl;
#endif
if (createdList)
   {
      list->AssignController(CloneRefHierarchy(res.anim),count);
      list->AssignController(slave,count+1);
      }  
else
   {
   list->AssignController(slave,count);
   }
return slave;
}


int BlockControl::AddControl(HWND hWnd)
{
//pop up track view selector
Interface *ip = GetCOREInterface();

TrackViewPick res;
MasterBlockTrackViewFilter filter;
if (ip->TrackViewPickDlg(hWnd, &res,&filter ))
   {
//pop frame selector
   if (res.anim != NULL)
      {
      int OK = DialogBoxParam  (hInstance, MAKEINTRESOURCE(IDD_TRACKPROP),
         hWnd, TrackPropDlgProc, (LPARAM)this);
      if (OK)
         {
         Control *list;
         BOOL createdList = FALSE;
//check for list control if not add
         list = BuildListControl(res,createdList);
//check if list has a slave control
         Control *slaveControl;
         slaveControl = BuildSlave(res,list,createdList);


         int i = controls.Count();
         Control *ctemp = NULL;
         controls.Append(1,&ctemp,1);
         tempControls.Append(1,&ctemp,1);
         ReplaceReference(i,CloneRefHierarchy(res.anim));

//copy relvant keys
         propStart = propStart * GetTicksPerFrame();
				Interval iv(propStart,propStart + (m_end-start));
         TrackClipObject *cpy = controls[i]->CopyTrack(iv, TIME_INCLEFT|TIME_INCRIGHT);
//nuke all keys 
         controls[i]->DeleteKeys(TRACK_DOALL);
//paste back relevant keys
				iv.Set(0,m_end-start);
         controls[i]->PasteTrack(cpy, iv, TIME_INCLEFT|TIME_INCRIGHT);


         tempControls[i] = (Control *) CloneRefHierarchy(res.anim);
         SlaveControl *sl = (SlaveControl *) slaveControl;
         backPointers.Append(1,&sl,1);

         AddBlockName(res.anim,res.client,res.subNum,names);
//add slaves controls to the selected tracks and put the original as a sub anim of the slaves
//set slave to have reference to master
         slaveControl->ReplaceReference(1,this);
//copy selected track into slave sub
         slaveControl->ReplaceReference(0,(Control*)CloneRefHierarchy(res.anim));
         int bc;

         #define ID_TV_GETFIRSTSELECTED   680
				MaxSDK::Array<TrackViewPick> r;
         SendMessage(trackHWND,WM_COMMAND,ID_TV_GETFIRSTSELECTED,(LPARAM)&r);
         bc = r[0].subNum-1;

         if (res.anim->SuperClassID() == CTRL_FLOAT_CLASS_ID)
            {
            float f = 0.0f;
            tempControls[i]->DeleteKeys(TRACK_DOALL);
            tempControls[i]->SetValue(0,&f);
            SlaveFloatControl *slave = (SlaveFloatControl *) slaveControl;

            slave->scratchControl = (Control *) CloneRefHierarchy(res.anim);
//now replace track with slave
// int bc = Blocks.Count()-1;
            slave->blockID.Append(1,&bc,1);
            slave->subID.Append(1,&i,1);
            }
#ifndef NO_CONTROLLER_SLAVE_POSITION
         else if (res.anim->SuperClassID() == CTRL_POSITION_CLASS_ID)
            {
            Point3 f(0.0f,0.0f,0.0f);
            tempControls[i]->DeleteKeys(TRACK_DOALL);
            tempControls[i]->SetValue(0,&f);
            SlavePosControl *slave = (SlavePosControl *) slaveControl;

            slave->scratchControl = (Control *) CloneRefHierarchy(res.anim);
//now replace track with slave
// int bc = Blocks.Count()-1;
            slave->blockID.Append(1,&bc,1);
            slave->subID.Append(1,&i,1);
            }
#endif
#ifndef NO_CONTROLLER_SLAVE_ROTATION
         else if (res.anim->SuperClassID() == CTRL_ROTATION_CLASS_ID)
            {
            Quat f;
            f.Identity();
            tempControls[i]->DeleteKeys(TRACK_DOALL);
            tempControls[i]->SetValue(0,&f);
            SlaveRotationControl *slave = (SlaveRotationControl *) slaveControl;

            slave->scratchControl = (Control *) CloneRefHierarchy(res.anim);
//now replace track with slave
            slave->blockID.Append(1,&bc,1);
            slave->subID.Append(1,&i,1);

            }
#endif
#ifndef NO_CONTROLLER_SLAVE_SCALE
         else if (res.anim->SuperClassID() == CTRL_SCALE_CLASS_ID)
            {
            Matrix3 f(1);
//    f.Identity();
            tempControls[i]->DeleteKeys(TRACK_DOALL);
            tempControls[i]->SetValue(0,&f);
            SlaveScaleControl *slave = (SlaveScaleControl *) slaveControl;

            slave->scratchControl = (Control *) CloneRefHierarchy(res.anim);
//now replace track with slave
// int bc = Blocks.Count()-1;
            slave->blockID.Append(1,&bc,1);
            slave->subID.Append(1,&i,1);
            }
#endif
         if (createdList)
            res.client->AssignController(list,res.subNum);
         return 1;
         }
      }

   }
return 1;
}

int BlockControl::DeleteControl(int Index)
{
	if ((Index < 0) || (Index >= controls.Count())) 
		return 0;
//notify all back pointer that there block is about to be deleted

DeleteReference(Index);
controls.Delete(Index,1);
if (tempControls[Index] != NULL)
{ 
   HoldSuspend hs;
   RefResult res = tempControls[Index]->MaybeAutoDelete();
   DbgAssert(REF_SUCCEED == res);
}
tempControls.Delete(Index,1);
names.Delete(Index,1);

for (int i = 0;i <backPointers.Count();i++)
   {
		// lock at the blockid table
// int subCount = Blocks[whichBlock]->backPointers[i]->blockID.Count();
   for (int j=0; j< backPointers[i]->blockID.Count(); j++)
      {
      if (backPointers[i]->subID[j] == Index)
         {
         backPointers[i]->blockID.Delete(j,1);
         backPointers[i]->subID.Delete(j,1);
         j--;
         }
      else if (backPointers[i]->subID[j] > Index)
         {
         backPointers[i]->subID[j] -= 1;
         }
      }
   }

backPointers.Delete(Index,1);

for (int i = 0;i < externalBackPointers.Count();i++)
   {
   for (int j=0; j< externalBackPointers[i]->blockID.Count(); j++)
      {
      if (externalBackPointers[i]->subID[j] == Index)
         {
         externalBackPointers[i]->blockID.Delete(j,1);
         externalBackPointers[i]->subID.Delete(j,1);
         j--;
         }
      else if (externalBackPointers[i]->subID[j] > Index)
         {
         externalBackPointers[i]->subID[j] -= 1;
         }
      }

   }
NotifyDependents(FOREVER,0,REFMSG_CHANGE);
NotifyDependents(FOREVER,0,REFMSG_SUBANIM_STRUCTURE_CHANGED);

return 1;
}



INT_PTR CALLBACK BlockPropDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   BlockControl *blk = DLGetWindowLongPtr<BlockControl*>(hWnd);

   switch (msg) {
   case WM_INITDIALOG:
      {
      blk = (BlockControl*)lParam;
      DLSetWindowLongPtr(hWnd, lParam);
//load up list box with sub anims names
      for (int i = 0; i < blk->NumSubs(); i++)
         {
         TSTR finalName = blk->SubAnimName(i);
         SendMessage(GetDlgItem(hWnd,IDC_LIST1),
            LB_ADDSTRING,0,(LPARAM)(const TCHAR*)finalName);
         }
      SendMessage(GetDlgItem(hWnd,IDC_LIST1),
         LB_SETCURSEL,0,0);
      CenterWindow(hWnd,GetParent(hWnd));
      break;
      }
      
   case WM_COMMAND:
      switch (LOWORD(wParam)) {
      case IDC_ADD:
         {
         blk->AddControl( hWnd);
         SendMessage(GetDlgItem(hWnd,IDC_LIST1),
            LB_SETCOUNT,0,0);
         for (int i = 0; i < blk->NumSubs(); i++)
            {
            TSTR finalName = blk->SubAnimName(i);
            SendMessage(GetDlgItem(hWnd,IDC_LIST1),
               LB_ADDSTRING,0,(LPARAM)(const TCHAR*)finalName);
            }
         SendMessage(GetDlgItem(hWnd,IDC_LIST1),
         LB_SETCURSEL,0,0);
         break;
         }
      case IDC_REMOVE:
         {
         int sel = SendMessage(GetDlgItem(hWnd,IDC_LIST1),
                  LB_GETCURSEL,0,0);
         if (sel >=0)
            {
            blk->DeleteControl(sel);
            SendMessage(GetDlgItem(hWnd,IDC_LIST1),
               LB_DELETESTRING,sel,0);

            }
         break;
         }
      case IDOK:
         {
         EndDialog(hWnd,1);
         break;
         }
      }
      break;

   default:
      return FALSE;
   }
   return TRUE;
}




INT_PTR CALLBACK TrackPropDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
   BlockControl *blk = DLGetWindowLongPtr<BlockControl*>(hWnd);

	ISpinnerControl* spin = NULL;
   Rect rect;

   switch (msg) {
   case WM_INITDIALOG:
      {
      blk = (BlockControl*)lParam;
      DLSetWindowLongPtr(hWnd, lParam);

      Interval range = GetCOREInterface()->GetAnimRange();

      
      spin = GetISpinner(GetDlgItem(hWnd,IDC_STARTSPIN));
      spin->SetLimits(-999999.0f,9999999.0f, FALSE);
      spin->SetAutoScale();
      spin->LinkToEdit(GetDlgItem(hWnd,IDC_START), EDITTYPE_INT);
      spin->SetValue(range.Start()/GetTicksPerFrame(),FALSE);
      ReleaseISpinner(spin);

      blk->propStart = range.Start()/GetTicksPerFrame();
      CenterWindow(hWnd,GetParent(hWnd));
      break;
      }
      
   case CC_SPINNER_CHANGE:
      spin = (ISpinnerControl*)lParam;
      switch (LOWORD(wParam)) {
      case IDC_STARTSPIN: blk->propStart = spin->GetIVal(); break;
      }
      break;

   case WM_COMMAND:
      switch (LOWORD(wParam)) {
      case IDOK:
         {
         EndDialog(hWnd,1);
         blk->NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);

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
