//
// Copyright 2011 Autodesk, Inc.  All rights reserved.
//
// Use of this software is subject to the terms of the Autodesk license
// agreement provided at the time of installation or download, or which 
// otherwise accompanies this software in either electronic or hard copy form.   
//
//

#pragma once

#include "Types.h"
#include "GraphicsExport.h"

namespace MaxSDK { namespace Graphics {
	
	/** Supported primitive types. 
	*/
	enum PrimitiveType
	{
		/** A list of independent points.
		*/
		PrimitivePointList,

		/** A list of unconnected two-point line segments, such that the number 
		of lines is the the total number of points divided by two.
		*/
		PrimitiveLineList,

		/** A polyline, where each point is connected to the previous vertex, 
		yielding a single polyline with (vertex_count - 1) two-point line segments.
		*/
		PrimitiveLineStrip,

		/** A list of unconnected triangles, yielding (vertex_count / 3) triangles.
		*/
		PrimitiveTriangleList,

		/** 
		A strip of connected triangles, such that each vertex after the first 
		three vertices defines a new triangle, yielding (vertex_count - 1) connected triangles.

		\remarks Strips of even a small number of triangles provides better performance 
		and reduced memory consumption relative to the same geometry represented with a 
		triangle list. There is substantial research and software devoted to generating 
		optimal triangle strips.  See http://en.wikipedia.org/wiki/Triangle_strip for details.
		*/
		PrimitiveTriangleStrip,

		PrimitiveTypeCount, 
	};

	/** This flag describes the way to clear the current buffers.
	*/
	enum BufferClearFlag
	{
		/** clear the target
		*/
		ClearBackbuffer = 0x1, 
		/** clear the zbuffer
		*/
		ClearDepthBuffer = 0x2, 
		/** clear the stencil
		*/
		ClearStencilBuffer = 0x4, 
		/** clear the zbuffer, target & stencil
		*/
		ClearAll = 0x7,
	};

	/** blend selector. selects a blend source/destination
	*/
	enum BlendSelector
	{
		/** use blend factor in blend state
		*/
		BlendSelectorBlendFactor,
		/** use inverse source alpha
		*/
		BlendSelectorBothInvSourceAlpha,
		/** use source aplha
		*/
		BlendSelectorBothSourceAlpha,
		/** use the destination alpha
		*/
		BlendSelectorDestinationAlpha,
		/** use the destination color
		*/
		BlendSelectorDestinationColor,
		/** use inversed blend factor
		*/
		BlendSelectorInvBlendFactor,
		/** use the inverse destination alpha
		*/
		BlendSelectorInvDestinationAlpha,
		/** use the inverse destination color
		*/
		BlendSelectorInvDestinationColor,
		/** use inverse source alpha
		*/
		BlendSelectorInvSourceAlpha,
		/** use inverse source color
		*/
		BlendSelectorInvSourceColor,
		/** always 1,1,1
		*/
		BlendSelectorOne,
		/** use the source alpha
		*/
		BlendSelectorSourceAlpha,
		/** use the source alpha and saturate it
		*/
		BlendSelectorSourceAlphaSat,
		/** use the source color
		*/
		BlendSelectorSourceColor,
		/** always 0
		*/
		BlendSelectorZero

	};


	/** cull type.
	*/
	enum CullMode
	{
		/** cull backfaces of CW polygons
		*/
		CullModeCW, 
		/** cull backfaces of CCW polygons
		*/
		CullModeCCW,
		/** don't cull backfaces 
		*/
		CullModeNone
	};

	/// <description>
	/// fill mode for polygons
	/// </description>
	enum FillMode
	{
		/** draw as a solid object
		*/
		FillSolid, 
		/** draw as a wireframe object
		*/
		FillWireFrame
	} ;

	/** compare functions
	*/
	enum CompareFunction 
	{
		/** always true
		*/
		CompareFunctionAlways,
		/** true if equal
		*/
		CompareFunctionEqual,
		/** true if greater
		*/
		CompareFunctionGreater, 
		/** true if greater or equal
		*/
		CompareFunctionGreaterEqual,
		/** true if less
		*/
		CompareFunctionLess,
		/** true if less or equal
		*/
		CompareFunctionLessEqual,
		/** never true
		*/
		CompareFunctionNever,
		/** true if not equal
		*/
		CompareFunctionNotEqual
	};


	/** blend operations
	*/
	enum BlendOperation 
	{
		/** add the source and destination together
		*/
		BlendOperationAdd,
		/** max of the source and destination
		*/
		BlendOperationMax,
		/** min of the source and destination
		*/
		BlendOperationMin, 
		/** dest gets dest minus source
		*/
		BlendOperationSubtract,
		/** dest gets source minus dest
		*/
		BlendOperationReverseSubtract,
	};

		/** AccessType Types for accessing buffers.
	*/
	enum AccessType
	{
		/** Data is not accessed yet.
		*/
		AccessTypeNone = 0x0,
		/** Data will only be read.
		*/
		ReadAcess = 0x1,
		/** Data will only be written.
		*/
		WriteAcess = 0x2,
		/* Data will be read and written.
		*/
		ReadWriteAcess = ReadAcess | WriteAcess,  //0x3
		/** To be used by dynamic buffer for write-discard access
		*/
		WriteDiscardAcess
	};

	/** TextureFormat indicates which type the texture is.
	*/
	enum TextureFormat
	{
		/** Unknown texture format
		*/
		TextureFormatUnknown, 

		/** 8 bit int for each of RGB and A, with byte ordering ARGB
		*/
		TextureFormatA8R8G8B8,  

		/** 16 bit int for each of RGB and A, with byte ordering ABGR
		*/
		TextureFormatA16B16G16R16,

		/** 32 bit float for each of RGB and A, with byte ordering ABGR
		*/
		TextureFormatA32B32G32R32F,

		TextureFormatCount, 
	};
	
	/** Get the number of bytes for each pixel by a given texture format.
	\param format The given texture format.
	\returns The number of bytes for each pixel by that given texture format.
	*/
	GraphicsDriverAPI size_t GetTextureFormatPixelSize(TextureFormat format);


} } // end namespace

