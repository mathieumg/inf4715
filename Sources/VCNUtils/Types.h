///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Defines engine common types
///
/// @TODO Move to VCNCore
///

#ifndef VCNTYPES_H
#define VCNTYPES_H

#pragma once

// System includes
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <tchar.h>
#include <fstream>

#define VCN_NOINITVTABLE __declspec(novtable)
#define VCN_FORCEINLINE __forceinline

// Basic types
typedef bool                          VCNBool;
typedef char                          VCNChar;
typedef wchar_t                       VCNWChar;
typedef unsigned char                 VCNByte;
typedef int                           VCNInt;
typedef unsigned int                  VCNUInt;
typedef long                          VCNLong;
typedef unsigned long                 VCNULong;
typedef short                         VCNShort;
typedef unsigned short                VCNUShort;
typedef char                          VCNInt8;
typedef short                         VCNInt16;
typedef int                           VCNInt32;
typedef __int64                       VCNInt64;
typedef unsigned char                 VCNUInt8;
typedef unsigned short                VCNUInt16;
typedef unsigned int                  VCNUInt32;
typedef unsigned __int64              VCNUInt64;
typedef float                         VCNFloat;
typedef float                         VCNFloat32;
typedef double                        VCNFloat64;
typedef std::string                   VCNAnsiString;
typedef std::wstring                  VCNUnicodeString;

// NOTE: Use VCNFloat if you want to quickly change from float to double and vice versa.

// Resource core ID
typedef unsigned int                  VCNResID;

// Animation core ID
typedef unsigned int                  VCNAnimID;

// Node ID
typedef unsigned int                  VCNNodeID;

// ResourceID vector
typedef std::vector<VCNResID>         VCNResourceVector;

// Continuous array of bytes
typedef std::vector<VCNByte>          VCNBuffer;

typedef VCNUInt32											VCNFlags;

/// Convert constant literal value to float or double using the right notation.
#define  _R(_x)                       (_x##f)

// ANSI / Unicode string definition.
#ifdef _UNICODE

  typedef wchar_t                     VCNTChar;
  typedef std::wstring                VCNString;
  typedef std::wstringstream          VCNStringStream;
  typedef std::wofstream              VCNFileStream;
  #define VCNTXT(txt)                 L##txt

#else

  typedef char                        VCNTChar;
  typedef std::string                 VCNString;
  typedef std::stringstream           VCNStringStream;
  typedef std::ofstream               VCNFileStream;
  #define VCNTXT(txt)                 txt
  
#endif //_UNICODE

#endif
