///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// The Vicuna Texture resource base class (shortcut!).
///

#ifndef VICUNA_TEXTURE
#define VICUNA_TEXTURE

#pragma once

#include "Resource.h"

#include "VCNUtils/RenderTypes.h"

//-------------------------------------------------------------
// The texture class
//-------------------------------------------------------------
class VCNTexture : public VCNResource
{
  VCN_CLASS;

public:

  /// Destructs the texture
  virtual ~VCNTexture();

  /// Locks the texture to read or write data
  virtual const bool Lock(VCNByte** data, VCNTextureFlags flags) = 0;

  /// Unlock the texture.
  virtual void Unlock() = 0;

protected:

  /// Constructs the texture
  VCNTexture();

};


#endif
