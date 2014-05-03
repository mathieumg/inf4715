///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Direct3D 9 renderer implementation
///

#include "Precompiled.h"
#include "D3D9.h"

#include "D3D9Cache.h"
#include "D3D9Declaration.h"
#include "D3D9Mesh.h"
#include "D3D9ModelLoader.h"
#include "D3D9StateBlock.h"
#include "D3D9Texture.h"

#include "VCNNodes/NodeCore.h"
#include "VCNNodes/RenderNode.h"
#include "VCNRenderer/XformCore.h"
#include "VCNResources/Material.h"
#include "VCNResources/Model.h"
#include "VCNResources/ResourceCore.h"
#include "VCNUtils/StringUtils.h"
#include "VCNUtils/Constants.h"
#include "VCNUtils/Macros.h"

#include <AntTweakBar.h>

namespace
{
	///////////////////////////////////////////////////////////////////////
	///
	/// Little helper function... Converts FLOAT to DWORD
	/// 
	/// @param f [IN] float to be converted
	///
	/// @return bits
	///
	inline DWORD FtoDW( FLOAT f ) 
	{ 
		return *((DWORD*)&f);
	}
}

VCN_TYPE( VCND3D9, VCNRenderCore );

///////////////////////////////////////////////////////////////////////
///
/// Default constructor
///
VCND3D9::VCND3D9()
: mD3D(NULL)
, mDevice(NULL)
, mClearColor(D3DCOLOR_COLORVALUE( 0.0f, 0.0f, 0.0f, 1.0f ))
, mIsSceneRunning(false)
, mFonts(NULL)
, mNumFonts(0)
, mLineVertexBuffer(0)
, mLineBufferCount(0)
, mMaxLineBufferCount(2048)
{
}


///////////////////////////////////////////////////////////////////////
///
/// Destructor. Releases all D3D9 resources.
///
VCND3D9::~VCND3D9() 
{
	Release();
}


///////////////////////////////////////////////////////////////////////
///
/// Release all of our own and the Direct3D COM stuff.
///
void VCND3D9::Release() 
{
	for( UINT k=0; k<mNumFonts; k++ )
	{
		if (mFonts[k]) 
		{
			ULONG refCount = mFonts[k]->Release();
			VCN_ASSERT( refCount == 0 );
			mFonts[k] = NULL;
		}
	}

	if( mFonts )
	{
		free(mFonts);
		mFonts = NULL;
	}

	if( mDevice ) 
	{
		ULONG refCount = mDevice->Release();
		VCN_ASSERT_MSG( refCount == 0, "Direct3D resources (%d) weren't successfully released.", refCount );
		mDevice = NULL;
	}

	if( mD3D ) 
	{
		ULONG refCount = mD3D->Release();
		VCN_ASSERT( refCount == 0 );
		mD3D = NULL;
	}
}


///////////////////////////////////////////////////////////////////////
///
/// Method used to initialize the D3D renderer.
/// Careful, there are many early outs on errors.
///
/// @return true if everything was successfully initialized
///
VCNBool VCND3D9::Initialize()
{
	if ( !VCNRenderCore::Initialize() )
		return false;

	VCN_ASSERT( GLOBAL_WINDOW_HANDLE != NULL && "Need to define window before we can init!" );

	HRESULT  hr;
	
	// Release any previous D3D object
	if( mD3D )
	{
		mD3D->Release();
		mD3D = NULL;
	}

	// Create main D3D object
	// Initialize Direct3D using the Ex function
	mD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if ( !mD3D )
	{
		VCN_ASSERT_FAIL( VCNTXT("VCND3D::Initialize - ERROR: Failed to create Direct3D9 device") );
		return false;
	}

	// Verify that creation was successful
	if( !mD3D )
	{
		TRACE(L"VCND3D::Initialize - ERROR: Direct3DCreate9 failed!");
		return false;
	}

	// Get the device capabilities
	D3DCAPS9 caps;
	mD3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);

	// Make sure we support at least hardware T&L pipeline
	if( (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 )
	{
		TRACE(L"VCND3D::Initialize - ERROR: No hardware vertex processing!");
		mD3D->Release();
		mD3D = NULL;
		return false;
	}


	// Prepare present parameters structure
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof(d3dpp) );

	// get the display mode
	D3DDISPLAYMODE d3ddm;
	mD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);

	// Fill in the structure
	d3dpp.Windowed               = TRUE;
	d3dpp.BackBufferFormat       = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount        = 1;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.MultiSampleType        = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality     = 0;
	d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
	d3dpp.Flags                  = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
	d3dpp.hDeviceWindow          = GLOBAL_WINDOW_HANDLE;
#ifdef FINAL
	d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_DEFAULT;
#else
	d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;   // Disable VSync
#endif

	// Create D3D device
	// Set default settings
	UINT AdapterToUse=D3DADAPTER_DEFAULT;
	D3DDEVTYPE DeviceType=D3DDEVTYPE_HAL;
#ifdef FINAL
	// When building a shipping version, disable PerfHUD (opt-out)
#else
	// Look for 'NVIDIA PerfHUD' adapter
	// If it is present, override default settings
	for (UINT Adapter=0;Adapter<mD3D->GetAdapterCount();Adapter++)
	{
		D3DADAPTER_IDENTIFIER9 Identifier;
		HRESULT Res;
		Res = mD3D->GetAdapterIdentifier(Adapter,0,&Identifier);
		if (strstr(Identifier.Description,"PerfHUD") != 0)
		{
			AdapterToUse=Adapter;
			DeviceType=D3DDEVTYPE_REF;
			break;
		}
	}
#endif // FINAL

	// Create the D3D device
	hr = mD3D->CreateDevice(AdapterToUse, DeviceType, GLOBAL_WINDOW_HANDLE, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &mDevice);

	// Write any errors in the log and bail on error
	if( FAILED(hr) ) 
	{
		VCN_ASSERT_FAIL( L"VCND3D::Initialize - ERROR: IDirect3D::CreateDevice failed!" );

		if( hr == D3DERR_NOTAVAILABLE )
		{
			VCN_ASSERT_FAIL( L"VCND3D::Initialize - D3DERR_NOTAVAILABLE" );
		}
		else if( hr == D3DERR_INVALIDCALL )
		{
			VCN_ASSERT_FAIL( L"VCND3D::Initialize - D3DERR_INVALIDCALL" );
		}
		else if( hr == D3DERR_OUTOFVIDEOMEMORY )
		{
			VCN_ASSERT_FAIL( L"VCND3D::Initialize - D3DERR_OUTOFVIDEOMEMORY" );
		}
		else
		{
			VCN_ASSERT_FAIL( L"VCND3D::Initialize - unknown error" );
		}

		return false;
	}
	
	mIsSceneRunning = false;
	mScreenWidth = d3dpp.BackBufferWidth;
	mScreenHeight = d3dpp.BackBufferHeight;

	// Initialize AntTweakBar
	if( !TwInit(TW_DIRECT3D9, mDevice) )
	{
		MessageBoxA(GLOBAL_WINDOW_HANDLE, TwGetLastError(), "Cannot initialize AntTweakBar", MB_OK|MB_ICONERROR);
		return false;
	}

	return OneTimeInit();
}


//////////////////////////////////////////////////////////////////////////
VCNBool VCND3D9::Uninitialize()
{
	// Terminate AntTweakBar
	TwTerminate();

	ReleaseLineBuffer();

	return VCNRenderCore::Uninitialize();
}


//////////////////////////////////////////////////////////////////////////
VCNBool VCND3D9::Process(const float elapsedTime)
{
	const Matrix4& view = VCNXformCore::GetInstance()->GetViewMatrix();
	const Matrix4& proj = VCNXformCore::GetInstance()->GetProjectionMatrix();
	mDevice->SetTransform(D3DTS_PROJECTION, (const D3DMATRIX*)proj.m);
	mDevice->SetTransform(D3DTS_VIEW,       (const D3DMATRIX*)view.m);

	return VCNRenderCore::Process( elapsedTime );
}


//-------------------------------------------------------------
/// Sets the initial render states.
//-------------------------------------------------------------
VCNBool VCND3D9::OneTimeInit(void)
{
	// Solid rendering
	mShadeMode = RS_SHADE_SOLID;

	// Activate lighting, counter-clock culling and enable the Z buffer
	mDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	mDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW); 
	mDevice->SetRenderState(D3DRS_ZENABLE,  D3DZB_TRUE);

	// Set texture filtering
	mDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	mDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	mDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

	// Set a viewport that takes the full area
	if ( !RestoreFullViewport() )
		return false;

	CreateLineBuffers();

	return true;
}


//-------------------------------------------------------------
/// Creates a D3DXFont object and returns its ID to the 
/// caller for later use.
//-------------------------------------------------------------
VCNBool VCND3D9::CreateFont( const VCNTChar* chType, VCNInt nWeight, VCNBool bItalic,
							 VCNULong dwSize, VCNUInt* pID )
{
	HRESULT hr;
	HDC     hDC;
	int     nHeight;

	if( !pID ) 
		return false;

	hDC = GetDC( NULL );
	nHeight = -MulDiv(dwSize, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	ReleaseDC(NULL, hDC);

	mFonts = (LPD3DXFONT*)realloc( mFonts, sizeof(LPD3DXFONT)*(mNumFonts+1) );

	hr = D3DXCreateFont( mDevice, nHeight, 0, nWeight,
		0, bItalic, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
		chType, &mFonts[mNumFonts] );

	if( SUCCEEDED(hr) )
	{
		(*pID) = mNumFonts;
		mNumFonts++;
		return true;
	}
	else 
		return false;

}


//-------------------------------------------------------------
/// Draws text using the previously created font object.
//-------------------------------------------------------------
VCNRESULT VCND3D9::RenderText(
	VCNUInt nID, VCNInt x, VCNInt y, VCNByte r, VCNByte g, VCNByte b, VCNRect* rect, const VCNTChar* format, va_list args)
{
	if (nID >= mNumFonts)
		return VCN_INVALIDPARAM;

	VCNTChar textBuffer[256];
	_vsntprintf(textBuffer, STATIC_ARRAY_COUNT(textBuffer), format, args);

	RECT rc = { x, y, x+1024, y+1024 };
	
	// Calculate actual size of the text
	if ( rect )
	{
		mFonts[nID]->DrawText( NULL, textBuffer, -1, &rc, DT_SINGLELINE | DT_CALCRECT, 0 );
	}
	// Now draw the text 
	mFonts[nID]->DrawText( NULL, textBuffer, -1, &rc, DT_SINGLELINE, D3DCOLOR_ARGB(255,r,g,b) );

	// Returns the calculated drawing region if requested.
	if ( rect )
	{
		rect->left = rc.left;
		rect->right = rc.right;
		rect->top = rc.top;
		rect->bottom = rc.bottom;
	}

	return VCN_OK;
}


//-------------------------------------------------------------
/// Clear the scene and prepare device for retrieving geometry.
//-------------------------------------------------------------
VCNBool VCND3D9::BeginRendering( bool bClearPixel, bool bClearDepth, bool bClearStencil ) 
{
	// is there anything at all to clear?
	if( bClearPixel || bClearDepth || bClearStencil )
	{
		DWORD flags = 0;

		if( bClearPixel )   
			flags |= D3DCLEAR_TARGET;
		
		if( bClearDepth )   
			flags |= D3DCLEAR_ZBUFFER;

		if( bClearStencil )
			flags |= D3DCLEAR_STENCIL;

		if( FAILED(mDevice->Clear(0, NULL, flags, mClearColor, 1.0f, 0)) )
		{
			TRACE(L"VCND3D::BeginRendering - ERROR!");
			return false;
		}
	}

	if( FAILED(mDevice->BeginScene()) )
		return false;

	mIsSceneRunning = true;
	return true;
}


//-------------------------------------------------------------
/// End of render operations and flip scene to front buffer.
//-------------------------------------------------------------
void VCND3D9::EndRendering(void) 
{
	// Flush non empty buffers.
	FlushLines();
	FlushScreenLines();

	mDevice->EndScene();

	// Swap buffers
	if( FAILED(mDevice->Present(NULL, NULL, NULL, NULL)) )
	{
		TRACE(L"VCND3D - error: Device->Present() from EndRendering() failed");
	}

	mIsSceneRunning = false;
}


//-------------------------------------------------------------
/// Just clear the scene. Only call this when Scene has 
/// already begun.
//-------------------------------------------------------------
VCNBool VCND3D9::Clear( bool bClearPixel, bool bClearDepth, bool bClearStencil ) 
{
	DWORD flags = 0;

	if( bClearPixel )
		flags |= D3DCLEAR_TARGET;

	if( bClearDepth )
		flags |= D3DCLEAR_ZBUFFER;

	if( bClearStencil )
		flags |= D3DCLEAR_STENCIL;

	if( mIsSceneRunning )
		mDevice->EndScene();

	if( FAILED(mDevice->Clear(0, NULL, flags, mClearColor, 1.0f, 0)) ) 
	{
		TRACE(L"VCND3D::Clear - ERROR!");
		return false;
	}

	if( mIsSceneRunning )
		mDevice->BeginScene();

	return true;
}


//-------------------------------------------------------------
/// Change the color of the screen clearing operation.
//-------------------------------------------------------------
void VCND3D9::SetClearColor(float fRed, float fGreen, float fBlue) 
{
	mClearColor = D3DCOLOR_COLORVALUE( fRed, fGreen, fBlue, 1.0f );
}


//-------------------------------------------------------------
/// Set backface culling
//-------------------------------------------------------------
void VCND3D9::SetBackfaceCulling( VCNRenderState rs )
{
	switch( rs )
	{

	case RS_CULL_CW:
		mDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
		break;

	case RS_CULL_CCW:
		mDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		break;

	case RS_CULL_NONE:
		mDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		break;

	default:
		VCN_ASSERT( "Unknown culling state!?" );
	}
}


//-------------------------------------------------------------
/// Set stencil buffer
//-------------------------------------------------------------
void VCND3D9::SetStencilBufferMode( VCNRenderState rs, DWORD dw ) 
{
	switch( rs )
	{
		// switch on and off
		case RS_STENCIL_DISABLE:
			mDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
			break;
		case RS_STENCIL_ENABLE:
			mDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);
			break;
		case RS_DEPTHBIAS:
			mDevice->SetRenderState(D3DRS_DEPTHBIAS, dw);

			// function modes and values
		case RS_STENCIL_FUNC_ALWAYS:
			mDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
			break;
		case RS_STENCIL_FUNC_LESSEQUAL:
			mDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_LESSEQUAL);
			break;
		case RS_STENCIL_MASK:
			mDevice->SetRenderState(D3DRS_STENCILMASK, dw);
			break;
		case RS_STENCIL_WRITEMASK:
			mDevice->SetRenderState(D3DRS_STENCILWRITEMASK, dw);
			break;
		case RS_STENCIL_REF:
			mDevice->SetRenderState(D3DRS_STENCILREF, dw);
			break;

			// stencil test fails modes
		case RS_STENCIL_FAIL_DECR:
			mDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_DECR);
			break;
		case RS_STENCIL_FAIL_INCR:
			mDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_INCR);
			break;
		case RS_STENCIL_FAIL_KEEP:
			mDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
			break;

			// stencil test passes but z test fails modes
		case RS_STENCIL_ZFAIL_DECR:
			mDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_DECR);
			break;
		case RS_STENCIL_ZFAIL_INCR:
			mDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_INCR);
			break;
		case RS_STENCIL_ZFAIL_KEEP:
			mDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
			break;

			// stencil test passes modes
		case RS_STENCIL_PASS_DECR:
			mDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_DECR);
			break;
		case RS_STENCIL_PASS_INCR:
			mDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR);
			break;
		case RS_STENCIL_PASS_KEEP:
			mDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
			break;
	} // switch
}


//-------------------------------------------------------------
/// Set depth buffer
//-------------------------------------------------------------
void VCND3D9::SetDepthBufferMode( VCNRenderState rs ) 
{
	if( rs == RS_DEPTH_READWRITE ) 
	{
		mDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
		mDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	}

	else if( rs == RS_DEPTH_READONLY )
	{
		mDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
		mDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	}

	else if( rs == RS_DEPTH_NONE )
	{
		mDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
		mDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	}
}


//-------------------------------------------------------------
/// Set the shading
//-------------------------------------------------------------
void VCND3D9::SetShadeMode( VCNRenderState smd, float f ) 
{
	switch ( smd )
	{
	case RS_SHADE_POINTS:

		mDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_POINT );
		mDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_FLAT );

		if( f > 0.0f ) 
		{
			mDevice->SetRenderState( D3DRS_POINTSPRITEENABLE,TRUE );
			mDevice->SetRenderState( D3DRS_POINTSCALEENABLE, TRUE );
			mDevice->SetRenderState( D3DRS_POINTSIZE, FtoDW( f ) );
			mDevice->SetRenderState( D3DRS_POINTSIZE_MIN, FtoDW(0.00f) );
			mDevice->SetRenderState( D3DRS_POINTSCALE_A, FtoDW(0.00f) );
			mDevice->SetRenderState( D3DRS_POINTSCALE_B, FtoDW(0.00f) );
			mDevice->SetRenderState( D3DRS_POINTSCALE_C, FtoDW(1.00f) );
		}
		else 
		{
			mDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
			mDevice->SetRenderState(D3DRS_POINTSCALEENABLE,  FALSE);
		}

		break;

	case RS_SHADE_WIREFRAME:

		mDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
		mDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_FLAT );

		break;

	case RS_SHADE_SOLID:

		mDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
		mDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );

		break;

	default:

		VCN_ASSERT_FAIL( "Shading mode not supported" );
		break;

	}

	mShadeMode = smd;

}



///////////////////////////////////////////////////////////////////////
void VCND3D9::SetBlendMode(VCNRenderState mode)
{
	// TODO: Remember last mode to prevent changing state if the same mode as previously
	if( mode == RS_BLEND_ADDITIVE )
	{
		mDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		mDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		mDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		mDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
	}
	else if( mode == RS_BLEND_SUBTRACTIVE )
	{
		mDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		mDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHASAT );
		mDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		mDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT );
	}
	else if ( mode == RS_BLEND_TRANSPARENCY )
	{
		mDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		mDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		mDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	}
	else if ( mode == RS_BLEND_ALPHA )
	{
		mDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		mDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		mDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		mDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
	}
	else if ( mode == RS_BLEND_NONE )
	{
		mDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		mDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
		mDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
	}
	else
	{
		VCN_ASSERT_FAIL( "Blending mode not supported" );
	}
}



//-------------------------------------------------------------
/// Create a cache and add it to the resource core
//-------------------------------------------------------------
VCNResID VCND3D9::CreateCache(VCNCacheType cacheType, const void* pBuffer, VCNUInt numBytes, VCNCacheFlags flags)
{
	VCNResID cacheID = kInvalidResID;

	VCN_ASSERT( numBytes > 0 );

	if(  numBytes )
	{
		// Now lets create the cache
		VCND3D9Cache* cache = new VCND3D9Cache( cacheType, pBuffer, numBytes, flags );

		// Now lets put it in the resource core to get our ID
		cacheID = VCNResourceCore::GetInstance()->AddResource( cache );
	}

	return cacheID;
}



///////////////////////////////////////////////////////////////////////
VCNResID VCND3D9::CreateCache(const void* buffer, VCNUInt numBytes, VCNUInt stride, VCNCacheFlags flags /*= CF_NONE*/)
{
	VCNResID cacheID = kInvalidResID;

	VCN_ASSERT( stride > 0 );
	VCN_ASSERT( numBytes > 0 );

	// Now lets create the cache
	VCND3D9Cache* cache = new VCND3D9Cache( buffer, numBytes, stride, flags );

	// Now lets put it in the resource core to get our ID
	cacheID = VCNResourceCore::GetInstance()->AddResource( cache );

	return cacheID;
}



//-------------------------------------------------------------
/// Loads the index stream
//-------------------------------------------------------------
void VCND3D9::LoadIndexStream( VCNResID cacheID )
{
	// Get the cache
	VCND3D9Cache* cache = VCNResourceCore::GetInstance()->GetResource<VCND3D9Cache>( cacheID );
	VCN_ASSERT( cache->IsIndexBuffer() );

	// Set the indices in the renderer
	mDevice->SetIndices( cache->GetIndexBuffer() );
}


//-------------------------------------------------------------
/// Loads a vertex stream
//-------------------------------------------------------------
void VCND3D9::LoadVertexStream( VCNResID cacheID, VCNUInt streamIndex )
{
	// Get the cache
	VCND3D9Cache* cache = VCNResourceCore::GetInstance()->GetResource<VCND3D9Cache>( cacheID );

	VCN_ASSERT_MSG( cache, VCNTXT("Can't load vertex stream") );

	// Set up the position as stream 0
	mDevice->SetStreamSource( streamIndex, cache->GetVertexBuffer(), 0, cache->GetStride() );
}


//-------------------------------------------------------------
void VCND3D9::ScreenToWorld(VCNInt x, VCNInt y, Vector3& rayOrigin, Vector3& rayDir) const
{
	D3DVIEWPORT9 mainViewport;
	mDevice->GetViewport( &mainViewport );

	Matrix4 matViewInverse;

	const Matrix4& matProjection = VCNXformCore::GetInstance()->GetProjectionMatrix();
	const Matrix4& matView = VCNXformCore::GetInstance()->GetViewMatrix();
	matViewInverse = matView.GetInverse();

	Vector3 v;
	v.x =  ( ( ( _R(2.0) * x ) / mainViewport.Width  ) - 1 ) / matProjection._11;
	v.y = -( ( ( _R(2.0) * y ) / mainViewport.Height ) - 1 ) / matProjection._22;
	v.z = 1.0f;

	// Transform the screen space pick ray into 3D space
	rayDir.x  = v.x*matViewInverse._11 + v.y*matViewInverse._21 + v.z*matViewInverse._31;
	rayDir.y  = v.x*matViewInverse._12 + v.y*matViewInverse._22 + v.z*matViewInverse._32;
	rayDir.z  = v.x*matViewInverse._13 + v.y*matViewInverse._23 + v.z*matViewInverse._33;
	rayDir.Normalize();

	rayOrigin.x = matViewInverse._41;
	rayOrigin.y = matViewInverse._42;
	rayOrigin.z = matViewInverse._43;
}


///////////////////////////////////////////////////////////////////////
VCNMesh* VCND3D9::CreateMesh( const VCNString& filename )
{
	// Now lets create the mesh
	VCNDXMesh* mesh = new VCNDXMesh();
	mesh->LoadFromFile( filename );  

	return mesh;
}


///////////////////////////////////////////////////////////////////////
void VCND3D9::GetViewer( Vector3& rayOrigin, Vector3& rayDir ) const
{
	Matrix4 matViewInverse;

	const Matrix4& matView = VCNXformCore::GetInstance()->GetViewMatrix();
	matViewInverse = matView.GetInverse();

	rayDir.x  = -matViewInverse._31;//v.x*matViewInverse._11 + v.y*matViewInverse._21 + v.z*matViewInverse._31;
	rayDir.y  = -matViewInverse._32;//v.x*matViewInverse._12 + v.y*matViewInverse._22 + v.z*matViewInverse._32;
	rayDir.z  = -matViewInverse._33;//v.x*matViewInverse._13 + v.y*matViewInverse._23 + v.z*matViewInverse._33;
	rayDir.Normalize();

	rayOrigin.x = matViewInverse._41;
	rayOrigin.y = matViewInverse._42;
	rayOrigin.z = matViewInverse._43;
}


///////////////////////////////////////////////////////////////////////
void VCND3D9::FlushLines()
{
	if ( mLineBufferCount == 0 )
		return;

	DWORD oldFVF;
	CComPtr<IDirect3DVertexShader9> oldVS;
	CComPtr<IDirect3DPixelShader9> oldPS;
	CComPtr<IDirect3DVertexDeclaration9> oldVD;
	mDevice->GetVertexShader( &oldVS );
	mDevice->GetPixelShader( &oldPS );
	mDevice->GetFVF( &oldFVF );
	mDevice->GetVertexDeclaration( &oldVD );

	// Deactivate shaders if any
	mDevice->SetVertexShader(NULL);
	mDevice->SetPixelShader(NULL);

	//mDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	mDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	mDevice->SetFVF(D3DFVF_LINE);
	mDevice->SetStreamSource(0, mLineVertexBuffer, 0, sizeof(LINEVERTEX) );
	mDevice->DrawPrimitive(D3DPT_LINELIST, 0, mLineBufferCount);

	// Restore state
	mDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	mDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	mDevice->SetVertexShader(oldVS);
	mDevice->SetPixelShader(oldPS);     
	mDevice->SetVertexDeclaration(oldVD);

	mLineBufferCount = 0;
}


//////////////////////////////////////////////////////////////////////////
void VCND3D9::DrawLine(const Vector3& p1, const Vector3& p2, const VCNColor& c)
{
	LINEVERTEX line[] = {
		{p1.x, p1.y, p1.z, D3DCOLOR_COLORVALUE( c.R, c.G, c.B, 1.0 ) },
		{p2.x, p2.y, p2.z, D3DCOLOR_COLORVALUE( c.R, c.G, c.B, 1.0 ) }
	};

	// Flush buffer if too full.
	if ( mLineBufferCount >= mMaxLineBufferCount )
	{
		FlushLines();
	}
	
	const size_t kBufSize = 2 * sizeof(LINEVERTEX);
	VOID* pBuf = 0;
	HRESULT hr = mLineVertexBuffer->Lock( mLineBufferCount * kBufSize, kBufSize, (void**)&pBuf, 0 );
	memcpy(pBuf, line, kBufSize);
	mLineVertexBuffer->Unlock();

	mLineBufferCount++;
}


///////////////////////////////////////////////////////////////////////
void VCND3D9::CreateLineBuffers()
{
	HRESULT hr = mDevice->CreateVertexBuffer( 
		2 * mMaxLineBufferCount * sizeof(LINEVERTEX), 
		D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 
		D3DFVF_LINE, 
		D3DPOOL_DEFAULT, 
		&mLineVertexBuffer,
		0); 
	VCN_ASSERT( SUCCEEDED(hr) );

	hr = D3DXCreateLine( mDevice, &mScreenLineBuffer );
	VCN_ASSERT( SUCCEEDED(hr) );
}


///////////////////////////////////////////////////////////////////////
void VCND3D9::ReleaseLineBuffer()
{
	ULONG refCount = mLineVertexBuffer->Release();
	VCN_ASSERT( refCount == 0 );

	refCount = mScreenLineBuffer->Release();
	VCN_ASSERT( refCount == 0 );
}


///////////////////////////////////////////////////////////////////////
void VCND3D9::DrawScreenLine(const Vector2& p1, const Vector2& p2, const VCNColor& color)
{
	SCREENLINEVERTEX slv1 = { p1.x, p1.y, 1, 1, color.ToARGB() };
	SCREENLINEVERTEX slv2 = { p2.x, p2.y, 1, 1, color.ToARGB() };

	mScreenLines.push_back( slv1 );
	mScreenLines.push_back( slv2 );
}


///////////////////////////////////////////////////////////////////////
void VCND3D9::DrawScreenLine(const Vector3& p1, const Vector3& p2, const VCNColor& color)
{
	DrawScreenLine( Vector2(p1.x, p1.y), Vector2(p1.x, p2.y), color );
}


///////////////////////////////////////////////////////////////////////
void VCND3D9::FlushScreenLines()
{
	if ( mScreenLines.empty() )
		return;

	mDevice->SetFVF(D3DFVF_SCREENLINE);
	mDevice->DrawPrimitiveUP( D3DPT_LINELIST, mScreenLines.size() / 2, 
		reinterpret_cast<const void*>(&mScreenLines[0]), sizeof( SCREENLINEVERTEX ) );
	
	/*mScreenLineBuffer->Begin();
	mScreenLineBuffer->Draw( , mScreenLines.size() * 2, color.ToARGB() );
	mScreenLineBuffer->End();*/

	mScreenLines.clear();
}


///////////////////////////////////////////////////////////////////////
VCNDeclaration* VCND3D9::CreateVertexDeclaration(const VCNDeclarationElementList& elements) const 
{
	// Create vertex elements from declaration list
	std::vector<D3DVERTEXELEMENT9> d3dVertexElements;
	std::vector<int> offsets(elements.size(), 0);

	// Parse declaration elements
	for (VCNDeclarationElementList::const_iterator it = elements.begin(), end = elements.end(); it != end; ++it)
	{
		D3DVERTEXELEMENT9 tempElement;

		// Stream, offset and method
		tempElement.Stream = it->Stream;
		if ( it->Offset != -1 )
		{
			tempElement.Offset = it->Offset;
			offsets[it->Stream] = it->Offset;
		}
		else
		{
			tempElement.Offset = offsets[it->Stream];
		}
		tempElement.Method = D3DDECLMETHOD_DEFAULT;

		// Usage
		switch (it->Usage)
		{
		case ELT_USAGE_POSITION     : tempElement.Usage = D3DDECLUSAGE_POSITION;    tempElement.UsageIndex = 0; break;
		case ELT_USAGE_NORMAL       : tempElement.Usage = D3DDECLUSAGE_NORMAL;      tempElement.UsageIndex = 0; break;
		case ELT_USAGE_COLOR        : tempElement.Usage = D3DDECLUSAGE_COLOR;       tempElement.UsageIndex = 0; break;
        case ELT_USAGE_TANGENT      : tempElement.Usage = D3DDECLUSAGE_TANGENT;     tempElement.UsageIndex = 0; break;
        case ELT_USAGE_BINORMAL     : tempElement.Usage = D3DDECLUSAGE_BINORMAL;    tempElement.UsageIndex = 0; break;
        case ELT_USAGE_BLENDWEIGHT  : tempElement.Usage = D3DDECLUSAGE_BLENDWEIGHT; tempElement.UsageIndex = 0; break;
		case ELT_USAGE_BLENDINDICES : tempElement.Usage = D3DDECLUSAGE_BLENDINDICES;tempElement.UsageIndex = 0; break;
		case ELT_USAGE_TEXCOORD0    : tempElement.Usage = D3DDECLUSAGE_TEXCOORD;    tempElement.UsageIndex = 0; break;
		case ELT_USAGE_TEXCOORD1    : tempElement.Usage = D3DDECLUSAGE_TEXCOORD;    tempElement.UsageIndex = 1; break;
		case ELT_USAGE_TEXCOORD2    : tempElement.Usage = D3DDECLUSAGE_TEXCOORD;    tempElement.UsageIndex = 2; break;
		case ELT_USAGE_TEXCOORD3    : tempElement.Usage = D3DDECLUSAGE_TEXCOORD;    tempElement.UsageIndex = 3; break;
		}

		// Incrementation type
		switch (it->DataType)
		{
		case ELT_TYPE_FLOAT1 : tempElement.Type = D3DDECLTYPE_FLOAT1;   offsets[it->Stream] += sizeof(float) * 1; break;
		case ELT_TYPE_FLOAT2 : tempElement.Type = D3DDECLTYPE_FLOAT2;   offsets[it->Stream] += sizeof(float) * 2; break;
		case ELT_TYPE_FLOAT3 : tempElement.Type = D3DDECLTYPE_FLOAT3;   offsets[it->Stream] += sizeof(float) * 3; break;
		case ELT_TYPE_FLOAT4 : tempElement.Type = D3DDECLTYPE_FLOAT4;   offsets[it->Stream] += sizeof(float) * 4; break;
		case ELT_TYPE_COLOR :  tempElement.Type = D3DDECLTYPE_D3DCOLOR; offsets[it->Stream] += 4;  break;
		}

		// Add element
		d3dVertexElements.push_back(tempElement);
	}

	// Add element marking the end of the declaration
	D3DVERTEXELEMENT9 EndElement = D3DDECL_END();
	d3dVertexElements.push_back(EndElement);

	// Create the D3D9 declaration object.
	LPDIRECT3DVERTEXDECLARATION9 Declaration = NULL;
	if (FAILED(mDevice->CreateVertexDeclaration(&d3dVertexElements[0], &Declaration)))
		throw VCNException("D3D: CreateVertexDeclaration failed");

	return new VCNDeclarationD3D9(elements, Declaration);
}


///////////////////////////////////////////////////////////////////////
void VCND3D9::SetDeclaration(const VCNDeclaration* declaration)
{
	// Get declaration
	const VCNDeclarationD3D9* declarationD3D9 = static_cast<const VCNDeclarationD3D9*>(declaration);

	// Envoi à l'API
	mDevice->SetVertexDeclaration(declarationD3D9 ? declarationD3D9->GetDeclaration() : NULL);
}



///////////////////////////////////////////////////////////////////////
///
/// To create a model from DirectX mesh, we need
///  - Create a VCNModel
///     - If only one attribute per mesh, the single render node becomes the root model node.
///     - If more than one attribute, create an helper node to group all sub renderable nodes.
///     - Create one vertex cache for position, normals, colors and texture coords
///     - For each attribute
///          - Load material texture
///          - Load face cache
///          - Create VCNMesh and VCNRenderNode
///          - Bind vertex caches to VCNMesh
///          - Bind face cache to VCNMesh
///          - Compute binding sphere of mesh
///          - Bind VCNMesh to VCNRenderNode
///          - Create VCNMaterial
///          - Bind VCNMaterial to VCNRenderNode
/// 
/// @param filename  [IN] file name from which we will load the model
///
/// @return new model
///
VCNModel* VCND3D9::CreateModel(const VCNString& filename)
{
	return D3DModelLoader::Load( filename );
}



///////////////////////////////////////////////////////////////////////
void VCND3D9::ProfileBeginEvent(const VCNString& label, const VCNColor& labelColor)
{
	D3DPERF_BeginEvent( labelColor.ToARGB(), VCN_TCHAR_TO_UNICODE(label.c_str()) );
}



///////////////////////////////////////////////////////////////////////
void VCND3D9::ProfileEndEvent()
{
	D3DPERF_EndEvent();
}

///////////////////////////////////////////////////////////////////////
void VCND3D9::RenderScreenCache(const VCNResID cacheID) const 
{
	// Get D3D cache handle
	VCND3D9Cache* cache = VCNResourceCore::GetInstance()->GetResource<VCND3D9Cache>(cacheID);
	VCN_ASSERT_MSG(cache, "Cache must be valid");

	// Deactivate shaders if any
	mDevice->SetTexture(0, 0);
	mDevice->SetVertexShader(NULL);
	mDevice->SetPixelShader(NULL);

	// Set the render states
	mDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);  
	mDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	mDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE );
	mDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
	mDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);

	// Set my stream source
	mDevice->SetStreamSource( 0, cache->GetVertexBuffer(), 0, sizeof(VT_SCREEN_STRUCT) );
	mDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

	// Draw the background
	mDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP /* TODO: Make this parameter dynamic*/, 0, 2);

	// Restore state
	mDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
}



///////////////////////////////////////////////////////////////////////
void VCND3D9::RenderScreenCache(const VCNResID cacheID, const VCNResID textureID) const 
{
	// Get D3D cache handle
	VCND3D9Cache* cache = VCNResourceCore::GetInstance()->GetResource<VCND3D9Cache>(cacheID);
	VCN_ASSERT_MSG(cache, "Cache must be valid");

	VCND3D9Texture* texture = VCNResourceCore::GetInstance()->GetResource<VCND3D9Texture>( textureID );
	VCN_ASSERT_MSG(texture, "Can't fetch texture");

	// Deactivate shaders if any
	mDevice->SetVertexShader(NULL);
	mDevice->SetPixelShader(NULL);                  

	// Select which vertex format we are using
	mDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);

	// select the vertex buffer to display
	mDevice->SetStreamSource(0, cache->GetVertexBuffer(), 0, sizeof(VT_SCREEN_TEX_STRUCT));

	// Select web view texture to be drawn
	mDevice->SetTexture(0, texture->GetPointer());

	// Draw the image to a screen-quad
	mDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}



///////////////////////////////////////////////////////////////////////
void VCND3D9::SetTextureMode(VCNRenderState mode)
{
	VCN_ASSERT( mDevice );
	VCN_ASSERT( mode == RS_TEXTURE_CLAMP || mode == RS_TEXTURE_WRAP );

	if ( mode == RS_TEXTURE_CLAMP )
	{
		mDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
		mDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
	}
	else
	{
		mDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
		mDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
	}
}

///////////////////////////////////////////////////////////////////////
VCNStateBlock* VCND3D9::CreateStateBlock()
{
	VCND3D9StateBlock* newStateBlock = new VCND3D9StateBlock();
	VCNResourceCore::GetInstance()->AddResource( newStateBlock );
	return newStateBlock;
}



///////////////////////////////////////////////////////////////////////
void VCND3D9::SetTextureFiltering(VCNRenderState mag, VCNRenderState min, VCNRenderState max)
{
	mDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, GetD3DTextureFilter(mag));
	mDevice->SetSamplerState(0, D3DSAMP_MINFILTER, GetD3DTextureFilter(min));
	mDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, GetD3DTextureFilter(max));
}



///////////////////////////////////////////////////////////////////////
const DWORD VCND3D9::GetD3DTextureFilter(const VCNRenderState filter)
{
	if ( filter == RS_TEXUTRE_FILTER_POINT )
		return D3DTEXF_POINT;

	return D3DTEXF_NONE;
}

///////////////////////////////////////////////////////////////////////
const VCNBool VCND3D9::RestoreFullViewport()
{
	return SetViewport( mScreenWidth, mScreenHeight );
}

///////////////////////////////////////////////////////////////////////
const VCNBool VCND3D9::SetViewport(const VCNUInt32 width, const VCNUInt32 height)
{
	D3DVIEWPORT9 d3dVP;
	d3dVP.X      = 0;
	d3dVP.Y      = 0;
	d3dVP.Width  = width;
	d3dVP.Height = height;
	d3dVP.MinZ   = 0.0f;
	d3dVP.MaxZ   = 1.0f;
	if( FAILED(mDevice->SetViewport(&d3dVP)) )
		return false;

	return true;
}
