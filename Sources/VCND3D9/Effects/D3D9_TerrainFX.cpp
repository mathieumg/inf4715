///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#include "Precompiled.h"
#include "D3D9_TerrainFX.h"

// Header to cores we'll need
#include "VCND3D9/D3D9MaterialCore.h"
#include "VCND3D9/D3D9Shader.h"
#include "VCND3D9/D3D9ShaderCore.h"
#include "VCNNodes/Light.h"
#include "VCNRenderer/LightingCore.h"
#include "VCNRenderer/XformCore.h"
#include "VCNRenderer/DirectionalLight.h"

// Material params used by this shaders
namespace
{
  const VCNStringHash kGrassTextureParamName( VCNTXT("GrassTexture") );
  const VCNStringHash kSandTextureParamName( VCNTXT("SandTexture") );
  const VCNStringHash kSnowTextureParamName( VCNTXT("SnowTexture") );
  const VCNStringHash kGrassRockTextureParamName( VCNTXT("GrassRockTexture") );
  const VCNStringHash kSnowRockTextureParamName( VCNTXT("SnowRockTexture") );
}

///////////////////////////////////////////////////////////////////////
VCND3D9_TerrainFX::VCND3D9_TerrainFX()
  : mSnowHeightValue(45.0f)
  , mSandHeightValue(5.0f)
  , mRockCosAngleValue(0.80f)
  , mHeightTransitionValue(86.0f)
  , mCosAngleTransitionValue(0.10f)
{
  // Load up our effect file with all our shaders inside
  mShaderHandle = VCNShaderCore::GetInstance()->LoadShader( VCNTXT("Terrain") );

  if(VCNShaderPointer shader = mShaderHandle.lock())
  {
    // Create the vertex declaration
    VCNDeclarationElementList declarationList;
    declarationList.push_back(VCNDeclarationElement(0,  0, ELT_USAGE_POSITION,  ELT_TYPE_FLOAT3));
    declarationList.push_back(VCNDeclarationElement(1,  0, ELT_USAGE_NORMAL,    ELT_TYPE_FLOAT3));
    declarationList.push_back(VCNDeclarationElement(1, 12, ELT_USAGE_COLOR,     ELT_TYPE_FLOAT3));
    declarationList.push_back(VCNDeclarationElement(2,  0, ELT_USAGE_TEXCOORD0, ELT_TYPE_FLOAT2));
    shader->CreateVertexDeclaration( declarationList );

    // Pick up handles to our constants
    mMaterialAmbientColor             = shader->GetParameterByName( "gMaterialAmbientColor" );
    mMaterialDiffuseColor             = shader->GetParameterByName( "gMaterialDiffuseColor" );
    mMaterialSpecularColor            = shader->GetParameterByName( "gMaterialSpecularColor" );
    mMaterialSpecularPower            = shader->GetParameterByName( "gMaterialSpecularPower" );

    mMaterialGrassDiffuseTexture      = shader->GetParameterByName( "gGrassTexture" );
    mMaterialSandDiffuseTexture       = shader->GetParameterByName( "gSandTexture" );
    mMaterialSnowDiffuseTexture       = shader->GetParameterByName( "gSnowTexture" );
    mMaterialGrassRockDiffuseTexture  = shader->GetParameterByName( "gGrassRockTexture" );
    mMaterialSnowRockDiffuseTexture   = shader->GetParameterByName( "gSnowRockTexture" );

    mSnowHeight                       = shader->GetParameterByName( "gSnowHeight" );
    mSandHeight                       = shader->GetParameterByName( "gSandHeight" );
    mRockCosAngle                     = shader->GetParameterByName( "gRockCosAngle" );
    mHeightTransition                 = shader->GetParameterByName( "gHeightTransition" );
    mCosAngleTransition               = shader->GetParameterByName( "gCosAngleTransition" );

    mMaxDepth                         = shader->GetParameterByName( "gMaxDepth" );
    mLightModelViewProjection         = shader->GetParameterByName( "gLightWorldViewProjection" );
    mShadowMapTexture                 = shader->GetParameterByName( "gShadowMapTexture" );

    mLightingFunctions.CreateParameterHandles( static_cast<VCND3D9Shader*>(shader.get())->GetEffect() );

    // Validate parameters
    VCN_ASSERT( mMaterialAmbientColor && mMaterialDiffuseColor && mMaterialSpecularColor && mMaterialSpecularPower &&
                mMaterialGrassDiffuseTexture && mMaterialSandDiffuseTexture && mMaterialSnowDiffuseTexture && mMaterialGrassRockDiffuseTexture && mMaterialSnowRockDiffuseTexture &&
                mSnowHeight && mSandHeight && mRockCosAngle && mHeightTransition && mCosAngleTransition &&
                mMaxDepth && mLightModelViewProjection && mShadowMapTexture );

    // Create the tweaking toolbar
    CreateToolbar();
  }
  else
  {
    VCN_ASSERT_FAIL( "Shader failed to load" );
  }
}


///////////////////////////////////////////////////////////////////////
VCND3D9_TerrainFX::~VCND3D9_TerrainFX()
{
}


///////////////////////////////////////////////////////////////////////
void VCND3D9_TerrainFX::Prepare()
{
  // Set frame specific shader parameters
  //
  if(VCNShaderPointer shader = mShaderHandle.lock())
  {
    // TODO: Use multiple light source for shadowing
    VCNLight* shadowLightSource = VCNLightingCore::GetInstance()->GetLight(0);
    VCN_ASSERT( shadowLightSource && shadowLightSource->Is<VCNDirectionalLight>() );

    mLightViewProjectionMatrix = shadowLightSource->GetViewMatrix() * shadowLightSource->GetProjectionMatrix();

    shader->SetFloat( mMaxDepth, VCNXformCore::GetInstance()->GetFar() );
  }
}

///////////////////////////////////////////////////////////////////////
void VCND3D9_TerrainFX::RenderMesh(const VCNMesh* mesh, const VCNSphere& boundingSphere, const VCNEffectParamSet& effectParams)
{
  // Put in the only shader we'll use
  VCNShaderCore::GetInstance()->SelectShader( mShaderHandle );

  // Make our rounds
  PerformLightingPasses( mesh, boundingSphere, effectParams );
}


///////////////////////////////////////////////////////////////////////
void VCND3D9_TerrainFX::PerformLightingPasses(const VCNMesh* mesh, 
                                              const VCNSphere& boundingSphere, 
                                              const VCNEffectParamSet& effectParams)
{
  // Find all the lights that touch us
  mLights.clear();
  VCNLightingCore::GetInstance()->IntersectLights( boundingSphere, mLights );

  if( !mLights.empty() )
  {
    if ( VCNShaderPointer shader = mShaderHandle.lock() )
    {
      // Get the current material and set the color in the shader
      VCNMaterial* currentMaterial = VCNMaterialCore::GetInstance()->GetCurrentMaterial();
      VCN_ASSERT( currentMaterial );

      // Get the current material and set the color in the shader
      shader->SetColor( mMaterialAmbientColor, currentMaterial->GetAmbientColor() );
      shader->SetColor( mMaterialDiffuseColor, currentMaterial->GetDiffuseColor() );
      shader->SetColor( mMaterialSpecularColor, currentMaterial->GetSpecularColor() );
      shader->SetFloat( mMaterialSpecularPower, currentMaterial->GetSpecularPower() );

      // Set the diffuse textures
      const VCNResID grassTexID = currentMaterial->GetEffectParamSet().GetResource( kGrassTextureParamName );
      const VCNResID sandTexID = currentMaterial->GetEffectParamSet().GetResource( kSandTextureParamName );
      const VCNResID snowTexID = currentMaterial->GetEffectParamSet().GetResource( kSnowTextureParamName );
      const VCNResID grassRockTexID = currentMaterial->GetEffectParamSet().GetResource( kGrassRockTextureParamName );
      const VCNResID snowRockTexID = currentMaterial->GetEffectParamSet().GetResource( kSnowRockTextureParamName );

      shader->SetTexture( mMaterialGrassDiffuseTexture, grassTexID );
      shader->SetTexture( mMaterialSandDiffuseTexture, sandTexID );
      shader->SetTexture( mMaterialSnowDiffuseTexture, snowTexID );
      shader->SetTexture( mMaterialGrassRockDiffuseTexture, grassRockTexID );
      shader->SetTexture( mMaterialSnowRockDiffuseTexture, snowRockTexID );

      // Set the parameter
      shader->SetFloat( mSnowHeight, mSnowHeightValue );
      shader->SetFloat( mSandHeight, mSandHeightValue );
      shader->SetFloat( mRockCosAngle, mRockCosAngleValue );
      shader->SetFloat( mHeightTransition, mHeightTransitionValue );
      shader->SetFloat( mCosAngleTransition, mCosAngleTransitionValue );

      // Apply shadow parameters
      const Matrix4 lightTransform = VCNXformCore::GetInstance()->GetWorld() * mLightViewProjectionMatrix;
      shader->SetMatrix( mLightModelViewProjection, lightTransform );

      // Prepare streams
      PrepareRender( mesh, static_cast<VCND3D9Shader*>(shader.get()) );

      // Make an ambient pass on the mesh
      if( VCNLightingCore::GetInstance()->HasAmbientLight() )
      {
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
        // Have the shader read its params
        mLightingFunctions.InitLightPass( static_cast<VCND3D9Shader*>(shader.get())->GetEffect(), *mLights[i] );

        // Call for the right type of render
        TriggerRender( mesh, static_cast<VCND3D9Shader*>(shader.get()) );
      }

      VCNRenderCore::GetInstance()->SetBlendMode( RS_BLEND_NONE );
      VCNRenderCore::GetInstance()->SetDepthBufferMode( RS_DEPTH_READWRITE );
    }
  }
}

///////////////////////////////////////////////////////////////////////
void VCND3D9_TerrainFX::SetShadowMapTexture(LPDIRECT3DTEXTURE9 texture)
{
  // TODO: Make the effect pull on the shadow map texture somehow
  if ( VCNShaderPointer shader = mShaderHandle.lock() )
  {
    static_cast<VCND3D9Shader*>(shader.get())->GetEffect()->SetTexture( (D3DXHANDLE)mShadowMapTexture, texture );
  }
}



//////////////////////////////////////////////////////////////////////////
void VCND3D9_TerrainFX::CreateToolbar()
{
  std::stringstream ss;

  const VCNUInt32 barWidth = 300;
  const VCNUInt32 barHeight = 225;

  mBar = TwNewBar("TerrainShadingBar");

  const VCNPoint& screenDimension = VCNRenderCore::GetInstance()->GetResolution();

  ss <<
    "TerrainShadingBar "
    "label='[Terrain Shading]' "
    "color='50 40 180 50' "
    "position='860 " << screenDimension.y - barHeight - 31 << "' "
    "size='" << barWidth << " " << barHeight << "' "
    "valuesWidth=130 " << 
    "iconified=true " 
    ;

  TwDefine(ss.str().c_str());

  TwAddVarRW(mBar, 0, TW_TYPE_FLOAT, &mSnowHeightValue, "label='Snow Height' min=0");
  TwAddVarRW(mBar, 0, TW_TYPE_FLOAT, &mSandHeightValue, "label='Sand Height' min=0");
  TwAddVarRW(mBar, 0, TW_TYPE_FLOAT, &mRockCosAngleValue, "label='Rock Angle' step=0.05");
  TwAddVarRW(mBar, 0, TW_TYPE_FLOAT, &mHeightTransitionValue, "label='Height Transition' min=0");
  TwAddVarRW(mBar, 0, TW_TYPE_FLOAT, &mCosAngleTransitionValue, "label='Transition Angle' step=0.05");
}
