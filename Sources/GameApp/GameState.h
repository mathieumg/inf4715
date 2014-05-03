///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief  Declares the game state abstract class. 
///

#ifndef __GAMESTATE_H__
#define __GAMESTATE_H__

#include "InputController.h"

class StateMachine;

class GameState
{
public:

  /// Default dtor
  virtual ~GameState() = 0 {}

protected:

  friend class StateMachine;
  
  /// Default ctor
  GameState() {}

  /// Initialize the state
  virtual VCNBool Initialize() = 0;

  /// Uninitialize the state
  virtual VCNBool Uninitialize() = 0;

  /// Update state
  virtual void Update(float elapsedTime) = 0;

  /// Pre render the current frame (pre effects)
  virtual void PreRender() const {};

  /// Render state
  virtual void Render() const = 0;

  /// Manage the state transitions
  virtual void OnLeave() = 0;
  virtual void OnEnter() = 0;

  /// Called when the user press a key
  virtual const bool OnKeyPress(VCNUInt8 keycode) { return false; }

  /// Called when the user move the mouse
  virtual void OnMouseMotion(MouseEventArgs& args) {}

  /// Called when the user press the left mouse button
  virtual void OnLeftMouseButtonDown(MouseEventArgs& args) {}

  /// Called when the user release the left mouse button
  virtual void OnLeftMouseButtonUp(MouseEventArgs& args) {}
  
  /// Called when the user press the right mouse button
  virtual void OnRightMouseButtonDown(MouseEventArgs& args) {}

  /// Called when the user release the right mouse button
  virtual void OnRightMouseButtonUp(MouseEventArgs& args) {}

  /// Triggers a change state
  void ChangeState( int nextState );
};

#endif /* __GAMESTATE_H__ */
