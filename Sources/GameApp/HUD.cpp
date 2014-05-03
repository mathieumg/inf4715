///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#include "Precompiled.h"
#include "HUD.h"
#include "AweHandle.h"
#include "PlayState.h"
#include "StateMachine.h"

//////////////////////////////////////////////////////////////////////////
HUD::HUD()
  : Menu("HUD.html")
{
    RegisterCallback("itemEquippedCallback");
	RegisterCallback("takeMouseCallback");
	RegisterCallback("freeMouseCallback");
}

//////////////////////////////////////////////////////////////////////////
HUD::~HUD()
{
}

void HUD::toggleInventory()
{
    // Call a javascript function to toggle the inventory.
    awe_webview_call_javascript_function(
        mWebView, awe_string_empty(), AweString("toggleInventory"),
        AweValueArray(""), awe_string_empty());
}

void HUD::setCurrentAction(std::string index)
{
    // Call a javascript function to set the current action.
    awe_webview_call_javascript_function(
        mWebView, awe_string_empty(), AweString("setCurrentAction"),
        AweValueArray(index.c_str()), awe_string_empty());
}

void HUD::showTextMessage(std::string textMessage)
{
    // Call a javascript function to set the current action.
    awe_webview_call_javascript_function(
        mWebView, awe_string_empty(), AweString("showTextMessage"),
        AweValueArray(textMessage.c_str()), awe_string_empty());
}

void HUD::displayAwarenessLevel()
{
	// Call a javascript function to show the awareness level.
    awe_webview_call_javascript_function(
        mWebView, awe_string_empty(), AweString("showAwarenessLevel"),
        AweValueArray(""), awe_string_empty());
}

void HUD::hideAwarenessLevel()
{
	// Call a javascript function to show the awareness level.
    awe_webview_call_javascript_function(
        mWebView, awe_string_empty(), AweString("hideAwarenessLevel"),
        AweValueArray(""), awe_string_empty());
}

void HUD::setAwarenessLevel(VCNInt awareness)
{
    // Call a javascript function to set the current awareness level.
    awe_webview_call_javascript_function(
        mWebView, awe_string_empty(), AweString("setAwarenessLevel"),
        AweValueArray(VCN_W2A(StringUtils::NumberToString(awareness, 10))), awe_string_empty());
}

void HUD::OnItemRegistered(const std::string& registeredId, const std::string& readableName, const std::string& imageId)
{
	std::vector<AweValue> registeredItem;
    
    registeredItem.push_back(registeredId.c_str());
    registeredItem.push_back(readableName.c_str());
    registeredItem.push_back(imageId.c_str());

	// Call a javascript function to update the web view with the new item list.
    awe_webview_call_javascript_function( 
      mWebView, awe_string_empty(), AweString( "registerItem" ), 
      AweValueArray( registeredItem ), awe_string_empty());
}

void HUD::OnInventoryChanged(const std::vector<std::string>& items)
{
	// Query inventory to get complete list of items.
	std::vector<AweValue> itemIds;
	for(auto it = items.begin(); it != items.end(); it++)
	{
		itemIds.push_back((*it).c_str());
	}

	// Call a javascript function to update the web view with the new item list.
    awe_webview_call_javascript_function( 
      mWebView, awe_string_empty(), AweString( "itemList" ), 
      AweValueArray( itemIds ), awe_string_empty() );
}

void HUD::OnItemUnequipped(const std::string& removedItem)
{
	// Call a javascript function to update the web view without the equipped item.
    awe_webview_call_javascript_function( 
      mWebView, awe_string_empty(), AweString( "unequip" ), 
      AweValueArray( removedItem.c_str() ), awe_string_empty() );
}

void HUD::DisplayActionDescription(const std::string& description, unsigned int timeout)
{
	std::vector<AweValue> args;
	args.push_back(description.c_str());
    if (timeout != 0)
    {
        //FIXME: Need to either enable sending ints to JS, or need to cast this timeout to a string... (Ew.)
        VCNString arg = StringUtils::NumberToString(timeout, 10);
        args.push_back(VCN_W2A(arg));
    }
	
	awe_webview_call_javascript_function( 
      mWebView, awe_string_empty(), AweString( "displayActionDescription" ), 
      AweValueArray( args ), awe_string_empty() );
}

//////////////////////////////////////////////////////////////////////////
void HUD::OnCallback(const std::string& objectName, const std::string& callbackName, const MenuArgs& args)
{
    if (callbackName == "itemEquippedCallback")
    {
        const std::string& itemId = args.front().Cast<std::string>();
		HUDAction data;
		data.actionType = ItemEquipped;
		data.actionTarget = itemId;
		Notify(data);
    } else if (callbackName == "takeMouseCallback")
	{
		HUDAction data;
		data.actionType = MouseTaken;
		Notify(data);
	} else if (callbackName == "freeMouseCallback")
	{
		HUDAction data;
		data.actionType = MouseFreed;
		Notify(data);
	}
}

///////////////////////////////////////////////////////////////////////
const bool HUD::OnKeyUp(VCNUInt8 keycode, VCNUInt32 modifiers)
{
  /*
  if ( keycode == 'I' )
  {


    return true;
  }
  */

  return InputController::OnKeyUp( keycode, modifiers );
}
