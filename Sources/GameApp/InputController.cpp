///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Implements the input controller class. 
///

#include "Precompiled.h"
#include "InputController.h"

#include "InputManager.h"

InputController::InputController(void)
{
   InputManager::GetInstance().RegisterController(this);
}

InputController::~InputController(void)
{
   InputManager::GetInstance().UnregisterController(this);
}
