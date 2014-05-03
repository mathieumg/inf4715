///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#include "Precompiled.h"
#include "InputManager.h"

#include "InputController.h"

#include "VCNUtils/Macros.h"
#include "VCNUtils/RenderTypes.h"

#include <algorithm>

////////////////////////////////////////////////////////////////////////
///
/// Constructs input manager
///
/// @return Nothing
///
////////////////////////////////////////////////////////////////////////
InputManager::InputManager(void)
: mMouseDeltaX(0)
, mMouseDeltaY(0)
, mInputWindow(0)
, mCaptureMouse(false)
, mCx((VCNUInt32)-1)
, mCy((VCNUInt32)-1)
, mClipCursor(false)
, mMouseDownStarter(0)
{
  ResetKeys();
  mInputControllers.clear();
}

////////////////////////////////////////////////////////////////////////
///
/// Input manager destructor
///
/// @return Nothing
///
////////////////////////////////////////////////////////////////////////
InputManager::~InputManager(void)
{
  ::ClipCursor(NULL);

  VCN_ASSERT_MSG( mInputControllers.empty(), 
    TEXT("Not all (%d) input controllers have been unregistered"), mInputControllers.size() );
}



////////////////////////////////////////////////////////////////////////
///
/// Reset the key map. Will map all key as released.
///
/// @return Nothing
///
////////////////////////////////////////////////////////////////////////
void InputManager::ResetKeys()
{
  for(int i = 0; i < 256; i++)
    mKeys[i] = false;
}



////////////////////////////////////////////////////////////////////////
///
/// Process and returns mouse movements. The cursor is set back to the
/// middle of the window for next movement.
///
/// @param[out] deltaX: Returns the delta movement on the x axis.
/// @param[out] deltaY: Returns the delta movement on the y axis
///
/// @return
///
////////////////////////////////////////////////////////////////////////
void InputManager::GetMouseMovements(int& deltaX, int& deltaY)
{
  // get cursor absolute position
  POINT CursorPos;
  ::GetCursorPos(&CursorPos);

  // compute center of screen
  POINT CenterPt;   
  RECT ClientRect;
  ::GetClientRect(mInputWindow, &ClientRect);

  int XOffset = (ClientRect.right - ClientRect.left) / 2;
  int YOffset = (ClientRect.bottom - ClientRect.top) / 2;
  CenterPt.x = ClientRect.left+XOffset;
  CenterPt.y = ClientRect.top+YOffset;

  // reset cursor position to center of screen.
  ::ClientToScreen(mInputWindow, &CenterPt);

  // compute moving deltas
  deltaX = CenterPt.x - CursorPos.x;
  deltaY = CenterPt.y - CursorPos.y;

  if (deltaX != 0 || deltaY != 0)
  {
    ::SetCursorPos(CenterPt.x, CenterPt.y);
  }
}

////////////////////////////////////////////////////////////////////////
///
/// @return true if the manager is accepting input command, otherwise false
///         is returned.
///
const bool InputManager::IsMouseCaptured(WindowHandle windowHandle /*= 0L*/) const
{
  return mCaptureMouse;
}

////////////////////////////////////////////////////////////////////////
///
/// Sets acceptation of input. If the manager directly accepts input, the
/// cursor's visibility state is managed by him.
///
/// @param[in] val: input acceptation
///
/// @return Nothing
///
void InputManager::SetMouseCaptured(bool val)
{
  mCaptureMouse = val;

  if (mCaptureMouse) 
  {
    while( ShowCursor(FALSE) >= 0 )  // Hide cursor
      ;

    CenterCursor();
  }
  else
  {
    while( ShowCursor(TRUE) < 0 )     // Show cursor
      ;
  }
}



////////////////////////////////////////////////////////////////////////
///
/// Centers the cursor in the middle of the input window.
///
/// @return Nothing
///
////////////////////////////////////////////////////////////////////////
void InputManager::CenterCursor()
{
  POINT CenterPt;   
  RECT ClientRect;
  ::GetClientRect(mInputWindow, &ClientRect);

  int XOffset = (ClientRect.right - ClientRect.left) / 2;
  int YOffset = (ClientRect.bottom - ClientRect.top) / 2;
  CenterPt.x = ClientRect.left+XOffset;
  CenterPt.y = ClientRect.top+YOffset;

  // reset cursor position to center of screen.
  ::ClientToScreen(mInputWindow, &CenterPt);

  // Cache center information
  mCx = CenterPt.x;
  mCy = CenterPt.y;

  // Finally set the cursor position
  ::SetCursorPos(CenterPt.x, CenterPt.y);
}


////////////////////////////////////////////////////////////////////////
///
/// Clips the cursor to the game window.
///
/// @return Nothing
///
////////////////////////////////////////////////////////////////////////
void InputManager::SetClipCursor(bool enable)
{
    RECT ClientRect;
    
    ::GetWindowRect(mInputWindow, &ClientRect);    
    if (enable)
        ::ClipCursor(&ClientRect);
    else
        ::ClipCursor(NULL);

    mClipCursor = enable;
}

////////////////////////////////////////////////////////////////////////
///
/// Return which window border(s) the mouse cursor is actually touching.
///
/// @return Nothing
///
const InputManager::WindowBorder InputManager::GetMouseBorderPosition() const
{
    WindowBorder border = BORDER_NONE;
    POINT CursorPt;  
    RECT ClientRect;

    ::GetWindowRect(mInputWindow, &ClientRect);
    ::GetCursorPos(&CursorPt);

    if (CursorPt.x == ClientRect.left) border = static_cast<WindowBorder>(border | BORDER_LEFT);
    if (CursorPt.x > ClientRect.right - 2) border = static_cast<WindowBorder>(border | BORDER_RIGHT);
    if (CursorPt.y == ClientRect.top) border = static_cast<WindowBorder>(border | BORDER_TOP);
    if (CursorPt.y > ClientRect.bottom - 2) border = static_cast<WindowBorder>(border | BORDER_BOTTOM);

    return border;
}

////////////////////////////////////////////////////////////////////////
///
/// Update registered input controllers.
///
/// @param[in] elapsedTime: Elapsed time between calls
///
/// @return Nothing
///
void InputManager::Update(float elapsedTime)
{
  // Update all controllers
  std::for_each(mInputControllers.begin(), mInputControllers.end(), [=](ControllerList::value_type inputController) 
  {
    inputController->Update(elapsedTime);
  });
}

////////////////////////////////////////////////////////////////////////
///
/// Registers an input controller.
///
/// @param[in] inputController: the input controller to register.
///
/// @return nothing
///
void InputManager::RegisterController(InputController* inputController)
{
  ASSERT(inputController != 0 && "input controller is invalid");

  mInputControllers.insert( mInputControllers.begin(), inputController );
}

////////////////////////////////////////////////////////////////////////
///
/// Unregister an input controller. The input controller is removed from the
/// list.
///
/// @param[in] inputController: the input controller to be removed
///
/// @return nothing
///
void InputManager::UnregisterController(InputController* inputController)
{
  ControllerList::iterator foundItr = std::find(
    mInputControllers.begin(),
    mInputControllers.end(),
    inputController);
  if (foundItr != mInputControllers.end())
  {
    mInputControllers.erase(foundItr);
  }
}

////////////////////////////////////////////////////////////////////////
///
/// Resets keys inputs and mouse capture information.
///
void InputManager::Reset()
{
  ResetKeys();
  mCaptureMouse = false;
}

////////////////////////////////////////////////////////////////////////
///
/// Handles user events.
/// 
/// @param hWnd   [IN] window handle
/// @param msg    [IN] message id
/// @param wParam [IN] parameters
/// @param lParam [IN] parameters
///
/// @return 0 if processed
///
int InputManager::HandleEvent( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
  // Extract mouse coordinates as they are used in many cases.
  const int x = GET_X_LPARAM(lParam);
  const int y = GET_Y_LPARAM(lParam);

  switch(msg)
  {
  case WM_KEYDOWN:     
    HandleKeyDown((UINT)wParam, 0, (lParam & 0x40000000) != 0, (VCNUInt16)(lParam >> 16));  
    return 0;

  case WM_KEYUP:       
    HandleKeyUp((UINT)wParam, 0);                                                           
    return 0;

  case WM_CHAR:        
    HandleTypedChar( static_cast<VCNUInt32>(wParam) );                                      
    return 0;

  case WM_LBUTTONDOWN: 
    HandleMouseLeftButtonDown(x, y);                                                        
    return 0;

  case WM_LBUTTONUP:   
    HandleMouseLeftButtonUp(x, y);                                                          
    return 0;

  case WM_RBUTTONDOWN: 
    HandleMouseRightButtonDown(x, y);                                                       
    return 0;

  case WM_RBUTTONUP:   
    HandleMouseRightButtonUp(x, y);                                                         
    return 0;

  case WM_MBUTTONDOWN: 
    HandleMouseMiddleButtonDown(x, y);                                                      
    return 0;

  case WM_MBUTTONUP:   
    HandleMouseMiddleButtonUp(x, y);                                                        
    return 0;

  case WM_MOUSEMOVE:   
    HandleMouseMotion(x, y, wParam);                                                        
    return 0;

  case WM_MOUSEWHEEL:  
    HandleMouseWheel(x, y, (int)LOWORD(wParam), (short)HIWORD(wParam));                     
    return 0;

  default: 
    break;
  }

  return 0;
}

/// Handles key press events
void InputManager::HandleKeyDown(VCNUInt8 keycode, VCNUInt32 modifiers, bool autoRepeated, VCNUInt16 flags)
{
  mKeys[keycode] = true;

  for (ControllerList::iterator itr = mInputControllers.begin(), end = mInputControllers.end(); itr != end; ++itr)
  {
    (*itr)->OnKeyDown(keycode, modifiers);
  }
}

/// Handles key release events
void InputManager::HandleKeyUp(VCNUInt8 keycode, VCNUInt32 modifiers)
{
  mKeys[keycode] = false;

  for (ControllerList::iterator itr = mInputControllers.begin(), end = mInputControllers.end(); itr != end; ++itr)
  {
    if ((*itr)->OnKeyUp(keycode, modifiers))
      return;
  }
}

/// Handles mouse motion events
void InputManager::HandleMouseMotion(VCNInt32 x, VCNInt32 y, VCNInt32 modifiers)
{
  MouseEventArgs args(x,y);
  args.modifiers = modifiers;

  for (ControllerList::iterator itr = mInputControllers.begin(), end = mInputControllers.end(); itr != end; ++itr)
  {
    (*itr)->OnMouseMotion(args);
  }
}

/// Handle left mouse button press.
void InputManager::HandleMouseLeftButtonDown(int x, int y)
{
  MouseEventArgs args(x,y);

  for (ControllerList::iterator it = mInputControllers.begin(), end = mInputControllers.end(); it != end && !args.handled; ++it)
  {
    (*it)->OnLeftMouseButtonDown(args);

    if ( args.handled )
    {
      mMouseDownStarter = *it;
    }
  }
}

/// Handle left mouse button releases.
void InputManager::HandleMouseLeftButtonUp(int x, int y)
{
  MouseEventArgs args(x,y);

  // Mouse down starter has presence over everyone else.
  if ( std::find(mInputControllers.begin(), mInputControllers.end(), mMouseDownStarter) != mInputControllers.end() )
  {
    mMouseDownStarter->OnLeftMouseButtonUp( args );
  }

  for (ControllerList::iterator it = mInputControllers.begin(), end = mInputControllers.end(); it != end && !args.handled; ++it)
  {
    if ( mMouseDownStarter != *it )
    {
      (*it)->OnLeftMouseButtonUp(args);
    }
  }

  mMouseDownStarter = 0;
}

/// Handle right mouse button press.
void InputManager::HandleMouseRightButtonDown(int x, int y)
{
  MouseEventArgs args(x,y);

  for (ControllerList::iterator it = mInputControllers.begin(), end = mInputControllers.end(); it != end && !args.handled; ++it)
  {
    (*it)->OnRightMouseButtonDown(args);

    if ( args.handled )
    {
      mMouseDownStarter = *it;
    }
  }
}

/// Handle right mouse button releases.
void InputManager::HandleMouseRightButtonUp(int x, int y)
{
  MouseEventArgs args(x,y);

  // Mouse down starter has presence over everyone else.
  if ( std::find(mInputControllers.begin(), mInputControllers.end(), mMouseDownStarter) != mInputControllers.end() )
  {
    mMouseDownStarter->OnRightMouseButtonUp( args );
  }

  for (ControllerList::iterator it = mInputControllers.begin(), end = mInputControllers.end(); it != end && !args.handled; ++it)
  {
    if ( mMouseDownStarter != *it )
    {
      (*it)->OnRightMouseButtonUp(args);
    }
  }
}

/// Handle middle mouse button press.
void InputManager::HandleMouseMiddleButtonDown(int x, int y)
{
  for (ControllerList::iterator itr = mInputControllers.begin(), end = mInputControllers.end(); itr != end; ++itr)
  {
    if ( (*itr)->OnMiddleMouseButtonDown(x, y) )
      return;
  }
}

/// Handle middle mouse button releases.
void InputManager::HandleMouseMiddleButtonUp(int x, int y)
{
  for (ControllerList::iterator itr = mInputControllers.begin(), end = mInputControllers.end(); itr != end; ++itr)
  {
    if ( (*itr)->OnMiddleMouseButtonUp(x, y) )
      return;
  }
}

/// Handle key char events.
void InputManager::HandleTypedChar(VCNUInt32 nChar)
{
  for (ControllerList::iterator itr = mInputControllers.begin(), end = mInputControllers.end(); itr != end; ++itr)
  {
    (*itr)->OnChar(nChar);
  }
}

/// Handle mouse wheel events.
void InputManager::HandleMouseWheel(int xPos, int yPos, int fwKeys, int zDelta)
{
  for (ControllerList::iterator itr = mInputControllers.begin(), end = mInputControllers.end(); itr != end; ++itr)
  {
    (*itr)->OnMouseWheel(xPos, yPos, fwKeys, zDelta);
  }
}

///////////////////////////////////////////////////////////////////////
const VCNPoint InputManager::GetCursorPosition() const
{
  VCNPoint cursorPosition;
  ::GetCursorPos( reinterpret_cast<LPPOINT>( &cursorPosition ) );
  return cursorPosition;
}

///////////////////////////////////////////////////////////////////////
void InputManager::GetCursorPosition(VCNPoint& point) const
{
  ::GetCursorPos( reinterpret_cast<LPPOINT>( &point ) );
}

///////////////////////////////////////////////////////////////////////
void InputManager::GetCursorPosition(VCNLong& x, VCNLong& y) const
{
  const VCNPoint& point = GetCursorPosition();
  x = point.x;
  y = point.y;
}

///////////////////////////////////////////////////////////////////////
void InputManager::SetCursorPosition(const VCNPoint& point)
{
  SetCursorPosition( point.x, point.y );
}

///////////////////////////////////////////////////////////////////////
void InputManager::SetCursorPosition(const VCNLong x, const VCNLong y)
{
  ::SetCursorPos( x, y );
}
