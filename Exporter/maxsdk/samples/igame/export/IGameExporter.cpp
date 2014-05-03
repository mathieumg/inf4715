/**********************************************************************
 *<
	FILE: IGameExporter.cpp

	DESCRIPTION:	Sample to test the IGame Interfaces.  It follows a similar format
					to the Ascii Exporter.  However is does diverge a little in order
					to handle properties etc..

	TODO: Break the file down into smaller chunks for easier loading.

	CREATED BY:		Neil Hazzard	

	HISTORY:		parttime coding Summer 2002

 *>	Copyright (c) 2002, All Rights Reserved.
 **********************************************************************/

#include "msxml2.h"
#include "IGameExporter.h"
#include "XMLUtility.h"
#include "decomp.h"

#include "IGame.h"
#include "IGameObject.h"
#include "IGameProperty.h"
#include "IGameControl.h"
#include "IGameModifier.h"
#include "IConversionManager.h"
#include "IGameError.h"
#include "IGameFX.h"

#include "3dsmaxport.h"

#define IGAMEEXPORTER_CLASS_ID	Class_ID(0x79d613a4, 0x4f21c3ad)

#define BEZIER	0
#define TCB		1
#define LINEAR	2
#define SAMPLE	3

//corresponds to XML schema
TCHAR* mapSlotNames[] = {
		_T("Diffuse"),
		_T("Ambient"),
		_T("Specular"),
		_T("SpecularLevel"),
		_T("Opacity"),
		_T("Glossiness"),
		_T("SelfIllumination"),
		_T("Filter"),
		_T("Bump"),
		_T("Reflection"),
		_T("Refraction"),
		_T("Displacement"),
		_T("Unknown") };



// XML helper class		
class CCoInitialize {
	public:
		CCoInitialize() : m_hr(CoInitialize(NULL)) { }
		~CCoInitialize() { if (SUCCEEDED(m_hr)) CoUninitialize(); }
		operator HRESULT() const { return m_hr; }
		HRESULT m_hr;
};

class IGameExporter : public SceneExport {
	public:

		IGameScene * pIgame;

		CCoInitialize init;              //must be declared before any IXMLDOM objects
		CComPtr<IXMLDOMDocument>  pXMLDoc;
		CComPtr<IXMLDOMNode> pRoot;		//this is our root node 	
		CComPtr<IXMLDOMNode> iGameNode;	//the IGame child - which is the main node
		CComPtr<IXMLDOMNode> rootNode;
		static HWND hParams;

		
		int curNode;

		int staticFrame;
		int framePerSample;
		BOOL exportGeom;
		BOOL exportNormals;
		BOOL exportVertexColor;
		BOOL exportControllers;
		BOOL exportFaceSmgp;
		BOOL exportTexCoords;
		BOOL exportMappingChannel;
		BOOL exportConstraints;
		BOOL exportMaterials;
		BOOL exportSplines;
		BOOL exportModifiers;
		BOOL exportSkin;
		BOOL exportGenMod;
		BOOL forceSample;
		BOOL splitFile;
		BOOL exportQuaternions;
		BOOL exportObjectSpace;
		BOOL exportRelative;
		BOOL exportNormalsPerFace;
		int cS;
		int exportCoord;
		bool showPrompts;
		bool exportSelected;

		TSTR fileName;
		TSTR splitPath;

		float igameVersion, exporterVersion;



		int				ExtCount();					// Number of extensions supported
		const TCHAR *	Ext(int n);					// Extension #n (i.e. "3DS")
		const TCHAR *	LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
		const TCHAR *	ShortDesc();				// Short ASCII description (i.e. "3D Studio")
		const TCHAR *	AuthorName();				// ASCII Author name
		const TCHAR *	CopyrightMessage();			// ASCII Copyright message
		const TCHAR *	OtherMessage1();			// Other message #1
		const TCHAR *	OtherMessage2();			// Other message #2
		unsigned int	Version();					// Version number * 100 (i.e. v3.01 = 301)
		void			ShowAbout(HWND hWnd);		// Show DLL's "About..." box

		BOOL SupportsOptions(int ext, DWORD options);
		int	DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);

		
		CComPtr <IXMLDOMNode> ExportSceneInfo(CComPtr<IXMLDOMDocument> pXMLDOMDoc, CComPtr<IXMLDOMNode>  pDOMRoot);
		void ExportNodeInfo(IGameNode * node);
		void ExportChildNodeInfo(CComPtr<IXMLDOMNode> parent, IGameNode * child);
		void ExportMaterials();
		void ExportPositionControllers(CComPtr<IXMLDOMNode> node, IGameControl * cont, Point3);
		void ExportRotationControllers(CComPtr<IXMLDOMNode> node, IGameControl * cont);
		void ExportScaleControllers(CComPtr<IXMLDOMNode> node, IGameControl * cont);
		void ExportControllers(CComPtr<IXMLDOMNode> node, IGameControl * cont);

		void DumpMaterial(CComPtr<IXMLDOMNode> node,IGameMaterial * mat, int index, int matID = -1);
		void DumpTexture(CComPtr<IXMLDOMNode> node,IGameMaterial * mat);
		void DumpBezierKeys(IGameControlType Type, IGameKeyTab Keys, CComPtr<IXMLDOMNode> prsData);
		void DumpTCBKeys(IGameControlType Type, IGameKeyTab Keys, CComPtr<IXMLDOMNode> prsData);
		void DumpLinearKeys(IGameControlType Type, IGameKeyTab Keys, CComPtr<IXMLDOMNode> prsData);
		void DumpBezierFloatKeys(IGameControlType Type, IGameKeyTab, int, CComPtr<IXMLDOMNode> );
		void DumpBezierPosKeys(IGameControlType, IGameKeyTab, Point3, CComPtr<IXMLDOMNode> );
		void DumpBezierRotKeys(IGameKeyTab, CComPtr<IXMLDOMNode> );
		void DumpBezierSclKeys(IGameKeyTab, CComPtr<IXMLDOMNode> );
		void DumpConstraints(CComPtr<IXMLDOMNode> prsData, IGameConstraint * c);
		void DumpModifiers(CComPtr<IXMLDOMNode> prsData, IGameModifier * m);
		void DumpSkin(CComPtr<IXMLDOMNode> modNode, IGameSkin * s);
		void DumpIKChain(IGameIKChain * ikch, CComPtr<IXMLDOMNode> ikData);

		void DumpEulerController(IGameControl * sc,CComPtr<IXMLDOMNode> prsNode);
		void DumpIndePositionController(IGameControl * sc,CComPtr<IXMLDOMNode>, Point3);
		void DumpProperties(CComPtr<IXMLDOMNode> node, IGameProperty * prop);
		void DumpFXProperties(CComPtr<IXMLDOMNode> node, IGameFXProperty * prop);
		void DumpNamedProperty(CComPtr<IXMLDOMNode>, TCHAR*, IGameProperty* );
		void DumpProperty(IGameProperty*, CComPtr<IXMLDOMNode>);


		void DumpMesh(IGameMesh *gm,CComPtr<IXMLDOMNode> geomData);
		void DumpSpline(IGameSpline *sp,CComPtr<IXMLDOMNode> splineData);
		void DumpLight(IGameLight *lt, CComPtr<IXMLDOMNode> parent);
		void DumpCamera(IGameCamera *ca, CComPtr<IXMLDOMNode> parent);
		void DumpSampleKeys(IGameControl * sc,CComPtr<IXMLDOMNode> prsNode, IGameControlType Type, bool quick = false);
		void DumpListController(IGameControl * sc,CComPtr<IXMLDOMNode> listNode);
		void DumpMatrix(Matrix3 tm,CComPtr<IXMLDOMNode> parent);


		void MakeSplitFilename(IGameNode * node, TSTR & buf);
		void makeValidURIFilename(TSTR&, bool = false);
		BOOL ReadConfig();
		void WriteConfig();
		TSTR GetCfgFilename();
		IGameExporter();
		~IGameExporter();		

};


class IGameExporterClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new IGameExporter(); }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
	Class_ID		ClassID() { return IGAMEEXPORTER_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("IGameExporter"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle

};



static IGameExporterClassDesc IGameExporterDesc;
ClassDesc2* GetIGameExporterDesc() { return &IGameExporterDesc; }

static bool IsFloatController(IGameControlType Type)
{
	if(Type == IGAME_FLOAT || Type==IGAME_EULER_X || Type == IGAME_EULER_Y || 
		Type == IGAME_EULER_Z || Type == IGAME_POS_X || Type == IGAME_POS_Y || 
		Type == IGAME_POS_Z)
		return true;

	return false;
}

int numVertex;

INT_PTR CALLBACK IGameExporterOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
   IGameExporter *exp = DLGetWindowLongPtr<IGameExporter*>(hWnd); 
	ISpinnerControl * spin;
	int ID;
	
	switch(message) {
		case WM_INITDIALOG:
			exp = (IGameExporter *)lParam;
         DLSetWindowLongPtr(hWnd, lParam); 
			CenterWindow(hWnd,GetParent(hWnd));
			spin = GetISpinner(GetDlgItem(hWnd, IDC_STATIC_FRAME_SPIN)); 
			spin->LinkToEdit(GetDlgItem(hWnd,IDC_STATIC_FRAME), EDITTYPE_INT ); 
			spin->SetLimits(0, 100, TRUE); 
			spin->SetScale(1.0f);
			spin->SetValue(exp->staticFrame ,FALSE);
			ReleaseISpinner(spin);
						
			spin = GetISpinner(GetDlgItem(hWnd, IDC_SAMPLE_FRAME_SPIN)); 
			spin->LinkToEdit(GetDlgItem(hWnd,IDC_SAMPLE_FRAME), EDITTYPE_INT ); 
			spin->SetLimits(1, 100, TRUE); 
			spin->SetScale(1.0f);
			spin->SetValue(exp->framePerSample ,FALSE);
			ReleaseISpinner(spin);
			CheckDlgButton(hWnd,IDC_EXP_GEOMETRY,exp->exportGeom);
			CheckDlgButton(hWnd,IDC_EXP_NORMALS,exp->exportNormals);
			CheckDlgButton(hWnd,IDC_EXP_CONTROLLERS,exp->exportControllers);
			CheckDlgButton(hWnd,IDC_EXP_FACESMGRP,exp->exportFaceSmgp);
			CheckDlgButton(hWnd,IDC_EXP_VCOLORS,exp->exportVertexColor);
			CheckDlgButton(hWnd,IDC_EXP_TEXCOORD,exp->exportTexCoords);
			CheckDlgButton(hWnd,IDC_EXP_MAPCHAN,exp->exportMappingChannel);
			CheckDlgButton(hWnd,IDC_EXP_MATERIAL,exp->exportMaterials);
			CheckDlgButton(hWnd,IDC_EXP_SPLINES,exp->exportSplines);
			CheckDlgButton(hWnd,IDC_EXP_MODIFIERS,exp->exportModifiers);
			CheckDlgButton(hWnd,IDC_EXP_SAMPLECONT,exp->forceSample);
			CheckDlgButton(hWnd,IDC_EXP_CONSTRAINTS,exp->exportConstraints);
			CheckDlgButton(hWnd,IDC_EXP_SKIN,exp->exportSkin);
			CheckDlgButton(hWnd,IDC_EXP_GENMOD,exp->exportGenMod);
			CheckDlgButton(hWnd,IDC_SPLITFILE,exp->splitFile);
			CheckDlgButton(hWnd,IDC_EXP_OBJECTSPACE,exp->exportObjectSpace);
			CheckDlgButton(hWnd,IDC_EXP_QUATERNIONS,exp->exportQuaternions);
			CheckDlgButton(hWnd,IDC_EXP_RELATIVE,exp->exportRelative);
			

			ID = IDC_COORD_MAX + exp->cS;
			CheckRadioButton(hWnd,IDC_COORD_MAX,IDC_COORD_OGL,ID);

			ID = IDC_NORMALS_LIST + exp->exportNormalsPerFace;
			CheckRadioButton(hWnd,IDC_NORMALS_LIST,IDC_NORMALS_FACE,ID);

			EnableWindow(GetDlgItem(hWnd, IDC_EXP_NORMALS), exp->exportGeom);
			EnableWindow(GetDlgItem(hWnd, IDC_EXP_FACESMGRP), exp->exportGeom);
			EnableWindow(GetDlgItem(hWnd, IDC_EXP_VCOLORS),  exp->exportGeom);
			EnableWindow(GetDlgItem(hWnd, IDC_EXP_TEXCOORD),  exp->exportGeom);
			EnableWindow(GetDlgItem(hWnd, IDC_EXP_MAPCHAN),  exp->exportGeom);
			EnableWindow(GetDlgItem(hWnd, IDC_EXP_OBJECTSPACE),  exp->exportGeom);
			
			EnableWindow(GetDlgItem(hWnd, IDC_EXP_CONSTRAINTS),  exp->exportControllers);
			EnableWindow(GetDlgItem(hWnd, IDC_EXP_SAMPLECONT),  exp->exportControllers);
			EnableWindow(GetDlgItem(hWnd, IDC_EXP_QUATERNIONS), exp->exportControllers);
			EnableWindow(GetDlgItem(hWnd, IDC_EXP_RELATIVE), exp->exportControllers);
	
			EnableWindow(GetDlgItem(hWnd, IDC_EXP_SKIN),  exp->exportModifiers);
			EnableWindow(GetDlgItem(hWnd, IDC_EXP_GENMOD),  exp->exportModifiers);

			EnableWindow(GetDlgItem(hWnd, IDC_NORMALS_LIST),exp->exportNormals);
			EnableWindow(GetDlgItem(hWnd, IDC_NORMALS_FACE),exp->exportNormals);

			//Versioning
			TCHAR Title [256];
            _stprintf(Title,_T("IGame Exporter version %.3f; IGame version %.3f"),
				exp->exporterVersion, exp->igameVersion);
			SetWindowText(hWnd,Title);
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {

				case IDC_EXP_GEOMETRY:
					EnableWindow(GetDlgItem(hWnd, IDC_EXP_NORMALS), IsDlgButtonChecked(hWnd, IDC_EXP_GEOMETRY));
					EnableWindow(GetDlgItem(hWnd, IDC_EXP_FACESMGRP), IsDlgButtonChecked(hWnd, IDC_EXP_GEOMETRY));
					EnableWindow(GetDlgItem(hWnd, IDC_EXP_VCOLORS),  IsDlgButtonChecked(hWnd, IDC_EXP_GEOMETRY));
					EnableWindow(GetDlgItem(hWnd, IDC_EXP_TEXCOORD),  IsDlgButtonChecked(hWnd, IDC_EXP_GEOMETRY));
					EnableWindow(GetDlgItem(hWnd, IDC_EXP_MAPCHAN),  IsDlgButtonChecked(hWnd, IDC_EXP_GEOMETRY));
					EnableWindow(GetDlgItem(hWnd, IDC_EXP_OBJECTSPACE),  IsDlgButtonChecked(hWnd, IDC_EXP_GEOMETRY));
					break;
				case IDC_EXP_CONTROLLERS:
					EnableWindow(GetDlgItem(hWnd, IDC_EXP_CONSTRAINTS), IsDlgButtonChecked(hWnd, IDC_EXP_CONTROLLERS));
					EnableWindow(GetDlgItem(hWnd, IDC_EXP_SAMPLECONT),  IsDlgButtonChecked(hWnd, IDC_EXP_CONTROLLERS));
					EnableWindow(GetDlgItem(hWnd, IDC_EXP_QUATERNIONS), IsDlgButtonChecked(hWnd, IDC_EXP_CONTROLLERS));
					EnableWindow(GetDlgItem(hWnd, IDC_EXP_RELATIVE), IsDlgButtonChecked(hWnd, IDC_EXP_CONTROLLERS));
					break;
				case IDC_EXP_MODIFIERS:
					EnableWindow(GetDlgItem(hWnd, IDC_EXP_SKIN), IsDlgButtonChecked(hWnd, IDC_EXP_MODIFIERS));
					EnableWindow(GetDlgItem(hWnd, IDC_EXP_GENMOD),  IsDlgButtonChecked(hWnd, IDC_EXP_MODIFIERS));

					break;

				case IDC_EXP_NORMALS:
					if(exp->igameVersion >= 1.12)
					{
						EnableWindow(GetDlgItem(hWnd, IDC_NORMALS_FACE), IsDlgButtonChecked(hWnd, IDC_EXP_NORMALS));
					}
					EnableWindow(GetDlgItem(hWnd, IDC_NORMALS_LIST), IsDlgButtonChecked(hWnd, IDC_EXP_NORMALS));
					break;

				case IDOK:
					exp->exportGeom = IsDlgButtonChecked(hWnd, IDC_EXP_GEOMETRY);
					exp->exportNormals = IsDlgButtonChecked(hWnd, IDC_EXP_NORMALS);
					exp->exportControllers = IsDlgButtonChecked(hWnd, IDC_EXP_CONTROLLERS);
					exp->exportFaceSmgp = IsDlgButtonChecked(hWnd, IDC_EXP_FACESMGRP);
					exp->exportVertexColor = IsDlgButtonChecked(hWnd, IDC_EXP_VCOLORS);
					exp->exportTexCoords = IsDlgButtonChecked(hWnd, IDC_EXP_TEXCOORD);
					exp->exportMappingChannel = IsDlgButtonChecked(hWnd, IDC_EXP_MAPCHAN);
					exp->exportMaterials = IsDlgButtonChecked(hWnd, IDC_EXP_MATERIAL);
					exp->exportSplines = IsDlgButtonChecked(hWnd, IDC_EXP_SPLINES);
					exp->exportModifiers = IsDlgButtonChecked(hWnd, IDC_EXP_MODIFIERS);
					exp->forceSample = IsDlgButtonChecked(hWnd, IDC_EXP_SAMPLECONT);
					exp->exportConstraints = IsDlgButtonChecked(hWnd, IDC_EXP_CONSTRAINTS);
					exp->exportSkin = IsDlgButtonChecked(hWnd, IDC_EXP_SKIN);
					exp->exportGenMod = IsDlgButtonChecked(hWnd, IDC_EXP_GENMOD);
					exp->splitFile = IsDlgButtonChecked(hWnd,IDC_SPLITFILE);
					exp->exportQuaternions = IsDlgButtonChecked(hWnd,IDC_EXP_QUATERNIONS);
					exp->exportObjectSpace = IsDlgButtonChecked(hWnd,IDC_EXP_OBJECTSPACE);
					exp->exportRelative = IsDlgButtonChecked(hWnd,IDC_EXP_RELATIVE);
					if (IsDlgButtonChecked(hWnd, IDC_COORD_MAX))
						exp->cS = IGameConversionManager::IGAME_MAX;
					else if (IsDlgButtonChecked(hWnd, IDC_COORD_OGL))
						exp->cS = IGameConversionManager::IGAME_OGL;
					else
						exp->cS = IGameConversionManager::IGAME_D3D;


					exp->exportNormalsPerFace = (IsDlgButtonChecked(hWnd,IDC_NORMALS_LIST))? FALSE : TRUE ;

					spin = GetISpinner(GetDlgItem(hWnd, IDC_STATIC_FRAME_SPIN)); 
					exp->staticFrame = spin->GetIVal(); 
					ReleaseISpinner(spin);
					spin = GetISpinner(GetDlgItem(hWnd, IDC_SAMPLE_FRAME_SPIN)); 
					exp->framePerSample = spin->GetIVal(); 
					ReleaseISpinner(spin);
					EndDialog(hWnd, 1);
					break;
				case IDCANCEL:
					EndDialog(hWnd,0);
					break;
			}
		
		default:
			return FALSE;
	
	}
	return TRUE;
	
}

// Replace some characters we don't care for.
TCHAR *FixupName (TCHAR *buf)
{
	static TCHAR buffer[256];
	TCHAR* cPtr;

    _tcscpy(buffer, buf);
    cPtr = buffer;

    while(*cPtr) {
		if (*cPtr == _T('"')) *cPtr = 39;	// Replace double-quote with single quote.
        else if (*cPtr <= 31) *cPtr = _T('_');	// Replace control characters with underscore
        cPtr++;
    }

	return buffer;
}

//void stripWhiteSpace(TSTR * buf, TCHAR &newBuf)
//{
//
//	TCHAR newb[256]={""};
//	strcpy(newb,buf->data());
//
//	int len = strlen(newb);
//
//	int index = 0;
//
//	for(int i=0;i<len;i++)
//	{
//		if((newb[i] != ' ') && (!ispunct(newb[i])))
//			(&newBuf)[index++] = newb[i];
//	}
//}

//--- IGameExporter -------------------------------------------------------
IGameExporter::IGameExporter()
{

	staticFrame = 0;
	framePerSample = 4;
	exportGeom = TRUE;
	exportNormals = TRUE;
	exportVertexColor = FALSE;
	exportControllers = FALSE;
	exportFaceSmgp = FALSE;
	exportTexCoords = TRUE;
	exportMappingChannel = FALSE;
	exportMaterials = TRUE;
	exportConstraints = FALSE;
	exportSplines = FALSE;
	exportModifiers = FALSE;
	forceSample = FALSE;
	exportSkin = TRUE;
	exportGenMod = FALSE;
	cS = 0;	//max default
	rootNode = NULL;
	iGameNode = NULL;
	pRoot = NULL;
	pXMLDoc = NULL;
	splitFile = TRUE;
	exportQuaternions = TRUE;
	exportObjectSpace = FALSE;
	exportRelative = FALSE;
	exportNormalsPerFace = FALSE;
	exporterVersion = 2.0f;
	
}

IGameExporter::~IGameExporter() 
{
	rootNode.Release();
	iGameNode.Release();
    pRoot.Release(); 
	pXMLDoc.Release();
}

int IGameExporter::ExtCount()
{
	//TODO: Returns the number of file name extensions supported by the plug-in.
	return 1;
}

const TCHAR *IGameExporter::Ext(int n)
{		
	//TODO: Return the 'i-th' file name extension (i.e. "3DS").
	return _T("xml");
}

const TCHAR *IGameExporter::LongDesc()
{
	//TODO: Return long ASCII description (i.e. "Targa 2.0 Image File")
	return _T("XML Export for IGame");
}
	
const TCHAR *IGameExporter::ShortDesc() 
{			
	//TODO: Return short ASCII description (i.e. "Targa")
	return _T("IGame Exporter");
}

const TCHAR *IGameExporter::AuthorName()
{			
	//TODO: Return ASCII Author name
	return _T("Neil Hazzard");
}

const TCHAR *IGameExporter::CopyrightMessage() 
{	
	// Return ASCII Copyright message
	return _T("");
}

const TCHAR *IGameExporter::OtherMessage1() 
{		
	//TODO: Return Other message #1 if any
	return _T("");
}

const TCHAR *IGameExporter::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int IGameExporter::Version()
{				
	//TODO: Return Version number * 100 (i.e. v3.01 = 301)
	return exporterVersion*100;
}

void IGameExporter::ShowAbout(HWND hWnd)
{			
	// Optional
}

BOOL IGameExporter::SupportsOptions(int ext, DWORD options)
{
	// TODO Decide which options to support.  Simply return
	// true for each option supported by each Extension 
	// the exporter supports.

	return TRUE;
}


void IGameExporter::MakeSplitFilename(IGameNode * node, TSTR & buf)
{
	buf =  splitPath;
	buf += _T("\\");
	buf += fileName + _T("_") + node->GetName() + _T(".xml");
	
}

CComPtr <IXMLDOMNode> IGameExporter::ExportSceneInfo(CComPtr<IXMLDOMDocument> pXMLDOMDoc, CComPtr<IXMLDOMNode> pDOMRoot)
{
	TSTR buf;
	CComPtr <IXMLDOMNode> mainNode;
	CComPtr <IXMLDOMNode> sceneNode;

	struct tm *newtime;
	time_t aclock;

	time( &aclock );
	newtime = localtime(&aclock);

	TSTR today = _tasctime(newtime);	// The date string has a \n appended.
	today.remove(today.length()-1);		// Remove the \n

	CreateXMLNode(pXMLDOMDoc, pDOMRoot, _T("IGame"), &mainNode);
	buf.printf (_T("%.1f"), exporterVersion);
	AddXMLAttribute(mainNode, _T("xmlns"), _T("urn:maxxml"));
	AddXMLAttribute(mainNode, _T("Version"), buf.data());
	AddXMLAttribute(mainNode, _T("Date"), today.data());
	
	CreateXMLNode(pXMLDOMDoc,mainNode,_T("SceneInfo"),&sceneNode);

	AddXMLAttribute(sceneNode,_T("FileName"),const_cast<TCHAR*>(pIgame->GetSceneFileName()));

	buf.printf(_T("%d"),pIgame->GetSceneStartTime() / pIgame->GetSceneTicks());
	AddXMLAttribute(sceneNode,_T("StartFrame"),buf.data());
	
	buf.printf(_T("%d"),pIgame->GetSceneEndTime()/pIgame->GetSceneTicks());
	AddXMLAttribute(sceneNode,_T("EndFrame"),buf.data());
	
	buf.printf(_T("%d"),GetFrameRate());
	AddXMLAttribute(sceneNode,_T("FrameRate"),buf.data());
	
	buf.printf(_T("%d"),pIgame->GetSceneTicks());
	AddXMLAttribute(sceneNode,_T("TicksPerFrame"),buf.data());

	if(cS == 0)
		buf = _T("3dsmax");
	else if(cS == 1)
		buf = _T("directx");
	else if(cS == 2)
		buf = _T("opengl");
	AddXMLAttribute(sceneNode,_T("CoordinateSystem"),buf.data());

	buf = (exportQuaternions) ? _T("Quaternion") : _T("AngleAxis");
	AddXMLAttribute (sceneNode, _T("RotationFormat"), buf.data());

	if (exportObjectSpace) buf = _T("true");
	else buf = _T("false");
	AddXMLAttribute (sceneNode, _T("ObjectSpace"), buf.data());

	sceneNode = NULL;
	return mainNode;

}

void IGameExporter::ExportChildNodeInfo(CComPtr<IXMLDOMNode> parent, IGameNode * child)
{
	IGameKeyTab poskeys;
	IGameKeyTab rotkeys;
	IGameKeyTab scalekeys;
	TSTR buf,data;

	CComPtr <IXMLDOMNode> prsNode,group;
	CComPtr <IXMLDOMNode> matIndex;
	CComPtr <IXMLDOMNode> geomData,splineData,ikData;
	CComPtr <IXMLDOMNode> tmNodeParent;

	CComPtr<IXMLDOMDocument> pSubDocMesh;
	CComPtr <IXMLDOMNode> pRootSubMesh, pSubMesh;

	if(splitFile)
	{
		CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER,  IID_IXMLDOMDocument, (void**)&pSubDocMesh);
		pSubDocMesh->QueryInterface(IID_IXMLDOMNode, (void **)&pRootSubMesh);
 		pSubMesh = ExportSceneInfo(pSubDocMesh, pRootSubMesh);
	}

	curNode ++;
	buf += _T("Processing: ");
	buf += child->GetName();
	GetCOREInterface()->ProgressUpdate((int)((float)curNode/pIgame->GetTotalNodeCount()*100.0f),FALSE,buf.data()); 

	buf = child->GetName();
	AddXMLAttribute(parent,_T("Name"), FixupName (buf.dataForWrite()));

	buf.printf(_T("%d"),child->GetNodeID());
	AddXMLAttribute(parent,_T("NodeID"),buf.data());

	IGameNode * p = child->GetNodeParent();
	if(p)
	{
		buf.printf(_T("%d"),p->GetNodeID());
		AddXMLAttribute(parent,_T("ParentID"),buf.data());
	}

	CreateXMLNode(pXMLDoc,parent,_T("NodeTM"),&tmNodeParent);
	DumpMatrix(child->GetWorldTM(staticFrame).ExtractMatrix3(),tmNodeParent);

	if(child->IsGroupOwner())
	{
		AddXMLAttribute(parent,_T("NodeType"), _T("Group"));

		buf.printf(_T("%d"),child->GetChildCount());
		AddXMLAttribute(parent,_T("NumChildren"),buf.data());
	}
	else
	{
		ULONG  handle = child->GetMaxNode()->GetHandle();

		if(child->GetMaterialIndex() !=-1){
			buf.printf(_T("%d"),child->GetMaterialIndex());
			AddXMLAttribute(parent,_T("MaterialIndex"),buf.data());
		}
		else
		{
			IPoint3 & color = child->GetWireframeColor();
			buf.printf (_T("%02x%02x%02xff"), color.x, color.y, color.z);
			AddXMLAttribute(parent,_T("WireframeColor"),buf.data());
		}

		IGameObject * obj = child->GetIGameObject();

		IGameObject::MaxType T = obj->GetMaxType();

		if(obj->IsObjectXRef())
			AddXMLAttribute(parent,_T("XRefObject"),_T("True"));

		switch(obj->GetIGameType())
		{
			
			case IGameObject::IGAME_XREF:
			{
				const TCHAR* fn = ((IGameXRefObject*)obj)->GetOriginalFileName();
				if (fn != NULL)
				{
					CreateXMLNode(pXMLDoc, parent, _T("XRef"), &geomData);	;
					AddXMLAttribute(geomData, _T("Include"), fn);
				}
			}
			break;

			case IGameObject::IGAME_BONE:
			{
				AddXMLAttribute(parent,_T("NodeType"),_T("Bone"));
				IGameSupportObject * hO = (IGameSupportObject*)obj;
				IGameMesh * hm = hO->GetMeshObject();
				if(hm->InitializeData())
				{
					CreateXMLNode(pXMLDoc,parent,_T("Mesh"),&geomData);
					if(splitFile)
					{
						TSTR filename;
						MakeSplitFilename(child,filename);
						CComPtr<IXMLDOMNode>subMeshNode;						
						AddXMLAttribute(geomData, _T("Include"),filename.data());
						CreateXMLNode(pSubDocMesh,pSubMesh,_T("Mesh"),&subMeshNode);
						AddXMLAttribute(subMeshNode,_T("Node"),child->GetName());
						geomData = subMeshNode;

					}
					DumpMesh(hm,geomData);
				}

				break;
			}

			case IGameObject::IGAME_HELPER:
			{
				AddXMLAttribute(parent,_T("NodeType"),_T("Helper"));

				IGameSupportObject * hO = (IGameSupportObject*)obj;
				IPropertyContainer * cc = hO->GetIPropertyContainer();
				IGameProperty * prop = cc->QueryProperty(101);
				if(prop)
				{
					const TCHAR * buf;
					prop->GetPropertyValue(buf);
				}
				prop = cc->QueryProperty(_T("IGameTestString"));

				if(prop)
				{
					const TCHAR * buf;
					prop->GetPropertyValue(buf);
				}
				prop = cc->QueryProperty(_T("IGameTestString"));

				IGameMesh * hm = hO->GetMeshObject();
				if(hm->InitializeData())
				{
					CreateXMLNode(pXMLDoc,parent,_T("Mesh"),&geomData);
					if(splitFile)
					{
						TSTR filename;
						MakeSplitFilename(child,filename);
						CComPtr<IXMLDOMNode>subMeshNode;						
						AddXMLAttribute(geomData, _T("Include"),filename.data());
						CreateXMLNode(pSubDocMesh,pSubMesh,_T("GeomData"),&subMeshNode);
						AddXMLAttribute(subMeshNode,_T("Node"),child->GetName());
						geomData = subMeshNode;

					}
					DumpMesh(hm,geomData);
				}


				break;
			}
			case IGameObject::IGAME_LIGHT:
			{
				AddXMLAttribute(parent,_T("NodeType"),_T("Light"));

				CComPtr <IXMLDOMNode> lightNode;
				CreateXMLNode(pXMLDoc,parent,_T("Light"),&lightNode);

				IGameLight * l = (IGameLight*)obj;
				if(l->GetLightType()==IGameLight::IGAME_OMNI)
					AddXMLAttribute(lightNode,_T("Type"),_T("Omni"));
				else if(l->GetLightType()==IGameLight::IGAME_TSPOT)
					AddXMLAttribute(lightNode,_T("Type"),_T("Targeted"));
				else if(l->GetLightType()==IGameLight::IGAME_FSPOT)
					AddXMLAttribute(lightNode,_T("Type"),_T("Free"));
				else if(l->GetLightType()==IGameLight::IGAME_TDIR)
					AddXMLAttribute(lightNode,_T("Type"),_T("TargetedDirectional"));
				else
					AddXMLAttribute(lightNode,_T("Type"),_T("Directional"));
				DumpLight(l,lightNode);
				break;
			}
			case IGameObject::IGAME_CAMERA:
			{
				AddXMLAttribute(parent,_T("NodeType"),_T("Camera"));

				CComPtr <IXMLDOMNode> camNode;
				CreateXMLNode(pXMLDoc,parent,_T("Camera"),&camNode);
				DumpCamera((IGameCamera*)obj,camNode);
				break;
			}

			case IGameObject::IGAME_MESH:
				if(exportGeom )
				{
					AddXMLAttribute(parent,_T("NodeType"),_T("Mesh"));
					IGameMesh * gM = (IGameMesh*)obj;
					CComPtr<IXMLDOMNode>subMeshNode;
					gM->SetCreateOptimizedNormalList();
					if(gM->InitializeData())
					{
						CreateXMLNode(pXMLDoc,parent,_T("Mesh"),&geomData);
						if(splitFile)
						{
							TSTR filename;
							MakeSplitFilename(child,filename);
							
							AddXMLAttribute(geomData, _T("Include"),filename.data());
							CreateXMLNode(pSubDocMesh,pSubMesh,_T("Mesh"),&subMeshNode);
							AddXMLAttribute(subMeshNode,_T("Node"),child->GetName());
							geomData = subMeshNode;

						}
						DumpMesh(gM,geomData);
					}
					else
					{
						CreateXMLNode(pXMLDoc,parent,_T("GeomData"),&geomData);
						AddXMLAttribute(geomData,_T("Error"),_T("BadObject"));
					}
				}
				break;
			case IGameObject::IGAME_SPLINE:
				if(exportSplines)
				{
					AddXMLAttribute(parent,_T("NodeType"),_T("Spline"));
					IGameSpline * sp = (IGameSpline*)obj;
					sp->InitializeData();
					CreateXMLNode(pXMLDoc,parent,_T("SplineData"),&splineData);
					DumpSpline(sp,splineData);
				}
				break;

			case IGameObject::IGAME_IKCHAIN:
				AddXMLAttribute(parent,_T("NodeType"),_T("IKChain"));
				IGameIKChain * ikch = (IGameIKChain*)obj;
				CreateXMLNode(pXMLDoc,parent,_T("IKChain"), & ikData);
				DumpIKChain(ikch,ikData);
				break;
				
		}

		if(splitFile)
		{
			TSTR buf;
			MakeSplitFilename(child,buf);
			pSubDocMesh->save(CComVariant(buf.data()));

		}
		//dump PRS Controller data
		prsNode = NULL;

		// In our "Game Engine" we deal with Bezier Position, Scale and TCB Rotation controllers !!
		// Only list controllers on position, rotation...
		if(exportControllers)
		{
			CComPtr<IXMLDOMNode>subContNode;
			IGameControl * pGameControl = child->GetIGameControl();

			if ((pGameControl->IsAnimated(IGAME_POS)) || pGameControl->IsAnimated(IGAME_ROT) || pGameControl->IsAnimated(IGAME_SCALE))
			{
			    bool bipedExported = false;
				CreateXMLNode(pXMLDoc,parent,_T("TMController"),&prsNode);
				GMatrix tm = child->GetWorldTM(); // Should be TM in Parent space

				CreateXMLNode(pXMLDoc,prsNode,_T("PRSController"),&subContNode);
				if (pGameControl->IsAnimated(IGAME_POS)){
					if (pGameControl->GetControlType(IGAME_POS)==IGameControl::IGAME_BIPED){
						DumpSampleKeys(pGameControl,subContNode,IGAME_TM);                        // with biped we're exporting sampled NodeTM
						bipedExported = true;
					}
					else						                       
					ExportPositionControllers(subContNode,pGameControl,tm.Translation());
			}
				if (pGameControl->IsAnimated(IGAME_ROT)) {
					if(pGameControl->GetControlType(IGAME_ROT)==IGameControl::IGAME_BIPED){
						if ( bipedExported == false){
							DumpSampleKeys(pGameControl,subContNode,IGAME_TM);
							bipedExported = true;
						}
					}
					else
						ExportRotationControllers(subContNode,pGameControl);
				}
				if (pGameControl->IsAnimated(IGAME_SCALE)){	
					if(pGameControl->GetControlType(IGAME_SCALE)==IGameControl::IGAME_BIPED){
						if (bipedExported == false){
							DumpSampleKeys(pGameControl,subContNode,IGAME_TM);
							bipedExported = true;
						}
					}	
					else
						ExportScaleControllers(subContNode,pGameControl);
				}
			}
#if	0     //az:  Vertex Animation test code 
			else
			{
				IGameControl::MaxControlType TV = pGameControl->GetControlType(IGAME_POINT3);
				if(TV==IGameControl::IGAME_MASTER)
				{
					CreateXMLNode(pXMLDoc,parent,_T("MasterPointController"),&prsNode);
					
					int num = pGameControl->GetNumOfVertControls();
					for (int index=0; index < num; index++)
					{
					    IGameControl * subCont = pGameControl->GetVertexControl(index);
						if (subCont)
						{
							TSTR buf;
							buf.printf(_T("%d"), index);
							CreateXMLNode(pXMLDoc, prsNode,_T("Vertex"), &subContNode);
							AddXMLAttribute(subContNode,_T("ID"), buf.data());
							
							IGameControl::MaxControlType T = subCont->GetControlType(IGAME_POINT3);
							
							IGameKeyTab poskeys;
                            if (!forceSample)
							{
								if(T==IGameControl::IGAME_MAXSTD && subCont->GetTCBKeys(poskeys,IGAME_POINT3))
								{
									DumpTCBKeys(IGAME_POINT3,poskeys,subContNode);
								}
								else if(T==IGameControl::IGAME_MAXSTD && subCont->GetBezierKeys(poskeys,IGAME_POINT3))
								{
									DumpBezierKeys(IGAME_POINT3,poskeys,subContNode);
								}
                            }
							else
								DumpSampleKeys(subCont, subContNode, IGAME_POINT3);
							
							subContNode = NULL;   //clean up smart pointer
						}
	
					}

				}
			}
#endif
		}

		if(exportModifiers)
		{
			int numMod = obj->GetNumModifiers();
			if(numMod > 0)
			{
				CComPtr <IXMLDOMNode> mod;
				CreateXMLNode(pXMLDoc,parent,_T("Modifiers"),&mod);
				TSTR buf;
				buf.printf(_T("%d"),numMod);
				AddXMLAttribute(mod,_T("Count"),buf.data());

				for(int i=0;i<numMod;i++)
				{
					IGameModifier * m = obj->GetIGameModifier(i);
					DumpModifiers(mod,m);
				}
			}
		}
	}	
	
	child->ReleaseIGameObject();

	for(int i=0;i<child->GetChildCount();i++)
	{
		CComPtr <IXMLDOMNode> cNode = NULL;
		IGameNode * newchild = child->GetNodeChild(i);
		CreateXMLNode(pXMLDoc,parent,_T("Node"),&cNode);

		// we deal with targets in the light/camera section
		if(newchild->IsTarget())
			continue;

		ExportChildNodeInfo(cNode,newchild);
	}

	

	prsNode = NULL;
	group = NULL;

}


void IGameExporter::ExportPositionControllers(CComPtr<IXMLDOMNode> node, IGameControl * pGameControl, Point3 pos)
{

	//CreateXMLNode(pXMLDoc,node,_T("Position"),&pNode);
	
	CComPtr<IXMLDOMNode> pNode;
	pXMLDoc->createNode(CComVariant(NODE_ELEMENT), CComBSTR(_T("Position")), NULL, &pNode);

	IGameKeyTab poskeys;
	IGameControl::MaxControlType T = pGameControl->GetControlType(IGAME_POS);

	//position
	if(T==IGameControl::IGAME_MAXSTD && pGameControl->GetBezierKeys(poskeys,IGAME_POS) && !forceSample)
	{
		DumpBezierPosKeys(IGAME_POS,poskeys,pos,pNode);
	}
	else if(T==IGameControl::IGAME_INDEPENDENT_POS && !forceSample)
	{
		DumpIndePositionController(pGameControl,pNode,pos);
	}

	else if(T==IGameControl::IGAME_MAXSTD && pGameControl->GetLinearKeys(poskeys,IGAME_POS) && !forceSample)
		DumpLinearKeys(IGAME_POS,poskeys,node);

	else if(T==IGameControl::IGAME_POS_CONSTRAINT && !forceSample)
	{
		IGameConstraint * cnst = pGameControl->GetConstraint(IGAME_POS);
		DumpConstraints(node,cnst);
	}
	else if(T==IGameControl::IGAME_LIST && !forceSample)
	{
		DumpListController(pGameControl,node);

	}
	else
	{
		if(forceSample || T==IGameControl::IGAME_UNKNOWN)
			DumpSampleKeys(pGameControl,node,IGAME_POS);
	}
		
	VARIANT_BOOL vBool;
	pNode->hasChildNodes(&vBool);

	if (vBool != FALSE)
	{
		CComPtr <IXMLDOMNode> posNode = NULL;
		node->appendChild(pNode,&posNode);
	}
}

void IGameExporter::ExportRotationControllers(CComPtr<IXMLDOMNode> node, IGameControl * pGameControl)
{

	IGameKeyTab rotkeys;


	//rotation
	IGameControl::MaxControlType T = pGameControl->GetControlType(IGAME_ROT);

	if(T==IGameControl::IGAME_MAXSTD && pGameControl->GetTCBKeys(rotkeys,IGAME_ROT) && !forceSample)
	{
		DumpTCBKeys(IGAME_ROT,rotkeys,node);
	}
	else if(T==IGameControl::IGAME_MAXSTD && pGameControl->GetLinearKeys(rotkeys,IGAME_ROT) && !forceSample)
	{
		DumpLinearKeys(IGAME_ROT,rotkeys,node);
	}
	else if(T==IGameControl::IGAME_MAXSTD && pGameControl->GetBezierKeys(rotkeys,IGAME_ROT) && !forceSample)
	{
		DumpBezierKeys(IGAME_ROT,rotkeys,node);
	}
	else if(T==IGameControl::IGAME_ROT_CONSTRAINT && !forceSample)
	{
		IGameConstraint * cnst = pGameControl->GetConstraint(IGAME_ROT);
		DumpConstraints(node,cnst);
	}
	else if(T==IGameControl::IGAME_LIST&& !forceSample)
	{
		DumpListController(pGameControl,node);

	}
	else if(T==IGameControl::IGAME_EULER&& !forceSample)
	{
		DumpEulerController(pGameControl,node);
	}
	else
	{
		if(forceSample || T==IGameControl::IGAME_UNKNOWN)
			DumpSampleKeys(pGameControl,node, IGAME_ROT);
	}
}

void IGameExporter::ExportScaleControllers(CComPtr<IXMLDOMNode> node, IGameControl * pGameControl)
{
	IGameKeyTab scalekeys;

	//scale
	IGameControl::MaxControlType T= pGameControl->GetControlType(IGAME_SCALE);

	if(T==IGameControl::IGAME_MAXSTD && pGameControl->GetBezierKeys(scalekeys,IGAME_SCALE) && !forceSample)
		DumpBezierKeys(IGAME_SCALE,scalekeys,node);
	else
	{
		if(forceSample || T==IGameControl::IGAME_UNKNOWN)
			DumpSampleKeys(pGameControl,node, IGAME_SCALE);
	}

}

void IGameExporter::ExportNodeInfo(IGameNode * node)
{
		
	rootNode = NULL;
	
	CreateXMLNode(pXMLDoc,iGameNode,_T("Node"),&rootNode);
	ExportChildNodeInfo(rootNode,node);

}

void IGameExporter::DumpMatrix(Matrix3 tm,CComPtr<IXMLDOMNode> parent)
{
	CComPtr <IXMLDOMNode> tmNode;
	AffineParts ap;
	float rotAngle;
	Point3 rotAxis;
	float scaleAxAngle;
	Point3 scaleAxis;
	Matrix3 m = tm;
	TSTR Buf;
	
	decomp_affine(m, &ap);

	// Quaternions are dumped as angle axis.
	AngAxisFromQ(ap.q, &rotAngle, rotAxis);
	AngAxisFromQ(ap.u, &scaleAxAngle, scaleAxis);

	CreateXMLNode(pXMLDoc,parent,_T("Translation"),&tmNode);
	Buf.printf(_T("%f %f %f"),ap.t.x,ap.t.y,ap.t.z);
	AddXMLText(pXMLDoc,tmNode,Buf.data());
	tmNode = NULL;

	CreateXMLNode(pXMLDoc,parent,_T("Rotation"),&tmNode);
	if(!exportQuaternions)
		Buf.printf(_T("%f %f %f %f"),rotAxis.x, rotAxis.y, rotAxis.z, rotAngle);
	else
		Buf.printf(_T("%f %f %f %f"),ap.q.x, ap.q.y, ap.q.z, ap.q.w);

	AddXMLText(pXMLDoc,tmNode,Buf.data());
	tmNode = NULL;

	CreateXMLNode(pXMLDoc,parent,_T("Scale"),&tmNode);
	if (!exportQuaternions)
		Buf.printf (_T("%f %f %f %f %f %f %f"), ap.k.x, ap.k.y, ap.k.z, scaleAxis.x,scaleAxis.y,scaleAxis.z, scaleAxAngle);
	else
		Buf.printf (_T("%f %f %f %f %f %f %f"), ap.k.x, ap.k.y, ap.k.z, ap.u.x, ap.u.y, ap.u.z, ap.u.w);

	AddXMLText(pXMLDoc,tmNode,Buf.data());


}

void GetKeyTypeName(TSTR &name, IGameControlType Type)
{
	if(Type==IGAME_POS)
		name.printf(_T("Position"));
	else if(Type==IGAME_ROT)
		name.printf(_T("Rotation"));
	else if(Type==IGAME_POINT3)
		name.printf(_T("Point3"));
	else if(Type==IGAME_FLOAT)
		name.printf(_T("float"));
	else if(Type==IGAME_SCALE)
		name.printf(_T("Scale"));
	else if(Type == IGAME_TM)
		name.printf(_T("NodeTM"));
	else if(Type == IGAME_EULER_X)
		name.printf(_T("EulerX"));
	else if(Type == IGAME_EULER_Y)
		name.printf(_T("EulerY"));
	else if(Type == IGAME_EULER_Z)
		name.printf(_T("EulerZ"));
	else if(Type == IGAME_POS_X)
		name.printf(_T("IndePosX"));
	else if(Type == IGAME_POS_Y)
		name.printf(_T("IndePosY"));
	else if(Type == IGAME_POS_Z)
		name.printf(_T("IndePosZ"));
	else
		name.printf(_T("Huh!!"));

}

void GetConstraintTypeName(TSTR &name, IGameConstraint::ConstraintType Type)
{
	if(Type == IGameConstraint::IGAME_PATH)
		name.printf(_T("Path"));
	else if(Type == IGameConstraint::IGAME_POSITION)
		name.printf(_T("Position"));
	else if(Type == IGameConstraint::IGAME_ORIENTATION)
		name.printf(_T("Orientation"));
	else if (Type == IGameConstraint::IGAME_LOOKAT)
		name.printf(_T("lookAt"));
	else
		name.printf(_T("Huh!!"));
}

void IGameExporter::DumpIndePositionController(IGameControl * sc,CComPtr<IXMLDOMNode> prsNode, Point3 pos)
{
	TSTR buf;
	CComPtr <IXMLDOMNode> prsChild;
	CComPtr <IXMLDOMNode> prsAxis;
	IGameKeyTab xCont,yCont,zCont;

	buf.printf(_T("%f %f %f"), pos.x, pos.y, pos.z);
	AddXMLAttribute(prsNode,_T("Value"),buf.data());

	CreateXMLNode(pXMLDoc,prsNode,_T("PositionXYZController"),&prsChild);

	CreateXMLNode(pXMLDoc,prsChild,_T("X"),&prsAxis);
	buf.printf(_T("%f"), pos.x);
	AddXMLAttribute(prsAxis,_T("Value"),buf.data());

	if(sc->GetBezierKeys(xCont,IGAME_POS_X))
		DumpBezierFloatKeys(IGAME_POS_X, xCont, -1, prsAxis);
	else if(sc->GetLinearKeys(xCont,IGAME_POS_X))
		DumpLinearKeys(IGAME_POS_X,xCont,prsAxis);

	prsAxis = NULL;
	CreateXMLNode(pXMLDoc,prsChild,_T("Y"),&prsAxis);
	buf.printf(_T("%f"), pos.y);
	AddXMLAttribute(prsAxis,_T("Value"),buf.data());

	if(sc->GetBezierKeys(yCont,IGAME_POS_Y))
		DumpBezierFloatKeys(IGAME_POS_Y, yCont, -1, prsAxis);
	else if(sc->GetLinearKeys(yCont,IGAME_POS_Y))
		DumpLinearKeys(IGAME_POS_Y,yCont,prsAxis);

	// Dump Z axis keys
	prsAxis = NULL;
	CreateXMLNode(pXMLDoc,prsChild,_T("Z"),&prsAxis);
	buf.printf(_T("%f"), pos.z);
	AddXMLAttribute(prsAxis,_T("Value"),buf.data());

	if(sc->GetBezierKeys(zCont,IGAME_POS_Z))
		DumpBezierFloatKeys(IGAME_POS_Z, zCont, -1, prsAxis);
	if(sc->GetLinearKeys(zCont,IGAME_POS_Z))
		DumpLinearKeys(IGAME_POS_Z,zCont,prsAxis);
}

void IGameExporter::DumpEulerController(IGameControl * sc,CComPtr<IXMLDOMNode> prsNode)
{
	TSTR data;
	CComPtr <IXMLDOMNode> eulerNode = NULL;

	IGameKeyTab xCont,yCont,zCont;

	if(sc->GetBezierKeys(xCont,IGAME_EULER_X))
	{
		if(eulerNode ==NULL)
			CreateXMLNode(pXMLDoc,prsNode,_T("EulerController"),&eulerNode);

		DumpBezierKeys(IGAME_EULER_X,xCont,eulerNode);
	}

	if(sc->GetBezierKeys(yCont,IGAME_EULER_Y))
	{
		if(eulerNode ==NULL)
			CreateXMLNode(pXMLDoc,prsNode,_T("EulerController"),&eulerNode);
		
		DumpBezierKeys(IGAME_EULER_Y,yCont,eulerNode);
		
	}

	if(sc->GetBezierKeys(zCont,IGAME_EULER_Z))
	{
		if(eulerNode ==NULL)
			CreateXMLNode(pXMLDoc,prsNode,_T("EulerController"),&eulerNode);
			
		DumpBezierKeys(IGAME_EULER_Z,zCont,eulerNode);
		
	}
	if(sc->GetLinearKeys(xCont,IGAME_EULER_X))
	{
		if(eulerNode ==NULL)
			CreateXMLNode(pXMLDoc,prsNode,_T("EulerController"),&eulerNode);
	
		DumpLinearKeys(IGAME_EULER_X,xCont,eulerNode);

	}

	if(sc->GetLinearKeys(yCont,IGAME_EULER_Y))
	{
		if(eulerNode ==NULL)
			CreateXMLNode(pXMLDoc,prsNode,_T("EulerController"),&eulerNode);
		
		DumpLinearKeys(IGAME_EULER_Y,yCont,eulerNode);
		
	}

	if(sc->GetLinearKeys(zCont,IGAME_EULER_Z))
	{
		if(eulerNode ==NULL)
			CreateXMLNode(pXMLDoc,prsNode,_T("EulerController"),&eulerNode);
		
		DumpLinearKeys(IGAME_EULER_Z,zCont,eulerNode);
	}

}

void IGameExporter::DumpListController(IGameControl * sc,CComPtr<IXMLDOMNode> prsNode)
{
	TSTR data;
	CComPtr <IXMLDOMNode> posListNode,rotListNode;
	int subNum;
	subNum = sc->GetNumOfListSubControls(IGAME_POS);
	if(subNum)
	{
		data.printf(_T("%d"),subNum);
		CreateXMLNode(pXMLDoc,prsNode,_T("PositionListController"),&posListNode);
		AddXMLAttribute(posListNode,_T("NumSubController"),data.data());
		for(int i=0;i<subNum;i++)
		{
			IGameKeyTab SubCont;
			IGameControl * sub = sc->GetListSubControl(i,IGAME_POS);
			ExportPositionControllers(posListNode,sub,Point3(0,0,0));

		}
	}
	subNum = sc->GetNumOfListSubControls(IGAME_ROT);
	if(subNum)
	{
		data.printf(_T("%d"),subNum);
		CreateXMLNode(pXMLDoc,prsNode,_T("RotationListController"),&rotListNode);
		AddXMLAttribute(rotListNode,_T("NumSubController"),data.data());
		for(int i=0;i<subNum;i++)
		{
			IGameKeyTab SubCont;
			IGameControl * sub = sc->GetListSubControl(i,IGAME_POS);
			ExportRotationControllers(rotListNode,sub);

		}
	}
}

void IGameExporter::DumpSampleKeys(IGameControl * sc,CComPtr<IXMLDOMNode> prsNode, IGameControlType Type, bool quick)
{
	Tab<Matrix3>sKey;  
	Tab<GMatrix>gKey;
	IGameKeyTab Key;
	IGameControl * c = sc;
	CComPtr<IXMLDOMNode> sampleData;
	TSTR Buf;
	bool relative = false;

	TSTR name;
	GetKeyTypeName(name,Type);
	sampleData = NULL;

	if(!c)
		return;

	relative = exportRelative ? true : false;

	if(!quick && c->GetFullSampledKeys(Key,framePerSample,IGameControlType(Type),relative) )
	{

		CreateXMLNode(pXMLDoc,prsNode,name,&sampleData);
		Buf.printf(_T("%d"),Key.Count());
		AddXMLAttribute(sampleData,_T("KeyCount"),Buf.data());
		AddXMLAttribute(sampleData,_T("Type"),_T("FullSampled"));
		Buf.printf(_T("%d"),framePerSample);
		AddXMLAttribute(sampleData,_T("SampleRate"),Buf.data());

		for(int i=0;i<Key.Count();i++)
		{
			CComPtr<IXMLDOMNode> data;
			CreateXMLNode(pXMLDoc,sampleData,_T("Sample"),&data);
			int fc = Key[i].t;
			Buf.printf(_T("%d"),fc);
			AddXMLAttribute(data,_T("frame"),Buf.data());

			if(Type ==IGAME_POS)
			{
				Point3 k = Key[i].sampleKey.pval;
				Buf.printf(_T("%f %f %f"),k.x,k.y,k.z); 
				AddXMLText(pXMLDoc,data,Buf.data());
			}
			if(Type == IGAME_ROT)
			{
				Quat q = Key[i].sampleKey.qval;
				AngAxis a(q);
				if(!exportQuaternions)
					Buf.printf(_T("%f %f %f %f"),a.axis.x, a.axis.y, a.axis.z, a.angle);
				else
					Buf.printf(_T("%f %f %f %f"),q.x,q.y, q.z, q.w);

				AddXMLText(pXMLDoc,data,Buf.data());

			}
			if(Type == IGAME_SCALE)
			{
				ScaleValue sval = Key[i].sampleKey.sval;
				Point3 s = sval.s;
				AngAxis a(sval.q);
				if(!exportQuaternions)
					Buf.printf(_T("%f %f %f %f %f %f %f"),sval.s.x,sval.s.y,sval.s.z,a.axis.x, a.axis.y, a.axis.z, a.angle);
				else
					Buf.printf(_T("%f %f %f %f %f %f %f"),sval.s.x,sval.s.y,sval.s.z,sval.q.x, sval.q.y,sval.q.z, sval.q.w);
				AddXMLText(pXMLDoc,data,Buf.data());

			}
			if(Type == IGAME_FLOAT)
			{
				float f = Key[i].sampleKey.fval;
				Buf.printf(_T("%f"),f);	
				AddXMLText(pXMLDoc,data,Buf.data());
			}
			if(Type == IGAME_POINT3)
			{
				Point3 k = Key[i].sampleKey.pval;
				Buf.printf(_T("%f %f %f"),k.x,k.y,k.z); 
				AddXMLText(pXMLDoc,data,Buf.data());
			}
			if(Type == IGAME_TM)
			{
				//Even though its a 4x4 we dump it as a 4x3 ;-)
				DumpMatrix(Key[i].sampleKey.gval.ExtractMatrix3(),data);
			}
		}
	}

	//mainly for the IK On/Off controller
	if(quick && c->GetQuickSampledKeys(Key,IGameControlType(Type)) )
	{

		CreateXMLNode(pXMLDoc,prsNode,name,&sampleData);
		Buf.printf(_T("%d"),Key.Count());
		AddXMLAttribute(sampleData,_T("KeyCount"),Buf.data());
		AddXMLAttribute(sampleData,_T("Type"),_T("QuickSampled"));

		for(int i=0;i<Key.Count();i++)
		{
			CComPtr<IXMLDOMNode> data;
			CreateXMLNode(pXMLDoc,sampleData,_T("Sample"),&data);
			int fc = Key[i].t;
			Buf.printf(_T("%d"),fc);
			AddXMLAttribute(data,_T("frame"),Buf.data());
			if(Type == IGAME_FLOAT)
			{
				float f = Key[i].sampleKey.fval;
				Buf.printf(_T("%f"),f);	
				AddXMLText(pXMLDoc,data,Buf.data());
			}

		}
	}

}
void IGameExporter::DumpSkin(CComPtr<IXMLDOMNode> modNode, IGameSkin * s)
{
	CComPtr <IXMLDOMNode> skinNode;
	TSTR Buf;

	if(s->GetSkinType()== IGameSkin::IGAME_PHYSIQUE)
		Buf.printf(_T("%s"),_T("Physique"));
	else
		Buf.printf(_T("%s"),_T("MaxSkin"));

	AddXMLAttribute(modNode,_T("SkinType"),Buf.data());

	GMatrix skinTM;

	s->GetInitSkinTM(skinTM);

	for(int x=0; x<s->GetNumOfSkinnedVerts();x++)
	{
		int type = s->GetVertexType(x);
		if(type==IGameSkin::IGAME_RIGID)
		{
			CComPtr <IXMLDOMNode> boneNode;
			skinNode = NULL;
			CreateXMLNode(pXMLDoc,modNode,_T("Skin"),&skinNode);
			Buf.printf(_T("%d"),x);
			AddXMLAttribute(skinNode,_T("VertexID"),Buf.data());
			AddXMLAttribute(skinNode,_T("Type"),_T("Rigid"));
			CreateXMLNode(pXMLDoc,skinNode,_T("Bone"),&boneNode);
			ULONG id = s->GetBoneID(x,0);
			Buf.printf(_T("%d"),id);
			AddXMLAttribute(boneNode,_T("BoneID"),Buf.data());

		}
		else //blended
		{
			CComPtr <IXMLDOMNode> boneNode;
			skinNode = NULL;
			CreateXMLNode(pXMLDoc,modNode,_T("Skin"),&skinNode);
			Buf.printf(_T("%d"),x);
			AddXMLAttribute(skinNode,_T("VertexID"),Buf.data());
			AddXMLAttribute(skinNode,_T("Type"),_T("Blended"));

			for(int y=0;y<s->GetNumberOfBones(x);y++)
			{
				boneNode = NULL;
				CreateXMLNode(pXMLDoc,skinNode,_T("Bone"),&boneNode);
				ULONG id = s->GetBoneID(x,y);
				Buf.printf(_T("%d"),id);
				AddXMLAttribute(boneNode,_T("BoneID"),Buf.data());
				float weight = s->GetWeight(x,y);
				Buf.printf(_T("%f"),weight);
				AddXMLAttribute(boneNode,_T("Weight"),Buf.data());
			}
		}
	}
}

void IGameExporter::DumpModifiers(CComPtr<IXMLDOMNode> modNode, IGameModifier * m)
{
	CComPtr <IXMLDOMNode> propNode;
	TSTR buf;
	if(exportSkin || exportGenMod)
	{
	
		CreateXMLNode(pXMLDoc,modNode,_T("Modifier"),&propNode);
		AddXMLAttribute(propNode,_T("modName"),m->GetInternalName());
		bool bS = m->IsSkin();
		if(bS)
			buf.printf(_T("true"));
		else
			buf.printf(_T("false"));
		AddXMLAttribute(propNode,_T("IsSkin"),buf.data());
		
		if(m->IsSkin() && exportSkin)
		{
			IGameSkin * skin = (IGameSkin*)m;
			DumpSkin(propNode,skin);
		}
	}

}

void IGameExporter::DumpLight(IGameLight *lt, CComPtr<IXMLDOMNode> parent)
{
	if(lt->GetLightType()==TSPOT_LIGHT )
	{
		TSTR buf;
		buf.printf(_T("%d"),lt->GetLightTarget()->GetNodeID());
		AddXMLAttribute(parent,_T("TargetID"),buf.data());
	}

	DumpNamedProperty(parent, _T("Color"), lt->GetLightColor());
	DumpNamedProperty(parent, _T("Multiplier"), lt->GetLightMultiplier());
	DumpNamedProperty(parent, _T("AspectRatio"), lt->GetLightAspectRatio());
	DumpNamedProperty(parent, _T("Start"), lt->GetLightAttenStart());
	DumpNamedProperty(parent, _T("End"), lt->GetLightAttenEnd());
	DumpNamedProperty(parent, _T("FallOff"), lt->GetLightFallOff());
}

void IGameExporter::DumpCamera(IGameCamera *ca, CComPtr<IXMLDOMNode> parent)
{
	if(ca->GetCameraTarget())
	{
		TSTR buf;
		buf.printf(_T("%d"),ca->GetCameraTarget()->GetNodeID());
		AddXMLAttribute(parent,_T("TargetID"),buf.data());
	}
	
	DumpNamedProperty( parent, _T("FOV"), ca->GetCameraFOV());
	DumpNamedProperty( parent, _T("FarClip"), ca->GetCameraFarClip());
	DumpNamedProperty( parent, _T("NearClip"), ca->GetCameraNearClip());
	DumpNamedProperty( parent, _T("TargetDistance"), ca->GetCameraTargetDist());
}

void IGameExporter::DumpIKChain(IGameIKChain * ikch, CComPtr<IXMLDOMNode> ikData)
{
	CComPtr <IXMLDOMNode> ikRoot,ikNode, ikEnabled;
	TSTR buf;

	CreateXMLNode(pXMLDoc,ikData,_T("IKNodes"),&ikRoot);
	buf.printf(_T("%d"),ikch->GetNumberofBonesinChain());
	AddXMLAttribute(ikRoot,_T("NumOfNodesInChain"),buf.data());

	for(int i=0;i<ikch->GetNumberofBonesinChain();i++)
	{
		ikNode = NULL;
		IGameNode * node = ikch->GetIGameNodeInChain(i);
		CreateXMLNode(pXMLDoc,ikRoot,_T("ChainNode"),&ikNode);
		buf.printf(_T("%d"),node->GetNodeID());
		AddXMLAttribute(ikNode,_T("NodeID"),buf.data());
		
	}

	IGameControl * cont = ikch->GetIKEnabledController();
	if(cont)
	{
		CreateXMLNode(pXMLDoc,ikData,_T("IKEnabled"),&ikEnabled);
		DumpSampleKeys(cont,ikEnabled,IGAME_FLOAT,true);
	}

	
}

void IGameExporter::DumpSpline(IGameSpline * sp,CComPtr<IXMLDOMNode> splineData)
{
	CComPtr <IXMLDOMNode> spline,knotData;
	TSTR buf;

	buf.printf(_T("%d"),sp->GetNumberOfSplines());
	AddXMLAttribute(splineData,_T("NumOfSplines"),buf.data());

	for(int i=0;i<sp->GetNumberOfSplines();i++)
	{
		IGameSpline3D * sp3d = sp->GetIGameSpline3D(i);
		spline=NULL;
		CreateXMLNode(pXMLDoc,splineData,_T("Spline"),&spline);
		buf.printf(_T("%d"),i+1);
		AddXMLAttribute(spline,_T("index"),buf.data());
		int num = sp3d->GetIGameKnotCount();
		buf.printf(_T("%d"),num);
		AddXMLAttribute(spline,_T("NumOfKnots"),buf.data());
		for(int j=0;j<num;j++)
		{
			TSTR data;
			Point3 v;
			CComPtr <IXMLDOMNode> point,invec,outvec;
			knotData=NULL;
			IGameKnot * knot = sp3d->GetIGameKnot(j);
			CreateXMLNode(pXMLDoc,spline,_T("knot"),&knotData);
			CreateXMLNode(pXMLDoc,knotData,_T("Point"),&point);
			v = knot->GetKnotPoint();
			if(exportObjectSpace)
			{
				GMatrix g = sp->GetIGameObjectTM();
				Matrix3 gm = g.ExtractMatrix3();
				Matrix3 invgm = Inverse(gm);
				v = v * invgm;
			}
			data.printf(_T("%f %f %f"),v.x,v.y,v.z);
			AddXMLText(pXMLDoc,point,data.data());
			CreateXMLNode(pXMLDoc,knotData,_T("inVec"),&invec);
			v = knot->GetInVec();
			if(exportObjectSpace)
			{
				GMatrix g = sp->GetIGameObjectTM();
				Matrix3 gm = g.ExtractMatrix3();
				Matrix3 invgm = Inverse(gm);
				v = v * invgm;
			}
			data.printf(_T("%f %f %f"),v.x,v.y,v.z);
			AddXMLText(pXMLDoc,invec,data.data());
			CreateXMLNode(pXMLDoc,knotData,_T("outVec"),&outvec);
			v = knot->GetOutVec();
			if(exportObjectSpace)
			{
				GMatrix g = sp->GetIGameObjectTM();
				Matrix3 gm = g.ExtractMatrix3();
				Matrix3 invgm = Inverse(gm);
				v = v * invgm;
			}
			data.printf(_T("%f %f %f"),v.x,v.y,v.z);
			AddXMLText(pXMLDoc,outvec,data.data());

		}
	}

	IPropertyContainer * cc = sp->GetIPropertyContainer();
	IGameProperty * prop = cc->QueryProperty(_T("IGameTestString"));
	prop = cc->QueryProperty(_T("IGameTestString"));

	if(prop)
	{
		const TCHAR * name;
		prop->GetPropertyValue(name);

	}

		

}

void IGameExporter::DumpMesh(IGameMesh * gm,CComPtr<IXMLDOMNode> geomData)
{
	CComPtr <IXMLDOMNode> vertData,faceData;
	CComPtr <IXMLDOMNode> node;
	TSTR buf;

	pXMLDoc->createNode(CComVariant(NODE_ELEMENT), CComBSTR(_T("Faces")), NULL, &faceData);
	int numFaces = gm->GetNumberOfFaces();
	buf.printf(_T("%d"),numFaces);
	AddXMLAttribute(faceData,_T("Count"),buf.data());

	// Create 'Vertices' element to declare vertex positions
	CreateXMLNode(pXMLDoc,geomData,_T("Vertices"),&vertData);
	int numVerts = gm->GetNumberOfVerts();
	buf.printf(_T("%d"),numVerts);
	AddXMLAttribute(vertData,_T("Count"),buf.data());
	
	// WARNING: the order of creation of the text nodes is important to match
	// the schema. First vertices, then normals, etc

	// Create 'vertices' element of 'Faces' element to declare vertex indices
	CComPtr<IXMLDOMNode> vertices;
	CreateXMLNode(pXMLDoc,faceData,_T("FaceVertices"),&vertices);
	

	// dump Vertices
	for(int i = 0;i<numVerts;i++)
	{
		Point3 v; 
		if(gm->GetVertex(i,v, (exportObjectSpace != 0)))
			{
			buf.printf(_T("%f %f %f "),v.x,v.y,v.z);
			AddXMLText(pXMLDoc,vertData,buf.data());
		}
		}
			
	// TODO: Export Vertex selections
	// TODO: Export Vertex hide

	CComPtr<IXMLDOMNode> normals;
	if(exportNormals && !exportNormalsPerFace && gm->GetNumberOfNormals() > 0)
	{
		// dump Normals
		CComPtr <IXMLDOMNode> normData;
		int numNorms = gm->GetNumberOfNormals();
			CreateXMLNode(pXMLDoc,geomData,_T("Normals"),&normData);
			buf.printf(_T("%d"),numNorms);
			AddXMLAttribute(normData,_T("Count"),buf.data());

		// Create 'normals' element of 'Faces' element to declare normal indices
		CreateXMLNode(pXMLDoc,faceData,_T("FaceNormals"),&normals);
		
		for(int i = 0;i<numNorms;i++)
		{
			Point3 n;
			if(gm->GetNormal(i,n, (exportObjectSpace != 0)))
			{
				buf.printf(_T("%f %f %f "),n.x,n.y,n.z);
				AddXMLText(pXMLDoc,normData,buf.data());
			}
		}
	}

	// TODO: Export Vertex weights
	// TODO: Export soft selection

	Tab<int> matidTab = gm->GetActiveMatIDs();
	CComPtr<IXMLDOMNode> matids;

	if (matidTab.Count() > 0)
		{
		CreateXMLNode(pXMLDoc,faceData,_T("MaterialIDs"),&matids);
	}

	Tab <DWORD> smgrps = gm->GetActiveSmgrps();
	CComPtr<IXMLDOMNode> smgroups;
	if( smgrps.Count() > 0)
		CreateXMLNode(pXMLDoc,faceData,_T("SmoothingGroups"),&smgroups);

	CComPtr<IXMLDOMNode> edges;
	CreateXMLNode(pXMLDoc,faceData,_T("EdgeVisibility"),&edges);

	// dump Face data
	geomData->appendChild(faceData,NULL);
	for(int n=0;n<numFaces;n++)
	{
		FaceEx* f = gm->GetFace(n);

		if(vertices != NULL) {
			buf.printf(_T(" %d %d %d"), f->vert[0], f->vert[1], f->vert[2]);
			AddXMLText(pXMLDoc,vertices,buf.data());
		}

		if(normals != NULL) {
			buf.printf(_T(" %d %d %d"), f->norm[0], f->norm[1], f->norm[2]);
			AddXMLText(pXMLDoc,normals,buf.data());
			}

		if(smgroups != NULL) {
			buf.printf(_T(" %u"),(unsigned int)f->smGrp);
			AddXMLText(pXMLDoc,smgroups,buf.data());
		}

		if(matids != NULL) {
			buf.printf(_T(" %d"), f->matID);
			AddXMLText(pXMLDoc,matids,buf.data());
		}
		
		if(edges != NULL) {
			buf.printf(_T(" %d %d %d"), f->edgeVis[0], f->edgeVis[1], f->edgeVis[2] );
			AddXMLText(pXMLDoc,edges,buf.data());
		}
        



		// TODO: Export Face selection data
		// TODO: Export Face hide
		// TODO: Export edge selection
		
	}

	if(exportMappingChannel)
	{
		Tab<int> mapNums = gm->GetActiveMapChannelNum();
		int mapCount = mapNums.Count();

		if( mapCount > 0)
		{
		TSTR data;
		CComPtr <IXMLDOMNode> channelNode;
		CreateXMLNode(pXMLDoc,geomData,_T("MapChannels"),&channelNode);
		buf.printf(_T("%d"),mapCount);
		AddXMLAttribute(channelNode,_T("Count"),buf.data());

		for(int i=0;i < mapCount;i++)
		{
			CComPtr <IXMLDOMNode> channelItem, mvertData,vert,mfaceData,face;
			CreateXMLNode(pXMLDoc,channelNode,_T("MapChannel"),&channelItem);
			buf.printf(_T("%d"),mapNums[i]);
				AddXMLAttribute(channelItem,_T("ID"),buf.data());
				AddXMLAttribute(channelItem,_T("Type"),_T("Texture"));
			// TODO: Implement name attribute for channel

			CreateXMLNode(pXMLDoc,channelItem,_T("MapVertices"),&mvertData);
			int vCount = gm->GetNumberOfMapVerts(mapNums[i]);
			buf.printf(_T("%d"),vCount);
			AddXMLAttribute(mvertData,_T("Count"),buf.data());
			// TODO: Set proper map vertices dimension
//			int vDim = gm->GetDimensionOfMapVerts(mapNums[i]);
//			buf.printf("%d",vDim);
			AddXMLAttribute(mvertData,_T("Dimension"),_T("3"));

			for(int j=0;j<vCount;j++)
			{
				Point3 v;
				if(gm->GetMapVertex(mapNums[i],j,v))
				{
					data.printf(_T("%f %f %f"),v.x,v.y,v.z);
					AddXMLText(pXMLDoc,mvertData,data.data());
				}
		
			}

			CreateXMLNode(pXMLDoc,channelItem,_T("MapFaces"),&mfaceData);
			int fCount = gm->GetNumberOfFaces();
			buf.printf(_T("%d"),fCount);
			AddXMLAttribute(mfaceData,_T("Count"),buf.data());

			for(int k=0;k<fCount;k++)
			{
				DWORD  v[3];
				gm->GetMapFaceIndex(mapNums[i],k,v);
				data.printf(_T("%d %d %d"),v[0],v[1],v[2]);
				AddXMLText(pXMLDoc,mfaceData,data.data());
				}
			}
		}

	}

#if 0
//test code
	Tab<int> matids;

	matids = gm->GetActiveMatIDs();

	for(i=0;i<matids.Count();i++)
	{
		Tab<FaceEx*> faces;

		faces = gm->GetFacesFromMatID(matids[i]);

		for(int k=0; k<faces.Count();k++)
		{
			IGameMaterial * faceMat = gm->GetMaterialFromFace(faces[k]);
//			TSTR name(faceMat->GetMaterialName());
		}
		for(k=0;k<gm->GetNumberOfFaces();k++)
		{
			IGameMaterial * faceMat = gm->GetMaterialFromFace(k);
		}
		
	}

	Tab <DWORD> smgrps = gm->GetActiveSmgrps();
#endif
}



void IGameExporter::DumpConstraints(CComPtr<IXMLDOMNode> prsData, IGameConstraint * c)
{
	TSTR buf,name;
	CComPtr <IXMLDOMNode> prsChild;
	CComPtr <IXMLDOMNode> constNode;

	if(exportConstraints)
	{
	
		CreateXMLNode(pXMLDoc,prsData,_T("Constrainst"),&prsChild);
		GetConstraintTypeName(name, c->GetConstraintType());
		AddXMLAttribute(prsChild,_T("Type"),name.data());
		buf.printf(_T("%d"),c->NumberOfConstraintNodes());
		AddXMLAttribute(prsChild,_T("NodeCount"),buf.data());

		for(int i=0;i<c->NumberOfConstraintNodes();i++ )
		{
			constNode = NULL;
			CreateXMLNode(pXMLDoc,prsChild,_T("ConstNode"),&constNode);
			float w = c->GetConstraintWeight(i);
			int id = c->GetConstraintNodes(i)->GetNodeID();
			
			buf.printf(_T("%d"), id);
			AddXMLAttribute(constNode,_T("NodeID"),buf.data());

			buf.printf(_T("%f"), w);
			AddXMLAttribute(constNode,_T("Weight"),buf.data());

		}
      for(int i=0;i<c->GetIPropertyContainer()->GetNumberOfProperties();i++)
		{
			DumpProperties(prsChild,c->GetIPropertyContainer()->GetProperty(i));
		}
	}
}




void IGameExporter::DumpBezierKeys(IGameControlType Type, IGameKeyTab Keys, CComPtr<IXMLDOMNode> prsData)
{
	TSTR buf;
	CComPtr <IXMLDOMNode> prsChild;
	GetKeyTypeName(buf,Type);

	if(Keys.Count()==0)
		return;

	CreateXMLNode(pXMLDoc,prsData,buf,&prsChild);

	buf.printf(_T("%d"),Keys.Count());
	AddXMLAttribute(prsChild,_T("Count"),buf.data());
	AddXMLAttribute(prsChild,_T("Type"),_T("Bezier"));

	for(int i = 0;i<Keys.Count();i++)
	{
		if(Type==IGAME_POS || Type==IGAME_POINT3)
		{
			Point3 k = Keys[i].bezierKey.pval;
			buf.printf(_T("%d %f %f %f"),Keys[i].t,k.x,k.y,k.z); 
		}
		else if (Type == IGAME_ROT)
		{
			Quat q = Keys[i].bezierKey.qval;
			if(!exportQuaternions)
			{
			AngAxis a(q);
				buf.printf (_T("%d %f %f %f %f"),Keys[i].t, a.axis.x, a.axis.y, a.axis.z, a.angle);
			}
			else
				buf.printf (_T("%d %f %f %f %f"),Keys[i].t, q.x, q.y, q.z, q.w);
		}
		else if ( IsFloatController(Type))
		{
			float f = Keys[i].bezierKey.fval;
			buf.printf(_T("%d %f"),Keys[i].t,f);
		}
		else{
			ScaleValue sval = Keys[i].bezierKey.sval;
			Point3 s = sval.s;
			AngAxis a(sval.q);
			if(!exportQuaternions)
				buf.printf(_T("%d %f %f %f %f %f %f %f"),Keys[i].t,sval.s.x,sval.s.y,sval.s.z,a.axis.x, a.axis.y, a.axis.z, a.angle);
			else
				buf.printf(_T("%d %f %f %f %f %f %f %f"),Keys[i].t,sval.s.x,sval.s.y,sval.s.z,sval.q.x, sval.q.y,sval.q.z, sval.q.w);

		}
		AddXMLText(pXMLDoc,prsChild,buf.data());
	}
}

#define GET_INDEX_VAL( _v, _i) ((_i==0)?_v.x:((_i==1)?_v.y:_v.z))

void IGameExporter::DumpBezierFloatKeys(IGameControlType Type, IGameKeyTab Keys, int index, CComPtr<IXMLDOMNode> prsData)
{
	TSTR buf;
	CComPtr <IXMLDOMNode> prsChild;

	CreateXMLNode(pXMLDoc,prsData,_T("BezierFloatController"),&prsChild);

	int count = Keys.Count();
	buf.printf(_T("%d"), count);
	AddXMLAttribute(prsChild,_T("count"),buf.data());

	for (int i = 0; i < count ; i++ )
	{
		float value;
		if(Type==IGAME_POS || Type==IGAME_POINT3)
		{
			value = GET_INDEX_VAL(Keys[i].bezierKey.pval, index);;
		}
		else if (Type == IGAME_ROT)
		{
			Quat q = Keys[i].bezierKey.qval;
			if(!exportQuaternions)
			{
				AngAxis a(q);
				value = a.angle;
			}
			else
				value = GET_INDEX_VAL(Keys[i].bezierKey.qval, index);
		}
		else if ( IsFloatController(Type))
		{
			value = Keys[i].bezierKey.fval;
		}
			else
			value = GET_INDEX_VAL(Keys[i].bezierKey.sval.s,index);

		CComPtr <IXMLDOMNode> kNode;
		CreateXMLNode(pXMLDoc,prsChild,_T("Key"),&kNode);
		
		buf.printf(_T("%d"),Keys[i].t); 
		AddXMLAttribute(kNode,_T("time"),buf.data());

		buf.printf(_T("%f"),value); 
		AddXMLAttribute(kNode,_T("value"),buf.data());
	}
		}
		
void IGameExporter::DumpBezierPosKeys(IGameControlType Type, IGameKeyTab Keys, Point3 pos, CComPtr<IXMLDOMNode> prsData)
{
	TSTR buf;
	CComPtr <IXMLDOMNode> prsChild;
	CComPtr <IXMLDOMNode> prsAxis;

	buf.printf(_T("%f %f %f"), pos.x, pos.y, pos.z);
	AddXMLAttribute(prsData,_T("Value"),buf.data());

	CreateXMLNode(pXMLDoc,prsData,_T("PositionXYZController"),&prsChild);

	CreateXMLNode(pXMLDoc,prsChild,_T("X"),&prsAxis);
	DumpBezierFloatKeys(Type, Keys, 0, prsAxis);
	buf.printf(_T("%f"), pos.x);
	AddXMLAttribute(prsAxis,_T("Value"),buf.data());

	prsAxis = NULL;
	CreateXMLNode(pXMLDoc,prsChild,_T("Y"),&prsAxis);
	buf.printf(_T("%f"), pos.x);
	AddXMLAttribute(prsAxis,_T("Value"),buf.data());
	DumpBezierFloatKeys(Type, Keys, 1, prsAxis);

	prsAxis = NULL;
	CreateXMLNode(pXMLDoc,prsChild,_T("Z"),&prsAxis);
	buf.printf(_T("%f"), pos.x);
	AddXMLAttribute(prsAxis,_T("Value"),buf.data());
	DumpBezierFloatKeys(Type, Keys, 2, prsAxis);
	}

void IGameExporter::DumpBezierRotKeys(IGameKeyTab Keys, CComPtr<IXMLDOMNode> prsData)
{
}

void IGameExporter::DumpBezierSclKeys(IGameKeyTab Keys, CComPtr<IXMLDOMNode> prsData)
{
}

void IGameExporter::DumpLinearKeys(IGameControlType Type, IGameKeyTab Keys, CComPtr<IXMLDOMNode> prsData)
{
	TSTR buf;
   CComPtr <IXMLDOMNode> prsChild;
	GetKeyTypeName(buf,Type);

	if(Keys.Count()==0)
		return;

	CreateXMLNode(pXMLDoc,prsData,buf.data(),&prsChild);

	buf.printf(_T("%d"),Keys.Count());
	AddXMLAttribute(prsChild,_T("KeyCount"),buf.data());
	AddXMLAttribute(prsChild,_T("Type"),_T("Linear"));

	for(int i = 0;i<Keys.Count();i++)
	{
		if(Type==IGAME_POS)
		{
			Point3 k = Keys[i].linearKey.pval;
			buf.printf(_T("%d %f %f %f"),Keys[i].t,k.x,k.y,k.z); 
		}
		else if (Type == IGAME_ROT)
		{
			Quat q = Keys[i].linearKey.qval;
			if(!exportQuaternions) {
			AngAxis a(q);
				buf.printf (_T("%d %f %f %f %f"),Keys[i].t, a.axis.x, a.axis.y, a.axis.z, a.angle);
			} else buf.printf (_T("%d %f %f %f %f"),Keys[i].t, q.x, q.y, q.z, q.w);
		}
		else if ( IsFloatController(Type))
		{
			float f = Keys[i].linearKey.fval;
			buf.printf(_T("%d %f"),Keys[i].t,f);	
		}
		else{
			ScaleValue sval = Keys[i].linearKey.sval;
			Point3 s = sval.s;
			AngAxis a(sval.q);
			if(exportQuaternions)
				buf.printf(_T("%d %f %f %f %f %f %f %f"),Keys[i].t,sval.s.x,sval.s.y,sval.s.z,a.axis.x, a.axis.y, a.axis.z, a.angle);
			else
				buf.printf(_T("%d %f %f %f %f %f %f %f"),Keys[i].t,sval.s.x,sval.s.y,sval.s.z,sval.q.x, sval.q.y,sval.q.z, sval.q.w);

		}
		AddXMLText(pXMLDoc,prsChild,buf.data());
		
	}

	prsChild = NULL;


}
void IGameExporter::DumpTCBKeys(IGameControlType Type, IGameKeyTab Keys, CComPtr<IXMLDOMNode> prsData)
{
	TSTR buf;
	CComPtr <IXMLDOMNode> prsChild;

	if(Keys.Count()==0)
		return;

	GetKeyTypeName(buf,Type);
	CreateXMLNode(pXMLDoc,prsData,buf.data(),&prsChild);

	buf.printf(_T("%d"),Keys.Count());
	AddXMLAttribute(prsChild,_T("KeyCount"),buf.data());
	AddXMLAttribute(prsChild,_T("Type"),_T("TCB"));

	for(int i = 0;i<Keys.Count();i++)
	{
		CComPtr <IXMLDOMNode> key = NULL;
		TSTR data;
		CreateXMLNode(pXMLDoc,prsChild,_T("Key"),&key);
		buf.printf(_T("%d"),Keys[i].t);
		AddXMLAttribute(key,_T("time"),buf.data());
		if(Type==IGAME_POS)
		{
			Point3 k = Keys[i].tcbKey.pval;
			data.printf(_T("%f %f %f"),k.x,k.y,k.z); 
		}
		else if (Type == IGAME_ROT)
		{
			AngAxis a = Keys[i].tcbKey.aval;
			data.printf(_T("%f %f %f %f"),a.axis.x, a.axis.y, a.axis.z, a.angle);

		}
		else{
			ScaleValue sval = Keys[i].tcbKey.sval;
			Point3 s = sval.s;
			AngAxis a(sval.q);
			if(!exportQuaternions)
				data.printf(_T("%f %f %f %f %f %f %f"),sval.s.x,sval.s.y,sval.s.z,a.axis.x, a.axis.y, a.axis.z, a.angle);
			else
				data.printf(_T("%f %f %f %f %f %f %f"),sval.s.x,sval.s.y,sval.s.z,sval.q.x, sval.q.y,sval.q.z, sval.q.w);

		}
		AddXMLText(pXMLDoc,key,data.data());
		
	}

	prsChild = NULL;


}

void IGameExporter::DumpProperty(IGameProperty * prop, CComPtr <IXMLDOMNode> propData)
{
	TSTR Buf;
	IGameKeyTab keys;

	if(!prop)	//fix me NH...
		return;

	if(prop->GetType() == IGAME_POINT3_PROP)
	{
		Point3 p; 
		prop->GetPropertyValue(p);
		Buf.printf(_T("%f %f %f"),p.x,p.y,p.z);
	}
	else if( prop->GetType() == IGAME_FLOAT_PROP)
	{
		float f;
		prop->GetPropertyValue(f);
		Buf.printf(_T("%f"), f);
	}
	else if(prop->GetType()==IGAME_STRING_PROP)
	{
		const TCHAR * b;
		prop->GetPropertyValue(b);
		Buf.printf(_T("$s"),b);
	}
	else
	{
		int i;
		prop->GetPropertyValue(i);
		Buf.printf(_T("%d"), i);

	}
	AddXMLAttribute(propData,_T("Value"),Buf.data());

	if(prop->IsPropAnimated() && exportControllers)
	{
		IGameControl * c = prop->GetIGameControl();


		if(prop->GetType() == IGAME_POINT3_PROP)
		{
			if(	c->GetBezierKeys(keys,IGAME_POINT3 )){
				Point3 p; 
				prop->GetPropertyValue(p);
				DumpBezierPosKeys(IGAME_POINT3,keys,p,propData);
			}
		}
		if(prop->GetType()==IGAME_FLOAT_PROP)
		{
			if(	c->GetBezierKeys(keys,IGAME_FLOAT ))
				DumpBezierFloatKeys(IGAME_FLOAT, keys, 0, propData);
			}
		}

	}

void IGameExporter::DumpNamedProperty(CComPtr<IXMLDOMNode> node, TCHAR* propName, IGameProperty * prop)
{
	if(!prop)	//fix me NH...
		return;
	CComPtr <IXMLDOMNode> propData;
	CreateXMLNode(pXMLDoc, node, propName,&propData);
	DumpProperty( prop, propData);
	propData = NULL;
}	

void IGameExporter::DumpProperties(CComPtr<IXMLDOMNode> node, IGameProperty * prop)
{
	CComPtr <IXMLDOMNode> propData;

	if(!prop)	//fix me NH...
		return;
	CreateXMLNode(pXMLDoc,node,_T("Prop"),&propData);
	AddXMLAttribute(propData,_T("name"),prop->GetName());
	DumpProperty(prop, propData);
	propData = NULL;
}	


void IGameExporter::DumpTexture(CComPtr<IXMLDOMNode> node,IGameMaterial * mat)
{
	int texCount = mat->GetNumberOfTextureMaps();
	if(texCount==0) return;

	CComPtr <IXMLDOMNode> textureRoot;
	TSTR buf;

		CreateXMLNode(pXMLDoc,node,_T("TextureMaps"),&textureRoot);
	
		buf.printf(_T("%d"),texCount);
		AddXMLAttribute(textureRoot,_T("Count"),buf.data());
	
	for(int i=0;i<texCount;i++)
	{
		
		//test code
#if 0
		IGameTextureMap * pTex = mat->GetIGameTextureMap(i);
		IGameUVGen * pUVGen = pTex->GetIGameUVGen();
		IGameProperty * prop = pUVGen->GetUTilingData();
		TCHAR * propName = prop->GetName();
		float value = -1.0f;
		prop->GetPropertyValue(value);     
#endif
		
		
		
		
		CComPtr <IXMLDOMNode> texture;
		IGameTextureMap * tex = mat->GetIGameTextureMap(i);

		CreateXMLNode(pXMLDoc,textureRoot,_T("Texture"),&texture);

		buf.printf(_T("%d"),i);
		AddXMLAttribute(texture,_T("Index"),buf.data());

        AddXMLAttribute(texture,_T("Name"),tex->GetTextureName());

		int mapSlot = tex->GetStdMapSlot();
		AddXMLAttribute(texture,_T("Slot"),(mapSlot == -1)?_T("Unknown"):mapSlotNames[mapSlot]);

		//TODO: Add attributes 'Source', 'UAddress', 'VAddress', 'WAddress', 'MapChannel'

		if(tex->IsEntitySupported())	//its a bitmap texture
		{
			// TODO: Add elements MapStrength, MapOffset, ...

			CComPtr <IXMLDOMNode> bitmapTexture;
			CreateXMLNode(pXMLDoc,texture,_T("BitmapTexture"),&bitmapTexture);

			TSTR str = tex->GetBitmapFileName();
			makeValidURIFilename(str);
			AddXMLAttribute(bitmapTexture,_T("Filename"),str);

			DumpNamedProperty( bitmapTexture, _T("ClipU"), tex->GetClipUData());
			DumpNamedProperty( bitmapTexture, _T("ClipV"), tex->GetClipVData());
			DumpNamedProperty( bitmapTexture, _T("ClipW"), tex->GetClipWData());
			DumpNamedProperty( bitmapTexture, _T("ClipH"), tex->GetClipHData());
		}

	}

}


void IGameExporter::DumpFXProperties(CComPtr<IXMLDOMNode> node, IGameFXProperty * fxprop)
{
	TSTR buf;
	int numberOfAnnotations;          // numberOfTechniques, numberOfPipes;
	CComPtr <IXMLDOMNode> fxAnnotRoot, fxAnnotData,fxPropName;

	CreateXMLNode(pXMLDoc,node,_T("ParamName"),&fxPropName);
	AddXMLText(pXMLDoc,fxPropName,(TCHAR*)fxprop->GetPropertyName());

	numberOfAnnotations = fxprop->GetNumberOfFXAnnotations();
	buf.printf(_T("%d"),numberOfAnnotations);
	CreateXMLNode(pXMLDoc,node,_T("FXAnnotations"), &fxAnnotRoot);
	AddXMLAttribute(fxAnnotRoot,_T("Count"),buf.data() );
	AddXMLAttribute(fxAnnotRoot,_T("Type"),(TCHAR*)fxprop->GetPropertyType());
	AddXMLAttribute(fxAnnotRoot,_T("DefaultValue"),(TCHAR*)fxprop->GetFXDefaultValue());

   for(int i=0; i< numberOfAnnotations;i++)
   {
      CComPtr < IXMLDOMNode> type = NULL;
      CComPtr < IXMLDOMNode> name = NULL;
      CComPtr < IXMLDOMNode> value = NULL;
      CComPtr < IXMLDOMNode> key = NULL;

      TSTR strType, strKey,strValue;
      fxprop->GetFXAnnotation(i,&strType,&strKey,&strValue);
      CreateXMLNode(pXMLDoc,fxAnnotRoot,_T("AnnotationData"), &fxAnnotData);

      CreateXMLNode(pXMLDoc, fxAnnotData, _T("Type"),&type);
      AddXMLText(pXMLDoc,type,strType.data());
      CreateXMLNode(pXMLDoc, fxAnnotData, _T("key"),&key);
      AddXMLText(pXMLDoc,key,strKey.data());
      CreateXMLNode(pXMLDoc, fxAnnotData, _T("value"),&value);
      AddXMLText(pXMLDoc,value,strValue.data());
      fxAnnotData = NULL;

      }
		}



static void GetProfile(IGameFX::IGameFXProfile target, bool vertexprog, TCHAR * profile)
{
//	TCHAR buf [20];

	switch (target)
	{
	case IGameFX::kVSPS_20:
		if(vertexprog)
			_tcscpy(profile,_T("VS_2_0"));
		else
			_tcscpy(profile,_T("PS_2_0"));
		break;
	case IGameFX::kVSPS_2X:
		if(vertexprog)
			_tcscpy(profile,_T("vs_2_a"));
		else
			_tcscpy(profile,_T("ps_2_a"));
		break;
	case IGameFX::kVSPS_30:
		if(vertexprog)
			_tcscpy(profile,_T("vs_3_0"));
		else
			_tcscpy(profile,_T("ps_3_0"));
		break;

	default:
		if(vertexprog)
			_tcscpy(profile,_T("vs_2_0"));
		else
			_tcscpy(profile,_T("ps_2_0"));
		break;
	}

}

void IGameExporter::DumpMaterial(CComPtr<IXMLDOMNode> node,IGameMaterial * mat, int index, int matID )
{
	TSTR buf;
	TCHAR profile[20];
	IGameProperty *prop;
	CComPtr <IXMLDOMNode> material;
	CComPtr <IXMLDOMNode> propNode;

	CreateXMLNode(pXMLDoc,node,_T("Material"),&material);
	buf.printf(_T("%d"),index);
	AddXMLAttribute(material,_T("Index"),buf.data());
	

	if(matID !=-1)
	{
		// we are a sub material
		buf.printf(_T("%d"),matID);
		AddXMLAttribute(material,_T("MaterialID"),buf.data());
	}

	buf = mat->GetMaterialName();
	AddXMLAttribute(material,_T("Name"), FixupName (buf.dataForWrite()));

	buf = mat->GetMaterialClass();
	AddXMLAttribute (material, _T("Class"), FixupName (buf.dataForWrite()));

	//WE ONLY WANT THE PROPERTIES OF THE ACTUAL MATERIAL NOT THE CONTAINER - FOR NOW.....
	if(!mat->IsMultiType())
	{
		
		//in order of  XML schema
		/*
		<xs:element name="Diffuse" type="AnimatableColor" minOccurs="0" />
		<xs:element name="Ambient" type="AnimatableColor" minOccurs="0" />
		<xs:element name="Specular" type="AnimatableColor" minOccurs="0" />
		<xs:element name="Glossiness" type="AnimatableFloat" minOccurs="0" />
		<xs:element name="Opacity" type="AnimatableFloat" minOccurs="0" />
		<xs:element name="SpecularLevel" type="AnimatableFloat" minOccurs="0" />
		<xs:element name="SelfIllumination" type="AnimatableFloat" minOccurs="0" />
		<xs:element name="SpecularSoftening" type="AnimatableFloat" minOccurs="0" />
        */

		prop = mat->GetDiffuseData();
		CComPtr <IXMLDOMNode> d_propData;
		CreateXMLNode(pXMLDoc,material,mapSlotNames[0],&d_propData);
		DumpProperty(prop, d_propData);
	
		prop = mat->GetAmbientData();
		CComPtr <IXMLDOMNode> a_propData;
		CreateXMLNode(pXMLDoc,material,mapSlotNames[1],&a_propData);
		DumpProperty(prop, a_propData);

		prop = mat->GetSpecularData();
		CComPtr <IXMLDOMNode> s_propData;
		CreateXMLNode(pXMLDoc,material,mapSlotNames[2],&s_propData);
		DumpProperty(prop, s_propData);

		prop = mat->GetGlossinessData();
		CComPtr <IXMLDOMNode> g_propData;
		CreateXMLNode(pXMLDoc,material,mapSlotNames[5],&g_propData);
		DumpProperty(prop, g_propData);

		prop = mat->GetOpacityData();
		CComPtr <IXMLDOMNode> o_propData;
		CreateXMLNode(pXMLDoc,material,mapSlotNames[4],&o_propData);
		DumpProperty(prop, o_propData);

		prop = mat->GetSpecularLevelData();
		CComPtr <IXMLDOMNode> sl_propData;
		CreateXMLNode(pXMLDoc,material,mapSlotNames[3],&sl_propData);
		DumpProperty(prop, sl_propData);
		
	}

	//do the textures if they are there

	DumpTexture(material,mat);

	if(mat->IsMultiType())
	{
		CComPtr <IXMLDOMNode> multi;
		CreateXMLNode(pXMLDoc,material,_T("SubMaterials"),&multi);
		buf.printf(_T("%d"),mat->GetSubMaterialCount());
		AddXMLAttribute(multi,_T("Count"),buf.data());

		for(int k=0;k<mat->GetSubMaterialCount();k++)
		{
			int mID = mat->GetMaterialID(k);
			IGameMaterial * subMat = mat->GetSubMaterial(k);
			DumpMaterial(multi,subMat,k, mID);
		}
	}
	
	IGameFX * gameFX = mat->GetIGameFX();

	if(gameFX)
	{
		CComPtr <IXMLDOMNode> dxeffect,vertexshader, pixelshader;
		CreateXMLNode(pXMLDoc,material,_T("Effect"),&dxeffect);
		buf.printf(_T("%s"),gameFX->GetEffectFile().GetFileName());
		AddXMLAttribute(dxeffect,_T("FileName"),buf.data());

		buf.printf(_T("%d"),gameFX->GetNumberOfProperties());
		AddXMLAttribute(dxeffect,_T("PropertyCount"),buf.data());

		for(int i=0; i< gameFX->GetNumberOfProperties();i++)
		{
			CComPtr < IXMLDOMNode> fxProp;
            IGameFXProperty * prop = gameFX->GetIGameFXProperty(i);
			CreateXMLNode(pXMLDoc,dxeffect,_T("FXProperties"),&fxProp);
			buf.printf(_T("%d"),i);
			AddXMLAttribute(fxProp,_T("index"),buf.data());
			DumpFXProperties(fxProp,prop);
		}
		int numberOfTechniques, numberOfPasses;

		numberOfTechniques = gameFX->GetNumberOfTechniques();
		buf.printf(_T("%d"),numberOfTechniques);
		AddXMLAttribute(dxeffect,_T("NumberOfTechniques"),buf.data());
		for(int i=0; i< numberOfTechniques;i++)
		{
			IGameFXTechnique * fxTech = gameFX->GetIGameFXTechnique(i);
			CComPtr <IXMLDOMNode> tech = NULL;
            CreateXMLNode(pXMLDoc,dxeffect,_T("Technique"),&tech);
			buf.printf(_T("%d"),i);
			AddXMLAttribute(tech,_T("Index"),buf.data());
			numberOfPasses = fxTech->GetNumberOfPasses();
			buf.printf(_T("%d"),numberOfPasses);
			AddXMLAttribute(tech,_T("NumberOfPasses"),buf.data());
			for(int j=0; j< numberOfPasses;j++)
			{
				TSTR vertex, pixel;
				CComPtr <IXMLDOMNode> pass = NULL;
				IGameFXPass * fxPass = fxTech->GetIGameFXPass(j);
				CreateXMLNode(pXMLDoc,tech,_T("Pass"),&pass);
				buf.printf(_T("%d"), j);
				AddXMLAttribute(pass,_T("Index"),buf.data());
				CreateXMLNode(pXMLDoc,pass,_T("VertexShader"),&vertexshader);
				GetProfile(fxPass->GetTargetProfile(),true,profile);
				AddXMLAttribute(vertexshader,_T("Profile"),profile);
				
				fxPass->GetVertexShader(NULL,&vertex);
				AddXMLText(pXMLDoc,vertexshader,vertex.data());

				CreateXMLNode(pXMLDoc,pass,_T("PixelShader"),&pixelshader);
				GetProfile(fxPass->GetTargetProfile(),false,profile);
				AddXMLAttribute(pixelshader,_T("Profile"),profile);				
				fxPass->GetPixelShader(NULL,&pixel);
				AddXMLText(pXMLDoc,pixelshader,pixel.data());
				pixelshader = NULL;
				vertexshader = NULL;

			}
		}
	}

}

void IGameExporter::ExportMaterials()
{
	CComPtr <IXMLDOMNode> matNode;
	TSTR buf;
	if(exportMaterials)
	{
	
		CreateXMLNode(pXMLDoc,iGameNode,_T("MaterialList"),&matNode);
		int matCount = pIgame->GetRootMaterialCount();
		buf.printf(_T("%d"),matCount);
		AddXMLAttribute(matNode,_T("Count"),buf.data());

		for(int j =0;j<matCount;j++)
			DumpMaterial(matNode,pIgame->GetRootMaterial(j),j);
	}
}

// Dummy function for progress bar
DWORD WINAPI fn(LPVOID arg)
{
	return(0);
}



class MyErrorProc : public IGameErrorCallBack
{
public:
	void ErrorProc(IGameError error)
	{
		const TCHAR * buf = GetLastIGameErrorText();
		DebugPrint(_T("ErrorCode = %d ErrorText = %s\n"), error,buf);
	}
};

#include "utilapi.h"
int	IGameExporter::DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options)
{
	HRESULT hr;

	Interface * ip = GetCOREInterface();

	MyErrorProc pErrorProc;

	UserCoord Whacky = {
		1,	//Right Handed
		1,	//X axis goes right
		3,	//Y Axis goes down
		4,	//Z Axis goes in.
		1,	//U Tex axis is right
		1,  //V Tex axis is Down
	};	

	SetErrorCallBack(&pErrorProc);

	ReadConfig();
	bool succeeded = SUCCEEDED(init) &&SUCCEEDED(CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER,  IID_IXMLDOMDocument, (void**)&pXMLDoc));
	if(!succeeded)
		return false;
	
	// Check the return value, hr...
	hr = pXMLDoc->QueryInterface(IID_IXMLDOMNode, (void **)&pRoot);
	if(FAILED(hr))
		return false;

	// Set a global prompt display switch
	showPrompts = suppressPrompts ? FALSE : TRUE;

	exportSelected = (options & SCENE_EXPORT_SELECTED) ? true : false;
	igameVersion  = GetIGameVersion();

	DebugPrint(_T("3ds max compatible version %.2f\n"),GetSupported3DSVersion());

	if(showPrompts) 
	{
		// Prompt the user with our dialogbox, and get all the options.
		if (!DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_PANEL),
			i->GetMAXHWnd(), IGameExporterOptionsDlgProc, (LPARAM)this)) {
			return 1;
		}
	}

	curNode = 0;
	ip->ProgressStart(_T("Exporting Using IGame.."), TRUE, fn, NULL);
	
	pIgame = GetIGameInterface();

	IGameConversionManager * cm = GetConversionManager();
//	cm->SetUserCoordSystem(Whacky);
	cm->SetCoordSystem((IGameConversionManager::CoordSystem)cS);
//	pIgame->SetPropertyFile(_T("hello world"));
	pIgame->InitialiseIGame(exportSelected);
	pIgame->SetStaticFrame(staticFrame);
	
	TSTR path,file,ext;

	SplitFilename(TSTR(name),&path,&file,&ext);

	fileName = file;
	splitPath = path;
	
	iGameNode = ExportSceneInfo(pXMLDoc, pRoot);
	ExportMaterials();


	for(int loop = 0; loop <pIgame->GetTopLevelNodeCount();loop++)
	{
		IGameNode * pGameNode = pIgame->GetTopLevelNode(loop);
		//check for selected state - we deal with targets in the light/camera section
		if(pGameNode->IsTarget())
			continue;

		rootNode = NULL;
		
		CreateXMLNode(pXMLDoc,iGameNode,_T("Node"),&rootNode);
		ExportChildNodeInfo(rootNode,pGameNode);

	}
	pIgame->ReleaseIGame();
	PrettyPrint(name, pXMLDoc);
	
	pXMLDoc.Release();
	pRoot.Release();
	rootNode.Release();
	iGameNode.Release();
	// This is a totally bogus call, but it seems to return memory to the system.
	if (SUCCEEDED(CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER,  IID_IXMLDOMDocument, (void**)&pXMLDoc)))
		pXMLDoc.Release();


	ip->ProgressEnd();	
	WriteConfig();
	return TRUE;
}


TSTR IGameExporter::GetCfgFilename()
{
	TSTR filename;
	
	filename += GetCOREInterface()->GetDir(APP_PLUGCFG_DIR);
	filename += _T("\\");
	filename += _T("IgameExport.cfg");
	return filename;
}

// NOTE: Update anytime the CFG file changes
#define CFG_VERSION 0x03

BOOL IGameExporter::ReadConfig()
{
	TSTR filename = GetCfgFilename();
	FILE* cfgStream;

	cfgStream = _tfopen(filename, _T("rb"));
	if (!cfgStream)
		return FALSE;
	
	exportGeom = fgetc(cfgStream);
	exportNormals = fgetc(cfgStream);
	exportControllers = fgetc(cfgStream);
	exportFaceSmgp = fgetc(cfgStream);
	exportVertexColor = fgetc(cfgStream);
	exportTexCoords = fgetc(cfgStream);
	staticFrame = _getw(cfgStream);
	framePerSample = _getw(cfgStream);
	exportMappingChannel = fgetc(cfgStream);
	exportMaterials = fgetc(cfgStream);
	exportSplines = fgetc(cfgStream);
	exportModifiers = fgetc(cfgStream);
	forceSample = fgetc(cfgStream);
	exportConstraints = fgetc(cfgStream);
	exportSkin = fgetc(cfgStream);
	exportGenMod = fgetc(cfgStream);
	cS = fgetc(cfgStream);
	splitFile = fgetc(cfgStream);
	exportQuaternions = fgetc(cfgStream);
	exportObjectSpace = fgetc(cfgStream);
	exportRelative = fgetc(cfgStream);
	exportNormalsPerFace = fgetc(cfgStream);
	fclose(cfgStream);
	return TRUE;
}

void IGameExporter::WriteConfig()
{
	TSTR filename = GetCfgFilename();
	FILE* cfgStream;

	cfgStream = _tfopen(filename, _T("wb"));
	if (!cfgStream)
		return;

	
	fputc(exportGeom,cfgStream);
	fputc(exportNormals,cfgStream);
	fputc(exportControllers,cfgStream);
	fputc(exportFaceSmgp,cfgStream);
	fputc(exportVertexColor,cfgStream);
	fputc(exportTexCoords,cfgStream);
	_putw(staticFrame,cfgStream);
	_putw(framePerSample,cfgStream);
	fputc(exportMappingChannel,cfgStream);
	fputc(exportMaterials,cfgStream);
	fputc(exportSplines,cfgStream);
	fputc(exportModifiers,cfgStream);
	fputc(forceSample,cfgStream);
	fputc(exportConstraints,cfgStream);
	fputc(exportSkin,cfgStream);
	fputc(exportGenMod,cfgStream);
	fputc(cS,cfgStream);
	fputc(splitFile,cfgStream);
	fputc(exportQuaternions,cfgStream);
	fputc(exportObjectSpace,cfgStream);
	fputc(exportRelative,cfgStream);
	fputc(exportNormalsPerFace, cfgStream);
	fclose(cfgStream);
}


void IGameExporter::makeValidURIFilename(TSTR& fn, bool stripMapPaths)
{
	// massage external filenames into valid URI: strip any prefix matching any declared map path,
	//
	/*map \ -> /, sp -> _, : -> $
	if (stripMapPaths) {
		int matchLen = 0, matchI;
		for (int i = 0; i < TheManager->GetMapDirCount(); i++) {
			TSTR dir = TheManager->GetMapDir(i);
			if (MatchPattern(fn, dir + TSTR("*"))) {
				if (dir.length() > matchLen) {
					matchLen = dir.length();
					matchI = i;
				}
			}
		}
		if (matchLen > 0) {
			// found map path prefix, strip it 
			TSTR dir = TheManager->GetMapDir(matchI);
			fn.remove(0, dir.length());
			if (fn[0] = _T('\\')) fn.remove(0, 1); // strip any dangling path-sep
		}
	}
	*/

	// map funny chars
	for (int i = 0; i < fn.length(); i++) {
		if (fn[i] == _T(':')) fn.dataForWrite()[i] = _T('$');
		else if (fn[i] == _T(' ')) fn.dataForWrite()[i] = _T('_');
		else if (fn[i] == _T('\\')) fn.dataForWrite()[i] = _T('/');
	}
}


