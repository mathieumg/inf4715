///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Implements the menu class. 
///

#include "Precompiled.h"
#include "Menu.h"

// Project includes
#include "Game.h"
#include "InputManager.h"
#include "StateMachine.h"
#include "AweHandle.h"

// Engine includes
#include "VCNRenderer/MaterialCore.h"
#include "VCNRenderer/RenderCore.h"
#include "VCNRenderer/StateBlock.h"
#include "VCNResources/Cache.h"
#include "VCNResources/ResourceCore.h"
#include "VCNResources/Texture.h"
#include "VCNUtils/Macros.h"
#include "VCNUtils/StringUtils.h"
#include "VCNUtils/Utilities.h"
#include "VCNUtils/Chrono.h"

static std::map<awe_webview*, Menu*> sViewMap;

inline const std::string AWE2STL(const awe_string* awes)
{
	std::string str;
	const size_t slen = awe_string_get_length( awes );
	str.resize( slen );
	awe_string_to_utf8( awes, &str[0], slen );
	return str;
}

inline const MenuArgs AWE2STL(const awe_jsarray* awearr)
{
	MenuArgs args;
	const size_t alen = awe_jsarray_get_size( awearr );
	args.resize( alen );

	for( size_t i = 0; i < alen; ++i )
	{
		const awe_jsvalue* value = awe_jsarray_get_element( awearr, i );
		awe_jsvalue_type type = awe_jsvalue_get_type( value );
		switch ( type )
		{
		case JSVALUE_TYPE_NULL:
			args[i] = 0;
			break;
		case JSVALUE_TYPE_BOOLEAN:
			args[i] = awe_jsvalue_to_boolean( value );
			break;
		case JSVALUE_TYPE_INTEGER:
			{
				args[i] = awe_jsvalue_to_integer( value );
			}
			break;
		case JSVALUE_TYPE_DOUBLE:
			{
				args[i] = (float)awe_jsvalue_to_double( value );
			}
			break;
		case JSVALUE_TYPE_STRING:
			args[i] = AWE2STL( awe_jsvalue_to_string(value) );
			break;

		// Unsupported cases
		case JSVALUE_TYPE_OBJECT:
		case JSVALUE_TYPE_ARRAY:
			VCN_ASSERT_FAIL( VCNTXT("Not supported") );
			args[i] = 0;
			break;
		default:
			VCN_ASSERT_FAIL( VCNTXT("Not supported") );
			args[i] = 0;
			break;
		}
	}
	
	return args;
}

///////////////////////////////////////////////////////////////////////
Menu::Menu(const char* url)
: mWebView(0)
, mPosX(0) , mPosY(0)
, mWidth(0) , mHeight(0)
, mViewLoaded(false)
, mURL(url)
, mVertexBuffer(0)
, mStateBlock(0)
, mFrameCount(0)
{
	static bool sbWebCoreInitilized = false;
	
	// Initialize once the Awesomium web core
	if ( !sbWebCoreInitilized )
	{
		// TODO: Move Awesomium to a separate core
		awe_webcore_initialize( true, true, false, awe_string_empty(), awe_string_empty(), 
			awe_string_empty(), awe_string_empty(), awe_string_empty(), AWE_LL_NORMAL, false, awe_string_empty(), true, 
			awe_string_empty(), awe_string_empty(), awe_string_empty(), awe_string_empty(), awe_string_empty(), awe_string_empty(),
			false, 0, false, false, awe_string_empty() );

		awe_webcore_set_base_directory( AweString("Menus/") ); 

		sbWebCoreInitilized = true;
	}

	// Get the screen dimension
	const VCNPoint& screenRes = VCNRenderCore::GetInstance()->GetResolution();
	mWidth = screenRes.x;
	mHeight = screenRes.y;
	VCN_ASSERT(mWidth != 0 && mHeight != 0);

	// Create the web view
	mWebView = awe_webcore_create_webview(mWidth, mHeight, false);
	ASSERT(mWebView && "Failed to create web view");

	// Map this view to recover in callbacks.
	sViewMap[mWebView] = this;

	// Load initial view
	awe_webview_load_file( mWebView, AweString(mURL.c_str()), awe_string_empty() );
	awe_webview_set_callback_js_callback( mWebView, OnCallbackHandler );
	awe_webview_set_callback_finish_loading( mWebView, OnFinishLoadingHandler );

	// Preserve transparency (for the HUD)
	awe_webview_set_transparent(mWebView, true);

	// Always create a Native object to call native functions from javascript.
	awe_webview_create_object( mWebView, AweString("Native") );
	
	CreateTexture();
	CreateStateBlock();
	CreateVertexBuffer();
}


///////////////////////////////////////////////////////////////////////
Menu::~Menu()
{
	// Release D3D resources
	mTexture->Release();
	mStateBlock->Release();
	mVertexBuffer->Release();

	// Remove menu from view map
	sViewMap[mWebView] = 0;

	// Release the AWE web view.
	awe_webview_destroy( mWebView );
}

///////////////////////////////////////////////////////////////////////
void Menu::Update( float elapsedTime )
{
	if ( mFrameCount++ % 15 )
		return;

	awe_webcore_update();
	
	// NOTE: Nothing should be bone after this update since the menu might have been deleted.
}

///////////////////////////////////////////////////////////////////////
void Menu::Render() const
{
	if ( !mViewLoaded)
		return;

	// Get the renderer
	VCNRenderCore* renderer = VCNRenderCore::GetInstance();

	// Check if the web view image has been refreshed
	if ( awe_webview_is_dirty(mWebView) )
	{
		const awe_renderbuffer* renderBuffer = awe_webview_render( mWebView );

		// Texture should be same size as WebView
		VCNByte* textureBuffer;
		mTexture->Lock( &textureBuffer, TF_LOCK_DISCARD );
		awe_renderbuffer_copy_to( renderBuffer, textureBuffer, awe_renderbuffer_get_rowspan(renderBuffer), 4, false, false);
		mTexture->Unlock();
	}

	// Apply rendering state to draw the menu
	mStateBlock->Apply();

	// Render the screen texture
	renderer->RenderScreenCache( mVertexBuffer->GetResourceID(), mTexture->GetResourceID() );
}


///////////////////////////////////////////////////////////////////////
void Menu::RegisterCallback(const std::string& callbackName)
{
	awe_webview_set_object_callback( mWebView, AweString("Native"), AweString(callbackName.c_str()) ); 
}


///////////////////////////////////////////////////////////////////////
void Menu::CreateStateBlock()
{
	VCNRenderCore* renderer = VCNRenderCore::GetInstance();

	mStateBlock = renderer->CreateStateBlock();
	mStateBlock->BeginRecord();

	// Setup rendering states
	renderer->SetDepthBufferMode( RS_DEPTH_NONE );
	renderer->SetBlendMode( RS_BLEND_TRANSPARENCY );
	renderer->SetTextureMode( RS_TEXTURE_CLAMP );
	renderer->SetTextureFiltering( 
		RS_TEXUTRE_FILTER_POINT, 
		RS_TEXUTRE_FILTER_POINT, 
		RS_TEXUTRE_FILTER_NONE );

	mStateBlock->EndRecord();
}

///////////////////////////////////////////////////////////////////////
void Menu::CreateVertexBuffer()
{
	VCNRenderCore* renderer = VCNRenderCore::GetInstance();

	const VCNFloat W = (VCNFloat)mWidth;
	const VCNFloat H = (VCNFloat)mHeight;
	VT_SCREEN_TEX_STRUCT vertices[] =
	{
		{ 0.0f, 0.0f, 0, 1.0f, VCNColor::FromInteger(255, 255, 255).ToARGB(), 0, 0},
		{  W,   0.0f, 0, 1.0f, VCNColor::FromInteger(255, 255, 255).ToARGB(), 1, 0},
		{ 0.0f,  H,   0, 1.0f, VCNColor::FromInteger(255, 255, 255).ToARGB(), 0, 1},
		{  W,    H,   0, 1.0f, VCNColor::FromInteger(255, 255, 255).ToARGB(), 1, 1},
	};

	// Adjust all the vertices to correctly line up texels with pixels 
	for (int i = 0; i < 4; ++i)
	{
		vertices[i].x -= 0.5f;
		vertices[i].y -= 0.5f;
	}

	// Create the vertex buffer
	mVertexBuffer = VCNResourceCore::GetInstance()->GetResource<VCNCache>( 
		renderer->CreateCache(VT_SCREEN_TEX, vertices, sizeof(vertices))
		);
	VCN_ASSERT( mVertexBuffer );
}

///////////////////////////////////////////////////////////////////////
void Menu::CreateTexture()
{
	VCNMaterialCore* materialCore = VCNMaterialCore::GetInstance();

	VCNResID texID = materialCore->CreateTexture(mWidth, mHeight, 0, TF_USAGE_DYNAMIC | TF_FMT_A8B8G8R8 | TF_POOL_DEFAULT); 
	mTexture = VCNResourceCore::GetInstance()->GetResource<VCNTexture>( texID );
}

///////////////////////////////////////////////////////////////////////
void Menu::OnMouseMotion(MouseEventArgs& args)
{
	if ( InputManager::GetInstance().IsMouseCaptured() )
		return;

	// Check in buffer if the pixel if transparent or not.
	args.handled |= GetPixelValue(args.x, args.y) != 0;

	mPosX = args.x;
	mPosY = args.y;

	awe_webview_inject_mouse_move( mWebView, mPosX, mPosY );
}

///////////////////////////////////////////////////////////////////////
void Menu::OnLeftMouseButtonDown(MouseEventArgs& args)
{
	// Check in buffer if the pixel if transparent or not.
	unsigned int pixel = GetPixelValue(args.x, args.y);

	awe_webview_inject_mouse_down( mWebView, AWE_MB_LEFT );
	args.handled = pixel != 0;
}

///////////////////////////////////////////////////////////////////////
void Menu::OnLeftMouseButtonUp(MouseEventArgs& args)
{
	// Check in buffer if the pixel if transparent or not.
	args.handled |= GetPixelValue(args.x, args.y) != 0;

	awe_webview_inject_mouse_up( mWebView, AWE_MB_LEFT );
}

///////////////////////////////////////////////////////////////////////
void Menu::OnFinishLoading()
{
	mViewLoaded = true;
}

///////////////////////////////////////////////////////////////////////
void Menu::OnCallbackHandler(awe_webview* caller, 
														 const awe_string* object_name, 
														 const awe_string* callback_name, 
														 const awe_jsarray* arguments)
{
	Menu* menu = sViewMap[caller];
	if ( menu )
	{
		menu->OnCallback( AWE2STL(object_name), AWE2STL(callback_name), AWE2STL(arguments) );
	}
}

///////////////////////////////////////////////////////////////////////
void Menu::OnFinishLoadingHandler(awe_webview* caller)
{
	Menu* menu = sViewMap[caller];
	if ( menu )
	{
		menu->OnFinishLoading();
	}
}

///////////////////////////////////////////////////////////////////////
const unsigned int Menu::GetPixelValue(int x, int y) const
{
	x = std::max(0, std::min(x, static_cast<int>(mWidth-1)));
	y = std::max(0, std::min(y, static_cast<int>(mHeight-1)));

	const awe_renderbuffer* renderBuffer = awe_webview_render( mWebView );
	unsigned char A = awe_renderbuffer_get_alpha_at_point( renderBuffer, x, y );

	return A;
}
