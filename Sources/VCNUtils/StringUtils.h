///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Defines string handling utilities.
///

#ifndef STRINGUTILS_H
#define STRINGUTILS_H

// Engine includes
#include "VCNUtils/Types.h"

// System includes
#include <tchar.h>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>

namespace StringUtils
{

	/// Splits a string into tokens given a delimiter.
	void Split(const VCNString& str, std::vector<VCNString>& tokens, const VCNString& delimiters = VCNTXT(" \t\n"));

	/// Splits a string into tokens given a delimiter.
	const std::vector<VCNString> Split(const VCNString& str, const VCNString& delimiters = VCNTXT(" \t\n"));

	/// Reads an entire file and returns its content.
	const VCNString ReadFile(const VCNString& filename);

	/// Returns the hash code of a given string.
	const unsigned long StringHash(const VCNString& str);

	/// Returns the lower case instance of a string.
	const VCNString ToLower(const VCNString& str);

	/// Returns the upper case instance of a string.
	const VCNString ToUpper(const VCNString& str);

	/// Converts a ANSI string into a UNICODE string.
	const std::wstring StringToWString(const std::string& s);

	/// Converts a UNICODE string into an ANSI string.
	const std::string WStringToString(const std::wstring& s);

	/// Trims right.
	const VCNString TrimRight(const VCNString& source, const VCNString& t = VCNTXT(" "));

	/// Trims left.
	const VCNString TrimLeft(const VCNString& source, const VCNString& t = VCNTXT(" "));

	/// Trims left and right.
	const VCNString Trim(const VCNString& source, const VCNString& t = VCNTXT(" "));

	/// Find and replace a string
	void FindAndReplace(VCNString& source, const VCNString& find, const VCNString& replace);

	/// Converts a binary number to a decimal based number
	const VCNString BinaryToDecimal(const VCNString& rhs);

	/// Converts a string decimal number to a binary based number
	const VCNString DecimalTobinary(const VCNString& rhs);

	/// Converts a decimal number to a binary based number
	const VCNString DecimalTobinary(int l);

	/// C++ version VCNString style "itoa"
	const VCNString NumberToString(int value, int base);

	// Hash function
	VCNUInt64 BernsteinHash(const void* pdataIn, VCNUInt64 size, VCNUInt64 seed = 42);
}

///
/// Build strings
///
class StringBuilder
{
public :

	/// Default ctor
	StringBuilder();

	/// Construct the builder with a first value.
	template <typename T> StringBuilder(const T& value);

	/// Adds a value to the string using ()
	template <typename T> StringBuilder& operator ()(const T& value);
	
	/// Adds a value to the string using +
	template <typename T> StringBuilder& operator +(const T& value);

	/// Adds a value to the string using <<
	template <typename T> StringBuilder& operator <<(const T& value);

	/// Adds a value to the string using +=
	template <typename T> StringBuilder& operator +=(const T& value);

	/// Explicitly converts to a string object.
	operator const VCNString();

private :

	// String stream.
	std::wostringstream mOutStream;
};

///
/// Extract values from a string.
///
class StringExtractor
{
public :

	/// Builds the extractor with the string to parse.
	StringExtractor(const VCNString& text);

	/// Extracts a value at buffer position.
	template <typename T> StringExtractor& operator ()(T& Value);

	/// Throws an exception if there's nothing to extract.
	void ThrowIfEOF();

private :

	// In buffer stream
	std::wistringstream mInStream;
};

#include "StringUtils.inl"

#ifdef _UNICODE

	/// Macros used to convert string around.
	#define  VCN_TCHAR_TO_UNICODE(txt)    txt
	#define  VCN_TCHAR_TO_ASCII(txt)      StringUtils::WStringToString(txt).c_str()
	#define  VCN_UNICODE_TO_TCHAR(txt)    txt
	#define  VCN_ASCII_TO_TCHAR(txt)      StringUtils::StringToWString(txt).c_str()
				 
#else

	// Conversion functions
	#define  VCN_TCHAR_TO_UNICODE(txt)    StringUtils::StringToWString(txt).c_str()
	#define  VCN_TCHAR_TO_ASCII(txt)      txt
	#define  VCN_UNICODE_TO_TCHAR(txt)    StringUtils::WStringToString(txt).c_str()
	#define  VCN_ASCII_TO_TCHAR(txt)      txt

#endif //_UNICODE

	#define VCN_A2W(txt)  StringUtils::StringToWString(txt).c_str()
	#define VCN_W2A(txt)  StringUtils::WStringToString(txt).c_str()

#endif // STRINGUTILS_H
