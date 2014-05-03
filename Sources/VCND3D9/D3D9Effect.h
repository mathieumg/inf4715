///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief The Vicuna DirectX Effect base class.
///

#ifndef VICUNA_DIRECTX_EFFECT
#define VICUNA_DIRECTX_EFFECT

#pragma once

#include "D3D9.h"
#include "D3D9ShaderCore.h"

#include "VCNRenderer/Effect.h"
#include "VCNUtils/RenderTypes.h"

class VCND3D9Effect : public VCNEffect
{
public:

  /// Default constructor
  VCND3D9Effect();

  /// Destructor
  virtual ~VCND3D9Effect();

protected:

  /// Returns the shader core
  VCNDXShaderCore* GetDXShaderCore() const;

  /// Helper function to get the device
  LPDIRECT3DDEVICE9 GetD3DDevice() const;

  /// Prepare the effect before starting rendering.
  virtual void Prepare() override { /* do nothing by default */ };
  
  /// Prepare the rendering for the mesh (steams, texture, etc.)
  virtual VCNBool PrepareRender(const class VCNMesh* mesh, class VCND3D9Shader* shader);

  /// Triggers the render of the mesh for indexed or non-indexed geometry.
  virtual VCNBool TriggerRender(const class VCNMesh* mesh, class VCND3D9Shader* shader);

  /// Triggers the render of the mesh for indexed or non-indexed geometry.
  virtual VCNBool TriggerRender(const class VCNMesh* mesh, VCNShaderPointer shader);

  /// Loads a vertex cache to GPU if it is valid.
  virtual VCNBool LoadVertexCache( const class VCNMesh* mesh, const VCNCacheType vertexType, VCNUInt streamIndex );
};

//
// INLINES
//

///////////////////////////////////////////////////////////////////////
inline VCNDXShaderCore* VCND3D9Effect::GetDXShaderCore() const
{
  return static_cast<VCNDXShaderCore*>( VCNDXShaderCore::GetInstance() );
}


///////////////////////////////////////////////////////////////////////
inline LPDIRECT3DDEVICE9 VCND3D9Effect::GetD3DDevice() const
{
  return static_cast<VCND3D9*>(VCND3D9::GetInstance())->GetD3DDevice();
}

#endif
