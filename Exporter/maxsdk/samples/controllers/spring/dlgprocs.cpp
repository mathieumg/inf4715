/**********************************************************************
 *<
   FILE:       DlgProcs.cpp

   DESCRIPTION:   Dialog Handlers and UI functions for the Spring Controller

   CREATED BY:    Adam Felt

   HISTORY: 

 *>   Copyright (c) 1999-2000, All Rights Reserved.
 **********************************************************************/

#include "jiggle.h"

#include "3dsmaxport.h"

//Function to invoke the UI of the modeless trackview dialog
//***********************************************************************
void Jiggle::EditTrackParams(TimeValue /*t*/,ParamDimensionBase* /*dim*/,
            const TCHAR* /*pname*/,HWND hParent,IObjParam *ip,DWORD /*flags*/)
{
   //many, many bugs might occur if you end up with two Spring dialogs open!
   //changed it so it actives the existing open window.
   if (!dlg || !dlg->hWnd) {
      dlg = new JiggleTrackDlg(ip,this);
      dlg->Init(hParent);
      RegisterJiggleWindow(dlg->hWnd,hParent,this);
   } 
   else {
      SetActiveWindow(dlg->hWnd);
   }
   this->ip = ip;
}


//Function to update the node lists in both the motion panel and the properties dialog
//**************************************************************************************
void Jiggle::UpdateNodeList(bool updateSpinners)
{
   if (!dlg && !hParams1) return;

   int nSelItemsInDlg = 0, nSelItemsInPanel = 0;
   int nDlgBuffer[100], nPanelBuffer[100];

   int i, ct; //, sel1, sel2;

   ct = dyn_pb->Count(jig_control_node);
   if (ct > partsys->GetParticle(0)->GetSprings()->length()) return;

   if (dlg)
   {
      nSelItemsInDlg = SendMessage(GetDlgItem(dlg->dynDlg, IDC_LIST2), LB_GETSELITEMS, 100, (LPARAM) nDlgBuffer); 
      SendDlgItemMessage(dlg->dynDlg, IDC_LIST2, LB_RESETCONTENT, 0, 0);
      if (nSelItemsInDlg==0 && ct>0) nDlgBuffer[0] = 0; //always select the first one
   } 
   
   if (hParams1)
   {
      nSelItemsInPanel = SendMessage(GetDlgItem(hParams1, IDC_LIST2), LB_GETSELITEMS, 100, (LPARAM) nPanelBuffer); 
      SendDlgItemMessage(hParams1, IDC_LIST2, LB_RESETCONTENT, 0, 0);
      if (nSelItemsInPanel==0 && ct>0) nPanelBuffer[0] = 0; //always select the first one
   }
   //add the self influence spring
   TSTR buf;
   TCHAR name[256] = {0};
   float tension = partsys->GetParticle(0)->GetSprings()->length();
   float dampening = JIGGLE_DEFAULT_DAMPENING;
   if (tension)
   {
      tension = partsys->GetParticle(0)->GetSpring(0)->GetTension();
      dampening = partsys->GetParticle(0)->GetSpring(0)->GetDampening();
   }
   buf.printf(_T("%-16.18s %6.1f %s %.1f"), _T("Self Influence"), tension, _T("/"), dampening);
   _tcscpy(name, buf);
   if (dlg) SendDlgItemMessage(dlg->dynDlg, IDC_LIST2, LB_ADDSTRING, 0, (LPARAM)name);
   if (hParams1) SendDlgItemMessage(hParams1, IDC_LIST2, LB_ADDSTRING, 0, (LPARAM)name);

   //add the bones
   for(i = 1; i < ct; i++)
   {
      INode* node;
      tension = partsys->GetParticle(0)->GetSpring(i)->GetTension();
      dampening = partsys->GetParticle(0)->GetSpring(i)->GetDampening();
	  Interval for_ever = FOREVER;
      dyn_pb->GetValue(jig_control_node, 0, node, for_ever, i);
      
      if (node != NULL){
         buf.printf(_T("%-16.18s %6.1f %s %.1f"), node->GetName(), tension, _T("/"), dampening);
      }else buf.printf(_T("%s"), _T("<deleted>"));
      _tcscpy(name, buf);
      if (dlg) SendDlgItemMessage(dlg->dynDlg, IDC_LIST2, LB_ADDSTRING, 0, (LPARAM)name);
      if (hParams1) SendDlgItemMessage(hParams1, IDC_LIST2, LB_ADDSTRING, 0, (LPARAM)name);
   }
   if (dlg){
      for (i = nSelItemsInDlg - 1; i >= 0; i--) 
      { 
         if (nDlgBuffer[i] <= ct) SendMessage(GetDlgItem(dlg->dynDlg, IDC_LIST2), LB_SETSEL, TRUE, nDlgBuffer[i]);
      }
      dlg->valid = FALSE;
      if (!theHold.Holding() && updateSpinners) dlg->Update();
   }

   if (hParams1)
   {
      for (i = nSelItemsInPanel - 1; i >= 0; i--) 
      { 
         if (nPanelBuffer[i] <= ct) SendMessage(GetDlgItem(hParams1, IDC_LIST2), LB_SETSEL, TRUE, nPanelBuffer[i]);
      }
      if (updateSpinners && pmap) pmap->Update(GetCOREInterface()->GetTime()); 
   }
}


//ParamMap2 dialog proc for the dynamics rollout in the Motion Panel
//********************************************************************
INT_PTR DynMapDlgProc::DlgProc(
      TimeValue ,IParamMap2* map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM )
{
    int nSelItemsInBuffer; 
    int nBuffer[100]; 
   int i;
   int spinStyle;
   TSTR undoStr = GetString(IDS_UNDO_SPINNER_CHANGE);

   switch (msg) 
   {
      case WM_INITDIALOG: 
         cont->hParams1 = hWnd;
         cont->pmap = this;
         paramMap = map;
         InitParams();
         break;

      case CC_SPINNER_BUTTONDOWN:
         if (!theHold.Holding())
            theHold.Begin();
         break;
      case CC_SPINNER_CHANGE:
		  {
         nSelItemsInBuffer = SendMessage(GetDlgItem(hWnd, IDC_LIST2), LB_GETSELITEMS, 100, (LPARAM) nBuffer);
		 Interval for_ever = FOREVER;
         cont->dyn_pb->GetValue(jig_how, 0, spinStyle, for_ever);

         switch (LOWORD(wParam)) 
         {
            case IDC_JIGGLE_TENSION_SPIN:
               if (spinStyle == SET_PARAMS_RELATIVE ) theHold.Restore();
               cont->HoldAll();
               for (i = nSelItemsInBuffer - 1; i >= 0; i--) 
               { 
                  cont->SetTension(nBuffer[i], (float)iTension->GetFVal(), spinStyle, false);
               }
               break;
            case IDC_JIGGLE_DAMP_SPIN:
               if (spinStyle == SET_PARAMS_RELATIVE ) theHold.Restore();
               cont->HoldAll();
               for (i = nSelItemsInBuffer - 1; i >= 0; i--) 
               { 
                  cont->SetDampening(nBuffer[i], (float)iDampening->GetFVal(), spinStyle, false);
               }
               break;
            case IDC_JIGGLE_MASS_SPIN:
               cont->HoldAll();
               cont->SetMass((float)iMass->GetFVal(), false);
               break;
            case IDC_JIGGLE_DRAG_SPIN:
               cont->HoldAll();
               cont->SetDrag((float)iDrag->GetFVal(), false);
            default: break;
         }
         cont->UpdateNodeList(false);
         cont->NotifyDependents(FOREVER, (PartID)PART_ALL, REFMSG_CHANGE);
         cont->ip->RedrawViews(cont->ip->GetTime());

         break;
	}
      case WM_CUSTEDIT_ENTER:
         //break;

      case CC_SPINNER_BUTTONUP:
         switch (LOWORD(wParam))
         {
            case IDC_JIGGLE_TENSION_SPIN:
               undoStr = GetString(IDS_UNDO_TENSION);
               break;
            case IDC_JIGGLE_DAMP_SPIN:
               undoStr = GetString(IDS_UNDO_DAMPENING);
               break;
            case IDC_JIGGLE_MASS_SPIN:
               undoStr = GetString(IDS_UNDO_MASS);
               break;
            case IDC_JIGGLE_DRAG_SPIN:
               undoStr = GetString(IDS_UNDO_DRAG);
               break;
            default: break;
         }
         if (theHold.Holding()) theHold.Accept(undoStr);
         cont->UpdateNodeList();
         cont->NotifyDependents(FOREVER, (PartID)PART_ALL, REFMSG_CHANGE);
         cont->ip->RedrawViews(cont->ip->GetTime());

         break;

      case WM_COMMAND:
         switch (LOWORD(wParam)) {
            case IDC_PICK_SPRING:
               cont->ip->SetPickMode(cont->pickNodeMode);
               break;

            case IDC_REMOVE_SPRING: 
               theHold.Begin();
               nSelItemsInBuffer = SendMessage(GetDlgItem(hWnd, IDC_LIST2), LB_GETSELITEMS, 100, (LPARAM) nBuffer); 
               for (i = nSelItemsInBuffer - 1; i >= 0; i--) 
               { 
                  cont->RemoveSpring(nBuffer[i]);
               }
               theHold.Accept(GetString(IDS_UNDO_REMOVE_SPRING));
               break;
            case IDC_LIST2:
               Update(cont->ip->GetTime());
               break;
         }
         break;
      case WM_DESTROY:
         DestroyParams();
         cont->pmap = NULL;
         return FALSE;

   }
   return FALSE;
}

//ParamMap2 dialog proc for the forces rollout in the Motion Panel
//********************************************************************
INT_PTR ForceMapDlgProc::DlgProc(
      TimeValue ,IParamMap2* ,HWND hWnd,UINT msg,WPARAM wParam,LPARAM )
   {
   switch (msg) {
      case WM_INITDIALOG: {
         cont->hParams2 = hWnd;
         break;
         }

      case CC_SPINNER_BUTTONDOWN:
         break;
      case CC_SPINNER_CHANGE:
         break;

      case WM_CUSTEDIT_ENTER:
      case CC_SPINNER_BUTTONUP:
         break;

      case WM_COMMAND:
         switch (LOWORD(wParam)) 
         {
         case IDC_FORCE_PICKNODE:
         case IDC_REMOVE_FORCE:
            
            break;
         }
         break;


      }
   return FALSE;
   }

//DynMapDlgProc : UI functions for Dynamics rollout in the Motion Panel
//********************************************************************
void DynMapDlgProc::InitParams()
{
   if (!cont->dlg) cont->pickNodeMode = new PickNodeMode(this);
      else cont->pickNodeMode->map = this;

   iAddBone = GetICustButton(GetDlgItem(cont->hParams1,IDC_PICK_SPRING));
   iAddBone->SetType(CBT_CHECK);
   iAddBone->SetHighlightColor(GREEN_WASH);
   iAddBone->SetTooltip(TRUE, GetString(IDS_ADD_SPRING));

   iDeleteBone = GetICustButton(GetDlgItem(cont->hParams1,IDC_REMOVE_SPRING));
   iDeleteBone->SetType(CBT_PUSH);
   iDeleteBone->SetTooltip(TRUE, GetString(IDS_DELETE_SPRING));
   iDeleteBone->Disable();

   iTension = GetISpinner(GetDlgItem(cont->hParams1,IDC_JIGGLE_TENSION_SPIN));
   iTension->SetLimits(-100.0f,100.0f,FALSE);
   iTension->SetScale(0.01f);
   iTension->LinkToEdit(GetDlgItem(cont->hParams1,IDC_JIGGLE_TENSION),EDITTYPE_FLOAT);
   iTension->SetValue(JIGGLE_DEFAULT_TENSION, 0);
   
   iDampening = GetISpinner(GetDlgItem(cont->hParams1,IDC_JIGGLE_DAMP_SPIN));
   iDampening->SetLimits(-10.0f,10.0f,FALSE);
   iDampening->SetAutoScale();
   iDampening->LinkToEdit(GetDlgItem(cont->hParams1,IDC_JIGGLE_DAMP),EDITTYPE_FLOAT);
   iDampening->SetValue(JIGGLE_DEFAULT_DAMPENING, 0);
   
   iMass = GetISpinner(GetDlgItem(cont->hParams1,IDC_JIGGLE_MASS_SPIN));
   iMass->SetLimits(-10000.0f,10000.0f,FALSE);
   iMass->SetScale(1.0f);
   iMass->LinkToEdit(GetDlgItem(cont->hParams1,IDC_JIGGLE_MASS),EDITTYPE_FLOAT);
   iMass->SetValue(JIGGLE_DEFAULT_MASS, 0);

   iDrag = GetISpinner(GetDlgItem(cont->hParams1,IDC_JIGGLE_DRAG_SPIN));
   iDrag->SetLimits(0.0f,10.0f,FALSE);
   iDrag->SetScale(0.01f);
   iDrag->LinkToEdit(GetDlgItem(cont->hParams1,IDC_JIGGLE_DRAG),EDITTYPE_FLOAT);

   float mass, drag;
   mass = cont->partsys->GetParticle(0)->GetMass();
   drag = cont->partsys->GetParticle(0)->GetDrag();
   
   iMass->SetValue(mass, FALSE);
   iDrag->SetValue(drag, FALSE);
   
   cont->UpdateNodeList();
}

void DynMapDlgProc::DestroyParams()
{
   cont->ip->ClearPickMode();
   if (!cont->dlg)
   {
      delete cont->pickNodeMode; 
      cont->pickNodeMode = NULL;
   } else cont->pickNodeMode->map = NULL;

   ReleaseISpinner(iTension);    iTension = NULL;
   ReleaseISpinner(iDampening);  iDampening = NULL;
   ReleaseISpinner(iMass);       iMass = NULL;
   ReleaseISpinner(iDrag);       iDrag = NULL;
   ReleaseICustButton(iAddBone); iAddBone = NULL;
   ReleaseICustButton(iDeleteBone); iDeleteBone = NULL;
}

//void DynMapDlgProc::UpdateParams()
void DynMapDlgProc::Update(TimeValue )
{
   int radioSel = 0;
   Interval for_ever = FOREVER;
   cont->dyn_pb->GetValue(jig_how, 0, radioSel, for_ever);
   
   int nSelItemsInBuffer; 
   int nBuffer[100]; 

   nSelItemsInBuffer = SendMessage(GetDlgItem(cont->hParams1, IDC_LIST2), LB_GETSELITEMS, 100, (LPARAM) nBuffer); 

   if (nSelItemsInBuffer == 0)
   {
      iDeleteBone->Disable();
      return;
   }

   if (nSelItemsInBuffer == 1 && nBuffer[0] == 0)
   {
      iDeleteBone->Disable();
   }else iDeleteBone->Enable();

   if (radioSel == SET_PARAMS_RELATIVE)
   {
      iDampening->SetIndeterminate(FALSE); iTension->SetValue(0.0f, 0);
      iTension->SetIndeterminate(FALSE); iDampening->SetValue(0.0f, 0);
   } else {
      float tension = 0.0f;
      float dampening = 0.0f;
      BOOL tensionsDiffer = FALSE;
      BOOL dampsDiffer = FALSE;

      for (int i = nSelItemsInBuffer - 1; i >= 0; i--) 
      { 
         if (i == nSelItemsInBuffer - 1)
         {
            tension = cont->partsys->GetParticle(0)->GetSpring(nBuffer[i])->GetTension();
            dampening = cont->partsys->GetParticle(0)->GetSpring(nBuffer[i])->GetDampening();
         }
         if ( tensionsDiffer == FALSE && tension != cont->partsys->GetParticle(0)->GetSpring(nBuffer[i])->GetTension())
            tensionsDiffer = TRUE;
         if ( dampsDiffer == FALSE && dampening != cont->partsys->GetParticle(0)->GetSpring(nBuffer[i])->GetDampening())
            dampsDiffer = TRUE;
      }
      if (tensionsDiffer) iTension->SetIndeterminate();
      else {
         iTension->SetIndeterminate(FALSE);
         iTension->SetValue(tension, 0);
      }
      if (dampsDiffer) iDampening->SetIndeterminate();
      else {
         iDampening->SetIndeterminate(FALSE);
         iDampening->SetValue(dampening, 0);
      }
   }
   iMass->SetValue(cont->partsys->GetParticle(0)->GetMass(), 0);
   iDrag->SetValue(cont->partsys->GetParticle(0)->GetDrag(), 0);

   //this fixes the problem with the radio buttons being out of sync, but causes flashing and slowness
   //paramMap->Invalidate();
}


//The Window handler for the Dynamics rollout in the modeless propertis dialog
//******************************************************************************
INT_PTR CALLBACK DynamicsRollupDialogProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	JiggleTrackDlg *dlg = DLGetWindowLongPtr<JiggleTrackDlg*>(hDlg);
	if ( !dlg && message != WM_INITDIALOG ) return FALSE;
	// TimeValue t = 0;
	//int sel;
	//int state;
	int nSelItemsInBuffer; 
	int nBuffer[100]; 
	int i;
	int spinStyle;
	TSTR undoStr = GetString(IDS_UNDO_SPINNER_CHANGE);

	switch ( message ) 
	{
	case WM_INITDIALOG:
		DLSetWindowLongPtr(hDlg, lParam);
		dlg = (JiggleTrackDlg*)lParam;
		dlg->dynDlg = hDlg;
		dlg->InitDynamicsParams();
		return TRUE;

	case WM_DESTROY:
		//dlg->DestroyDynamicsParams();
		return FALSE;

	case WM_MOUSEACTIVATE:
		return FALSE;

	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MOUSEMOVE:            
		//          cont->ip->RollupMouseMessage(hDlg,message,wParam,lParam);
		return FALSE;


	case CC_SPINNER_BUTTONDOWN:
		if (!theHold.Holding())
			theHold.Begin();
		break;

	case CC_SPINNER_CHANGE:
		{
			nSelItemsInBuffer = SendMessage(GetDlgItem(dlg->dynDlg, IDC_LIST2), LB_GETSELITEMS, 100, (LPARAM) nBuffer); 
			Interval for_ever = FOREVER;
			dlg->cont->dyn_pb->GetValue(jig_how, 0, spinStyle, for_ever);
			dlg->blockRedraw = TRUE;

			switch (LOWORD(wParam)) 
			{
			case IDC_JIGGLE_TENSION_SPIN:
				if (spinStyle == SET_PARAMS_RELATIVE ) theHold.Restore();
				dlg->cont->HoldAll();
				for (i = nSelItemsInBuffer - 1; i >= 0; i--) 
				{ 
					dlg->cont->SetTension(nBuffer[i], (float)dlg->iTension->GetFVal(), spinStyle, false);
				}
				break;
			case IDC_JIGGLE_DAMP_SPIN:
				if (spinStyle == SET_PARAMS_RELATIVE ) theHold.Restore();
				dlg->cont->HoldAll();
				for (i = nSelItemsInBuffer - 1; i >= 0; i--) 
				{ 
					dlg->cont->SetDampening(nBuffer[i], (float)dlg->iDampening->GetFVal(), spinStyle, false);
				}
				break;
			case IDC_JIGGLE_MASS_SPIN:
				dlg->cont->HoldAll();
				dlg->cont->SetMass((float)dlg->iMass->GetFVal(), false);
				break;
			case IDC_JIGGLE_DRAG_SPIN:
				dlg->cont->HoldAll();
				dlg->cont->SetDrag((float)dlg->iDrag->GetFVal(), false);
			default: break;
			}
			dlg->cont->UpdateNodeList();
			dlg->cont->NotifyDependents(FOREVER,(PartID)PART_ALL,REFMSG_CHANGE);
			dlg->blockRedraw = FALSE;
			dlg->ip->RedrawViews(dlg->ip->GetTime());
			break;
		}
	case WM_CUSTEDIT_ENTER:
		//break;

	case CC_SPINNER_BUTTONUP:
		switch (LOWORD(wParam))
		{
		case IDC_JIGGLE_TENSION_SPIN:
			undoStr = GetString(IDS_UNDO_TENSION);
			break;
		case IDC_JIGGLE_DAMP_SPIN:
			undoStr = GetString(IDS_UNDO_DAMPENING);
			break;
		case IDC_JIGGLE_MASS_SPIN:
			undoStr = GetString(IDS_UNDO_MASS);
			break;
		case IDC_JIGGLE_DRAG_SPIN:
			undoStr = GetString(IDS_UNDO_DRAG);
			break;
		default: break;
		}
		theHold.Accept(undoStr);
		dlg->cont->UpdateNodeList();
		dlg->cont->NotifyDependents(FOREVER, (PartID)PART_ALL, REFMSG_CHANGE);
		dlg->ip->RedrawViews(dlg->ip->GetTime());
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_PICK_SPRING:
			dlg->ip->SetPickMode(dlg->cont->pickNodeMode);
			break;

		case IDC_REMOVE_SPRING: 
			theHold.Begin();
			nSelItemsInBuffer = SendMessage(GetDlgItem(dlg->dynDlg, IDC_LIST2), LB_GETSELITEMS, 100, (LPARAM) nBuffer); 
			for (i = nSelItemsInBuffer - 1; i >= 0; i--) 
			{ 
				dlg->cont->RemoveSpring(nBuffer[i]);
			}
			theHold.Accept(GetString(IDS_UNDO_REMOVE_SPRING));
			break;
		case IDC_LIST2:
			dlg->Invalidate();
			break;
		case IDC_RELATIVE_RB:
			dlg->cont->dyn_pb->SetValue(jig_how, 0, SET_PARAMS_RELATIVE);
			break;
		case IDC_ABSOLUTE_RB:
			dlg->cont->dyn_pb->SetValue(jig_how, 0, SET_PARAMS_ABSOLUTE);
			break;
		}
		break;
	case WM_PAINT:
		if (!dlg->valid) dlg->Update();
		return FALSE;

	}
	return FALSE;

}

//The Window handler for the Forces rollout in the modeless properties dialog
//******************************************************************************
INT_PTR CALLBACK ForcesRollupDialogProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	// Jiggle *cont = DLGetWindowLongPtr<Jiggle *>( hDlg);
	JiggleTrackDlg *dlg = DLGetWindowLongPtr<JiggleTrackDlg*>(hDlg);
	if ( !dlg && message != WM_INITDIALOG ) return FALSE;
	TimeValue t = 0;
	int sel;
	IParamMap2* pmap;

	switch ( message ) 
	{
	case WM_INITDIALOG:
		DLSetWindowLongPtr(hDlg, lParam);
		dlg = (JiggleTrackDlg*)lParam;
		dlg->forceDlg = hDlg;

		dlg->pickForceMode = new PickForceMode(dlg);
		dlg->iPickForce = GetICustButton(GetDlgItem(hDlg,IDC_FORCE_PICKNODE));
		dlg->iPickForce->SetType(CBT_CHECK);
		dlg->iPickForce->SetHighlightColor(GREEN_WASH);

		dlg->xSpin        = GetISpinner(GetDlgItem(hDlg,IDC_JIGGLE_X_SPIN));
		dlg->ySpin        = GetISpinner(GetDlgItem(hDlg,IDC_JIGGLE_Y_SPIN));
		dlg->zSpin        = GetISpinner(GetDlgItem(hDlg,IDC_JIGGLE_Z_SPIN));
		dlg->startSpin    = GetISpinner(GetDlgItem(hDlg,IDC_JIGGLE_START_SPIN));
		dlg->stepsSpin    = GetISpinner(GetDlgItem(hDlg,IDC_JIGGLE_STEPS_SPIN));

		dlg->xSpin->SetLimits( (float)0.0, (float)1000.0, FALSE );
		dlg->ySpin->SetLimits( (float)0.0, (float)1000.0, FALSE );
		dlg->zSpin->SetLimits( (float)0.0, (float)1000.0, FALSE );
		dlg->startSpin->SetLimits( (int)-99999, (int)99999, FALSE );
		dlg->stepsSpin->SetLimits( (int)0, (int)4, FALSE );

		dlg->xSpin->SetScale(1.0f);
		dlg->ySpin->SetScale(1.0f);
		dlg->zSpin->SetScale(1.0f);
		dlg->startSpin->SetScale(1.0f);
		dlg->stepsSpin->SetScale(1.0f);

		dlg->xSpin->LinkToEdit( GetDlgItem(hDlg,IDC_JIGGLE_X), EDITTYPE_FLOAT );
		dlg->ySpin->LinkToEdit( GetDlgItem(hDlg,IDC_JIGGLE_Y), EDITTYPE_FLOAT );
		dlg->zSpin->LinkToEdit( GetDlgItem(hDlg,IDC_JIGGLE_Z), EDITTYPE_FLOAT );
		dlg->startSpin->LinkToEdit( GetDlgItem(hDlg,IDC_JIGGLE_START), EDITTYPE_INT );
		dlg->stepsSpin->LinkToEdit( GetDlgItem(hDlg,IDC_JIGGLE_STEPS), EDITTYPE_INT );

		int steps, start;
		float x, y, z;
		t = GetCOREInterface()->GetTime();
#pragma warning(push)
#pragma warning(disable:4239)
		dlg->cont->force_pb->GetValue(jig_tolerence,t,steps,FOREVER);
		dlg->cont->force_pb->GetValue(jig_start,t,start,FOREVER);
		dlg->cont->force_pb->GetValue(jig_xeffect,t,x,FOREVER);
		dlg->cont->force_pb->GetValue(jig_yeffect,t,y,FOREVER);
		dlg->cont->force_pb->GetValue(jig_zeffect,t,z,FOREVER);
#pragma warning(pop)
		dlg->xSpin->SetValue(x, FALSE);
		dlg->ySpin->SetValue(y, FALSE);
		dlg->zSpin->SetValue(z, FALSE);
		dlg->startSpin->SetValue(start, FALSE);
		dlg->stepsSpin->SetValue(steps, FALSE);

		dlg->UpdateForceList();

		return TRUE;

	case WM_DESTROY:
		return FALSE;

	case WM_MOUSEACTIVATE:
		return FALSE;

	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MOUSEMOVE:            
		//          cont->ip->RollupMouseMessage(hDlg,message,wParam,lParam);
		return FALSE;


	case CC_SPINNER_BUTTONDOWN:
		break;

	case CC_SPINNER_CHANGE:
		t = GetCOREInterface()->GetTime();
		switch (LOWORD(wParam)) 
		{
		case IDC_JIGGLE_X_SPIN:
			dlg->cont->force_pb->SetValue(jig_xeffect, t, dlg->xSpin->GetFVal());
			pmap = dlg->cont->force_pb->GetMap();
			if (pmap) pmap->Invalidate(jig_xeffect);
			break;
		case IDC_JIGGLE_Y_SPIN:
			dlg->cont->force_pb->SetValue(jig_yeffect, t, dlg->ySpin->GetFVal());
			pmap = dlg->cont->force_pb->GetMap();
			if (pmap) pmap->Invalidate(jig_yeffect);
			break;
		case IDC_JIGGLE_Z_SPIN:
			dlg->cont->force_pb->SetValue(jig_zeffect, t, dlg->zSpin->GetFVal());
			pmap = dlg->cont->force_pb->GetMap();
			if (pmap) pmap->Invalidate(jig_zeffect);
			break;
		case IDC_JIGGLE_START_SPIN:
			dlg->cont->force_pb->SetValue(jig_start, t, dlg->startSpin->GetIVal());
			break;
		case IDC_JIGGLE_STEPS_SPIN:
			dlg->cont->force_pb->SetValue(jig_tolerence, t, dlg->stepsSpin->GetIVal());
			break;
		}
		dlg->cont->NotifyDependents(FOREVER,(PartID)PART_ALL,REFMSG_CHANGE);
		dlg->ip->RedrawViews(dlg->ip->GetTime());
		break;


	case WM_COMMAND:
		switch (LOWORD(wParam)) {
	case IDC_FORCE_PICKNODE:
		dlg->ip->SetPickMode(dlg->pickForceMode);
		break;

	case IDC_REMOVE_FORCE: 
		sel = SendDlgItemMessage(hDlg,IDC_LIST1,LB_GETCURSEL,0,0);
		if (sel!=LB_ERR) {
			dlg->cont->force_pb->Delete(jig_force_node, sel, 1);
			dlg->UpdateForceList();
			pmap = dlg->cont->force_pb->GetMap();
			if (pmap) 
				pmap->Invalidate(jig_force_node);
			dlg->ip->RedrawViews(dlg->ip->GetTime());
		}
		break;
		}
		break;

	case WM_PAINT:
		if (!dlg->valid)
			dlg->Update();
		return FALSE;

	}
	return FALSE;

}

//Dialog handler for the About rollout
//*********************************************
INT_PTR CALLBACK AboutRollupDialogProc( HWND , UINT message, WPARAM , LPARAM  )
{
   switch ( message ) 
   {
      case WM_INITDIALOG:
         break;
      break;
   }
   return false;
}

//Dialog Handler for the modeless properties dialog
//********************************************************************
static INT_PTR CALLBACK JiggleDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
   JiggleDlg *dlg = DLGetWindowLongPtr<JiggleDlg*>(hWnd);
   HWND rollout;

   switch (message) {
      case WM_INITDIALOG: {         
         DLSetWindowLongPtr(hWnd, lParam);
		 SendMessage(hWnd, WM_SETICON, ICON_SMALL, 
			 DLGetClassLongPtr<LPARAM>(GetCOREInterface()->GetMAXHWnd(), GCLP_HICONSM));
         dlg = (JiggleDlg*)lParam;
         
         if (!dlg) return FALSE;
         dlg->TVRollUp = GetIRollup(GetDlgItem(hWnd,IDC_JIGGLE_ROLLOUT));

         static TCHAR buf[80];
         const size_t bufLen = _countof(buf);

         LoadString(hInstance, IDS_DYN_PARAMS, buf, bufLen);
         dlg->TVRollUp->AppendRollup(hInstance, 
            MAKEINTRESOURCE(IDD_DYNAMICS_PANEL), 
            DynamicsRollupDialogProc, 
            buf, (LPARAM)dlg);

         LoadString(hInstance, IDS_FORCE_PARAMS, buf, bufLen);
         dlg->TVRollUp->AppendRollup(hInstance, 
            MAKEINTRESOURCE(IDD_FORCES_PANEL), 
            ForcesRollupDialogProc, 
            buf, (LPARAM)dlg);
         /*
         LoadString(hInstance, IDS_ABOUT, buf, bufLen);
         dlg->TVRollUp->AppendRollup(hInstance, 
            MAKEINTRESOURCE(IDD_ABOUT_PANEL), 
            AboutRollupDialogProc, 
            buf, (LPARAM)dlg);
         */
         dlg->TVRollUp->Show(0);
         dlg->TVRollUp->Show(1); dlg->TVRollUp->SetPanelOpen(1, false);
         //dlg->TVRollUp->Show(2); dlg->TVRollUp->SetPanelOpen(2, false);

         break;
         }
      case WM_COMMAND:
//       switch (LOWORD(wParam)) {
            break;
//       }

      case WM_PAINT:
         if (!dlg->valid) dlg->Update();
         return 0;
         
      case WM_SIZING:
         switch (wParam)
         {
            case WMSZ_BOTTOMLEFT:
            case WMSZ_LEFT:
            case WMSZ_TOPLEFT:
               ((LPRECT)lParam)->left = ((LPRECT)lParam)->right - 190; break;
            case WMSZ_BOTTOMRIGHT:
            case WMSZ_RIGHT:
            case WMSZ_TOPRIGHT:
               ((LPRECT)lParam)->right = ((LPRECT)lParam)->left + 190; break;
         }
         rollout = GetDlgItem(hWnd, IDC_JIGGLE_ROLLOUT);
         SetWindowPos ( rollout, HWND_TOP, 5, 7,
                     (((LPRECT)lParam)->right  - ((LPRECT)lParam)->left) - 16,
                     (((LPRECT)lParam)->bottom - ((LPRECT)lParam)->top) - 40,
                     SWP_SHOWWINDOW);

         break;

      case WM_CLOSE:
         DestroyWindow(hWnd);       
         break;

      case WM_DESTROY:     
         dlg->ip->ClearPickMode();
         dlg->DestroyDynamicsParams();
         ReleaseISpinner( dlg->xSpin ); dlg->xSpin = NULL;
         ReleaseISpinner( dlg->ySpin ); dlg->ySpin = NULL;
         ReleaseISpinner( dlg->zSpin ); dlg->zSpin = NULL;
         ReleaseISpinner( dlg->startSpin ); dlg->startSpin = NULL;
         ReleaseISpinner( dlg->stepsSpin ); dlg->stepsSpin = NULL;
         ReleaseICustButton( dlg->iPickForce ); 
         delete dlg->pickForceMode;

         ReleaseIRollup(dlg->TVRollUp);   dlg->TVRollUp = NULL;
         delete dlg;
         break;

      case WM_LBUTTONDOWN:
      case WM_LBUTTONUP:
      case WM_MOUSEMOVE:         
         dlg->MouseMessage(message,wParam,lParam);
         return FALSE;

      default:
         return 0;
      }
   return 1;
}

//TimeChangeCallback method:
void JiggleDlgTimeChangeCallback::TimeChanged(TimeValue )
{
	dlg->UpdateForceSpinners();
}

// Functions for the modeless trackview dialog
//***********************************************************
JiggleDlg::JiggleDlg(IObjParam *i,Jiggle *c)
   {
   cont = NULL;
   theHold.Suspend();
   ReplaceReference(0,c);
   theHold.Resume();
   ip = i;
   valid = FALSE;
   blockRedraw = FALSE;
   hWnd  = NULL;
   dynDlg = forceDlg = NULL;
   cont->dlg = this;
   pickForceMode = NULL;
   timeChangeCallback = new JiggleDlgTimeChangeCallback(this);
   ip->RegisterTimeChangeCallback(timeChangeCallback);
   }

JiggleDlg::~JiggleDlg()
   {
   cont->dlg = NULL;
   ip->UnRegisterTimeChangeCallback(timeChangeCallback);
   delete timeChangeCallback;
   UnRegisterJiggleWindow(hWnd);

   theHold.Suspend();
   DeleteAllRefsFromMe();
   theHold.Resume();
   }
      

void JiggleDlg::Init(HWND hParent)
{
   hWnd = CreateWin(hParent);
   //Update();
}

void JiggleDlg::InitDynamicsParams()
   {
   if (!cont->hParams1) cont->pickNodeMode = new PickNodeMode(cont);

   iAddBone = GetICustButton(GetDlgItem(dynDlg,IDC_PICK_SPRING));
   iAddBone->SetType(CBT_CHECK);
   iAddBone->SetHighlightColor(GREEN_WASH);
   iAddBone->SetTooltip(TRUE, GetString(IDS_ADD_SPRING));
   //iAddBone->Disable();

   iDeleteBone = GetICustButton(GetDlgItem(dynDlg,IDC_REMOVE_SPRING));
   iDeleteBone->SetType(CBT_PUSH);
   iDeleteBone->SetTooltip(TRUE, GetString(IDS_DELETE_SPRING));
   iDeleteBone->Disable();

   iTension = GetISpinner(GetDlgItem(dynDlg,IDC_JIGGLE_TENSION_SPIN));
   iTension->SetLimits(-100.0f,100.0f,FALSE);
   iTension->SetScale(0.01f);
   iTension->LinkToEdit(GetDlgItem(dynDlg,IDC_JIGGLE_TENSION),EDITTYPE_FLOAT);
   iTension->SetValue(JIGGLE_DEFAULT_TENSION, 0);
   
   iDampening = GetISpinner(GetDlgItem(dynDlg,IDC_JIGGLE_DAMP_SPIN));
   iDampening->SetLimits(-10.0f,10.0f,FALSE);
   iDampening->SetAutoScale();
   iDampening->LinkToEdit(GetDlgItem(dynDlg,IDC_JIGGLE_DAMP),EDITTYPE_FLOAT);
   iDampening->SetValue(JIGGLE_DEFAULT_DAMPENING, 0);
   
   iMass = GetISpinner(GetDlgItem(dynDlg,IDC_JIGGLE_MASS_SPIN));
   iMass->SetLimits(-10000.0f,10000.0f,FALSE);
   iMass->SetScale(1.0f);
   iMass->LinkToEdit(GetDlgItem(dynDlg,IDC_JIGGLE_MASS),EDITTYPE_FLOAT);

   iDrag = GetISpinner(GetDlgItem(dynDlg,IDC_JIGGLE_DRAG_SPIN));
   iDrag->SetLimits(0.0f,10.0f,FALSE);
   iDrag->SetScale(0.01f);
   iDrag->LinkToEdit(GetDlgItem(dynDlg,IDC_JIGGLE_DRAG),EDITTYPE_FLOAT);
         
   float mass, drag;
   mass = cont->partsys->GetParticle(0)->GetMass();
   drag = cont->partsys->GetParticle(0)->GetDrag();
   
   iMass->SetValue(mass, FALSE);
   iDrag->SetValue(drag, FALSE);
   
   int how;
   Interval for_ever = FOREVER;
   cont->dyn_pb->GetValue(jig_how, 0, how, for_ever);
   if (how == 0) how = IDC_RELATIVE_RB;
      else how = IDC_ABSOLUTE_RB;
   CheckRadioButton(dynDlg, IDC_RELATIVE_RB, IDC_ABSOLUTE_RB, how);

   cont->UpdateNodeList();
}

void JiggleDlg::DestroyDynamicsParams()
{
   if (cont->ip) cont->ip->ClearPickMode();
   if (!(cont->hParams1))
   {
      delete cont->pickNodeMode; 
      cont->pickNodeMode = NULL;
   }
   ReleaseISpinner(iTension);    iTension = NULL;
   ReleaseISpinner(iDampening);  iDampening = NULL;
   ReleaseISpinner(iMass);       iMass = NULL;
   ReleaseISpinner(iDrag);       iDrag = NULL;
   ReleaseICustButton(iAddBone); iAddBone = NULL;
   ReleaseICustButton(iDeleteBone); iDeleteBone = NULL;
}

void JiggleDlg::Invalidate()
{
   if (!blockRedraw)
   {
      valid = FALSE;
      if (hWnd) 
         InvalidateRect(hWnd,NULL,FALSE);
   }
}

void JiggleDlg::Update()
{
   if (!valid && dynDlg && cont && cont->dyn_pb) 
   {     
      if (!cont->partsys->GetParticle(0)->GetSprings()->length())
         cont->SetSelfReference();
      
      int radioSel = 0;
	  Interval for_ever = FOREVER;
      cont->dyn_pb->GetValue(jig_how, 0, radioSel, for_ever);
      
      int nSelItemsInBuffer; 
      int nBuffer[100]; 

      nSelItemsInBuffer = SendMessage(GetDlgItem(dynDlg, IDC_LIST2), LB_GETSELITEMS, 100, (LPARAM) nBuffer); 
      
      if (nSelItemsInBuffer == 0)
      {
         iDeleteBone->Disable();
         return;
      }

      if (nSelItemsInBuffer == 1 && nBuffer[0] == 0)
      {
         iDeleteBone->Disable();
      }else iDeleteBone->Enable();

      if (radioSel == SET_PARAMS_RELATIVE)
      {
         iTension->SetValue(0.0f, 0);
         iDampening->SetValue(0.0f, 0);
      } else {
         float tension = 0.0f;
         float dampening = 0.0f;
         BOOL tensionsDiffer = FALSE;
         BOOL dampsDiffer = FALSE;

         for (int i = nSelItemsInBuffer - 1; i >= 0; i--) 
         { 
            if (i == nSelItemsInBuffer - 1){
               tension = cont->partsys->GetParticle(0)->GetSpring(nBuffer[i])->GetTension();
               dampening = cont->partsys->GetParticle(0)->GetSpring(nBuffer[i])->GetDampening();
            }
            if ( tensionsDiffer == FALSE && tension != cont->partsys->GetParticle(0)->GetSpring(nBuffer[i])->GetTension())
               tensionsDiffer = TRUE;
            if ( dampsDiffer == FALSE && dampening != cont->partsys->GetParticle(0)->GetSpring(nBuffer[i])->GetDampening())
               dampsDiffer = TRUE;
         }
         if (tensionsDiffer) iTension->SetIndeterminate();
         else {
            iTension->SetIndeterminate(FALSE);
            iTension->SetValue(tension, 0);
         }
         if (dampsDiffer) iDampening->SetIndeterminate();
         else {
            iDampening->SetValue(dampening, 0);
            iDampening->SetIndeterminate(FALSE);
         }
      }
      iMass->SetValue(cont->partsys->GetParticle(0)->GetMass(), 0);
      iDrag->SetValue(cont->partsys->GetParticle(0)->GetDrag(), 0);

      int how;
	  for_ever = FOREVER;
      cont->dyn_pb->GetValue(jig_how, 0, how, for_ever);
      if (how == 0) how = IDC_RELATIVE_RB;
         else how = IDC_ABSOLUTE_RB;
      CheckRadioButton(dynDlg, IDC_RELATIVE_RB, IDC_ABSOLUTE_RB, how);

      valid = TRUE;
   }
}

void JiggleDlg::UpdateForceList()
{
   int i, ct;

   ct = cont->force_pb->Count(jig_force_node);
   SendDlgItemMessage(forceDlg, IDC_LIST1, LB_RESETCONTENT, 0, 0);

   for(i = 0; i < ct; i++)
   {
      INode* node = NULL;
	  Interval for_ever = FOREVER;
      cont->force_pb->GetValue(jig_force_node, 0, node, for_ever, i);
      TCHAR name[256];
      if (node != NULL) _tcscpy(name, node->GetName());
         else _tcscpy(name, _T("<deleted>"));
      SendDlgItemMessage(forceDlg, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)name);
   }
   Invalidate();
   //Update();  //I shouldn't need this here but I do.
}

void JiggleDlg::UpdateForceSpinners()
{
   int steps, start;
   float x, y, z;
   TimeValue t = GetCOREInterface()->GetTime();

#pragma warning(push)
#pragma warning(disable:4239)
   cont->force_pb->GetValue(jig_tolerence,t,steps,FOREVER);
   cont->force_pb->GetValue(jig_start,t,start,FOREVER);
   cont->force_pb->GetValue(jig_xeffect,t,x,FOREVER);
   cont->force_pb->GetValue(jig_yeffect,t,y,FOREVER);
   cont->force_pb->GetValue(jig_zeffect,t,z,FOREVER);
#pragma warning(pop)
   xSpin->SetValue(x, FALSE);
   ySpin->SetValue(y, FALSE);
   zSpin->SetValue(z, FALSE);
   xSpin->SetKeyBrackets(cont->force_pb->KeyFrameAtTimeByID(jig_xeffect, t));
   ySpin->SetKeyBrackets(cont->force_pb->KeyFrameAtTimeByID(jig_yeffect, t));
   zSpin->SetKeyBrackets(cont->force_pb->KeyFrameAtTimeByID(jig_zeffect, t));
   startSpin->SetValue(start, FALSE);
   stepsSpin->SetValue(steps, FALSE);

}


void JiggleDlg::SetupUI(){ }


int JiggleDlg::NumRefs() { return 1; }


RefTargetHandle JiggleDlg::GetReference(int i)
{
	if (i == 0)
	{
		return cont;	
	}
	return NULL;
}

void JiggleDlg::SetReference(int i, RefTargetHandle rtarg)
{
	if (i == 0)
	{
		cont=(Jiggle*)rtarg;
	}
}

RefResult JiggleDlg::NotifyRefChanged(
      Interval , 
      RefTargetHandle , 
      PartID& , 
      RefMessage message)
{
   switch (message) {
      case REFMSG_CHANGE:
         if (!blockRedraw) Invalidate();
         break;
            
      case REFMSG_REF_DELETED:
         MaybeCloseWindow();
         break;
      }
   return REF_SUCCEED;
}


//Functions for subclass of Jiggle Dialog
//***************************************************
HWND JiggleTrackDlg::CreateWin(HWND hParent)
   {
   return CreateDialogParam(
      hInstance,
      MAKEINTRESOURCE(IDD_JIGGLE_TRACK),
      hParent,
      JiggleDlgProc,
      (LPARAM)this);
   }

void JiggleTrackDlg::MaybeCloseWindow()
{
   CheckForNonJiggleDlg check(cont);
   cont->DoEnumDependents(&check);
   if (!check.non) 
      PostMessage(hWnd,WM_CLOSE,0,0);
}

void DynamicsPBAccessor::Set(PB2Value& , ReferenceMaker* owner, ParamID id, int /*tabIndex*/, TimeValue )    // set from v
{
   Jiggle* cont = (Jiggle*)owner;
   switch (id)
   {
      case jig_how:
         if (cont->pmap) cont->pmap->Update(GetCOREInterface()->GetTime());
         if (cont->dlg) { cont->dlg->valid = false; cont->dlg->Update(); }
         break;
      default: break;

   }
}

void DynamicsPBAccessor::Get(PB2Value& , ReferenceMaker* /*owner*/, ParamID id, int /*tabIndex*/, TimeValue , Interval& )    // get into v
{
	// Is this method finished?
	switch (id) 
	{
	case jig_how:
		break;
	default: break;
	}
}


void ForcesPBAccessor::Set(PB2Value& , ReferenceMaker* owner, ParamID id, int /*tabIndex*/, TimeValue )    // set from v
{
   Jiggle* cont = (Jiggle*)owner;
   switch (id)
   {
      case jig_start: 
      case jig_tolerence:
      case jig_xeffect:
      case jig_yeffect:
      case jig_zeffect:
         if (cont->dlg) cont->dlg->UpdateForceSpinners();
         break;
      default: break;
   }
}

void ForcesPBAccessor::Get(PB2Value& , ReferenceMaker* /*owner*/, ParamID id, int /*tabIndex*/, TimeValue , Interval& )    // get into v
{
	// Is this method finished?
	switch (id)
	{
	case jig_start: 
		break;
	default: break;
	}
}

void ForcesPBAccessor::TabChanged(tab_changes /*changeCode*/, Tab<PB2Value>* /*tab*/, ReferenceMaker* owner, ParamID id, int /*tabIndex*/, int /*count*/) 
{ 
   Jiggle* cont = (Jiggle*)owner;
   switch (id)
   {
      case jig_force_node:
         if (cont->dlg) cont->dlg->UpdateForceList();
         break;
      default: break;
   }

}
