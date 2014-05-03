///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// Vicuna's Shader Core
/// Core module for shader management.  DirectX impl.
///

#ifndef VCND3D9SHADERCORE_H
#define VCND3D9SHADERCORE_H

#pragma once

#include "D3D9.h"

#include "VCNRenderer/ShaderCore.h"

///
/// Handles D3D9 shaders and effect files
///
class VCNDXShaderCore : public VCNShaderCore
{
public:
  VCNDXShaderCore();
  virtual ~VCNDXShaderCore();

  // Core module overloads
  virtual VCNBool Initialize() override;
  virtual VCNBool Uninitialize() override;
  virtual VCNBool Process(const float elapsedTime) override;

  // Load up all the shaders
  virtual VCNBool LoadShaders() override;
  virtual VCNBool UnloadShaders() override;

  /// Loads a shader from disk.
  virtual VCNShaderHandle LoadShader(const VCNTChar* filename, VCNULong flags /* = 0 */) override;

  // Sets shading world matrix
  virtual void SetWorldTransform(const Matrix4& world) override;
  virtual void SetWorldViewProjMatrix(const Matrix4& mat) override;
  virtual void SetViewMatrix(const Matrix4& mat) override;
  virtual void SetViewPosition(const Vector3& postion) override;
  virtual void SetModelViewMatrix(const Matrix4& mat) override;
  virtual void SetNormalMatrix(const Matrix4& mat) override;
  virtual void SetFadeAmount(const VCNFloat fadeAmount) override;
  
  // Select the current shader
  virtual VCNBool SelectShader( VCNShaderHandle shaderHandle ) override;

protected:

  // Tests for shader capabilities
  const VCNBool CanSupportShaders() const;

  /// Functions to manage pooled shader constants
  VCNBool InitConstantPool();  
  LPD3DXEFFECTPOOL GetPool() const;

  /// Loads a .fx file and returns the loaded D3D9 effect
  LPD3DXEFFECT LoadEffectFile(const VCNTChar* filename, VCNULong flags = 0);

  /// Functions to set pool constants
  void SetIntPoolConstant( VCNShaderPoolConstants index, const VCNInt* array, const VCNUInt size );
  void SetFloatPoolConstant( VCNShaderPoolConstants index, const VCNFloat* array, const VCNUInt size );
  void SetFloatPoolConstant( VCNShaderPoolConstants index, const VCNFloat f);

  /// Helper function to get a hold of the D3D device
  LPDIRECT3DDEVICE9 GetD3DDevice() const;
protected:

  typedef std::map<VCNString, VCNShaderPointer> VCNShaderMap;

  VCNShaderMap     mShaderMap;
  VCNShaderHandle  mActiveShader;
  
  LPD3DXEFFECTPOOL mParamPool;
  LPD3DXEFFECT     mDummyShader;
  D3DXHANDLE       mShaderPoolConstants[kNumShaderPoolConstants];

  
};


///////////////////////////////////////////////////////////////////////
inline LPD3DXEFFECTPOOL VCNDXShaderCore::GetPool() const
{
  return mParamPool;
}


///////////////////////////////////////////////////////////////////////
///
/// Helper function to get a hold of the D3D device
///
inline LPDIRECT3DDEVICE9 VCNDXShaderCore::GetD3DDevice() const
{
  return static_cast<VCND3D9*>( VCND3D9::GetInstance() )->GetD3DDevice();
}

#endif // VCND3D9SHADERCORE_H
