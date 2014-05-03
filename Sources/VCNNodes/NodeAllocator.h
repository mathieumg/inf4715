///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Defines a custom node allocator that is more cache friendly.
///

#ifndef VCNNODEALLOCATOR_H
#define VCNNODEALLOCATOR_H

#pragma once

#include "VCNUtils/Types.h"

#include <deque>

class VCNNode;

class VCNNodePool
{
public:
  static const size_t kBlockCount = 512;
  static const size_t kBlockSize = 512;

  /// Construct the pool
  VCNNodePool();

  /// Destroy the allocator
  ~VCNNodePool();

  /// Returns the number of slot used
  const size_t GetSize() const;

  /// Returns the maximum capacity of the pool
  const size_t GetCapacity() const;

  /// Allocates a new node of type T
  template<class T> 
  void* Allocate();

  /// Allocates a new node of a given size
  void* Allocate(const size_t typeSize);

  /// Deallocate and frees a node slot
  void Deallocate( VCNNode* node );

private:

  struct Block
  {
    VCNByte data[kBlockSize];
  };  

  std::deque<size_t>  mFreeBlocks;
  std::vector<size_t> mUsedBlocks;
  std::vector<Block>  mPool;
};

//
// INLINES
//


///////////////////////////////////////////////////////////////////////
template<class T> 
inline void* VCNNodePool::Allocate()
{
#ifdef USE_MALLOC
  return malloc( sizeof(T) );
#else
  return Allocate( sizeof(T) );
#endif
}


///////////////////////////////////////////////////////////////////////
inline const size_t VCNNodePool::GetSize() const
{
  return mUsedBlocks.size();
}

///////////////////////////////////////////////////////////////////////
inline const size_t VCNNodePool::GetCapacity() const
{
  return mPool.size();
}

#endif // VCNNODEALLOCATOR_H
