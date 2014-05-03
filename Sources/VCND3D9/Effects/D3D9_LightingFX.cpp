///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Direct3D shader lighting utility functions
///

#include "Precompiled.h"
#include "D3D9_LightingFX.h"

#include "VCND3D9/D3D9ShaderConstants.h"
#include "VCND3D9/D3D9ShaderCore.h"

#include "VCNNodes/Light.h"
#include "VCNRenderer/DirectionalLight.h"
#include "VCNRenderer/PointLight.h"
#include "VCNRenderer/RenderCore.h"
#include "VCNRenderer/SpotLight.h"

#include <AntTweakBar.h>
#include <sstream>

static TwBar* sBar = 0;
float VCND3D9_LightingFX::mArg1(0.0005350f);
float VCND3D9_LightingFX::mArg2(0.0000085f);
float VCND3D9_LightingFX::mArg3(0.0021770f);

//-------------------------------------------------------------
/// Constructor
//-------------------------------------------------------------
VCND3D9_LightingFX::VCND3D9_LightingFX()
{
}

//-------------------------------------------------------------
/// Destructor
//-------------------------------------------------------------
VCND3D9_LightingFX::~VCND3D9_LightingFX()
{
}

//-------------------------------------------------------------
/// Get the handles to all the params
//-------------------------------------------------------------
void VCND3D9_LightingFX::CreateParameterHandles( LPD3DXEFFECT pShader )
{
  mLightAmbientColor = pShader->GetParameterByName( NULL, "gLightAmbientColor" );
  mLightDiffuseColor = pShader->GetParameterByName( NULL, "gLightDiffuseColor" );
  mLightSpecularColor = pShader->GetParameterByName( NULL, "gLightSpecularColor" );

  mLightTypeAmbient = pShader->GetParameterByName( NULL, "gLightTypeAmbient" );
  mLightTypePoint = pShader->GetParameterByName( NULL, "gLightTypePoint" );
  mLightTypeSpot = pShader->GetParameterByName( NULL, "gLightTypeSpot" );
  mLightTypeDir = pShader->GetParameterByName( NULL, "gLightTypeDirectional" );
  mLightDirection = pShader->GetParameterByName( NULL, "gLightDirection" );
  mLightPosition = pShader->GetParameterByName( NULL, "gLightPosition" );
  mLightInnerAngle = pShader->GetParameterByName( NULL, "gLightCosPhi" );
  mLightOuterAngle = pShader->GetParameterByName( NULL, "gLightCosTheta" );
  mLightFalloff = pShader->GetParameterByName( NULL, "gLightFalloff" );
  mLightRange = pShader->GetParameterByName( NULL, "gLightRange" );
  mLightConstantAttenuation = pShader->GetParameterByName( NULL, "gLightConstantAttenuation" );
  mLightLinearAttenuation = pShader->GetParameterByName( NULL, "gLightLinearAttenuation" );
  mLightQuadAttenuation = pShader->GetParameterByName( NULL, "gLightQuadAttenuation" );
}

//-------------------------------------------------------------
/// Set up the dynamic branching in the shader
//-------------------------------------------------------------
void VCND3D9_LightingFX::SetLightType( LPD3DXEFFECT pShader, const VCNLightType light )
{
  HRESULT hr;
  switch(light)
  {
  case LT_DIRECTIONAL:
    hr = pShader->SetBool( mLightTypePoint, FALSE );
    VCN_ASSERT( SUCCEEDED(hr) );
    hr = pShader->SetBool( mLightTypeSpot, FALSE );
    VCN_ASSERT( SUCCEEDED(hr) );
    hr = pShader->SetBool( mLightTypeDir, TRUE );
    VCN_ASSERT( SUCCEEDED(hr) );
    hr = pShader->SetBool( mLightTypeAmbient, FALSE );
    VCN_ASSERT( SUCCEEDED(hr) );
    break;
  case LT_OMNI:
    hr = pShader->SetBool( mLightTypePoint, TRUE );
    VCN_ASSERT( SUCCEEDED(hr) );
    hr = pShader->SetBool( mLightTypeSpot, FALSE );
    VCN_ASSERT( SUCCEEDED(hr) );
    hr = pShader->SetBool( mLightTypeDir, FALSE );
    VCN_ASSERT( SUCCEEDED(hr) );
    hr = pShader->SetBool( mLightTypeAmbient, FALSE );
    VCN_ASSERT( SUCCEEDED(hr) );
    break;
  case LT_SPOT:
    hr = pShader->SetBool( mLightTypePoint, FALSE );
    VCN_ASSERT( SUCCEEDED(hr) );
    hr = pShader->SetBool( mLightTypeSpot, TRUE );
    VCN_ASSERT( SUCCEEDED(hr) );
    hr = pShader->SetBool( mLightTypeDir, FALSE );
    VCN_ASSERT( SUCCEEDED(hr) );
    hr = pShader->SetBool( mLightTypeAmbient, FALSE );
    VCN_ASSERT( SUCCEEDED(hr) );
    break;
  case LT_AMBIENT:
    hr = pShader->SetBool( mLightTypePoint, FALSE );
    VCN_ASSERT( SUCCEEDED(hr) );
    hr = pShader->SetBool( mLightTypeSpot, FALSE );
    VCN_ASSERT( SUCCEEDED(hr) );
    hr = pShader->SetBool( mLightTypeDir, FALSE );
    VCN_ASSERT( SUCCEEDED(hr) );
    hr = pShader->SetBool( mLightTypeAmbient, TRUE );
    VCN_ASSERT( SUCCEEDED(hr) );
    break;
  default:
    VCN_ASSERT( false );
  }
}

//-------------------------------------------------------------
/// Get ready for some ambient action
//-------------------------------------------------------------
void VCND3D9_LightingFX::InitAmbientPass( LPD3DXEFFECT pShader, const VCNColor& color )
{
  HRESULT hr;

  // Set the type correctly
  SetLightType( pShader, LT_AMBIENT );

  // Set the ambient light color
  hr = pShader->SetFloatArray( mLightAmbientColor, (VCNFloat*)&color, 4 );
  VCN_ASSERT( SUCCEEDED(hr) );

  // Replace existing colors
  VCNRenderCore::GetInstance()->SetBlendMode( RS_BLEND_NONE );
  VCNRenderCore::GetInstance()->SetDepthBufferMode( RS_DEPTH_READWRITE );
}

//-------------------------------------------------------------
/// Set all the params of the light into the shader at once
//-------------------------------------------------------------
void VCND3D9_LightingFX::InitLightPass( LPD3DXEFFECT pShader, const VCNLight& light )
{
  HRESULT hr;

  // Lazy load tool bar
  if ( sBar == 0 )
  {
    CreateToolbar();
  }

  // Set the type
  SetLightType( pShader, light.GetLightType() );

  // Set the diffuse color. 
  hr = pShader->SetFloatArray( mLightDiffuseColor, (VCNFloat*)&light.GetDiffuse(), 3 );
  VCN_ASSERT( SUCCEEDED(hr) );

  // Compute the light's direction if it's directional or the position for other types
  switch(light.GetLightType())
  {
  case LT_DIRECTIONAL:
    {
      hr = pShader->SetFloatArray( mLightDirection, (VCNFloat*)&((VCNDirectionalLight*)&light)->GetDirection(), 3 );
      VCN_ASSERT( SUCCEEDED(hr) );

      // Arguments used for shadow tweaking
      hr = pShader->SetFloat( mLightConstantAttenuation, mArg1 );
      hr = pShader->SetFloat( mLightLinearAttenuation, mArg2 );
      hr = pShader->SetFloat( mLightQuadAttenuation, mArg3 );
    }
    break;

  case LT_SPOT:
    {
      hr = pShader->SetFloatArray( mLightPosition, (VCNFloat*)&light.GetWorldTransformation().GetTranslation(), 3 );
      VCN_ASSERT( SUCCEEDED(hr) );

      hr = pShader->SetFloat( mLightInnerAngle, ((VCNSpotLight*)&light)->GetPhi() );
      VCN_ASSERT( SUCCEEDED(hr) );

      hr = pShader->SetFloat( mLightOuterAngle, ((VCNSpotLight*)&light)->GetTheta() );
      VCN_ASSERT( SUCCEEDED(hr) );

      hr = pShader->SetFloat( mLightFalloff, ((VCNSpotLight*)&light)->GetFalloff() );
      VCN_ASSERT( SUCCEEDED(hr) );

      hr = pShader->SetFloatArray( mLightDirection, (VCNFloat*)&((VCNSpotLight*)&light)->GetDirection(), 3 );
      VCN_ASSERT( SUCCEEDED(hr) );

      hr = pShader->SetFloat( mLightRange, ((VCNSpotLight*)&light)->GetMaxRange() );
      VCN_ASSERT( SUCCEEDED(hr) );

      hr = pShader->SetFloat( mLightConstantAttenuation, ((VCNSpotLight*)&light)->GetConstantAttenuation() );
      VCN_ASSERT( SUCCEEDED(hr) );

      hr = pShader->SetFloat( mLightLinearAttenuation, ((VCNSpotLight*)&light)->GetLinearAttenuation() );
      VCN_ASSERT( SUCCEEDED(hr) );

      hr = pShader->SetFloat( mLightQuadAttenuation, ((VCNSpotLight*)&light)->GetQuadAttenuation() );
      VCN_ASSERT( SUCCEEDED(hr) );
    }
    break;

  case LT_OMNI:
    {
      Vector3 lightPos = light.GetWorldTransformation().GetTranslation();
      hr = pShader->SetFloatArray( mLightPosition, (VCNFloat*)&lightPos, 3 );
      VCN_ASSERT( SUCCEEDED(hr) );

      hr = pShader->SetFloat( mLightRange, ((VCNPointLight*)&light)->GetMaxRange() );
      VCN_ASSERT( SUCCEEDED(hr) );

      hr = pShader->SetFloat( mLightConstantAttenuation, ((VCNPointLight*)&light)->GetConstantAttenuation() );
      VCN_ASSERT( SUCCEEDED(hr) );

      hr = pShader->SetFloat( mLightLinearAttenuation, ((VCNPointLight*)&light)->GetLinearAttenuation() );
      VCN_ASSERT( SUCCEEDED(hr) );

      hr = pShader->SetFloat( mLightQuadAttenuation, ((VCNPointLight*)&light)->GetQuadAttenuation() );
      VCN_ASSERT( SUCCEEDED(hr) );
    }
    break;

  default:
    VCN_ASSERT( false && "Light type not supported!" );
    break;
  }

  // Add colors together.
  // And the ambient pass already set the depth buffer.
  VCNRenderCore::GetInstance()->SetBlendMode( RS_BLEND_ADDITIVE );
  VCNRenderCore::GetInstance()->SetDepthBufferMode( RS_DEPTH_READONLY );
}

//////////////////////////////////////////////////////////////////////////
void VCND3D9_LightingFX::CreateToolbar()
{
  std::stringstream ss;

  const VCNUInt32 barWidth = 300;
  const VCNUInt32 barHeight = 225;

  sBar = TwNewBar("LightingFunctionsBar");

  const VCNPoint& screenDimension = VCNRenderCore::GetInstance()->GetResolution();

  ss <<
    "LightingFunctionsBar "
    "label='[Lighting Tool]' "
    "color='202 202 202 50' "
    "position='540 " << screenDimension.y - barHeight - 30 << "' "
    "size='" << barWidth << " " << barHeight << "' "
    "valuesWidth=140 "
    "refresh=1.0"
    << " iconified=true "
    ;

  TwDefine(ss.str().c_str());

  TwAddVarRW(sBar, "Arg1", TW_TYPE_FLOAT, &mArg1, "label='Arg1 - Const' min=0 step=0.000001");
  TwAddVarRW(sBar, "Arg2", TW_TYPE_FLOAT, &mArg2, "label='Arg2 - Linear' min=0 step=0.0000001");
  TwAddVarRW(sBar, "Arg3", TW_TYPE_FLOAT, &mArg3, "label='Arg3 - Quad' min=0 step=0.000001");
}
