///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#ifndef USERALLOCATOR_H
#define USERALLOCATOR_H

#include <malloc.h>

class VCNPhysXDefaultAllocator : public PxAllocatorCallback
{
  void* allocate(size_t size, const char*, const char*, int);

  void deallocate(void* ptr);
};


#endif
