///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#ifndef D3D9TYPES_H
#define D3D9TYPES_H

#pragma once

#include <VCNUtils\Sphere.h>

#include <d3dx9.h>
#include <d3dx9anim.h>
#include <string>
#include <vector>

//--------------------------------------------------------------------------------------
// Name: struct MultiAnimFrame
// Desc: Structure derived from D3DXFRAME so we can add some app-specific
//       info that will be stored with each frame
//--------------------------------------------------------------------------------------
struct MultiAnimFrame : public D3DXFRAME
{
    D3DXMATRIX CombinedTransformationMatrix;
};

//--------------------------------------------------------------------------------------
// Name: struct MultiAnimMC
// Desc: Structure derived from D3DXMESHCONTAINER so we can add some app-specific
//       info that will be stored with each mesh
//--------------------------------------------------------------------------------------
struct MultiAnimMC : public D3DXMESHCONTAINER
{
    MultiAnimMC();
    ~MultiAnimMC();

    // SkinMesh info             
    LPD3DXMESH m_pWorkingMesh;
    std::vector<D3DXMATRIX> m_amxBoneOffsets;
    std::vector<LPD3DXMATRIX> m_apmxBonePointers;

    DWORD m_dwNumAttrGroups;
    DWORD m_dwMaxNumFaceInfls;
    LPD3DXBUFFER m_pBufBoneCombos;
    DWORD m_dwNumPaletteEntries;

    HRESULT SetupBonePtrs( D3DXFRAME* pFrameRoot );
};

struct D3DModel
{
    D3DModel();
    ~D3DModel();

    long GenerateSkinnedMesh( IDirect3DDevice9* pd3dDevice, MultiAnimMC* pMeshContainer );

    std::wstring m_Name;

    unsigned int m_NumBoneMatricesMax;

    std::vector<LPD3DXMESHCONTAINER> m_MeshContainers;
    ID3DXAnimationController* m_AnimController;
    LPD3DXFRAME m_FrameRoot;
    VCNSphere m_BoundingSphere;
};

#endif // D3D9TYPES_H