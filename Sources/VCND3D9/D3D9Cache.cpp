///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Direct3D 9 cache implementation
///

#include "Precompiled.h"
#include "D3D9Cache.h"

#include "D3D9.h"

VCN_TYPE( VCND3D9Cache, VCNCache );

//-------------------------------------------------------------
// Constructor
//-------------------------------------------------------------
VCND3D9Cache::VCND3D9Cache(VCNCacheType cacheType, const void* buffer, VCNUInt bufferSize, VCNCacheFlags flags)
  : VCNCache()
{
  mCacheFlags = flags;

  if( cacheType == VT_INDEX )
  {
    LoadIndexBuffer( buffer, bufferSize, flags );
  }
  else
  {
    LoadVertexBuffer( cacheType, buffer, bufferSize, flags );
  }
}



///////////////////////////////////////////////////////////////////////
VCND3D9Cache::VCND3D9Cache(const void* buffer, VCNUInt bufferSize, VCNUInt stride, VCNCacheFlags flags)
  : VCNCache()
{
  mCacheType = VT_CUSTOM;
  mCacheFlags = flags;
  mStride = stride;

  LoadVertexBuffer( VT_CUSTOM, buffer, bufferSize, flags );
}



//-------------------------------------------------------------
// Destructor
//-------------------------------------------------------------
VCND3D9Cache::~VCND3D9Cache()
{
  // Release the buffer
  if( mBufferHandle )
  {
    if( IsIndexBuffer() )
    {
      LPDIRECT3DINDEXBUFFER9 pIB = (LPDIRECT3DINDEXBUFFER9)mBufferHandle;
      pIB->Release();
    }
    else
    {
      LPDIRECT3DVERTEXBUFFER9 pVB = (LPDIRECT3DVERTEXBUFFER9)mBufferHandle;
      pVB->Release();
    }
    mBufferHandle = NULL;
  }

  // Reset the counts
  mBufferSize = 0;
  mCount = 0;
}

//-------------------------------------------------------------
// Makes an Index Cache out of this cache
//-------------------------------------------------------------
void VCND3D9Cache::LoadIndexBuffer(const void* buffer, VCNUInt bufferSize, VCNCacheFlags flags)
{
  VCN_ASSERT( mBufferHandle == NULL && "MEMORY LEAK!" );

  // Start off clean
  mBufferHandle = NULL;
  mBufferSize = 0;
  mCount = 0;

  // First, we need our 3D device
  LPDIRECT3DDEVICE9 d3dDevice = static_cast<VCND3D9*>(VCNRenderCore::GetInstance())->GetD3DDevice();

  // Create the buffer
  LPDIRECT3DINDEXBUFFER9 d3dIndexBuffer;
  HRESULT hr = d3dDevice->CreateIndexBuffer( 
    bufferSize, GetD3DUsage(flags), D3DFMT_INDEX16, GetD3DPool(flags), &d3dIndexBuffer, NULL);

  // If it succeeded, then take down the info
  if( SUCCEEDED(hr) )
  {
    if ( buffer )
    {
      // Lock the buffer
      VCNByte* tmpBuffPtr = NULL;
      if( SUCCEEDED(d3dIndexBuffer->Lock( 0, bufferSize, (void**)&tmpBuffPtr, 0)) )
      {
        // Copy the data into our buffer
        memcpy( tmpBuffPtr, buffer, bufferSize );

        // Let go of the buffer
        d3dIndexBuffer->Unlock();
        mFilled = true;
      }
    }

    // Keep the buffer
    mBufferHandle = d3dIndexBuffer;
    mBufferSize   = bufferSize;
    mCount        = bufferSize / kCacheStrides[VT_INDEX];
    mCacheType    = VT_INDEX;
  }
}

//-------------------------------------------------------------
// Makes an Vertex Cache out of this cache
//-------------------------------------------------------------
void VCND3D9Cache::LoadVertexBuffer(VCNCacheType vertexType, const void* buffer, VCNUInt bufferSize, VCNCacheFlags flags)
{
  VCN_ASSERT( mBufferHandle == NULL && "MEMORY LEAK!" );

  // Start off clean
  mBufferHandle = NULL;
  mBufferSize = 0;

  // First, we need our 3D device
  LPDIRECT3DDEVICE9 d3dDevice = static_cast<VCND3D9*>(VCNRenderCore::GetInstance())->GetD3DDevice();
  
  // Create the buffer
  LPDIRECT3DVERTEXBUFFER9 d3dVertexBuffer;
  HRESULT hr = d3dDevice->CreateVertexBuffer( bufferSize, GetD3DUsage(flags), 0, GetD3DPool(flags), &d3dVertexBuffer, NULL );

  // If it succeeded, then take down the info if any is available
  if( SUCCEEDED(hr) )
  {
    // Push some data if any
    if ( buffer )
    {
      // Lock the buffer
      VCNByte* tmpBuffPtr = NULL;
      if( !FAILED(d3dVertexBuffer->Lock( 0, bufferSize, (void**)&tmpBuffPtr, 0)) )
      {
        // Copy the data into our buffer
        memcpy( tmpBuffPtr, buffer, bufferSize );

        // Let go of the buffer
        d3dVertexBuffer->Unlock();
        mFilled = true;
      }
    }

    // Keep the buffer
    mBufferHandle = d3dVertexBuffer;
    mBufferSize   = bufferSize;
    if ( vertexType != VT_CUSTOM )
    {
      mStride       = kCacheStrides[vertexType];
      mCount        = bufferSize / mStride;
      mCacheType    = vertexType;
    }
    else
    {
      VCN_ASSERT( vertexType == VT_CUSTOM );
      VCN_ASSERT( mStride );

      mCount        = bufferSize / mStride;
    }
  }
  else
  {
    VCN_ASSERT_FAIL( "Failed to create vertex buffer" );
  }
}

///////////////////////////////////////////////////////////////////////
const bool VCND3D9Cache::Lock(VCNByte** data, const VCNCacheFlags flags) const
{
  return Lock(0, mBufferSize, data, flags);
}



///////////////////////////////////////////////////////////////////////
const bool VCND3D9Cache::Lock(int startOffset, int byteCount, VCNByte** data, const VCNCacheFlags flags) const 
{
  if( IsIndexBuffer() )
  {
    LPDIRECT3DINDEXBUFFER9 buffer = (LPDIRECT3DINDEXBUFFER9)mBufferHandle;
    if ( SUCCEEDED(buffer->Lock(startOffset, byteCount, (void**)data, GetD3DLock(flags))) )
    {
      return true;
    }

    return false;
  }
  else if ( IsVertexBuffer() )
  {
    LPDIRECT3DVERTEXBUFFER9 buffer = (LPDIRECT3DVERTEXBUFFER9)mBufferHandle;
    if ( SUCCEEDED(buffer->Lock(startOffset, byteCount, (void**)data, GetD3DLock(flags))) )
    {
      return true;
    }

    return false;
  }

  return false;
}



///////////////////////////////////////////////////////////////////////
void VCND3D9Cache::Unlock() const
{
  if( IsIndexBuffer() )
  {
    LPDIRECT3DINDEXBUFFER9 buffer = (LPDIRECT3DINDEXBUFFER9)mBufferHandle;
    buffer->Unlock();
  }
  else if ( IsVertexBuffer() )
  {
    LPDIRECT3DVERTEXBUFFER9 buffer = (LPDIRECT3DVERTEXBUFFER9)mBufferHandle;
    buffer->Unlock();
  }

  mFilled = true;
}

///////////////////////////////////////////////////////////////////////
const VCNBuffer VCND3D9Cache::GetBuffer() const 
{
  VCNByte* rawData = 0;
  VCNBuffer buffer( mBufferSize );

  if ( Lock(&rawData, CF_LOCK_READ_ONLY) )
  {
    memcpy( &buffer[0], rawData, mBufferSize );
    Unlock();
  }

  return buffer;
}



///////////////////////////////////////////////////////////////////////
const DWORD VCND3D9Cache::GetD3DUsage(const VCNCacheFlags flags)
{
  DWORD d3dUsage = 0;

  if ( flags != CF_NONE )
  {
    if ( flags & CF_DYNAMIC   ) d3dUsage |= D3DUSAGE_DYNAMIC;
    if ( flags & CF_WRITEONLY ) d3dUsage |= D3DUSAGE_WRITEONLY;
  }

  return d3dUsage;
}



///////////////////////////////////////////////////////////////////////
const D3DPOOL VCND3D9Cache::GetD3DPool(const VCNCacheFlags flags)
{
  D3DPOOL d3dPool = D3DPOOL_MANAGED;

  if ( flags != CF_NONE )
  {
    if      ( flags & CF_POOL_MANAGED ) d3dPool = D3DPOOL_MANAGED;
    else if ( flags & CF_POOL_DEFAULT ) d3dPool = D3DPOOL_DEFAULT;
    else if ( flags & CF_POOL_SYSMEM)   d3dPool = D3DPOOL_SYSTEMMEM;
    else
    {
      if ( flags & CF_DYNAMIC ) d3dPool = D3DPOOL_DEFAULT;
    }
  }

  return d3dPool;
}



///////////////////////////////////////////////////////////////////////
const DWORD VCND3D9Cache::GetD3DLock(const VCNCacheFlags flags)
{
  DWORD d3dFlags = 0;
  
  if ( flags & CF_LOCK_READ_ONLY )
    d3dFlags |= D3DLOCK_READONLY;

  if ( flags & CF_LOCK_DISCARD )
    d3dFlags |= D3DLOCK_DISCARD;

  if ( flags & CF_LOCK_NOOVERWRITE )
    d3dFlags |= D3DLOCK_NOOVERWRITE;

  return d3dFlags;
}
