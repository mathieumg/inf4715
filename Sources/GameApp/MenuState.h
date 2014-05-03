///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief  Declares the menu state class. 
///

#ifndef __MENUSTATE_H__
#define __MENUSTATE_H__

#include "GameState.h"
#include "MainMenu.h"

class MenuState : public GameState  
{

public:

  MenuState();
  ~MenuState();

  /// Initialize the main menu state
  VCNBool Initialize();

  /// Release resources used by the main menu state
  VCNBool Uninitialize();

  /// Renders the main menu
  void Render() const;

  /// Updates the menu
  void Update(float elapsedTime);

  /// Manage the state transitions
  void OnLeave();
  void OnEnter();

private:

  Menu* mMenu;
};

#endif /* __MENUSTATE_H__ */
