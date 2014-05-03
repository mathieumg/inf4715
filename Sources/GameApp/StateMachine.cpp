///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Implements the state machine class that manages the state changes. 
///

#include "Precompiled.h"
#include "StateMachine.h"

// States includes
#include "MenuState.h"
#include "PauseState.h"
#include "PlayState.h"

#include "VCNUtils/Macros.h"
#include "VCNUtils/Assert.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

StateMachine::StateMachine()
: mCurrentState(0)
, mCurrentStateEnum(GS_NONE)
{
  // Create states
  mStates[ GS_MAIN_MENU ] = new MenuState();
  mStates[ GS_PLAY ] = new PlayState();
  
  VCN_ASSERT_MSG( mStates.size() == GS_STATE_COUNT, VCNTXT("Maybe you forgot to instantiate all your game state?") );
}

StateMachine::~StateMachine()
{
  for (GameStateIterator itr = mStates.begin(), end = mStates.end(); itr != end; ++itr)
  {
    delete itr->second;
  }
}

////////////////////////////////////////////////////////////////////////
///
/// ChangeState
///
/// @param[in] GameState *mState
///
/// @return Nothing
///
////////////////////////////////////////////////////////////////////////
void StateMachine::ChangeState(GameStateID newState)
{
  GameState* nextState = mStates[ newState ];

  if ( nextState != mCurrentState )
  {
    // We leave the old state
    if (mCurrentState)
    {
      mCurrentState->OnLeave();
    }

    // We change the game state
    mCurrentState = nextState;
    mCurrentStateEnum = newState;
  }

  // We enter the new state
  mCurrentState->OnEnter();
}

//////////////////////////////////////////////////////////////////////////
VCNBool StateMachine::Initialize(const GameStateID firstState)
{
  // Initialize child states
  for (GameStateIterator itr = mStates.begin(), end = mStates.end(); itr != end; ++itr)
  {
    itr->second->Initialize();
  }

  // Set the first state
  ChangeState( firstState );
  VCN_ASSERT( mCurrentState );

  return true;
}

//////////////////////////////////////////////////////////////////////////
VCNBool StateMachine::Uninitialize()
{
  // Release states
  for (GameStateIterator itr = mStates.begin(), end = mStates.end(); itr != end; ++itr)
  {
    itr->second->Uninitialize();
  }

  return true;
}


///////////////////////////////////////////////////////////////////////
void StateMachine::PreRender() const
{
  VCN_ASSERT( mCurrentState );

  mCurrentState->PreRender();
}

//////////////////////////////////////////////////////////////////////////
void StateMachine::Render() const
{
  VCN_ASSERT( mCurrentState );

  mCurrentState->Render();
}

//////////////////////////////////////////////////////////////////////////
void StateMachine::Update( float elapsedTime )
{
  VCN_ASSERT( mCurrentState );

  mCurrentState->Update( elapsedTime );
}

//////////////////////////////////////////////////////////////////////////
const VCNBool StateMachine::IsStateActive( const GameStateID state ) const
{
  return mCurrentStateEnum == state;
}

//////////////////////////////////////////////////////////////////////////
const bool StateMachine::OnKeyUp( VCNUInt8 keycode, VCNUInt32 modifiers )
{
  return mCurrentState->OnKeyPress( keycode );
}

//////////////////////////////////////////////////////////////////////////
void StateMachine::OnMouseMotion(MouseEventArgs& args) 
{
  mCurrentState->OnMouseMotion(args);
}

//////////////////////////////////////////////////////////////////////////
void StateMachine::OnLeftMouseButtonDown(MouseEventArgs& args)
{
  mCurrentState->OnLeftMouseButtonDown(args);
}

//////////////////////////////////////////////////////////////////////////
void StateMachine::OnLeftMouseButtonUp(MouseEventArgs& args)
{
  mCurrentState->OnLeftMouseButtonUp(args);
}

//////////////////////////////////////////////////////////////////////////
void StateMachine::OnRightMouseButtonDown(MouseEventArgs& args)
{
  mCurrentState->OnRightMouseButtonDown( args );
}

//////////////////////////////////////////////////////////////////////////
void StateMachine::OnRightMouseButtonUp(MouseEventArgs& args)
{
  mCurrentState->OnRightMouseButtonUp( args );
}

//////////////////////////////////////////////////////////////////////////
const bool StateMachine::OnMiddleMouseButtonDown( int x, int y )
{
  return false;
}

//////////////////////////////////////////////////////////////////////////
const bool StateMachine::OnMiddleMouseButtonUp( int x, int y )
{
  return false;
}
