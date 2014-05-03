///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#include "Precompiled.h"
#include "PhysxUserAllocator.h"

void* VCNPhysXDefaultAllocator::allocate(size_t size, const char*, const char*, int)
{
  return _aligned_malloc(size, 16);
}

void VCNPhysXDefaultAllocator::deallocate(void* ptr)
{
  _aligned_free(ptr);
}
