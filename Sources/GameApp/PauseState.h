///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Declares the pause state class. 
///

#ifndef __PAUSESTATE_H__
#define __PAUSESTATE_H__

#include "GameState.h"

class PauseState : public GameState  
{

public:

  PauseState();
  ~PauseState();

  void Render();

  /// Manage the state transitions
  void OnLeave();
  void OnEnter();

private:

};

#endif /* __PAUSESTATE_H__ */
