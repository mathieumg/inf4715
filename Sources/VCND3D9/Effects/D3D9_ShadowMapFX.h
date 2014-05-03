///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief  Defines the shadow map effect. The shadow map effect
///         create a shadow map for a given light.
///

#ifndef VCNDXFX_SHADOWMAP_H
#define VCNDXFX_SHADOWMAP_H

#pragma once

#include "VCND3D9/D3D9Effect.h"

class VCNDXFX_ShadowMap : public VCND3D9Effect
{
public:
  VCNDXFX_ShadowMap();
  virtual ~VCNDXFX_ShadowMap();

  virtual void Prepare();

  virtual void RenderMesh( const VCNMesh* mesh, const VCNSphere& boundingSphere, const VCNEffectParamSet& effectParams );

private:

  // Shader
  VCNShaderHandle     mShader;

  // Effect global parameters
  VCNShaderParameter  mMaxDepthParam;
  VCNShaderParameter  mMaterialDiffuseTexture;

};

#endif // VCNDXFX_SHADOWMAP_H
