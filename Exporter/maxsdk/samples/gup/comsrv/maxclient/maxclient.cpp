//-----------------------------------------------------------------------------
// MaxClient.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
//#include <objbase.h>
#include <atlbase.h>
#include <atlcom.h>
#if _MSC_VER < 1300  // Visual Studio .NET
#include <atlimpl.cpp>
#endif

#include "MaxClient.h"
#include "MaxClientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CComModule _Module;
BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()

//-----------------------------------------------------------------------------
// CMaxClientApp

BEGIN_MESSAGE_MAP(CMaxClientApp, CWinApp)
	//{{AFX_MSG_MAP(CMaxClientApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
// #> CMaxClientApp::CMaxClientApp()
//

CMaxClientApp::CMaxClientApp() {

}

//-----------------------------------------------------------------------------
// The one and only CMaxClientApp object

CMaxClientApp theApp;

//-----------------------------------------------------------------------------
// #> CMaxClientApp::IsTypeLibRegistered()

bool CMaxClientApp::IsTypeLibRegistered( ) {
	HKEY hKey;
	long retVal = RegOpenKeyEx(HKEY_CLASSES_ROOT,_T("TypeLib\\{4AD72E61-5A4B-11D2-91CB-0060081C257E}"),0,KEY_READ,&hKey);
	if (retVal == ERROR_SUCCESS) {
		RegCloseKey(hKey);
		return true;
	}
	return false;
}


//-----------------------------------------------------------------------------
// #> CMaxClientApp::RegisterTypeLibrary()
//

bool CMaxClientApp::RegisterTypeLibrary() {
	bool result = false;
	wchar_t szFileName[MAX_PATH];
	GetModuleFileNameW(m_hInstance,szFileName,MAX_PATH);
	ITypeLib *ptl = 0;
	HRESULT hr = LoadTypeLib(szFileName, &ptl);
	if (SUCCEEDED(hr)) {
		hr = RegisterTypeLib(ptl, szFileName, 0);
		ptl->Release();
		result = true;
	}
	return result;
}

//-----------------------------------------------------------------------------
// CMaxClientApp initialization

BOOL CMaxClientApp::InitInstance() {

	HRESULT hr = CoInitialize(NULL);
	_ASSERTE(SUCCEEDED(hr));
	_Module.Init(ObjectMap,m_hInstance);
	
	//-- Handle Registration

	if (!IsTypeLibRegistered()) {
		if (!RegisterTypeLibrary()) {

			//-- Won't do much...

		}
	}
#if _MSC_VER < 1300  // Visual Studio .NET
	#ifdef _AFXDLL
	Enable3dControls();
	#else
	Enable3dControlsStatic();
	#endif
#endif

	CMaxClientDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();
	CoUninitialize();
	return FALSE;
}
