///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Declares the declaration element class. 
///

#ifndef DECLARATIONELEMENT_H
#define DECLARATIONELEMENT_H

///
/// Element declaration usages
///
enum ElementUsageType
{
	ELT_USAGE_POSITION,      ///< Vecteur position
	ELT_USAGE_NORMAL,        ///< Vecteur normal
    ELT_USAGE_TANGENT,       /// Tangent
    ELT_USAGE_BINORMAL,      /// Binormal
	ELT_USAGE_COLOR,         ///< Couleur diffuse
	ELT_USAGE_BLENDWEIGHT,   ///< Blend weights
	ELT_USAGE_BLENDINDICES,  ///< Blend indices
	ELT_USAGE_TEXCOORD0,     ///< Coordonnées de texture 0
	ELT_USAGE_TEXCOORD1,     ///< Coordonnées de texture 1
	ELT_USAGE_TEXCOORD2,     ///< Coordonnées de texture 2
	ELT_USAGE_TEXCOORD3      ///< Coordonnées de texture 3
};

///
/// Element declaration types
///
enum ElementType
{
	ELT_TYPE_FLOAT1, ///< 1 float   (32 bits)
	ELT_TYPE_FLOAT2, ///< 2 floats  (64 bits)
	ELT_TYPE_FLOAT3, ///< 3 floats  (96 bits)
	ELT_TYPE_FLOAT4, ///< 4 floats (128 bits)
	ELT_TYPE_COLOR   ///< color     (32 bits)
};

///
/// Structure describing the elements of a vertex declaration.
///
struct VCNDeclarationElement
{
	VCNDeclarationElement(unsigned int _Stream, ElementUsageType _Usage, ElementType _DataType)
		: Stream(_Stream)
		, Offset(-1)
		, Usage(_Usage)
		, DataType(_DataType)
	{
	}

	VCNDeclarationElement(unsigned int _Stream, unsigned short _Offset, ElementUsageType _Usage, ElementType _DataType)
		: Stream(_Stream)
		, Offset(_Offset)
		, Usage(_Usage)
		, DataType(_DataType)
	{
	}

	unsigned int      Stream;   ///< Stream ID
	unsigned short    Offset;   ///< Element offset in stream (in bytes)
	ElementUsageType  Usage;    ///< Element usage
	ElementType       DataType; ///< Element type
};


#endif // DECLARATIONELEMENT_H
