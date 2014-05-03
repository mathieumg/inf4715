///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief DirectX mesh wrapper implementation
///

#include "Precompiled.h"
#include "D3D9Mesh.h"

// Engine includes
#include "VCND3D9/D3D9.h"
#include "VCND3D9/D3D9Effect.h"
#include "VCND3D9/D3D9Texture.h"
#include "VCNResources/ResourceCore.h"
#include "VCNUtils/StringUtils.h"
#include <atlcomcli.h>
#include "VCNResources/Material.h"
#include "VCNUtils/RenderTypes.h"

///////////////////////////////////////////////////////////////////////
VCNDXMesh::VCNDXMesh()
{
}

///////////////////////////////////////////////////////////////////////
VCNDXMesh::~VCNDXMesh()
{
}

////////////////////////////////////////////////////////////////////////
///
/// Loads DirectX mesh file. 
///
/// @param  filename  Filename to load. 
///
/// @return success or failure. 
///
////////////////////////////////////////////////////////////////////////
const VCNBool VCNDXMesh::LoadFromFile( const VCNString& filename )
{
  VCND3D9* renderer = static_cast<VCND3D9*>( VCNRenderCore::GetInstance() );
  LPDIRECT3DDEVICE9 d3dDevice = renderer->GetD3DDevice();

  CComPtr<ID3DXMesh> systemMesh;
  CComPtr<ID3DXBuffer> materialBuffer;

  // Load the mesh from the specified file
  //
  DWORD numMaterials = 0;
  HRESULT hr = D3DXLoadMeshFromX(filename.c_str(), D3DXMESH_SYSTEMMEM,
    d3dDevice, NULL, &materialBuffer,NULL, &numMaterials, &systemMesh );
  if ( FAILED(hr) )
    return false;
  
  // Load materials
  //
  D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)materialBuffer->GetBufferPointer();
  for (DWORD i = 0; i < numMaterials; ++i)
  {
    VCN_ASSERT_MSG( i == 0 || mMaterialID == kInvalidResID, VCNTXT("We do not support multiple material per meshes") );
    
    // Create the texture if it exists - it may not
    if (d3dxMaterials[i].pTextureFilename)
    {
      VCNString texturePath = VCNTXT("Textures/");
      texturePath += VCN_A2W(d3dxMaterials[i].pTextureFilename);

      // Check if the texture is already loaded
      VCNResID texID = kInvalidResID;
      VCND3D9Texture* resTexture = VCNResourceCore::GetInstance()->GetResource<VCND3D9Texture>(texturePath);
      if (!resTexture)
      {
        LPDIRECT3DTEXTURE9 d3dTexture = NULL;
        hr = D3DXCreateTextureFromFile(d3dDevice, texturePath.c_str(), &d3dTexture);
        VCN_ASSERT_MSG( SUCCEEDED(hr), _T("Can't load texture [%s]"), texturePath.c_str() );

        resTexture = new VCND3D9Texture( d3dTexture );
        resTexture->SetName( texturePath );
        texID = VCNResourceCore::GetInstance()->AddResource( texturePath, resTexture );
      }
      else
      {
        texID = resTexture->GetResourceID();
      }

      VCNMaterial* material = new VCNMaterial();
      material->SetName( VCNString(VCNTXT("material_dx_mesh_")) + filename );
      material->SetAmbientColor( VCNColor((const VCNFloat*)&d3dxMaterials[i].MatD3D.Ambient) );
      material->SetDiffuseColor( VCNColor((const VCNFloat*)&d3dxMaterials[i].MatD3D.Diffuse) );
      material->SetSpecularColor( VCNColor((const VCNFloat*)&d3dxMaterials[i].MatD3D.Specular) );
      material->SetSpecularPower( d3dxMaterials[i].MatD3D.Power );

      VCNEffectParamSet& params = material->GetEffectParamSet();
      params.SetEffectID( eidLitTextured );
      params.AddResource( VCNTXT("DiffuseTexture"), texID );

      // Add material as a resource.
      mMaterialID = VCNResourceCore::GetInstance()->AddResource( material->GetName(), material );
    }
  }
  
  // Optimize the mesh if possible
  //
  const VCNUInt faceCount = systemMesh->GetNumFaces();
  DWORD* adjac = new DWORD[faceCount*3];
  hr = systemMesh->GenerateAdjacency(0.5f, adjac);
  hr = systemMesh->OptimizeInplace(D3DXMESHOPT_VERTEXCACHE, adjac, NULL, NULL, NULL);

  // Calculate Tangent and Binormal
  hr = D3DXComputeTangentFrameEx(systemMesh, 
                                    D3DDECLUSAGE_TEXCOORD, 0,
                                    D3DDECLUSAGE_TANGENT, 0,
                                    D3DDECLUSAGE_BINORMAL, 0,
                                    D3DDECLUSAGE_NORMAL, 0,
                                    D3DXTANGENT_DONT_ORTHOGONALIZE | D3DXTANGENT_WEIGHT_BY_AREA,
                                    adjac,
                                    -1.01f, -0.01f, -1.01f,
                                    &systemMesh,
                                    NULL);
  delete [] adjac;

  // Load caches
  //
  const VCNUInt vertexCount = systemMesh->GetNumVertices();
  const DWORD meshFVF = systemMesh->GetFVF();
  const DWORD stride = D3DXGetFVFVertexSize( meshFVF );
  const DWORD positionStride = D3DXGetFVFVertexSize( D3DFVF_XYZ );
  const DWORD normalStride = D3DXGetFVFVertexSize( D3DFVF_NORMAL );
  const DWORD diffuseStride = D3DXGetFVFVertexSize( D3DFVF_DIFFUSE );
  const DWORD textureStride = D3DXGetFVFVertexSize( D3DFVF_TEX1 );

  VCNFloat* vtPositionBufStart = new VCNFloat[vertexCount * kCacheStrides[VT_POSITION]];
  VCNFloat* vtNormalBufStart = new VCNFloat[vertexCount * kCacheStrides[VT_LIGHTING]];
  VCNFloat* vtTextureBufStart = new VCNFloat[vertexCount * kCacheStrides[VT_DIFFUSE_TEX_COORDS]];
  
  VCNFloat* vtPositionBuf = vtPositionBufStart;
  VCNFloat* vtNormalBuf = vtNormalBufStart;
  VCNFloat* vtTextureBuf = vtTextureBufStart;
  
  BYTE* vbptr = NULL;
  systemMesh->LockVertexBuffer(D3DLOCK_READONLY, (LPVOID*)&vbptr);
  
  for(VCNUInt i = 0; i < vertexCount; ++i)
  {
    if ( meshFVF == (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1) )
    {
      // Read position
      D3DXVECTOR3* pos = (D3DXVECTOR3*)vbptr;
      *vtPositionBuf = pos->x; vtPositionBuf++;
      *vtPositionBuf = pos->y; vtPositionBuf++;
      *vtPositionBuf = pos->z; vtPositionBuf++;

      // Read normal
      D3DXVECTOR3* normal = (D3DXVECTOR3*)(vbptr + positionStride);
      *vtNormalBuf = normal->x; vtNormalBuf++;
      *vtNormalBuf = normal->y; vtNormalBuf++;
      *vtNormalBuf = normal->z; vtNormalBuf++;

      // Set default diffuse color
      std::fill(vtNormalBuf, vtNormalBuf+3, 1.0f);
      vtNormalBuf += 3;

      float* texCoords = (float*)(vbptr + positionStride + normalStride);
      *vtTextureBuf = texCoords[0]; vtTextureBuf++;
      *vtTextureBuf = texCoords[1]; vtTextureBuf++;

      vbptr += stride;
    }
    else
    {
      VCN_ASSERT_FAIL( VCNTXT("Mesh FVF not supported [FVF(%d) stride = %d]"), meshFVF, stride );
    }
  }
  systemMesh->UnlockVertexBuffer();

  VCNResID positionCache = renderer->CreateCache(VT_POSITION, vtPositionBufStart, vertexCount * kCacheStrides[VT_POSITION]);
  VCNResID lightingCache = renderer->CreateCache(VT_LIGHTING, vtNormalBufStart, vertexCount * kCacheStrides[VT_LIGHTING]);
  VCNResID textureCache = renderer->CreateCache(VT_DIFFUSE_TEX_COORDS, vtTextureBufStart, vertexCount * kCacheStrides[VT_DIFFUSE_TEX_COORDS]);

  SetCacheID(VT_POSITION, positionCache);
  SetCacheID(VT_LIGHTING, lightingCache);
  SetCacheID(VT_DIFFUSE_TEX_COORDS, textureCache);

  delete [] vtPositionBufStart;
  delete [] vtNormalBufStart;
  delete [] vtTextureBufStart;

  VCNUShort* ibptr = NULL;
  VCNUShort* indices = new VCNUShort[faceCount * 3];
  systemMesh->LockIndexBuffer(D3DLOCK_READONLY, (LPVOID*)&ibptr);

  for(VCNUInt i = 0; i < systemMesh->GetNumFaces(); i++)
  {
    indices[(i * 3) + 0] = *(ibptr++);
    indices[(i * 3) + 1] = *(ibptr++);
    indices[(i * 3) + 2] = *(ibptr++);
  }

  systemMesh->UnlockIndexBuffer();

  VCNResID indexCacheID = renderer->CreateCache(VT_INDEX, indices, faceCount * 3 * kCacheStrides[VT_INDEX]);
  SetFaceCache(indexCacheID);
  SetFaceCount(faceCount);
  SetPrimitiveType(PT_TRIANGLELIST);

  delete [] indices;
  
  // Compute bounding sphere
  if ( !ComputeBoundingSphere(systemMesh) )
    return false;

  return true;
}


////////////////////////////////////////////////////////////////////////
///
/// Calculates the bounding sphere. 
///
/// @return true if the bounding sphere was properly calculated
///
////////////////////////////////////////////////////////////////////////
const VCNBool VCNDXMesh::ComputeBoundingSphere(ID3DXMesh* d3dMesh)
{
  D3DXVECTOR3 center;
  float radius;
  DWORD numVertices = d3dMesh->GetNumVertices();
  DWORD fvfSize = D3DXGetFVFVertexSize( d3dMesh->GetFVF() );
  char* pData = NULL;
  if ( FAILED( d3dMesh->LockVertexBuffer( 0, (void**)&pData ) ) )
  {
    VCN_ASSERT_FAIL( VCNTXT("Failed to lock mesh vertex buffer.") );
    return false;
  }
  D3DXComputeBoundingSphere( (D3DXVECTOR3*)pData, numVertices, fvfSize, &center, &radius );
  mBoundingSphere = VCNSphere(radius, V2V<Vector3>(center));


  // ***Hack use same flow as Bounding sphere
  D3DXVECTOR3 aabbMin, aabbMax;
  D3DXComputeBoundingBox((D3DXVECTOR3*)pData, numVertices, fvfSize, &aabbMin, &aabbMax);
  mBoundingAabb = VCNAabb(V2V<Vector3>(aabbMin), V2V<Vector3>(aabbMax));
  if ( FAILED( d3dMesh->UnlockVertexBuffer() ) )
  {
	  VCN_ASSERT_FAIL( VCNTXT("Failed to unlock mesh vertex buffer.") );
	  return false;
  } 

  return true;
}
