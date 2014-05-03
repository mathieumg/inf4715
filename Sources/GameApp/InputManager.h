///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

// Project includes
#include "InputKeys.h"

// Engine includes
#include "VCNUtils/Singleton.h"
#include "VCNUtils/Assert.h"
#include "VCNUtils/Types.h"

// System includes
#include <array>

// Forward declarations
class InputController;
struct VCNPoint;

///
/// The input manager tracks keyboard and mouse actions.
///
class InputManager : public Singleton<InputManager>
{
  SINGLETON_DECLARATION( InputManager );

  typedef HWND      WindowHandle;
  typedef void      DisplayHandle; ///< Dummy type for Win32

public:

  InputManager();
  ~InputManager();

// Enums

  enum WindowBorder
  {
      BORDER_NONE      = 0,
      BORDER_TOP       = 1 << 1,
      BORDER_BOTTOM    = 1 << 2,
      BORDER_LEFT      = 1 << 3,
      BORDER_RIGHT     = 1 << 4,
  };

// Attributes

  /// Checks if a key is pressed.
  const bool IsKeyPressed(int keyCode) const;

  /// Checks if a key is released.
  const bool IsKeyReleased(int keyCode) const;

  /// Checks if the manager has the mouse captured.
  const bool IsMouseCaptured(WindowHandle windowHandle = 0L) const;

  /// Checks if the cursor is clipped.
  const bool IsCursorClipped() const { return mClipCursor; }

  /// Returns the cursor position.
  const VCNPoint GetCursorPosition() const;
  void GetCursorPosition(VCNPoint& point) const;
  void GetCursorPosition(VCNLong& x, VCNLong& y) const;

  /// Returns the center cursor x coordinate
  const int GetCursorCenterX() const { VCN_ASSERT(IsMouseCaptured()); return mCx; }

  /// Returns the center cursor y coordinate
  const int GetCursorCenterY() const { VCN_ASSERT(IsMouseCaptured()); return mCy; }

  /// Return which window border(s) the mouse cursor is actually touching.
  const WindowBorder GetMouseBorderPosition() const;

  /// Sets the input window whose manager is tracking input from.
  void SetInputWindow(WindowHandle wnd) { mInputWindow = wnd; }

  /// Sets mouse capture acceptation.
  void SetMouseCaptured(bool val);

  /// Clip the cursor to the game window.
  void SetClipCursor(bool enabled);

  /// Sets the display cursor new position
  void SetCursorPosition(const VCNPoint& point);
  void SetCursorPosition(const VCNLong x, const VCNLong y);

  /// Reset the input manager
  void Reset();

  // Operations

  /// Update input controllers
  void Update(float elapsedTime);

  /// Process mouse movements
  void GetMouseMovements(int& deltaX, int& deltaY);

  /// Register an input controller
  void RegisterController(InputController* inputController);

  /// Unregister an input controller
  void UnregisterController(InputController* inputController);

  /// Resets mouse cursor position to center of window
  void CenterCursor();

  /// Triggered when a Win32 event occurs
  int HandleEvent(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

  /// Handles key press events
  void HandleKeyDown(VCNUInt8 keycode, VCNUInt32 modifiers, bool autoRepeated, VCNUInt16 flags);

  /// Handles key release events
  void HandleKeyUp(VCNUInt8 keycode, VCNUInt32 modifiers);

  /// Handles mouse motion events
  void HandleMouseMotion(VCNInt32 x, VCNInt32 y, VCNInt32 modifiers);

  /// Handle left mouse button press.
  void HandleMouseLeftButtonDown(int x, int y);

  /// Handle left mouse button releases.
  void HandleMouseLeftButtonUp(int x, int y);

  /// Handle right mouse button press.
  void HandleMouseRightButtonDown(int x, int y);

  /// Handle right mouse button releases.
  void HandleMouseRightButtonUp(int x, int y);

  /// Handle middle mouse button press.
  void HandleMouseMiddleButtonDown(int x, int y);

  /// Handle middle mouse button releases.
  void HandleMouseMiddleButtonUp(int x, int y);

  /// Handle key char events.
  void HandleTypedChar(VCNUInt32 nChar);

  /// Handle mouse wheel events.
  void HandleMouseWheel(int xPos, int yPos, int fwKeys, int zDelta);

private:
  
// Implementation

  /// Resets key map
  void ResetKeys();

// Data Members

  typedef std::vector<InputController*> ControllerList;

  int                   mMouseDeltaX;       ///< Mouse delta movement on the x axis
  int                   mMouseDeltaY;       ///< Mouse delta movement on the y axis
  WindowHandle          mInputWindow;       ///< Window receiving input
  bool                  mCaptureMouse;      ///< Indicates if the manager is processing input
  ControllerList        mInputControllers;  ///< Registered input controllers
  VCNUInt32             mCx, mCy;           ///< Center coordinates when cursor is set at center of screen
  std::array<bool, 256> mKeys;              ///< Array Used For The Keyboard Routine
  bool                  mClipCursor;        ///< Remember to clip cursor or not
  InputController*      mMouseDownStarter;  ///< Tracks the controller who started the mouse down event.
};

//
// INLINES
// 

////////////////////////////////////////////////////////////////////////
///
/// Checks if a given key is currently pressed.
/// 
/// @param keyCode   [IN] key to check
///
/// @return true if the key is pressed
///
inline const bool InputManager::IsKeyPressed(int keyCode) const
{
  return mKeys[keyCode];
}

////////////////////////////////////////////////////////////////////////
///
/// Checks if the key is released.
/// 
/// @param keyCode [IN] key code to check
///
/// @return true if the key state is released
///
inline const bool InputManager::IsKeyReleased(int keyCode) const
{
  return !mKeys[keyCode];
}

#endif /* INPUTMANAGER_H */
