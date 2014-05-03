///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Direct3D lighting effect implementation
///

#include "Precompiled.h"

#include "D3D9_LitFX.h"

// Engine includes
#include "VCND3D9/D3D9Shader.h"
#include "VCNRenderer/LightingCore.h"
#include "VCNRenderer/MaterialCore.h"
#include "VCNRenderer/RenderCore.h"
#include "VCNRenderer/Shader.h"
#include "VCNResources/Mesh.h"


///////////////////////////////////////////////////////////////////////
VCNDXFX_Lit::VCNDXFX_Lit()
{
  // Load up our effect file with all our shaders inside
  mShader = VCNShaderCore::GetInstance()->LoadShader( VCNTXT("LitGeometry") );
  if(VCNShaderPointer shader = mShader.lock())
  {
    // Create the vertex declaration
    VCNDeclarationElementList declarationList;
    declarationList.push_back(VCNDeclarationElement(0,  0, ELT_USAGE_POSITION, ELT_TYPE_FLOAT3));
    declarationList.push_back(VCNDeclarationElement(1,  0, ELT_USAGE_NORMAL,   ELT_TYPE_FLOAT3));
    declarationList.push_back(VCNDeclarationElement(1, 12, ELT_USAGE_COLOR,    ELT_TYPE_FLOAT3));
    shader->CreateVertexDeclaration( declarationList );

    // Pick up handles to our constants
    mMaterialAmbientColor = shader->GetParameterByName( "gMaterialAmbientColor" );
    mMaterialDiffuseColor = shader->GetParameterByName( "gMaterialDiffuseColor" );
    mMaterialSpecularColor = shader->GetParameterByName( "gMaterialSpecularColor" );
    mMaterialSpecularPower = shader->GetParameterByName( "gMaterialSpecularPower" );
    
    mLightingFunctions.CreateParameterHandles( static_cast<VCND3D9Shader*>(shader.get())->GetEffect() );
  }
  else
  {
    throw VCNException( "Shader failed to load" );
  }
}


///////////////////////////////////////////////////////////////////////
VCNDXFX_Lit::~VCNDXFX_Lit()
{
}


///////////////////////////////////////////////////////////////////////
void VCNDXFX_Lit::RenderMesh( const VCNMesh* mesh, const VCNSphere& boundingSphere, const VCNEffectParamSet& effectParams )
{
  // Bind shader to context.
  VCNShaderCore::GetInstance()->SelectShader( mShader );

  // Make our rounds
  PerformLightingPasses( mesh, boundingSphere, effectParams );
}


///////////////////////////////////////////////////////////////////////
void VCNDXFX_Lit::PerformLightingPasses( const VCNMesh* mesh, const VCNSphere& boundingSphere, const VCNEffectParamSet& effectParams )
{
  // Find all the lights that touch us
  std::vector<VCNLight*> lights;
  VCNLightingCore::GetInstance()->IntersectLights( boundingSphere, lights );

  if( !lights.empty() )
  { 
    if ( VCNShaderPointer shader = mShader.lock() )
    {
      VCNMaterial* currentMaterial = VCNMaterialCore::GetInstance()->GetCurrentMaterial();
      VCN_ASSERT( currentMaterial );

      // Get the current material and set the color in the shader
      shader->SetColor( mMaterialAmbientColor, currentMaterial->GetAmbientColor() );
      shader->SetColor( mMaterialDiffuseColor, currentMaterial->GetDiffuseColor() );
      shader->SetColor( mMaterialSpecularColor, currentMaterial->GetSpecularColor() );
      shader->SetFloat( mMaterialSpecularPower, currentMaterial->GetSpecularPower() );

      // Have a variable to keep track of how many streams we have
      VCNUInt streamCount = 0;

      // Put positions in
      VCNRenderCore::GetInstance()->LoadVertexStream( mesh->GetCacheID( VT_POSITION ), streamCount++ );

      // Put normals and vertex colors in
      VCNRenderCore::GetInstance()->LoadVertexStream( mesh->GetCacheID( VT_LIGHTING ), streamCount++ );

      // Send the indexes if we need them
      if( mesh->IsIndexed() )
      {
        // If we have indexes, load those in
        VCNRenderCore::GetInstance()->LoadIndexStream( mesh->GetFaceCache() );
      }

      // Make an ambient pass on the mesh
      if( VCNLightingCore::GetInstance()->HasAmbientLight() )
      {
        // Have the shader read its params
        mLightingFunctions.InitAmbientPass( 
          std::static_pointer_cast<VCND3D9Shader>(shader)->GetEffect(), 
          VCNLightingCore::GetInstance()->GetAmbientLight() );

        // Call for the right type of render
        TriggerRender( mesh, static_cast<VCND3D9Shader*>(shader.get()) );
      }

      // Render once for each light
      for( VCNUInt i=0; i<lights.size(); i++ )
      {
        // Have the shader read its params
        mLightingFunctions.InitLightPass( 
          std::static_pointer_cast<VCND3D9Shader>(shader)->GetEffect(), 
          *lights[i] );

        // Call for the right type of render
        TriggerRender( mesh, shader );
      }

      VCNRenderCore::GetInstance()->SetDepthBufferMode( RS_DEPTH_READWRITE );
    }
  }
}
