///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#ifndef VCND3D9_TERRAINFX_H
#define VCND3D9_TERRAINFX_H

#pragma once

#include "AntTweakBar.h"

#include "D3D9_LightingFX.h"

#include "VCND3D9/D3D9Effect.h"
#include "VCNUtils/Matrix.h"

class VCND3D9_TerrainFX : public VCND3D9Effect
{

public:

  /// Default constructor
  VCND3D9_TerrainFX();

  /// Destructor
  virtual ~VCND3D9_TerrainFX();

  /// Sets the shadow map that will be used to lit objects
  void SetShadowMapTexture(LPDIRECT3DTEXTURE9 texture);

// Interface

  /// Prepares the effect for this frame
  virtual void Prepare() override;

  /// Renders the terrain mesh
  virtual void RenderMesh(const VCNMesh* mesh, const VCNSphere& boundingSphere, const VCNEffectParamSet& effectParams) override;

protected:

  /// Apply lighting effect to terrain
  void PerformLightingPasses(const VCNMesh* mesh, const VCNSphere& boundingSphere, const VCNEffectParamSet& effectParams);

private:

  /// Create a tweaking bar
  void CreateToolbar();

// Data members

  // Shader
  VCNShaderHandle         mShaderHandle;

  // Effect global parameters
  VCND3D9_LightingFX      mLightingFunctions;
  VCNShaderParameter      mMaterialAmbientColor;
  VCNShaderParameter      mMaterialDiffuseColor;
  VCNShaderParameter      mMaterialSpecularColor;
  VCNShaderParameter      mMaterialSpecularPower;
  VCNShaderParameter      mMaterialDiffuseTexture;

  // Texture params held in the material
  VCNShaderParameter      mMaterialGrassDiffuseTexture;
  VCNShaderParameter      mMaterialSandDiffuseTexture;
  VCNShaderParameter      mMaterialSnowDiffuseTexture;
  VCNShaderParameter      mMaterialGrassRockDiffuseTexture;
  VCNShaderParameter      mMaterialSnowRockDiffuseTexture;

  VCNShaderParameter      mSnowHeight;
  VCNShaderParameter      mSandHeight;
  VCNShaderParameter      mRockCosAngle;
  VCNShaderParameter      mHeightTransition;
  VCNShaderParameter      mCosAngleTransition;

  // Parameters for shadowing
  VCNShaderParameter      mMaxDepth;
  VCNShaderParameter      mLightModelViewProjection;
  VCNShaderParameter      mShadowMapTexture;

  // Lights lighting the current mesh
  std::vector<VCNLight*>  mLights;

  // Shadow mapping info
  Matrix4                 mLightViewProjectionMatrix;

  // Tweaking bar
  TwBar*                  mBar;
  VCNFloat                mSnowHeightValue;
  VCNFloat                mSandHeightValue;
  VCNFloat                mRockCosAngleValue;
  VCNFloat                mHeightTransitionValue;
  VCNFloat                mCosAngleTransitionValue;
};

#endif // VCND3D9_TERRAINFX_H
