///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Generic OS service interface
///

#ifndef VCNSYSTEM_H
#define VCNSYSTEM_H

#pragma once

#include "VCNCore/Core.h"

#include <functional>

class VCNSystem : public VCNCore<VCNSystem>
{
  VCN_CLASS;

public:

  enum SupportedOS
  {
    UnknownOS = -1,
    Windows32 = 1 << 1,
    Windows64 = 1 << 2,

    Windows = Windows32 | Windows64,

    CurrentOS = 
#if defined( _WINDOWS )
  #if defined( _WIN64 )
      Windows64
  #else
      Windows32
  #endif
#else
      UnknownOS
#endif
  };

  /// Destructor
  virtual ~VCNSystem();

  /// Returns the amount of bytes used by the current process.
  virtual VCNUInt64 GetUsedMemory() const = 0;

  virtual VCNUInt64 GetMinUsedMemory() const = 0;

  virtual VCNUInt64 GetMaxUsedMemory() const = 0;

  /// Returns the amount of free space available to the process.
  virtual VCNUInt64 GetFreeMemory() const = 0;

  /// Returns the amount of memory allocated in the last second.
  virtual VCNFloat32 GetDMPS() const = 0;

	virtual void FindFilesRecursively(const VCNTChar* lpFolder, const VCNTChar* lpFilePattern, std::function<void(const VCNTChar* path)> func) = 0;

  /// Returns the current running OS ID
  SupportedOS GetCurrentOS() const;

  /// Sugar syntax methods to return used memory in various order.
  VCNFloat32 GetUsedMemoryInBytes() const;
  VCNFloat32 GetUsedMemoryInKB() const;
  VCNFloat32 GetUsedMemoryInMB() const;
  VCNFloat32 GetUsedMemoryInGB() const;

  /// Sugar syntax methods to return free memory in various order.
  VCNFloat32 GetFreeMemoryInBytes() const;
  VCNFloat32 GetFreeMemoryInKB() const;
  VCNFloat32 GetFreeMemoryInMB() const;
  VCNFloat32 GetFreeMemoryInGB() const;

protected:

  /// Default constructor
  VCNSystem();

private:

};

//
// INLINES
//

///////////////////////////////////////////////////////////////////////
inline VCNFloat32 VCNSystem::GetUsedMemoryInBytes() const
{
  return static_cast<VCNFloat32>( GetUsedMemory() );
}

///////////////////////////////////////////////////////////////////////
inline VCNFloat32 VCNSystem::GetUsedMemoryInKB() const
{
  return static_cast<VCNFloat32>( GetUsedMemory() / _R(1024.0) );
}

///////////////////////////////////////////////////////////////////////
inline VCNFloat32 VCNSystem::GetUsedMemoryInMB() const
{
  return static_cast<VCNFloat32>( GetUsedMemory() / _R(1048576.0) );
}

///////////////////////////////////////////////////////////////////////
inline VCNFloat32 VCNSystem::GetUsedMemoryInGB() const
{
  return static_cast<VCNFloat32>( GetUsedMemory() / _R(1073741824.0) );
}

///////////////////////////////////////////////////////////////////////
inline VCNFloat32 VCNSystem::GetFreeMemoryInBytes() const
{
  return static_cast<VCNFloat32>( GetFreeMemory() );
}

///////////////////////////////////////////////////////////////////////
inline VCNFloat32 VCNSystem::GetFreeMemoryInKB() const
{
  return static_cast<VCNFloat32>( GetFreeMemory() / _R(1024.0) );
}

///////////////////////////////////////////////////////////////////////
inline VCNFloat32 VCNSystem::GetFreeMemoryInMB() const
{
  return static_cast<VCNFloat32>( GetFreeMemory() / _R(1048576.0) );
}

///////////////////////////////////////////////////////////////////////
inline VCNFloat32 VCNSystem::GetFreeMemoryInGB() const
{
  return static_cast<VCNFloat32>( GetFreeMemory() / _R(1073741824.0) );
}

///////////////////////////////////////////////////////////////////////
inline VCNSystem::SupportedOS VCNSystem::GetCurrentOS() const
{
  return CurrentOS;
}

#endif // VCNSYSTEM_H
