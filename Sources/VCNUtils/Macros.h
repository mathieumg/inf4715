///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Debug macros
///
/// Common debugging macros.  Many of these macros are similar to those provided
/// by MFC and are designed to allow for "MFC neutral" code (code than can be
/// compiled with or without MFC support).  Other macros add even more debugging
/// facilities.
///
/// Description of available macros:
///
/// ASSERT : Identical to the MFC macro of the same name.
/// VERIFY : Identical to the MFC macro of the same name.
/// TRACE  : Identical to the MFC macro of the same name.
/// INFO   : Similar to TRACE but includes a preamble specifying the
///          file name and line number where the INFO is called as
///          well as forces a line break at the end.
/// DBG    : Code contained within this macro is included only in none FINAL builds.
/// BREAK  : Forces a break in _DEBUG builds via DebugBreak.
///
/// Calling examples:
///   bool test = true;
///   TRACE( L"Testing [test]=%d\n", test );
///   VERIFY( test );
///   test = false;
///   VERIFY( test );
///   INFO( L"[test]=%d", test );
///   ASSERT( test );
///   ASSERT_MSG( test, "Asserted!" );
///   BREAK;
///   DBG( assert( 0 ) );
///

#ifndef VCNMACROS_H
#define VCNMACROS_H

#pragma once

#include "VCNLog/Log.h"

#include <cstdarg>

/// Macros used to transform a statement into à literal string (i.e. "statement").
#define ASTRING2(x) #x
#define ASTRING(x) ASTRING2(x)
#define WSTRING2(x) L#x
#define WSTRING(x) WSTRING2(x)
#define STRING2(x) TEXT(#x)
#define STRING(x) STRING2(x)

#define CONCAT_IMPL( x, y ) x##y
#define MACRO_CONCAT( x, y ) CONCAT_IMPL( x, y )

#ifndef FINAL

  /// Outputs to the debugger's console a message.
  inline void TRACE(const TCHAR* format, ...)
  {
    va_list args;
    va_start(args, format);

    TCHAR buffer[4096];
    _vsntprintf(buffer,
        sizeof (buffer) / sizeof (TCHAR),
        format,
        args);

    VCNLog.LogMessage( kInfo, buffer );

    va_end(args);
  }

#ifndef VERIFY
  /// Verifies that the condition is true, otherwise it asserts.
  #define  VERIFY(f)  { if ( !f ) { ASSERT(!#f); } }
#endif

  /// Traces with the file information
  #define INFO(f, ...)  TRACE(TEXT("%s(%d): "), _T(__FILE__), __LINE__); TRACE(f, __VA_ARGS__); TRACE(TEXT("\n"))

  /// Simple assertion definition
  #define ASSERT assert

  /// Asserts with a simple literal message
  #define ASSERT_MSG(expr, msg) assert(expr && msg);

  /// Breaks the debugger when hit.
  #define BREAK DebugBreak();

  /// Make the statement only active in non-final builds.
  #define DBG(f) f
  
  /// Checks and cast a pointer to another type.
  template<typename T, typename U> inline T safe_pointer_cast(U p) 
  { 
    T casted_pointer = dynamic_cast<T>(p); 
    ASSERT(casted_pointer && "Failed to cast pointer");
    return casted_pointer;
  }

#else // !FINAL

  #define TRACE 
  #define VERIFY(f)  ((void)(f))
  #define INFO(f, ...)
  #ifndef ASSERT
    #define ASSERT(exp)
  #endif
  #define ASSERT_MSG(expr, msg)
  #define BREAK
  #define DBG(f)

  /// Since the safe_pointer_cast should be test in DEBUG build, 
  /// we static_cast here for performance reasons.
  template<typename T, typename U> inline T safe_pointer_cast(U p) 
  { 
    return reinterpret_cast<T>(p); 
  }

#endif  // !FINAL

/// Counts the number of elements in a static array.
#define STATIC_ARRAY_COUNT(_arr) (sizeof(_arr) / sizeof(_arr[0]))

/// Safely deletes a pointer if it is non-null.
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }

/// Safely deletes an dynamic array if it is non-null.
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }

/// Safely releases an interface if it is non-null.
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

/// Safely releases an interface if it is non-null, if the interface is not
/// fully released we print an message saying so.
#define SAFE_RELEASE_CHECK(p)                                                           \
  {                                                                                     \
    if(p)                                                                               \
    {                                                                                   \
      int newRefCount = (p)->Release();                                                 \
      if( newRefCount > 0 )                                                             \
      {                                                                                 \
        TCHAR strError[256];                                                            \
        _stprintf( strError,                                                            \
          TEXT(__FILE__) TEXT(":") STRING(__LINE__) TEXT("\n\n")                        \
          TEXT("The device object failed to cleanup properly.\n ")                      \
          STRING(p) TEXT("->Release() returned a reference count of %d"), newRefCount );\
        VCN_ASSERT_FAIL( strError );                                                    \
      }                                                                                 \
      (p)=NULL;                                                                         \
    }                                                                                   \
  }

/// Prints in VS build console TODO.
/// Usage: TODO( "Refactor this code!" )
#define TODO(msg) __pragma(message(__FILE__ "(" ASTRING(__LINE__) ") : TODO: " ASTRING(msg)))

#endif  // VCNMACROS_H
