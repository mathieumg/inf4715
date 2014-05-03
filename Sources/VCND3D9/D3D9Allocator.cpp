///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief D3D9 custom allocators when loading a mesh file
///

#include "Precompiled.h"
#include "D3D9Allocator.h"

#include "D3D9Types.h"
#include "D3D9Help.h"

#include "VCNUtils\Assert.h"
#include "VCNUtils\Matrix.h"

#include <string>

D3DAllocator::D3DAllocator(D3DModel* model)
    : m_Model(model)
{}

D3DAllocator::~D3DAllocator()
{}


//--------------------------------------------------------------------------------------
// Name: CAllocateHierarchy::CreateFrame()
// Desc: 
//--------------------------------------------------------------------------------------
HRESULT D3DAllocator::CreateFrame( LPCSTR Name, LPD3DXFRAME* ppNewFrame )
{
    HRESULT hr = S_OK;

    *ppNewFrame = nullptr;

    MultiAnimFrame* pFrame = new MultiAnimFrame();

    hr = AllocateName( Name, &pFrame->Name );
    if( FAILED( hr ) )
        goto e_Exit;

    // initialize other data members of the frame
    D3DXMatrixIdentity( &pFrame->TransformationMatrix );
    D3DXMatrixIdentity( &pFrame->CombinedTransformationMatrix );

    pFrame->pMeshContainer = nullptr;
    pFrame->pFrameSibling = nullptr;
    pFrame->pFrameFirstChild = nullptr;

    *ppNewFrame = pFrame;
    pFrame = NULL;

e_Exit:
    delete pFrame;
    return hr;
}


//--------------------------------------------------------------------------------------
// Name: CAllocateHierarchy::CreateMeshContainer()
// Desc: 
//--------------------------------------------------------------------------------------
HRESULT D3DAllocator::CreateMeshContainer(
    LPCSTR Name,
    CONST D3DXMESHDATA *pMeshData,
    CONST D3DXMATERIAL *pMaterials,
    CONST D3DXEFFECTINSTANCE *pEffectInstances,
    DWORD NumMaterials,
    CONST DWORD *pAdjacency,
    LPD3DXSKININFO pSkinInfo,
    LPD3DXMESHCONTAINER *ppNewMeshContainer )
{
    HRESULT hr;
    UINT NumFaces;
    UINT iMaterial;
    UINT iBone, cBones;
    LPDIRECT3DDEVICE9 pd3dDevice = NULL;

    LPD3DXMESH pMesh = NULL;

    *ppNewMeshContainer = NULL;

    // this sample does not handle patch meshes, so fail when one is found
    if( pMeshData->Type != D3DXMESHTYPE_MESH )
    {
        hr = E_FAIL;
        goto e_Exit;
    }

    // get the pMesh interface pointer out of the mesh data structure
    pMesh = pMeshData->pMesh;

    // this sample does not FVF compatible meshes, so fail when one is found
    if( pMesh->GetFVF() == 0 )
    {
        hr = E_FAIL;
        goto e_Exit;
    }

    // allocate the overloaded structure to return as a D3DXMESHCONTAINER
    MultiAnimMC* pMeshContainer = new MultiAnimMC();

    // make sure and copy the name.  All memory as input belongs to caller, interfaces can be addref'd though
    hr = AllocateName( Name, &pMeshContainer->Name );
    if( FAILED( hr ) )
        goto e_Exit;

    pMesh->GetDevice( &pd3dDevice );
    NumFaces = pMesh->GetNumFaces();

    // if no normals are in the mesh, add them
    if( !( pMesh->GetFVF() & D3DFVF_NORMAL ) )
    {
        pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

        // clone the mesh to make room for the normals
        hr = pMesh->CloneMeshFVF( pMesh->GetOptions(),
            pMesh->GetFVF() | D3DFVF_NORMAL,
            pd3dDevice, &pMeshContainer->MeshData.pMesh );
        if( FAILED( hr ) )
            goto e_Exit;

        // get the new pMesh pointer back out of the mesh container to use
        // NOTE: we do not release pMesh because we do not have a reference to it yet
        pMesh = pMeshContainer->MeshData.pMesh;

        // now generate the normals for the pmesh
        D3DXComputeNormals( pMesh, NULL );
    }
    else  // if no normals, just add a reference to the mesh for the mesh container
    {
        pMeshContainer->MeshData.pMesh = pMesh;
        pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

        pMesh->AddRef();
    }

    // allocate memory to contain the material information.  This sample uses
    //   the D3D9 materials and texture names instead of the EffectInstance style materials
    pMeshContainer->NumMaterials = std::max<DWORD>( 1, NumMaterials );
    pMeshContainer->pMaterials = new D3DXMATERIAL[pMeshContainer->NumMaterials];
    pMeshContainer->pAdjacency = new DWORD[NumFaces*3];
    if( ( pMeshContainer->pAdjacency == NULL ) || ( pMeshContainer->pMaterials == NULL ) )
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    memcpy( pMeshContainer->pAdjacency, pAdjacency, sizeof( DWORD ) * NumFaces*3 );
    // if materials provided, copy them
    if( NumMaterials > 0 )
    {
        memcpy( pMeshContainer->pMaterials, pMaterials, sizeof( D3DXMATERIAL ) * NumMaterials );

        for( iMaterial = 0; iMaterial < NumMaterials; iMaterial++ )
        {
            if( pMeshContainer->pMaterials[iMaterial].pTextureFilename != nullptr )
            {
                // don't remember a pointer into the dynamic memory
                //duplicate texture name for usage later on
                size_t len = strlen(pMaterials[iMaterial].pTextureFilename);
                pMeshContainer->pMaterials[iMaterial].pTextureFilename = new char[len + 1];
                strcpy(pMeshContainer->pMaterials[iMaterial].pTextureFilename, pMaterials[iMaterial].pTextureFilename);
                pMeshContainer->pMaterials[iMaterial].pTextureFilename[len] = '\0';
            }
        }
    }
    else // if no materials provided, use a default one
    {
        pMeshContainer->pMaterials[0].pTextureFilename = NULL;
        memset( &pMeshContainer->pMaterials[0].MatD3D, 0, sizeof( D3DMATERIAL9 ) );
        pMeshContainer->pMaterials[0].MatD3D.Diffuse.r = 0.5f;
        pMeshContainer->pMaterials[0].MatD3D.Diffuse.g = 0.5f;
        pMeshContainer->pMaterials[0].MatD3D.Diffuse.b = 0.5f;
        pMeshContainer->pMaterials[0].MatD3D.Specular = pMeshContainer->pMaterials[0].MatD3D.Diffuse;
    }

    m_Model->m_MeshContainers.push_back(pMeshContainer);

    // if there is skinning information, save off the required data and then setup for HW skinning
    if( pSkinInfo != NULL )
    {
        // first save off the SkinInfo and original mesh data
        pMeshContainer->pSkinInfo = pSkinInfo;
        pSkinInfo->AddRef();

        pMeshContainer->m_pWorkingMesh = pMesh;
        pMesh->AddRef();

        // Will need an array of offset matrices to move the vertices from the figure space to the bone's space
        cBones = pSkinInfo->GetNumBones();

        VCN_ASSERT( pMeshContainer->m_amxBoneOffsets.empty() );
        // get each of the bone offset matrices so that we don't need to get them later
        for( iBone = 0; iBone < cBones; iBone++ )
        {
            pMeshContainer->m_amxBoneOffsets.push_back( *pMeshContainer->pSkinInfo->GetBoneOffsetMatrix( iBone ) );
        }

        // GenerateSkinnedMesh will take the general skinning information and transform it to a HW friendly version
        hr = m_Model->GenerateSkinnedMesh( pd3dDevice, pMeshContainer );
        if( FAILED( hr ) )
            goto e_Exit;
    }

    *ppNewMeshContainer = pMeshContainer;
    pMeshContainer = NULL;

e_Exit:
    SAFE_RELEASE( pd3dDevice );

    // call Destroy function to properly clean up the memory allocated 
    if( pMeshContainer != NULL )
    {
        DestroyMeshContainer( pMeshContainer );
    }

    return hr;
}




//--------------------------------------------------------------------------------------
// Name: CAllocateHierarchy::DestroyFrame()
// Desc: 
//--------------------------------------------------------------------------------------
HRESULT D3DAllocator::DestroyFrame( LPD3DXFRAME pFrameToFree )
{
    SAFE_DELETE_ARRAY( pFrameToFree->Name );
    SAFE_DELETE( pFrameToFree );
    return S_OK;
}




//--------------------------------------------------------------------------------------
// Name: CAllocateHierarchy::DestroyMeshContainer()
// Desc: 
//--------------------------------------------------------------------------------------
HRESULT D3DAllocator::DestroyMeshContainer( LPD3DXMESHCONTAINER pMeshContainerBase )
{
    //THIS IS NEVER EVEN FUCKING CALLED!
    UINT iMaterial;
    MultiAnimMC* pMeshContainer = ( MultiAnimMC* )pMeshContainerBase;

    SAFE_DELETE_ARRAY( pMeshContainer->Name );
    SAFE_DELETE_ARRAY( pMeshContainer->pAdjacency );

    for( iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++ )
    {
        SAFE_DELETE_ARRAY( pMeshContainer->pMaterials[iMaterial].pTextureFilename );
    }
    SAFE_DELETE_ARRAY( pMeshContainer->pMaterials );
    SAFE_RELEASE( pMeshContainer->m_pBufBoneCombos );
    SAFE_RELEASE( pMeshContainer->MeshData.pMesh );
    SAFE_RELEASE( pMeshContainer->pSkinInfo );
    SAFE_RELEASE( pMeshContainer->m_pWorkingMesh );
    SAFE_DELETE( pMeshContainer );
    return S_OK;
}