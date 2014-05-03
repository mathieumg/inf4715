///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief InputController Interface prototype
///

#ifndef inputcontroller_h__
#define inputcontroller_h__

#pragma once

#include "VCNUtils/Types.h"

// TODO: Move to InputEvents.h
struct EventArgs
{
  EventArgs()
    : handled(false)
  {
  }

  bool handled;
};

struct MouseEventArgs : public ::EventArgs
{
  MouseEventArgs(const int x = std::numeric_limits<int>::lowest(), const int y = std::numeric_limits<int>::lowest())
    : x(x)
    , y(y)
    , modifiers(0)
  {
  }

  int x;
  int y;
  int modifiers;
};

// TODO: Create IInputController

// TODO: Make InputController an adapter class
class InputController
{

public:

  // TODO: change other events to use event args.

  /// Update the controller.
  virtual void Update(float elapsedTime) {};

  /// Handles key press events
  virtual void OnKeyDown(VCNUInt8 keycode, VCNUInt32 modifiers) {}

  /// Handles key release events
  virtual const bool OnKeyUp(VCNUInt8 keycode, VCNUInt32 modifiers) { return false; }

  /// Handles mouse motion events
  virtual void OnMouseMotion(MouseEventArgs& args) {}

  /// Handle left mouse button press.
  virtual void OnLeftMouseButtonDown(MouseEventArgs& args) {}

  /// Handle left mouse button releases.
  virtual void OnLeftMouseButtonUp(MouseEventArgs& args) {}

  /// Handle right mouse button press.
  virtual void OnRightMouseButtonDown(MouseEventArgs& args) {}

  /// Handle right mouse button releases.
  virtual void OnRightMouseButtonUp(MouseEventArgs& args) {}

  /// Handle middle mouse button press.
  virtual const bool OnMiddleMouseButtonDown(int x, int y) { return false; }

  /// Handle middle mouse button releases.
  virtual const bool OnMiddleMouseButtonUp(int x, int y) { return false; }

  /// Handle key char events.
  virtual void OnChar(UINT nChar) {}

  /// Handle mouse wheel events.
  virtual void OnMouseWheel(int xPos, int yPos, int fwKeys, short zDelta) {}

protected: // Abstract class

  InputController(void);
  virtual ~InputController(void);

};

#endif // inputcontroller_h__
