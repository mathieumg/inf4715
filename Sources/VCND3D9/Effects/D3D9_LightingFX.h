///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief D3D9 shading lighting functions
///

#ifndef VICUNA_DIRECTX_LIGHTING_FUNCTIONS
#define VICUNA_DIRECTX_LIGHTING_FUNCTIONS

#pragma once

#include "VCNUtils/RenderTypes.h"

struct VCNColor;
class VCNLight;
class VCNMaterial;

//-------------------------------------------------------------
/// Common lighting functions for shaders that wish to
/// implement lighting functionality
//-------------------------------------------------------------

class VCND3D9_LightingFX
{
public:
  VCND3D9_LightingFX();
  ~VCND3D9_LightingFX();

  // Create handles to all the lighting functions
  void CreateParameterHandles( LPD3DXEFFECT pShader );

  // Set light
  void InitAmbientPass( LPD3DXEFFECT pShader, const VCNColor& color );
  void InitLightPass( LPD3DXEFFECT pShader, const VCNLight& light );
  void SetLightType( LPD3DXEFFECT pShader, const VCNLightType light );

protected:

  // The ambient color of the scene
  D3DXHANDLE mLightAmbientColor;

  // Parameters held in all lights
  D3DXHANDLE mLightTypeAmbient;         // 0=omni, 1=spot, 2=directional, 3=ambient
  D3DXHANDLE mLightTypePoint;            // 0=omni, 1=spot, 2=directional, 3=ambient
  D3DXHANDLE mLightTypeSpot;            // 0=omni, 1=spot, 2=directional, 3=ambient
  D3DXHANDLE mLightTypeDir;             // 0=omni, 1=spot, 2=directional, 3=ambient
  D3DXHANDLE mLightDiffuseColor;        // diffuse color of the light
  D3DXHANDLE mLightSpecularColor;       // specular color of the light

  // Parameters specific to certain types of lights
  D3DXHANDLE mLightRange;               // range that is perfectly lit (omni and spot)
  D3DXHANDLE mLightMaxRange;            // range after which the light has no effect (omni and spot)
  D3DXHANDLE mLightPosition;            // position of the light (omni and spot)
  D3DXHANDLE mLightDirection;           // direction of the light (dir and spot)
  D3DXHANDLE mLightInnerAngle;          // angle up to where spot lights perfectly (spot)
  D3DXHANDLE mLightOuterAngle;          // angle at which the spot stops lighting (spot)
  D3DXHANDLE mLightFalloff;
  D3DXHANDLE mLightConstantAttenuation;
  D3DXHANDLE mLightLinearAttenuation;
  D3DXHANDLE mLightQuadAttenuation;

private:

  void CreateToolbar();

  // Shadowing parameters
  // TODO: Refactor
  static float mArg1, mArg2, mArg3;

};

//-------------------------------------------------------------

#endif
