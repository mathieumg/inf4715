///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Abstract render core interface
///

#ifndef VCNRENDERCORE_H
#define VCNRENDERCORE_H

#pragma once

#include "VCNCore/Core.h"
#include "VCNUtils/Error.h"
#include "VCNUtils/RenderTypes.h"
#include "VCNUtils/Types.h"
#include "VCNUtils/Vector.h"

class VCNDeclaration;
class VCNLight;
class VCNMesh;
class VCNModel;
class VCNStateBlock;

class VCNRenderCore : public VCNCore<VCNRenderCore>
{
  VCN_CLASS;

public:
  
  /// Destructor
  virtual ~VCNRenderCore();

// Attributes

  /// Returns the rendering window dimension.
  const VCNPoint GetResolution() const;

// Interface
  
  /// Initialize the rendering core
  virtual VCNBool Initialize() override = 0;

  /// Uninitialize the rendering core.
  virtual VCNBool Uninitialize() override = 0;

  /// Process the upcoming frame
  virtual VCNBool Process(const float dt) override = 0;

// Rendering states

  /// Returns the current shading mode
  virtual VCNRenderState GetShadeMode() = 0;

  /// Sets color blending mode
  virtual void SetBlendMode(VCNRenderState) = 0;

  /// Activate backface culling
  virtual void SetBackfaceCulling(VCNRenderState) = 0;

  /// Activate stencil buffer
  virtual void SetStencilBufferMode(VCNRenderState,VCNULong) = 0;

  /// Activate depth buffer
  virtual void SetDepthBufferMode(VCNRenderState) = 0;

  /// Activate wireframe mode
  virtual void SetShadeMode(VCNRenderState, VCNFloat = 0.0f) = 0;

  /// Sets the current texture/sampler mode
  virtual void SetTextureMode(VCNRenderState) = 0;

  /// Sets texture filtering for mag, min and max
  virtual void SetTextureFiltering(VCNRenderState mag, VCNRenderState min, VCNRenderState max) = 0;

  /// Change background color
  virtual void SetClearColor(VCNFloat fRed, VCNFloat fGreen, VCNFloat fBlue) = 0;

  /// Set the current declaration
  virtual void SetDeclaration(const VCNDeclaration* Declaration) = 0;

  /// Sets the rendering viewport
  virtual const VCNBool SetViewport(const VCNUInt32 width, const VCNUInt32 height) = 0;

  /// Sets a viewport that cover the entire screen
  virtual const VCNBool RestoreFullViewport() = 0;

// Rendering support

  /// Clear all the active graphic buffers
  virtual VCNBool Clear(VCNBool, VCNBool, VCNBool) = 0;

  /// Clear buffers and prepare for rendering
  virtual VCNBool BeginRendering(VCNBool bClearPixel, VCNBool bClearDepth, VCNBool bClearStencil) = 0;

  /// End rendering and flip pixel buffer to front
  virtual void EndRendering() = 0;

  /// Loads an index cache on GPU
  virtual void LoadIndexStream(VCNResID cacheID) = 0;

  /// Loads an vertex buffer on the GPU.
  virtual void LoadVertexStream(VCNResID cacheID, VCNUInt streamIndex) = 0;

//
// Resource creation support
//

  /// Creates a mesh from a DirectX file (.x) on disk
  virtual VCNMesh* CreateMesh(const VCNString& filename) = 0;

  /// Creates a model from a DirectX file (.x) on disk
  virtual VCNModel* CreateModel(const VCNString& filename) = 0;

  /// Creates a rendering cache (supports vertex and index caches)
  virtual VCNResID CreateCache(VCNCacheType cacheType, const void* buffer, VCNUInt numBytes, VCNCacheFlags flags = CF_NONE) = 0;

  /// Creates a vertex cache using a custom declaration
  virtual VCNResID CreateCache(const void* buffer, VCNUInt numBytes, VCNUInt stride, VCNCacheFlags flags = CF_NONE) = 0;

  /// Create a render state block
  virtual VCNStateBlock* CreateStateBlock() = 0;

  /// Creates a vertex declaration
  virtual VCNDeclaration* CreateVertexDeclaration(const VCNDeclarationElementList& elements) const = 0;

//
// Resource rendering support
//

  /// Renders a screen cache to the back buffer
  virtual void RenderScreenCache(const VCNResID cacheID) const = 0;

  /// Renders a screen cache with texture to the back buffer
  virtual void RenderScreenCache(const VCNResID cacheID, const VCNResID textureID) const = 0;

//
// Font support
//

  /// Create a font
  virtual VCNBool CreateFont(const VCNTChar* chType, VCNInt nWeight, VCNBool bItalic, VCNULong dwSize, VCNUInt* pID) = 0;

  /// Draw text: font id, x, y, r, g, b, format string, variables
  VCNRESULT RenderText(VCNUInt nID, VCNInt x, VCNInt y, VCNByte r, VCNByte g, VCNByte b, VCNRect* rect, const VCNTChar* format, ...);
  VCNRESULT RenderText(VCNUInt nID, VCNInt x, VCNInt y, VCNByte r, VCNByte g, VCNByte b, const VCNTChar* format, ...);
  VCNRESULT RenderText(VCNUInt nID, VCNInt x, VCNInt y, const VCNColor& c, VCNRect* rect, const VCNTChar* format, ...);
  VCNRESULT RenderText(VCNUInt nID, VCNInt x, VCNInt y, const VCNColor& c, const VCNTChar* format, ...);
  VCNRESULT RenderText(VCNUInt nID, VCNFloat x, VCNFloat y, VCNByte r, VCNByte g, VCNByte b, VCNRect* rect, const VCNTChar* format, ...);
  VCNRESULT RenderText(VCNUInt nID, VCNFloat x, VCNFloat y, VCNByte r, VCNByte g, VCNByte b, const VCNTChar* format, ...);
  VCNRESULT RenderText(VCNUInt nID, VCNFloat x, VCNFloat y, const VCNColor& c, VCNRect* rect, const VCNTChar* format, ...);
  VCNRESULT RenderText(VCNUInt nID, VCNFloat x, VCNFloat y, const VCNColor& c, const VCNTChar* format, ...);

//
// View support
//

  /// Returns the viewer ray.
  virtual void GetViewer(Vector3& rayOrigin, Vector3& rayDir) const = 0;

  /// Transform screen coordinate into a picking ray
  virtual void ScreenToWorld(VCNInt x, VCNInt y, Vector3& rayOrigin, Vector3& rayDir) const = 0;

//
// Line drawing support
//

  /// Flushes the screen line buffer
  virtual void FlushScreenLines() = 0;

  /// Draws a line on the screen using 2D point coordinates
  virtual void DrawScreenLine(const Vector2& p1, const Vector2& p2, const VCNColor& color) = 0;

  /// Draws a line on the screen using 3D point coordinates (z = 1)
  virtual void DrawScreenLine(const Vector3& p1, const Vector3& p2, const VCNColor& color) = 0;
  
  /// Flush the 3D line buffer to the back buffer.
  virtual void FlushLines() = 0;

  /// Draw a line in 3D space, use Being/EndDrawLine3D
  virtual void DrawLine(const Vector3& p1, const Vector3& p2, const VCNColor& color) = 0;

//
// Profiling support
//

  /// Starts a profiling block
  virtual void ProfileBeginEvent(const VCNString& label, const VCNColor& labelColor) = 0;

  /// Ends the previous profiling block
  virtual void ProfileEndEvent() = 0;

protected:

  /// Draws text using the concrete rendering core
  virtual VCNRESULT RenderText(VCNUInt nID, VCNInt x, VCNInt y, VCNByte r, VCNByte g, VCNByte b, VCNRect* rect, const VCNTChar* format, va_list ap) = 0;

  /// Default constructor
  VCNRenderCore();
  
// Shared data

  VCNULong        mScreenWidth;
  VCNULong        mScreenHeight;
  VCNRenderState  mShadeMode;       // wireframe rendering?
};

//
// INLINE FUNCTIONS
//

///////////////////////////////////////////////////////////////////////
inline const VCNPoint VCNRenderCore::GetResolution() const
{
  return VCNPoint(mScreenWidth, mScreenHeight);
}

#endif // VCNRENDERCORE_H
