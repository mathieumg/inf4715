///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Custom Assertion interface
///
///    Assert can be called using:
///          VCN_ASSERT( false );
///          VCN_ASSERT_MSG( expression, "This is a message trying to explain why it asserts" );
///          VCN_ASSERT_MSG( expression, "You can use printf(%d, %f) like syntax",  42, 2.4f);
///          VCN_ASSERT_FAIL( "Critical path!, This assertion always make it fails." );
///

#ifndef VCNASSERT_H
#define VCNASSERT_H

#ifndef WSTRING2
  #define WSTRING2(x) L#x
  #define WSTRING(x) WSTRING2(x)
#endif

#include <cassert>

/// Display an assertion (not meant to be used directly, use MACRO below)
const bool CustomAssert(
  int value, 
  const wchar_t* expression, 
  const wchar_t* message, 
  int line, 
  const char* file, 
  bool& ignored);

/// Formats the assertion using an ASNI string.
const bool CustomAssert(
  int value, 
  const wchar_t* expression, 
  int line, 
  const char* file, 
  bool& ignored, 
  const char* format, ...);

/// Formats the assertion using an UNICODE string.
const bool CustomAssert(
  int value, 
  const wchar_t* expression, 
  int line, 
  const char* file, 
  bool& ignored, 
  const wchar_t* format, ...);

#ifndef FINAL

  /// Asserts if exp is false
  #define VCN_ASSERT( exp )                                                                           \
  {                                                                                                   \
    static bool ignoreAlways = false;                                                                 \
    if( !ignoreAlways && !(exp) )                                                                     \
    {                                                                                                 \
      if( CustomAssert( (int)(exp), WSTRING(exp), L"", __LINE__, __FILE__, ignoreAlways ) ) \
      {                                                                                               \
        _asm { int 3 }                                                                                \
      }                                                                                               \
    }                                                                                                 \
  }

  /// Asserts with a printf like message if exp is false
  #define VCN_ASSERT_MSG( exp, format, ... )                                                                          \
  {                                                                                                                   \
    static bool ignoreAlways = false;                                                                                 \
    if( !ignoreAlways && !(exp) )                                                                                     \
    {                                                                                                                 \
      if( CustomAssert( (int)(exp), WSTRING(exp), __LINE__, __FILE__, ignoreAlways, format, __VA_ARGS__) )  \
      {                                                                                                               \
        _asm { int 3 }                                                                                                \
      }                                                                                                               \
    }                                                                                                                 \
  }

  /// Always assert
#define VCN_ASSERT_FAIL(format, ... )  { VCN_ASSERT_MSG(false, format, __VA_ARGS__) }

#else

  // In FINAL build, asserts are disabled for performance reasons.

  #define VCN_ASSERT( exp )
  #define VCN_ASSERT_MSG( exp, format, ... )
  #define VCN_ASSERT_FAIL(format, ... ) assert( !format )

#endif

/// Assert that can be used in final builds
#define VCN_FINAL_ASSERT( exp, format, ... )                                                              \
{                                                                                                         \
  static bool ignoreAlways = false;                                                                       \
  if( !ignoreAlways && !(exp) )                                                                           \
  {                                                                                                       \
    if( CustomAssert( (int)(exp), WSTRING(exp), __LINE__, __FILE__, ignoreAlways, format, __VA_ARGS__) )  \
    {                                                                                                     \
      _asm { int 3 }                                                                                      \
    }                                                                                                     \
  }                                                                                                       \
}

#endif // VCNASSERT_H
