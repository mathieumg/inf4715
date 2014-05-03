///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Basic effect implementation
///

#include "Precompiled.h"
#include "D3D9_BasicFX.h"

// Engine includes
#include "VCNRenderer/MaterialCore.h"
#include "VCNRenderer/Shader.h"
#include "VCNRenderer/ShaderCore.h"
#include "VCNResources/Mesh.h"

///////////////////////////////////////////////////////////////////////
VCNDXFX_Basic::VCNDXFX_Basic()
{
  // Load up our effect file with all our shaders inside
  mShader = VCNShaderCore::GetInstance()->LoadShader( VCNTXT("BasicGeometry") );
  if ( VCNShaderPointer shader = mShader.lock() )
  {
    // Create the vertex declaration
    VCNDeclarationElementList declarationList;
    declarationList.push_back(VCNDeclarationElement(0,  0, ELT_USAGE_POSITION, ELT_TYPE_FLOAT3));
    declarationList.push_back(VCNDeclarationElement(1, 12, ELT_USAGE_COLOR,  ELT_TYPE_FLOAT3));
    shader->CreateVertexDeclaration( declarationList );

    // Pick up handles to our constants
    mColorParam = shader->GetParameterByName( "gColor" );
  }
  else
  {
    VCN_ASSERT_FAIL( "Shader failed to load" );
  }
}

///////////////////////////////////////////////////////////////////////
VCNDXFX_Basic::~VCNDXFX_Basic()
{
}

///////////////////////////////////////////////////////////////////////
void VCNDXFX_Basic::RenderMesh( const VCNMesh* mesh, const VCNSphere& boundingSphere, const VCNEffectParamSet& effectParams )
{  
  if(VCNShaderPointer shader = mShader.lock())
  {
    // Get a pointer to the renderer
    VCNRenderCore* renderer = VCNRenderCore::GetInstance();

    // Bind shader to context.
    VCNShaderCore::GetInstance()->SelectShader( mShader );
    
    // Get the current material and set the color in the shader
    VCNMaterial* currentMaterial = VCNMaterialCore::GetInstance()->GetCurrentMaterial();
    shader->SetColor( mColorParam, currentMaterial->GetAmbientColor() );
    
    // Put positions in
    VCNUInt streamCount = 0;
    renderer->LoadVertexStream( mesh->GetCacheID( VT_POSITION ), streamCount++ );
    renderer->LoadVertexStream( mesh->GetCacheID( VT_LIGHTING ), streamCount++ );

    // Replace existing colors
    renderer->SetBlendMode( RS_BLEND_NONE );
    renderer->SetDepthBufferMode( RS_DEPTH_READWRITE );
    renderer->SetBackfaceCulling( RS_CULL_CCW );

    if( mesh->GetFaceCache() != kInvalidResID )
    {
      // If we have indexes, load those in
      renderer->LoadIndexStream( mesh->GetFaceCache() );

      // Call for an indexed render
      shader->RenderIndexedPrimitive( mesh->GetFaceCount(), mesh->GetVertexCount() );
    }
    else
    {
      // Call for an non-indexed render
      shader->RenderPrimitive( mesh->GetFaceCount() );
    }
  }
}
