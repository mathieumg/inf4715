///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Direct3D 9 renderer interface
///

#ifndef VCND3D_H
#define VCND3D_H

#pragma once

// Engine includes
#include "VCNRenderer/RenderCore.h"
#include "VCNUtils/Error.h"

// D3D9 forward declarations
// TODO: Make Pimpl to hide these
typedef interface ID3DXFont *LPD3DXFONT;
typedef interface ID3DXLine *LPD3DXLINE;
typedef struct IDirect3D9 *LPDIRECT3D9, *PDIRECT3D9;
typedef struct IDirect3DDevice9 *LPDIRECT3DDEVICE9, *PDIRECT3DDEVICE9;
typedef struct IDirect3DVertexBuffer9 *LPDIRECT3DVERTEXBUFFER9, *PDIRECT3DVERTEXBUFFER9;
typedef struct IDirect3DTexture9 *LPDIRECT3DTEXTURE9, *PDIRECT3DTEXTURE9;
typedef struct IDirect3DStateBlock9 *LPDIRECT3DSTATEBLOCK9, *PDIRECT3DSTATEBLOCK9;
typedef interface ID3DXEffectPool *LPD3DXEFFECTPOOL;
typedef interface ID3DXEffect *LPD3DXEFFECT;
typedef LPCSTR D3DXHANDLE;

///
/// Vicuna class definition for a Direct3D render device
///
class VCND3D9 : public VCNRenderCore
{
  VCN_CLASS;

public:

  /// Default constructor
  VCND3D9();

  /// Destructor
  virtual ~VCND3D9();

// Interface

  // Use with CAUTION!
  LPDIRECT3DDEVICE9 GetD3DDevice() const;

// VCNCore overrides

  /// Initialize the D3D rendering context.
  virtual VCNBool Initialize() override;

  /// Release D3D resources.
  virtual VCNBool Uninitialize() override;

  /// Sets rendering state if needed.
  virtual VCNBool Process(const float elapsedTime) override;
  
  /// Release D3D resources
  virtual void Release();

// VCNRenderCore overrides

  virtual VCNBool CreateFont(const VCNTChar* chType, VCNInt nWeight, VCNBool bItalic, VCNULong dwSize, VCNUInt* pID) override;
  virtual VCNRESULT RenderText(VCNUInt nID, VCNInt x, VCNInt y, VCNByte r, VCNByte g, VCNByte b, VCNRect* rect, const VCNTChar* format, va_list ap) override;
  virtual VCNBool BeginRendering(VCNBool,VCNBool,VCNBool) override;
  virtual VCNBool Clear( VCNBool, VCNBool, VCNBool ) override;
  virtual void EndRendering(void) override;
  virtual void SetClearColor( VCNFloat, VCNFloat, VCNFloat ) override;
  virtual void SetDeclaration(const VCNDeclaration* Declaration) override;
  virtual void SetBackfaceCulling( VCNRenderState ) override;
  virtual void SetDepthBufferMode( VCNRenderState ) override;
  virtual void SetStencilBufferMode( VCNRenderState, DWORD ) override;
  virtual void SetShadeMode( VCNRenderState, float ) override;
  virtual VCNRenderState GetShadeMode() override { return mShadeMode; }
  virtual void SetBlendMode(VCNRenderState) override;
  virtual VCNModel* CreateModel( const VCNString& filename );
  virtual VCNMesh* CreateMesh( const VCNString& filename ) override;
  virtual VCNResID CreateCache(VCNCacheType cacheType, const void* pBuffer, VCNUInt numBytes, VCNCacheFlags flags = CF_NONE) override;
  virtual VCNResID CreateCache(const void* buffer, VCNUInt numBytes, VCNUInt stride, VCNCacheFlags flags = CF_NONE) override;
  virtual void LoadIndexStream( VCNResID cacheID ) override;
  virtual void LoadVertexStream( VCNResID cacheID, VCNUInt streamIndex ) override;
  virtual VCNStateBlock* CreateStateBlock() override;
  virtual VCNDeclaration* CreateVertexDeclaration(const VCNDeclarationElementList& elements) const override;
  virtual void ScreenToWorld(VCNInt x, VCNInt y, Vector3& rayOrigin, Vector3& rayDir) const override;
  virtual void GetViewer(Vector3& rayOrigin, Vector3& rayDir) const override;
  virtual void FlushLines() override;
  virtual void DrawLine(const Vector3& p1, const Vector3& p2, const VCNColor& color) override;
  virtual void DrawScreenLine(const Vector2& p1, const Vector2& p2, const VCNColor& color) override;
  virtual void DrawScreenLine(const Vector3& p1, const Vector3& p2, const VCNColor& color) override;
  virtual void FlushScreenLines() override;
  virtual void ProfileBeginEvent(const VCNString& label, const VCNColor& labelColor) override;
  virtual void ProfileEndEvent() override;
  virtual void RenderScreenCache(const VCNResID cacheID) const override;
  virtual void RenderScreenCache(const VCNResID cacheID, const VCNResID textureID) const override;
  virtual void SetTextureMode(VCNRenderState) override;
  virtual void SetTextureFiltering(VCNRenderState mag, VCNRenderState min, VCNRenderState max) override;
  virtual const VCNBool SetViewport(const VCNUInt32 width, const VCNUInt32 height) override;
  virtual const VCNBool RestoreFullViewport() override;

private:

  static const DWORD GetD3DTextureFilter(const VCNRenderState filter);

  /// Initialize D3D
  VCNBool OneTimeInit(void);

  /// Create the 3D line drawing buffer.
  void CreateLineBuffers();

  /// Releases the 3D line buffer.
  void ReleaseLineBuffer();

//
// Data members
//

  #define D3DFVF_LINE (D3DFVF_XYZ | D3DFVF_DIFFUSE)
  struct LINEVERTEX
  {
    FLOAT x, y, z;
    DWORD color;
  };

  #define D3DFVF_SCREENLINE (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)
  struct SCREENLINEVERTEX
  {
    float x, y, z, rhw;
    DWORD color;
  };

  // Direct 3D interface and device
  LPDIRECT3D9                   mD3D;
  LPDIRECT3DDEVICE9             mDevice;

  // Various flags
  DWORD                         mClearColor;
  VCNBool                       mIsSceneRunning;
  LPD3DXFONT*                   mFonts;  
  UINT                          mNumFonts;

  // Line rendering
  LPDIRECT3DVERTEXBUFFER9       mLineVertexBuffer;
  size_t                        mLineBufferCount;
  const size_t                  mMaxLineBufferCount;
  std::vector<SCREENLINEVERTEX> mScreenLines;
  LPD3DXLINE                    mScreenLineBuffer;
};

//-------------------------------------------------------------
inline LPDIRECT3DDEVICE9 VCND3D9::GetD3DDevice() const
{ 
  return mDevice;
}

#endif // VCND3D_H
