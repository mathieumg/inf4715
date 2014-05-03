///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Defines various rendering constants
///
/// TODO: Move to VCNRenderer module.
///

#ifndef VCNRENDERTYPES_H
#define VCNRENDERTYPES_H

#pragma once

#include "VCNRenderer/DeclarationElement.h"
#include "VCNUtils/Types.h"
#include "VCNUtils/Vector.h"

// Supported built-in effects
enum VCNEffectID
{
	// Renders only geometry
	eidBasic,

	// Renders geometry with lighting
	eidLit,

	// Renders geometry with texture without lighting effect
	eidTextured,

	// Put the geometry out there with textures and apply lighting passes to it
	eidLitTextured,

	// eidLitTextured + Skinning
	eidSkinned,

	// Terrain with multi-texturing
	eidTerrain,

	// Renders a shadow map of the scene
	eidShadowMap,

	// And a constant at the end for the count
	kNumEffects
};


// Names of the constants shared between all shaders
enum VCNShaderPoolConstants
{
	// World matrix
	SPC_World,

	// View/Camera matrix
	SPC_View,

	// Model x View matrix
	SPC_ModelView,

	// Model x View x Projection matrix
	SPC_WorldViewProjection,

	// World normal matrix
	SPC_NormalMatrix,

	// View position vector
	SPC_ViewPosition,

    // Fade amount for fadeout effect
    SPC_FadeAmount,

	kNumShaderPoolConstants
};

//
// ***** LIGHTING TYPES *****
//
enum VCNLightType
{
	// Ambient light
	LT_AMBIENT,     

	// Point light source
	LT_OMNI,        

	// Spot light source
	LT_SPOT,        

	// Directional light source
	LT_DIRECTIONAL, 

	LIGHT_TYPE_COUNT
};

//
// Defines the position vertex structure
//
struct VT_POSITION_STRUCT
{
	union {
		struct {
			VCNFloat x, y, z;
		};
		struct {
			Vector3 v;
		};
	};
};

//
// Defines the lighting vertex structure
//
struct VT_LIGHTING_STRUCT
{
	union {
		struct {
			VCNFloat nx, ny, nz;
		};
		struct {
			Vector3 n;
		};
	};
	struct {
		VCNFloat r, g, b;
	};
};

//
// Defines the screen vertex structure
//
struct VT_SCREEN_STRUCT
{
	VCNFloat  x, y, z, rhw;
	VCNUInt32 color;
};

//
// Defines the screen texture vertex structure
//
struct VT_SCREEN_TEX_STRUCT
{
	VCNFloat  x, y, z, rhw; 
	VCNUInt32 color;        
	VCNFloat  u, v;         
};

//
// ***** CACHE TYPES *****
//
enum VCNCacheType
{
	VT_POSITION,            // Position only
	VT_LIGHTING,            // Colors and normals
	VT_ANIMATION,           // Animation information
	VT_DIFFUSE_TEX_COORDS,  // Texture coordinates for diffuse texture
	VT_NORMAL_TEX_COORDS,   // Texture coordinates for normal map
	VT_BLENDWEIGHTS,        // Blend weight for skinning
	VT_BLENDINDICES,        // Blend indices for skinning
	VT_NUM_VERTEX_TYPES,    // Number of different vertex types (ALWAYS BEFORE LAST)

	VT_CUSTOM,              // Defines a custom buffer declared with a vertex declaration
	VT_SCREEN,              // Screen position and color
	VT_SCREEN_TEX,          // Screen position, color and texture coordinates
	VT_INDEX,               // Index caches don't actually vertex (special case, ALWAYS LAST)

	VT_COUNT
};

enum VCNPrimitiveType
{
	PT_POINTLIST = 1,
	PT_LINELIST,   
	PT_LINESTRIP,
	PT_TRIANGLELIST,
	PT_TRIANGLESTRIP,
	PT_TRIANGLEFAN
};

//
// Cache strides according to buffer type
//
const VCNUInt kCacheStrides[VT_COUNT] =
{
	sizeof(VT_POSITION_STRUCT),   // VT_POSITION
	sizeof(VT_LIGHTING_STRUCT),   // VT_LIGHTING
	24,                           // VT_ANIMATION
	8,                            // VT_DIFFUSE_TEX_COORDS
	8,                            // VT_NORMAL_TEX_COORDS
	16,                           // VT_BLENDWEIGHTS,
	4,                            // VT_BLENDINDICES,
	0,                            // NOT USED! (VT_NUM_VERTEX_TYPES)
	0,                            // VT_CUSTOM (specified at creation)
	sizeof(VT_SCREEN_STRUCT),     // VT_SCREEN
	sizeof(VT_SCREEN_TEX_STRUCT), // VT_SCREEN_TEX
	2                             // VT_INDEX
};

//
// Defines cache operation flags
//
enum VCNCacheFlag
{
	CF_NONE             = 0,

	// Pools
	CF_POOL_MANAGED     = 1 << 1,
	CF_POOL_DEFAULT     = 1 << 2,
	CF_POOL_SYSMEM      = 1 << 3,

	// Usages
	CF_DYNAMIC          = 1 << 4,
	CF_WRITEONLY        = 1 << 5,

	// Locks
	CF_LOCK_DEFAULT     = CF_NONE,
	CF_LOCK_READ_ONLY   = 1 << 10,
	CF_LOCK_DISCARD     = 1 << 11,
	CF_LOCK_NOOVERWRITE = 1 << 12,

	CF_WRITE            = CF_POOL_DEFAULT | CF_DYNAMIC | CF_WRITEONLY
}; typedef VCNUInt32 VCNCacheFlags;

//
// Defines texture flags
//
enum VCNTextureFlag
{
	TF_NONE             = 0,

	TF_USAGE_DYNAMIC    = 1 << 1,

	// Locks
	TF_LOCK_DEFAULT     = TF_NONE,
	TF_LOCK_READ_ONLY   = 1 << 5,
	TF_LOCK_DISCARD     = 1 << 6,
	TF_LOCK_NOOVERWRITE = 1 << 7,

	TF_FMT_A8B8G8R8     = 1 << 10,
	TF_FMT_A8R8G8B8     = 1 << 11,

	TF_POOL_DEFAULT     = 1 << 29,
	TF_POOL_MANAGED     = 1 << 30,
	TF_POOL_SYSMEM      = 1 << 31

}; typedef VCNUInt32 VCNTextureFlags;

//
// Defines rendering states
//
enum VCNRenderState
{
	RS_NONE,                    // just nothing
	RS_CULL_CW,                 // cull clockwise ordered triangles
	RS_CULL_CCW,                // cull counter cw ordered triangles
	RS_CULL_NONE,               // render front- and backsides
	RS_DEPTH_READWRITE,         // read and write depth buffer
	RS_DEPTH_READONLY,          // read but don't write depth buffer
	RS_DEPTH_NONE,              // no read or write with depth buffer
	RS_SHADE_POINTS,            // render just vertices
	RS_SHADE_WIREFRAME,         // render triangulated wire
	RS_SHADE_SOLID,             // render solid polygons
	RS_STENCIL_DISABLE,         // stencil buffer off
	RS_STENCIL_ENABLE,          // stencil buffer off
	RS_STENCIL_FUNC_ALWAYS,     // stencil pass mode
	RS_STENCIL_FUNC_LESSEQUAL,  // stencil pass mode
	RS_STENCIL_MASK,            // stencil mask
	RS_STENCIL_WRITEMASK,       // stencil write mask
	RS_STENCIL_REF,             // reference value
	RS_STENCIL_FAIL_DECR,       // stencil fail decrements
	RS_STENCIL_FAIL_INCR,       // stencil fail increments
	RS_STENCIL_FAIL_KEEP,       // stencil fail keeps
	RS_STENCIL_ZFAIL_DECR,      // stencil pass but z fail decrements
	RS_STENCIL_ZFAIL_INCR,      // stencil pass but z fail increments
	RS_STENCIL_ZFAIL_KEEP,      // stencil pass but z fail keeps
	RS_STENCIL_PASS_DECR,       // stencil and z pass decrements
	RS_STENCIL_PASS_INCR,       // stencil and z pass increments
	RS_STENCIL_PASS_KEEP,       // stencil and z pass keeps
	RS_DEPTHBIAS,               // bis value to add to depth value
	RS_TEXTURE_ADDSIGNED,       // texture stage operation
	RS_TEXTURE_MODULATE,        // texture stage operation
	RS_TEXUTRE_FILTER_NONE,     // disable texture filtering
	RS_TEXUTRE_FILTER_POINT,    // point texture filtering
	RS_TEXTURE_CLAMP,           // clamp texture coordinates
	RS_TEXTURE_WRAP,            // wrap texture coordinates
	RS_BLEND_NONE,              // turn blend off
	RS_BLEND_ADDITIVE,          // set blending to add colors
	RS_BLEND_SUBTRACTIVE,       // set blending to subtract colors
	RS_BLEND_TRANSPARENCY,      // set blending to blend color using transparency
	RS_BLEND_ALPHA,             // set alpha blending            

	RS_LAST
};

// Used by the XformCore to determine what view we are using.
enum VCNProjectionType
{
	PT_PERSPECTIVE,   // perspective projection
	PT_ORTHOGONAL,    // orthogonal projection
	PT_UI             // Makes world coords equal to screen coords
};

typedef VCNUInt64                            VCNShaderParameter;
typedef std::vector<VCNDeclarationElement>   VCNDeclarationElementList;

struct VCNViewport 
{
	VCNULong X;
	VCNULong Y;
	VCNULong Width;
	VCNULong Height;
};

struct VCNPoint
{
	VCNPoint()
	{
	}

	VCNPoint(const VCNLong x, const VCNLong y)
		: x(x)
		, y(y)
	{
	}

	VCNLong x;
	VCNLong y;
};

struct VCNSize
{
	VCNLong width;
	VCNLong height;
};

struct VCNRect
{
	VCNRect()
	{
	}

	VCNRect(VCNLong left, VCNLong top, VCNLong right, VCNLong bottom)
		: left(left), top(top), right(right), bottom(bottom)
	{
	}

	union {
		struct {
			VCNPoint topleft;
			VCNPoint bottomright;
		};
		struct {
			VCNLong left, top, right, bottom;
		};
	};
};

// Defines a base color in Vicuna
struct VCNColor 
{
	VCNColor()
		: R(0.0f)
		, G(0.0f)
		, B(0.0f)
		, A(1.0f)
	{}

	explicit VCNColor(const VCNFloat* colorbuffer)
		: R(colorbuffer[0])
		, G(colorbuffer[1])
		, B(colorbuffer[2])
		, A(colorbuffer[3])
	{}

	explicit VCNColor(VCNFloat r, VCNFloat g, VCNFloat b, VCNFloat a = 1.0f)
		: R(r)
		, G(g)
		, B(b)
		, A(a)
	{
	}

	explicit VCNColor(VCNByte r, VCNByte g, VCNByte b, VCNByte a = 255)
		: R(r / 255.0f)
		, G(g / 255.0f)
		, B(b / 255.0f)
		, A(a / 255.0f)
	{
	}

	static const VCNColor FromInteger(VCNByte r, VCNByte g, VCNByte b, VCNByte a = 255)
	{
		VCNColor color;
		color.R = r / 255.0f;
		color.G = g / 255.0f;
		color.B = b / 255.0f;
		color.A = a / 255.0f;
		return color;
	}

	static const VCNColor FromInteger(VCNUInt32 code)
	{
		VCNColor color;
		color.A = static_cast<VCNByte>(code >> 24);
		color.R = static_cast<VCNByte>(code >> 16);
		color.G = static_cast<VCNByte>(code >> 8);
		color.B = static_cast<VCNByte>(code & 0xFF);
		return color;
	}

	inline VCNColor& operator += ( const VCNColor& c )
	{
		r += c.r;
		g += c.g;
		b += c.b;
		a += c.a;
		return *this;
	}

	inline VCNColor& operator -= ( const VCNColor& c )
	{
		r -= c.r;
		g -= c.g;
		b -= c.b;
		a -= c.a;
		return *this;
	}

	inline VCNColor& operator *= ( float f )
	{
		r *= f;
		g *= f;
		b *= f;
		a *= f;
		return *this;
	}

	inline VCNColor& operator /= ( float f )
	{
		float fInv = 1.0f / f;
		r *= fInv;
		g *= fInv;
		b *= fInv;
		a *= fInv;
		return *this;
	}

	// unary operators
	inline const VCNColor operator + () const
	{
		return *this;
	}

	inline const VCNColor operator - () const
	{
		return VCNColor(-r, -g, -b, -a);
	}

	// binary operators
	inline const VCNColor operator + ( const VCNColor& c ) const
	{
		return VCNColor(r + c.r, g + c.g, b + c.b, a + c.a);
	}

	inline const VCNColor operator - ( const VCNColor& c ) const
	{
		return VCNColor(r - c.r, g - c.g, b - c.b, a - c.a);
	}

	inline const VCNColor operator * ( float f ) const
	{
		return VCNColor(r * f, g * f, b * f, a * f);
	}

	inline const VCNColor operator / ( float f ) const
	{
		float fInv = 1.0f / f;
		return VCNColor(r * fInv, g * fInv, b * fInv, a * fInv);
	}

	friend const VCNColor operator * ( float f, const VCNColor& c)
	{
		return VCNColor(f * c.r, f * c.g, f * c.b, f * c.a);
	}

	const bool operator == ( const VCNColor& c ) const
	{
		return r == c.r && g == c.g && b == c.b && a == c.a;
	}

	const bool operator != ( const VCNColor& c ) const
	{
		return r != c.r || g != c.g || b != c.b || a != c.a;
	}

	const VCNByte Red() const
	{
		return static_cast<VCNByte>(R * 255.0f);
	}

	const VCNByte Green() const
	{
		return static_cast<VCNByte>(G * 255.0f);
	}

	const VCNByte Blue() const
	{
		return static_cast<VCNByte>(B * 255.0f);
	}

	const VCNByte Alpha() const
	{
		return static_cast<VCNByte>(A * 255.0f);
	}

	/// Returns the ARGB value.
	unsigned long ToARGB() const
	{
		return (static_cast<VCNByte>(A*255.0f) << 24) | 
			     (static_cast<VCNByte>(R*255.0f) << 16) | 
			     (static_cast<VCNByte>(G*255.0f) << 8) | 
			     (static_cast<VCNByte>(B*255.0f) << 0);
	}

	// Union between four separate floats, or an array
	union 
	{
		struct 
		{
			VCNFloat R;
			VCNFloat G;
			VCNFloat B;
			VCNFloat A;
		};
		struct 
		{
			VCNFloat r;
			VCNFloat g;
			VCNFloat b;
			VCNFloat a;
		};
		VCNFloat c[4];
	};
};

#endif // VCNRENDERTYPES_H
