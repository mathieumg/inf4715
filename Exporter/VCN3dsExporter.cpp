///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Exporter implementation
///

#include "VCN3dsExporter.h"
#include "VCNExportTools.h"
#include "VCNNodes\TimerComponent.h"
#include "VCNINodeWrapper.h"
#include <notify.h>
#include "VCN3dsExporterBridge\VCN3dsExporterBridge.h"
#include "VCN3dsExporterProgressbarNotifier.h"
#include "IPathConfigMgr.h"
#include <unordered_set>

#define iniFilename "C:\\Temp\\3dsMaxPlugins\\VCNExporter.ini"


// Lets define ourselves a few shortcuts
#define XMLDocPtr MSXML2::IXMLDOMDocument2Ptr
#define XMLNodePtr MSXML2::IXMLDOMNodePtr
#define XMLElementPtr MSXML2::IXMLDOMElementPtr
#define XMLAttributePtr MSXML2::IXMLDOMAttributePtr
#define XMLNodeMapPtr MSXML2::IXMLDOMNamedNodeMapPtr
#define XMLNodeListPtr MSXML2::IXMLDOMNodeListPtr
#define XMLProcessingInstructionPtr MSXML2::IXMLDOMProcessingInstructionPtr
#define XMLTextPtr MSXML2::IXMLDOMTextPtr

HWND      GLOBAL_WINDOW_HANDLE = NULL;




class VCN3dsExporter : public UtilityObj 
{
public:

  HWND			hGlobalOptionsDlg;
  HWND			hModelDlg;
  HWND			hMeshDlg;
  HWND			hAnimDlg;
  HWND			hLightDlg;
  HWND			hCameraDlg;
  HWND			hMaterialDlg;
  HWND			hAboutDlg;
  HWND			hExportDlg;
  IUtil			*iu;
  Interface		*ip;
  HWND			hPropertyEditorWindowHandle;

  int CoordSys;
  bool bExportFaces;
  bool bExportVertPos;
  bool bExportVertNormals;
  bool bExportVertexColor;
  bool bExportUVs;
  bool bRemoveDoubles;
  bool bSampleAnimation;
  bool bExportPosAnim;
  bool bExportRotAnim;
  bool bExportSclAnim;
  int iSamplingFrameRate;
  bool bObjectSpace;
  bool bEditorEnabled;

  bool mIsInit;

  std::wstring mExporterGameDataFolderPath;

  VCN3dsExporter();
  ~VCN3dsExporter();		

  void BeginEditParams(Interface *ip,IUtil *iu);
  void EndEditParams(Interface *ip,IUtil *iu);

  void Init(HWND hWnd);
  void Destroy(HWND hWnd);

  void DeleteThis() { }		
  //Constructor/Destructor

  //My functions

  static void ClosePropertyEditor();
  static void PropertyEditorModificationReceived(const VCNINodeWrapperData& newData);
  static void ActivateStateChanged(bool activated);

  void UpdateUI();

  static void ExportScene(bool exportNodes = true, bool exportMeshes = true);
  static void ExportNodesOnly();
  static void ExportAllNodes(const MSTR& filename, const MSTR& initialDir, const MSTR& gameDataFolder);
  static void ExportAllMeshes(const MSTR& filename, const MSTR& initialDir);


  static IGameScene* Init3DXI(Interface* ip);
  static bool InitXMLDoc(XMLDocPtr &spDocOutput, XMLElementPtr &spElemRoot, _bstr_t strRootName);
  static bool FinishXMLDoc(XMLDocPtr &spDocOutput, MSTR filename);
  static void SetNameAndVersion(XMLElementPtr &spElemCurrentNode, const TCHAR* name);
  static int ExportNodesRecursive(IGameNode* Node, XMLDocPtr XMLDoc, XMLElementPtr ParentXMLElement, MSTR dir, MSTR gameDataFolder, VCN3dsExporterProgressbarNotifier& notifier, std::unordered_set<std::wstring>& nameCollection);
  static int ExportMeshesRecursive(IGameNode* Node, XMLDocPtr spXMLDoc, XMLElementPtr ParentXMLElement, VCN3dsExporterProgressbarNotifier& notifier);
  static bool GetMeshInfo(IGameObject* Obj, vector<Point3>& aFaces, vector<Point3>& aVertPos, vector<Point3>& aVertNormals, vector<Point3>& aVertColors, vector<Point2>& aVertUVs);
  static int FillVertexInfoInArrays(IGameMesh* Mesh, FaceEx* Face, int CornerIndex, vector<Point3>& aVertPos, vector<Point3>& aVertNormals, vector<Point2>& aVertUVs, vector<Point3>& aVertColors, vector<VCNExportVertex>& aVertices);
  static bool ExportXformsKFRecursively(IGameNode* Node, XMLDocPtr spXMLDoc, XMLElementPtr ParentXMLElement);
  static bool ExportLightsRecursive(IGameNode* Node, XMLDocPtr spXMLDoc, XMLElementPtr ParentXMLElement);
  static bool ExportCamerasRecursive(IGameNode* Node, XMLDocPtr spXMLDoc, XMLElementPtr ParentXMLElement);
  static bool CreateModelMaterialFile(IGameNode* Node, MSTR dir);
};

static VCN3dsExporter ExporterInstance;


class VCN3dsExporterClassDesc : public ClassDesc2 
{
public:
  int 			IsPublic() { return TRUE; }
  void *			Create(BOOL loading = FALSE) { return &ExporterInstance; }
  const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
  SClass_ID		SuperClassID() { return UTILITY_CLASS_ID; }
  Class_ID		ClassID() { return VCN3dsExporter_CLASS_ID; }
  const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

  const TCHAR*	InternalName() { return _T("VCN3dsExporter"); }	// returns fixed parsable name (scripter-visible name)
  HINSTANCE		HInstance() { return hInstance; }					// returns owning module handle

};


ClassDesc2* GetVCN3dsExporterDesc() 
{ 
  static VCN3dsExporterClassDesc VCN3dsExporterDesc;
  return &VCN3dsExporterDesc; 
}

void GetSelectedNodes(Tab<INode *> &nodeTab)
{
	nodeTab.SetCount(GetCOREInterface()->GetSelNodeCount());
	for(int i=0; i<GetCOREInterface()->GetSelNodeCount(); ++i)
	{
		INode *node = GetCOREInterface()->GetSelNode(i);
		nodeTab[i] = node;
	}
}



int GetNodeNodeCount(INode* node)
{
	int c = node->NumberOfChildren();
	int n = c;
	for (int i=0; i<c; i++)
	{
		INode* cNode = node->GetChildNode(i);
		if (cNode == NULL)
		{
			n += 1;
		}
		else
		{
			n += GetNodeNodeCount(cNode);
		}
	}
	return n;
}

int GetSceneNodeCount()
{
	INode* rootNode = ::GetCOREInterface()->GetRootNode();
	return GetNodeNodeCount(rootNode);
}


static INT_PTR CALLBACK GlobalOptionsDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

  switch (msg) {
  case WM_INITDIALOG:
    ExporterInstance.Init(hWnd);
    CheckRadioButton(hWnd, IDC_FILE_FORMAT_XML, IDC_FILE_FORMAT_BIN, IDC_FILE_FORMAT_XML);
    CheckRadioButton(hWnd, IDC_COORD_SYS_OGL, IDC_COORD_SYS_MAX, IDC_COORD_SYS_D3D);
    break;

  case WM_DESTROY:
    ExporterInstance.Destroy(hWnd);
    break;

  case WM_COMMAND:
    switch(LOWORD(wParam)) {
    case IDC_COORD_SYS_MAX : 	
      ExporterInstance.CoordSys = IGameConversionManager::IGAME_MAX;
      break;

    case IDC_COORD_SYS_D3D : 	
      ExporterInstance.CoordSys = IGameConversionManager::IGAME_D3D;
      break;

    case IDC_COORD_SYS_OGL : 	
      ExporterInstance.CoordSys = IGameConversionManager::IGAME_OGL;
      break;
    }
    break;


  case WM_LBUTTONDOWN:
  case WM_LBUTTONUP:
  case WM_MOUSEMOVE:
    ExporterInstance.ip->RollupMouseMessage(hWnd,msg,wParam,lParam); 
    break;

  default:
    return FALSE;
  }
  return TRUE;
}

static INT_PTR CALLBACK ModelDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

  switch (msg) {
  case WM_INITDIALOG:
    ExporterInstance.Init(hWnd);
    break;

  case WM_DESTROY:
    ExporterInstance.Destroy(hWnd);
    break;

  case WM_COMMAND:
    switch(LOWORD(wParam)) {
    case IDC_BTN_EXPORT_ALL_MODELS: 	
      ExporterInstance.ExportScene();
      break;
    }
    break;


  case WM_LBUTTONDOWN:
  case WM_LBUTTONUP:
  case WM_MOUSEMOVE:
    ExporterInstance.ip->RollupMouseMessage(hWnd,msg,wParam,lParam); 
    break;

  default:
    return FALSE;
  }
  return TRUE;
}

static INT_PTR CALLBACK MeshDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg) {
  case WM_INITDIALOG:
    ExporterInstance.Init(hWnd);
    CheckDlgButton(hWnd, IDC_CHECK_MESH_FACES, ExporterInstance.bExportFaces);
    CheckDlgButton(hWnd, IDC_CHECK_VERTEX_POS, ExporterInstance.bExportVertPos);
    CheckRadioButton(hWnd, IDC_RADIO_WORLD_SPACE, IDC_RADIO_OBJECT_SPACE, IDC_RADIO_OBJECT_SPACE);
    CheckDlgButton(hWnd, IDC_CHECK_VERTEX_NORMALS, ExporterInstance.bExportVertNormals);
    CheckDlgButton(hWnd, IDC_CHECK_VERTEX_COLORS, ExporterInstance.bExportVertexColor);
    CheckDlgButton(hWnd, IDC_CHECK_TEXT_COORD, ExporterInstance.bExportUVs);
    CheckDlgButton(hWnd, IDC_CHECK_REMOVE_DOUBLES, ExporterInstance.bRemoveDoubles);
    break;

  case WM_DESTROY:
    ExporterInstance.Destroy(hWnd);
    break;

  case WM_COMMAND:
    switch(LOWORD(wParam)) {
    case IDC_CHECK_MESH_FACES :
      ExporterInstance.bExportFaces = IsDlgButtonChecked(hWnd, IDC_CHECK_MESH_FACES) ? true : false;
      break;

    case IDC_CHECK_VERTEX_POS :
      ExporterInstance.bExportVertPos = IsDlgButtonChecked(hWnd, IDC_CHECK_VERTEX_POS) ? true : false;
      break;

    case IDC_RADIO_OBJECT_SPACE :
      ExporterInstance.bObjectSpace = true;
      break;

    case IDC_RADIO_WORLD_SPACE :
      ExporterInstance.bObjectSpace = false;
      break;

    case IDC_CHECK_VERTEX_NORMALS :
      ExporterInstance.bExportVertNormals = IsDlgButtonChecked(hWnd, IDC_CHECK_VERTEX_NORMALS) ? true : false;
      break;

    case IDC_CHECK_VERTEX_COLORS :
      ExporterInstance.bExportVertexColor = IsDlgButtonChecked(hWnd, IDC_CHECK_VERTEX_COLORS) ? true : false;
      break;

    case IDC_CHECK_TEXT_COORD :
      ExporterInstance.bExportUVs = IsDlgButtonChecked(hWnd, IDC_CHECK_TEXT_COORD) ? true : false;
      break;

    case IDC_CHECK_REMOVE_DOUBLES :
      ExporterInstance.bRemoveDoubles = IsDlgButtonChecked(hWnd, IDC_CHECK_REMOVE_DOUBLES) ? true : false;
      break;

    case IDC_BTN_EXPORT_MESH_RECURSIVE :
//       MSTR filename;
//       MSTR initialDir;
//       FilterList filterList;
//       filterList.Append(_T("Vicuna mesh files(*.MSH.xml)"));
//       filterList.Append(_T("*.MSH.xml"));
//       if(((Interface10*)ExporterInstance.ip)->DoMaxSaveAsDialog(ExporterInstance.ip->GetMAXHWnd(), 
//         _T("Save Mesh file as..."), filename, initialDir, filterList ))
//       {
//         //Init IGame
//         IGameScene * pIgame = VCN3dsExporter::Init3DXI(ExporterInstance.ip);					
// 
//         //Init XMLDoc with MSXML COM smart pointers
//         XMLDocPtr spDocOutput;
//         XMLElementPtr spElemRoot;
//         VCN3dsExporter::InitXMLDoc(spDocOutput, spElemRoot, "VCNMeshes");
// 
//         //Get MeshInfo in the xml document
//         for(int loop = 0; loop < pIgame->GetTopLevelNodeCount(); loop++) 
//         { 
//           IGameNode * pGameNode = pIgame->GetTopLevelNode(loop);
//           VCN3dsExporter::ExportMeshesRecursive(pGameNode, spDocOutput, spElemRoot);
//         }
// 
//         // Reformat filename if we find .MDL.xml.MDL.xml cause it was unintended
//         if (_tcsstr(filename, MSTR(_T(".MSH.xml.MSH.xml"))))
//         {
//           filename.remove(filename.Length() - 8);
//         }
// 
//         // Release 3DXI and Finish XML document					
//         VCN3dsExporter::FinishXMLDoc(spDocOutput, filename);
//         pIgame->ReleaseIGame();
//       }
      break;
    }

    break;


  case WM_LBUTTONDOWN:
  case WM_LBUTTONUP:
  case WM_MOUSEMOVE:
    ExporterInstance.ip->RollupMouseMessage(hWnd,msg,wParam,lParam); 
    break;

  default:
    return FALSE;
  }
  return TRUE;
}

static INT_PTR CALLBACK AnimDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  ISpinnerControl * spin;

  switch (msg) {
  case WM_INITDIALOG:
    ExporterInstance.Init(hWnd);
    CheckDlgButton(hWnd, IDC_CHECK_EXPORT_POS_ANIM, ExporterInstance.bExportPosAnim);
    CheckDlgButton(hWnd, IDC_CHECK_EXPORT_ROT_ANIM, ExporterInstance.bExportRotAnim);
    CheckDlgButton(hWnd, IDC_CHECK_EXPORT_SCL_ANIM, ExporterInstance.bExportSclAnim);
    CheckRadioButton(hWnd, IDC_RADIO_KF_CURRENT, IDC_RADIO_KF_SAMPLE, IDC_RADIO_KF_CURRENT+((ExporterInstance.bSampleAnimation)?1:0));
    spin = GetISpinner(GetDlgItem(hWnd, IDC_SPIN_ANIM_SAMPLES)); 
    spin->LinkToEdit(GetDlgItem(hWnd, IDC_EDIT_ANIM_SAMPLES), EDITTYPE_POS_INT); 
    spin->SetLimits(1, 100, false); 
    spin->SetScale(1.0f);
    spin->SetValue(ExporterInstance.iSamplingFrameRate, false);
    ReleaseISpinner(spin);
    EnableWindow(GetDlgItem(hWnd, IDC_SPIN_ANIM_SAMPLES), ExporterInstance.bSampleAnimation);
    EnableWindow(GetDlgItem(hWnd, IDC_EDIT_ANIM_SAMPLES), ExporterInstance.bSampleAnimation);
    break;

  case WM_DESTROY:
    ExporterInstance.Destroy(hWnd);
    break;

  case CC_SPINNER_CHANGE:
    switch(LOWORD(wParam)) {
    case IDC_SPIN_ANIM_SAMPLES :
      spin = GetISpinner(GetDlgItem(hWnd, IDC_SPIN_ANIM_SAMPLES)); 		
      ExporterInstance.iSamplingFrameRate = spin->GetIVal();
      ReleaseISpinner(spin);
      break;
    }

  case WM_COMMAND:
    switch(LOWORD(wParam)) {
    case IDC_RADIO_KF_CURRENT :
      ExporterInstance.bSampleAnimation = false;
      EnableWindow(GetDlgItem(hWnd, IDC_SPIN_ANIM_SAMPLES), ExporterInstance.bSampleAnimation);
      EnableWindow(GetDlgItem(hWnd, IDC_EDIT_ANIM_SAMPLES), ExporterInstance.bSampleAnimation);
      break;

    case IDC_RADIO_KF_SAMPLE :
      ExporterInstance.bSampleAnimation = true;
      EnableWindow(GetDlgItem(hWnd, IDC_SPIN_ANIM_SAMPLES), ExporterInstance.bSampleAnimation);
      EnableWindow(GetDlgItem(hWnd, IDC_EDIT_ANIM_SAMPLES), ExporterInstance.bSampleAnimation);
      break;

    case IDC_EDIT_ANIM_SAMPLES :
      spin = GetISpinner(GetDlgItem(hWnd, IDC_SPIN_ANIM_SAMPLES)); 		
      ExporterInstance.iSamplingFrameRate = spin->GetIVal();
      ReleaseISpinner(spin);
      break;

    case IDC_CHECK_EXPORT_POS_ANIM :
      ExporterInstance.bExportPosAnim = IsDlgButtonChecked(hWnd, IDC_CHECK_EXPORT_POS_ANIM) ? true : false;
      break;

    case IDC_CHECK_EXPORT_ROT_ANIM :
      ExporterInstance.bExportRotAnim = IsDlgButtonChecked(hWnd, IDC_CHECK_EXPORT_ROT_ANIM) ? true : false;
      break;

    case IDC_CHECK_EXPORT_SCL_ANIM :
      ExporterInstance.bExportSclAnim = IsDlgButtonChecked(hWnd, IDC_CHECK_EXPORT_SCL_ANIM) ? true : false;
      break;

    case IDC_BTN_EXPORT_ANIMATION :
      {
        MSTR filename;
        MSTR initialDir;
        FilterList filterList;
        filterList.Append(_T("VCNMSH files(*.ANM.xml)"));
        filterList.Append(_T("*.ANM.xml"));
        if(((Interface10*)ExporterInstance.ip)->DoMaxSaveAsDialog(ExporterInstance.ip->GetMAXHWnd(), 
          _T("Save Animation file as..."), filename, initialDir, filterList ))
        {
          //Init IGame
          IGameScene * pIgame = VCN3dsExporter::Init3DXI(ExporterInstance.ip);					

          //Init XMLDoc with MSXML COM smart pointers
          XMLDocPtr spDocOutput;
          XMLElementPtr spElemRoot;
          VCN3dsExporter::InitXMLDoc(spDocOutput, spElemRoot, "VCNAnims");

          //Create an XML node for the animation
          XMLElementPtr spElemAnim;
          spDocOutput->createElement(L"VCNAnim", &spElemAnim);

          VCN3dsExporter::SetNameAndVersion(spElemAnim, pIgame->GetSceneFileName());

          //Get AnimationInfo in the xml document
          VCN3dsExporter::ExportXformsKFRecursively(pIgame->GetTopLevelNode(0), spDocOutput, spElemAnim);

          //Attach this node to its parent
          spElemRoot->appendChild(spElemAnim, NULL);

          // Reformat filename if we find .MDL.xml.MDL.xml cause it was unindented
          if (_tcsstr(filename, MSTR(_T(".ANM.xml.ANM.xml"))))
          {
            filename.remove(filename.Length() - 8);
          }

          //Release 3DXI and Finish XML document					
          VCN3dsExporter::FinishXMLDoc(spDocOutput, filename);
          pIgame->ReleaseIGame();
        }
      }
      break;
    }

    break;


  case WM_LBUTTONDOWN:
  case WM_LBUTTONUP:
  case WM_MOUSEMOVE:
    ExporterInstance.ip->RollupMouseMessage(hWnd,msg,wParam,lParam); 
    break;

  default:
    return FALSE;
  }
  return TRUE;
}

static INT_PTR CALLBACK LightDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg) {
  case WM_INITDIALOG:
    ExporterInstance.Init(hWnd);
    break;

  case WM_DESTROY:
    ExporterInstance.Destroy(hWnd);
    break;

  case WM_COMMAND:
    switch(LOWORD(wParam)) {
    case IDC_BTN_EXPORT_SELECTED_LIGHTS :
      {
        MSTR filename;
        MSTR initialDir;
        FilterList filterList;
        filterList.Append(_T("VCNMSH files(*.LIGHT.xml)"));
        filterList.Append(_T("*.LIGHT.xml"));
        if(((Interface10*)ExporterInstance.ip)->DoMaxSaveAsDialog(ExporterInstance.ip->GetMAXHWnd(), 
          _T("Save Light file as..."), filename, initialDir, filterList ))
        {
          //Init IGame
          IGameScene * pIgame = VCN3dsExporter::Init3DXI(ExporterInstance.ip);					

          //Init XMLDoc with MSXML COM smart pointers
          XMLDocPtr spDocOutput;
          XMLElementPtr spElemRoot;
          VCN3dsExporter::InitXMLDoc(spDocOutput, spElemRoot, "VCNLights");

          //Get Lights info in the xml document
          for(int loop = 0; loop < pIgame->GetTopLevelNodeCount(); loop++) 
          { 
            IGameNode * pGameNode = pIgame->GetTopLevelNode(loop);
            VCN3dsExporter::ExportLightsRecursive(pGameNode, spDocOutput, spElemRoot);
          }

          // Reformat filename if we find .MDL.xml.MDL.xml cause it was unindented
          if (_tcsstr(filename, MSTR(_T(".LIGHT.xml.LIGHT.xml"))))
          {
            filename.remove(filename.Length() - 8);
          }

          //Release 3DXI and Finish XML document					
          VCN3dsExporter::FinishXMLDoc(spDocOutput, filename);
          pIgame->ReleaseIGame();
        }
      }
      break;
    }

    break;


  case WM_LBUTTONDOWN:
  case WM_LBUTTONUP:
  case WM_MOUSEMOVE:
    ExporterInstance.ip->RollupMouseMessage(hWnd,msg,wParam,lParam); 
    break;

  default:
    return FALSE;
  }
  return TRUE;
}

static INT_PTR CALLBACK CameraDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg) {
  case WM_INITDIALOG:
    ExporterInstance.Init(hWnd);
    break;

  case WM_DESTROY:
    ExporterInstance.Destroy(hWnd);
    break;

  case WM_COMMAND:
    switch(LOWORD(wParam)) {
    case IDC_BTN_EXPORT_SCENES_CAMERAS :
      {
        MSTR filename;
        MSTR initialDir;
        FilterList filterList;
        filterList.Append(_T("VCNMSH files(*.CAM.xml)"));
        filterList.Append(_T("*.CAM.xml"));
        if(((Interface10*)ExporterInstance.ip)->DoMaxSaveAsDialog(ExporterInstance.ip->GetMAXHWnd(), 
          _T("Save Camera file as..."), filename, initialDir, filterList ))
        {
          //Init IGame
          IGameScene * pIgame = VCN3dsExporter::Init3DXI(ExporterInstance.ip);					

          //Init XMLDoc with MSXML COM smart pointers
          XMLDocPtr spDocOutput;
          XMLElementPtr spElemRoot;
          VCN3dsExporter::InitXMLDoc(spDocOutput, spElemRoot, "VCNCameras");

          //Get Cameras info in the xml document
          for(int loop = 0; loop < pIgame->GetTopLevelNodeCount(); loop++) 
          { 
            IGameNode * pGameNode = pIgame->GetTopLevelNode(loop);
            VCN3dsExporter::ExportCamerasRecursive(pGameNode, spDocOutput, spElemRoot);
          }

          // Reformat filename if we find .MDL.xml.MDL.xml cause it was unindented
          if (_tcsstr(filename, MSTR(_T(".CAM.xml.CAM.xml"))))
          {
            filename.remove(filename.Length() - 8);
          }

          //Release 3DXI and Finish XML document					
          VCN3dsExporter::FinishXMLDoc(spDocOutput, filename);
          pIgame->ReleaseIGame();
        }
      }
      break;
    }

    break;


  case WM_LBUTTONDOWN:
  case WM_LBUTTONUP:
  case WM_MOUSEMOVE:
    ExporterInstance.ip->RollupMouseMessage(hWnd,msg,wParam,lParam); 
    break;

  default:
    return FALSE;
  }
  return TRUE;
}

static INT_PTR CALLBACK MaterialDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg) {
  case WM_INITDIALOG:
    ExporterInstance.Init(hWnd);
    break;

  case WM_DESTROY:
    ExporterInstance.Destroy(hWnd);
    break;

  case WM_COMMAND:
    switch(LOWORD(wParam)) {
    case IDC_BTN_EXPORT_ANIMATION :

      break;
    }

    break;


  case WM_LBUTTONDOWN:
  case WM_LBUTTONUP:
  case WM_MOUSEMOVE:
    ExporterInstance.ip->RollupMouseMessage(hWnd,msg,wParam,lParam); 
    break;

  default:
    return FALSE;
  }
  return TRUE;
}

static void UpdateGameDataFolderPath(const std::wstring& path)
{
	const WCHAR* test = path.c_str(); 
	SetDlgItemText(ExporterInstance.hExportDlg, IDC_EDIT_GAME_DATA_FOLDER, path.c_str());
}

static void BrowseAndSetGameDataFolderPath()
{
	MSTR buf;
	if (GetCOREInterface9()->DoMaxBrowseForFolder(GetCOREInterface()->GetMAXHWnd(), L"", buf))
	{
		ExporterInstance.mExporterGameDataFolderPath = std::wstring(buf.ToBSTR());
		UpdateGameDataFolderPath(ExporterInstance.mExporterGameDataFolderPath);
	}
}

static void SaveIni()
{
	WritePrivateProfileStringW(_T("EXPORTER_PLUGIN_SETTINGS"), _T("EDITOR_GAME_DATA_FOLDER_PATH"), ExporterInstance.mExporterGameDataFolderPath.c_str(), _T(iniFilename));
	
}

static void LoadFromIni()
{
	WCHAR buf[4096];
	GetPrivateProfileStringW(_T("EXPORTER_PLUGIN_SETTINGS"), _T("EDITOR_GAME_DATA_FOLDER_PATH"), _T(""), buf, 4096,  _T(iniFilename));
	ExporterInstance.mExporterGameDataFolderPath = std::wstring(buf);
	UpdateGameDataFolderPath(ExporterInstance.mExporterGameDataFolderPath);
}


struct ParamStruct{ INodeTab* origNodes; INodeTab* clonedNodes; CloneType cloneType;};
void NodeClonedCallback(void *param, NotifyInfo *info)
{
	
	ParamStruct* params = (ParamStruct*) info->callParam;

	INodeTab* origNodes = params->origNodes;
	INodeTab* clonedNodes = params->clonedNodes;
	CloneType cloneType = params->cloneType;

	if (cloneType == NODE_COPY || cloneType == NODE_INSTANCE || cloneType == NODE_REFERENCE)
	{
		for (int i=0; i<origNodes->Count(); i++)
		{
			INode* oNode = (*origNodes)[i];
			INode* cNode = (*clonedNodes)[i];

			if (oNode)
			{
				VCNINodeWrapper oWrapper(*oNode);
				VCNINodeWrapper cWrapper(*cNode);

				cWrapper.SetData(oWrapper.GetData());
				cWrapper.SaveIfDirty();
			}
		}
	}
}

void SelectionsetChangedCallback(void *param, NotifyInfo *info)
{
	if (!ExporterInstance.mIsInit || !ExporterInstance.bEditorEnabled)
	{
		return;
	}

	Tab<INode*>nodeTab;
	GetSelectedNodes(nodeTab);
	try
	{
		// TODO: Handle editing multiple items
		if (nodeTab.Count() == 1)
		{
			VCNINodeWrapper wrapper(*nodeTab[0]);
			VCN3dsExporterBridge::VCN3dsExporterBridgeManager::UpdatePropertyEditor(wrapper.GetData());
			VCN3dsExporterBridge::VCN3dsExporterBridgeManager::SetValidSelectionState(true);
		}
		else
		{
			VCNINodeWrapperData emptyData;
			VCN3dsExporterBridge::VCN3dsExporterBridgeManager::UpdatePropertyEditor(emptyData);
			VCN3dsExporterBridge::VCN3dsExporterBridgeManager::SetValidSelectionState(false);
		}
	}
	catch (std::exception& e)
	{
		VCN3dsExporterBridge::VCN3dsExporterBridgeManager::SetValidSelectionState(false);
		VCN3dsExporterBridge::VCN3dsExporterBridgeManager::AddLogEntry("Could not load the node's custom data. Contact Mathieu ASAP!! ", VCN3dsExporterBridge::LogTypeCPP_FATAL);
	}
	
}

static INT_PTR CALLBACK ExportDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_INITDIALOG:
		ExporterInstance.Init(hWnd);
		CheckDlgButton(hWnd, IDC_CHECK_ENABLE_EDITOR_MODE, ExporterInstance.bEditorEnabled);

		break;

	case WM_DESTROY:
		ExporterInstance.Destroy(hWnd);
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam)) {
		case IDC_BUTTON_GAME_DATA_FOLDER_BROWSE:
			BrowseAndSetGameDataFolderPath();
			break;
		case IDC_BUTTON_VICUNA_EXPORT_SCENE:
			ExporterInstance.ExportScene();
			break;
		case IDC_BUTTON_EXPORT_NODES_COMPONENTS:
			ExporterInstance.ExportNodesOnly();
			break;
		case IDC_CHECK_ENABLE_EDITOR_MODE:
			ExporterInstance.bEditorEnabled = IsDlgButtonChecked(hWnd, IDC_CHECK_ENABLE_EDITOR_MODE) ? true : false;
			if (ExporterInstance.bEditorEnabled)
			{
				if (!ExporterInstance.mIsInit)
				{
					if (!VCN3dsExporterBridge::VCN3dsExporterBridgeManager::Init(::GetCOREInterface()->GetMAXHWnd()))
					{
						CheckDlgButton(hWnd, IDC_CHECK_ENABLE_EDITOR_MODE, false);
						VCN3dsExporterBridge::VCN3dsExporterBridgeManager::ShowErrorMessage(L"Could not open the plugin because it could not be init.");
						break; // Error while init. Skip
					}
				}
				ExporterInstance.hPropertyEditorWindowHandle = VCN3dsExporterBridge::VCN3dsExporterBridgeManager::OpenPropertyEditor(::GetCOREInterface()->GetMAXHWnd(), VCN3dsExporter::ClosePropertyEditor, VCN3dsExporter::PropertyEditorModificationReceived, VCN3dsExporter::ActivateStateChanged);
				// The is a bug with keyboard input when using this
				//::GetCOREInterface()->RegisterDlgWnd(ExporterInstance.hPropertyEditorWindowHandle);
				SelectionsetChangedCallback(nullptr, nullptr); // Update the property editor
			}
			else
			{
				VCN3dsExporterBridge::VCN3dsExporterBridgeManager::ClosePropertyEditor();
				//::GetCOREInterface()->UnRegisterDlgWnd(ExporterInstance.hPropertyEditorWindowHandle);
			}
			break;
		case IDC_BUTTON_RELOAD_DLL:
			if (!VCN3dsExporterBridge::VCN3dsExporterBridgeManager::ReloadDll())
			{
				VCN3dsExporterBridge::VCN3dsExporterBridgeManager::ShowErrorMessage(L"Reload of the DLLs failed");
			}
			break;
		}

		break;


	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MOUSEMOVE:
		ExporterInstance.ip->RollupMouseMessage(hWnd,msg,wParam,lParam); 
		break;

	default:
		return FALSE;
	}
	return TRUE;
}


static INT_PTR CALLBACK AboutDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg) {
  case WM_INITDIALOG:
    ExporterInstance.Init(hWnd);
    break;

  case WM_DESTROY:
    ExporterInstance.Destroy(hWnd);
    break;

  case WM_COMMAND:
    switch(LOWORD(wParam)) {
    case IDC_BTN_ABOUT :
      MessageBox(ExporterInstance.ip->GetMAXHWnd(), 
        _T("Vicuna 3ds Max Exporter v3.2\n")
        _T("Copyright (C) 2012 - All Rights Reserved\n")
        _T("All rights reserved. http://www.equals-forty-two.com"),  
        _T("About Vicuña Exporter"), MB_OK);
      break;
    }

    break;


  case WM_LBUTTONDOWN:
  case WM_LBUTTONUP:
  case WM_MOUSEMOVE:
    ExporterInstance.ip->RollupMouseMessage(hWnd,msg,wParam,lParam); 
    break;

  default:
    return FALSE;
  }
  return TRUE;
}


/* Return TRUE if file/directory 'fileName' exists */
bool FileExists(const TCHAR *fileName)
{
  DWORD       fileAttr;

  fileAttr = GetFileAttributes(fileName);
  if (INVALID_FILE_ATTRIBUTES == fileAttr)
    return false;
  return true;
}





//--- VCN3dsExporter -------------------------------------------------------
VCN3dsExporter::VCN3dsExporter()
{
  iu = NULL;
  ip = NULL;	
  hGlobalOptionsDlg = NULL;
  hModelDlg = NULL;
  hMeshDlg = NULL;
  hAnimDlg= NULL;
  hLightDlg = NULL;
  hCameraDlg = NULL;
  hMaterialDlg = NULL;
  hAboutDlg = NULL;
  hExportDlg = NULL;

  CoordSys = IGameConversionManager::IGAME_D3D;
  bExportFaces = true;
  bExportVertPos = true;
  bExportVertNormals = true;
  bExportVertexColor = false;
  bExportUVs = true;
  bRemoveDoubles = true;
  bSampleAnimation = false;
  bExportPosAnim = true;
  bExportRotAnim = true;
  bExportSclAnim = true;
  iSamplingFrameRate = 1;
  bObjectSpace = true;
  bEditorEnabled = false;
  mIsInit = false;


  

  // Bind callback for when the selectionset changes (selected nodes)
  RegisterNotification(SelectionsetChangedCallback, this, NOTIFY_SELECTIONSET_CHANGED);
  RegisterNotification(NodeClonedCallback, this, NOTIFY_POST_NODES_CLONED);

  
}

VCN3dsExporter::~VCN3dsExporter()
{
	SaveIni();

	// Unbind callback for selectionset
	UnRegisterNotification(SelectionsetChangedCallback, this, NOTIFY_SELECTIONSET_CHANGED);
	UnRegisterNotification(NodeClonedCallback, this, NOTIFY_POST_NODES_CLONED);

}

void VCN3dsExporter::BeginEditParams(Interface *ip,IUtil *iu) 
{
  LoadFromIni();

  this->iu = iu;
  this->ip = ip;

  if (!mIsInit)
  {
	  if (VCN3dsExporterBridge::VCN3dsExporterBridgeManager::Init(::GetCOREInterface()->GetMAXHWnd()))
	  {
		  mIsInit = true;
	  }
  }

  hGlobalOptionsDlg = ip->AddRollupPage(
    hInstance,
    MAKEINTRESOURCE(IDD_PANEL_GLOBAL_OPTIONS),
    GlobalOptionsDlgProc,
    GetString(IDS_GLOBAL_OPTIONS_PANEL),
    0);

  hExportDlg = ip->AddRollupPage(
	  hInstance,
	  MAKEINTRESOURCE(IDD_PANEL_EXPORT),
	  ExportDlgProc,
	  GetString(IDS_EXPORT_PANEL),
	  0);

  hModelDlg = ip->AddRollupPage(
    hInstance,
    MAKEINTRESOURCE(IDD_PANEL_MODEL),
    ModelDlgProc,
    GetString(IDS_PARAMS),
    0);

  hMeshDlg = ip->AddRollupPage(
    hInstance,
    MAKEINTRESOURCE(IDD_PANEL_MESH),
    MeshDlgProc,
    GetString(IDS_MESH_PANEL),
    0);

  hAnimDlg = ip->AddRollupPage(
    hInstance,
    MAKEINTRESOURCE(IDD_PANEL_ANIM),
    AnimDlgProc,
    GetString(IDS_ANIM_PANEL),
    0);
#ifdef NOT_DONE
  hLightDlg = ip->AddRollupPage(
    hInstance,
    MAKEINTRESOURCE(IDD_PANEL_LIGHT),
    LightDlgProc,
    GetString(IDS_LIGHT_PANEL),
    0);

  hCameraDlg = ip->AddRollupPage(
    hInstance,
    MAKEINTRESOURCE(IDD_PANEL_CAMERA),
    CameraDlgProc,
    GetString(IDS_CAMERA_PANEL),
    0);

  hMaterialDlg = ip->AddRollupPage(
    hInstance,
    MAKEINTRESOURCE(IDD_PANEL_MATERIALS),
    MaterialDlgProc,
    GetString(IDS_MATERIAL_PANEL),
    0);
#endif
  hAboutDlg = ip->AddRollupPage(
    hInstance,
    MAKEINTRESOURCE(IDD_PANEL_ABOUT),
    AboutDlgProc,
    GetString(IDS_ABOUT_PANEL),
    0);

  UpdateGameDataFolderPath(ExporterInstance.mExporterGameDataFolderPath);
}

void VCN3dsExporter::EndEditParams(Interface *ip,IUtil *iu) 
{
	SaveIni();

  this->iu = NULL;
  this->ip = NULL;

  ip->DeleteRollupPage(hGlobalOptionsDlg);
  ip->DeleteRollupPage(hModelDlg);
  ip->DeleteRollupPage(hMeshDlg);
  ip->DeleteRollupPage(hAnimDlg);
  ip->DeleteRollupPage(hLightDlg);
  ip->DeleteRollupPage(hCameraDlg);
  ip->DeleteRollupPage(hMaterialDlg);
  ip->DeleteRollupPage(hAboutDlg);
  ip->DeleteRollupPage(hExportDlg);

  hGlobalOptionsDlg = NULL;
  hModelDlg = NULL;
  hMeshDlg = NULL;
  hAnimDlg = NULL;
  hLightDlg = NULL;
  hCameraDlg = NULL;
  hMaterialDlg = NULL;
  hAboutDlg = NULL;
  hExportDlg = NULL;
}

void VCN3dsExporter::Init(HWND hWnd)
{

}

void VCN3dsExporter::Destroy(HWND hWnd)
{

}

IGameScene* VCN3dsExporter::Init3DXI(Interface* ip)
{
  // Grab the IGame Interface
  IGameScene* gameScene = GetIGameInterface(); 

  // Set the coordinate system
  IGameConversionManager* convertionManager = GetConversionManager(); 
  convertionManager->SetCoordSystem((IGameConversionManager::CoordSystem)ExporterInstance.CoordSys);

  // Initialize the scene
  bool exportSelected = false;
  gameScene->InitialiseIGame(exportSelected); // false : all scene, true : selected only 
  gameScene->SetStaticFrame(ip->GetTime()/GetTicksPerFrame()); 

  return gameScene;
}

bool VCN3dsExporter::InitXMLDoc(XMLDocPtr &spDocOutput, XMLElementPtr &spElemRoot, _bstr_t strRootName)
{
  // Local variables and initializations
  HRESULT hResult = S_OK;

  //  Initialize the COM library
  hResult = CoInitialize(NULL);
  if (FAILED(hResult) && hResult != S_FALSE)
  {
    //cerr << "Failed to initialize COM environment" << endl;
    return false;
  }

  //  Main try block for MSXML DOM operations
  try
  {
    XMLProcessingInstructionPtr spXMLDecl;

    //  Create the COM DOM Document object
    hResult = spDocOutput.CreateInstance(__uuidof(MSXML2::DOMDocument60));

    if FAILED(hResult)
    {
      //cerr << "Failed to create Document instance" << endl;
      return false;
    }

    //  Create the XML Declaration as a Processing Instruction and append it to the document node
    spDocOutput->createProcessingInstruction(L"xml",	L"version=\"1.0\" encoding=\"UTF-8\"", &spXMLDecl);
    spDocOutput->appendChild(spXMLDecl, 0);

    //Create the root element and append it to the Document
    spDocOutput->createElement(strRootName, &spElemRoot);
    //spElemRoot->setAttribute("version", "3.0");
    spDocOutput->appendChild(spElemRoot, 0);


  } // End of try block

  //  Catch COM exceptions
  catch (_com_error&/*e*/)
  {
    //cerr << "COM Error" << endl;
    //cerr << "Message = " << e.ErrorMessage() << endl;
    return false;
  }
  return true;
}

bool VCN3dsExporter::FinishXMLDoc(XMLDocPtr &spDocOutput, MSTR filename)
{
  // Local variables and initializations
  HRESULT hResult = S_OK;
  CComVariant name = filename;

  // Create the final document which will be indented properly
  XMLDocPtr pXMLFormattedDoc = spDocOutput;

  XMLDocPtr styleSheetXSL;
  hResult = styleSheetXSL.CreateInstance(__uuidof(MSXML2::DOMDocument60));
  if( SUCCEEDED(hResult) )
  {
    // We need to load the style sheet which will be used to indent the XMl properly.
    VARIANT_BOOL success;
    styleSheetXSL->load( variant_t(_T("StyleSheet.xsl")), &success );
    if( success == VARIANT_TRUE )
    {
      hResult = pXMLFormattedDoc.CreateInstance(__uuidof(MSXML2::DOMDocument60));

      CComPtr<IDispatch> pDispatch;
      hResult = pXMLFormattedDoc->QueryInterface(IID_IDispatch, (void**)&pDispatch);
      if ( SUCCEEDED(hResult) )
      {
        _variant_t vtOutObject;
        vtOutObject.vt = VT_DISPATCH;
        vtOutObject.pdispVal = pDispatch;
        vtOutObject.pdispVal->AddRef();
        // Apply the transformation to format the final document    
        hResult = spDocOutput->transformNodeToObject(styleSheetXSL, vtOutObject);
        if ( FAILED(hResult) )
        {
           pXMLFormattedDoc = spDocOutput;
        }
      }
    }
  }

  // By default it is writing the encoding = UTF-16. Let us change the encoding to UTF-8
  // A map of the a attributes (version, encoding) values (1.0, UTF-8) pair
  // <?xml version="1.0" encoding="UTF-8"?>
  XMLNodePtr pXMLFirstChild;
  XMLNodeMapPtr pXMLAttributeMap;
  XMLNodePtr pXMLEncodNode;
  pXMLFormattedDoc->get_firstChild( &pXMLFirstChild );
  pXMLFirstChild->get_attributes( &pXMLAttributeMap );
  pXMLAttributeMap->getNamedItem(L"encoding", &pXMLEncodNode);
  pXMLEncodNode->put_nodeValue(_variant_t(L"UTF-8"));    //encoding = UTF-8

  //  Save the output XML Document
  hResult = pXMLFormattedDoc->save(name);
  if ( FAILED(hResult) )
  {
    // cerr << "Failed to save document" << endl;
    return false;
  }

  //  Release COM resources
  CoUninitialize();

  return true;
}


void VCN3dsExporter::SetNameAndVersion(XMLElementPtr &spElemCurrentNode, const TCHAR* name)
{
  spElemCurrentNode->setAttribute(L"Name", CComVariant(name));
  spElemCurrentNode->setAttribute(L"version", CComVariant(L"3.0"));
}

bool VCN3dsExporter::CreateModelMaterialFile(IGameNode* Node, MSTR dir)
{
	std::wstring nodeNameWStr(Node->GetName());
  const MSTR matName = MSTR(Node->GetName()) + "Material";
  const MSTR matDir = dir + "/Materials/AutoGenerated/";
  const MSTR newMatFilename = dir + "/Materials/AutoGenerated/" + Node->GetName() + ".MAT.xml";

  if (!FileExists(matDir))
    return false;

  // Always overwrite file
//   if (FileExists(newMatFilename))
//     return false;

  Point3 nodeAmbiant(1.0f, 0.0f, 1.0f); // Default value is pink (in case that there is no material)
  Point3 nodeDiffuse(1.0f, 0.0f, 1.0f);
  Point3 nodeSpecular(0.0f, 0.0f, 0.0f);
  float gloss = 30.0f;
  float opacity = 1.0f;
  bool useBumpMap = false;

  IGameMaterial* nodeMaterial = Node->GetNodeMaterial();
  std::wstring textureFilename = L"Textures/NoTexture.jpg";
  std::wstring bumpMapFilename = L"Textures/NoTexture.jpg";
  
  if (nodeMaterial)
  {
	  IGameProperty* ambiantProp = nodeMaterial->GetAmbientData();
	  if (ambiantProp)
	  {
		  ambiantProp->GetPropertyValue(nodeAmbiant);
	  }
	  IGameProperty* diffuseProp = nodeMaterial->GetDiffuseData();
	  if (diffuseProp)
	  {
		  diffuseProp->GetPropertyValue(nodeDiffuse);
	  }
	  IGameProperty* specularProp = nodeMaterial->GetSpecularData();
	  if (specularProp)
	  {
		  // Do not use the specular data yet. Some problems between Vicuna and 3ds Max
		  //specularProp->GetPropertyValue(nodeSpecular);
	  }
	  IGameProperty* glossProp = nodeMaterial->GetGlossinessData();
	  if (glossProp)
	  {
		  // Do not use the specular data yet. Some problems between Vicuna and 3ds Max
		  //glossProp->GetPropertyValue(gloss);
	  }
	  IGameProperty* opacityProp = nodeMaterial->GetOpacityData();
	  if (opacityProp)
	  {
		  opacityProp->GetPropertyValue(opacity);
	  }

	  int numTextures = nodeMaterial->GetNumberOfTextureMaps();
	  for (int i=0; i<numTextures; i++)
	  {
		  IGameTextureMap* nodeTexture = nodeMaterial->GetIGameTextureMap(i);
		  if (nodeTexture->IsEntitySupported() && (nodeTexture->GetStdMapSlot() == ID_DI || nodeTexture->GetStdMapSlot() == ID_BU))
		  {
			  
			  const wchar_t* textureFilenameChar = nodeTexture->GetBitmapFileName();
			  if (textureFilenameChar == NULL)
			  {
				  continue;
			  }

			  MaxSDK::Util::Path& projectPath = IPathConfigMgr::GetPathConfigMgr()->GetCurrentProjectFolderPath();
			  projectPath.RemoveLeaf();
			  projectPath.Append(MaxSDK::Util::Path(textureFilenameChar));
			  std::wstring newTextureFilename = projectPath.GetString().ToWStr();
			  const wchar_t* testNewPathAbs = newTextureFilename.c_str();
			  int lastBackSlash = newTextureFilename.find_last_of(L'\\');

			  if (lastBackSlash != -1)
			  {
				  std::wstring destinationTectureFilePath = ExporterInstance.mExporterGameDataFolderPath + L"\\Textures\\";
				  std::wstring fileName = newTextureFilename.substr(lastBackSlash + 1);
				  destinationTectureFilePath.append(fileName);
				  BOOL success = CopyFile(newTextureFilename.c_str(), destinationTectureFilePath.c_str(), false);
				  if (success) // If the bitmap could be copied in the Game/Data/Textures folder
				  {
					  newTextureFilename = L"Textures/" + newTextureFilename.substr(lastBackSlash+1);
					  if (nodeTexture->GetStdMapSlot() == ID_DI)
					  {
						  textureFilename = newTextureFilename;
					  }
					  else if (nodeTexture->GetStdMapSlot() == ID_BU)
					  {
						  bumpMapFilename = newTextureFilename;
						  useBumpMap = true;
					  }
				  }
				  else
				  {
					  VCN3dsExporterBridge::VCN3dsExporterBridgeManager::AddLogEntry("Could not copy the material for node " + std::string(nodeNameWStr.begin(), nodeNameWStr.end()) + ". Material path is: " + std::string(newTextureFilename.begin(), newTextureFilename.end()), VCN3dsExporterBridge::LogTypeCPP_ERROR);
				  }
			  }
		  }
	  }
  }
  
  XMLDocPtr spDocOutput;
  XMLElementPtr spElemRoot;
  InitXMLDoc(spDocOutput, spElemRoot, "Material");	

  spElemRoot->setAttribute(L"Name", CComVariant(matName.data()));
  spElemRoot->setAttribute(L"Version", CComVariant(L"1"));

  XMLElementPtr ambientNode, diffuseNode, specularNode, useLightingNode, effectNode, textureParamNode, bumpMapParamNode, floatParamNode;
  spDocOutput->createElement(L"AmbientColor", &ambientNode);
  spDocOutput->createElement(L"DiffuseColor", &diffuseNode);
  spDocOutput->createElement(L"SpecularColor", &specularNode);
  spDocOutput->createElement(L"UseLighting", &useLightingNode);
  spDocOutput->createElement(L"Effect", &effectNode);
  spDocOutput->createElement(L"TextureParam", &textureParamNode);
  spDocOutput->createElement(L"TextureParam", &bumpMapParamNode);
  //spDocOutput->createElement(L"FloatParam", &floatParamNode);

  ambientNode->setAttribute(L"r", _variant_t(nodeAmbiant.x));
  ambientNode->setAttribute(L"g", _variant_t(nodeAmbiant.y));
  ambientNode->setAttribute(L"b", _variant_t(nodeAmbiant.z));
  ambientNode->setAttribute(L"a", _variant_t(opacity));

  diffuseNode->setAttribute(L"r", _variant_t(nodeDiffuse.x));
  diffuseNode->setAttribute(L"g", _variant_t(nodeDiffuse.y));
  diffuseNode->setAttribute(L"b", _variant_t(nodeDiffuse.z));
  diffuseNode->setAttribute(L"a", _variant_t(opacity));

  specularNode->setAttribute(L"r", _variant_t(nodeSpecular.x));
  specularNode->setAttribute(L"g", _variant_t(nodeSpecular.y));
  specularNode->setAttribute(L"b", _variant_t(nodeSpecular.z));
  specularNode->setAttribute(L"a", _variant_t(opacity));
  specularNode->setAttribute(L"power", _variant_t(gloss));

  useLightingNode->setAttribute(L"value", _variant_t(1));

  effectNode->setAttribute(L"name", _variant_t(L"LitTextured"));

  textureParamNode->setAttribute(L"name", _variant_t(L"DiffuseTexture"));
  textureParamNode->setAttribute(L"value", _variant_t(textureFilename.c_str()));

  if (useBumpMap)
  {
	  bumpMapParamNode->setAttribute(L"name", _variant_t(L"BumpMapTexture"));
	  bumpMapParamNode->setAttribute(L"value", _variant_t(bumpMapFilename.c_str()));
  }
  
  //floatParamNode->setAttribute(L"name", _variant_t(L"DiffuseTextureAmount"));
  //floatParamNode->setAttribute(L"value", _variant_t(1.0));

  spElemRoot->appendChild(ambientNode,0);
  spElemRoot->appendChild(diffuseNode,0);
  spElemRoot->appendChild(specularNode,0);

  spElemRoot->appendChild(useLightingNode,0);

  effectNode->appendChild(textureParamNode,0);
  if (useBumpMap)
  {
	  effectNode->appendChild(bumpMapParamNode,0);
  }
  //effectNode->appendChild(floatParamNode,0);
  spElemRoot->appendChild(effectNode,0);

  FinishXMLDoc(spDocOutput, newMatFilename);

  return true;
}

// 
// bool VCN3dsExporter::ExportModelBaseRecursive(IGameNode* Node, XMLDocPtr spXMLDoc, XMLElementPtr ParentXMLElement, MSTR dir)
// {
// 	VCNINodeWrapper wrapper(*Node->GetMaxNode());
// 
// 	XMLElementPtr spElemCurrentNode;
// 
// 	//Get the Instance Manager
// 	IInstanceMgr* InstanceMgr = IInstanceMgr::GetInstanceMgr();
// 	INodeTab NodeInstances;
// 
// 	spXMLDoc->createElement(L"VCNModel", &spElemCurrentNode);
// 	spElemCurrentNode->setAttribute(L"Name", CComVariant(Node->GetName()));
// 
// 	IGameObject * obj = Node->GetIGameObject(); 
// 	switch(obj->GetIGameType()) 
// 	{ 
// 	case IGameObject::IGAME_UNKNOWN: 
// 		spElemCurrentNode->setAttribute(L"Type", CComVariant("Unknown"));
// 		break;
// 
// 	case IGameObject::IGAME_LIGHT: 
// 		spElemCurrentNode->setAttribute(L"Type", CComVariant("Light"));
// 		break;
// 
// 	case IGameObject::IGAME_MESH: 
// 		{
// 			MSTR matName;
// 			spElemCurrentNode->setAttribute(L"Type", CComVariant("Render"));
// 
// 			//Get nodes instances
// 			InstanceMgr->GetInstances(*(Node->GetMaxNode()), NodeInstances);
// 
// 			matName = NodeInstances[0]->GetName();
// 			matName += _T("Material");
// 
// 			//Mesh exported will be the first instance of this node);
// 			spElemCurrentNode->setAttribute(L"Mesh", CComVariant(NodeInstances[0]->GetName()));
// 			spElemCurrentNode->setAttribute(L"Material", CComVariant(matName.data()));	
// 
// 			CreateModelMaterialFile(Node, dir);
// 		}
// 		break;
// 
// 	case IGameObject::IGAME_SPLINE: 
// 		spElemCurrentNode->setAttribute(L"Type", CComVariant("Spline"));
// 		break;
// 
// 	case IGameObject::IGAME_CAMERA: 
// 		spElemCurrentNode->setAttribute(L"Type", CComVariant("Camera"));
// 		break;
// 
// 	case IGameObject::IGAME_HELPER: 
// 		spElemCurrentNode->setAttribute(L"Type", CComVariant("Helper"));
// 		break;
// 
// 	case IGameObject::IGAME_BONE: 
// 		spElemCurrentNode->setAttribute(L"Type", CComVariant("Bone"));
// 		break;
// 
// 	case IGameObject::IGAME_IKCHAIN: 
// 		spElemCurrentNode->setAttribute(L"Type", CComVariant("IK_Chain"));
// 		break;
// 	}
// 
// 	//Set Default-Neutral Xforms as attributes
// 	GMatrix NodeMatrix;
// 	Point3 NodeTranslation;
// 	Quat NodeRotation;
// 	Point3 NodeScaling;
// 
// 	NodeMatrix = Node->GetLocalTM();
// 	NodeTranslation = NodeMatrix.Translation();
// 	NodeRotation = NodeMatrix.Rotation();
// 	NodeScaling = NodeMatrix.Scaling();
// 
// 	spElemCurrentNode->setAttribute(L"posx", CComVariant(NodeTranslation.x));
// 	spElemCurrentNode->setAttribute(L"posy", CComVariant(NodeTranslation.y));
// 	spElemCurrentNode->setAttribute(L"posz", CComVariant(NodeTranslation.z));
// 
// 	spElemCurrentNode->setAttribute(L"qrotw", CComVariant(NodeRotation.w));
// 	spElemCurrentNode->setAttribute(L"qrotx", CComVariant(NodeRotation.x));
// 	spElemCurrentNode->setAttribute(L"qroty", CComVariant(NodeRotation.y));
// 	spElemCurrentNode->setAttribute(L"qrotz", CComVariant(NodeRotation.z));
// 
// 	spElemCurrentNode->setAttribute(L"sclx", CComVariant(NodeScaling.x));
// 	spElemCurrentNode->setAttribute(L"scly", CComVariant(NodeScaling.y));
// 	spElemCurrentNode->setAttribute(L"sclz", CComVariant(NodeScaling.z));
// 
// 
// 	// Add custom component data
// 	VCNINodeWrapperData data = wrapper.GetData();
// 	if (data.size() > 0)
// 	{
// 		XMLElementPtr spComponents;
// 		spXMLDoc->createElement(L"Components", &spComponents);
// 
// 		for (int i=0; i<data.size(); i++)
// 		{
// 			XMLElementPtr spCurrentComponent;
// 			VCNINodeComponent component = data[i];
// 			std::string componentName = component.GetName();
// 
// 			std::wstring ws(componentName.begin(), componentName.end());
// 			BSTR componentNameBstr = SysAllocStringLen(ws.data(), ws.size());
// 			spXMLDoc->createElement(componentNameBstr, &spCurrentComponent);
// 
// 			for (auto it = component.GetAttributesBegin(); it != component.GetAttributesEnd(); it++)
// 			{
// 				std::wstring attrWs(it->first.name.begin(), it->first.name.end());
// 				BSTR attributeNameBstr = SysAllocStringLen(attrWs.data(), attrWs.size());
// 
// 				std::string attrValue = it->second.ToString();
// 				std::wstring attrValWs(attrValue.begin(), attrValue.end());
// 				BSTR attributeValueBstr = SysAllocStringLen(attrValWs.data(), attrValWs.size());
// 
// 				spCurrentComponent->setAttribute(attributeNameBstr, CComVariant(attributeValueBstr));
// 			}
// 
// 
// 			spComponents->appendChild(spCurrentComponent, 0);
// 		}
// 		spElemCurrentNode->appendChild(spComponents, 0);
// 	}
// 
// 
// 
// 	//Attach this node to its parent
// 	ParentXMLElement->appendChild(spElemCurrentNode, 0);
// 
// 	// For each child of this node, we recurse into ourselves 
// 	// until no more children are found.
// 	for(int i=0;i<Node->GetChildCount();i++)
// 	{
// 		IGameNode * NodeChild = Node->GetNodeChild(i);
// 		if (!ExportModelsRecursive(NodeChild, spXMLDoc, spElemCurrentNode, dir))
// 			return FALSE;
// 	}
// 
// 	Node->ReleaseIGameObject();
// 
// 	return TRUE;
// }

int VCN3dsExporter::ExportNodesRecursive(IGameNode* Node, XMLDocPtr spXMLDoc, XMLElementPtr ParentXMLElement, MSTR dir, MSTR gameDataFolder, VCN3dsExporterProgressbarNotifier& notifier, std::unordered_set<std::wstring>& nameCollection)
{

  XMLElementPtr spElemCurrentNode;

  //Get the Instance Manager
  IInstanceMgr* InstanceMgr = IInstanceMgr::GetInstanceMgr();
  INodeTab NodeInstances;

  

  IGameObject * obj = Node->GetIGameObject(); 
//   switch(obj->GetIGameType()) 
//   { 
//   case IGameObject::IGAME_UNKNOWN: 
//     spElemCurrentNode->setAttribute(L"Type", CComVariant("Unknown"));
//     break;
// 
//   case IGameObject::IGAME_LIGHT: 
//     spElemCurrentNode->setAttribute(L"Type", CComVariant("Light"));
//     break;
// 
//   case IGameObject::IGAME_MESH: 
//     {
//       MSTR matName;
//       spElemCurrentNode->setAttribute(L"Type", CComVariant("Render"));
// 
//       //Get nodes instances
//       InstanceMgr->GetInstances(*(Node->GetMaxNode()), NodeInstances);
// 
//       matName = NodeInstances[0]->GetName();
//       matName += _T("Material");
// 
//       //Mesh exported will be the first instance of this node);
//       spElemCurrentNode->setAttribute(L"Mesh", CComVariant(NodeInstances[0]->GetName()));
//       spElemCurrentNode->setAttribute(L"Material", CComVariant(matName.data()));	
// 
//       CreateModelMaterialFile(Node, gameDataFolder);
//     }
//     break;
// 
//   case IGameObject::IGAME_SPLINE: 
//     spElemCurrentNode->setAttribute(L"Type", CComVariant("Spline"));
//     break;
// 
//   case IGameObject::IGAME_CAMERA: 
//     spElemCurrentNode->setAttribute(L"Type", CComVariant("Camera"));
//     break;
// 
//   case IGameObject::IGAME_HELPER: 
//     spElemCurrentNode->setAttribute(L"Type", CComVariant("Helper"));
//     break;
// 
//   case IGameObject::IGAME_BONE: 
//     spElemCurrentNode->setAttribute(L"Type", CComVariant("Bone"));
//     break;
// 
//   case IGameObject::IGAME_IKCHAIN: 
//     spElemCurrentNode->setAttribute(L"Type", CComVariant("IK_Chain"));
//     break;
//   }

  std::wstring nodeName(Node->GetName());
  if (nameCollection.find(nodeName) == nameCollection.end())
  {
	  bool exportNode = false;
	  IGameObject::ObjectTypes ot = obj->GetIGameType();
	  if (ot == IGameObject::IGAME_MESH || ot == IGameObject::IGAME_HELPER || ot == IGameObject::IGAME_UNKNOWN || ot == IGameObject::IGAME_LIGHT)
	  {
		  exportNode = true;
	  }

	  if (exportNode)
	  {
		  nameCollection.insert(nodeName);

		  

		  /////////
		  if (ot == IGameObject::IGAME_MESH)
		  {
			  spXMLDoc->createElement(L"Node", &spElemCurrentNode);
			  spElemCurrentNode->setAttribute(L"Name", CComVariant(Node->GetName()));

			  MSTR matName;
			  spElemCurrentNode->setAttribute(L"Type", CComVariant("Render"));
	  
			  //Get nodes instances
			  InstanceMgr->GetInstances(*(Node->GetMaxNode()), NodeInstances);
	  
			  matName = NodeInstances[0]->GetName();
			  matName += _T("Material");
	  
			  //Mesh exported will be the first instance of this node);
			  spElemCurrentNode->setAttribute(L"Mesh", CComVariant(NodeInstances[0]->GetName()));
			  spElemCurrentNode->setAttribute(L"Material", CComVariant(matName.data()));	
	  
			  CreateModelMaterialFile(Node, gameDataFolder);
		  }
		  else if (ot == IGameObject::IGAME_LIGHT)
		  {
			  const float diffuseMultiplier = 1.0f;
			  const float specularMultiplier = 0.1f;


			  IGameLight* gameLight = (IGameLight*) obj;

			  IGameLight::LightType lt = gameLight->GetLightType();
			  if (lt == IGameLight::IGAME_OMNI || lt == IGameLight::IGAME_DIR || lt == IGameLight::IGAME_FSPOT)
			  {
				  spXMLDoc->createElement(L"Node", &spElemCurrentNode);
				  spElemCurrentNode->setAttribute(L"Name", CComVariant(Node->GetName()));

				  // Set the light data
				  spElemCurrentNode->setAttribute(L"Type", CComVariant("Light"));
				  spElemCurrentNode->setAttribute(L"IsOn", CComVariant(gameLight->IsLightOn()));

				  // Properties to write
				  float lightMultiplier = 1.0f;
				  Point3 lightColor(0.0f, 0.0f, 0.0f);
				  Point3 lightDiffuse(0.0f, 0.0f, 0.0f);
				  Point3 lightSpecular(0.0f, 0.0f, 0.0f);
				  float constantAttenuation = 0.0f;
				  float linearAttenuation = 0.0f;
				  float quadAttenuation = 0.0f;
				  float attenStart = 0.0f;
				  float attenEnd = 1.0f;
				  int decayType = 0;
				  float hotSpotAngle = 10.0f;
				  float fallOffAngle = 20.0f;

				  // Get and set the color properties
				  IGameProperty* lightColorProperty = gameLight->GetLightColor();
				  if (lightColorProperty)
				  {
					  lightColorProperty->GetPropertyValue(lightColor);
					  IGameProperty* lightMultiplierProperty = gameLight->GetLightMultiplier();
					  if (lightMultiplierProperty)
					  {
						  lightMultiplierProperty->GetPropertyValue(lightMultiplier);
					  }
				  }
				  lightDiffuse = lightColor * diffuseMultiplier * lightMultiplier;
				  lightSpecular = lightColor * specularMultiplier * lightMultiplier;

				  spElemCurrentNode->setAttribute(L"diffuseR", CComVariant(lightDiffuse.x));
				  spElemCurrentNode->setAttribute(L"diffuseG", CComVariant(lightDiffuse.y));
				  spElemCurrentNode->setAttribute(L"diffuseB", CComVariant(lightDiffuse.z));
				  spElemCurrentNode->setAttribute(L"diffuseA", CComVariant(1.0f));

				  spElemCurrentNode->setAttribute(L"specularR", CComVariant(lightSpecular.x));
				  spElemCurrentNode->setAttribute(L"specularG", CComVariant(lightSpecular.y));
				  spElemCurrentNode->setAttribute(L"specularB", CComVariant(lightSpecular.z));
				  spElemCurrentNode->setAttribute(L"specularA", CComVariant(1.0f));


				  
				  // Get and set the Attenuation properties
				  IGameProperty* attenStartProperty = gameLight->GetLightAttenStart();
				  if (attenStartProperty)
				  {
					  attenStartProperty->GetPropertyValue(attenStart);
				  }
				  IGameProperty* attenEndProperty = gameLight->GetLightAttenEnd();
				  if (attenEndProperty)
				  {
					  attenEndProperty->GetPropertyValue(attenEnd);
				  }

				  IGameProperty* hotSpotAngleProperty = gameLight->GetLightHotSpot();
				  if (hotSpotAngleProperty)
				  {
					  hotSpotAngleProperty->GetPropertyValue(hotSpotAngle);
				  }

				  IGameProperty* fallOffAngleProperty = gameLight->GetLightFallOff();
				  if (fallOffAngleProperty)
				  {
					  fallOffAngleProperty->GetPropertyValue(fallOffAngle);
				  }

				  decayType = gameLight->GetLightDecayType();
				  float attenLength = max(0.001f, attenEnd - attenStart);
				  switch (decayType)
				  {
				  case 0: // Const (not supported in 3dsMax)
					  constantAttenuation = 0.0f;
					  linearAttenuation = 0.0f;
					  quadAttenuation = 0.0f;
					  break;
				  case 1: // Linear
					  constantAttenuation = 0.0f;
					  linearAttenuation = 1.0f / attenLength;
					  quadAttenuation = 0.0f;
					  break;
				  case 2: // Quad
					  constantAttenuation = 0.0f;
					  linearAttenuation = 1.0f / (attenLength + (attenLength * attenLength));
					  quadAttenuation = linearAttenuation;
					  break;
				  default: // error
					  constantAttenuation = 0.0f;
					  linearAttenuation = 0.0f;
					  quadAttenuation = 0.0f;
					  break;
				  }


				  // Set specific properties
				  switch (gameLight->GetLightType())
				  {
				  case IGameLight::IGAME_OMNI : // Point light
					  {
						  spElemCurrentNode->setAttribute(L"LightType", CComVariant("PointLight"));

						  spElemCurrentNode->setAttribute(L"ConstantAttenuation", CComVariant(constantAttenuation));
						  spElemCurrentNode->setAttribute(L"LinearAttenuation", CComVariant(linearAttenuation));
						  spElemCurrentNode->setAttribute(L"QuadAttenuation", CComVariant(quadAttenuation));

						  spElemCurrentNode->setAttribute(L"Range", CComVariant(attenStart));
						  spElemCurrentNode->setAttribute(L"MaxRange", CComVariant(attenEnd));
					  }
					  break;
				  case IGameLight::IGAME_FSPOT  : // Spot light (non targeted)
					  {
						  spElemCurrentNode->setAttribute(L"LightType", CComVariant("SpotLight"));

						  spElemCurrentNode->setAttribute(L"directionX", CComVariant(0.0f));
						  spElemCurrentNode->setAttribute(L"directionY", CComVariant(-1.0f));
						  spElemCurrentNode->setAttribute(L"directionZ", CComVariant(0.0f));

						  spElemCurrentNode->setAttribute(L"ConstantAttenuation", CComVariant(constantAttenuation));
						  spElemCurrentNode->setAttribute(L"LinearAttenuation", CComVariant(linearAttenuation));
						  spElemCurrentNode->setAttribute(L"QuadAttenuation", CComVariant(quadAttenuation));

						  spElemCurrentNode->setAttribute(L"Range", CComVariant(attenStart));
						  spElemCurrentNode->setAttribute(L"MaxRange", CComVariant(attenEnd));

						  spElemCurrentNode->setAttribute(L"Phi", CComVariant(DEG_TO_RAD * hotSpotAngle));
						  spElemCurrentNode->setAttribute(L"Theta", CComVariant(DEG_TO_RAD * fallOffAngle));
					  }
					  break;
				  case IGameLight::IGAME_DIR  : // Directional light
					  {
						  spElemCurrentNode->setAttribute(L"LightType", CComVariant("DirectionalLight"));
						  spElemCurrentNode->setAttribute(L"directionX", CComVariant(0.0f));
						  spElemCurrentNode->setAttribute(L"directionY", CComVariant(-1.0f));
						  spElemCurrentNode->setAttribute(L"directionZ", CComVariant(0.0f));
					  }
					  break;
				  default:
					  break;
				  }
			  }
			  else
			  {
				  VCN3dsExporterBridge::VCN3dsExporterBridgeManager::AddLogEntry("Unknown Light Type on node " + std::string(nodeName.begin(), nodeName.end()) + ". The only supported Lights type are Omni (Point lights), Directional Lights and Free Spot Lights.", VCN3dsExporterBridge::LogTypeCPP_WARNING);
			  }
		  }
		  else
		  {
			  spXMLDoc->createElement(L"Node", &spElemCurrentNode);
			  spElemCurrentNode->setAttribute(L"Name", CComVariant(Node->GetName()));

			  spElemCurrentNode->setAttribute(L"Type", CComVariant("Helper"));
		  }

		  //////////

		  //Set Default-Neutral Xforms as attributes
		  GMatrix NodeMatrix;
		  Point3 NodeTranslation;
		  Quat NodeRotation;
		  Point3 NodeScaling;

		  NodeMatrix = Node->GetLocalTM();
		  NodeTranslation = NodeMatrix.Translation();
		  NodeRotation = NodeMatrix.Rotation();
		  NodeScaling = NodeMatrix.Scaling();

		  spElemCurrentNode->setAttribute(L"posx", CComVariant(NodeTranslation.x));
		  spElemCurrentNode->setAttribute(L"posy", CComVariant(NodeTranslation.y));
		  spElemCurrentNode->setAttribute(L"posz", CComVariant(NodeTranslation.z));

		  spElemCurrentNode->setAttribute(L"qrotw", CComVariant(NodeRotation.w));
		  spElemCurrentNode->setAttribute(L"qrotx", CComVariant(NodeRotation.x));
		  spElemCurrentNode->setAttribute(L"qroty", CComVariant(NodeRotation.y));
		  spElemCurrentNode->setAttribute(L"qrotz", CComVariant(NodeRotation.z));

		  spElemCurrentNode->setAttribute(L"sclx", CComVariant(NodeScaling.x));
		  spElemCurrentNode->setAttribute(L"scly", CComVariant(NodeScaling.y));
		  spElemCurrentNode->setAttribute(L"sclz", CComVariant(NodeScaling.z));


		  // Add custom component data
		  try
		  {
			  VCNINodeWrapper wrapper(*Node->GetMaxNode());
			  VCNINodeWrapperData data = wrapper.GetData();
			  if (data.size() > 0)
			  {
				  XMLElementPtr spComponents;
				  spXMLDoc->createElement(L"Components", &spComponents);

				  for (int i=0; i<data.size(); i++)
				  {
					  XMLElementPtr spCurrentComponent;
					  VCNINodeComponent component = data[i];
					  std::string componentName = component.GetName();

					  std::wstring ws(componentName.begin(), componentName.end());
					  BSTR componentNameBstr = SysAllocStringLen(ws.data(), ws.size());
					  spXMLDoc->createElement(componentNameBstr, &spCurrentComponent);

					  for (auto it = component.GetAttributesBegin(); it != component.GetAttributesEnd(); it++)
					  {
						  std::wstring attrWs(it->first.name.begin(), it->first.name.end());
						  BSTR attributeNameBstr = SysAllocStringLen(attrWs.data(), attrWs.size());

						  std::string attrValue = it->second.ToString();
						  std::wstring attrValWs(attrValue.begin(), attrValue.end());
						  BSTR attributeValueBstr = SysAllocStringLen(attrValWs.data(), attrValWs.size());

						  spCurrentComponent->setAttribute(attributeNameBstr, CComVariant(attributeValueBstr));
					  }

					  spComponents->appendChild(spCurrentComponent, 0);
				  }
				  spElemCurrentNode->appendChild(spComponents, 0);
			  }
		  }
		  catch (std::exception& e)
		  {
			  std::stringstream ss;
			  ss << "Cound not load node's custom data. Contact Mathieu ASAP!! Node: " << Node->GetName();
			  VCN3dsExporterBridge::VCN3dsExporterBridgeManager::AddLogEntry(ss.str(), VCN3dsExporterBridge::LogTypeCPP_FATAL);
		  }




		  //Attach this node to its parent
		  ParentXMLElement->appendChild(spElemCurrentNode, 0);


		  // Update the progressbar
		  notifier.AddOneEntry();

		  // For each child of this node, we recurse into ourselves 
		  // until no more children are found.
		  for(int i=0;i<Node->GetChildCount();i++)
		  {
			  IGameNode * NodeChild = Node->GetNodeChild(i);
			  ExportNodesRecursive(NodeChild, spXMLDoc, spElemCurrentNode, dir, gameDataFolder, notifier, nameCollection);
		  }



	  }
	  else
	  {
		  VCN3dsExporterBridge::VCN3dsExporterBridgeManager::AddLogEntry("The node \"" + std::string(nodeName.begin(), nodeName.end()) + "\" does not have a valid type. Only the mesh nodes can be exporter. It (and it's children) will be ignored.", VCN3dsExporterBridge::LogTypeCPP_WARNING);
		  int nodeCount = GetNodeNodeCount(Node->GetMaxNode());
		  notifier.AddEntry(nodeCount);
	  }
	}
	else
	{
		VCN3dsExporterBridge::VCN3dsExporterBridgeManager::AddLogEntry("A node with the name \"" + std::string(nodeName.begin(), nodeName.end()) + "\" already exists. It (and it's children) will be ignored.", VCN3dsExporterBridge::LogTypeCPP_WARNING);
		int nodeCount = GetNodeNodeCount(Node->GetMaxNode());
		notifier.AddEntry(nodeCount);
	}

  
  Node->ReleaseIGameObject();

  return 0;
}


int VCN3dsExporter::ExportMeshesRecursive(IGameNode* gameNode, XMLDocPtr spXMLDoc, XMLElementPtr ParentXMLElement, VCN3dsExporterProgressbarNotifier& notifier)
{
  XMLElementPtr spElemCurrentNode;
  int counter = 0;

  //Get the Instance Manager
  IInstanceMgr* InstanceMgr = IInstanceMgr::GetInstanceMgr();
  INodeTab NodeInstances;

  IGameObject* gameObject = gameNode->GetIGameObject();

  if (gameObject->GetIGameType() == IGameObject::IGAME_MESH)
  {
    spXMLDoc->createElement(L"VCNMesh", &spElemCurrentNode);
    SetNameAndVersion(spElemCurrentNode,  gameNode->GetName());
	const wchar_t* name = gameNode->GetName();
    vector<Point3> aFaces;
    vector<Point3> aVertPos;
    vector<Point3> aVertNormals;
    vector<Point3> aVertColors;
    vector<Point2> aVertUVs;
    bool bGotMeshInfo = GetMeshInfo(gameObject, aFaces, aVertPos, aVertNormals, aVertColors, aVertUVs);
    
    if (bGotMeshInfo)
    {
      // Transform mesh info using object pivot
      if ( ExporterInstance.bObjectSpace )
      {
        INode* maxNode = gameNode->GetMaxNode();

        // Compute the pivot TM
        Matrix3 piv(1);
        piv.SetTrans(maxNode->GetObjOffsetPos());
        PreRotateMatrix(piv, maxNode->GetObjOffsetRot());
        ApplyScaling(piv, maxNode->GetObjOffsetScale());

        Matrix3 transMT = TransformMatrixLHY(piv);
        
        for(vector<Point3>::iterator it = aVertPos.begin(); it != aVertPos.end(); ++it)
        {
          Point3& v = *it;
          v = transMT * v;
        }

        transMT.NoTrans();
        for(vector<Point3>::iterator it = aVertNormals.begin(); it != aVertNormals.end(); ++it)
        {
          Point3& n = *it;
          n = transMT * n;
          n = n.Normalize();
        }
      }

      // Faces
      if (ExporterInstance.bExportFaces)
      {
        XMLElementPtr spElemFaces;
        spXMLDoc->createElement(L"Faces", &spElemFaces);
        spElemFaces->setAttribute(L"size", CComVariant(aFaces.size()));

        //Print Data from our face array
        vector<Point3>::iterator iterFaces;
        counter = 0;
        for(iterFaces = aFaces.begin(); iterFaces != aFaces.end(); iterFaces++)
        {
          //Create a Face element
          XMLElementPtr spElemFace;
          spXMLDoc->createElement(L"Face", &spElemFace);

          //Set ID Attribute
          spElemFace->setAttribute(L"id", CComVariant(counter));

          //Set vertex index Attribute
          spElemFace->setAttribute(L"VertIndex.x", CComVariant((*iterFaces).x));
          spElemFace->setAttribute(L"VertIndex.y", CComVariant((*iterFaces).y));
          spElemFace->setAttribute(L"VertIndex.z", CComVariant((*iterFaces).z));

          //Append to Faces element
          spElemFaces->appendChild(spElemFace, 0);

          //increment counter
          counter++;
        }

        spElemCurrentNode->appendChild(spElemFaces, 0);
      }

      //Vertex Positions
      if (ExporterInstance.bExportVertPos)
      {
        XMLElementPtr spElemVertsPos;
        spXMLDoc->createElement(L"VertsPos", &spElemVertsPos);
        spElemVertsPos->setAttribute(L"size", CComVariant(aVertPos.size()));

        //Print Data from our vertex positions array
        vector<Point3>::iterator iterVertPos;
        counter = 0;
        for(iterVertPos = aVertPos.begin(); iterVertPos != aVertPos.end(); iterVertPos++)
        {
          //Create a Face element
          XMLElementPtr spElemVertPos;
          spXMLDoc->createElement(L"VertPos", &spElemVertPos);

          //Set ID Attribute
          spElemVertPos->setAttribute(L"id", CComVariant(counter));

          //Set vertex position Attribute
          spElemVertPos->setAttribute(L"Pos.x", CComVariant((*iterVertPos).x));
          spElemVertPos->setAttribute(L"Pos.y", CComVariant((*iterVertPos).y));
          spElemVertPos->setAttribute(L"Pos.z", CComVariant((*iterVertPos).z));

          //Append to Faces element
          spElemVertsPos->appendChild(spElemVertPos, 0);

          //increment counter
          counter++;
        }

        spElemCurrentNode->appendChild(spElemVertsPos, 0);
      }

      //Vertex Normals
      if (ExporterInstance.bExportVertNormals)
      {
        XMLElementPtr spElemVertsNormals;
        spXMLDoc->createElement(L"VertsNormals", &spElemVertsNormals);
        spElemVertsNormals->setAttribute(L"size", CComVariant(aVertNormals.size()));

        //Print Data from our vertex normal array
        vector<Point3>::iterator iterVertNormal;
        counter = 0;
        for(iterVertNormal = aVertNormals.begin(); iterVertNormal != aVertNormals.end(); iterVertNormal++)
        {
          //Create a Face element
          XMLElementPtr spElemVertNormal;
          spXMLDoc->createElement(L"VertNormal", &spElemVertNormal);

          //Set ID Attribute
          spElemVertNormal->setAttribute(L"id", CComVariant(counter));

          //Set vertex normal Attribute
          spElemVertNormal->setAttribute(L"Normal.x", CComVariant((*iterVertNormal).x));
          spElemVertNormal->setAttribute(L"Normal.y", CComVariant((*iterVertNormal).y));
          spElemVertNormal->setAttribute(L"Normal.z", CComVariant((*iterVertNormal).z));

          //Append to Faces element
          spElemVertsNormals->appendChild(spElemVertNormal, 0);

          //increment counter
          counter++;
        }

        spElemCurrentNode->appendChild(spElemVertsNormals, 0);
      }

      //Diffuse UVs
      if (ExporterInstance.bExportUVs)
      {
        XMLElementPtr spElemDiffuseUVs;
        spXMLDoc->createElement(L"DiffuseUVs", &spElemDiffuseUVs);
        spElemDiffuseUVs->setAttribute(L"size", CComVariant(aVertUVs.size()));

        //Print Data from our vertex uvs array
        vector<Point2>::iterator iterVertUV;
        counter = 0;
        for(iterVertUV = aVertUVs.begin(); iterVertUV != aVertUVs.end(); iterVertUV++)
        {
          //Create a Face element
          XMLElementPtr spElemVertUV;
          spXMLDoc->createElement(L"VertUV", &spElemVertUV);

          //Set ID Attribute
          spElemVertUV->setAttribute(L"id", CComVariant(counter));

          //Set vertex normal Attribute
          spElemVertUV->setAttribute(L"UV.x", CComVariant((*iterVertUV).x));
          spElemVertUV->setAttribute(L"UV.y", CComVariant((*iterVertUV).y));

          //Append to Faces element
          spElemDiffuseUVs->appendChild(spElemVertUV, 0);

          //increment counter
          counter++;
        }
        spElemCurrentNode->appendChild(spElemDiffuseUVs, 0);
      }

      //Vertex Colors
      if (ExporterInstance.bExportVertexColor)
      {
        XMLElementPtr spElemVertsColors;
        spXMLDoc->createElement(L"VertsColors", &spElemVertsColors);
        spElemVertsColors->setAttribute(L"size", CComVariant(aVertColors.size()));

        //Print Data from our vertex normal array
        vector<Point3>::iterator iterVertColor;
        counter = 0;
        for(iterVertColor = aVertColors.begin(); iterVertColor != aVertColors.end(); iterVertColor++)
        {
          //Create a Face element
          XMLElementPtr spElemVertColor;
          spXMLDoc->createElement(L"VertColor", &spElemVertColor);

          //Set ID Attribute
          spElemVertColor->setAttribute(L"id", CComVariant(counter));

          //Set vertex normal Attribute
          spElemVertColor->setAttribute(L"Color.x", CComVariant((*iterVertColor).x));
          spElemVertColor->setAttribute(L"Color.y", CComVariant((*iterVertColor).y));
          spElemVertColor->setAttribute(L"Color.z", CComVariant((*iterVertColor).z));

          //Append to Faces element
          spElemVertsColors->appendChild(spElemVertColor, 0);

          //increment counter
          counter++;
        }

        spElemCurrentNode->appendChild(spElemVertsColors, 0);
      }
    } 
    else 
    { 
      //DebugPrint(Bad Object\n);
    } 

    //Attach this node to its parent
    ParentXMLElement->appendChild(spElemCurrentNode, 0);
  }

  notifier.AddOneEntry();

  // For each child of this node, we recurse into ourselves 
  // until no more children are found.
  int curExportedNodes = 1;
  for(int i=0;i<gameNode->GetChildCount();i++)
  {
    IGameNode * NodeChild = gameNode->GetNodeChild(i);
    int nIt = ExportMeshesRecursive(NodeChild, spXMLDoc, ParentXMLElement, notifier);
	curExportedNodes += nIt;
	if (nIt == 0)
		return curExportedNodes;
  }

  gameNode->ReleaseIGameObject();

  return curExportedNodes;
}

bool VCN3dsExporter::GetMeshInfo(IGameObject* gameObject, vector<Point3>& faces, vector<Point3>& vertices, 
                                 vector<Point3>& normals, vector<Point3>& colors, vector<Point2>& uvs)
{
  //This function fills and returns the arrays for aFaces aVertices aVertPos aVertNormals aVertColors aVertUVWs
  //The function returns true if everything went well
  //The option bRemoveDoubles takes longer to compute but returns arrays without doubles.

  //TODO : INDEX SHOULD BE IN DWORD and not in int

  //Get the mesh and initialize its data for export
  IGameMesh* gameMesh = (IGameMesh*)gameObject; 
  if(gameMesh->InitializeData()) 
  { 
    //Tell IGame to create a flattened Normals Array 
    gameMesh->SetCreateOptimizedNormalList();		

    //Get number of faces
    int numFaces = gameMesh->GetNumberOfFaces(); 

	// Preallocate
	faces.reserve(numFaces);
	vertices.reserve(numFaces * 3);
	normals.reserve(numFaces);
	colors.reserve(numFaces);
	uvs.reserve(numFaces * 3);

    //Declare array for vertices structure
    vector<VCNExportVertex> aVertices;

    //Get Data in our structure arrays
    for(int f=0;f<numFaces;f++) 
    {
      //Get a face 
      FaceEx * face = gameMesh->GetFace(f);

      // Vertex 0
      int vertIndex0 = FillVertexInfoInArrays(gameMesh, face, 0, vertices, normals, uvs, colors, aVertices);
      int vertIndex1 = FillVertexInfoInArrays(gameMesh, face, 1, vertices, normals, uvs, colors, aVertices);
      int vertIndex2 = FillVertexInfoInArrays(gameMesh, face, 2, vertices, normals, uvs, colors, aVertices);

      //get a vector for first and second segment of face
      Point3 vect_0_1 = vertices[vertIndex1] - vertices[vertIndex0];
      Point3 vect_1_2 = vertices[vertIndex2] - vertices[vertIndex1];

      //get the cross product of those 2 vectors
      Point3 cross_product = CrossProd(vect_0_1, vect_1_2);

      //Lets approximate face normal as average of vertex normals
      Point3 fnormal = (normals[vertIndex0] + normals[vertIndex1] + normals[vertIndex2]).Normalize();
      double dot_product = DotProd(cross_product,  fnormal);

      //if the dot product is greater than 0 then it means it's pointing in the same direction as the face normal
      if (dot_product > 0)
      {
        faces.push_back(Point3(vertIndex0, vertIndex1, vertIndex2));
      }
      else //change point order to respect CW or CWW convention
      {
        faces.push_back(Point3(vertIndex0, vertIndex2, vertIndex1));
      }
    }
  } 
  else 
  { 
    //DebugPrint(Bad Object\n);
    return false;
  } 

  return true;
}

int VCN3dsExporter::FillVertexInfoInArrays(IGameMesh* Mesh, FaceEx* Face, int CornerIndex, vector<Point3>& aVertPos, vector<Point3>& aVertNormals, vector<Point2>& aVertUVs, vector<Point3>& aVertColors, vector<VCNExportVertex>& aVertices)
{
  //Fonction qui ajoute l'information du vertex dans les tableaux appropriés
  //Retourne l'iterateur du vertex dans le tableau de vertex

  //Déclaration des variables
  int vertIndex = static_cast<int>( aVertices.size() );
  int i = 0;
  bool bVertFound = false;
  VCNExportVertex tVertex;
  Point3 vertPos;
  Point3 vertNormal;
  Point2 vertUV;
  Point3 vertColor;

  //position
  if (ExporterInstance.bExportVertPos)
  {
    vertPos = Mesh->GetVertex(Face->vert[CornerIndex], ExporterInstance.bObjectSpace); //(false = exportWorldSpace, so I guess true is objectspace)
  }
  else
  {
    vertPos = Point3(0.0, 0.0, 0.0);
  }
  tVertex.Pos = vertPos;

  //normal
  if (ExporterInstance.bExportVertNormals)
  {
    vertNormal = Mesh->GetNormal(Face, CornerIndex);
    if(ExporterInstance.bObjectSpace)//(convert normal to ObjectSpace)
    {
      GMatrix g = Mesh->GetIGameObjectTM();
      Matrix3 gm = g.ExtractMatrix3();
      gm.NoScale(); //This might fail if matrix not orthogonal, but I GameObjectTM is always orthogonal, see doc...
      gm.NoTrans(); 
      Matrix3 invgm = Inverse(gm);
      vertNormal = vertNormal * invgm;
    }
  }
  else
  {
    vertNormal = Point3(0.0, 0.0, 0.0);
  }
  tVertex.Normal = vertNormal;

  //UVW
  if ((Mesh->GetNumberOfTexVerts() != 0) && (ExporterInstance.bExportUVs))
  {
    //vertUV = Mesh->GetTexVertex(Face->texCoord[CornerIndex]);
        Point3 vertUVW = Mesh->GetMapVertex(1,Face->texCoord[CornerIndex]);
        vertUV.x = vertUVW.x;
        vertUV.y = 1 - vertUVW.y;
  }
  else
  {
    vertUV = Point2(0.0, 0.0);
  }
  tVertex.UV = vertUV;

  //VertexColor
  if ((Mesh->GetNumberOfColorVerts() != 0) && (ExporterInstance.bExportVertexColor))
  {
    vertColor = Mesh->GetColorVertex(Face->color[CornerIndex]);
  }
  else
  {
    vertColor = Point3(1.0, 1.0, 1.0);
  }
  tVertex.Color = vertColor;

  //Add Vertex to array
  if (ExporterInstance.bRemoveDoubles)
  {
    while(i<aVertices.size() && !bVertFound)
    {
      if(aVertices[i] == tVertex)
      {
        vertIndex = i;
        bVertFound = true;
      }
      i++;
    }
  }

  if (!bVertFound)
  {
    aVertices.push_back(tVertex);
    aVertPos.push_back(vertPos);
    aVertNormals.push_back(vertNormal);
    aVertUVs.push_back(vertUV);
    aVertColors.push_back(vertColor);
  }

  return vertIndex;

}

bool VCN3dsExporter::ExportXformsKFRecursively(IGameNode* Node, XMLDocPtr spXMLDoc, XMLElementPtr ParentXMLElement)
{
  //Déclaration des variables
  XMLElementPtr spElemCurrentNode;
  XMLElementPtr spElemCurrentXForm;
  IGameKeyTab PosKeys;	
  IGameKeyTab RotKeys;	
  IGameKeyTab SclKeys;	
  IGameObject * obj = Node->GetIGameObject();
  IGameControl * pGC = Node->GetIGameControl();
  float framerate = 30;

  //Collecter les keyframes dans les tableaux appropriés
  if(!ExporterInstance.bSampleAnimation)
  {
    if(ExporterInstance.bExportPosAnim)
    {
      pGC->GetQuickSampledKeys(PosKeys, IGAME_POS);
    }
    if(ExporterInstance.bExportRotAnim)
    {
      pGC->GetQuickSampledKeys(RotKeys,IGAME_ROT);
    }
    if(ExporterInstance.bExportSclAnim)
    {
      pGC->GetQuickSampledKeys(SclKeys,IGAME_SCALE);
    }
  }
  else
  {
    if((ExporterInstance.bExportPosAnim)&&(pGC->IsAnimated(IGAME_POS)))
    {
      pGC->GetFullSampledKeys(PosKeys, ExporterInstance.iSamplingFrameRate, IGAME_POS, false); //According to doc, last parameter should be set to true to get relative values... weird...
    }
    if((ExporterInstance.bExportRotAnim)&&(pGC->IsAnimated(IGAME_ROT)))
    {
      pGC->GetFullSampledKeys(RotKeys, ExporterInstance.iSamplingFrameRate, IGAME_ROT, false); //According to doc, last parameter should be set to true to get relative values... weird...
    }
    if((ExporterInstance.bExportSclAnim)&&(pGC->IsAnimated(IGAME_SCALE)))
    {
      pGC->GetFullSampledKeys(SclKeys, ExporterInstance.iSamplingFrameRate, IGAME_SCALE, false); //According to doc, last parameter should be set to true to get relative values... weird...
    }
  }

  //Créer un Element Joint pour le XML
  spXMLDoc->createElement(L"Joint", &spElemCurrentNode);
  spElemCurrentNode->setAttribute(L"Name", CComVariant(Node->GetName()));
  spElemCurrentNode->setAttribute(L"NbKFPos", CComVariant(PosKeys.Count()));
  spElemCurrentNode->setAttribute(L"NbKFRot", CComVariant(RotKeys.Count()));
  spElemCurrentNode->setAttribute(L"NbKFScl", CComVariant(SclKeys.Count()));
  spElemCurrentNode->setAttribute(L"PosAnimated", CComVariant((PosKeys.Count() > 0)? 1 : 0));
  spElemCurrentNode->setAttribute(L"RotAnimated", CComVariant((RotKeys.Count() > 0)? 1 : 0));
  spElemCurrentNode->setAttribute(L"SclAnimated", CComVariant((SclKeys.Count() > 0)? 1 : 0));

  //Ajouter les Key frames de Position dans le XML
  if(ExporterInstance.bExportPosAnim)
  {
    for(int i = 0;i<PosKeys.Count();i++)
    {		
      spXMLDoc->createElement(L"KF_Pos", &spElemCurrentXForm);
      spElemCurrentXForm->setAttribute(L"frame", CComVariant((int)(PosKeys[i].t/(4800/framerate))));
      spElemCurrentXForm->setAttribute(L"time", CComVariant(((float)PosKeys[i].t/4800.0)));
      spElemCurrentXForm->setAttribute(L"x", CComVariant(PosKeys[i].sampleKey.pval.x));
      spElemCurrentXForm->setAttribute(L"y", CComVariant(PosKeys[i].sampleKey.pval.y));
      spElemCurrentXForm->setAttribute(L"z", CComVariant(PosKeys[i].sampleKey.pval.z));

      //Attach this node to its parent
      spElemCurrentNode->appendChild(spElemCurrentXForm, 0);
    }
  }

  //Ajouter les Key frames de Rotation dans le XML
  if(ExporterInstance.bExportRotAnim)
  {
    float multiplier = 1 ;
    if(ExporterInstance.CoordSys == IGameConversionManager::IGAME_D3D)
    {
      multiplier = -1;
    }

    for(int i = 0;i<RotKeys.Count();i++)
    {		
      spXMLDoc->createElement(L"KF_Rot", &spElemCurrentXForm);
      spElemCurrentXForm->setAttribute(L"frame", CComVariant((int)(RotKeys[i].t/(4800/framerate))));
      spElemCurrentXForm->setAttribute(L"time", CComVariant(((float)RotKeys[i].t/4800.0)));
      spElemCurrentXForm->setAttribute(L"x", CComVariant(multiplier*RotKeys[i].sampleKey.qval.x));
      spElemCurrentXForm->setAttribute(L"y", CComVariant(multiplier*RotKeys[i].sampleKey.qval.y));
      spElemCurrentXForm->setAttribute(L"z", CComVariant(multiplier*RotKeys[i].sampleKey.qval.z));
      spElemCurrentXForm->setAttribute(L"w", CComVariant(RotKeys[i].sampleKey.qval.w));

      //Attach this node to its parent
      spElemCurrentNode->appendChild(spElemCurrentXForm, 0);
    }
  }

  //Ajouter les Key frames de Scale dans le XML
  if(ExporterInstance.bExportSclAnim)
  {
    for(int i = 0;i<SclKeys.Count();i++)
    {		
      spXMLDoc->createElement(L"KF_Scl", &spElemCurrentXForm);
      spElemCurrentXForm->setAttribute(L"frame", CComVariant((int)(SclKeys[i].t/(4800/framerate))));
      spElemCurrentXForm->setAttribute(L"time", CComVariant(((float)SclKeys[i].t/4800.0)));
      spElemCurrentXForm->setAttribute(L"x", CComVariant(SclKeys[i].sampleKey.sval.s.x));
      spElemCurrentXForm->setAttribute(L"y", CComVariant(SclKeys[i].sampleKey.sval.s.y));
      spElemCurrentXForm->setAttribute(L"z", CComVariant(SclKeys[i].sampleKey.sval.s.z));
      spElemCurrentXForm->setAttribute(L"qx", CComVariant(SclKeys[i].sampleKey.sval.q.x));
      spElemCurrentXForm->setAttribute(L"qy", CComVariant(SclKeys[i].sampleKey.sval.q.y));
      spElemCurrentXForm->setAttribute(L"qz", CComVariant(SclKeys[i].sampleKey.sval.q.z));
      spElemCurrentXForm->setAttribute(L"qx", CComVariant(SclKeys[i].sampleKey.sval.q.w));

      //Attach this node to its parent
      spElemCurrentNode->appendChild(spElemCurrentXForm, 0);
    }
  }

  //Attach this node to its parent
  ParentXMLElement->appendChild(spElemCurrentNode, 0);

  // For each child of this node, we recurse into ourselves 
  // until no more children are found.
  for(int i=0;i<Node->GetChildCount();i++)
  {
    IGameNode * NodeChild = Node->GetNodeChild(i);
    if (!ExportXformsKFRecursively(NodeChild, spXMLDoc, ParentXMLElement))
      return false;
  }

  Node->ReleaseIGameObject();

  return true;
}


bool VCN3dsExporter::ExportLightsRecursive(IGameNode* Node, XMLDocPtr spXMLDoc, XMLElementPtr ParentXMLElement)
{
  XMLElementPtr spElemCurrentNode;
  int counter = 0;

  //Get the Instance Manager
  IInstanceMgr* InstanceMgr = IInstanceMgr::GetInstanceMgr();
  INodeTab NodeInstances;

  IGameObject * obj = Node->GetIGameObject();

  if (obj->GetIGameType() == IGameObject::IGAME_LIGHT)
  {
    IGameLight* pLight = (IGameLight*)obj;
    spXMLDoc->createElement(L"Light", &spElemCurrentNode);
    SetNameAndVersion(spElemCurrentNode,  Node->GetName());

    //Color
    XMLElementPtr spElemLightColor;
    spXMLDoc->createElement(L"LightColor", &spElemLightColor);
    Point3 Color(0.0, 0.0, 0.0);
    pLight->GetLightColor()->GetPropertyValue(Color);
    spElemLightColor->setAttribute(L"r", CComVariant(Color.x));
    spElemLightColor->setAttribute(L"b", CComVariant(Color.y));
    spElemLightColor->setAttribute(L"g", CComVariant(Color.z));
    spElemCurrentNode->appendChild(spElemLightColor, 0);

    //LightMultiplier
    XMLElementPtr spElemLightMultiplier;
    spXMLDoc->createElement(L"LightMultiplier", &spElemLightMultiplier);
    float LightMultiplier = 0.0;
    pLight->GetLightMultiplier()->GetPropertyValue(LightMultiplier);
    spElemLightMultiplier->setAttribute(L"value", CComVariant(LightMultiplier));
    spElemCurrentNode->appendChild(spElemLightMultiplier, 0);

    //LightAttenEnd
    XMLElementPtr spElemLightAttenEnd;
    spXMLDoc->createElement(L"LightAttenEnd", &spElemLightAttenEnd);
    float LightAttenEnd = 0.0;

    pLight->GetLightAttenEnd()->GetPropertyValue(LightAttenEnd);
    spElemLightAttenEnd->setAttribute(L"value", CComVariant(LightAttenEnd));
    spElemCurrentNode->appendChild(spElemLightAttenEnd, 0);

    //LightAttenStart
    XMLElementPtr spElemLightAttenStart;
    spXMLDoc->createElement(L"LightAttenStart", &spElemLightAttenStart);
    float LightAttenStart = 0.0;
    pLight->GetLightAttenStart()->GetPropertyValue(LightAttenStart);
    spElemLightAttenStart->setAttribute(L"value", CComVariant(LightAttenStart));		
    spElemCurrentNode->appendChild(spElemLightAttenStart, 0);

    //LightFallOff
    XMLElementPtr spElemLightFallOff;
    spXMLDoc->createElement(L"LightFallOff", &spElemLightFallOff);
    float LightFallOff = 0.0;
    pLight->GetLightFallOff()->GetPropertyValue(LightFallOff);
    spElemLightFallOff->setAttribute(L"value", CComVariant(LightFallOff));
    spElemCurrentNode->appendChild(spElemLightFallOff, 0);

    //LightHotSpot
    XMLElementPtr spElemLightHotSpot;
    spXMLDoc->createElement(L"LightHotSpot", &spElemLightHotSpot);
    float LightHotSpot = 0.0;
    pLight->GetLightHotSpot()->GetPropertyValue(LightHotSpot);
    spElemLightHotSpot->setAttribute(L"value", CComVariant(LightHotSpot));		
    spElemCurrentNode->appendChild(spElemLightHotSpot, 0);

    //LightAspectRatio
    XMLElementPtr spElemLightAspectRatio;
    spXMLDoc->createElement(L"LightAspectRatio", &spElemLightAspectRatio);
    float LightAspectRatio = 0.0;
    pLight->GetLightAspectRatio()->GetPropertyValue(LightAspectRatio);
    spElemLightAspectRatio->setAttribute(L"value", CComVariant(LightAspectRatio));
    spElemCurrentNode->appendChild(spElemLightAspectRatio, 0);

    //LightDecayStart
    XMLElementPtr spElemLightDecayStart;
    spXMLDoc->createElement(L"LightDecayStart", &spElemLightDecayStart);
    float LightDecayStart = 0.0;
    pLight->GetLightDecayStart()->GetPropertyValue(LightDecayStart);
    spElemLightDecayStart->setAttribute(L"value", CComVariant(LightDecayStart));
    spElemCurrentNode->appendChild(spElemLightDecayStart, 0);

    //LightType
    XMLElementPtr spElemLightType;
    spXMLDoc->createElement(L"LightType", &spElemLightType);
    switch (pLight->GetLightType())
    {
    case IGameLight::IGAME_OMNI :
      spElemLightType->setAttribute(L"value", CComVariant("Omni"));
      break;

    case IGameLight::IGAME_DIR :
      spElemLightType->setAttribute(L"value", CComVariant("Directional"));
      break;

    case IGameLight::IGAME_TDIR:
      spElemLightType->setAttribute(L"value", CComVariant("Target_Directional"));
      break;		

    case IGameLight::IGAME_FSPOT :
      spElemLightType->setAttribute(L"value", CComVariant("Free_Spot"));
      break;

    case IGameLight::IGAME_TSPOT :
      spElemLightType->setAttribute(L"value", CComVariant("Target_Spot"));
      break;
    }
    spElemCurrentNode->appendChild(spElemLightType, 0);

    //LightOvershoot
    XMLElementPtr spElemLightOvershoot;
    spXMLDoc->createElement(L"LightOvershoot", &spElemLightOvershoot);
    bool LightOvershoot = false;
    LightOvershoot = pLight->GetLightOvershoot() ? true : false;
    spElemLightOvershoot->setAttribute(L"value", CComVariant(LightOvershoot))		;
    spElemCurrentNode->appendChild(spElemLightOvershoot, 0);

    //LightDecayType
    XMLElementPtr spElemLightDecayType;
    spXMLDoc->createElement(L"LightDecayType", &spElemLightDecayType);
    switch (pLight->GetLightDecayType())
    {
    case 0 :
      spElemLightDecayType->setAttribute(L"value", CComVariant("None"));
      break;

    case 1 :
      spElemLightDecayType->setAttribute(L"value", CComVariant("Inverse"));
      break;

    case 2 :
      spElemLightDecayType->setAttribute(L"value", CComVariant("Inverse_Square"));
      break;		
    }
    spElemCurrentNode->appendChild(spElemLightDecayType, 0);

    //SpotLightShape
    XMLElementPtr spElemSpotLightShape;
    spXMLDoc->createElement(L"SpotLightShape", &spElemSpotLightShape);
    switch (pLight->GetSpotLightShape())
    {
    case 0 :
      spElemSpotLightShape->setAttribute(L"value", CComVariant("Rect"));
      break;

    case 1 :
      spElemSpotLightShape->setAttribute(L"value", CComVariant("Circle"));
      break;
    }
    spElemCurrentNode->appendChild(spElemSpotLightShape, 0);

    //Target
    XMLElementPtr spElemTarget;
    spXMLDoc->createElement(L"Target", &spElemTarget);
    IGameNode* pTarget = pLight->GetLightTarget ();
    if(pTarget != NULL)
    {
      spElemTarget->setAttribute(L"name", CComVariant(pTarget->GetName()));
    }
    else
    {
      spElemTarget->setAttribute(L"name", CComVariant("No_Target"));
    }
    spElemCurrentNode->appendChild(spElemTarget, 0);

    //TODO : ADD excluded list

    //Attach this node to its parent
    ParentXMLElement->appendChild(spElemCurrentNode, 0);
  }

  // For each child of this node, we recurse into ourselves 
  // until no more children are found.
  for(int i=0;i<Node->GetChildCount();i++)
  {
    IGameNode * NodeChild = Node->GetNodeChild(i);
    if (!ExportLightsRecursive(NodeChild, spXMLDoc, ParentXMLElement))
      return false;
  }

  Node->ReleaseIGameObject();

  return true;
}

bool VCN3dsExporter::ExportCamerasRecursive(IGameNode* Node, XMLDocPtr spXMLDoc, XMLElementPtr ParentXMLElement)
{
  XMLElementPtr spElemCurrentNode;
  int counter = 0;

  //Get the Instance Manager
  IInstanceMgr* InstanceMgr = IInstanceMgr::GetInstanceMgr();
  INodeTab NodeInstances;

  IGameObject * obj = Node->GetIGameObject();

  if (obj->GetIGameType() == IGameObject::IGAME_CAMERA)
  {
    IGameCamera * pCam = (IGameCamera*)obj;
    spXMLDoc->createElement(L"Camera", &spElemCurrentNode);
    SetNameAndVersion(spElemCurrentNode,  Node->GetName());

    XMLElementPtr spElemFOV;
    spXMLDoc->createElement(L"FOV", &spElemFOV);
    float FOV = 0.0;
    pCam->GetCameraFOV()->GetPropertyValue(FOV);
    spElemFOV->setAttribute(L"value", CComVariant(FOV));
    spElemCurrentNode->appendChild(spElemFOV,0);

    XMLElementPtr spElemFarClip;
    spXMLDoc->createElement(L"FarClip", &spElemFarClip);
    float FarClip = 0.0;
    pCam->GetCameraFarClip()->GetPropertyValue(FarClip);
    spElemFarClip->setAttribute(L"value", CComVariant(FarClip));
    spElemCurrentNode->appendChild(spElemFarClip, 0);

    XMLElementPtr spElemNearClip;
    spXMLDoc->createElement(L"NearClip", &spElemNearClip);
    float NearClip = 0.0;
    pCam->GetCameraNearClip()->GetPropertyValue(NearClip);
    spElemNearClip->setAttribute(L"value", CComVariant(NearClip));
    spElemCurrentNode->appendChild(spElemNearClip, 0);

    XMLElementPtr spElemTargetDist;
    spXMLDoc->createElement(L"TargetDist", &spElemTargetDist);
    float TargetDist = 0.0;
    pCam->GetCameraTargetDist()->GetPropertyValue(TargetDist);
    spElemTargetDist->setAttribute(L"value", CComVariant(TargetDist));
    spElemCurrentNode->appendChild(spElemTargetDist, 0);

    XMLElementPtr spElemTarget;
    spXMLDoc->createElement(L"Target", &spElemTarget);
    IGameNode* pTarget = pCam->GetCameraTarget();
    if(pTarget != NULL)
    {
      spElemTarget->setAttribute(L"name", CComVariant(pTarget->GetName()));
    }
    else
    {
      spElemTarget->setAttribute(L"name", CComVariant("No_Target"));
    }
    spElemCurrentNode->appendChild(spElemTarget, 0);

    //Attach this node to its parent
    ParentXMLElement->appendChild(spElemCurrentNode, 0);
  }

  // For each child of this node, we recurse into ourselves 
  // until no more children are found.
  for(int i=0;i<Node->GetChildCount();i++)
  {
    IGameNode * NodeChild = Node->GetNodeChild(i);
    if (!ExportCamerasRecursive(NodeChild, spXMLDoc, ParentXMLElement))
      return false;
  }

  Node->ReleaseIGameObject();

  return true;
}


void VCN3dsExporter::ClosePropertyEditor()
{
	ExporterInstance.bEditorEnabled = false;
	//::GetCOREInterface()->UnRegisterDlgWnd(ExporterInstance.hPropertyEditorWindowHandle);
	ExporterInstance.UpdateUI();
}

void VCN3dsExporter::UpdateUI()
{
	CheckDlgButton(ExporterInstance.hExportDlg, IDC_CHECK_ENABLE_EDITOR_MODE, ExporterInstance.bEditorEnabled);
	UpdateGameDataFolderPath(ExporterInstance.mExporterGameDataFolderPath);
}




void VCN3dsExporter::PropertyEditorModificationReceived( const VCNINodeWrapperData& newData )
{
	Tab<INode*> t;
	GetSelectedNodes(t);
	if (t.Count() == 1)
	{
		try
		{
			VCNINodeWrapper wrapper(*(t[0]));
			wrapper.SetData(newData);
			wrapper.SaveIfDirty();
		}
		catch (std::exception& e)
		{
			VCN3dsExporterBridge::VCN3dsExporterBridgeManager::AddLogEntry("Could not save the node's data. Contact Mathieu ASAP!!!", VCN3dsExporterBridge::LogTypeCPP_FATAL);
		}	
		
		ExporterInstance.UpdateUI();
	}
}

void VCN3dsExporter::ActivateStateChanged( bool activated )
{
	if (activated)
	{
		DisableAccelerators();
	}
	else
	{
		EnableAccelerators();
	}
	BOOL state = AcceleratorsEnabled();
	int i=0;
}


void VCN3dsExporter::ExportNodesOnly()
{
	ExportScene(true, false);
}


void VCN3dsExporter::ExportScene(bool exportNodes /*= true*/, bool exportMeshes /*= true*/)
{

	// Do validations first

	// Validate Root Node
	IGameScene * pIgame = VCN3dsExporter::Init3DXI(ExporterInstance.ip);	
	if (pIgame->GetTopLevelNodeCount() < 1)
	{
		VCN3dsExporterBridge::VCN3dsExporterBridgeManager::AddLogEntry("There is no Root node in this scene. Cancelling...", VCN3dsExporterBridge::LogTypeCPP_ERROR);
		return;
	}
	else if (pIgame->GetTopLevelNodeCount() > 1)
	{
		VCN3dsExporterBridge::VCN3dsExporterBridgeManager::AddLogEntry("There is more than one Root node in the scene. Cancelling...", VCN3dsExporterBridge::LogTypeCPP_ERROR);
		return;
	}


	// Export nodes first
	std::wstring dataFolderPath = ExporterInstance.mExporterGameDataFolderPath;
	std::wstring baseModelsDir;
	std::wstring baseMeshesDir;
	std::wstring outputModelFilePath = L"";
	std::wstring outputMeshFilePath = L"";
	// Check if filepath is valid with this folder path
	// TODO: Let the user select a file other than default file
	bool validModelFilePath = true;
	bool validMeshesFilePath = true;
	if (dataFolderPath.length() < 2)
	{
		validModelFilePath = false;
		validMeshesFilePath = false;
	}
	else
	{
		wchar_t lastChar = dataFolderPath[dataFolderPath.length()-1];
		if (lastChar == '/' || lastChar == '\\')
		{
			dataFolderPath = dataFolderPath.substr(0, dataFolderPath.length()-1);
		}
		baseModelsDir = dataFolderPath + L"\\Models";
		baseMeshesDir = dataFolderPath + L"\\Meshes";
		outputModelFilePath = dataFolderPath;
		outputMeshFilePath = dataFolderPath;
		outputModelFilePath += L"\\Models\\DefaultScene.MDL.xml";
		outputMeshFilePath += L"\\Meshes\\DefaultScene.MSH.xml";
		if (exportNodes)
		{
			FILE* testFile = _wfopen(outputModelFilePath.c_str(), L"w");
			if (testFile)
			{
				fclose(testFile);
			}
			else
			{
				validModelFilePath = false;
			}
		}

		if (exportMeshes)
		{
			FILE* testFile2 = _wfopen(outputMeshFilePath.c_str(), L"w");
			if (testFile2)
			{
				fclose(testFile2);
			}
			else
			{
				validMeshesFilePath = false;
			}
		}
	}


	MSTR modelFilename(outputModelFilePath.c_str());
	MSTR meshFilename(outputMeshFilePath.c_str());
	MSTR initialModelsDir(baseModelsDir.c_str());
	MSTR initialMesheDir(baseMeshesDir.c_str());
	if (!validModelFilePath)
	{
		FilterList filterList;
		filterList.Append(_T("VCNMDL files (*.MDL.xml)"));
		filterList.Append(_T("*.MDL.xml"));
		if(!((Interface10*)ExporterInstance.ip)->DoMaxSaveAsDialog(ExporterInstance.ip->GetMAXHWnd(), _T("Save Model file as..."), modelFilename, initialModelsDir, filterList))
		{
			// Cancel
			return;
		}
		std::wstring ws(initialModelsDir.data());
		int l1 = ws.find_last_of('/');
		int l2 = ws.find_last_of('\\');
		int l = max(l1, l2);
		if (l != -1)
		{
			ws = ws.substr(0, l);
			dataFolderPath = ws;
		}
	}
	if (!validMeshesFilePath)
	{
		FilterList filterList;
		filterList.Append(_T("VCNMSH files (*.MSH.xml)"));
		filterList.Append(_T("*.MSH.xml"));
		if(!((Interface10*)ExporterInstance.ip)->DoMaxSaveAsDialog(ExporterInstance.ip->GetMAXHWnd(), _T("Save Mesh file as..."), meshFilename, initialMesheDir, filterList))
		{
			// Cancel
			return;
		}
	}

	if (exportNodes)
	{
		// Reformat filename if we find .MDL.xml.MDL.xml cause it was unindented
		if (_tcsstr(modelFilename, MSTR(_T(".MDL.xml.MDL.xml"))))
		{
			modelFilename.remove(modelFilename.Length() - 8);
		}
		MSTR dataFolderPathMstr(dataFolderPath.c_str());
		ExportAllNodes(modelFilename, initialModelsDir, dataFolderPathMstr);
	}
	
	if (exportMeshes)
	{
		// Reformat filename if we find .MSH.xml.MSH.xml cause it was unintended
		if (_tcsstr(meshFilename, MSTR(_T(".MSH.xml.MSH.xml"))))
		{
			meshFilename.remove(meshFilename.Length() - 8);
		}
		ExportAllMeshes(meshFilename, initialMesheDir);
	}

	VCN3dsExporterBridge::VCN3dsExporterBridgeManager::AddLogEntry("Adding material and texture files to SVN.", VCN3dsExporterBridge::LogTypeCPP_MESSAGE);
	// Try to add it to svn
	std::wstring fullSVN = dataFolderPath;
	fullSVN.append(L"\\Textures");
	fullSVN.append(L"*");
	fullSVN.append(dataFolderPath);
	fullSVN.append(L"\\Materials\\AutoGenerated");
	bool svnSuccessTextures = VCN3dsExporterBridge::VCN3dsExporterBridgeManager::TryToAddToSVN(std::string(dataFolderPath.begin(), dataFolderPath.end()));
	if (svnSuccessTextures)
	{
		VCN3dsExporterBridge::VCN3dsExporterBridgeManager::AddLogEntry("Files added to SVN.", VCN3dsExporterBridge::LogTypeCPP_MESSAGE);
	}
	else
	{
		VCN3dsExporterBridge::VCN3dsExporterBridgeManager::AddLogEntry("Could not add the file " + std::string(fullSVN.begin(), fullSVN.end()) + " to SVN.", VCN3dsExporterBridge::LogTypeCPP_WARNING);
	}
}

void VCN3dsExporter::ExportAllNodes(const MSTR& filename, const MSTR& initialDir, const MSTR& gameDataFolder)
{
	//Init IGame
	IGameScene * pIgame = VCN3dsExporter::Init3DXI(ExporterInstance.ip);					

	//Init XMLDoc with MSXML COM smart pointers
	XMLDocPtr spDocOutput;
	XMLElementPtr spElemRoot;
	VCN3dsExporter::InitXMLDoc(spDocOutput, spElemRoot, "VCNModels");

	int totalNodeCount = GetSceneNodeCount();
	VCN3dsExporterProgressbarNotifier notifier("Exporting models (node tree)", totalNodeCount);

	std::unordered_set<std::wstring> nameCollection;
	
	//Get ModelInfo in the xml document
	for(int loop = 0; loop < pIgame->GetTopLevelNodeCount(); loop++) 
	{
		//Create an XML node for the model
		XMLElementPtr spElemModel;
		spDocOutput->createElement(_bstr_t("VCNModel"), &spElemModel);

		MSTR elemNodeName = MSTR(_T("MDL_")) + pIgame->GetTopLevelNode(loop)->GetName();
		VCN3dsExporter::SetNameAndVersion(spElemModel, elemNodeName);

		//Get the nodes for this model
		IGameNode * pGameNode = pIgame->GetTopLevelNode(loop);
		VCN3dsExporter::ExportNodesRecursive(pGameNode, spDocOutput, spElemModel, initialDir, gameDataFolder, notifier, nameCollection);
		//Attach this node to its parent
		spElemRoot->appendChild(spElemModel, NULL);
	}


	//Release 3DXI and Finish XML document					
	VCN3dsExporter::FinishXMLDoc(spDocOutput, filename);
	pIgame->ReleaseIGame();
}

void VCN3dsExporter::ExportAllMeshes(const MSTR& filename, const MSTR& initialDir)
{
	// Export meshes contained in the node tree
	int totalNodeCount = GetSceneNodeCount();
	VCN3dsExporterProgressbarNotifier notifier("Exporting meshes", totalNodeCount);
	//Init IGame
	IGameScene * pIgame = VCN3dsExporter::Init3DXI(ExporterInstance.ip);					

	//Init XMLDoc with MSXML COM smart pointers
	XMLDocPtr spDocOutput;
	XMLElementPtr spElemRoot;
	VCN3dsExporter::InitXMLDoc(spDocOutput, spElemRoot, "VCNMeshes");

	

	//Get MeshInfo in the xml document
	for(int loop = 0; loop < pIgame->GetTopLevelNodeCount(); loop++) 
	{ 
		IGameNode * pGameNode = pIgame->GetTopLevelNode(loop);
		VCN3dsExporter::ExportMeshesRecursive(pGameNode, spDocOutput, spElemRoot, notifier);
	}

	// Release 3DXI and Finish XML document					
	VCN3dsExporter::FinishXMLDoc(spDocOutput, filename);
	pIgame->ReleaseIGame();
	
}
