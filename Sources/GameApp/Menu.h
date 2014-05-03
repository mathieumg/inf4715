///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Declares the menu class. 
///

#ifndef MENU_H
#define MENU_H

#pragma once

#include "InputController.h"
#include "CameraController.h"

// Engine includes
#include "VCNUtils/Any.h"
#include "VCNUtils/RenderTypes.h"

class VCNCache;
class VCNStateBlock;
class VCNTexture;

// Awesomium forward declarations
typedef struct _awe_webview awe_webview;
typedef struct _awe_jsvalue awe_jsvalue;
typedef struct _awe_jsarray awe_jsarray;
typedef struct _awe_jsobject awe_jsobject;
typedef struct _awe_renderbuffer awe_renderbuffer;
typedef struct _awe_header_definition awe_header_definition;
typedef struct _awe_string awe_string;

typedef std::vector<VCNAny> MenuArgs;

///
/// This class serves as a GUI panel to group GUI elements
///
class Menu : public InputController
{
public:

	/// Default ctor
	Menu(const char* url);

	/// Default dtor
	virtual ~Menu() = 0;

	// Interface

	const VCNBool IsOverView(VCNInt32 x, VCNInt32 y) const;

	/// Update panel view VCNFloat
	virtual void Update(float elapsedTime);

	/// Render panel view
	virtual void Render() const;

	/// Handles mouse motion events
	virtual void OnMouseMotion(MouseEventArgs& args) override;

	/// Triggered when the left mouse button is pressed
	virtual void OnLeftMouseButtonDown(MouseEventArgs& args) override;

	/// Triggered when the left mouse button is released
	virtual void OnLeftMouseButtonUp(MouseEventArgs& args) override;

protected:

	/// Register web engine client functions
	void RegisterCallback(const std::string& callbackName);

	/// Returns the last known value pixel in render buffer.
	const unsigned int GetPixelValue(int x, int y) const;

	// Shared data
	awe_webview*  mWebView;
	std::string   mURL;

private:

	/// Handles web engine callback handler
	static void OnCallbackHandler(
		awe_webview* caller, 
		const awe_string* object_name, 
		const awe_string* callback_name, 
		const awe_jsarray* arguments);

	/// Handles web engine finish handler.
	static void OnFinishLoadingHandler(awe_webview* caller);

	/// Called when the page is finish loading
	virtual void OnFinishLoading();

	/// Called when a events come from web engine
	virtual void OnCallback(
		const std::string& objectName, 
		const std::string& callbackName, 
		const MenuArgs& args) {}

	/// Create the state block that will be used to draw the web view texture
	void CreateStateBlock();

	/// Creates the vertex buffer that will used to draw the quad/image on screen
	void CreateVertexBuffer();

	/// Create the web view texture
	void CreateTexture();

// Data members

	VCNInt32       mPosX, mPosY;
	VCNUInt32      mWidth, mHeight;
	VCNBool        mViewLoaded;
	VCNStateBlock* mStateBlock;
	VCNTexture*    mTexture;         ///< Texture to store the menu
	VCNCache*      mVertexBuffer;    ///< Vertex buffer for drawing quads
	VCNUInt        mFrameCount;
};

#endif // MENU_H
