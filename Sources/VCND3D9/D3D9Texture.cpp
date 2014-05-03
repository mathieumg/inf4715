///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#include "Precompiled.h"
#include "D3D9Texture.h"

VCN_TYPE( VCND3D9Texture, VCNTexture );


///////////////////////////////////////////////////////////////////////
const DWORD VCND3D9Texture::ToD3DUsage(VCNTextureFlags flags)
{
  DWORD usage = 0;

  if ( flags & TF_USAGE_DYNAMIC ) usage |= D3DUSAGE_DYNAMIC;

  return usage;
}



///////////////////////////////////////////////////////////////////////
const D3DPOOL VCND3D9Texture::ToD3DPool(VCNTextureFlags flags)
{
  D3DPOOL dxPool = D3DPOOL_MANAGED;

       if ( flags & TF_POOL_DEFAULT ) dxPool = D3DPOOL_DEFAULT;
  else if ( flags & TF_POOL_MANAGED ) dxPool = D3DPOOL_MANAGED;
  else if ( flags & TF_POOL_SYSMEM )  dxPool = D3DPOOL_SYSTEMMEM;

  return dxPool;
}



///////////////////////////////////////////////////////////////////////
const D3DFORMAT VCND3D9Texture::ToD3DFormat(VCNTextureFlags flags)
{
  D3DFORMAT dxFormat =  D3DFMT_A8R8G8B8;

       if ( flags & TF_FMT_A8B8G8R8 ) dxFormat = D3DFMT_A8B8G8R8;
  else if ( flags & TF_FMT_A8R8G8B8 ) dxFormat = D3DFMT_A8R8G8B8;

  return dxFormat;
}



///////////////////////////////////////////////////////////////////////
const DWORD VCND3D9Texture::ToD3DLock(VCNTextureFlags flags)
{
  DWORD lock = 0;

  if ( flags & TF_LOCK_READ_ONLY )    lock |= D3DLOCK_READONLY;
  if ( flags & TF_LOCK_DISCARD )      lock |= D3DLOCK_DISCARD;
  if ( flags & TF_LOCK_NOOVERWRITE )  lock |= D3DLOCK_NOOVERWRITE;

  return lock;
}



//-------------------------------------------------------------
/// Constructor
//-------------------------------------------------------------
VCND3D9Texture::VCND3D9Texture(LPDIRECT3DTEXTURE9 ptr)
  : mTexturePtr(ptr)
{
}

//-------------------------------------------------------------
/// Destructor
//-------------------------------------------------------------
VCND3D9Texture::~VCND3D9Texture()
{
  if( mTexturePtr )
  {
    mTexturePtr->Release();
  }
}



///////////////////////////////////////////////////////////////////////
const bool VCND3D9Texture::Lock(VCNByte** data, VCNTextureFlags flags)
{
  D3DLOCKED_RECT lockedRect;
  if ( SUCCEEDED(mTexturePtr->LockRect(0, &lockedRect, NULL, ToD3DLock(flags))) )
  {
    *data = (VCNByte*)lockedRect.pBits;
    return true;
  }

  return false;
}



///////////////////////////////////////////////////////////////////////
void VCND3D9Texture::Unlock()
{
  mTexturePtr->UnlockRect(0);
}
