///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Particle main effect
///

#ifndef VCNPARTICLEEFFECT_H
#define VCNPARTICLEEFFECT_H

#pragma once

#include "VCNRenderer/Effect.h"
#include "VCNUtils/SmartTypes.h"
#include "VCNUtils/RenderTypes.h"

class VCNCache;

struct VCNParticleVertex
{
  Vector3   pos;
  VCNUInt32 color;
  Vector3   tex; 
};

class VCNParticleEffect : public VCNEffect
{
  VCN_CLASS;

public:

  /// Constructs the effect
  VCNParticleEffect();

  /// Destructs the effect
  virtual ~VCNParticleEffect();

  /// Start rendering particles
  void Begin(VCNCache* indices, VCNCache* vertexes);

  /// End rendering particles
  void End();

  /// Sets the particle texture to render
  void SetTexture(VCNResID texID);

  /// Renders particles
  void RenderIndexedPrimitive(
    VCNPrimitiveType primitiveType, VCNInt baseVertexIndex, 
    VCNUInt minVertexIndex, VCNUInt numVertices, VCNUInt startIndex, VCNUInt primCount);

// VCNEffect interface

  /// Prepare the effect for this frame
  virtual void Prepare();

  /// DO NOT USE: the particle effect only renders private geometry
  virtual void RenderMesh(const VCNMesh* mesh, const VCNSphere& boundingSphere, const VCNEffectParamSet& effectParams);

private:

// Shader
  VCNShaderHandle     mShader;

// Effect global parameters
  VCNShaderParameter  mProjEffectParam;
  VCNShaderParameter  mViewEffectParam;
  VCNShaderParameter  mTextureEffectParam;

};

#endif // VCNPARTICLEEFFECT_H
