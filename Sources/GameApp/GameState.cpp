///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief  Implements the game state class. 
///

#include "Precompiled.h"
#include "GameState.h"
#include "StateMachine.h"

//////////////////////////////////////////////////////////////////////////
void GameState::ChangeState( int nextState )
{
  StateMachine::GetInstance().ChangeState( static_cast<GameStateID>( nextState ) );
}
