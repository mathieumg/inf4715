///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#include "Precompiled.h"
#include "D3D9Types.h"

#include "D3D9Help.h"

#include "VCNUtils\Assert.h"
#include "VCNUtils\Macros.h"

MultiAnimMC::MultiAnimMC()
:   m_pWorkingMesh(nullptr),
    m_dwNumAttrGroups(0),
    m_dwMaxNumFaceInfls(0),
    m_pBufBoneCombos(nullptr),
    m_dwNumPaletteEntries(0)
{
    memset( static_cast<D3DXMESHCONTAINER*>(this), 0, sizeof( D3DXMESHCONTAINER ) );
}

MultiAnimMC::~MultiAnimMC()
{
    SAFE_DELETE_ARRAY( Name );
    SAFE_DELETE_ARRAY( pAdjacency );

    for(std::size_t iMaterial = 0; iMaterial < NumMaterials; iMaterial++ )
    {
        SAFE_DELETE_ARRAY( pMaterials[iMaterial].pTextureFilename );
    }

    SAFE_DELETE_ARRAY( pMaterials );

    SAFE_RELEASE( m_pBufBoneCombos );
    SAFE_RELEASE( MeshData.pMesh );
    SAFE_RELEASE( pSkinInfo );
    SAFE_RELEASE( m_pWorkingMesh );
}

HRESULT MultiAnimMC::SetupBonePtrs( D3DXFRAME* pFrameRoot )
{
    if( !pSkinInfo )
        return S_OK;

    m_apmxBonePointers.clear();

    DWORD dwNumBones = pSkinInfo->GetNumBones();
    m_apmxBonePointers.reserve(dwNumBones);
    for( DWORD i = 0; i < dwNumBones; ++i )
    {
        MultiAnimFrame* pFrame = ( MultiAnimFrame* )D3DXFrameFind( pFrameRoot, pSkinInfo->GetBoneName( i ) );
        if( pFrame == NULL )
            return E_FAIL;

        m_apmxBonePointers.push_back(&pFrame->CombinedTransformationMatrix);
    }

    return S_OK;
}

D3DModel::D3DModel()
    :   m_NumBoneMatricesMax(0),
        m_AnimController(nullptr),
        m_FrameRoot(nullptr)
{}

D3DModel::~D3DModel()
{
    SAFE_RELEASE( m_AnimController );

    for(std::size_t i = 0; i < m_MeshContainers.size(); ++i)
    {
        MultiAnimMC* container = (MultiAnimMC*)m_MeshContainers[i];
        delete container;
    }
}

//--------------------------------------------------------------------------------------
// Called either by CreateMeshContainer when loading a skin mesh, or when 
// changing methods.  This function uses the pSkinInfo of the mesh 
// container to generate the desired drawable mesh and bone combination 
// table.
//--------------------------------------------------------------------------------------
HRESULT D3DModel::GenerateSkinnedMesh( IDirect3DDevice9* pd3dDevice, MultiAnimMC* pMeshContainer )
{
    HRESULT hr = S_OK;
    D3DCAPS9 d3dCaps;
    pd3dDevice->GetDeviceCaps( &d3dCaps );

    if( pMeshContainer->pSkinInfo == NULL )
        return hr;

    SAFE_RELEASE( pMeshContainer->MeshData.pMesh );
    SAFE_RELEASE( pMeshContainer->m_pBufBoneCombos );

    // Get palette size
    // First 9 constants are used for other data.  Each 4x3 matrix takes up 3 constants.
    // (96 - 9) /3 i.e. Maximum constant count - used constants 
    UINT MaxMatrices = 26;
    pMeshContainer->m_dwNumPaletteEntries = std::min<DWORD>( MaxMatrices, pMeshContainer->pSkinInfo->GetNumBones() );

    DWORD Flags = D3DXMESHOPT_VERTEXCACHE;
    if( d3dCaps.VertexShaderVersion >= D3DVS_VERSION( 1, 1 ) )
    {
        Flags |= D3DXMESH_MANAGED;
    }
    else
    {
        Flags |= D3DXMESH_SYSTEMMEM;
    }

    SAFE_RELEASE( pMeshContainer->MeshData.pMesh );

    hr = pMeshContainer->pSkinInfo->ConvertToIndexedBlendedMesh
        (
        pMeshContainer->m_pWorkingMesh,
        Flags,
        pMeshContainer->m_dwNumPaletteEntries,
        pMeshContainer->pAdjacency,
        NULL, NULL, NULL,
        &pMeshContainer->m_dwMaxNumFaceInfls,
        &pMeshContainer->m_dwNumAttrGroups,
        &pMeshContainer->m_pBufBoneCombos,
        &pMeshContainer->MeshData.pMesh );
    if( FAILED( hr ) )
        goto e_Exit;


    // FVF has to match our declarator. Vertex shaders are not as forgiving as FF pipeline
    DWORD NewFVF = ( pMeshContainer->MeshData.pMesh->GetFVF() & D3DFVF_POSITION_MASK ) | D3DFVF_NORMAL |
        D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4;
    if( NewFVF != pMeshContainer->MeshData.pMesh->GetFVF() )
    {
        LPD3DXMESH pMesh;
        hr = pMeshContainer->MeshData.pMesh->CloneMeshFVF( pMeshContainer->MeshData.pMesh->GetOptions(), NewFVF,
            pd3dDevice, &pMesh );
        if( !FAILED( hr ) )
        {
            pMeshContainer->MeshData.pMesh->Release();
            pMeshContainer->MeshData.pMesh = pMesh;
            pMesh = NULL;
        }
    }

    D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];
    LPD3DVERTEXELEMENT9 pDeclCur;
    hr = pMeshContainer->MeshData.pMesh->GetDeclaration( pDecl );
    if( FAILED( hr ) )
        goto e_Exit;

    // the vertex shader is expecting to interpret the UBYTE4 as a D3DCOLOR, so update the type 
    //   NOTE: this cannot be done with CloneMesh, that would convert the UBYTE4 data to float and then to D3DCOLOR
    //          this is more of a "cast" operation
    pDeclCur = pDecl;
    while( pDeclCur->Stream != 0xff )
    {
        if( ( pDeclCur->Usage == D3DDECLUSAGE_BLENDINDICES ) && ( pDeclCur->UsageIndex == 0 ) )
            pDeclCur->Type = D3DDECLTYPE_D3DCOLOR;
        pDeclCur++;
    }

    hr = pMeshContainer->MeshData.pMesh->UpdateSemantics( pDecl );
    if( FAILED( hr ) )
        goto e_Exit;

e_Exit:
    return hr;
}