///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Defines an wrapper for quicker access to the D3D renderer.
///

#pragma once

#include "D3D9.h"

template<class T>
class VCND3D9ObjectWrapper : public T
{
protected:

  /// Returns the D3D9 renderer
  VCND3D9* GetRenderer()
  {
    return static_cast<VCND3D9*>(VCNRenderCore::GetInstance());
  }

  /// Returns a const D3D9 renderer
  const VCND3D9* GetRenderer() const
  {
    return static_cast<const VCND3D9*>(VCNRenderCore::GetInstance());
  }

  /// Returns the D3D9 device
  LPDIRECT3DDEVICE9 GetDevice() const
  { 
    return GetRenderer()->GetD3DDevice();
  }

private:

};
