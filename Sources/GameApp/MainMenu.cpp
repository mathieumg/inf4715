///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief  Implements the main menu class. 
///

#include "Precompiled.h"
#include "MainMenu.h"

#include "Game.h"
#include "PlayState.h"
#include "StateMachine.h"

//////////////////////////////////////////////////////////////////////////
MainMenu::MainMenu(const char* url)
: Menu(url)
{
  RegisterCallback( "newGameCallback" );
  RegisterCallback( "creditsCallback" );
  RegisterCallback( "quitCallback" );
  RegisterCallback( "controllerCallback" );
}


//////////////////////////////////////////////////////////////////////////
MainMenu::~MainMenu()
{
}


//////////////////////////////////////////////////////////////////////////
void MainMenu::OnCallback(const std::string& objectName, const std::string& callbackName, const MenuArgs& args)
{
  // Execute button actions
  if( callbackName == "newGameCallback" )
  {
    PlayState* playState = StateMachine::GetInstance().GetState<PlayState>(GS_PLAY);
    playState->NewGame();

    StateMachine::GetInstance().ChangeState( GS_PLAY );
  }
  else if( callbackName == "quitCallback" )
  {
    // Set a flag to end game processing and quit the application cleanly
    Game::GetInstance().Quit();
  }
}
