///;
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Node allocator implementation
///


#include "Precompiled.h"
#include "NodeAllocator.h"
#include "Node.h"

#include "VCNUtils/Assert.h"

///////////////////////////////////////////////////////////////////////
VCNNodePool::VCNNodePool() : mPool( kBlockCount ), mUsedBlocks( kBlockCount, 0 )
{
  // Mark unused blocks
  for (int i = 0; i < kBlockCount; ++i)
  {
    mFreeBlocks.push_back( i );
  }
}



///////////////////////////////////////////////////////////////////////
VCNNodePool::~VCNNodePool()
{
}


///////////////////////////////////////////////////////////////////////
void* VCNNodePool::Allocate(const size_t nodeSize)
{
  if ( mFreeBlocks.empty() )
  {
#if 0
    // TODO: Implement pool reallocation
    // NEED: Access nodes through handles
    size_t oldSize = mPool.size();
    size_t newSize = oldSize * 2;
    mPool.resize( newSize );

    // Flag new free blocks
    for (size_t i = oldSize; i < newSize; ++i)
    {
      mFreeBlocks.push_back( i );
    }
#else
    assert( !VCNTXT("Pool is not big enough, increase VCNNodePool::kBlockCount.") );
#endif

  }

  //TRACE( VCNTXT("Node size %d\n"), nodeSize );

  // Get a free block
  const size_t blockNeeded = nodeSize / kBlockSize + 1;

  // If we only need one more (most case) return it right now.
  if ( blockNeeded == 1 )
  {
    const size_t freeBlockIndex = mFreeBlocks.front();
    mFreeBlocks.pop_front();
    mUsedBlocks[freeBlockIndex] = blockNeeded;

    return &mPool[freeBlockIndex];
  }

  // We need more than one sequential block
  // We need to found these blocks
  std::sort( mFreeBlocks.begin(), mFreeBlocks.end() );

  size_t firstCheckIndex = 0;
  size_t checkIndex = 1;
  size_t freeCount = 1;
  size_t firstFreeBlockIndex = mFreeBlocks[firstCheckIndex];
  size_t nextFreeBlockIndex = firstFreeBlockIndex+1;
  while (freeCount < blockNeeded && checkIndex < mFreeBlocks.size())
  {
    if ( nextFreeBlockIndex == mFreeBlocks[checkIndex++] )
    {
      ++nextFreeBlockIndex;
      ++freeCount;
    }
    else
    {
      firstCheckIndex = checkIndex;
      freeCount = 1;
      firstFreeBlockIndex = mFreeBlocks[firstCheckIndex];
      nextFreeBlockIndex = firstFreeBlockIndex+1;
    }
  }

  if (freeCount != blockNeeded)
  {
    VCN_ASSERT_FAIL( "Failed to find enough free blocks (%d) to allocate the node (size=%d)", blockNeeded, nodeSize );
  }

  // Remove free blocks markers
  mFreeBlocks.erase( mFreeBlocks.begin()+firstCheckIndex, mFreeBlocks.begin()+firstCheckIndex+blockNeeded );
  mUsedBlocks[firstFreeBlockIndex] = blockNeeded;
  
  return &mPool[firstFreeBlockIndex];
}


///////////////////////////////////////////////////////////////////////
///
/// Frees some fixed blocks given a node. We first find what current 
/// blocks the node uses and mark these as free after calling the node 
/// destructor explicitly.
/// 
/// @param node  [IN] node to delete
///
void VCNNodePool::Deallocate(VCNNode* node)
{
  // Nothing to do when the node is null.
  if ( !node )
    return;

  // Get the index of the node in the pool
  const size_t index = ((VCNByte*)node - (VCNByte*)&mPool[0]) / kBlockSize;
  
  // Validate the index
  VCN_ASSERT( index < mPool.size() );

  // How many block is used by this node?
  const size_t usedBlocks = mUsedBlocks[index];

  // Explicitly call the node destructor
  node->~VCNNode();

  // Mark block as free
  for (size_t i = 0; i < usedBlocks; ++i)
  {
    mFreeBlocks.push_back( index+i );
  }
}
