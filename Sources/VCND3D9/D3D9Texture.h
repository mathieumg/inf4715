///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// The Vicuna DirectX Texture resource base class 
///

#ifndef VICUNA_DIRECTX_TEXTURE
#define VICUNA_DIRECTX_TEXTURE

#pragma once

#include "D3D9ObjectWrapper.h"

#include "VCNResources/Texture.h"
#include "VCNUtils/RenderTypes.h"

//-------------------------------------------------------------
/// The texture class
//-------------------------------------------------------------
class VCND3D9Texture : public VCND3D9ObjectWrapper<VCNTexture>
{
  VCN_CLASS;

public:

  /// Converts flags to D3D9 usages
  static const DWORD ToD3DUsage(VCNTextureFlags flags);

  /// Converts to D3DPOOL enum
  static const D3DPOOL ToD3DPool(VCNTextureFlags flags);

  /// Converts to D3DFORMAT enum
  static const D3DFORMAT ToD3DFormat(VCNTextureFlags flags);

  /// Converts to D3DLOCK flag
  static const DWORD ToD3DLock(VCNTextureFlags flags);
  
  /// Constructs the D3D9 texture
  VCND3D9Texture(LPDIRECT3DTEXTURE9 ptr);

  /// Destructs the texture
  virtual ~VCND3D9Texture();

  /// Returns the D3D9 texture pointer
  LPDIRECT3DTEXTURE9 GetPointer() const;

  /// Locks the texture to read or write data
  virtual const bool Lock(VCNByte** data, VCNTextureFlags flags) override;

  /// Unlock the texture.
  virtual void Unlock() override;

protected:

  LPDIRECT3DTEXTURE9  mTexturePtr;
};

//-------------------------------------------------------------
inline LPDIRECT3DTEXTURE9 VCND3D9Texture::GetPointer() const
{
  return mTexturePtr;
}

#endif
