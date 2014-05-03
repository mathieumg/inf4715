///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief The Vicuna DirectX Basic Lit Effect.
///

#ifndef VICUNA_DIRECTX_LIT_EFFECT
#define VICUNA_DIRECTX_LIT_EFFECT

#pragma once

#include "VCND3D9/D3D9Effect.h"
#include "VCND3D9/Effects/D3D9_LightingFX.h"

class VCNDXFX_Lit : public VCND3D9Effect
{
public:

  /// Default constructor
  VCNDXFX_Lit();

  /// Destructor
  virtual ~VCNDXFX_Lit();

  /// Renders a mesh using the current effect.
  virtual void RenderMesh( const VCNMesh* mesh, const VCNSphere& boundingSphere, const VCNEffectParamSet& effectParams );
  
private:

  /// Renders the mesh with active light contribution.
  void PerformLightingPasses( const VCNMesh* mesh, const VCNSphere& boundingSphere, const VCNEffectParamSet& effectParams );

// Shader
  VCNShaderHandle         mShader;
  
// Effect global parameters
  VCND3D9_LightingFX      mLightingFunctions;
  VCNShaderParameter      mMaterialAmbientColor;
  VCNShaderParameter      mMaterialDiffuseColor;
  VCNShaderParameter      mMaterialSpecularColor;
  VCNShaderParameter      mMaterialSpecularPower;

};

#endif
