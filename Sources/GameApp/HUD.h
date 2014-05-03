///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief  Declares the HUD class. 
///

#ifndef __HUD_H__
#define __HUD_H__

#pragma once

#include "Menu.h"

#include "VCNUtils/Observer.h"

using namespace patterns;

enum HUDActionType
{
   ItemEquipped,
   MouseTaken,
   MouseFreed
};

struct HUDAction {
	HUDActionType actionType;
	std::string actionTarget;
};


class HUD : public Menu, public Subject<HUDAction>
{
public:

  /// Default ctor
  HUD();

  /// Default dtor
  virtual ~HUD();

  void toggleInventory();

  void setCurrentAction(std::string index);

  void showTextMessage(std::string textMessage);

  void displayAwarenessLevel();

  void hideAwarenessLevel();

  void setAwarenessLevel(VCNInt awareness);

// InputController interface

  /// Called when the user release a key.
  virtual const bool OnKeyUp(VCNUInt8 keycode, VCNUInt32 modifiers) override;

  /// Called when the item store changed.
  virtual void OnItemRegistered(const std::string& registeredId, const std::string& readableName, const std::string& imageId);

  /// Called when the inventory changed.
  virtual void OnInventoryChanged(const std::vector<std::string>& items);

  /// Called when the current item was unequipped.
  virtual void OnItemUnequipped(const std::string& removedItem);

  /// Call to change the description shown at the bottom.
  virtual void DisplayActionDescription(const std::string& description, unsigned int timeout = 0);

private:

  /// Called when a view event occurs
  virtual void OnCallback(const std::string& objectName, const std::string& callbackName, const MenuArgs& args) override;

// Data members
};

#endif // __HUD_H__