#include "VCNUtils/Types.h"
///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Defines some macro to profile GPU statements
///

#ifndef VCNGPUPROFILING_H
#define VCNGPUPROFILING_H

#include "VCNUtils/Types.h"

// These first two macros are taken from the
// VStudio help files - necessary to convert the
// __FUNCTION__ symbol from char to wchar_t.
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)

#if !defined(FINAL)

// Only the first of these macro's should be used. The _INTERNAL
// one is so that the sp##id part generates "sp1234" type identifiers
// instead of always "sp__LINE__"...
#define GPU_PROFILE_BLOCK() GPU_PROFILE_BLOCK_INTERNAL( __LINE__ )
#define GPU_PROFILE_BLOCK_INTERNAL(id) VCNGPUProfiling sp##id ( WIDEN(__FUNCTION__), __LINE__ );
#define GPU_PROFILE_BLOCK_NAME(_name_)  VCNGPUProfiling sp##__COUNTER__( _name_ L"[" WIDEN(__FUNCTION__) L"]", __LINE__ );
#define GPU_PROFILE_BLOCK_NAME_IDX(_name_, _idx_)  VCNGPUProfiling sp##__COUNTER__( _name_ L"[" WIDEN(__FUNCTION__) L"]", _idx_ );
#define GPU_PROFILE_OBJECT_BLOCK(_object_name_)  VCNGPUProfiling sp##__COUNTER__( _object_name_ );

#else

#define GPU_PROFILE_BLOCK()
#define GPU_PROFILE_BLOCK_INTERNAL(id)
#define GPU_PROFILE_BLOCK_NAME(_name_)
#define GPU_PROFILE_BLOCK_NAME_IDX(_name_, _idx_)
#define GPU_PROFILE_OBJECT_BLOCK(_object_name_)

#endif

// To avoid polluting the global namespace,
// all D3D utility functions/classes are wrapped
// up in the D3DUtils namespace.
class VCNGPUProfiling
{
public:

  /// Begin the profiling block
  explicit VCNGPUProfiling(const VCNTChar* name, const int line = 0);

  /// Ends the profiling block
  ~VCNGPUProfiling();

private:
  VCNGPUProfiling();
};

#endif // VCNGPUPROFILING_H
