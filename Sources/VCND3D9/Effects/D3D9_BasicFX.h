///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief The Vicuna DirectX Basic Effect.
///

#ifndef VCNDXFX_BASIC_H
#define VCNDXFX_BASIC_H

#include "VCND3D9/D3D9Effect.h"
#include "VCNUtils/SmartTypes.h"

class VCNDXFX_Basic : public VCND3D9Effect
{
public:

  /// Default destructor
  VCNDXFX_Basic();

  /// Destructor
  virtual ~VCNDXFX_Basic();

  /// Renders a mesh
  virtual void RenderMesh( const VCNMesh* mesh, const VCNSphere& boundingSphere, const VCNEffectParamSet& effectParams ) override;

protected:

// Shader
  VCNShaderHandle     mShader;

// Effect global parameters
  VCNShaderParameter  mColorParam;

};

#endif // VCNDXFX_BASIC_H
