///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Vicuna's class to create/manage materials using DirectX as an API.
///

#ifndef VICUNA_DIRECTX_MATERIAL_CORE
#define VICUNA_DIRECTX_MATERIAL_CORE

#pragma once

#include "D3D9ObjectWrapper.h"

#include "VCNRenderer/MaterialCore.h"

//-------------------------------------------------------------
class VCNDXMaterialCore : public VCND3D9ObjectWrapper<VCNMaterialCore> 
{
  VCN_CLASS;

public:

  VCNDXMaterialCore();
  virtual ~VCNDXMaterialCore( void );

  // Needed to be a Core
  virtual VCNBool Initialize() override;
  virtual VCNBool Uninitialize() override;
  virtual VCNBool Process(const float elapsedTime) override;

  // Create a DirectX texture and add it to the resource core
  virtual VCNResID CreateTexture(const VCNString& filename) override;
  virtual VCNResID CreateTexture(VCNUInt width, VCNUInt height, VCNUInt32 mipLevel, VCNTextureFlags flags) override;
  virtual VCNResID CreateCubeTexture(const VCNString& filename) override;

protected:

};

#endif // VICUNA_DIRECTX_MATERIAL_CORE

