///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#ifndef VCND3D9CACHE_H
#define VCND3D9CACHE_H

#pragma once


#include "VCNResources/Cache.h"
#include "VCNUtils/Assert.h"

///
/// D3D9 cache implementation
///
class VCND3D9Cache : public VCNCache
{
  VCN_CLASS;

public:
  
  /// Converts VCN cache flags to D3D9 usage flags
  static const DWORD GetD3DUsage(const VCNCacheFlags flags);

  /// Converts the VCN cache flags to the desire D3D_POOL setting
  static const D3DPOOL GetD3DPool(const VCNCacheFlags flags);

  /// Converts the VCN cache flags to the desired D3DLOCK setting
  static const DWORD GetD3DLock(const VCNCacheFlags flags);

  /// Construct the D3D9 cache buffer
  VCND3D9Cache(VCNCacheType cacheType, const void* buffer, VCNUInt bufferSize, VCNCacheFlags flags);

  /// Construct the D3D9 cache buffer using a custom vertex declaration
  VCND3D9Cache(const void* buffer, VCNUInt bufferSize, VCNUInt stride, VCNCacheFlags flags);

  /// Destructs the buffer
  virtual ~VCND3D9Cache();

  /// Returns the D3D9 index buffer
  LPDIRECT3DINDEXBUFFER9 GetIndexBuffer() const;

  /// Returns the D3D9 vertex buffer
  LPDIRECT3DVERTEXBUFFER9 GetVertexBuffer() const;

  /// Locks the buffer to read bytes 
  virtual const bool Lock(VCNByte** buffer, const VCNCacheFlags flags) const override;

  /// Locks the buffer start at @startOffset for @byteCount bytes
  virtual const bool Lock(int startOffset, int byteCount, VCNByte** buffer, const VCNCacheFlags flags) const override;

  /// Unlock the buffer
  virtual void Unlock() const override;

  /// Returns a copy of the stored data
  virtual const VCNBuffer GetBuffer() const override;

protected:

  virtual void LoadIndexBuffer(const void* buffer, VCNUInt bufferSize, VCNCacheFlags flags) override;
  virtual void LoadVertexBuffer(VCNCacheType vertexType, const void* buffer, VCNUInt bufferSize, VCNCacheFlags flags) override;

private:
};


//-------------------------------------------------------------
inline LPDIRECT3DINDEXBUFFER9 VCND3D9Cache::GetIndexBuffer() const
{
  VCN_ASSERT( IsIndexBuffer() && "ERROR!" );
  return (LPDIRECT3DINDEXBUFFER9)mBufferHandle;
}

//-------------------------------------------------------------
inline LPDIRECT3DVERTEXBUFFER9 VCND3D9Cache::GetVertexBuffer() const
{ 
  VCN_ASSERT( IsVertexBuffer() && "ERROR!" );
  return (LPDIRECT3DVERTEXBUFFER9)mBufferHandle; 
}

#endif // VCND3D9CACHE_H
