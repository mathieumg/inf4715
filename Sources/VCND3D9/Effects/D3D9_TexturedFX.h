///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief The Vicuna DirectX Basic Textured Effect.
///

#ifndef VCNDXFX_TEXTURED_H
#define VCNDXFX_TEXTURED_H

#pragma once

#include "VCND3D9/D3D9Effect.h"
#include "D3D9_SelectionFX.h"

class VCNDXFX_Textured : public VCND3D9Effect
{
public:

  VCNDXFX_Textured();

  virtual ~VCNDXFX_Textured();

  virtual void RenderMesh( const VCNMesh* mesh, const VCNSphere& boundingSphere, const VCNEffectParamSet& effectParams ) override;

protected:

// Shader
  VCNShaderHandle         mShader;

// Effect global parameters
  VCND3D9_SelectionFX     mSelectionFunctions;
  VCNShaderParameter      mMaterialDiffuseColor;
  VCNShaderParameter      mMaterialDiffuseTexture;
};

#endif // VCNDXFX_TEXTURED_H
