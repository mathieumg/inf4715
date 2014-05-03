///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#ifndef VCN_CACHE
#define VCN_CACHE

#include "VCNResources/Resource.h"
#include "VCNUtils/RenderTypes.h"

//-------------------------------------------------------------
// Vicuna Cache resource base class
//-------------------------------------------------------------
class VCNCache : public VCNResource
{
public:
  
  /// Default constructor
  VCNCache();

  /// Destructor
  virtual ~VCNCache( void );

  /// Checks if the cache as been set
  const bool IsValid() const;

  /// Indicates if the cache has been filled.
  const bool IsFilled() const;

  /// Checks if the cache stores an index buffer
  const bool IsIndexBuffer() const;

  /// Checks if the cache stores an vertex buffer
  const bool IsVertexBuffer() const;
  
  /// Returns the number of bytes used by the buffer
  const VCNUInt GetBufferSize() const;

  /// Returns the number of elements in the buffer
  const VCNUInt GetCount() const;

  /// Returns the buffer stride in bytes
  const VCNUInt GetStride() const;
  
  /// Locks the buffer to read bytes 
  virtual const bool Lock(VCNByte** buffer, const VCNCacheFlags flags = CF_LOCK_DEFAULT) const = 0;

  /// Locks the buffer start at @startOffset for @byteCount bytes
  virtual const bool Lock(int startOffset, int byteCount, VCNByte** buffer, const VCNCacheFlags flags) const = 0;

  /// Unlock the buffer
  virtual void Unlock() const = 0;

  /// Returns a copy of the stored data
  virtual const VCNBuffer GetBuffer() const = 0;

  /// Returns a copy of the elements in the user buffer, buffer gets resize if needed.
  template<typename ElementType> const bool GetBuffer(std::vector<ElementType>& buffer) const;

protected:

  /// Loads the index buffer
  virtual void LoadIndexBuffer(const void* buffer, VCNUInt bufferSize, VCNCacheFlags flags) = 0;

  /// Loads the vertex buffer
  virtual void LoadVertexBuffer(VCNCacheType vertexType, const void* buffer, VCNUInt bufferSize, VCNCacheFlags flags) = 0;

protected:

  // Identifies what is stored in the cache
  VCNCacheType  mCacheType;

  // Store the flags with which the cache was created
  VCNCacheFlags mCacheFlags;

  // Handle to buffer
  void*         mBufferHandle;

  // Size of buffer in bytes
  VCNUInt       mBufferSize; 

  // Number of elements in buffer (vertex count, indices count, etc)
  VCNUInt       mCount;     

  // Stride of the element
  VCNUInt       mStride;

  // Indicates if the cache has been filled.
  mutable bool  mFilled;
};

//
// INLINES
//

//-------------------------------------------------------------
inline const bool VCNCache::IsValid() const
{
  return mBufferHandle != NULL && mCacheType != VT_NUM_VERTEX_TYPES;
}

//-------------------------------------------------------------
inline const bool VCNCache::IsIndexBuffer() const
{ 
  return mCacheType==VT_INDEX; 
}

//-------------------------------------------------------------
inline const bool VCNCache::IsVertexBuffer() const
{ 
  return mCacheType!=VT_INDEX && mCacheType!=VT_NUM_VERTEX_TYPES; 
}

//-------------------------------------------------------------
inline const VCNUInt VCNCache::GetBufferSize() const
{ 
  return mBufferSize; 
}

//-------------------------------------------------------------
inline const VCNUInt VCNCache::GetCount() const
{ 
  return mCount; 
}

//-------------------------------------------------------------
inline const VCNUInt VCNCache::GetStride() const
{
  return kCacheStrides[mCacheType];
}


///////////////////////////////////////////////////////////////////////
inline const bool VCNCache::IsFilled() const
{
  return mFilled;
}



///////////////////////////////////////////////////////////////////////
template<typename ElementType>
inline const bool VCNCache::GetBuffer(std::vector<ElementType>& buffer) const
{
  // Get the vertex buffer cache
  VCNByte* rawData = 0;
  if ( !Lock(&rawData) )
  {
    return false;
  }

  buffer.resize( mBufferSize / sizeof(ElementType) );
  memcpy( &buffer[0], rawData, mBufferSize );
  Unlock();

  return true;
}


#endif
