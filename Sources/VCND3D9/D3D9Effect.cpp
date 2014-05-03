///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief DirectX effect implementation
///

#include "Precompiled.h"
#include "D3D9Effect.h"

#include "D3D9EffectCore.h"
#include "D3D9Shader.h"
#include "D3D9ShaderCore.h"
#include "VCNRenderer/RenderCore.h"
#include "VCNResources/Mesh.h"
#include "VCNUtils/Constants.h"

//-------------------------------------------------------------
VCND3D9Effect::VCND3D9Effect()
{
}

//-------------------------------------------------------------
VCND3D9Effect::~VCND3D9Effect()
{
}

VCNBool VCND3D9Effect::PrepareRender( const VCNMesh* mesh, class VCND3D9Shader* shader )
{
  // Have a variable to keep track of how many streams we have
  VCNUInt streamCount = 0;

  // Put positions in
  if ( LoadVertexCache( mesh, VT_POSITION, streamCount ) )
    ++streamCount;

  // Put normals and vertex colors in
  if ( LoadVertexCache( mesh, VT_LIGHTING, streamCount ) )
    ++streamCount;

  // Put diffuse tex coords in
  LoadVertexCache( mesh, VT_DIFFUSE_TEX_COORDS, streamCount );
    
  // Send the indexes if we need them
  if( mesh->IsIndexed() )
  {
    // If we have indexes, load those in
    VCNRenderCore::GetInstance()->LoadIndexStream( mesh->GetFaceCache() );
  }

  return true;
}

///////////////////////////////////////////////////////////////////////
VCNBool VCND3D9Effect::TriggerRender( const VCNMesh* mesh, VCND3D9Shader* shader )
{
  // Call for the right type of render
  if( mesh->IsIndexed() )
  {
    // Render the mesh
    shader->RenderIndexedPrimitive( mesh->GetFaceCount(), mesh->GetVertexCount(), mesh->GetPrimitiveType() );
  }
  else
  {
    shader->RenderPrimitive( mesh->GetFaceCount(), (VCNInt)mesh->GetPrimitiveType() );
  }

  return true;
}



///////////////////////////////////////////////////////////////////////
VCNBool VCND3D9Effect::TriggerRender(const VCNMesh* mesh, VCNShaderPointer shader)
{
  return TriggerRender( mesh, static_cast<VCND3D9Shader*>(shader.get()) );
}


///////////////////////////////////////////////////////////////////////
VCNBool VCND3D9Effect::LoadVertexCache(const VCNMesh* mesh, const VCNCacheType vertexType, VCNUInt streamIndex)
{
  VCNResID cacheID = mesh->GetCacheID( vertexType );
  if ( cacheID != kInvalidResID )
  {
    VCNRenderCore::GetInstance()->LoadVertexStream( cacheID, streamIndex );

    return true;
  }

  return false; 
}
