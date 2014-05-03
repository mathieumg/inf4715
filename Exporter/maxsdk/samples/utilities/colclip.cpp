/**********************************************************************
 *<
   FILE: colclip.cpp

   DESCRIPTION:  A simple color swatch clipboard

   CREATED BY: Rolf Berteig

   HISTORY: created December 23 1995

 *>   Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/
#include "util.h"
#include "utilapi.h"
#include "helpsys.h"

#include "3dsmaxport.h"
#include "maxtextfile.h"

#ifndef NO_UTILITY_COLORCLIPBOARD   // russom - 12/04/01

#define COLOR_CLIP_CLASS_ID      0x8fbc04e9

#define NUM_COLORS         4
#define NUM_FLOAT_COLORS   12

static int csIDs[] = {
   IDC_COLOR_SWATCH1,IDC_COLOR_SWATCH2,IDC_COLOR_SWATCH3,
   IDC_COLOR_SWATCH4,IDC_COLOR_SWATCH5,IDC_COLOR_SWATCH6,
   IDC_COLOR_SWATCH7,IDC_COLOR_SWATCH8,IDC_COLOR_SWATCH9,
   IDC_COLOR_SWATCH10,IDC_COLOR_SWATCH11,IDC_COLOR_SWATCH12};

class ColorClip : public UtilityObj {
   public:
      IUtil *iu;
      Interface *ip;
      HWND hPanel;
      AColor colors[NUM_COLORS];
      IColorSwatch *cs[NUM_COLORS];    

      ColorClip();
      void BeginEditParams(Interface *ip,IUtil *iu);
      void EndEditParams(Interface *ip,IUtil *iu);
      void DeleteThis() {}

      void Init(HWND hWnd);
      void Destroy(HWND hWnd);
      void CreateNewFloater();
   };
static ColorClip theColorClip;

class ColorClipClassDesc:public ClassDesc {
   public:
   int         IsPublic() {return 1;}
   void *         Create(BOOL loading = FALSE) {return &theColorClip;}
   const TCHAR *  ClassName() {return GetString(IDS_RB_COLORCLIPBOARD);}
   SClass_ID      SuperClassID() {return UTILITY_CLASS_ID;}
   Class_ID    ClassID() {return Class_ID(COLOR_CLIP_CLASS_ID,0);}
   const TCHAR*   Category() {return _T("");}
   };

static ColorClipClassDesc colorClipDesc;
ClassDesc* GetColorClipDesc() {return &colorClipDesc;}


static INT_PTR CALLBACK ColorClipDlgProc(
      HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
   {
   switch (msg) {
      case WM_INITDIALOG:
         theColorClip.Init(hWnd);
         break;

      case WM_DESTROY:
         theColorClip.Destroy(hWnd);
         break;

      case WM_COMMAND:
         switch (LOWORD(wParam)) {
            case IDOK:
               theColorClip.iu->CloseUtility();
               break;
            
            case IDC_COLORCLIP_NEWFLOAT:
               theColorClip.CreateNewFloater();
               break;
            }
         break;

      case CC_COLOR_CHANGE:
         switch (LOWORD(wParam)) {
            case IDC_COLOR_SWATCH1:
               theColorClip.colors[0] = theColorClip.cs[0]->GetAColor();
               break;   
            case IDC_COLOR_SWATCH2:
               theColorClip.colors[1] = theColorClip.cs[1]->GetAColor();
               break;   
            case IDC_COLOR_SWATCH3:
               theColorClip.colors[2] = theColorClip.cs[2]->GetAColor();
               break;
            case IDC_COLOR_SWATCH4:
               theColorClip.colors[3] = theColorClip.cs[3]->GetAColor();
               break;
            }
         break;

      case WM_LBUTTONDOWN:
      case WM_LBUTTONUP:
      case WM_MOUSEMOVE:
         theColorClip.ip->RollupMouseMessage(hWnd,msg,wParam,lParam); 
         break;

      default:
         return FALSE;
      }
   return TRUE;
   }  

static TCHAR fdir[_MAX_PATH] = {_T('\0')};
static TCHAR fname[_MAX_PATH] = {_T('\0')};

void SetupTitle(HWND hWnd,TSTR &name)
   {
   TSTR fl, ext;
   SplitFilename(name,NULL,&fl,&ext);
   fl = TSTR(GetString(IDS_RB_COLORCLIPSHORT)) + 
      TSTR(_T(" - ")) + fl + ext;
   SetWindowText(hWnd,fl);
   }

static void LoadColorFile(HWND hWnd)
   {  
   // RB 10/15/2000: Init directory string
   if (fdir[0]==_T('\0')) {
      _tcscpy(fdir, GetCOREInterface()->GetDir(APP_IMAGE_DIR));
      }

   OPENFILENAME ofn;
   memset(&ofn,0,sizeof(ofn));
   FilterList fl;
   fl.Append(GetString(IDS_RB_COLORCLIPFILES));
   fl.Append(_T("*.ccb"));
   TSTR title = GetString(IDS_RB_LOADCOLOR);
   ofn.lStructSize     = sizeof(OPENFILENAME);  // No OFN_ENABLEHOOK
   ofn.hwndOwner       = hWnd;
   ofn.lpstrFilter     = fl;
   ofn.lpstrFile       = fname;
   ofn.nMaxFile        = 256;    
   ofn.lpstrInitialDir = fdir;
   ofn.Flags           = OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST;
   ofn.FlagsEx         = OFN_EX_NOPLACESBAR;
   ofn.lpstrDefExt     = _T("ccb");
   ofn.lpstrTitle      = title;

   if (GetOpenFileName(&ofn)) {
      MaxSDK::Util::TextFile::Reader file;	
      if(!file.Open(fname)) {
         TSTR buf2 = GetString(IDS_RB_COLORCLIPBOARD);
         TSTR buf1;
         buf1.printf(GetString(IDS_RB_CANTOPENFILE),fname);       
         MessageBox(hWnd,buf1,buf2,MB_ICONEXCLAMATION);
         return;
         }
      TSTR colorLine;
      // pre-R6
      for (int i=0; i<12; i++) {
         int r, g, b;
		 colorLine = file.ReadLine();
		 if(EOF == _stscanf(colorLine, _T("%d %d %d\n"), &r, &g, &b)) break;
         Color color(float(r)/255.0f,float(g)/255.0f,float(b)/255.0f);
         TSTR name;
         name.printf(GetString(IDS_RB_COLORNUM),i);
         IColorSwatch *cs = GetIColorSwatch(GetDlgItem(hWnd,csIDs[i]),color,name);
         ReleaseIColorSwatch(cs);
         }     
      
      // post-R6
      for (int i=0; i<12; i++) {
         float r, g, b, a;
		 colorLine = file.ReadLine();
         if(EOF == _stscanf(colorLine, _T("%f %f %f %f\n"),&r, &g, &b, &a)) break;
         AColor color(r,g,b,a);
         TSTR name;
         name.printf(GetString(IDS_RB_COLORNUM),i);
         IColorSwatch *cs = GetIColorSwatch(GetDlgItem(hWnd,csIDs[i]),color,name);
         ReleaseIColorSwatch(cs);
      }     

      file.Close();

      TSTR *fileName = DLGetWindowLongPtr<TSTR*>(hWnd);
      delete fileName;
      fileName = new TSTR(fname);
      DLSetWindowLongPtr(hWnd, fileName);
      SetupTitle(hWnd,*fileName);
      }  
   }

static void SaveColorFile(HWND hWnd,TSTR &name)
   {
    MaxSDK::Util::TextFile::Writer file;	

   	Interface14 *iface = GetCOREInterface14();
	UINT codepage  = iface-> DefaultTextSaveCodePage(true); //dll\PaintLayerMod\ColorPaletteTool.cpp :: SaveColorFile, UTF8 allowed, keep to same.

   if(!file.Open(fname, false, MaxSDK::Util::TextFile::Writer::WRITE_BOM | codepage )){
      TSTR buf2 = GetString(IDS_RB_COLORCLIPBOARD);
      TSTR buf1;
      buf1.printf(GetString(IDS_RB_CANTOPENFILE),fname);       
      MessageBox(hWnd,buf1,buf2,MB_ICONEXCLAMATION);
      return;
      }

   for (int i=0; i<12; i++) {
      int r, g, b;
      IColorSwatch *cs = GetIColorSwatch(GetDlgItem(hWnd,csIDs[i]));
      COLORREF col = cs->GetColor();
      ReleaseIColorSwatch(cs);
      r = GetRValue(col); g = GetGValue(col); b = GetBValue(col);
      file.Printf(_T("%d %d %d\n"), r, g, b);
      }  

   for (int i=0; i<12; i++) {
      IColorSwatch *cs = GetIColorSwatch(GetDlgItem(hWnd,csIDs[i]));
      AColor col = cs->GetAColor();
      ReleaseIColorSwatch(cs);
      file.Printf(_T("%f %f %f %f\n"), col.r, col.g, col.b, col.a);
   }  

   SetupTitle(hWnd,name);
   file.Close();
   }

static void SaveAsColorFile(HWND hWnd) 
   {
   TSTR *fileName = DLGetWindowLongPtr<TSTR*>(hWnd);
   if (fileName) _tcscpy(fname,fileName->data());  

   // RB 10/15/2000: Init directory string
   if (fdir[0]==_T('\0')) {
      _tcscpy(fdir, GetCOREInterface()->GetDir(APP_IMAGE_DIR));
      }

   OPENFILENAME ofn;
   memset(&ofn,0,sizeof(ofn));
   FilterList fl;
   fl.Append(GetString(IDS_RB_COLORCLIPFILES));
   fl.Append(_T("*.ccb"));
   TSTR title = GetString(IDS_RB_SAVECOLOR);
   ofn.lStructSize     = sizeof(OPENFILENAME);  // No OFN_ENABLEHOOK
   ofn.hwndOwner       = hWnd;
   ofn.lpstrFilter     = fl;
   ofn.lpstrFile       = fname;
   ofn.nMaxFile        = 256;    
   ofn.lpstrInitialDir = fdir;
   ofn.Flags           = OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST;
   ofn.FlagsEx         = OFN_EX_NOPLACESBAR;
   ofn.lpstrDefExt     = _T("ccb");
   ofn.lpstrTitle      = title;

tryAgain:
   if (GetSaveFileName(&ofn)) {
      if (DoesFileExist(fname)) {
         TSTR buf1;
         TSTR buf2 = GetString(IDS_RB_SAVECOLOR);
         buf1.printf(GetString(IDS_RB_FILEEXISTS),fname);
         if (IDYES!=MessageBox(
            hWnd,
            buf1,buf2,MB_YESNO|MB_ICONQUESTION)) {
            goto tryAgain;
            }
         }
      
      if (!fileName) fileName = new TSTR;
      *fileName = fname;
      DLSetWindowLongPtr(hWnd, fileName);

      SaveColorFile(hWnd,*fileName);      
      }  
   }

static INT_PTR CALLBACK ColorClipFloaterDlgProc(
      HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
   {
   switch (msg) {
      case WM_INITDIALOG: {
         SetWindowContextHelpId(hWnd, idh_colorclip_floater);
         CenterWindow(hWnd,GetParent(hWnd));
         Color color(0.5f,0.5f,0.5f);
         for (int i=0; i<NUM_FLOAT_COLORS; i++) {
            TSTR name;
            name.printf(GetString(IDS_RB_COLORNUM),i);
            IColorSwatch *cs = GetIColorSwatch(
               GetDlgItem(hWnd,csIDs[i]),color,name);
            cs->SetUseAlpha(TRUE);
            ReleaseIColorSwatch(cs);
            }
         break;
         }

      case WM_SYSCOMMAND:
         if ((wParam & 0xfff0) == SC_CONTEXTHELP) {
            MaxSDK::IHelpSystem::GetInstance()->ShowProductHelpForTopic(idh_colorclip_floater);          
            }
         return FALSE;
      
      case WM_DESTROY: {
         TSTR *fileName = DLGetWindowLongPtr<TSTR*>(hWnd);
         delete fileName;
         break;
         }

      case WM_COMMAND:
         switch (LOWORD(wParam)) {
            case IDC_COLOR_LOAD:
               LoadColorFile(hWnd);
               break;
            
            case IDC_COLOR_SAVE: {
               TSTR *fileName = DLGetWindowLongPtr<TSTR*>(hWnd);
               if (fileName) SaveColorFile(hWnd,*fileName);
               else SaveAsColorFile(hWnd);                  
               break;
               }

            case IDC_COLOR_SAVEAS:
               SaveAsColorFile(hWnd);
               break;

            case IDOK:
            case IDCANCEL:
               DestroyWindow(hWnd);
               break;
            }
         break;

      default:
         return FALSE;
      }

   return TRUE;
   }

ColorClip::ColorClip()
   {
   iu = NULL;
   ip = NULL;  
   hPanel = NULL;
   srand(0);
   for (int i=0; i<NUM_COLORS; i++) {     
      cs[i] = NULL;
      colors[i].r = float(rand())/float(RAND_MAX);
      colors[i].g = float(rand())/float(RAND_MAX);
      colors[i].b = float(rand())/float(RAND_MAX);
      colors[i].a = 1.0f;
      }  
   }

void ColorClip::BeginEditParams(Interface *ip,IUtil *iu) 
   {
   this->iu = iu;
   this->ip = ip;
   hPanel = ip->AddRollupPage(
      hInstance,
      MAKEINTRESOURCE(IDD_COLORCLIP_PANEL),
      ColorClipDlgProc,
      GetString(IDS_RB_COLORCLIPBOARD),
      0);
   }
   
void ColorClip::EndEditParams(Interface *ip,IUtil *iu) 
   {
   this->iu = NULL;
   this->ip = NULL;
   ip->DeleteRollupPage(hPanel);
   hPanel = NULL;
   }

void ColorClip::Init(HWND hWnd)
   {
   for (int i=0; i<NUM_COLORS; i++) {
      TSTR name;
      name.printf(GetString(IDS_RB_COLORNUM),i);
      cs[i] = GetIColorSwatch(GetDlgItem(hWnd,csIDs[i]),colors[i],name);
      cs[i]->SetUseAlpha(TRUE);
      cs[i]->SetAColor(colors[i]);
      }
   }

void ColorClip::Destroy(HWND hWnd)
   {
   for (int i=0; i<NUM_COLORS; i++) {
      ReleaseIColorSwatch(cs[i]);
      cs[i] = NULL;
      }
   }

void ColorClip::CreateNewFloater()
   {
   CreateDialog(
      hInstance,
      MAKEINTRESOURCE(IDD_COLORCLIP_FLOATER),
      ip->GetMAXHWnd(),
      ColorClipFloaterDlgProc);
   }

#endif // NO_UTILITY_COLORCLIPBOARD