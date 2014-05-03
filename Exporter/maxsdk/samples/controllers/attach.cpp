/**********************************************************************
 *<
   FILE: attach.cpp

   DESCRIPTION: Attachment controller

   CREATED BY: Rolf Berteig

   HISTORY: 11/18/96

 *>   Copyright (c) 1996, All Rights Reserved.
 **********************************************************************/
#include "ctrl.h"
#include "attach.h"
#include "units.h"
#include "tcbgraph.h"

#include "3dsmaxport.h"
#define ATTACH_CONTROL_CNAME     GetString(IDS_RB_ATTACHCONTROL)

static void ComputeHermiteBasis(float u, float *v);
static float Ease(float u, float a, float b);

#define KEY_SELECTED (1<<0)
#define KEY_FLAGGED     (1<<1)

class PickObjectMode : 
      public PickModeCallback,
      public PickNodeCallback {
   public:     
      AttachCtrl *cont;
      
      PickObjectMode(AttachCtrl *c) {cont=c;}
      BOOL HitTest(IObjParam *ip,HWND hWnd,ViewExp *vpt,IPoint2 m,int flags);
      BOOL Pick(IObjParam *ip,ViewExp *vpt);
      void EnterMode(IObjParam *ip);
      void ExitMode(IObjParam *ip);
      BOOL RightClick(IObjParam *ip,ViewExp *vpt)  {return TRUE;}
      BOOL Filter(INode *node);     
      PickNodeCallback *GetFilter() {return this;}
   };

#define CID_SETPOS 0x938ff27a

class SetPosMouseProc : public MouseCallBack {
   public:
      AttachCtrl *cont;
      IObjParam *ip;
      SetPosMouseProc(AttachCtrl *c,IObjParam *i) {cont=c;ip=i;}
      int proc(HWND hWnd, int msg, int point, int flags, IPoint2 m);    
      void SetPos(HWND hWnd,IPoint2 m);
   };

class SetPosCMode : public CommandMode {
   public:
      ChangeFGObject fgProc;
      SetPosMouseProc proc;
      IObjParam *ip;
      AttachCtrl *cont;

      SetPosCMode(AttachCtrl *c,IObjParam *i) 
         : fgProc((ReferenceTarget*)c), proc(c,i) {ip=i;cont=c;}

      int Class() {return MOVE_COMMAND;}     
      int ID() {return CID_SETPOS;}
      MouseCallBack *MouseProc(int *numPoints) {*numPoints=2;return &proc;}
      ChangeForegroundCallback *ChangeFGProc() {return &fgProc;}
      BOOL ChangeFG(CommandMode *oldMode) {return oldMode->ChangeFGProc() != &fgProc;}
      void EnterMode();
      void ExitMode();
   };


HWND             AttachCtrl::hWnd       = NULL;
IObjParam       *AttachCtrl::ip         = NULL;
AttachCtrl      *AttachCtrl::editCont   = NULL;
BOOL             AttachCtrl::uiValid    = FALSE;
ISpinnerControl *AttachCtrl::iTime      = NULL;
ISpinnerControl *AttachCtrl::iFace      = NULL;
ISpinnerControl *AttachCtrl::iA         = NULL;
ISpinnerControl *AttachCtrl::iB         = NULL;
ISpinnerControl *AttachCtrl::iTens      = NULL;
ISpinnerControl *AttachCtrl::iCont      = NULL;
ISpinnerControl *AttachCtrl::iBias      = NULL;
ISpinnerControl *AttachCtrl::iEaseTo    = NULL;
ISpinnerControl *AttachCtrl::iEaseFrom  = NULL;
ICustButton     *AttachCtrl::iPickOb    = NULL;
ICustButton     *AttachCtrl::iSetPos    = NULL;
ICustButton     *AttachCtrl::iPrev      = NULL;
ICustButton     *AttachCtrl::iNext      = NULL;
ICustButton     *AttachCtrl::iUpdate    = NULL;
ICustStatus     *AttachCtrl::iStat      = NULL;
int              AttachCtrl::index      = -1;
PickObjectMode  *AttachCtrl::pickObMode = NULL;
SetPosCMode     *AttachCtrl::setPosMode = NULL;

class AttachClassDesc:public ClassDesc {
   public:
   int         IsPublic() {return 1;}
   void *         Create(BOOL loading) {return new AttachCtrl();}
   const TCHAR *  ClassName() {return ATTACH_CONTROL_CNAME;}
   SClass_ID      SuperClassID() {return CTRL_POSITION_CLASS_ID; }
   Class_ID    ClassID() {return ATTACH_CONTROL_CLASS_ID;}
   const TCHAR*   Category() {return _T("");}
   };
static AttachClassDesc attachCD;
ClassDesc* GetAttachControlDesc() {return &attachCD;}


class AttachCtrlRestore : public RestoreObj {
   public:
	MaxSDK::Array<AKey> undo;
	MaxSDK::Array<AKey> redo;
      Interval urange, rrange;
      AttachCtrl *cont;

      AttachCtrlRestore(AttachCtrl *c) {
         cont   = c;
         undo   = c->keys;       
         urange = c->range;
         }
      
      void Restore(int isUndo) {
         if (isUndo) {
            redo   = cont->keys;
            rrange = cont->range;
            }
         cont->keys  = undo;
         cont->range = urange;
         cont->Invalidate();
         cont->NotifyDependents(FOREVER,0,REFMSG_CHANGE);
         }

      void Redo() {
         cont->keys  = redo;
         cont->range = rrange;
         cont->Invalidate();
         cont->NotifyDependents(FOREVER,0,REFMSG_CHANGE);
         }

      int Size() {return 1;}
      void EndHold() {cont->ClearAFlag(A_HELD);}
   };


class AttachClipObject : public TrackClipObject {
   public:
	MaxSDK::Array<AKey> tab;

      Class_ID ClassID() {return ATTACH_CONTROL_CLASS_ID;}
      SClass_ID SuperClassID() { return CTRL_FLOAT_CLASS_ID; }
      void DeleteThis() {delete this;}

      AttachClipObject(Interval iv) : TrackClipObject(iv) {}
   };



//-----------------------------------------------------------------------------

AttachCtrl::AttachCtrl() 
   {  
   node        = NULL;
   rangeLinked = TRUE;
   align       = TRUE;
   trackValid  = FALSE;
   manUpdate   = FALSE;
   doManUpdate = FALSE;
   setPosButton = FALSE;
   val  = Point3(0,0,0);
   qval = IdentQuat();
   }

RefTargetHandle AttachCtrl::Clone(RemapDir &remap)
   {
   AttachCtrl *bc = new AttachCtrl;
   bc->range      = range;
   bc->valid.SetEmpty();
   bc->keys       = keys;
   bc->val        = val;
   bc->ReplaceReference(Control::NumRefs(),(RefTargetHandle)node);
   bc->Control::NumRefs();
   bc->manUpdate  = manUpdate;
   bc->setPosButton = setPosButton;
   bc->mLocked = mLocked;
   CloneControl(bc,remap);
   BaseClone(this, bc, remap);
   return bc;
   }

void AttachCtrl::GetClassName(TSTR& s)
   {
      s = ATTACH_CONTROL_CNAME;
   }

#define KEYTIME_CHUNKID    0x0100
#define KEYFLAGS_CHUNKID   0x0110
#define NUMKEYS_CHUNKID    0x0130
#define KEYFACE_CHUNKID    0x0140
#define KEYU0_CHUNKID      0x0150
#define KEYU1_CHUNKID      0x0160
#define KEYTENS_CHUNKID    0x0170
#define KEYCONT_CHUNKID    0x0180
#define KEYBIAS_CHUNKID    0x0190
#define KEYEASEIN_CHUNKID  0x0200
#define KEYEASEOUT_CHUNKID 0x0210
#define ALIGN_CHUNKID      0x0220
#define MANUPDATE_CHUNKID  0x0230
#define LOCK_CHUNK		   0x2535  //the lock value

IOResult AttachCtrl::Save(ISave *isave)
   {
   ULONG nb;
	int ct = (int)keys.length();
      
   // Save align
   isave->BeginChunk(ALIGN_CHUNKID);
   isave->Write(&align,sizeof(align),&nb);   
   isave->EndChunk();

   // Manual update
   isave->BeginChunk(MANUPDATE_CHUNKID);
   isave->Write(&manUpdate,sizeof(manUpdate),&nb);
   isave->EndChunk();

   //save locked
	int on = (mLocked==true) ? 1 :0;
	isave->BeginChunk(LOCK_CHUNK);
	isave->Write(&on,sizeof(on),&nb);	
	isave->EndChunk();

   // Save the number of keys
   isave->BeginChunk(NUMKEYS_CHUNKID);
   isave->Write(&ct,sizeof(ct),&nb);
   isave->EndChunk();

   // Write each key
   for (int i=0; i<ct; i++) {
      isave->BeginChunk(KEYTIME_CHUNKID);
      isave->Write(&keys[i].time,sizeof(keys[i].time),&nb);
      isave->EndChunk();

      isave->BeginChunk(KEYFLAGS_CHUNKID);
      isave->Write(&keys[i].flags,sizeof(keys[i].flags),&nb);
      isave->EndChunk();      

      isave->BeginChunk(KEYFACE_CHUNKID);
      isave->Write(&keys[i].face,sizeof(keys[i].face),&nb);
      isave->EndChunk();      

      isave->BeginChunk(KEYU0_CHUNKID);
      isave->Write(&keys[i].u0,sizeof(keys[i].u0),&nb);
      isave->EndChunk();      

      isave->BeginChunk(KEYU1_CHUNKID);
      isave->Write(&keys[i].u1,sizeof(keys[i].u1),&nb);
      isave->EndChunk();      

      isave->BeginChunk(KEYTENS_CHUNKID);
      isave->Write(&keys[i].tens,sizeof(keys[i].tens),&nb);
      isave->EndChunk();      

      isave->BeginChunk(KEYCONT_CHUNKID);
      isave->Write(&keys[i].cont,sizeof(keys[i].cont),&nb);
      isave->EndChunk();      

      isave->BeginChunk(KEYBIAS_CHUNKID);
      isave->Write(&keys[i].bias,sizeof(keys[i].bias),&nb);
      isave->EndChunk();

      isave->BeginChunk(KEYEASEIN_CHUNKID);
      isave->Write(&keys[i].easeIn,sizeof(keys[i].easeIn),&nb);
      isave->EndChunk();      

      isave->BeginChunk(KEYEASEOUT_CHUNKID);
      isave->Write(&keys[i].easeOut,sizeof(keys[i].easeOut),&nb);
      isave->EndChunk();
      }
   
   return IO_OK;
   }

IOResult AttachCtrl::Load(ILoad *iload)
   {
   ULONG nb;
   int ct;  
   IOResult res;
      
   while (IO_OK==(res=iload->OpenChunk())) {
      switch (iload->CurChunkID()) {   
         case ALIGN_CHUNKID:
            iload->Read(&align,sizeof(align),&nb);
            break;

         case MANUPDATE_CHUNKID:
            iload->Read(&manUpdate,sizeof(manUpdate),&nb);
            break;
		case LOCK_CHUNK:
			{
				int on;
				res=iload->Read(&on,sizeof(on),&nb);
				if(on)
					mLocked = true;
				else
					mLocked = false;
			}
			break;
         case NUMKEYS_CHUNKID:   
            iload->Read(&ct,sizeof(ct),&nb);
            iload->CloseChunk(); 
			keys.setLengthUsed(ct);

            // Key data always follows
            for (int i=0; i<ct; i++) {
               keys[i] = AKey();

               iload->OpenChunk();
               assert(iload->CurChunkID()==KEYTIME_CHUNKID);
               iload->Read(&keys[i].time,sizeof(keys[i].time),&nb);
               iload->CloseChunk();
               
               iload->OpenChunk();
               assert(iload->CurChunkID()==KEYFLAGS_CHUNKID);
               iload->Read(&keys[i].flags,sizeof(keys[i].flags),&nb);
               iload->CloseChunk();    

               iload->OpenChunk();
               assert(iload->CurChunkID()==KEYFACE_CHUNKID);
               iload->Read(&keys[i].face,sizeof(keys[i].face),&nb);
               iload->CloseChunk();    

               iload->OpenChunk();
               assert(iload->CurChunkID()==KEYU0_CHUNKID);
               iload->Read(&keys[i].u0,sizeof(keys[i].u0),&nb);
               iload->CloseChunk();    

               iload->OpenChunk();
               assert(iload->CurChunkID()==KEYU1_CHUNKID);
               iload->Read(&keys[i].u1,sizeof(keys[i].u1),&nb);
               iload->CloseChunk();    

               iload->OpenChunk();
               assert(iload->CurChunkID()==KEYTENS_CHUNKID);
               iload->Read(&keys[i].tens,sizeof(keys[i].tens),&nb);
               iload->CloseChunk();    

               iload->OpenChunk();
               assert(iload->CurChunkID()==KEYCONT_CHUNKID);
               iload->Read(&keys[i].cont,sizeof(keys[i].cont),&nb);
               iload->CloseChunk();    

               iload->OpenChunk();
               assert(iload->CurChunkID()==KEYBIAS_CHUNKID);
               iload->Read(&keys[i].bias,sizeof(keys[i].bias),&nb);
               iload->CloseChunk();    

               iload->OpenChunk();
               assert(iload->CurChunkID()==KEYEASEIN_CHUNKID);
               iload->Read(&keys[i].easeIn,sizeof(keys[i].easeIn),&nb);
               iload->CloseChunk();    

               iload->OpenChunk();
               assert(iload->CurChunkID()==KEYEASEOUT_CHUNKID);
               iload->Read(&keys[i].easeOut,sizeof(keys[i].easeOut),&nb);
               iload->CloseChunk();    
               }
            continue;
         }
      iload->CloseChunk();
      if (res!=IO_OK)  return res;
      }
            
   return IO_OK;
   }

void AttachCtrl::Copy(Control *from)
   {  
   }

void AttachCtrl::HoldTrack()
   {
   if (theHold.Holding()&&!TestAFlag(A_HELD)) {    
      theHold.Put(new AttachCtrlRestore(this));
      SetAFlag(A_HELD);
      }
   }

void AttachCtrl::EditTimeRange(Interval range,DWORD flags)
{
   if(GetLocked()==false)
   {
	   HoldTrack();
		if (flags&EDITRANGE_LINKTOKEYS && keys.length()) {
			this->range.Set(keys[0].time,keys[keys.length()-1].time);
		  rangeLinked = TRUE;
	   } else {    
		  rangeLinked = FALSE;
		  this->range = range;    
		  }
	   Invalidate();
	   NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
   }
}

Interval AttachCtrl::GetTimeRange(DWORD flags)
{
	if (rangeLinked && keys.length()) 
	  if (flags&TIMERANGE_SELONLY) {
		 Interval iv;   
			size_t n = keys.length();
			if (!n)
				return Animatable::GetTimeRange(flags);
			for (size_t i = 0; i < n; i++) {
			if (keys[i].flags & KEY_SELECTED) {
			   if (iv.Empty()) {
				  iv.SetInstant(keys[i].time);  
			   } else {
				  iv += keys[i].time;
			   }
			}
		 }
		 return iv;
	  }
	  else
			return Interval(keys[0].time,keys[keys.length()-1].time);
   else return range;
}

void AttachCtrl::MapKeys(TimeMap *map,DWORD flags)
{
	if(GetLocked()==false)
	{
		size_t n = keys.length();
	   BOOL changed = FALSE;
	   if (!n) return;
	   HoldTrack();

	   if (flags&TRACK_MAPRANGE) {      
		  TimeValue t0 = map->map(range.Start());
		  TimeValue t1 = map->map(range.End());
		  range.Set(t0,t1);
		  changed = TRUE;
		  }  

	   if (flags&TRACK_DOALL) {
		  for (int i=0; i<n; i++) {        
			 keys[i].time = map->map(keys[i].time);
			 changed = TRUE;
			 }
	   } else 
	   if (flags&TRACK_DOSEL) {
		  TimeValue mappedRangeStart = 0, mappedRangeEnd = 0, duration = 0;
		  // When using either the "Replace" or the "Insert" options, compute the selected keys' time range.
		  if ( flags&(TRACK_REPLACEKEYS|TRACK_INSERTKEYS) ) {
			 Interval iv = GetTimeRange(TIMERANGE_SELONLY);
			 mappedRangeStart = map->map( iv.Start() );
			 mappedRangeEnd   = map->map( iv.End() );
			 // Add a frame to the insert buffer so that we don't delete a key that would be right at the insertion
			 // point (because it would otherwise become coincident with the last key of the selected keys buffer).
			 if ( flags&TRACK_INSERTKEYS )
				duration = iv.End() - iv.Start() + GetTicksPerFrame();
		  }

		  BOOL slide = flags&TRACK_SLIDEUNSEL;
		  TimeValue delta = 0, prev;
				size_t start, end, inc;
		  if (flags&TRACK_RIGHTTOLEFT) {
			 start = n-1;
			 end = -1;
			 inc = -1;
		  } else {
			 start = 0;
			 end = n;
			 inc = 1;
			 } 
				for (size_t i=start; i!=end; i+=inc) {
			 if (keys[i].flags & KEY_SELECTED) {
				prev = keys[i].time;
				keys[i].time = map->map(keys[i].time);
				delta = keys[i].time - prev;
				changed = TRUE;
			 } else if (slide) {
				keys[i].time += delta;
			 } else if (flags&TRACK_INSERTKEYS)
				// When using the Insert option and the current key time is on, or beyond selected keys'
				// insertion point, add the length of the selected keys' buffer.
				if ( keys[i].time >= mappedRangeStart )
				   keys[i].time += duration;
		  }

		  // If the Replace option is on, remove all unselected keys within the range of currently selected keys.
		  // Note: there's no point doing this when the Slide or Insert options are used because unselected keys
		  //     have already been moved out of the interval.
		  if ( flags&TRACK_REPLACEKEYS && !(slide || flags&TRACK_INSERTKEYS) ) {
			 // Delete any unselected key within the selected keys interval
			 // Note: unfortunately, we can't use FindKeyIndex to optimize the loop
			 //     because mapped (i.e. selected) keys have not been sorted yet.
					for ( size_t i = n-1; i >= 0; i-- ) {
				if ( (!keys[i].flags & KEY_SELECTED) && (keys[i].time <= mappedRangeEnd) ) {
				   if ( keys[i].time < mappedRangeStart )
					  break;
							keys.removeAt(i);
				}
			 }         
		  }
	   }
	   
	   if (changed) {
		  Invalidate();
		  SortKeys();
		  NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
		  }
   }
}


int AttachCtrl::GetKeyIndex(TimeValue t)
   {
	for (int i=0; i<keys.length(); i++) {
      if (keys[i].time==t) return i;
      if (keys[i].time>t) return -1;
      }
   return -1;
   }

void AttachCtrl::DeleteKeyAtTime(TimeValue t)
{
	if(GetLocked()==false)
	{
	   int index = GetKeyIndex(t);
	   if (index>=0) {
		  HoldTrack();
			keys.removeAt(index);
		  Invalidate();
		  NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
		  }
	}
}

BOOL AttachCtrl::IsKeyAtTime(TimeValue t,DWORD flags)
   {
	for (int i=0; i<keys.length(); i++) {
      if (keys[i].time>t) return FALSE;
      if (keys[i].time==t) return TRUE;      
      }
   return FALSE;
   }

int AttachCtrl::GetKeyTimes(
      Tab<TimeValue> &times,Interval range,DWORD flags)
   {
   int skip = 0;
	for (int i=0; i<keys.length(); i++) {      
      if (keys[i].time >= range.Start() && keys[i].time <= range.End()) {
         times.Append(1,&keys[i].time,10);
      } else {
         if (keys[i].time > range.End()) break;
         skip++;
         }
      }
   times.Shrink();
   return skip;
   }

int AttachCtrl::GetKeySelState(
      BitArray &sel,Interval range,DWORD flags)
   {
   int c = 0;
   int skip = 0;
	for (int i=0; i<keys.length(); i++) {
      if (keys[i].time >= range.Start() && keys[i].time <= range.End()) {
         sel.Set(c++,keys[i].flags&KEY_SELECTED);
      } else {       
         if (keys[i].time > range.End()) break;
         skip++;
         }
      }  
   return skip;
   }

BOOL AttachCtrl::GetNextKeyTime(
      TimeValue t,DWORD flags,TimeValue &nt)
   {
	size_t num_keys = keys.length();
	if (num_keys < 0) 
		return FALSE;
   if (flags&NEXTKEY_RIGHT) {    
		for (size_t i=0; i < num_keys; i++) {
         if (keys[i].time > t ) {
            nt = keys[i].time;
            return TRUE;
            }
         }
      nt = keys[0].time;
      return TRUE;
	} 
	else {
		for (size_t i = num_keys - 1; i>=0; i--) {
         if (keys[i].time < t ) {
            nt = keys[i].time;
            return TRUE;
            }
         }
		nt = keys[num_keys - 1].time;
      return TRUE;
      }
   }


void AttachCtrl::DeleteTime(Interval iv, DWORD flags)
{
	if(GetLocked()==false)
	{
	   Interval test = TestInterval(iv,flags);
		size_t n = keys.length();   
	   int d = iv.Duration()-1;
	   if (d<0) d = 0;
	   HoldTrack();

		for (size_t i = n-1; i >= 0; i--) {
		  if (test.InInterval(keys[i].time)) {
				keys.removeAt(i);
		  } else 
		  if (!(flags&TIME_NOSLIDE)) {        
			 if (keys[i].time > test.End()) {
				keys[i].time -= d;
				}
			 }
		  }         
	   
	   Invalidate();
	   SortKeys();
	   NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
   }
}

void AttachCtrl::ReverseTime(Interval iv, DWORD flags)
{
	if(GetLocked()==false)
	{
	   Interval test = TestInterval(iv,flags);
		size_t n = keys.length();
	   HoldTrack();

	   for (int i = 0; i < n; i++) {    
		  if (test.InInterval(keys[i].time)) {
			 TimeValue delta = keys[i].time - iv.Start();
			 keys[i].time = iv.End()-delta;         
			 }
		  }
	   Invalidate();
	   SortKeys();
	   NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
   }
}

void AttachCtrl::ScaleTime(Interval iv, float s)
{
	if(GetLocked()==false)
	{
		size_t n = keys.length();
	   TimeValue delta = int(s*float(iv.End()-iv.Start())) + iv.Start()-iv.End();
	   HoldTrack();

	   for (int i = 0; i < n; i++) {    
		  if (iv.InInterval(keys[i].time)) {
			 keys[i].time = 
				int(s*float(keys[i].time - iv.Start())) + iv.Start();
		  } else 
		  if (keys[i].time > iv.End()) {
			 keys[i].time += delta;
			 }
		  }
	   Invalidate();
	   SortKeys();
	   NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
   }
}
void AttachCtrl::InsertTime(TimeValue ins, TimeValue amount)
{
	if(GetLocked()==false)
	{
		size_t n = keys.length();      
	   HoldTrack();

	   for (int i = 0; i < n; i++) {    
		  if (keys[i].time >= ins) {
			 keys[i].time += amount;
			 }     
		  }
	   Invalidate();
	   SortKeys();
	   NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
   }
}

void AttachCtrl::DeleteKeys(DWORD flags)
{
	if(GetLocked()==false)
	{
		int n = (int)keys.length(); // downcast to 2G here.
		HoldTrack();

		for (int i = n-1; i >= 0; i--) {
			if (flags&TRACK_DOALL || keys[i].flags&KEY_SELECTED) {
				keys.removeAt(i);
			}
		}
		Invalidate();
		NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
	}
}

void AttachCtrl::DeleteKeyByIndex(int index)
{
	if(GetLocked()==false)
	{
	   HoldTrack();
		keys.removeAt(index);
	   NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
	}
}

void AttachCtrl::SelectKeyByIndex(int i,BOOL sel)
   {
   HoldTrack();
   if (sel) keys[i].flags |=  KEY_SELECTED;
   else     keys[i].flags &= ~KEY_SELECTED;
   NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
   }

void AttachCtrl::SelectKeys(TrackHitTab& sel, DWORD flags)
   {
   HoldTrack();
   
   if (flags&SELKEYS_CLEARKEYS) {
		size_t n = keys.length();
      for (int i = 0; i < n; i++ ) {
         keys[i].flags &= ~KEY_SELECTED;
         }
      }
   
   if (flags&SELKEYS_DESELECT) {
      for (int i = 0; i < sel.Count(); i++ ) {        
         keys[sel[i].hit].flags &= ~KEY_SELECTED;
         }     
      }  
   if (flags&SELKEYS_SELECT) {         
      for (int i = 0; i < sel.Count(); i++ ) {
         keys[sel[i].hit].flags |= KEY_SELECTED;
         }
      }  
   NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
   }

void AttachCtrl::FlagKey(TrackHitRecord hit)
   {
	size_t n = keys.length();
   for (int i = 0; i < n; i++) {
      keys[i].flags &= ~KEY_FLAGGED;
      }
   assert(hit.hit>=0&&hit.hit<(DWORD)n);
   keys[hit.hit].flags |= KEY_FLAGGED;
   }

int AttachCtrl::GetFlagKeyIndex()
   {
	size_t n = keys.length();
   for (int i = 0; i < n; i++) {
      if (keys[i].flags & KEY_FLAGGED) {
         return i;
         }
      }
   return -1;
   }

int AttachCtrl::NumSelKeys()
   {
	size_t n = keys.length();
   int c = 0;
   for ( int i = 0; i < n; i++ ) {
      if (keys[i].flags & KEY_SELECTED) {
         c++;
         }
      }
   return c;
   }

void AttachCtrl::CloneSelectedKeys(BOOL offset)
{
	if(GetLocked()==false)
	{
		size_t n = keys.length();
	   HoldTrack();
	   BOOL changed = FALSE;

		int num_selected = 0;
	   for (int i = 0; i < n; i++) {
		  if (keys[i].flags & KEY_SELECTED) {
				num_selected++;
			}
		}

		// allocate memory only once here
		MaxSDK::Array<AKey> selected_keys(num_selected);

		for (int i = 0; i < n; i++) {
			if (keys[i].flags & KEY_SELECTED) {
				keys[i].flags &= ~KEY_SELECTED;
				// create a new key
			 AKey key(keys[i]);
			 key.flags |= KEY_SELECTED;
				selected_keys[i] = key; // use assignment instead of append to increase performance
			 changed = TRUE;
			 }
		  }

		keys.append(selected_keys);

	   if (changed) {
		  Invalidate();
		  SortKeys();
		  }
   }
}

void AttachCtrl::AddNewKey(TimeValue t,DWORD flags)
{
	if(GetLocked()==false)
	{
	   HoldTrack();
	   AKey key;
	   key.time = t;
	   key.face = 0;
	   key.u0 = key.u1 = 1.0f/3.0f;  
	   key.tens = key.cont = key.bias = key.easeIn = key.easeOut = 0.0f;

	   // Try to find a nearby key
		if (keys.length()) {
			size_t n0, n1;
		  GetInterpVal(t,n0,n1);
		  key.face = keys[n0].face;
		  key.u0   = keys[n0].u0;
		  key.u1   = keys[n0].u1;
		  }

	   if (flags&ADDKEY_SELECT) {
		  key.flags |= KEY_SELECTED;
		  }
	   if (flags&ADDKEY_FLAGGED) {
			size_t n = keys.length();
			for (size_t i = 0; i < n; i++) {
			 keys[i].flags &= ~KEY_FLAGGED;
			 }     
		  key.flags |= KEY_FLAGGED;
		  }
		keys.append(key);
	   Invalidate();
	   SortKeys();
	   NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);   
   }
}

BOOL AttachCtrl::IsKeySelected(int index)
   {
   return keys[index].flags & KEY_SELECTED;
   }

TrackClipObject *AttachCtrl::CopyTrack(Interval iv, DWORD flags)
  {  
   AttachClipObject *cobj = new AttachClipObject(iv); 
   Interval test = TestInterval(iv,flags);   
	for (int i = 0; i < keys.length(); i++) {
      if (test.InInterval(keys[i].time)) {
         AKey nk(keys[i]);
			cobj->tab.append(nk);
         }
      }
   return cobj;
   }

void AttachCtrl::PasteTrack(TrackClipObject *cobj,Interval iv, DWORD flags)
{
	if(GetLocked()==false)
	{
	   AttachClipObject *cob = (AttachClipObject*)cobj;   
	   HoldTrack();      
	   DeleteTime(iv,flags);   
	   InsertTime(iv.Start(),cob->clip.Duration()-1);  
		for (int i = 0; i < cob->tab.length(); i++) {
		  AKey key(cob->tab[i]);
		  key.time -= cob->clip.Start() - iv.Start();
			keys.append(key);
		  }  
	   Invalidate();
	   SortKeys();
	   NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
	}
}

int AttachCtrl::HitTestTrack(       
      TrackHitTab& hits,
      Rect& rcHit,
      Rect& rcTrack,       
      float zoom,
      int scroll,
      DWORD flags)
   {
   int left  = ScreenToTime(rcTrack.left,zoom,scroll) - 4;
   int right = ScreenToTime(rcTrack.right,zoom,scroll) + 4;
	size_t n = keys.length();
   int y = (rcTrack.top+rcTrack.bottom)/2;   
      
   for ( int i = 0; i < n; i++ ) {
      if (flags&HITTRACK_SELONLY && 
         !(keys[i].flags & KEY_SELECTED)) continue;
      if (flags&HITTRACK_UNSELONLY && 
         (keys[i].flags & KEY_SELECTED)) continue;

      if (keys[i].time > right) {
         break;
         }
      if (keys[i].time > left) {
         int x = TimeToScreen(keys[i].time,zoom,scroll);
         if (rcHit.Contains(IPoint2(x,y))) {
            TrackHitRecord rec(i,0);
            hits.Append(1,&rec);
            if (flags&HITTRACK_ABORTONHIT) return TRACK_DONE;
            }
         }     
      }
   return TRACK_DONE;
   }

int AttachCtrl::PaintTrack(         
      ParamDimensionBase *dim,
      HDC hdc,
      Rect& rcTrack,
      Rect& rcPaint,
      float zoom,
      int scroll,
      DWORD flags)
   {
   int left  = ScreenToTime(rcPaint.left-8,zoom,scroll);
   int right = ScreenToTime(rcPaint.right+8,zoom,scroll);
	size_t n = keys.length();
   int y = (rcTrack.top+rcTrack.bottom)/2, x = rcPaint.left-2;
   HBRUSH selBrush   = CreateSolidBrush(RGB(255,255,255));  
   HBRUSH unselBrush = (HBRUSH)GetStockObject(GRAY_BRUSH);     
   SelectObject(hdc,GetStockObject(BLACK_PEN)); 
      
   for (int i = 0; i < n; i++) {
      if (keys[i].time > right) {
         break;
         }
      if (keys[i].time > left) {       
         x  = TimeToScreen(keys[i].time,zoom,scroll);
         if ((flags&PAINTTRACK_SHOWSEL) && (keys[i].flags&KEY_SELECTED)) {
            SelectObject(hdc,selBrush);
         } else {
            SelectObject(hdc,unselBrush);
            }
         Ellipse(hdc,x-4,y-5,x+4,y+5);
         }     
      }

   DeleteObject(selBrush);   
   return TRACK_DONE;
   }


static int __cdecl CompareAKeys(const AKey *k1, const AKey *k2)
   {
   if (k1->time < k2->time) return -1;
   if (k1->time > k2->time) return 1;
   return 0;
   }

//local function no need to locked, already done so via exposed interfaces.
void AttachCtrl::SortKeys()
   {
	keys.sort((CompareFnc)CompareAKeys);
   }


RefResult AttachCtrl::NotifyRefChanged(
      Interval valid, RefTargetHandle targ, 
      PartID &partID, RefMessage msg) 
   {
   switch (msg) {
      case REFMSG_CHANGE:
         Invalidate();
         break;

      case REFMSG_TARGET_DELETED:
         node = NULL;
         break;

      case REFMSG_OBJECT_CACHE_DUMPED:
         
         return REF_STOP;

      break;

      }
   return REF_SUCCEED;
   }

RefTargetHandle AttachCtrl::GetReference(int i)
   {
   if (i<Control::NumRefs()) {
      return Control::GetReference(i);
   } else {
      return node;
      }
   }

void AttachCtrl::SetReference(int i, RefTargetHandle rtarg)
   {
   if (i<Control::NumRefs()) {
      Control::SetReference(i,rtarg);
   } else {
      node = (INode*)rtarg;
      }
   }

Interval AttachCtrl::CompValidity(TimeValue t)
   {
   Interval v = FOREVER;
	if (keys.length()) {
      if (t<keys[0].time) {
         v.Set(TIME_NegInfinity,keys[0].time);
      } else 
			if (t>keys[keys.length()-1].time) {
				v.Set(keys[keys.length()-1].time,TIME_PosInfinity);
      } else {
         v.SetInstant(t);
         }     
      }  
   return v;
   }

void AttachCtrl::GetValue(
      TimeValue t, void *val, Interval &valid, GetSetMethod method)
   {
   TimeValue oldTime = t;
   t = ApplyEase(t,valid); 
   if (!this->valid.InInterval(t)) {
      this->valid = CompValidity(t);
      PrepareTrack(t);
      this->val = PointOnPath(t);
      if (align) {
         qval = QuatOnPath(t);
         }           
      if (this->valid.Empty()) this->valid.Set(oldTime,oldTime);
      }
   if (method==CTRL_RELATIVE) {
      Matrix3 *tm = (Matrix3*)val;
      if (align) qval.MakeMatrix(*tm);    
      tm->SetTrans(this->val);
   } else {
      *((Point3*)val) = this->val;
      }
   valid &= this->valid;
   }

float AttachCtrl::GetInterpVal(TimeValue t, size_t& n0, size_t& n1)
   {
   float u=0.0f;

	for (int i=0; i<keys.length(); i++) {
      if (keys[i].time>t) {
         if (i) {
            n0 = i-1;
            n1 = i;
            u = float(t-keys[i-1].time)/
               float(keys[i].time-keys[i-1].time);
            u = Ease(u,keys[i-1].easeOut,keys[i].easeIn);
            return u;
         } else {
            n0 = n1 = 0;
            return u;
            }
         }
      }
	n0 = n1 = keys.length() - 1;
   return u;
   }

Point3 AttachCtrl::PointOnPath(TimeValue t)
   {
   Point3 p(0,0,0);
	if (keys.length()) {
		size_t n0, n1;
      float u = GetInterpVal(t,n0,n1);
      float v[4];
      ComputeHermiteBasis(u, v);
      p = keys[n0].pos  * v[0] + keys[n1].pos * v[1] +
         keys[n0].dout * v[2] + keys[n1].din * v[3];     
      }
   return p;
   }

Quat AttachCtrl::QuatOnPath(TimeValue t)
   {
   Quat q = IdentQuat();
	if (keys.length()) {
		size_t n0, n1;
      float u = GetInterpVal(t,n0,n1);
      Quat qd = keys[n1].quat/keys[n0].quat;
      float ang;
      Point3 axis;
      AngAxisFromQ(qd, &ang, axis);
      q = squadrev(
         ang, axis, 
         keys[n0].quat, keys[n0].qa, keys[n1].qb, keys[n1].quat, u); 
      }
   return q;
   }

static void MakeAngPositive(float &ang, Point3 &axis) 
   {
   if (ang < 0.0f) {    
      ang  = -ang;
      axis = -axis;
      }
   }

static void MakeAngLessThan180(float &ang, Point3 &axis) 
   {  
   MakeAngPositive(ang,axis); 
   while (ang > TWOPI) ang -= TWOPI;
   if (fabs(ang) > PI) ang -= TWOPI;   
   MakeAngPositive(ang,axis);
   }

static Quat ConstructOrientation(Point3 norm, Point3 dir)
   {
   // RB 11/28/2000: Fixed to handle case where norm and dir were the same.
   Point3 yAxis = norm^dir;
   Matrix3 tm(1);
   tm.SetRow(2,norm);
   if (Length(yAxis) > 0.0f) {
      tm.SetRow(1,Normalize(yAxis));
   } else {
      // Just use 'dir' vector
      
      // Find smallest component and construct a unit vector in that direction
      int ix = dir.MinComponent();     
      yAxis = Point3(0,0,0);
      yAxis[ix] = 1.0f;

      // Make it perp to dir.
      Point3 cp = CrossProd(yAxis, dir);
      yAxis = Normalize(CrossProd(dir, cp));

      tm.SetRow(1,yAxis);
      }
   tm.SetRow(0,tm.GetRow(1)^tm.GetRow(2));
   Quat q(tm);
   float ang;
   Point3 axis;
   AngAxisFromQ(q, &ang, axis);  
   MakeAngLessThan180(ang, axis);
   return QFromAngAxis(ang,axis);
   }


void AttachCtrl::PrepareTrack(TimeValue t)
   {
	if (trackValid && keys.length()!=1) return;
   trackValid = TRUE;
   if (!node) return;
   if (manUpdate && !doManUpdate) return;
   doManUpdate = FALSE;

   // First compute key positions and normals
	for (int i=0; i<keys.length(); i++) {
      AKey &k = keys[i];

      // Evaluate the node
      ObjectState os = node->EvalWorldState(
			keys.length()==1 ? t : k.time);      
      TriObject *obj = NULL;
      BOOL needsDel = FALSE;
      
      // Convert it to a tri object
      if (os.obj->IsSubClassOf(triObjectClassID)) {
         obj = (TriObject*)os.obj;
         needsDel = FALSE;
      } else {
         if (os.obj->CanConvertToType(triObjectClassID)) {
            Object *oldObj = os.obj;
            obj = (TriObject*)
               os.obj->ConvertToType(
					keys.length()==1 ? t : k.time,triObjectClassID);
            needsDel = (obj != oldObj);
            }
         }
      
      // Get the position on the face
      if (obj) {                 
         Interval iv = FOREVER;
         Matrix3 tm = node->GetObjTMAfterWSM(
				keys.length()==1 ? t : k.time, &iv);
			if (keys.length()==1) {
            // Special case for only one key
            valid &= iv;
            valid &= obj->ChannelValidity(t,GEOM_CHAN_NUM);
            valid &= obj->ChannelValidity(t,TOPO_CHAN_NUM);
            }
         Mesh &m = obj->GetMesh();
         if (m.getNumFaces()) {
            DWORD f = k.face % m.getNumFaces();

            // new code from Larry!
            Point3 v0 = m.verts[m.faces[f].v[0]] * tm;
            Point3 v1 = m.verts[m.faces[f].v[1]] * tm;
            Point3 v2 = m.verts[m.faces[f].v[2]] * tm;
            k.pos = v0 * k.u0 +
                  v1 * k.u1 +
                  v2 * (1.0f-k.u0-k.u1);
            k.norm = Normalize((v1-v0)^(v2-v1));
            k.dir  = Normalize(v1-v0);

         } else {
            k.pos  = Point3(0,0,0) * tm;
            k.norm = Point3(0,0,1);
            k.dir  = Point3(1,0,0);
            }
         
         if (needsDel) obj->DeleteThis();
         }
      }

   // Next compute position derivatives
	switch (keys.length()) {
      case 0: break;
      case 1:
         keys[0].din = keys[0].dout = Point3(0,0,0); 
         break;

      case 2:
         Comp2KeyDeriv();
         break;

      default:
		size_t num_keys = keys.length();
		if (num_keys > 0)
		{
			for (int i=1; i < num_keys - 1; i++) 
				CompMiddleDeriv(i);
		}
         CompFirstDeriv();
         CompLastDeriv();
         break;
      }

   // Compute orientations
   if (align) {
      // Compute orientations at each key
		for (int i=0; i<keys.length(); i++) {
/*       
         Point3 dir;
			if (i<keys.length()-1) {
            dir = Normalize(
               PointOnPath(keys[i].time+1)-
               PointOnPath(keys[i].time));
         } else {
            dir = Normalize(
               PointOnPath(keys[i].time)-
               PointOnPath(keys[i].time-1));
            }
         keys[i].quat = ConstructOrientation(keys[i].norm, dir);
*/
         keys[i].quat = ConstructOrientation(keys[i].norm, keys[i].dir);
         }

      // Now compute quat tangents
		if (keys.length()==1) keys[0].qa = keys[0].qb = IdentQuat();
		else for (int i=0; i<keys.length(); i++) CompAB(i);
      }
   trackValid = TRUE;
   }

void AttachCtrl::CompFirstDeriv()
   {
   float t;
   t = 0.5f * (1.0f - keys[0].tens);      
   keys[0].dout = 
      t * (3.0f * (keys[1].pos - keys[0].pos) - keys[1].din);  
   }

void AttachCtrl::CompLastDeriv()
   {
   float t;
	size_t n = keys.length();
   t = 0.5f * (1.0f - keys[n-1].tens);    
   keys[n-1].din = 
      -t * (3.0f * (keys[n-2].pos - keys[n-1].pos) + keys[n-2].dout);   
   }

void AttachCtrl::Comp2KeyDeriv()
   {  
   float t0, t1;
   Point3 dp = keys[1].pos - keys[0].pos; 
   t0 = 1.0f - keys[0].tens;
   t1 = 1.0f - keys[1].tens;           
   keys[0].dout = t0 * dp;
   keys[1].din  = t1 * dp;
   }

void AttachCtrl::CompMiddleDeriv(int i)
   {
   float tm,cm,cp,bm,bp,tmcm,tmcp,ksm,ksp,kdm,kdp,c;
   float dt,fp,fn;
	size_t n = keys.length();   

   /* fp,fn apply speed correction when continuity is 0.0 */   
   dt = .5f * (float)(keys[i+1].time - keys[i-1].time);
   fp = ((float)(keys[i].time - keys[i-1].time)) / dt;
   fn = ((float)(keys[i+1].time - keys[i].time)) / dt;
   
   c  = float(fabs(keys[i].cont));
   fp = fp + c - c * fp;
   fn = fn + c - c * fn;
   cm = 1.0f - keys[i].cont;  
   tm = 0.5f * (1.0f - keys[i].tens);
   cp = 2.0f - cm;
   bm = 1.0f - keys[i].bias;
   bp = 2.0f - bm;      
   tmcm = tm*cm;  tmcp = tm*cp;
   ksm = tmcm*bp*fp; ksp = tmcp*bm*fp;
   kdm = tmcp*bp*fn;    kdp = tmcm*bm*fn;
   
   Point3 delm, delp;   
   delm = keys[i].pos   - keys[i-1].pos;
   delp = keys[i+1].pos - keys[i].pos;
   keys[i].din  = ksm*delm + ksp*delp;
   keys[i].dout = kdm*delm + kdp*delp; 
   }


static float Ease(float u, float a, float b) 
   {
   float k;
   float s = a + b;
   
   if (u==0.0f || u==1.0f) return u;
   if (s == 0.0) return u;
   if (s > 1.0f) {
      a = a/s;
      b = b/s;
      }
   k = 1/(2.0f - a - b);
   if (u < a) return ((k/a)*u*u);
   else if (u < 1.0f - b) return (k*(2.0f*u - a));
   else {
      u = 1.0f - u;
      return (1.0f - (k/b) *u*u);
      }
   }

static void ComputeHermiteBasis(float u, float *v) 
   {
   float u2,u3,a;
   
   u2 = u*u;
   u3 = u2*u;
   a  = 2.0f*u3 - 3.0f*u2;
   v[0] = 1.0f + a;
   v[1] = -a;
   v[2] = u - 2.0f*u2 + u3;
   v[3] = -u2 + u3;
   }

static float qdot(Quat p, Quat q) 
   {
   return (q[0]*p[0] + q[1]*p[1] + q[2]*p[2] + q[3]*p[3]);
   }

void AttachCtrl::CompAB(int i)
   {  
	size_t n = keys.length();
   Quat qprev, qnext, q;
   Quat  qp, qm, qa, qb, qae, qbe;  
   float tm,cm,cp,bm,bp,tmcm,tmcp,ksm,ksp,kdm,kdp,c;
   float dt,fp,fn;
         
   if (i!=0) {
      if (qdot(keys[i-1].quat, keys[i].quat) < 0.0f) 
          qprev = -keys[i-1].quat;
      else qprev =  keys[i-1].quat;
      qm = LnDif(qprev,keys[i].quat);        
      }
   if (i<n-1) {
      if (qdot(keys[i+1].quat, keys[i].quat) < 0.0f) 
         qnext  = -keys[i+1].quat;
      else qnext =  keys[i+1].quat;
      qp = LnDif(keys[i].quat, qnext);
      }
   if (i==0) qm = qp;
   if (i==1) qp = qm;
   
   fp = fn = 1.0f;
   cm = 1.0f - keys[i].cont;  
   
   if (i>0 && i<n-1) {
      dt = 0.5f * float(keys[i+1].time - keys[i-1].time);
      fp = float(keys[i].time - keys[i-1].time)/dt;
      fn = float(keys[i+1].time - keys[i].time)/dt;
      c = float(fabs(keys[i].cont));
      fp = fp + c - c * fp;
      fn = fn + c - c * fn;
      }
   
   tm = .5f*(1.0f - keys[i].tens);
   cp = 2.0f - cm;
   bm = 1.0f - keys[i].bias;
   bp = 2.0f - bm;      
   tmcm = tm * cm;   
   tmcp = tm * cp;
   ksm  = 1.0f - tmcm * bp * fp; 
   ksp  = -tmcp * bm * fp;
   kdm  = tmcp * bp * fn;  
   kdp  = tmcm * bm * fn - 1.0f;
   
   for (int j = 0; j < 4; j++) {
      qa[j] = .5f * (kdm * qm[j] + kdp * qp[j]);
      qb[j] = .5f * (ksm * qm[j] + ksp * qp[j]);
      }

   qae = Exp(qa);
   qbe = Exp(qb);
   
   keys[i].qa = keys[i].quat * qae;
   keys[i].qb = keys[i].quat * qbe; 
   }

BOOL AttachCtrl::SetObject(INode *newNode)
   {
   if (node == newNode)
      return TRUE;
   if (newNode->TestForLoop(FOREVER,this)==REF_SUCCEED) {
      ReplaceReference(Control::NumRefs(),(RefTargetHandle)newNode);
      NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
      Invalidate();
      return TRUE;
   } else {
      return FALSE;
      }
   }

INode* AttachCtrl::GetObject()
   {  
      return node;
   }

void AttachCtrl::SetKeyPos(TimeValue t, DWORD fi, Point3 bary)
   {
   if (manUpdate) doManUpdate = TRUE;
   int index = GetKeyIndex(t);
   if (index<0) {
      AddNewKey(t,0);
      index = GetKeyIndex(t);
      }
   if (index>=0) {
      keys[index].face = fi;
      keys[index].u0   = bary.x;
      keys[index].u1   = bary.y;
      Invalidate();
      NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
      }
   }

AKey* AttachCtrl::GetKey(int index)
   {
	if (index >= 0 && index < keys.length())
      return &keys[index];
   else
      {
      DbgAssert(false);
      return NULL;
      }
   }

BOOL AttachCtrl::GetAlign()
   {
   return align;
   }

void AttachCtrl::SetAlign(BOOL newAlign)
   {
   if (align != newAlign) {
      align = newAlign;
      NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
      Invalidate();
      }
   }

BOOL AttachCtrl::GetManualUpdate()
   {
   return manUpdate;
   }

void AttachCtrl::SetManualUpdate(BOOL newManUpdate)
   {
   if (manUpdate != newManUpdate) {
      manUpdate = newManUpdate;
      NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
      Invalidate();
      }
   }

void AttachCtrl::Invalidate(BOOL forceUpdate)
   {  
   if (forceUpdate)
      doManUpdate = TRUE;
   valid.SetEmpty();
   trackValid = FALSE;
   InvalidateUI();
   }

//--- UI -------------------------------------------------------------

#define WM_GETBARY   (WM_USER+0xa38c)
#define WM_SETBARY   (WM_USER+0xa38d)

#define BARYGRAPH_UP 1
#define BARYGRAPH_DOWN  2
#define BARYGRAPH_MOVE  3

static LRESULT CALLBACK BaryGraphWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static INT_PTR CALLBACK AttachParamDialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#define ToTCBUI(a) (((a)+1.0f)*25.0f)
#define FromTCBUI(a) (((a)/25.0f)-1.0f)
#define ToEaseUI(a) ((a)*50.0f)
#define FromEaseUI(a) ((a)/50.0f)

void RegisterClasses()
   {
   static BOOL registered=FALSE;
   if (!registered) {
      InitTCBGraph(hInstance);
      
      WNDCLASS  wc;
      wc.style         = 0;
      wc.hInstance     = hInstance;
      wc.hIcon         = NULL;
      wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
      wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
      wc.lpszMenuName  = NULL;
      wc.cbClsExtra    = 0;
      wc.cbWndExtra    = 0;
      wc.lpfnWndProc   = BaryGraphWindowProc;
      wc.lpszClassName = _T("FacePosGraph");
      RegisterClass(&wc);
      
      registered = TRUE;
      }
   }

void AttachCtrl::BeginEditParams(
      IObjParam *ip, ULONG flags,Animatable *prev)
{
	if(GetLocked()==false)
	{
	   RegisterClasses();
	   this->ip = ip;
	   editCont = this;
	   uiValid  = FALSE;
	   hWnd = ip->AddRollupPage( 
		  hInstance, 
		  MAKEINTRESOURCE(IDD_ATTACH_PARAMS),
		  AttachParamDialogProc,
		  GetString(IDS_RB_ATTACHPARAMS), 
		  (LPARAM)this );
	   ip->RegisterTimeChangeCallback(this);
	   pickObMode = new PickObjectMode(this);
	   setPosMode = new SetPosCMode(this,ip);
	}
}

void AttachCtrl::EndEditParams(
      IObjParam *ip, ULONG flags,Animatable *next)
{  
   if(editCont) //otherwise it's been closed alreay
   {
	   ip->ClearPickMode();
	   ip->DeleteMode(setPosMode);
	   ip->UnRegisterTimeChangeCallback(this);   
	   if (hWnd) ip->DeleteRollupPage(hWnd);     
	   hWnd     = NULL;
	   ip       = NULL;
	   editCont = NULL;
	   if(pickObMode)
		   delete pickObMode;
	   pickObMode = NULL;
	   if(setPosMode)
		   delete setPosMode;
	   setPosMode = NULL;
   }
}

void AttachCtrl::SpinnerChange(int id)
   {
   HoldTrack();
// assert(index>=0);
   if (index < 0) {
      AddNewKey(ip->GetTime(),0);
      index = GetKeyIndex(ip->GetTime());
   }
   if (index >=0){
   switch (id) {
      case IDC_ATTACH_KEYTIMESPIN:
         keys[index].time = iTime->GetIVal();
         ip->SetTime(keys[index].time);
         break;
      case IDC_ATTACH_KEYFACESPIN:
         keys[index].face = iFace->GetIVal() - 1;
         break;
      case IDC_ATTACH_KEYASPIN:
         keys[index].u0 = iA->GetFVal();
         UpdateBaryGraph();
         break;
      case IDC_ATTACH_KEYBSPIN:
         keys[index].u1 = iB->GetFVal();
         UpdateBaryGraph();
         break;
      case IDC_ATTACH_KEYTENSSPIN:
         keys[index].tens = FromTCBUI(iTens->GetFVal());
         UpdateTCBGraph();
         break;
      case IDC_ATTACH_KEYCONTSPIN:
         keys[index].cont = FromTCBUI(iCont->GetFVal());
         UpdateTCBGraph();
         break;
      case IDC_ATTACH_KEYBIASSPIN:
         keys[index].bias = FromTCBUI(iBias->GetFVal());
         UpdateTCBGraph();
         break;
      case IDC_ATTACH_KEYEASETOSPIN:
         keys[index].easeIn = FromEaseUI(iEaseTo->GetFVal());
         UpdateTCBGraph();
         break;
      case IDC_ATTACH_KEYEASEFROMSPIN:
         keys[index].easeOut = FromEaseUI(iEaseFrom->GetFVal());
         UpdateTCBGraph();
         break;
      }
   }
   Invalidate();
   NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
   ip->RedrawViews(ip->GetTime());
   }

TSTR AttachCtrl::SvGetRelTip(IGraphObjectManager *gom, IGraphNode *gNodeTarger, int id, IGraphNode *gNodeMaker)
{
   return SvGetName(gom, gNodeMaker, false) + _T(" -> ") + gNodeTarger->GetAnim()->SvGetName(gom, gNodeTarger, false);
}

bool AttachCtrl::SvCanDetachRel(IGraphObjectManager *gom, IGraphNode *gNodeTarget, int id, IGraphNode *gNodeMaker)
{
   return (GetLocked()!=true); // if locked is false then we return true, else opposite.
}

bool AttachCtrl::SvDetachRel(IGraphObjectManager *gom, IGraphNode *gNodeTarget, int id, IGraphNode *gNodeMaker)
{
   if(GetLocked()==false)
   {
	   if( gNodeTarget->GetAnim() == node ) {
		  ReplaceReference(Control::NumRefs(),(RefTargetHandle)NULL);
		  NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
		  Invalidate();
		  return true;
	   }
   }

   return false;
}

bool AttachCtrl::SvCanConcludeLink(IGraphObjectManager *gom, IGraphNode *gNode, IGraphNode *gNodeChild)
{
	if(GetLocked()==false)
	{
	   Animatable *pChildAnim = gNodeChild->GetAnim();
	   if( pChildAnim->SuperClassID() == BASENODE_CLASS_ID )
		  return true;

	   return Control::SvCanConcludeLink(gom, gNode, gNodeChild);
	}
	return false;
}

bool AttachCtrl::SvLinkChild(IGraphObjectManager *gom, IGraphNode *gNodeThis, IGraphNode *gNodeChild)
{
   if(GetLocked()==false)
   {
	   Animatable *pChildAnim = gNodeChild->GetAnim();
	   if( pChildAnim->SuperClassID() == BASENODE_CLASS_ID ) {
		  SetObject( (INode*)pChildAnim  );
		  return true;
	   }
	   return Control::SvLinkChild(gom, gNodeThis, gNodeChild);
   }
   return false;
}

SvGraphNodeReference AttachCtrl::SvTraverseAnimGraph(IGraphObjectManager *gom, Animatable *owner, int id, DWORD flags)
{
   SvGraphNodeReference nodeRef = Control::SvTraverseAnimGraph( gom, owner, id, flags );

   if( nodeRef.stat == SVT_PROCEED ) {
      if( node )
         gom->AddRelationship( nodeRef.gNode, node, 0, RELTYPE_CONSTRAINT );
   }

   return nodeRef;
}

void AttachCtrl::Command(int id, LPARAM lParam)
   {
   switch (id) {
      case IDC_ATTACH_PICKOB:
         ip->SetPickMode(pickObMode);
         break;
      
      case IDC_PREVKEY:
		if (!keys.length()) 
            break;   // No keys to go to
         if (index < 0){
            if (ip->GetTime() < keys[0].time) {
				index = (int)keys.length()-1;
            }
			else if (ip->GetTime() > keys[keys.length()-1].time) {
				index = (int)keys.length()-1;
            }
            else {
				for (int kk = 0; kk < keys.length(); ++kk){
                  if (ip->GetTime() < keys[kk].time){
                     index = kk-1;
                     break;
                  }
               }
            }

         }
		else if (index==0) 
			index = (int)keys.length()-1;
         else index--;
         ip->SetTime(keys[index].time);
         break;

      case IDC_NEXTKEY:
		if (!keys.length()) 
            break;   // No keys to go to
         if (index < 0){
			if (ip->GetTime() > keys[keys.length()-1].time) {
               index = 0;
            }
            else{
				for (int kk = 0; kk < keys.length(); ++kk){
                  if (ip->GetTime() < keys[kk].time){
                     index = kk;
                     break;
                  }
               }
            }

         }
		else if (index==keys.length()-1) index = 0;
         else index++; // if (index >= 0)
         ip->SetTime(keys[index].time);
         break;
         
      case IDC_ATTACH_SETPOS:
         ICustButton *iPickObj1;
         iPickObj1 = GetICustButton(GetDlgItem(hWnd, IDC_ATTACH_SETPOS));
         setPosButton = iPickObj1->IsChecked();
         ReleaseICustButton(iPickObj1);

         if (!setPosButton && index < 0){
            iFace->Disable();
            iA->Disable();
            iB->Disable();
            EnableWindow(GetDlgItem(hWnd,IDC_ATTACH_FACEPOSGRAPH),FALSE);
         }
         else{
            iFace->Enable();
            iA->Enable();
            iB->Enable();
            EnableWindow(GetDlgItem(hWnd,IDC_ATTACH_FACEPOSGRAPH),TRUE);
         }
         
         if (ip->GetCommandMode()==setPosMode) {
            ip->SetStdCommandMode(CID_OBJMOVE);
         } else {
            ip->SetCommandMode(setPosMode);
            }
         break;

      case IDC_ATTACH_ALIGN:
         align = IsDlgButtonChecked(hWnd,id);
         valid.SetEmpty();
         NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
         ip->RedrawViews(ip->GetTime());
         break;

      case IDC_ATTACH_UPDATE:    
         Invalidate(TRUE);
         NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
         ip->RedrawViews(ip->GetTime());
         break;

      case IDC_ATTACH_MANUPDATE:
         manUpdate = IsDlgButtonChecked(hWnd,IDC_ATTACH_MANUPDATE);
         if (manUpdate) iUpdate->Enable();         
         else iUpdate->Disable();
         if (!manUpdate) {
            // RB 12/01/2000: If we're turning manual update off, make sure we update.
            Invalidate();
            NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
            ip->RedrawViews(ip->GetTime());
            }
         break;

      case IDC_ATTACH_FACEPOSGRAPH:
         switch (LOWORD(lParam)) {
            case BARYGRAPH_DOWN:
               theHold.Begin();
               if (index < 0) {
                  AddNewKey(ip->GetTime(),0);
                  index = GetKeyIndex(ip->GetTime());
               }
               break;

            case BARYGRAPH_UP:
               if (HIWORD(lParam)) theHold.Accept(GetString(IDS_RB_EDITATTACHKEY));
               else theHold.Cancel();
               break;

            case BARYGRAPH_MOVE: {
               HoldTrack();
               Point2 bary;
               SendDlgItemMessage(hWnd,IDC_ATTACH_FACEPOSGRAPH,
                  WM_GETBARY, 0, (LPARAM)&bary);
               keys[index].u0 = bary.x;
               keys[index].u1 = bary.y;
               Invalidate();
               NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
               UpdateBaryGraph();
               ip->RedrawViews(ip->GetTime());
               break;
               }
            }
         break;
      }
   }

void AttachCtrl::InvalidateUI()
   {
   if (editCont==this) {
      InvalidateRect(hWnd,NULL,FALSE);
      uiValid = FALSE;
      }
   }


void AttachCtrl::UpdateTCBGraph()
   {
   if (index>=0 && hWnd) {
      TCBGraphParams tp;
      tp.tens     = keys[index].tens;
      tp.cont     = keys[index].cont;
      tp.bias     = keys[index].bias;
      tp.easeTo   = keys[index].easeIn;
      tp.easeFrom = keys[index].easeOut;
      SendDlgItemMessage(hWnd,IDC_TCB_GRAPH,WM_SETTCBGRAPHPARAMS,0,(LPARAM)&tp);
      UpdateWindow(GetDlgItem(hWnd,IDC_TCB_GRAPH));
      }
   }

void AttachCtrl::UpdateBaryGraph()
   {
   if (index>=0 && hWnd) {
      Point2 pt(keys[index].u0,keys[index].u1);
      SendDlgItemMessage(hWnd,IDC_ATTACH_FACEPOSGRAPH,WM_SETBARY,0,(LONG_PTR)&pt);
      UpdateWindow(GetDlgItem(hWnd,IDC_ATTACH_FACEPOSGRAPH));
      }
   }

void AttachCtrl::UpdateUI()
   {
   if (!uiValid) {
      uiValid = TRUE;

      index = GetKeyIndex(ip->GetTime());
      if (index<0) {
         iTime->Disable();

         if (!setPosButton){
            iFace->Disable();
            iA->Disable();
            iB->Disable();
            EnableWindow(GetDlgItem(hWnd,IDC_ATTACH_FACEPOSGRAPH),FALSE);
         }
         else{
            iFace->Enable();
            iA->Enable();
            iB->Enable();
            EnableWindow(GetDlgItem(hWnd,IDC_ATTACH_FACEPOSGRAPH),TRUE);
         }
         iTens->Disable();
         iCont->Disable();
         iBias->Disable();
         iEaseFrom->Disable();
         iEaseTo->Disable();
         iStat->SetText(_T(""));
         EnableWindow(GetDlgItem(hWnd,IDC_TCB_GRAPH),FALSE);
         
      } else {
         iTime->Enable();
         iFace->Enable();
         iA->Enable();
         iB->Enable();
         iTens->Enable();
         iCont->Enable();
         iBias->Enable();        
         iEaseFrom->Enable();
         iEaseTo->Enable();
         iTime->SetLimits(GetAnimStart(),GetAnimEnd(),FALSE);
         
         TSTR buf;
         buf.printf(_T("%d"),index+1);
         iStat->SetText(buf);

         iTime->SetValue(keys[index].time,FALSE);
         iFace->SetValue((int)keys[index].face + 1,FALSE);
         iA->SetValue(keys[index].u0,FALSE);
         iB->SetValue(keys[index].u1,FALSE);
         iTens->SetValue(ToTCBUI(keys[index].tens),FALSE);
         iCont->SetValue(ToTCBUI(keys[index].cont),FALSE);
         iBias->SetValue(ToTCBUI(keys[index].bias),FALSE);
         iEaseTo->SetValue(ToEaseUI(keys[index].easeIn),FALSE);
         iEaseFrom->SetValue(ToEaseUI(keys[index].easeOut),FALSE);         

         UpdateTCBGraph();
         EnableWindow(GetDlgItem(hWnd,IDC_TCB_GRAPH),TRUE);       
         
         UpdateBaryGraph();
         EnableWindow(GetDlgItem(hWnd,IDC_ATTACH_FACEPOSGRAPH),TRUE);
         }
      // Allow the user to move to the next\prev key even if he's not on a key
      // in the timeline. If there are no keys, clikcing the btns won't do anything.
      iPrev->Enable();
      iNext->Enable();
      CheckDlgButton(hWnd,IDC_ATTACH_ALIGN,align);
      if (node) {
         SetDlgItemText(hWnd,IDC_ATTACH_OBNAME,node->GetName());
      } else {
         SetDlgItemText(hWnd,IDC_ATTACH_OBNAME,GetString(IDS_RB_NONE));
         }

      CheckDlgButton(hWnd,IDC_ATTACH_MANUPDATE,manUpdate);
      if (manUpdate) iUpdate->Enable();      
      else iUpdate->Disable();
      }
   }

void AttachCtrl::SetupWindow(HWND hWnd)
   {
   this->hWnd = hWnd;
   
   iTime = GetISpinner(GetDlgItem(hWnd,IDC_ATTACH_KEYTIMESPIN));
   iTime->SetLimits(GetAnimStart(),GetAnimEnd(),FALSE);
   iTime->SetScale(10.0f);
   iTime->LinkToEdit(GetDlgItem(hWnd,IDC_ATTACH_KEYTIME),EDITTYPE_TIME);

   iFace = GetISpinner(GetDlgItem(hWnd,IDC_ATTACH_KEYFACESPIN));  
   iFace->SetLimits(1,0xfffffff,FALSE);
   iFace->SetScale(0.1f);
   iFace->LinkToEdit(GetDlgItem(hWnd,IDC_ATTACH_KEYFACE),EDITTYPE_INT);
   
   iA = GetISpinner(GetDlgItem(hWnd,IDC_ATTACH_KEYASPIN));
   iA->SetLimits(-999999999.0f,999999999.0f,FALSE);
   iA->SetScale(0.005f);
   iA->LinkToEdit(GetDlgItem(hWnd,IDC_ATTACH_KEYA),EDITTYPE_FLOAT);
   
   iB = GetISpinner(GetDlgItem(hWnd,IDC_ATTACH_KEYBSPIN));
   iB->SetLimits(-999999999.0f,999999999.0f,FALSE);
   iB->SetScale(0.005f);
   iB->LinkToEdit(GetDlgItem(hWnd,IDC_ATTACH_KEYB),EDITTYPE_FLOAT);  

   iTens = GetISpinner(GetDlgItem(hWnd,IDC_ATTACH_KEYTENSSPIN));
   iTens->SetLimits(0.0f,50.0f,FALSE);
   iTens->SetScale(0.1f);
   iTens->LinkToEdit(GetDlgItem(hWnd,IDC_ATTACH_KEYTENS),EDITTYPE_FLOAT);
   
   iCont = GetISpinner(GetDlgItem(hWnd,IDC_ATTACH_KEYCONTSPIN));
   iCont->SetLimits(0.0f,50.0f,FALSE);
   iCont->SetScale(0.1f);
   iCont->LinkToEdit(GetDlgItem(hWnd,IDC_ATTACH_KEYCONT),EDITTYPE_FLOAT);
   
   iBias = GetISpinner(GetDlgItem(hWnd,IDC_ATTACH_KEYBIASSPIN));
   iBias->SetLimits(0.0f,50.0f,FALSE);
   iBias->SetScale(0.1f);
   iBias->LinkToEdit(GetDlgItem(hWnd,IDC_ATTACH_KEYBIAS),EDITTYPE_FLOAT);  

   iEaseTo = GetISpinner(GetDlgItem(hWnd,IDC_ATTACH_KEYEASETOSPIN));
   iEaseTo->SetLimits(0.0f,50.0f,FALSE);
   iEaseTo->SetScale(0.1f);
   iEaseTo->LinkToEdit(GetDlgItem(hWnd,IDC_ATTACH_KEYEASETO),EDITTYPE_FLOAT); 

   iEaseFrom = GetISpinner(GetDlgItem(hWnd,IDC_ATTACH_KEYEASEFROMSPIN));
   iEaseFrom->SetLimits(0.0f,50.0f,FALSE);
   iEaseFrom->SetScale(0.1f);
   iEaseFrom->LinkToEdit(GetDlgItem(hWnd,IDC_ATTACH_KEYEASEFROM),EDITTYPE_FLOAT);   

   iPickOb  = GetICustButton(GetDlgItem(hWnd,IDC_ATTACH_PICKOB));
   iPickOb->SetType(CBT_CHECK);  
   iPickOb->SetHighlightColor(GREEN_WASH);

   iSetPos  = GetICustButton(GetDlgItem(hWnd,IDC_ATTACH_SETPOS));
   iSetPos->SetType(CBT_CHECK);  
   iSetPos->SetHighlightColor(GREEN_WASH);

   iPrev = GetICustButton(GetDlgItem(hWnd,IDC_PREVKEY));
   iNext = GetICustButton(GetDlgItem(hWnd,IDC_NEXTKEY));
   iUpdate  = GetICustButton(GetDlgItem(hWnd,IDC_ATTACH_UPDATE));

   iStat   = GetICustStatus(GetDlgItem(hWnd,IDC_KEYNUM));
   iStat->SetTextFormat(STATUSTEXT_CENTERED);
   }

void AttachCtrl::DestroyWindow()
   {
   ReleaseICustStatus(iStat);
   ReleaseISpinner(iTime);
   ReleaseISpinner(iFace);
   ReleaseISpinner(iA);
   ReleaseISpinner(iB);
   ReleaseISpinner(iTens);
   ReleaseISpinner(iCont);
   ReleaseISpinner(iBias);
   ReleaseICustButton(iPickOb);
   ReleaseICustButton(iSetPos);
   ReleaseICustButton(iPrev);
   ReleaseICustButton(iNext);
   ReleaseICustButton(iUpdate);
   ReleaseISpinner(iEaseTo);
   ReleaseISpinner(iEaseFrom);
   iEaseTo = iEaseFrom = iTime = iFace = iA = 
      iB = iTens = iCont = iBias = NULL;
   iStat = NULL;
   iPickOb = iSetPos = iPrev = iNext = iUpdate = NULL;
   }

static INT_PTR CALLBACK AttachParamDialogProc(
      HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
   {
   AttachCtrl *ctrl = DLGetWindowLongPtr<AttachCtrl*>(hWnd);
   if (!ctrl && msg != WM_INITDIALOG) return FALSE;

   switch (msg) {
      case WM_INITDIALOG:
         ctrl = (AttachCtrl*)lParam;
         DLSetWindowLongPtr(hWnd, lParam);
         ctrl->SetupWindow(hWnd);
         break;

      case WM_DESTROY:
         ctrl->DestroyWindow();
         break;

      case WM_PAINT:
         ctrl->UpdateUI();
         return FALSE;

      case WM_LBUTTONDOWN:case WM_LBUTTONUP: case WM_MOUSEMOVE:
         ctrl->ip->RollupMouseMessage(hWnd,msg,wParam,lParam);
         return FALSE;

      case CC_SPINNER_BUTTONDOWN:         
         theHold.Begin();     
         break;

      case WM_CUSTEDIT_ENTER:
      case CC_SPINNER_BUTTONUP:
         if (HIWORD(wParam)) {
            theHold.Accept(GetString(IDS_RB_EDITATTACHKEY));
         } else {
            theHold.Cancel();
            }
         break;

      case CC_SPINNER_CHANGE:
         ctrl->SpinnerChange(LOWORD(wParam));
         break;

      case WM_COMMAND:
         ctrl->Command(LOWORD(wParam),lParam);
         break;

      default:
         return FALSE;
      }
   return TRUE;
   }


//--- PickObjectMode ------------------------------------------------

BOOL PickObjectMode::Filter(INode *node)
   {
   if (node) {
      //added code for looptest
      if (node->TestForLoop(FOREVER,(ReferenceMaker *) cont)!=REF_SUCCEED)
         return FALSE;

      ObjectState os = node->EvalWorldState(0);
      //added code such that lines are not selected
      if (os.obj->CanConvertToType(triObjectClassID)  && os.obj->SuperClassID() != SHAPE_CLASS_ID) {        
         return TRUE;
         }
      }
   return FALSE;
   }

BOOL PickObjectMode::HitTest(
      IObjParam *ip,HWND hWnd,ViewExp *vpt,IPoint2 m,int flags)
   {
   INode *node = cont->ip->PickNode(hWnd,m, this); //added "this" argument such that the Filter above is used
   return node?TRUE:FALSE;
   }

BOOL PickObjectMode::Pick(IObjParam *ip,ViewExp *vpt)
{
	if ( ! vpt || ! vpt->IsAlive() )
	{	
		// why are we here?
		DbgAssert(!_T("Doing Pick() on invalid viewport!"));
		return FALSE;
	}

	INode *node = vpt->GetClosestHit();
  if (node) {
     theHold.Begin();
     ObjectState os = node->EvalWorldState(0);
     if (os.obj->CanConvertToType(triObjectClassID)) {
        if (cont->SetObject(node)) {                       
           cont->ip->RedrawViews(cont->ip->GetTime());                 
        } else {
           TSTR buf1 = GetString(IDS_RB_ATTACHCONTROLLER);
           TSTR buf2 = GetString(IDS_RB_ILLEGALOBJECT);
           MessageBox(ip->GetMAXHWnd(),buf2,buf1,MB_OK|MB_ICONEXCLAMATION);
           }
        }
     theHold.Accept(GetString(IDS_AG_ATTACHMENT_PICKNODE));
     }
  return TRUE;
}

void PickObjectMode::EnterMode(IObjParam *ip)
   {cont->iPickOb->SetCheck(TRUE);}

void PickObjectMode::ExitMode(IObjParam *ip)
   {cont->iPickOb->SetCheck(FALSE);}


//--- SetPosMode ------------------------------------------------

void SetPosMouseProc::SetPos(HWND hWnd,IPoint2 m)
   {
   if (!cont->node) return;
   ViewExp& vpt = ip->GetViewExp(hWnd);
   if ( ! vpt.IsAlive() ) return;

   Ray ray, wray;
   float at;
   TimeValue t = ip->GetTime();  
   Point3 norm, pt, bary;
   DWORD fi;
   Interval valid;
   
   // Calculate a ray from the mouse point
   vpt.MapScreenToWorldRay(float(m.x), float(m.y),wray);   

   // Back transform the ray into object space.    
   Matrix3 obtm  = cont->node->GetObjTMAfterWSM(t);
   Matrix3 iobtm = Inverse(obtm);
   ray.p   = iobtm * wray.p;
   ray.dir = VectorTransform(iobtm, wray.dir);  

   // Evaluate the node
   ObjectState os = cont->node->EvalWorldState(t);
   TriObject *obj = NULL;
   BOOL needsDel = FALSE;

   // Convert it to a tri object
   if (os.obj->IsSubClassOf(triObjectClassID)) {
      obj = (TriObject*)os.obj;
      needsDel = FALSE;
   } else {
      if (os.obj->CanConvertToType(triObjectClassID)) {
         Object *oldObj = os.obj;
         obj = (TriObject*)
            os.obj->ConvertToType(t,triObjectClassID);
         needsDel = (obj != oldObj);
         }
      }
   if (!obj) return;

   if (obj->GetMesh().IntersectRay(ray, at, norm, fi, bary)) {
      cont->HoldTrack();
      cont->SetKeyPos(t, fi, bary);
      }
   
   if (needsDel) obj->DeleteThis();
}

int SetPosMouseProc::proc(
      HWND hWnd, int msg, int point, int flags, IPoint2 m)
   {
   static BOOL animated = FALSE;
   switch (msg) {
      case MOUSE_POINT:
         if (point==0) {
            animated = FALSE;
			if (!Animating() && cont->keys.length()==1) {
               int index = cont->GetKeyIndex(ip->GetTime());
               if (index<0) {
                  animated = TRUE;
                  }
               }
            theHold.Begin();
            ip->RedrawViews(ip->GetTime(),REDRAW_BEGIN);
         } else {
            if (animated) {
               // following strings moved to resources, 010808  --prs.
               TSTR string1 = GetString(IDS_ARE_YOU_SURE_YOU_WANT);
               TSTR string2 = GetString(IDS_ATTACHMENT_CONTROLLER);
               int res = MessageBox(hWnd, string1, string2, MB_ICONQUESTION|MB_YESNO);
               if (res!=IDYES) {
                  theHold.Cancel();
                  ip->RedrawViews(ip->GetTime(),REDRAW_END);
                  return FALSE;
                  }
               }
            theHold.Accept(GetString(IDS_RB_EDITATTACHKEY));
            ip->RedrawViews(ip->GetTime(),REDRAW_END);
            }
         break;

      case MOUSE_MOVE: {
         theHold.Restore();
         SetPos(hWnd,m);
         ip->RedrawViews(ip->GetTime(),REDRAW_INTERACTIVE);       
         break;
         }

      case MOUSE_ABORT:
         theHold.Cancel();
         ip->RedrawViews(ip->GetTime(),REDRAW_END);
         break;

      case MOUSE_FREEMOVE:       
         SetCursor(ip->GetSysCursor(SYSCUR_SELECT));
         break;
      }
   return TRUE;
   }


void SetPosCMode::EnterMode()
   {
   if (cont->iSetPos) cont->iSetPos->SetCheck(TRUE);
   }

void SetPosCMode::ExitMode()
   {
   if (cont->iSetPos) cont->iSetPos->SetCheck(FALSE);
   }


//--- Barycentric graph ----------------------------------------------

static LRESULT CALLBACK BaryGraphWindowProc(
      HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
   {
   Point2 *bary = DLGetWindowLongPtr<Point2*>(hWnd);
   static int mx, my;
   static Point2 oldBary;
   static BOOL mDown = FALSE;

   switch (msg) {
      case WM_CREATE:
         DLSetWindowLongPtr(hWnd,new Point2(0.33f,0.33f));
         break;

      case WM_SETBARY: {
         Point2 *nb = (Point2*)lParam;
         *bary = *nb;
         InvalidateRect(hWnd,NULL,TRUE);
         break;
         }

      case WM_GETBARY: {
         Point2 *nb = (Point2*)lParam;
         *nb = *bary;
         break;
         }

      case WM_ENABLE:
         InvalidateRect(hWnd,NULL,TRUE);
         break;

      case WM_LBUTTONDOWN:
         mDown = TRUE;
         mx = LOWORD(lParam);
         my = HIWORD(lParam);
         oldBary = *bary;
         SetCapture(hWnd);
         SendMessage(GetParent(hWnd),WM_COMMAND,IDC_ATTACH_FACEPOSGRAPH,
            MAKELPARAM(BARYGRAPH_DOWN,0));
         // Fallthrough

      case WM_MOUSEMOVE:
         if (mDown) {
            Rect rect;
            GetClientRect(hWnd,&rect);
            int x = (short)LOWORD(lParam) - 5;
            int y = (short)HIWORD(lParam) - 5;        
            float u = float(x)/float(rect.w()-11);
            float v = 1.0f-float(y)/float(rect.h()-11);
            
            bary->x = v;            
            bary->y = 1.0f-v-u;
            SendMessage(GetParent(hWnd),WM_COMMAND,IDC_ATTACH_FACEPOSGRAPH,
               MAKELPARAM(BARYGRAPH_MOVE,0));
            }
         break;
      
      case WM_LBUTTONUP:
         mDown = FALSE;
         ReleaseCapture();
         SendMessage(GetParent(hWnd),WM_COMMAND,IDC_ATTACH_FACEPOSGRAPH,
            MAKELPARAM(BARYGRAPH_UP,TRUE));
         break;
      
      case WM_RBUTTONDOWN:
         mDown = FALSE;
         *bary = oldBary;
         ReleaseCapture();
         SendMessage(GetParent(hWnd),WM_COMMAND,IDC_ATTACH_FACEPOSGRAPH,
            MAKELPARAM(BARYGRAPH_UP,FALSE));
         break;

      case WM_PAINT: {
         PAINTSTRUCT ps;
         HDC hdc = BeginPaint(hWnd,&ps);
         Rect rect, orect;
         GetClientRect(hWnd,&rect);
         orect = rect;
         
         if (IsWindowEnabled(hWnd)) {
            rect.left   += 5;
            rect.right  -= 5;
            rect.top    += 5;
            rect.bottom -= 5;
            IPoint2 pt0(rect.left,rect.top);
            IPoint2 pt1(rect.left,rect.bottom);
            IPoint2 pt2(rect.right,rect.bottom);
            SelectObject(hdc,GetStockObject(BLACK_PEN));
            MoveToEx(hdc,pt0.x, pt0.y,NULL);
            LineTo(hdc,pt1.x, pt1.y);
            LineTo(hdc,pt2.x, pt2.y);
            LineTo(hdc,pt0.x, pt0.y);
            
            IPoint2 pt( 
               int(pt0.x*bary->x + pt1.x*bary->y + pt2.x*(1.0f-bary->x-bary->y)),
               int(pt0.y*bary->x + pt1.y*bary->y + pt2.y*(1.0f-bary->x-bary->y)));

            SelectObject(hdc,CreatePen(PS_SOLID,0,RGB(255,0,0)));
            MoveToEx(hdc,pt.x, pt.y-3,NULL);
            LineTo(hdc,pt.x, pt.y+4);
            MoveToEx(hdc,pt.x-3, pt.y,NULL);
            LineTo(hdc,pt.x+4, pt.y);
            DeleteObject(SelectObject(hdc,GetStockObject(BLACK_PEN)));
            }

         WhiteRect3D(hdc,orect,TRUE);
         EndPaint(hWnd,&ps);
         break;
         }

      case WM_DESTROY:
         delete bary;
         break;

      default:
         return DefWindowProc(hWnd,msg,wParam,lParam);
      }

   return 0;
   }

//--- Key Dialog ----------------------------------------------------


static INT_PTR CALLBACK AttachDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#define ATTACHDLG_CLASS_ID Class_ID(0x8736fff2,0xaab37f23)

class AttachKeyDlg : public ReferenceMaker {
   public:
      HWND hWnd;
      AttachCtrl *cont;
      IObjParam *ip;
      BOOL valid;
      ISpinnerControl *iTime, *iFace, *iA, *iB, *iTens, *iCont, *iBias, *iEaseTo, *iEaseFrom;
      ICustButton *iPrev, *iNext;
      ICustStatus *iStat;

      AttachKeyDlg(AttachCtrl *c, const TCHAR *pname,IObjParam *ip,HWND hParent);
      ~AttachKeyDlg();

      Class_ID ClassID() {return ATTACHDLG_CLASS_ID;}
      SClass_ID SuperClassID() {return REF_MAKER_CLASS_ID;}
      RefResult NotifyRefChanged(Interval iv, RefTargetHandle hTarg,PartID& partID,RefMessage msg);
      int NumRefs() {return 1;}
      RefTargetHandle GetReference(int i) {return cont;}
private:
      virtual void SetReference(int i, RefTargetHandle rtarg) {cont=(AttachCtrl*)rtarg;}
public:

      void MaybeCloseWindow();
      void Invalidate();
      void Update();
      void UpdateBaryGraph(float a, float b);
      void UpdateTCBGraph(float t, float c, float b, float easeIn, float easeOut);
      void SetupUI(HWND hWnd);
      void Command(int id, LPARAM lParam);
      void SpinnerChange(int id);      
   };

void AttachCtrl::EditTrackParams(
         TimeValue t,
         ParamDimensionBase *dim,
         const TCHAR *pname,
         HWND hParent,
         IObjParam *ip,
         DWORD flags)
{
	if(GetLocked()==false)
	{
		RegisterClasses();
		new   AttachKeyDlg(this,pname,ip,hParent);
	}
}

AttachKeyDlg::AttachKeyDlg(
      AttachCtrl *c, const TCHAR *pname,IObjParam *ip,HWND hParent)
   {
   valid = FALSE;
   cont  = NULL;
   hWnd  = NULL;
   this->ip = ip;
   theHold.Suspend();
   ReplaceReference(0,c);
   theHold.Resume();
   hWnd = CreateDialogParam(
      hInstance,
      MAKEINTRESOURCE(IDD_ATTACH_DIALOG),
      hParent,
      AttachDlgProc,
      (LPARAM)this); 
   TSTR title = TSTR(GetString(IDS_RB_ATTACHTITLE)) + pname;
   SetWindowText(hWnd,title);
   }

AttachKeyDlg::~AttachKeyDlg()
   {
   theHold.Suspend();
   DeleteAllRefsFromMe();
   theHold.Resume();
   ReleaseICustStatus(iStat);
   ReleaseISpinner(iTime);
   ReleaseISpinner(iFace);
   ReleaseISpinner(iA);
   ReleaseISpinner(iB);
   ReleaseISpinner(iTens);
   ReleaseISpinner(iCont);
   ReleaseISpinner(iBias); 
   ReleaseICustButton(iPrev);
   ReleaseICustButton(iNext);
   ReleaseISpinner(iEaseTo);
   ReleaseISpinner(iEaseFrom);
   }

class CheckForNonAttachDlg : public DependentEnumProc {
   public:     
      BOOL non;
      ReferenceMaker *me;
      CheckForNonAttachDlg(ReferenceMaker *m) {non = FALSE;me = m;}
      int proc(ReferenceMaker *rmaker) {
         if (rmaker==me) return DEP_ENUM_CONTINUE;
         if (rmaker->SuperClassID()!=REF_MAKER_CLASS_ID &&
            rmaker->ClassID()!=ATTACHDLG_CLASS_ID) {
            non = TRUE;
            return DEP_ENUM_HALT;
            }
		 return DEP_ENUM_SKIP; // just look at direct dependents
         }
   };
void AttachKeyDlg::MaybeCloseWindow()
   {
   CheckForNonAttachDlg check(cont);
   cont->DoEnumDependents(&check);
   if (!check.non) {
      PostMessage(hWnd,WM_CLOSE,0,0);
      }
   }


void AttachKeyDlg::SpinnerChange(int id)
   {
   cont->HoldTrack();
   
	for (int index=0; index<cont->keys.length(); index++) {
      if (!(cont->keys[index].flags&KEY_SELECTED)) continue;
      switch (id) {
         case IDC_ATTACH_KEYTIMESPIN:
            cont->keys[index].time = iTime->GetIVal();            
            break;
         case IDC_ATTACH_KEYFACESPIN:
            cont->keys[index].face = iFace->GetIVal() - 1;
            break;
         case IDC_ATTACH_KEYASPIN:
            cont->keys[index].u0 = iA->GetFVal();           
            break;
         case IDC_ATTACH_KEYBSPIN:
            cont->keys[index].u1 = iB->GetFVal();           
            break;
         case IDC_ATTACH_KEYTENSSPIN:
            cont->keys[index].tens = FromTCBUI(iTens->GetFVal());          
            break;
         case IDC_ATTACH_KEYCONTSPIN:
            cont->keys[index].cont = FromTCBUI(iCont->GetFVal());          
            break;
         case IDC_ATTACH_KEYBIASSPIN:
            cont->keys[index].bias = FromTCBUI(iBias->GetFVal());          
            break;
         case IDC_ATTACH_KEYEASETOSPIN:
            cont->keys[index].easeIn = FromEaseUI(iEaseTo->GetFVal());           
            break;
         case IDC_ATTACH_KEYEASEFROMSPIN:
            cont->keys[index].easeOut = FromEaseUI(iEaseFrom->GetFVal());           
            break;
         }
      }

   cont->Invalidate();
   cont->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
   ip->RedrawViews(ip->GetTime());
   UpdateWindow(hWnd);
   }

void AttachKeyDlg::Command(int id, LPARAM lParam)
   {
   switch (id) {     
      case IDC_PREVKEY: {
		size_t n=0;
		for (int i=0; i<cont->keys.length(); i++) {
            if (cont->keys[i].flags&KEY_SELECTED) {
               if (i) n = i-1;
				else n = cont->keys.length()-1;
               }
            }
		for (int i=0; i<cont->keys.length(); i++) {
            if (i==n) cont->keys[i].flags |=  KEY_SELECTED;
            else      cont->keys[i].flags &= ~KEY_SELECTED;
            }
         cont->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
         break;
         }

      case IDC_NEXTKEY: {
         int n=0;
		for (int i=0; i<cont->keys.length(); i++) {
            if (cont->keys[i].flags&KEY_SELECTED) {
				if (i<cont->keys.length()-1) n = i+1;
               else n = 0;
               }
            }
		for (int i=0; i<cont->keys.length(); i++) {
            if (i==n) cont->keys[i].flags |=  KEY_SELECTED;
            else      cont->keys[i].flags &= ~KEY_SELECTED;
            }
         cont->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
         break;
         }

      case IDC_ATTACH_FACEPOSGRAPH:
         switch (LOWORD(lParam)) {
            case BARYGRAPH_DOWN:
               theHold.Begin();
               break;

            case BARYGRAPH_UP:
               if (HIWORD(lParam)) theHold.Accept(GetString(IDS_RB_EDITATTACHKEY));
               else theHold.Cancel();
               break;

            case BARYGRAPH_MOVE: {
               cont->HoldTrack();
               Point2 bary;
               SendDlgItemMessage(hWnd,IDC_ATTACH_FACEPOSGRAPH,
                  WM_GETBARY, 0, (LPARAM)&bary);
               
			for (int index=0; index<cont->keys.length(); index++) {
                  if (!(cont->keys[index].flags&KEY_SELECTED)) continue;
                  cont->keys[index].u0 = bary.x;
                  cont->keys[index].u1 = bary.y;
                  }
               cont->Invalidate();
               cont->NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);              
               ip->RedrawViews(ip->GetTime());
               UpdateWindow(hWnd);
               break;
               }
            }
         break;
      }
   }


void AttachKeyDlg::Invalidate()
   {
   valid = FALSE;
   InvalidateRect(hWnd,NULL,FALSE);
   }

void AttachKeyDlg::UpdateBaryGraph(float a, float b)
   {
   Point2 pt(a,b);
   SendDlgItemMessage(hWnd,IDC_ATTACH_FACEPOSGRAPH,WM_SETBARY,0,(LONG_PTR)&pt);
   UpdateWindow(GetDlgItem(hWnd,IDC_ATTACH_FACEPOSGRAPH));
   }

void AttachKeyDlg::UpdateTCBGraph(
      float t, float c, float b, float easeIn, float easeOut)
   {
   TCBGraphParams tp;
   tp.tens     = t;
   tp.cont     = c;
   tp.bias     = b;
   tp.easeTo   = easeIn;
   tp.easeFrom = easeOut;
   SendDlgItemMessage(hWnd,IDC_TCB_GRAPH,WM_SETTCBGRAPHPARAMS,0,(LPARAM)&tp);
   UpdateWindow(GetDlgItem(hWnd,IDC_TCB_GRAPH));
   }

void AttachKeyDlg::Update()
   {
   if (valid || !hWnd) return;
   valid = TRUE;
   BOOL init = FALSE;
   TimeValue time = 0;
   DWORD face = 0;
   float a=0.0f, b=0.0f, tens=0.0f, cnt=0.0f, bias=0.0f, easeTo=0.0f, easeFrom=0.0f;
   BOOL common[9] = {0,0,0,0,0,0,0,0,0};
   int selIndex = -1, selCount=0;

	for (int i=0; i<cont->keys.length(); i++) {
      if (!(cont->keys[i].flags&KEY_SELECTED)) continue;
      selCount++;

      if (!init) {
         init     = TRUE;
         time     = cont->keys[i].time;
         a        = cont->keys[i].u0;
         b        = cont->keys[i].u1;
         tens     = cont->keys[i].tens;
         cnt      = cont->keys[i].cont;
         bias     = cont->keys[i].bias;
         easeTo   = cont->keys[i].easeIn;
         easeFrom = cont->keys[i].easeOut;
         face     = cont->keys[i].face;
         selIndex = i;
         for (int j=0; j<9; j++) common[j] = TRUE;
      } else {
         if (time    !=cont->keys[i].time)     common[0] = FALSE;
         if (a       !=cont->keys[i].u0)       common[1] = FALSE;
         if (b       !=cont->keys[i].u1)       common[2] = FALSE;
         if (tens    !=cont->keys[i].tens)     common[3] = FALSE;
         if (cnt     !=cont->keys[i].cont)     common[4] = FALSE;
         if (bias    !=cont->keys[i].bias)     common[5] = FALSE;
         if (easeTo  !=cont->keys[i].easeIn)   common[6] = FALSE;
         if (easeFrom!=cont->keys[i].easeOut)  common[7] = FALSE;
         if (face    !=cont->keys[i].face)     common[8] = FALSE;
         selIndex = -1;
         }
      }

   UpdateBaryGraph(a,b);
   if (common[3]&&common[4]&&common[5]&&common[6]&&common[7]) {
      UpdateTCBGraph(tens,cnt,bias,easeTo,easeFrom);
      EnableWindow(GetDlgItem(hWnd,IDC_TCB_GRAPH),TRUE);
   } else {
      EnableWindow(GetDlgItem(hWnd,IDC_TCB_GRAPH),FALSE);
      }

   if (selIndex>=0) {
      TSTR buf;
      buf.printf(_T("%d"),selIndex+1);
      iStat->SetText(buf);
   } else {
      iStat->SetText(_T(""));
      }

   if (selCount==0) {
      iTime->Disable();
      iA->Disable();
      iB->Disable();
      iTens->Disable();
      iCont->Disable();
      iBias->Disable();
      iEaseTo->Disable();
      iEaseFrom->Disable();
      iFace->Disable();
      EnableWindow(GetDlgItem(hWnd,IDC_ATTACH_FACEPOSGRAPH),FALSE);
      return;
   } else {
      iTime->Enable();
      iA->Enable();
      iB->Enable();
      iTens->Enable();
      iCont->Enable();
      iBias->Enable();
      iEaseTo->Enable();
      iEaseFrom->Enable();
      iFace->Enable();
      EnableWindow(GetDlgItem(hWnd,IDC_ATTACH_FACEPOSGRAPH),TRUE);
      }

   if (common[0]) {
      iTime->SetIndeterminate(FALSE);
      iTime->SetValue(time,FALSE);
   } else {
      iTime->SetIndeterminate(TRUE);      
      }
   if (common[1]) {
      iA->SetIndeterminate(FALSE);
      iA->SetValue(a,FALSE);
   } else {
      iA->SetIndeterminate(TRUE);      
      }
   if (common[2]) {
      iB->SetIndeterminate(FALSE);
      iB->SetValue(b,FALSE);
   } else {
      iB->SetIndeterminate(TRUE);      
      }
   if (common[3]) {
      iTens->SetIndeterminate(FALSE);
      iTens->SetValue(ToTCBUI(tens),FALSE);
   } else {
      iTens->SetIndeterminate(TRUE);      
      }
   if (common[4]) {
      iCont->SetIndeterminate(FALSE);
      iCont->SetValue(ToTCBUI(cnt),FALSE);
   } else {
      iCont->SetIndeterminate(TRUE);      
      }
   if (common[5]) {
      iBias->SetIndeterminate(FALSE);
      iBias->SetValue(ToTCBUI(bias),FALSE);
   } else {
      iBias->SetIndeterminate(TRUE);      
      }
   if (common[6]) {
      iEaseTo->SetIndeterminate(FALSE);
      iEaseTo->SetValue(ToEaseUI(easeTo),FALSE);
   } else {
      iEaseTo->SetIndeterminate(TRUE);    
      }
   if (common[7]) {
      iEaseFrom->SetIndeterminate(FALSE);
      iEaseFrom->SetValue(ToEaseUI(easeFrom),FALSE);
   } else {
      iEaseFrom->SetIndeterminate(TRUE);     
      }
   if (common[8]) {
      iFace->SetIndeterminate(FALSE);
      iFace->SetValue((int)face + 1,FALSE);
   } else {
      iFace->SetIndeterminate(TRUE);      
      }  
   }

void AttachKeyDlg::SetupUI(HWND hWnd)
   {
   this->hWnd = hWnd;
   
   iTime = GetISpinner(GetDlgItem(hWnd,IDC_ATTACH_KEYTIMESPIN));
   iTime->SetLimits(GetAnimStart(),GetAnimEnd(),FALSE);
   iTime->SetScale(10.0f);
   iTime->LinkToEdit(GetDlgItem(hWnd,IDC_ATTACH_KEYTIME),EDITTYPE_TIME);

   iFace = GetISpinner(GetDlgItem(hWnd,IDC_ATTACH_KEYFACESPIN));  
   iFace->SetLimits(1,0xfffffff,FALSE);
   iFace->SetScale(0.1f);
   iFace->LinkToEdit(GetDlgItem(hWnd,IDC_ATTACH_KEYFACE),EDITTYPE_INT);
   
   iA = GetISpinner(GetDlgItem(hWnd,IDC_ATTACH_KEYASPIN));
   iA->SetLimits(-999999999.0f,999999999.0f,FALSE);
   iA->SetScale(0.005f);
   iA->LinkToEdit(GetDlgItem(hWnd,IDC_ATTACH_KEYA),EDITTYPE_FLOAT);
   
   iB = GetISpinner(GetDlgItem(hWnd,IDC_ATTACH_KEYBSPIN));
   iB->SetLimits(-999999999.0f,999999999.0f,FALSE);
   iB->SetScale(0.005f);
   iB->LinkToEdit(GetDlgItem(hWnd,IDC_ATTACH_KEYB),EDITTYPE_FLOAT);  

   iTens = GetISpinner(GetDlgItem(hWnd,IDC_ATTACH_KEYTENSSPIN));
   iTens->SetLimits(0.0f,50.0f,FALSE);
   iTens->SetScale(0.1f);
   iTens->LinkToEdit(GetDlgItem(hWnd,IDC_ATTACH_KEYTENS),EDITTYPE_FLOAT);
   
   iCont = GetISpinner(GetDlgItem(hWnd,IDC_ATTACH_KEYCONTSPIN));
   iCont->SetLimits(0.0f,50.0f,FALSE);
   iCont->SetScale(0.1f);
   iCont->LinkToEdit(GetDlgItem(hWnd,IDC_ATTACH_KEYCONT),EDITTYPE_FLOAT);
   
   iBias = GetISpinner(GetDlgItem(hWnd,IDC_ATTACH_KEYBIASSPIN));
   iBias->SetLimits(0.0f,50.0f,FALSE);
   iBias->SetScale(0.1f);
   iBias->LinkToEdit(GetDlgItem(hWnd,IDC_ATTACH_KEYBIAS),EDITTYPE_FLOAT);  

   iEaseTo = GetISpinner(GetDlgItem(hWnd,IDC_ATTACH_KEYEASETOSPIN));
   iEaseTo->SetLimits(0.0f,50.0f,FALSE);
   iEaseTo->SetScale(0.1f);
   iEaseTo->LinkToEdit(GetDlgItem(hWnd,IDC_ATTACH_KEYEASETO),EDITTYPE_FLOAT); 

   iEaseFrom = GetISpinner(GetDlgItem(hWnd,IDC_ATTACH_KEYEASEFROMSPIN));
   iEaseFrom->SetLimits(0.0f,50.0f,FALSE);
   iEaseFrom->SetScale(0.1f);
   iEaseFrom->LinkToEdit(GetDlgItem(hWnd,IDC_ATTACH_KEYEASEFROM),EDITTYPE_FLOAT);   
   
   iPrev = GetICustButton(GetDlgItem(hWnd,IDC_PREVKEY));
   iNext = GetICustButton(GetDlgItem(hWnd,IDC_NEXTKEY));

   iStat   = GetICustStatus(GetDlgItem(hWnd,IDC_KEYNUM));
   iStat->SetTextFormat(STATUSTEXT_CENTERED);
   }

RefResult AttachKeyDlg::NotifyRefChanged(
      Interval iv, RefTargetHandle hTarg,
      PartID& partID,RefMessage msg)
   {
   switch (msg) {
      case REFMSG_CHANGE:
         Invalidate();        
         break;
      
      case REFMSG_REF_DELETED:
         MaybeCloseWindow();
         break;
      }
   return REF_SUCCEED;
   }

static INT_PTR CALLBACK AttachDlgProc(
      HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
   {
   AttachKeyDlg *dlg = DLGetWindowLongPtr<AttachKeyDlg*>(hWnd);
   
   switch (msg) {
      case WM_INITDIALOG:
         dlg = (AttachKeyDlg*)lParam;
         DLSetWindowLongPtr(hWnd, lParam);
         dlg->SetupUI(hWnd);
         CenterWindow(hWnd,GetParent(hWnd));
         ShowWindow(hWnd,SW_SHOW);
         break;

      case WM_CLOSE:
         DestroyWindow(hWnd);       
         break;

      case WM_DESTROY:                 
         delete dlg;
         break;

      case WM_PAINT:
         dlg->Update();
         return FALSE;

      case CC_SPINNER_BUTTONDOWN:         
         theHold.Begin();     
         break;

      case WM_CUSTEDIT_ENTER:
      case CC_SPINNER_BUTTONUP:
         if (HIWORD(wParam)) {
            theHold.Accept(GetString(IDS_RB_EDITATTACHKEY));
         } else {
            theHold.Cancel();
            }
         break;

      case CC_SPINNER_CHANGE:
         dlg->SpinnerChange(LOWORD(wParam));
         break;

      case WM_COMMAND:
         dlg->Command(LOWORD(wParam),lParam);
         break;

      default:
         return FALSE;
      }
   return TRUE;
   }
