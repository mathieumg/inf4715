///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief String utility functions
///

#include "Precompiled.h"
#include "StringUtils.h"

#include "VCNCore/Exception.h"

using namespace StringUtils;

///////////////////////////////////////////////////////////////////////
const VCNString StringUtils::TrimRight(const VCNString& source, const VCNString& t /*= " " */)
{
	VCNString str = source;
	return str.erase ( str.find_last_not_of ( t ) + 1 ) ;
}

///////////////////////////////////////////////////////////////////////
const VCNString StringUtils::TrimLeft(const VCNString& source, const VCNString& t /*= " " */)
{
	VCNString str = source;
	return str.erase ( 0 , source.find_first_not_of ( t ) ) ;
}

///////////////////////////////////////////////////////////////////////
const VCNString StringUtils::Trim(const VCNString& source, const VCNString& t /*= " " */)
{
	VCNString str = source;
	return TrimLeft ( TrimRight ( str , t ) , t ) ;
}

///////////////////////////////////////////////////////////////////////
void StringUtils::FindAndReplace(VCNString& source, const VCNString& find, const VCNString& replace)
{
	size_t j;
	for (; (j = source.find(find)) != VCNString::npos;)
	{
		source.replace(j, find.length(), replace);
	}
}

///////////////////////////////////////////////////////////////////////
const VCNString StringUtils::BinaryToDecimal(const VCNString& rhs)
{
	long sum = 0;
	for (VCNString::const_iterator c = rhs.begin(), end = rhs.end(); c != end; ++c)
	{
		if (*c == '0')
		{
			sum <<= 1;
		}
		else if (*c == '1')
		{
			sum = (sum << 1) + 1;
		}
		else
		{
			sum = 0;
			break;
		}
	};

#ifdef UNICODE
	std::wstringstream ss;
#else
	std::stringstream ss;
#endif
	ss << sum;
	return ss.str();
}

///////////////////////////////////////////////////////////////////////
const VCNString StringUtils::DecimalTobinary(const VCNString& rhs)
{
#ifdef UNICODE
	std::wstringstream ss;
#else
	std::stringstream ss;
#endif
	ss << rhs;
	long l;
	ss >> l;
	return DecimalTobinary(l);
}

///////////////////////////////////////////////////////////////////////
const VCNString StringUtils::DecimalTobinary(int l)
{
	return StringUtils::NumberToString(l, 2);
}

///////////////////////////////////////////////////////////////////////
const VCNString StringUtils::NumberToString(int value, int base)
{

	enum { kMaxDigits = 35 };
	VCNString buf;
	buf.reserve( kMaxDigits ); // Pre-allocate enough space.

	// check that the base if valid
	if (base < 2 || base > 16) return buf;

	int quotient = value;

	// Translating number to string with base:
	do {
		buf += "0123456789abcdef"[ std::abs( quotient % base ) ];
		quotient /= base;
	} while ( quotient );

	// Append the negative sign for base 10
	if ( value < 0 && base == 10) buf += '-';

	std::reverse( buf.begin(), buf.end() );
	return buf;
}

VCNUInt64 StringUtils::BernsteinHash(const void* pdataIn, VCNUInt64 size, VCNUInt64 seed)
{
	const VCNByte* pdata   = (const VCNByte*) pdataIn;
	VCNUInt64       h      = seed;
	while (size > 0)
	{
		size--;
		h = ((h << 5) + h) ^ (unsigned) pdata[size];
	}

	return h;
}

///////////////////////////////////////////////////////////////////////
void StringExtractor::ThrowIfEOF()
{
	VCNString Left;
	if (std::getline(mInStream, Left))
		throw VCNException("@left string to long");
}
