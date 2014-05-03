///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#ifndef VCNSTRINGHASH_H
#define VCNSTRINGHASH_H

#pragma once

// Project includes
#include "Types.h"
#include "StringUtils.h"

template <unsigned int N, unsigned int I>
struct FnvHash
{
	VCN_FORCEINLINE static size_t Hash(const VCNTChar (&in_Str)[N])
	{
		return (FnvHash<N, I-1>::Hash(in_Str) ^ in_Str[I-1])*16777619UL;
	}
};

template <unsigned int N>
struct FnvHash<N, 1>
{
	VCN_FORCEINLINE static size_t Hash(const VCNTChar (&in_Str)[N])
	{
		return (2166136261UL ^ in_Str[0])*16777619UL;
	}
};

inline size_t CalculateFNV(const VCNTChar* str)
{
	const size_t length = _tcslen(str) + 1;

	size_t hash = 2166136261UL;

	for (size_t i=0; i<length; ++i)
	{
		hash ^= *str++;
		hash *= 16777619UL;
	}
	return hash;
}

class VCNStringHash
{
public:

	struct ConstCharWrapper
	{
		inline ConstCharWrapper(const VCNTChar* in_Str) : m_String(in_Str) {}
		const VCNTChar* m_String;
	};

	VCNStringHash(ConstCharWrapper in_Str)
		: m_Hash( CalculateFNV(in_Str.m_String) )
#if defined(DEBUG)
		, m_String( in_Str.m_String )
#endif
	{
	}

#if 0
	VCNStringHash(const VCNString& in_Str)
		: m_Hash( CalculateFNV(in_Str.c_str()) )
		, m_String( in_Str.c_str() )
	{
	}
#endif

	template<unsigned int N>
	VCN_FORCEINLINE VCNStringHash(const VCNTChar (&in_Str)[N])
		: m_Hash( FnvHash<N, N>::Hash(in_Str) )
#if defined(DEBUG)
		, m_String( in_Str )
#endif
	{
	}

	operator size_t() const
	{
		return m_Hash;
	}

	bool operator==(const VCNStringHash& rhs) const
	{
		return m_Hash == rhs.m_Hash;
	}

	bool operator!=(const VCNStringHash& rhs) const
	{
		return m_Hash != rhs.m_Hash;
	}

// Data members

	size_t       m_Hash;
#if defined(DEBUG)
	const VCNString m_String;
#endif
};

#endif // VCNSTRINGHASH_H
