///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Direct3D effect core implementation.
/// 
/// #if 0
/// static const unsigned int kFrameGrab = 512;
/// static unsigned int frame = 0;
/// if (frame % kFrameGrab == 0)
/// {
///   std::stringstream ss;
///   ss << "TextureDumpName" << frame / kFrameGrab << ".tga";
///   hr = D3DXSaveTextureToFileA(ss.str().c_str(), D3DXIFF_TGA, mSSAONDTexture, NULL);
///   VCN_ASSERT( SUCCEEDED(hr) );
/// }
/// frame++;
/// #endif
///

#include "Precompiled.h"
#include "D3D9EffectCore.h"

#include "D3D9.h"
#include "D3D9ShaderCore.h"
#include "D3D9Texture.h"

#include "Effects/D3D9_BasicFX.h"
#include "Effects/D3D9_LitFX.h"
#include "Effects/D3D9_LitTexSkinnedFX.h"
#include "Effects/D3D9_LitTexturedFX.h"
#include "Effects/D3D9_ShadowMapFX.h"
#include "Effects/D3D9_TerrainFX.h"
#include "Effects/D3D9_TexturedFX.h"
#include "Effects/D3D9_SelectionFX.h"

#include "VCNNodes/Light.h"
#include "VCNNodes/NodeCore.h"
#include "VCNRenderer/GPUProfiling.h"
#include "VCNRenderer/LightingCore.h"
#include "VCNRenderer/XformCore.h"
#include "VCNResources/ResourceCore.h"
#include "VCNUtils/Chrono.h"

#include <AntTweakBar.h>

namespace
{
	// Custom vertex
	struct SCREENVERTEX
	{
		FLOAT x, y, z, rhw; // from the D3DFVF_XYZRHW flag
		FLOAT u, v;         // from the D3DFVF_TEX1 flag

		static const DWORD FVF = (D3DFVF_XYZRHW | D3DFVF_TEX1);
	};

	static TwBar* sVCND3D9EffectCoreBar;
}

VCN_TYPE( VCND3D9EffectCore, VCNEffectCore )

///////////////////////////////////////////////////////////////////////
VCND3D9EffectCore::VCND3D9EffectCore()
: mScreenVertexBuffer(0)
, mShadowMapTexture(0)
{
}


///////////////////////////////////////////////////////////////////////
VCND3D9EffectCore::~VCND3D9EffectCore()
{
}


///////////////////////////////////////////////////////////////////////
void VCND3D9EffectCore::CreateToolbar()
{
	std::stringstream ss;
	const VCNUInt32 barWidth = 300;
	const VCNUInt32 barHeight = 225;

	sVCND3D9EffectCoreBar = TwNewBar("VCND3D9EffectCoreBar");

	const VCNPoint& screenDimension = VCNRenderCore::GetInstance()->GetResolution();

	ss <<
		"VCND3D9EffectCoreBar "
		"label='[Effects Tweaks]' "
		"color='150 85 150 235' "
		"position='100 " << screenDimension.y - barHeight - 30 << "' "
		"size='" << barWidth << " " << barHeight << "' "
		"valuesWidth=" << barWidth * 2/3 << " "
		"refresh=1.0"
		<< " iconified=true "
		;

	TwDefine(ss.str().c_str());
}


///////////////////////////////////////////////////////////////////////
///
/// Register all built-in system effects 
///
/// @return true if all effect were properly registered.
///
VCNBool VCND3D9EffectCore::CreateEffects()
{
	// Add our basic effect
	AddEffect( eidBasic, new VCNDXFX_Basic() );
	AddEffect( eidTextured, new VCNDXFX_Textured() );  
	AddEffect( eidLit, new VCNDXFX_Lit() );
	AddEffect( eidLitTextured, new VCND3D9_LitTexturedFX() );
	AddEffect( eidSkinned, new VCND3D9_LitTexturedSkinnedFX() );
	AddEffect( eidShadowMap, new VCNDXFX_ShadowMap() );
    AddEffect( eidTerrain, new VCND3D9_TerrainFX() );

	return true;
}

///////////////////////////////////////////////////////////////////////
VCNBool VCND3D9EffectCore::Initialize()
{
	if ( !VCNEffectCore::Initialize() )
		return false;

	LPDIRECT3DDEVICE9 device = VCNRenderCore::GetInstance()->Cast<VCND3D9>()->GetD3DDevice();

	const VCNPoint& screenRes = VCNRenderCore::GetInstance()->GetResolution();

	// Create the shadow map render target
	HRESULT hr = device->CreateTexture(screenRes.x, screenRes.y, 1, D3DUSAGE_RENDERTARGET, D3DFMT_G32R32F, D3DPOOL_DEFAULT, &mShadowMapTexture, 0);
	VCN_ASSERT_MSG( SUCCEEDED(hr), _T("Unable to create shadow map render target texture") );

	// Create a state block to capture all
	device->CreateStateBlock( D3DSBT_ALL, &mAllStateBlock );
	
	CreateScreenVertexBuffer(); 

	return true;
}


///////////////////////////////////////////////////////////////////////
VCNBool VCND3D9EffectCore::Uninitialize()
{
	// Release VCNEffectCore first so effect gets released first. 
	// These effects might have hold some resources.
	if ( !VCNEffectCore::Uninitialize() )
		return false;

	//
	// Release D3D resources
	//

	SAFE_RELEASE( mShadowMapTexture );

	SAFE_RELEASE_CHECK( mAllStateBlock );
	SAFE_RELEASE_CHECK( mScreenVertexBuffer );

	return true;
}


///////////////////////////////////////////////////////////////////////
void VCND3D9EffectCore::CreateScreenVertexBuffer()
{
	LPDIRECT3DDEVICE9 d3dDevice = static_cast<VCND3D9*>(VCNRenderCore::GetInstance())->GetD3DDevice();

	// Create four vertices using the CUSTOMVERTEX struct built earlier
	const VCNPoint& screenRes = VCNRenderCore::GetInstance()->GetResolution();
	const FLOAT W = (FLOAT)screenRes.x;
	const FLOAT H = (FLOAT)screenRes.y;
	SCREENVERTEX vertices[] =
	{
		{ 0.0f, 0.0f, 0, 1.0f, 0, 0},
		{  W,   0.0f, 0, 1.0f, 1, 0},
		{ 0.0f,  H,   0, 1.0f, 0, 1},
		{  W,    H,   0, 1.0f, 1, 1},
	};

	// Adjust all the vertices to correctly line up texels with pixels 
	for (int i = 0; i < 4; ++i)
	{
		vertices[i].x -= 0.5f;
		vertices[i].y -= 0.5f;
	}

	HRESULT hr = d3dDevice->CreateVertexBuffer(
		4 * sizeof(SCREENVERTEX),
		D3DUSAGE_WRITEONLY,
		SCREENVERTEX::FVF,
		D3DPOOL_MANAGED,
		&mScreenVertexBuffer,
		NULL);
	VCN_ASSERT( SUCCEEDED(hr) );

	VOID* pVoid;
	if ( SUCCEEDED(mScreenVertexBuffer->Lock(0, 0, (void**)&pVoid, 0)) )
	{
		memcpy(pVoid, vertices, sizeof(vertices)); 
		mScreenVertexBuffer->Unlock();  
	}
}


///////////////////////////////////////////////////////////////////////
///
/// Generate a shadow map of the scene
///
void VCND3D9EffectCore::GenerateShadowMap()
{
	GPU_PROFILE_BLOCK_NAME( VCNTXT("Shadow Map") );

	HRESULT hr = S_FALSE;
	
	// Notify that we have start to render shadow map
	mGeneratingShadowMap = true;

	VCND3D9* renderer = static_cast<VCND3D9*>(VCNRenderCore::GetInstance());
	LPDIRECT3DDEVICE9 device = renderer->GetD3DDevice();

	// Keep track of the previous back buffer
	CComPtr<IDirect3DSurface9> d3dBackBuffer;
	hr = device->GetRenderTarget(0, &d3dBackBuffer);

	// Set shadow map surface and start rendering of scene using the shadow map effect
	CComPtr<IDirect3DSurface9> shadowMapSurface;
	hr = mShadowMapTexture->GetSurfaceLevel(0, &shadowMapSurface);
	VCN_ASSERT( SUCCEEDED(hr) );

	hr = device->SetRenderTarget(0, shadowMapSurface);
	VCN_ASSERT( SUCCEEDED(hr) );

	// Set the map viewport
	D3DSURFACE_DESC d3dSurfaceDesc;
	shadowMapSurface->GetDesc( &d3dSurfaceDesc );
	VCNRenderCore::GetInstance()->SetViewport(d3dSurfaceDesc.Width, d3dSurfaceDesc.Height);

	// Clear the shadow map for what's coming
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

	// Start rendering to the map
	hr = device->BeginScene();
	VCN_ASSERT( SUCCEEDED(hr) );  
	
	device->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

	// Backup current view settings
	const Matrix4 oldView = VCNXformCore::GetInstance()->GetViewMatrix();
	const Matrix4 oldProj = VCNXformCore::GetInstance()->GetProjectionMatrix();
	const VCNProjectionType oldProjType = VCNXformCore::GetInstance()->GetProjectionType();
	
	// Set the view as the light view when generating the shadow map.
	VCNLight* shadowLightSource = VCNLightingCore::GetInstance()->GetLight(0);
	const Matrix4 lightProj = shadowLightSource->GetProjectionMatrix();
	const Matrix4 lightView = shadowLightSource->GetViewMatrix();
	
	VCNXformCore::GetInstance()->SetViewLookAt( lightView );
	VCNXformCore::GetInstance()->SetProjectionMatrix( lightProj );
	
	// Draw scene
	// TODO: Generate map for each light
	VCNNodeCore::GetInstance()->GetRootNode()->Render();

	// Restore view
	VCNXformCore::GetInstance()->SetProjectionType( oldProjType, oldProj );
	VCNXformCore::GetInstance()->SetViewLookAt( oldView );

	// Restore color writes
	device->SetRenderState(D3DRS_COLORWRITEENABLE, 
		D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);

	hr = device->EndScene();
	VCN_ASSERT( SUCCEEDED(hr) );

	// Set shadow map texture for effects that needs it
	// TODO: Effects that need the shadow map should pull it from the effect core shadow maps pool
	mEffectMap[eidLitTextured]->Cast<VCND3D9_LitTexturedFX>()->SetShadowMapTexture( mShadowMapTexture );
	mEffectMap[eidSkinned]->Cast<VCND3D9_LitTexturedSkinnedFX>()->SetShadowMapTexture( mShadowMapTexture );
	mEffectMap[eidTerrain]->Cast<VCND3D9_TerrainFX>()->SetShadowMapTexture( mShadowMapTexture );

	// Restore back buffer
	device->SetRenderTarget(0, d3dBackBuffer);

	// Restore back buffer viewport
	VCNRenderCore::GetInstance()->RestoreFullViewport();

	// We are done generating shadow map
	mGeneratingShadowMap = false;
}


///////////////////////////////////////////////////////////////////////
const VCNBool VCND3D9EffectCore::DebugRender() const 
{
	HRESULT hr = S_FALSE;

	VCND3D9* renderer = static_cast<VCND3D9*>(VCNRenderCore::GetInstance());
	LPDIRECT3DDEVICE9 device = renderer->GetD3DDevice();

	// Draw the SSAO result to final screen
	if (::GetAsyncKeyState(VK_F6) & 0x8000)
	{
		// Take a snapshot and save it to disk, when SHIFT is pressed.
		if ( ::GetAsyncKeyState(VK_SHIFT) & 0x8000 )
		{
			static unsigned int frame = 0;
			const unsigned int kFrameGrab = 512;
			
			if (frame++ % kFrameGrab == 0)
			{
				char textureName[128];
				_snprintf(textureName, sizeof(textureName), "ShadowMap%d.png", frame / kFrameGrab);
				hr = D3DXSaveTextureToFileA(textureName, D3DXIFF_PNG, mShadowMapTexture, NULL);
				VCN_ASSERT( SUCCEEDED(hr) );
			}
		}
		mAllStateBlock->Capture();

		device->SetVertexDeclaration(NULL);
		device->SetVertexShader(NULL);
		device->SetPixelShader(NULL);

		device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
		device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
		device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

		device->SetTexture(0, mShadowMapTexture);

		// Select the vertex buffer to display
		device->SetFVF(SCREENVERTEX::FVF);
		device->SetStreamSource(0, mScreenVertexBuffer, 0, sizeof(SCREENVERTEX));
		
		device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

		mAllStateBlock->Apply();

		return true;
	}
	
	return false;
}
