///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Defines various template structure utilities
///

#ifndef VCNTEMPLATEUTILS_H
#define VCNTEMPLATEUTILS_H

#pragma once

///
/// Prevents the derived class to be copied or assigned.
///
struct VCNNonCopyable
{

protected:

  VCNNonCopyable() {}
  ~VCNNonCopyable() {}

private:

  VCNNonCopyable(const VCNNonCopyable&);
  VCNNonCopyable& operator=(const VCNNonCopyable&);
};


///////////////////////////////////////////////////////////////////////
///
/// Returns the value based on the target configuration.
/// 
/// @param debug   [IN] value to use in debug mode
/// @param release [IN] value to use in release mode
/// @param final   [IN] value to use in final mode
///
/// @return the value based on the current configuration
///
template<typename T> inline const T VCNTargetValue(const T& debug, const T& release, const T& final)
{
#if defined(FINAL)
  return final;
#elif defined(NDEBUG)
  return release;
#else
  return debug;
#endif
}

///////////////////////////////////////////////////////////////////////
///
/// Same as above, but in final we use the release value.
/// 
/// @param debug   [IN] value to use in debug mode
/// @param release [IN] value to use in release mode
///
/// @return the value based on the current configuration
///
template<typename T> inline const T VCNTargetValue(const T& debug, const T& release)
{
  return VCNTargetValue(debug, release, release);
}

#endif // VCNTEMPLATEUTILS_H
