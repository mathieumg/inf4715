#ifndef STATEMACHINE_H
#define STATEMACHINE_H

///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Declares the state machine class. 
///

#pragma once

#include "GameState.h"

// Engine includes
#include "VCNUtils/Singleton.h"

enum GameStateID
{
  GS_NONE = -1,
  GS_MAIN_MENU,
  GS_PLAY,
  
  GS_STATE_COUNT
};

class StateMachine 
  : public Singleton<StateMachine>
  , public InputController
{
  SINGLETON_DECLARATION( StateMachine );

public:

  /// Default ctor
  StateMachine();

  /// Default dtor
  virtual ~StateMachine();

  /// Initialize the state machine
  VCNBool Initialize(const GameStateID firstState);

  /// Uninitialize the state machine
  VCNBool Uninitialize();

  /// Compares the current state
  const VCNBool IsStateActive(const GameStateID state) const;

  /// Request a state change
  void ChangeState(GameStateID newState);

  /// Return a state pointer
  template<typename T> T* GetState(GameStateID state) const;

  /// Get the current state
  GameState* GetCurrentState() const;

  void PreRender() const;

  /// Render states
  void Render() const;

  /// Update states
  void Update(float elapsedTime);

protected:

// InputController interface

  /// Handles key release events
  virtual const bool OnKeyUp(VCNUInt8 keycode, VCNUInt32 modifiers) override;

  /// Handles mouse motion events
  virtual void OnMouseMotion(MouseEventArgs& args) override;

  /// Handle left mouse button press.
  virtual void OnLeftMouseButtonDown(MouseEventArgs& args) override;

  /// Handle left mouse button releases.
  virtual void OnLeftMouseButtonUp(MouseEventArgs& args) override;

  /// Handle right mouse button press.
  virtual void OnRightMouseButtonDown(MouseEventArgs& args) override;

  /// Handle right mouse button releases.
  virtual void OnRightMouseButtonUp(MouseEventArgs& args) override;

  /// Handle middle mouse button press.
  virtual const bool OnMiddleMouseButtonDown(int x, int y) override;

  /// Handle middle mouse button releases.
  virtual const bool OnMiddleMouseButtonUp(int x, int y) override;

private:

  typedef std::map<GameStateID, GameState*> GameStateList;
  typedef GameStateList::iterator           GameStateIterator;
  
  GameState*    mCurrentState;
  GameStateID   mCurrentStateEnum;  
  GameStateList mStates;
};

//////////////////////////////////////////////////////////////////////////
inline GameState* StateMachine::GetCurrentState() const
{
  return mCurrentState;
}

//////////////////////////////////////////////////////////////////////////
template<typename T> inline T* StateMachine::GetState(GameStateID state) const
{
  return static_cast<T*>( mStates.find(state)->second );
}

#endif // STATEMACHINE_H
