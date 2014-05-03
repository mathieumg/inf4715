/*
 * Copyright 2009-2011 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.  Users and possessors of this source code
 * are hereby granted a nonexclusive, royalty-free license to use this code
 * in individual and commercial software.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.   This source code is a "commercial item" as
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of
 * "commercial computer  software"  and "commercial computer software
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 *
 * Any use of this source code in individual and commercial software must
 * include, in the user documentation and internal comments to the code,
 * the above Disclaimer and U.S. Government End Users Notice.
 */
#ifndef D3D9_WPF_RENDERER_H
#define D3D9_WPF_RENDERER_H
#include "D3D9Renderer.h"

namespace SampleRenderer
{
	class WPFRendererListener
	{
	protected:
		virtual ~WPFRendererListener(){}
	public:

		virtual void beforeSurfaceRelease( IDirect3DSurface9* surface ) = 0;
		virtual void afterSurfaceCreated( IDirect3DSurface9* surface ) = 0;
		virtual void release() = 0;
	};

	//This renderer has special interop needs with WFP so it needs to use a slightly slowing
	//device specification.  Also it uses some special creation mechanisms so we get as much
	//performance as we can on various platforms taking the interop considerations into account.

	//One component is that SamplePlatform isn't initialized nor available thus forcing us to override
	//details on some functions
	class D3D9WPFRenderer : public D3D9Renderer
	{
		HWND					mHwnd;
		IDirect3DSurface9*		mCanonicalSurface;
		PxU32					mDesiredWidth;
		PxU32					mDesiredHeight;
		const bool				mLockable;
		WPFRendererListener*	mListener;

	protected:
		
		D3D9WPFRenderer(	HWND hWnd
							, IDirect3D9*	inDirect3d
							, IDirect3DDevice9* inDevice
							, const char* devName
							, PxU32 dispWidth
							, PxU32 dispHeight
							, const char* assetDir
							, bool lockable
							, bool isDeviceEx );

	public:
		static D3D9WPFRenderer* createWPFRenderer(const char* inAssetDir, PxU32 initialWidth = 1024, PxU32 initialHeight = 768 );

		virtual ~D3D9WPFRenderer();

		virtual IDirect3DSurface9* getSurface() { return mCanonicalSurface; }
		virtual WPFRendererListener* getListener();
		virtual void setListener( WPFRendererListener* listener );

		virtual void onResize( PxU32 newWidth, PxU32 newHeight );
		virtual bool swapBuffers();
		virtual bool isOk() const;
		virtual void onDeviceLost();
		virtual void onDeviceReset();

		virtual PxU32 getWidth() { return mDesiredWidth; }
		virtual PxU32 getHeight() { return mDesiredHeight; }
		virtual void render() { beginRender(); endRender(); }
		virtual void setupAlphaBlendState();
		virtual void restoreAlphaBlendState();
		virtual void disableZWrite();
		virtual void enableZWrite();
	protected:
		virtual bool checkResize( bool isDeviceLost );
		void releaseSurface();
		void allocateSurface();
	};
}

#endif