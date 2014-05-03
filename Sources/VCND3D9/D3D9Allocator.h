///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#ifndef D3D9ALLOCATOR_H
#define D3D9ALLOCATOR_H

#pragma once

#include <d3dx9.h>
#include <d3dx9anim.h>

struct D3DModel;

class D3DAllocator : public ID3DXAllocateHierarchy
{
public:
	D3DAllocator(D3DModel* model);
	virtual ~D3DAllocator();

	STDMETHOD( CreateFrame )( THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame );
	STDMETHOD( CreateMeshContainer )( THIS_
		LPCSTR Name,
		CONST D3DXMESHDATA *pMeshData,
		CONST D3DXMATERIAL *pMaterials,
		CONST D3DXEFFECTINSTANCE *pEffectInstances,
		DWORD NumMaterials,
		CONST DWORD *pAdjacency,
		LPD3DXSKININFO pSkinInfo,
		LPD3DXMESHCONTAINER *ppNewMeshContainer );

	STDMETHOD( DestroyFrame )( THIS_ LPD3DXFRAME pFrameToFree );
	STDMETHOD( DestroyMeshContainer )( THIS_ LPD3DXMESHCONTAINER pMeshContainerBase );

private:
	D3DModel* m_Model;
};

#endif // D3D9ALLOCATOR_H
