/**********************************************************************
 *<
	FILE: Membrane.h

	DESCRIPTION:	Membrane Lighting Vertex Shader	

	CREATED BY:		Neil Hazzard, discreet	

	HISTORY:		created 1,9,01

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#ifndef __MEMBRANE__H
#define __MEMBRANE__H

#include "Max.h"
#include "resource.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"

#include "stdmat.h"
#include "imtl.h"
#include "macrorec.h"


extern TCHAR *GetString(int id);

#define MEMBRANE_CLASS_ID	Class_ID(0x305755d5, 0x3023465f)

#define NSUBTEX		1 // TODO: number of sub-textures supported by this plugin 
#define COORD_REF	0
#define PBLOCK_REF	1

#include "IDX9PixelShader.h"
#include "IDX9VertexShader.h"
#include "ID3D9GraphicsWindow.h"
#include "IStdDualVS.h"
#include "IViewportManager.h"


#define SAFE_DELETE(p)			{ if (p) { delete (p);		(p)=NULL; } }
#define SAFE_DELETE_ARRAY(p)	{ if (p) { delete[] (p);	(p)=NULL; } }
#define SAFE_RELEASE(p)			{ if (p) { (p)->Release();	(p)=NULL; } }

enum { pixelshader_params };

//TODO: Add enums for various parameters
enum { 
	pb_spin,
	pb_tv,
	pb_coords,
	pb_editbox,
};

class MembraneShader;
class MembraneVertexShader;
class MembranePixelShader;


class MembraneShader : public ReferenceTarget, public IDX9DataBridge {
	public:
		// Parameter block
		IParamBlock2 * pblock;

		MembraneVertexShader *pvs;
		MembranePixelShader *pps;



		// Loading/Saving
		IOResult Load(ILoad *iload);
		IOResult Save(ISave *isave);

		//From Animatable
		Class_ID ClassID() {return MEMBRANE_CLASS_ID;}		
		SClass_ID SuperClassID() { return REF_TARGET_CLASS_ID; }
		void GetClassName(TSTR& s) {s = GetString(IDS_BPSCLASS_NAME);}
		
		int	NumParamBlocks() { return 1; }			
		IParamBlock2* GetParamBlock(int i) { if(i == 0) return pblock; else return NULL;} 
		IParamBlock2* GetParamBlockByID(short id) { if(id == pixelshader_params ) return pblock; else return NULL;} 

		void DeleteThis() { delete this; }	
		int NumRefs(){return 1;}
private:
		virtual void SetReference(int i, RefTargetHandle  targ);
public:
		ReferenceTarget * GetReference(int i);		
		ReferenceTarget * Clone(RemapDir &remap);

		RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
		PartID& partID,  RefMessage message);

		// From IDXDataBridge
		ParamDlg * CreateEffectDlg(HWND hWnd, IMtlParams * imp);
		void DisableUI(){};
		void SetDXData(IHardwareMaterial * pHWMtl, Mtl * pMtl){};
		const TCHAR * GetName(){return _T("Membrane");}
		float GetDXVersion(){return 9.0f;}

		BaseInterface* GetInterface(Interface_ID id);


		//Constructor/Destructor
		MembraneShader();
		~MembraneShader();	


};

class MembranePixelShader : public IDX9PixelShader 
{
	bool	initDone;

	LPDIRECT3DDEVICE9 pd3dDevice;

	LPDIRECT3DCUBETEXTURE9	pCubeTexture;
	LPDIRECT3DPIXELSHADER9	dwPixelShader;
	LPDIRECT3DTEXTURE9 m_pShadeTexture;

public :
	MembranePixelShader(MembraneShader*);
	~MembranePixelShader();

	MembraneShader *map;
	HRESULT ConfirmDevice(ID3D9GraphicsWindow *d3dgw);

	HRESULT	ConfirmVertexShader(IDX9VertexShader *pvs);

	HRESULT Initialize(Material *mtl, INode *node);

	int GetNumMultiPass();

	LPDIRECT3DPIXELSHADER9 GetPixelShaderHandle(int numPass) { return dwPixelShader; }

	HRESULT SetPixelShader(ID3D9GraphicsWindow *d3dgw, int numPass);

	void LoadTexture( LPDIRECT3DTEXTURE9 * ppTex, TCHAR * filename );
};

class IDX9VertexShaderCache : public VertexShaderCache
{
public:
		
};

class MembraneVertexShader : public IDX9VertexShader, public IStdDualVSCallback
{
	ID3D9GraphicsWindow* m_pID3Dgw;

	bool initDone;

	LPDIRECT3DDEVICE9 pd3dDevice;

	LPDIRECT3DVERTEXDECLARATION9 vdecl;
	
	LPDIRECT3DTEXTURE9	pCubeTexture;
	// VertexShader Declarations
	Tab<DWORD> Declaration;

	// VertexShader Instructions
	LPD3DXBUFFER pCode;

	// VertexShader Constants
	Tab<D3DXVECTOR4> Constants;

		// VertexShader Handle
	LPDIRECT3DVERTEXSHADER9	dwVertexShader;

	D3DXMATRIX              m_matWorld;         // Local transform of the model
    D3DXMATRIX              m_matLight;         // Transform for the light
	
	IStdDualVS *stdDualVS;
	ReferenceTarget *rtarg;

	float m_tc[4];  //tvs

public:
	MembraneVertexShader(ReferenceTarget *rtarg);
	~MembraneVertexShader();
	
	HRESULT Initialize(Mesh *mesh, INode *node);
	HRESULT Initialize(MNMesh *mnmesh, INode *node);
	// From FPInterface
	LifetimeType	LifetimeControl() { return noRelease; }

	// From IVertexShader
	HRESULT ConfirmDevice(ID3D9GraphicsWindow *d3dgw);
	
	HRESULT ConfirmPixelShader(IDX9PixelShader *pps);

	bool CanTryStrips();

	int GetNumMultiPass();

	LPDIRECT3DVERTEXSHADER9 GetVertexShaderHandle(int numPass) { return dwVertexShader; }

	HRESULT SetVertexShader(ID3D9GraphicsWindow *d3dgw, int numPass);
	
	// Draw 3D mesh as TriStrips
	bool	DrawMeshStrips(ID3D9GraphicsWindow *d3dgw, MeshData *data);

	// Draw 3D mesh as wireframe
	bool	DrawWireMesh(ID3D9GraphicsWindow *d3dgw, WireMeshData *data);

	// Draw 3D lines
	void	StartLines(ID3D9GraphicsWindow *d3dgw, WireMeshData *data);
	void	AddLine(ID3D9GraphicsWindow *d3dgw, DWORD *vert, int vis);
	bool	DrawLines(ID3D9GraphicsWindow *d3dgw);
	void	EndLines(ID3D9GraphicsWindow *d3dgw, GFX_ESCAPE_FN fn);

	// Draw 3D triangles
	void	StartTriangles(ID3D9GraphicsWindow *d3dgw, MeshFaceData *data);
	void	AddTriangle(ID3D9GraphicsWindow *d3dgw, DWORD index, int *edgeVis);
	bool	DrawTriangles(ID3D9GraphicsWindow *d3dgw);
	void	EndTriangles(ID3D9GraphicsWindow *d3dgw, GFX_ESCAPE_FN fn);

	// from IStdDualVSCallback
	virtual ReferenceTarget *GetRefTarg();
	virtual VertexShaderCache *CreateVertexShaderCache();
	virtual HRESULT  InitValid(Mesh* mesh, INode *node);
	virtual HRESULT  InitValid(MNMesh* mnmesh, INode *node);
	HRESULT SetVertexShaderMatrices(ID3D9GraphicsWindow *d3dgw);
	HRESULT SetTextureStageStates();

	//!Finds the first Camera and returns its position 
	/*!
	\param &Pos A pointer to store the position
	*/
	void GetCameraPosition(Point3 &Pos);
	
};

class MembraneShaderClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading = FALSE) {return new MembraneShader();}
	const TCHAR *	ClassName() {return GetString(IDS_BPSCLASS_NAME);}
	SClass_ID		SuperClassID() {return REF_TARGET_CLASS_ID;}
	Class_ID		ClassID() {return MEMBRANE_CLASS_ID;}
	const TCHAR* 	Category() {return GetString(IDS_PSCATEGORY);}
	const TCHAR*	InternalName() { return _T("Membrane Shader"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle
};




#endif // __MEMBRANE__H
