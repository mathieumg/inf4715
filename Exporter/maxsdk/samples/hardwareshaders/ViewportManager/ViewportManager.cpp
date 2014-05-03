/**********************************************************************
 *<
	FILE: ViewportManagerControl.cpp

	DESCRIPTION:	Add the  ViewportManager to the Material Editor

	CREATED BY:		Neil Hazzard

	HISTORY:		02/15/02

	TODO:			Provide a class structure for handing the CA in material
					there is alot of code copying going on at the moment

 *>	Copyright (c) 2002, All Rights Reserved.
 **********************************************************************/
#include <d3dx9.h>
#include "ViewportManager.h"
#include "ViewportLoader.h"
#include "Notify.h"
#include <xref\iXrefMaterial.h>
#include <Util\IniUtil.h> // MaxSDK::Util::WritePrivateProfileString
#include <guplib.h>
#include "resource.h"
#include <icustattribcontainer.h>
#include <imtledit.h>

#define MRARCHMATERIAL_CLASS_ID	Class_ID(0x70b05735, 0x4a163654)

// Singleton class
class ViewportManagerControl : 
	public GUP,
	public IViewportManager
{
	public:

		BOOL ShowManager;

		// GUP Methods
		virtual DWORD	Start	();
		virtual void	Stop	();
		virtual void DeleteThis ();
		DWORD_PTR	Control			( DWORD parameter );
		
		// Loading/Saving
		IOResult Save(ISave *isave);
		IOResult Load(ILoad *iload);


		//Constructor/Destructor

		ViewportManagerControl();
		~ViewportManagerControl();	
		
		static BOOL IsManagerLoaded( MtlBase* mtl );
		static void RemoveManager( MtlBase* mtl );
		static void LoadManager( MtlBase* mtl );
		virtual BOOL IsValidMaterial( MtlBase* mtl );		
private:

	static void EditMtlBaseNotify( void *param, NotifyInfo *info );
};


ViewportManagerControl theViewportManagerControl;
class ViewportManagerControlClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) {return &theViewportManagerControl; }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return GUP_CLASS_ID; }
	Class_ID		ClassID() { return VIEWPORTMANAGERCONTROL_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("ViewportManagerControl"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle

};

static ViewportManagerControlClassDesc ViewportManagerControlDesc;
ClassDesc2* GetViewportManagerControlDesc() { return &ViewportManagerControlDesc; }



ViewportManagerControl::ViewportManagerControl()
{
	ShowManager = false;
}

ViewportManagerControl::~ViewportManagerControl()
{
}

static void UpdateINIFile(void *param, NotifyInfo *info) 
{
	TCHAR filename[MAX_PATH];
   	TCHAR Buf[256];
	IDXShaderManagerInterface * sm = GetDXShaderManager();
	_itot(sm->IsVisible(),Buf,10);
	
	_tcscpy(filename,GetCOREInterface()->GetDir(APP_PLUGCFG_DIR));
	_tcscat(filename,_T("\\DXManager.ini")); 
	if(CheckForDX())
		MaxSDK::Util::WritePrivateProfileString(_T("DXSettings"),_T("ManagerEnabled"),Buf,filename);
	else
		MaxSDK::Util::WritePrivateProfileString(_T("OGLSettings"),_T("ManagerEnabled"),Buf,filename);

}

void ViewportManagerControl::EditMtlBaseNotify( void *param, NotifyInfo *info ) 
{
	if ( !param || !info ) return ;

	ViewportManagerControl* manager = static_cast< ViewportManagerControl* >( param );

	MtlBase* mtlbase = (MtlBase*)info->callParam;


	if ( info->intcode == NOTIFY_MTLBASE_PARAMDLG_PRE_OPEN && mtlbase )
	{
		if ( theViewportManagerControl.IsValidMaterial( mtlbase ) )
		{
			if ( !IsManagerLoaded( mtlbase ) )
				LoadManager( mtlbase );
		}
		else
		{
			RemoveManager( mtlbase );
		}
	}

}

// Activate and Stay Resident
DWORD ViewportManagerControl::Start( ) 
{
	
	//Setup as ::Stop is called too late to use GetCOREInterface()
	RegisterNotification( UpdateINIFile, this, NOTIFY_SYSTEM_SHUTDOWN );
	RegisterNotification( EditMtlBaseNotify, this, NOTIFY_MTLBASE_PARAMDLG_PRE_OPEN );

	TCHAR filename[MAX_PATH];
	IDXShaderManagerInterface * sm = GetDXShaderManager();
	_tcscpy(filename,GetCOREInterface()->GetDir(APP_PLUGCFG_DIR));
	_tcscat(filename,_T("\\DXManager.ini"));

	// Always on for DX
	// NH 04|16|03 - Relaxed this, from a request from PFB and the vis users
	if ( CheckForDX() )
	{
		ShowManager = GetPrivateProfileInt(_T("DXSettings"),_T("ManagerEnabled"),1,filename);
		sm->SetVisible(ShowManager);
	}
		
	else
	{

		ShowManager = GetPrivateProfileInt(_T("OGLSettings"),_T("ManagerEnabled"),0,filename);
		sm->SetVisible(ShowManager);
	}
	return GUPRESULT_KEEP;
}

void ViewportManagerControl::Stop( ) 
{

	UnRegisterNotification( UpdateINIFile, this, NOTIFY_SYSTEM_SHUTDOWN );
	UnRegisterNotification( EditMtlBaseNotify, this, NOTIFY_MTLBASE_PARAMDLG_PRE_OPEN );
}

void ViewportManagerControl::DeleteThis()
{
	// statically allocated. Do nothing.
}

DWORD_PTR ViewportManagerControl::Control( DWORD parameter ) 
{
	return 0;
}

#define MANAGER_REMOVEALL_CHUNK	0x1000

IOResult ViewportManagerControl::Save(ISave *isave)
{
	return IO_OK;
}

IOResult ViewportManagerControl::Load(ILoad *iload)
{
	return IO_OK;
}

BOOL ViewportManagerControl::IsManagerLoaded( MtlBase* mtl  )
{
	if ( !mtl )
		return true;	//this just happens with early ref messeages
	
	ICustAttribContainer* cc = mtl->GetCustAttribContainer();
	if ( !cc )
	{
		return false;
	}

	for ( int i = 0; i < cc->GetNumCustAttribs(); i++ )
	{
		CustAttrib * ca = cc->GetCustAttrib( i );
		if ( ca->GetInterface(VIEWPORT_SHADER_MANAGER_INTERFACE) )
			return true;
	}
	return false;
}

// This needs to be reconsidered - at we should not really delete the CA - we just want to hide it
void ViewportManagerControl::RemoveManager( MtlBase* mtl )
{
	if ( !mtl )
		return;
	
	ICustAttribContainer* cc = mtl->GetCustAttribContainer();
	if ( cc )
	{
		for ( int i = 0; i < cc->GetNumCustAttribs(); i++ )
		{
			CustAttrib * ca = cc->GetCustAttrib( i );
			if ( ca->GetInterface(VIEWPORT_SHADER_MANAGER_INTERFACE) )
				cc->RemoveCustAttrib( i );
		}
	}
	
}

void ViewportManagerControl::LoadManager( MtlBase* mtl )
{
	if ( !mtl )
		return;		//only in very BAD cases or early ref messages

	if ( IsManagerLoaded( mtl ) )
		return;

	ICustAttribContainer* cc = mtl->GetCustAttribContainer();
	if ( !cc )
	{
		mtl->AllocCustAttribContainer();
		cc = mtl->GetCustAttribContainer();
	}

	if ( cc->GetNumCustAttribs() > 0 )
		cc->InsertCustAttrib( 0, (CustAttrib *)CreateInstance(CUST_ATTRIB_CLASS_ID,VIEWPORTLOADER_CLASS_ID) );
	else
		cc->AppendCustAttrib( (CustAttrib *)CreateInstance(CUST_ATTRIB_CLASS_ID,VIEWPORTLOADER_CLASS_ID) );
}

static Class_ID multiClassID(MULTI_CLASS_ID,0);
static Class_ID bakeShellClassID(BAKE_SHELL_CLASS_ID,0);
#define DXMATERIAL_DYNAMIC_UI Class_ID(0xef12512, 0x11351ed1)

bool IsDynamicDxMaterial(MtlBase * newMtl)
{

	DllDir * lookup = GetCOREInterface()->GetDllDirectory();
	ClassDirectory & dirLookup = lookup->ClassDir();

	ClassDesc * cd = dirLookup.FindClass(MATERIAL_CLASS_ID,newMtl->ClassID());
	if(cd->SubClassID() == DXMATERIAL_DYNAMIC_UI)
		return true;
	else
		return false;


}


BOOL ViewportManagerControl::IsValidMaterial( MtlBase * mtl )
{
	// We need to limit where the effect can be applied.  It can go on any top level material, but we restrict it
	// to only Multi Materials as a sublevel.  So a StdMtl2 on a blend is not allowed, but is OK for Multi.
	// If the material is only referenced by the editor then its a top level material, 
	if(!mtl)
		return false;		//only in very BAD cases or early ref messages

	if(mtl->SuperClassID()== TEXMAP_CLASS_ID)
		return false;
	// as the multi material is special we do not allow it at the parent, but rather at the child level
	// also added Shell Material
	// [dl | 24feb2005] Adding XRef material
	if(mtl->ClassID()==multiClassID
		|| mtl->ClassID()==bakeShellClassID
		|| mtl->ClassID()==LOCKMAT_CLASS_ID
		|| IXRefMaterial::Is_IXRefMaterial(*mtl))
		return false;
	if (mtl->ClassID() == MRARCHMATERIAL_CLASS_ID)
	{
		return false;
	}
	// we don't let it on the Viewport Shader Material.
	if(IsDynamicDxMaterial(mtl) )
		return false;


	DependentIterator di(mtl);
	ReferenceMaker* maker = NULL;
	BOOL multiparent = false;
	int mtlcount = 0;

	while ((maker = di.Next()) != NULL)
	{
		if (maker->SuperClassID()==MATERIAL_CLASS_ID ) 
		{
			if(maker->ClassID()==multiClassID)
				multiparent = true;
			else if (maker->ClassID()==LOCKMAT_CLASS_ID)
				multiparent = true;
			else if(maker->SuperClassID()==TEXMAP_CLASS_ID)
				multiparent = true;
			// added as per Claude's request
			else if(maker->ClassID()==bakeShellClassID)
				multiparent = true;
			// [dl | 24feb2005] Adding XRef material
			else if(IXRefMaterial::Is_IXRefMaterial(*maker))
				multiparent = true;

			mtlcount++;
		}
	}
	
	if(mtlcount==0 || (mtlcount>0 && multiparent))
		return true;
	else 
		return false;

}



class DXShaderManager :public IDXShaderManagerInterface
{
	public:
		BOOL vis;
		enum {
			getShaderManager,isVisible,setVisible,addShaderManager
		};
		
		CustAttrib* FindViewportShaderManager (MtlBase* mtl);
		CustAttrib*	AddViewportShaderManager(MtlBase * mtl);
		void	SetVisible(BOOL show=TRUE);
		BOOL	IsVisible();
	
		
		DECLARE_DESCRIPTOR(DXShaderManager);

		BEGIN_FUNCTION_MAP
			FN_1(getShaderManager, TYPE_REFTARG, FindViewportShaderManager, TYPE_MTL);
			FN_0(isVisible,TYPE_BOOL, IsVisible);
			VFN_1(setVisible, SetVisible,  TYPE_BOOL);
			FN_1(addShaderManager,TYPE_REFTARG,AddViewportShaderManager,TYPE_MTL);

		END_FUNCTION_MAP

};

static DXShaderManager iShaderManagerInterface (IDX_SHADER_MANAGER, _T("dxshadermanager"), 0,
		NULL, FP_CORE + FP_STATIC_METHODS,
	//methods
	DXShaderManager::getShaderManager, _T("getViewportManager"), 0, TYPE_REFTARG, 0, 1,
		_T("material"), 0, TYPE_MTL,
	
	DXShaderManager::isVisible,_T("IsVisible"),0,TYPE_BOOL,0,0,

	DXShaderManager::setVisible, _T("SetVisible"),0,0,0,1,
		_T("show"), 0, TYPE_BOOL,	
	
	DXShaderManager::addShaderManager, _T("addViewportManager"), 0, TYPE_REFTARG, 0, 1,
	_T("material"), 0, TYPE_MTL,

	p_end
	); 


CustAttrib * DXShaderManager::FindViewportShaderManager (MtlBase* mtl)
{	
	
	if(!mtl)
		return NULL;

	ICustAttribContainer* cc = mtl->GetCustAttribContainer();
	if(!cc)
	{
		return NULL;
	}

	for(int i=0; i<cc->GetNumCustAttribs();i++)
	{
		CustAttrib * ca = cc->GetCustAttrib(i);
		if(ca->GetInterface( VIEWPORT_SHADER_MANAGER_INTERFACE))
			return ca;
	}
	return NULL;
}

CustAttrib * DXShaderManager::AddViewportShaderManager (MtlBase* mtl)
{	
	if(!mtl)
		return NULL;

	// Do not add the manager if the material does not support it.
	if ( !theViewportManagerControl.IsValidMaterial( mtl ) )
		return NULL;

	CustAttrib * manager = (CustAttrib *)CreateInstance(CUST_ATTRIB_CLASS_ID,VIEWPORTLOADER_CLASS_ID);

	if(!manager)
		return NULL;

	ICustAttribContainer* cc = mtl->GetCustAttribContainer();
	if(!cc)
	{
		mtl->AllocCustAttribContainer();
		cc = mtl->GetCustAttribContainer();
	}
	if(cc->GetNumCustAttribs()>0)
		cc->InsertCustAttrib(0,manager);
	else
		cc->AppendCustAttrib(manager);

	return manager;
}

void DXShaderManager::SetVisible(BOOL show)
{
	IMtlEditInterface *mtlEdit = (IMtlEditInterface *)GetCOREInterface(MTLEDIT_INTERFACE);
#ifdef HIDE_VIEWPORT_MANAGER_UI
	vis = false;
#else // HIDE_VIEWPORT_MANAGER_UI
	vis = show;
#endif // HIDE_VIEWPORT_MANAGER_UI

//  NH 04|16|03 - Relaxed this, from a request from PFB and the vis users
//	if(CheckForDX())
//		vis = true;

	//force an update
	int slot = mtlEdit->GetActiveMtlSlot();
	mtlEdit->SetActiveMtlSlot(slot);
	

}
BOOL DXShaderManager::IsVisible()
{
	return vis;
}