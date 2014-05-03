///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Direct3D effect module interface
///

#ifndef VCND3D9EFFECTCORE_H
#define VCND3D9EFFECTCORE_H

#pragma once

#include "VCNRenderer/EffectCore.h"

class VCND3D9EffectCore : public VCNEffectCore
{
  VCN_CLASS;

public:

  /// Constructs the core
  VCND3D9EffectCore();

  /// Destructs the core
  virtual ~VCND3D9EffectCore();

  /// Initialize the module
  virtual VCNBool Initialize() override;

  /// Uninitialize the module
  virtual VCNBool Uninitialize() override;

  // Creates all built-in effects
  virtual VCNBool CreateEffects() override;

  /// Renders maps as debug info
  virtual const VCNBool DebugRender() const override;

  // Render a shadow map of the scene
  virtual void GenerateShadowMap() override;

private:

  /// Create an AntTweakBar to tweak global effects
  void CreateToolbar();

  /// Create the screen quad buffer
  void CreateScreenVertexBuffer();

// Data members

  // Vertex buffer for drawing quads
  LPDIRECT3DVERTEXBUFFER9 mScreenVertexBuffer;      

  // Shadow map stuff
  LPDIRECT3DTEXTURE9      mShadowMapTexture;

  LPDIRECT3DSTATEBLOCK9   mAllStateBlock;
};

#endif // VCND3D9EFFECTCORE_H
