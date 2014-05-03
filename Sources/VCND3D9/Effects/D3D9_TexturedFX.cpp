///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Effect for textured material implementation
///

#include "Precompiled.h"
#include "D3D9_TexturedFX.h"

// Engine includes
#include "VCNRenderer/MaterialCore.h"
#include "VCNRenderer/RenderCore.h"
#include "VCNRenderer/Shader.h"
#include "VCNResources/Mesh.h"
#include "../D3D9Shader.h"

//-------------------------------------------------------------
VCNDXFX_Textured::VCNDXFX_Textured()
{
  // Load up our effect file with all our shaders inside
  mShader = VCNShaderCore::GetInstance()->LoadShader( VCNTXT("TexturedGeometry") );
  if ( VCNShaderPointer shader = mShader.lock() )
  {
    // Create the vertex declaration
    VCNDeclarationElementList declarationList;
    declarationList.push_back(VCNDeclarationElement(0, 0, ELT_USAGE_POSITION, ELT_TYPE_FLOAT3));
    declarationList.push_back(VCNDeclarationElement(1, 0, ELT_USAGE_NORMAL, ELT_TYPE_FLOAT3));
    declarationList.push_back(VCNDeclarationElement(1, 0, ELT_USAGE_TEXCOORD0,ELT_TYPE_FLOAT2));
    shader->CreateVertexDeclaration( declarationList );

    // Pick up handles to our constants
    mMaterialDiffuseColor = shader->GetParameterByName( "gMaterialDiffuseColor" );
    mMaterialDiffuseTexture = shader->GetParameterByName( "gDiffuseTexture" );

    mSelectionFunctions.CreateParameterHandles(static_cast<VCND3D9Shader*>(shader.get())->GetEffect());
  }
  else
  {
    throw VCNException( "Shader failed to load" );
  }
}

//-------------------------------------------------------------
VCNDXFX_Textured::~VCNDXFX_Textured()
{
}

//-------------------------------------------------------------
void VCNDXFX_Textured::RenderMesh( const VCNMesh* mesh, const VCNSphere& boundingSphere, const VCNEffectParamSet& effectParams )
{
  // Bind shader to context.
  VCNShaderCore::GetInstance()->SelectShader( mShader );

  if ( VCNShaderPointer shader = mShader.lock() )
  {
    mSelectionFunctions.SetSelectionState( static_cast<VCND3D9Shader*>(shader.get())->GetEffect(), effectParams);

    // Get the current material and set the color in the shader
    VCNMaterial* currentMaterial = VCNMaterialCore::GetInstance()->GetCurrentMaterial();

    const VCNResID texID = currentMaterial->GetEffectParamSet().GetResource( VCNTXT("DiffuseTexture") );
    VCN_ASSERT( texID != kInvalidResID );

    // Get the current material and set the color in the shader
    shader->SetColor( mMaterialDiffuseColor, currentMaterial->GetDiffuseColor() );
    shader->SetTexture( mMaterialDiffuseTexture, texID );

    // Have a variable to keep track of how many streams we have
    VCNUInt streamCount = 0;

    // Put positions in
    VCNRenderCore::GetInstance()->LoadVertexStream( mesh->GetCacheID( VT_POSITION ), streamCount++ );

    // Put normals and vertex colors in
    VCNRenderCore::GetInstance()->LoadVertexStream( mesh->GetCacheID( VT_DIFFUSE_TEX_COORDS ), streamCount++ );

    // Send the indexes if we need them
    if( mesh->IsIndexed() )
    {
      // If we have indexes, load those in
      VCNRenderCore::GetInstance()->LoadIndexStream( mesh->GetFaceCache() );
    }

    // Call for the right type of render
    TriggerRender( mesh, shader );
  }
}
