///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Core module for shader management.
///

#ifndef VCNSHADERCORE_H
#define VCNSHADERCORE_H

#pragma once

#include "VCNCore/Core.h"
#include "VCNRenderer/XformCore.h"
#include "VCNUtils/Matrix.h"
#include "VCNUtils/Observer.h"
#include "VCNUtils/RenderTypes.h"
#include "VCNUtils/SmartTypes.h"

class VCNShaderCore 
  : public VCNCore<VCNShaderCore>
  , public patterns::Observer<VCNMsgTransformChanged>
{
  VCN_CLASS;

public:
  VCNShaderCore(void);
  virtual ~VCNShaderCore(void);

  // Core module overloads
  virtual VCNBool Initialize() override;
  virtual VCNBool Uninitialize() override;

  // Load up all the shaders
  virtual VCNBool LoadShaders() = 0;
  virtual VCNBool UnloadShaders() = 0;
  virtual VCNShaderHandle LoadShader(const VCNTChar* filename, VCNULong flags = 0) = 0;

  // Set transformation matrices
  virtual void SetWorldTransform(const Matrix4& world) {}
  virtual void SetWorldViewProjMatrix(const Matrix4& mat) {}
  virtual void SetViewMatrix(const Matrix4& mat) {}
  virtual void SetViewPosition(const Vector3& postion) {}
  virtual void SetModelViewMatrix(const Matrix4& mat) {}
  virtual void SetNormalMatrix(const Matrix4& mat) {}
  virtual void SetFadeAmount(const VCNFloat fadeAmount) {}

  // Select the current shader
  virtual VCNBool SelectShader( VCNShaderHandle shaderHandle ) = 0;

  // Called when a transformation change occurs.
  virtual void UpdateObserver(VCNMsgTransformChanged& hint);

protected:
   
};

#endif // VCNSHADERCORE_H
