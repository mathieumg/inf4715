///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief  Implements shadow mapping effect
///

#include "Precompiled.h"
#include "D3D9_ShadowMapFX.h"

// Engine includes
#include "VCNRenderer/RenderCore.h"
#include "VCNRenderer/Shader.h"
#include "VCNRenderer/XformCore.h"
#include "VCNResources/Mesh.h"
#include "VCNResources/Material.h"
#include "VCNRenderer/MaterialCore.h"

///////////////////////////////////////////////////////////////////////
///
/// Constructs the effect and load the shadow map shader.
///
VCNDXFX_ShadowMap::VCNDXFX_ShadowMap()
{
  // Load up our effect file with all our shaders inside
  mShader = VCNShaderCore::GetInstance()->LoadShader( VCNTXT("ShadowMap") );
  if ( VCNShaderPointer shader = mShader.lock() )
  {
    // Create the vertex declaration
    VCNDeclarationElementList declarationList;
    declarationList.push_back(VCNDeclarationElement(0,  0, ELT_USAGE_POSITION, ELT_TYPE_FLOAT3));
    declarationList.push_back(VCNDeclarationElement(1,  0, ELT_USAGE_TEXCOORD0, ELT_TYPE_FLOAT2));
    shader->CreateVertexDeclaration( declarationList );

    // Pick up handles for our shader global parameters
    mMaxDepthParam          = shader->GetParameterByName( "gMaxDepth" );
    mMaterialDiffuseTexture = shader->GetParameterByName( "gDiffuseTexture" );
  }
  else
  {
    throw VCNException( "Shader failed to load" );
  }
}

///////////////////////////////////////////////////////////////////////
VCNDXFX_ShadowMap::~VCNDXFX_ShadowMap()
{
}

///////////////////////////////////////////////////////////////////////
void VCNDXFX_ShadowMap::RenderMesh(const VCNMesh* mesh, const VCNSphere& boundingSphere, const VCNEffectParamSet& effectParams)
{
  if( VCNShaderPointer shader = mShader.lock() )
  {
    // Get a pointer to the renderer
    VCNRenderCore* renderer = VCNRenderCore::GetInstance();

    // Bind shader to context.
    VCNShaderCore::GetInstance()->SelectShader( mShader );

    // Get the current material and set the color in the shader
    VCNMaterial* currentMaterial = VCNMaterialCore::GetInstance()->GetCurrentMaterial();

    // Set object diffuse texture
    const VCNResID diffuseTextureID = currentMaterial->GetEffectParamSet().GetResource( VCNTXT("DiffuseTexture") );
    VCN_ASSERT( diffuseTextureID != kInvalidResID );
    shader->SetTexture( mMaterialDiffuseTexture, diffuseTextureID );
    
    // Prepare streams
    if ( LoadVertexCache( mesh, VT_POSITION, 0 ) )
    {
      if( LoadVertexCache( mesh, VT_DIFFUSE_TEX_COORDS, 1 ) )
      {
        // Send the indexes if we need them
        if( mesh->IsIndexed() )
        {
          // If we have indexes, load those in
          renderer->LoadIndexStream( mesh->GetFaceCache() );
        }

        // Render geometry
        TriggerRender( mesh, shader );
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////
void VCNDXFX_ShadowMap::Prepare()
{
  if ( VCNShaderPointer shader = mShader.lock() )
  {
    shader->SetFloat( mMaxDepthParam, VCNXformCore::GetInstance()->GetFar() );
  }
}
