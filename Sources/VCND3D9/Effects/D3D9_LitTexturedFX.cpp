///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#include "Precompiled.h"
#include "D3D9_LitTexturedFX.h"

#include "VCND3D9/D3D9MaterialCore.h"
#include "VCND3D9/D3D9ShaderCore.h"
#include "VCND3D9//D3D9Shader.h"

#include "VCNNodes/Light.h"
#include "VCNRenderer/LightingCore.h"
#include "VCNRenderer/XformCore.h"
#include "VCNRenderer/Shader.h"
#include "VCNRenderer/GPUProfiling.h"

///////////////////////////////////////////////////////////////////////
VCND3D9_LitTexturedFX::VCND3D9_LitTexturedFX()
{
  // Load up our effect file with all our shaders inside
  mShader = VCNShaderCore::GetInstance()->LoadShader( VCNTXT("LitTexturedGeometry") );

  if(VCNShaderPointer shader = mShader.lock())
  {
    // Create the vertex declaration
    VCNDeclarationElementList declarationList;
    declarationList.push_back(VCNDeclarationElement(0,  0, ELT_USAGE_POSITION,  ELT_TYPE_FLOAT3));
    declarationList.push_back(VCNDeclarationElement(1,  0, ELT_USAGE_NORMAL,    ELT_TYPE_FLOAT3));
    declarationList.push_back(VCNDeclarationElement(1, 12, ELT_USAGE_COLOR,     ELT_TYPE_FLOAT3));
    declarationList.push_back(VCNDeclarationElement(2,  0, ELT_USAGE_TEXCOORD0, ELT_TYPE_FLOAT2));
    shader->CreateVertexDeclaration( declarationList );

    // Pick up handles to our constants
    mMaterialAmbientColor     = shader->GetParameterByName( "gMaterialAmbientColor" );
    mMaterialDiffuseColor     = shader->GetParameterByName( "gMaterialDiffuseColor" );
    mMaterialSpecularColor    = shader->GetParameterByName( "gMaterialSpecularColor" );
    mMaterialSpecularPower    = shader->GetParameterByName( "gMaterialSpecularPower" );

    mMaterialDiffuseTexture   = shader->GetParameterByName( "gDiffuseTexture" );

    mMaxDepth                 = shader->GetParameterByName( "gMaxDepth" );
    mLightMVP                 = shader->GetParameterByName( "gLightWorldViewProjection" );
    mShadowMapTexture         = shader->GetParameterByName( "gShadowMapTexture" );

    mLightingFunctions.CreateParameterHandles( static_cast<VCND3D9Shader*>(shader.get())->GetEffect() );
    mSelectionFunctions.CreateParameterHandles( static_cast<VCND3D9Shader*>(shader.get())->GetEffect() );
  }
  else
  {
    VCN_ASSERT_FAIL( "Shader failed to load" );
  }
}



///////////////////////////////////////////////////////////////////////
VCND3D9_LitTexturedFX::~VCND3D9_LitTexturedFX()
{
}



///////////////////////////////////////////////////////////////////////
void VCND3D9_LitTexturedFX::Prepare()
{
  // Set frame specific shader parameters
  //
  if(VCNShaderPointer shader = mShader.lock())
  {
    // TODO: Use multiple light source for shadowing
    VCNLight* shadowLightSource = VCNLightingCore::GetInstance()->GetLight(0);
    VCN_ASSERT( shadowLightSource );

    mLightViewProjectionMatrix = shadowLightSource->GetViewMatrix() * shadowLightSource->GetProjectionMatrix();

    shader->SetFloat( mMaxDepth, VCNXformCore::GetInstance()->GetFar() );
  }
}



///////////////////////////////////////////////////////////////////////
void VCND3D9_LitTexturedFX::RenderMesh(const VCNMesh* mesh, const VCNSphere& boundingSphere, const VCNEffectParamSet& effectParams)
{
  // Put in the only shader we'll use
  VCNShaderCore::GetInstance()->SelectShader( mShader );

  if ( VCNShaderPointer shader = mShader.lock() )
  {
      mSelectionFunctions.SetSelectionState( static_cast<VCND3D9Shader*>(shader.get())->GetEffect(), effectParams);
  }

  // Make our rounds
  PerformLightingPasses( mesh, boundingSphere, effectParams );
}



///////////////////////////////////////////////////////////////////////
void VCND3D9_LitTexturedFX::PerformLightingPasses( const VCNMesh* mesh, const VCNSphere& boundingSphere, const VCNEffectParamSet& effectParams )
{
  // Find all the lights that touch us
  mLights.clear();
  VCNLightingCore::GetInstance()->IntersectLights( boundingSphere, mLights );

  if( !mLights.empty() )
  {
    if ( VCNShaderPointer shader = mShader.lock() )
    {
      // Get the current material and set the color in the shader
      VCNMaterial* currentMaterial = VCNMaterialCore::GetInstance()->GetCurrentMaterial();
      VCN_ASSERT( currentMaterial );

      // Get the current material and set the color in the shader
      shader->SetColor( mMaterialAmbientColor, currentMaterial->GetAmbientColor() );
      shader->SetColor( mMaterialDiffuseColor, currentMaterial->GetDiffuseColor() );
      shader->SetColor( mMaterialSpecularColor, currentMaterial->GetSpecularColor() );
      shader->SetFloat( mMaterialSpecularPower, currentMaterial->GetSpecularPower() );

      // Set the diffuse texture
      VCNResID texID = currentMaterial->GetEffectParamSet().GetResource( VCNTXT("DiffuseTexture") );
      VCN_ASSERT( texID != kInvalidResID );
      shader->SetTexture( mMaterialDiffuseTexture, texID );

      const Matrix4 lightTransform = VCNXformCore::GetInstance()->GetWorld() * mLightViewProjectionMatrix;
      shader->SetMatrix( mLightMVP, lightTransform );

      // Prepare streams
      PrepareRender( mesh, static_cast<VCND3D9Shader*>(shader.get()) );

      // Make an ambient pass on the mesh
      if( VCNLightingCore::GetInstance()->HasAmbientLight() )
      {
        GPU_PROFILE_BLOCK_NAME( VCNTXT("Ambient Lighting") );

        // Have the shader read its params
        mLightingFunctions.InitAmbientPass( 
          static_cast<VCND3D9Shader*>(shader.get())->GetEffect(), 
          VCNLightingCore::GetInstance()->GetAmbientLight() );

        // Call for the right type of render
        TriggerRender( mesh, static_cast<VCND3D9Shader*>(shader.get()) );
      }

      // Render once for each light
      for (VCNUInt i = 0, end = mLights.size(); i < end; ++i)
      {
        GPU_PROFILE_BLOCK_NAME_IDX( VCNTXT("Light"), i );

        // Have the shader read its params
        mLightingFunctions.InitLightPass( static_cast<VCND3D9Shader*>(shader.get())->GetEffect(), *mLights[i] );

        // Call for the right type of render
        TriggerRender( mesh, static_cast<VCND3D9Shader*>(shader.get()) );
      }

      VCNRenderCore::GetInstance()->SetDepthBufferMode( RS_DEPTH_READWRITE );
    }
  }
}


///////////////////////////////////////////////////////////////////////
void VCND3D9_LitTexturedFX::SetShadowMapTexture(LPDIRECT3DTEXTURE9 texture)
{
  // TODO: Make the effect pull on the shadow map texture somehow
  if ( VCNShaderPointer shader = mShader.lock() )
  {
    static_cast<VCND3D9Shader*>(shader.get())->GetEffect()->SetTexture( (D3DXHANDLE)mShadowMapTexture, texture );
  }
}
