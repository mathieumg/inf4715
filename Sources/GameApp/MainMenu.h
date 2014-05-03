///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Declares the main menu class. 
///

#ifndef __MAINMENU_H__
#define __MAINMENU_H__

#include "Menu.h"

// This class serves as a GUI panel to group GUI elements
class MainMenu : public Menu
{
public:

  /// Default ctor
  MainMenu(const char* url);

  /// Default dtor
  virtual ~MainMenu();

private:

  /// Called when a view event occurs
  virtual void OnCallback(const std::string& objectName, const std::string& callbackName, const MenuArgs& args) override;
};

#endif // __MAINMENU_H__