///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Interface of the gameplay controller.
///

#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H

#include "InputController.h"

#include "VCNUtils/RenderTypes.h"
#include "VCNUtils/Vector.h"
#include "VCNUtils/Filters.h"
#include "VCNPhysx/PhysxController.h"
#include "characterkinematic/PxController.h"
#include "PxScene.h"
#include "VCNNodes/Node.h"
#include "VCNNodes/NodeCore.h"
#include "VCNUtils/Observer.h"

class Camera;
class PhysXRaycast;

enum CameraMovingState
{
    Idle,
    Walking,
    Running
};

enum CameraEventType
{
    Moving,
    MovingStop,
    PointingObject,
    RunningStart,
    WalkingStart,
    Jumping
};

struct CameraData {
    CameraEventType eventType;
    Vector3 newPosition;
    Vector3 newLookAt;
    VCNNode* selected;
};

//////////////////////////////////////////////////////////////////////////
///
///  @class PlayerController
///
///  @brief The player controller respond to the player input to control
///         gameplay.
///
//////////////////////////////////////////////////////////////////////////
class CameraController : public InputController, public patterns::Subject<CameraData>
{
public:

  /// Construct the gameplay controller.
  explicit CameraController(Camera& camera);

  /// Destruct the controller
  virtual ~CameraController(void);

  /// Update the view in time (Reading user inputs).
  void Update(float elapsedTime);

  /// When space bar is pressed we capture the mouse to enter the rotation mode
  void EnterFreeLook();

  /// When space bar is released we exit the rotation mode and
  /// reset mouse cursor to its previous position.
  void LeaveFreeLook();

  VCNBool IsFreeLook();

  void teleport(double posX, double posY, double posZ);

  void setMobility(VCNBool enabled);

private:

  /// Handles mouse motion events
  virtual void OnMouseMotion(MouseEventArgs& args) override;

  /// Handles key pressed events
  virtual void OnKeyDown(VCNUInt8 keycode, VCNUInt32 modifiers) override;

  /// Handles key release events
  virtual const bool OnKeyUp(VCNUInt8 keycode, VCNUInt32 modifiers) override;

  /// Capture the mouse for free look.
  virtual void OnRightMouseButtonDown(MouseEventArgs& args) override;

  /// Releases the mouse from free look.
  virtual void OnRightMouseButtonUp(MouseEventArgs& args) override;

  /// Handle mouse wheel events.
  virtual void OnMouseWheel(int xPos, int yPos, int fwKeys, short zDelta) override;

  /// Updates the player movement
  void ReadInputs(const VCNFloat elapsedTime);

  /// Adjust the camera settings based on previous movement
  void UpdateView();

  /// Make sure the camera stays bound to a certain region.
  void BoundCamera();

  bool canJump(VCNPhysxController* actor);

  void startJump();

// Data members

  Camera&                   mCamera;     
  VCNPoint                  mPreviousCursorPosition;
  VCNBool                   mFreeLook;
  VCNLowPassFilter<Vector3> mInterpolatedCameraPosition;
  VCNPhysxController*		mPhysxController;
  PhysXRaycast*             mRaycast;
  PxReal					mGravity;
  VCNNode*					mCharacterNode;
  float						mJumpForce;
  PxVec3					mOldTargetKeyDisplacement;
  CameraMovingState         mMovingState;
  VCNBool                   mCanMove;
};

#endif // __GAMEPLAY_CONTROLLER_H__

