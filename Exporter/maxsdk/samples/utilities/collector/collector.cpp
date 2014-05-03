/*----------------------------------------------------------------------*
 |
 |	FILE: Collector.cpp
 | 
 |	DESC: Resource Collection plugin
 |
 |	AUTH: Harry Denholm, Kinetix
 |		  Copyright (c) 1998, All Rights Reserved.
 |
 |	HISTORY: 27.2.98
 |
 *----------------------------------------------------------------------*/


#include "collector.h"
#include "IPathConfigMgr.h"
#include "AssetManagement/iassetmanager.h"
#include "IFileResolutionManager.h"
#include "maxtextfile.h"

using namespace MaxSDK::AssetManagement;

#define CL_CLASSID	Class_ID(0x57ab2b60, 0x590572f5)
// two handy macros to set cursors for busy or normal operation
#define UI_MAKEBUSY			SetCursor(LoadCursor(NULL, IDC_WAIT));
#define UI_MAKEFREE			SetCursor(LoadCursor(NULL, IDC_ARROW));

HINSTANCE hInstance;

BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved)
{
   if( fdwReason == DLL_PROCESS_ATTACH )
   {
      MaxSDK::Util::UseLanguagePackLocale();
      hInstance = hinstDLL;
      DisableThreadLibraryCalls(hInstance);
   }

	return (TRUE);
}

__declspec( dllexport ) const TCHAR* LibDescription()
{
	return GetString(IDS_LIBDESCRIPTION);
}

//TODO: Must change this number when adding a new class
__declspec( dllexport ) int LibNumberClasses()
{
	return 1;
}

__declspec( dllexport ) ClassDesc* LibClassDesc(int i)
{
	switch(i) {
		case 0: return GetCollectionDesc();
		default: return 0;
	}
}

__declspec( dllexport ) ULONG LibVersion()
{
	return VERSION_3DSMAX;
}

TCHAR *GetString(int id)
{
	static TCHAR buf[256];

	if (hInstance)
		return LoadString(hInstance, id, buf, _countof(buf)) ? buf : NULL;
	return NULL;
}

class Collection : public UtilityObj {
	public:
		IUtil *iu;
		Interface *ip;
		HWND hPanel;
		
		TCHAR WZPath[255];
		BOOL WZisHere;

		Collection();
		~Collection();

		NameTab mapPaths;
		
		void BeginEditParams(Interface *ip,IUtil *iu);
		void EndEditParams(Interface *ip,IUtil *iu);
		void DeleteThis() {}

		void Init(HWND hWnd);
		void Destroy(HWND hWnd);

};

static Collection theCollection;


class CollectionClassDesc:public ClassDesc {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading = FALSE) {return &theCollection;}
	const TCHAR *	ClassName() {return GetString(IDS_RESOURCE_COLLECTOR);}
	SClass_ID		SuperClassID() {return UTILITY_CLASS_ID;}
	Class_ID		ClassID() {return CL_CLASSID;}
	const TCHAR* 	Category() {return _T("");}
};

static CollectionClassDesc CollectionDesc;
ClassDesc* GetCollectionDesc() {return &CollectionDesc;}


#define DXMATERIAL_DYNAMIC_UI Class_ID(0xef12512, 0x11351ed1)

bool IsDynamicDxMaterial(MtlBase * newMtl)
{

	DllDir * lookup = GetCOREInterface()->GetDllDirectory();
	ClassDirectory & dirLookup = lookup->ClassDir();

	ClassDesc * cd = dirLookup.FindClass(MATERIAL_CLASS_ID,newMtl->ClassID());
	if(cd && cd->SubClassID() == DXMATERIAL_DYNAMIC_UI)
		return true;
	else
		return false;


}

void CreateNewFilename(const AssetUser& name, AssetUser& newFile)
{
	TCHAR tfile[MAX_PATH];
	TCHAR textn[MAX_PATH];
	TCHAR newFileName[MAX_PATH];
	TCHAR to_path[255];
	GetWindowText(GetDlgItem(theCollection.hPanel,IDC_PATH),to_path,254);

	BMMSplitFilename(name.GetFileName(), NULL, tfile, textn);
	BMMAppendSlash(to_path);
	_stprintf(newFileName,_T("%s%s%s"),to_path,tfile,textn);
	newFile = IAssetManager::GetInstance()->GetAsset(newFileName,kBitmapAsset);
}

class CMtlEnum {
	public:

		virtual void  proc(MtlBase *m){
			Interface *ip = theCollection.ip;

			// Check for bitmaptex, and cast
			if(m->ClassID()==Class_ID(BMTEX_CLASS_ID,0)) {
				Texmap *bt = (Texmap*)m;
				BitmapTex *b = (BitmapTex*)bt;

				if(_tcsicmp(b->GetMapName(),_T(""))!=0){
					
					// Use the very useful BMMGetFull.. to scan for existing files
					BitmapInfo bi(b->GetMapName());
					BMMGetFullFilename(&bi);

					AssetUser BFile = bi.GetAsset();

					// If our found file exists, add it to the list
					if(BMMIsFile(BFile.GetFileName())){
						TSTR s = BFile.GetFileName();

						// check for update, and do so if needed
						BOOL upd = GetCheckBox(theCollection.hPanel,IDC_UPMAP);
						if (upd) {
							AssetUser asset;
							CreateNewFilename(BFile,asset);
							b->SetMapName(asset.GetFileName());
						}

						if(theCollection.mapPaths.FindName(BFile.GetFileName())==-1){
							theCollection.mapPaths.AddName(s);
						}
					}
				}
			}
			if(IsDynamicDxMaterial(m))
			{
				IDxMaterial * idxm = (IDxMaterial*)m->GetInterface(IDXMATERIAL_INTERFACE);
				if (NULL != idxm)
				{
					int bitmapCount = idxm->GetNumberOfEffectBitmaps();
					for(int j=0; j<bitmapCount;j++)
					{
						PBBitmap* bmap = idxm->GetEffectBitmap(j);
						if(bmap != NULL)
						{
							AssetUser BFile = bmap->bi.GetAsset();
							// add an entry in the data type index
							BOOL upd = GetCheckBox(theCollection.hPanel,IDC_UPMAP);
							if (upd) {
								AssetUser asset;
								CreateNewFilename(BFile,asset);
								PBBitmap newBitmap;
								newBitmap.bi.SetAsset(asset);
								idxm->SetEffectBitmap(j,&newBitmap);
							}
							if(theCollection.mapPaths.FindName(BFile.GetFileName())==-1)
								theCollection.mapPaths.AddName(BFile.GetFileName());
						}
					}
					PBBitmap * bm;
					bm = idxm->GetSoftwareRenderBitmap();
					if(bm)
					{
						AssetUser BFile = bm->bi.GetAsset();
						BOOL upd = GetCheckBox(theCollection.hPanel,IDC_UPMAP);
						if (upd) {
							AssetUser asset;
							CreateNewFilename(BFile,asset);
							PBBitmap newBitmap;
							newBitmap.bi.SetAsset(asset);
							idxm->SetSoftwareRenderBitmap(&newBitmap);
						}
						if(theCollection.mapPaths.FindName(BFile.GetFileName())==-1)
							theCollection.mapPaths.AddName(BFile.GetFileName());

					}
					AssetUser effectFile = idxm->GetEffectFile();
					BOOL upd = GetCheckBox(theCollection.hPanel,IDC_UPMAP);
					if (upd) {
						AssetUser asset;
						CreateNewFilename(effectFile,asset);
						idxm->SetEffectFile(asset);
					}
					if(theCollection.mapPaths.FindName(effectFile.GetFileName())==-1)
						theCollection.mapPaths.AddName(effectFile.GetFileName());
				}
			}
		}
};


CMtlEnum CEnym;

// do the material enumeration
void CEnumMtlTree(MtlBase *mb, CMtlEnum &tenum) {
	tenum.proc(mb);
	for (int i=0; i<mb->NumSubTexmaps(); i++) {
		Texmap *st = mb->GetSubTexmap(i); 
		if (st) 
			CEnumMtlTree(st,tenum);
		}
	if (IsMtl(mb)) {
		Mtl *m = (Mtl *)mb;
		for (int i=0; i<m->NumSubMtls(); i++) {
			Mtl *sm = m->GetSubMtl(i);
			if (sm) 
				CEnumMtlTree(sm,tenum);
			}
		}
	}

/**
 * Callback class for retrieving light distribution files
 */
	
class EnumLightDistFileCallBack : public AssetEnumCallback
{
public:

	NameTab fileList;
	int size;

	EnumLightDistFileCallBack() { size = 0; }

	void RecordAsset(const MaxSDK::AssetManagement::AssetUser& asset)
	{
		//convert to lower case
		//SS 3/17/2003 defect 490612: The size of the filename buffer in BitmapInfo is
		// MAX_PATH, or 260 characters. We need that same size here.
		TSTR fullPath = asset.GetFullFilePath();
		if(fullPath.isNull())
			fullPath = asset.GetFileName();
		//Convert to lower case.
		fullPath.toLower();
		// check for distribution types before appending.
		if(_tcsstr(fullPath, _T(".ies")) || 
			_tcsstr(fullPath, _T(".cibse")) ||
			_tcsstr(fullPath, _T(".ltli")))		{
			if(fileList.FindName(fullPath) < 0)	{
				fileList.AddName(fullPath);
				size++;
			}
		}
			
	}
};

EnumLightDistFileCallBack distributionLister;

// Enumerate the scene
void NodeEnum(INode *root)
{
	for (int k=0; k<root->NumberOfChildren(); k++)
	{
		INode *node = root->GetChildNode(k);
		MtlBase* mat = (MtlBase*)node->GetMtl();
		if (mat)
			CEnumMtlTree(mat,CEnym);
		// enumerate light distribution files
		node->EnumAuxFiles(distributionLister, FILE_ENUM_ALL);
		if(node->NumberOfChildren()>0) NodeEnum(node);
	}
}

TSTR defaultDir = _T("");

static INT_PTR CALLBACK CollectionDlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Interface *ip;
	ip = theCollection.ip;


	switch (msg) {
		case WM_INITDIALOG:
			theCollection.Init(hWnd);
			break;

		case WM_DESTROY:
			theCollection.Destroy(hWnd);
			break;

		case WM_COMMAND:

			switch(HIWORD(wParam)){
				case EN_SETFOCUS :
					DisableAccelerators();
					break;
				case EN_KILLFOCUS :
					EnableAccelerators();
					break;
			}

			switch (LOWORD(wParam)) {

			case IDC_BROWSE:
               TCHAR dir[256];
               TCHAR desc[256];
               _tcscpy(desc, GetString(IDS_TARGET_DIRECTORY));
               if(_tcscmp(defaultDir, _T("")) == 0)   {
                  defaultDir = IPathConfigMgr::GetPathConfigMgr()->GetDir(APP_ARCHIVES_DIR);
               }
               if(defaultDir.Length() < 256)    {
                  _tcscpy(dir, defaultDir);
               }
               ip->ChooseDirectory(hWnd, GetString(IDS_CHOOSE_OUTPUT), dir, desc);
               if (dir[0])    {
						SetWindowText(GetDlgItem(hWnd,IDC_PATH),dir);
			   }
               break;

			case IDC_GO:
				{
				UI_MAKEBUSY
				theCollection.mapPaths.ZeroCount();
				distributionLister.fileList.ZeroCount();

				// Collect bitmaps
				BOOL doMAPS = GetCheckBox(hWnd,IDC_PACKMAPS);

				if(doMAPS) 
				{
					INode *root = ip->GetRootNode();
					IFileResolutionManager::GetInstance()->PushAllowCachingOfUnresolvedResults(true);
					NodeEnum(root);
					IFileResolutionManager::GetInstance()->PopAllowCachingOfUnresolvedResults();
				}

				// Check and store MAX File
				BOOL doMAX = GetCheckBox(hWnd,IDC_PACKMAX);
				TCHAR winZipName[MAX_PATH];

				if(_tcsicmp(ip->GetCurFilePath(),_T(""))==0) 
				{
					TSTR msg;
					msg.printf(GetString(IDS_NO_VALID_FILE_PATH));
					MessageBox(NULL, msg, GetString(IDS_WARNING), MB_OK);

					// No file name. Assign a default.
					_tcscpy(winZipName,_T("UntitledScene.ZIP"));
				}
				else
				{
					if (doMAX) theCollection.mapPaths.AddName(ip->GetCurFilePath()); 

					// Figure out a Winzip ZIP file name from the filepath
					TCHAR tfile[MAX_PATH];
						
					BMMSplitFilename(ip->GetCurFilePath(), NULL, tfile, NULL);
					_stprintf(winZipName,_T("%s.ZIP"),tfile);
				}
				


				// Do the copy/move options
				TCHAR to_path[255];
				TCHAR fnl[MAX_PATH];
				GetWindowText(GetDlgItem(hWnd,IDC_PATH),to_path,254);
				BOOL type;

				type = GetCheckBox(hWnd,IDC_TYPE);

				NameTab zipList; zipList.ZeroCount();

				BOOL zipup = GetCheckBox(hWnd,IDC_PACKUP);
				
				// then do copy/move command
				for (int q=0; q<theCollection.mapPaths.Count(); q++)
				{
					TCHAR tfile[MAX_PATH];
					TCHAR textn[MAX_PATH];
					
					BMMSplitFilename(theCollection.mapPaths[q], NULL, tfile, textn);
					BMMAppendSlash(to_path);

					_stprintf(fnl,_T("%s%s%s"),to_path,tfile,textn);
					if(type)CopyFile(theCollection.mapPaths[q],fnl,FALSE);
					if(!type)MoveFile(theCollection.mapPaths[q],fnl);
					zipList.AddName(fnl);
				}

				// do the same for light distribution files
				// August 29, 2001 David Cunningham
				for (int q=0; q<distributionLister.fileList.Count(); q++)
				{
					TCHAR tfile[MAX_PATH];
					TCHAR textn[MAX_PATH];
					
					BMMSplitFilename(distributionLister.fileList[q], NULL, tfile, textn);
					BMMAppendSlash(to_path);

					_stprintf(fnl,_T("%s%s%s"),to_path,tfile,textn);
					if(type)CopyFile(distributionLister.fileList[q],fnl,FALSE);
					if(!type)MoveFile(distributionLister.fileList[q],fnl);

					zipList.AddName(fnl);
				}
				

				// Create file list, and WINZIP the files together
				if(zipup&&theCollection.WZisHere)
				{
					TCHAR zpn[MAX_PATH];
					_stprintf(zpn,_T("%sLIST$PCK.TXT"),to_path);
					MaxSDK::Util::TextFile::Writer stream;

					if(!stream.Open(zpn, false, MaxSDK::Util::TextFile::Writer::WRITE_BOM | CP_UTF8 )) return FALSE;
					for (int u=0;u<zipList.Count();u++)
					{
					 stream.Printf(_T("%s\n"),zipList[u]);
					}
					stream.Close();

					//Check maxzip.exe
					TCHAR curDir[_MAX_PATH];
					GetModuleFileName(NULL, curDir, _MAX_PATH);
					int i =(int)_tcslen(curDir) - 1;
					while( i>=0 && (curDir[i] != _T('\\') && curDir[i] != _T('/')))
						i--;
					curDir[i+1] = 0;
					_tcscat(curDir, _T("maxzip.exe"));
					HANDLE findhandle;
					WIN32_FIND_DATA file;
					findhandle = FindFirstFile(curDir,&file);
					FindClose(findhandle);
					TCHAR exec[MAX_PATH];
					if (findhandle != INVALID_HANDLE_VALUE)
					{
						// <maxzip path> <zip filename> @LIST$PCK.TXT
						_stprintf(exec,_T("\"\"%s\" \"%s\" @LIST$PCK.TXT\""),curDir,winZipName);

					}
					else
					{
						TCHAR tpath[MAX_PATH];
						TCHAR tfile[MAX_PATH];
						TCHAR textn[MAX_PATH];
						BMMSplitFilename(theCollection.WZPath, tpath, tfile, textn);

						// <winzip path> -a <zip filename> @LIST$PCK.TXT
						_stprintf(exec,_T("\"\"%s%s\" -a \"%s\" @LIST$PCK.TXT\""),tpath,tfile,winZipName);
					 }
					_flushall();
					_tchdir(to_path);
					_tsystem(exec);

					DeleteFile(zpn);
				}

				UI_MAKEFREE
				ip->RedrawViews(ip->GetTime());
				} //mloop
				break;
			}
			break;

		default:
			return FALSE;
	}
	return TRUE;
}	

Collection::Collection()
{
	iu = NULL;
	ip = NULL;	
	hPanel = NULL;
}

Collection::~Collection()
{
	
}
static BOOL CheckMaxZip()
{
	//Check maxzip.exe
	TCHAR curDir[_MAX_PATH];
	GetModuleFileName(NULL, curDir, _MAX_PATH);
	int i =(int)_tcslen(curDir) - 1;
	while( i>=0 && (curDir[i] != _T('\\') && curDir[i] != _T('/')))
		i--;
	curDir[i+1] = 0;
	_tcscat(curDir, _T("maxzip.exe"));
	HANDLE findhandle;
	WIN32_FIND_DATA file;
	findhandle = FindFirstFile(curDir,&file);
	FindClose(findhandle);
	if (findhandle == INVALID_HANDLE_VALUE)
		return FALSE;
	else
		return TRUE;
}

static BOOL CheckWinZip()
{
	HKEY	hKey;
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\classes\\WinZip"), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
		return FALSE;
	else
		return TRUE;
}
static void FindWinZipDir(TCHAR *Path)
{
	HKEY	hKey;
	long	kSize;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\classes\\WinZip\\shell\\open\\command"), 0, KEY_READ, &hKey);
	RegQueryValue(hKey,NULL,Path,&kSize);
}

void Collection::BeginEditParams(Interface *ip,IUtil *iu) 
{
	this->iu = iu;
	this->ip = ip;
	hPanel = ip->AddRollupPage(
		hInstance,
		MAKEINTRESOURCE(IDD_CL_PANEL),
		CollectionDlgProc,
		GetString(IDS_PARAMETERS),
		0);

	mapPaths.ZeroCount();

	// Do a simple registry check for WinZip
	WZisHere = CheckMaxZip();
	if(WZisHere == FALSE){
		if( (WZisHere = CheckWinZip()) == TRUE )
			FindWinZipDir(WZPath);
	}
	EnableWindow(GetDlgItem(hPanel,IDC_PACKUP),WZisHere);
	EnableWindow(GetDlgItem(hPanel,IDC_STATICPACK),WZisHere);
}
	
void Collection::EndEditParams(Interface *ip,IUtil *iu) 
{
	this->iu = NULL;
	ip->DeleteRollupPage(hPanel);
	hPanel = NULL;
}

void Collection::Init(HWND hWnd)
{
	// Set our defaults
	SetCheckBox(hWnd,IDC_UPMAP,FALSE);
	SetCheckBox(hWnd,IDC_PACKMAX,FALSE);
	SetCheckBox(hWnd,IDC_PACKUP,FALSE);
	SetCheckBox(hWnd,IDC_TYPE2,FALSE);

	SetCheckBox(hWnd,IDC_PACKMAPS,TRUE);
	SetCheckBox(hWnd,IDC_TYPE,TRUE);

	SetWindowText(GetDlgItem(hWnd,IDC_PATH),ip->GetDir(APP_ARCHIVES_DIR));
}

void Collection::Destroy(HWND hWnd)
{

}
