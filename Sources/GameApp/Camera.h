///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Camera interface
///

#ifndef __CAMERA_H__
#define __CAMERA_H__

#pragma once

#include "VCNUtils/Matrix.h"
#include "VCNUtils/Vector.h"

//////////////////////////////////////////////////////////////////////////
///
///  @class Camera
///
///  @brief The camera class is responsible to view a scene from different 
///         angles. This camera class support the viewing and projection.
///
///  <pre>
///           up
///           ^        dir (focus - position)
///           |        /      
///           |      /
///           |    /
///           |  /
///           |/_________>strafe
///           pos
///  </pre>
///
///  @image html http://www.evl.uic.edu/ralph/508S98/gif/lefty.gif "Left-handed coordinate system"
///
///  @remarks the camera system is based on UVN system.
//////////////////////////////////////////////////////////////////////////
class Camera
{

public:

  /// Construct the camera
  explicit Camera(
    VCNFloat positionX = 0.0f, VCNFloat positionY = 0.0f, VCNFloat positionZ = 0.0f,
    VCNFloat focusX    = 0.0f, VCNFloat focusY    = 0.0f, VCNFloat focusZ    = -1.0f,
    VCNFloat upVectorX = 0.0f, VCNFloat upVectorY = 1.0f, VCNFloat upVectorZ = 0.0f);


  /// Setup the  camera properties using vectors
  void setup(
    const Vector3 &position,
    const Vector3 &focus,
    const Vector3 &up);

  /// Setup the camera properties using coordinates
  void setup(
    VCNFloat positionX, VCNFloat positionY, VCNFloat positionZ,
    VCNFloat focusX   , VCNFloat focusY   , VCNFloat focusZ   ,
    VCNFloat upVectorX, VCNFloat upVectorY, VCNFloat upVectorZ);

// Attributes

  /// Return the associated view matrix.
  const Matrix4&   GetViewMatrix() const { return mViewMatrix; }

  /// Return the viewer position.
  const Vector3&   GetViewerPosition() const { return mPosition; }

  /// Return the focus position (target's position)
  const Vector3&   GetFocusPosition() const { return mFocus; }

  /// Return the up vector.
  const Vector3&   GetUpDirection() const { return mUp; }

  /// Return the pointing direction (Position -> focus)
  const Vector3&   GetDirection() const { return mDir; }

  /// Returns the strafing direction of the camera
  const Vector3&   GetStrafeDirection() const { return mStrafe; }

  /// Set the viewer position.
  void             SetViewerPosition(const Vector3 &position_)  { mPosition = position_; }

  /// Set the focus position.
  void             SetFocusPosition(const Vector3 &focus_) { mFocus = focus_; }

  /// Set the up vector of the camera.
  void             SetUpDirection(const Vector3 &up_) { mUp = up_; }

  /// Set the rotation sensitivity (maybe used by a controller)
  void             SetRotationSensitivity(VCNFloat zeroToOne);

// Interface
  
  /// Strafe the camera left or right
  void StrafeMove(VCNFloat speed);

  /// Strafe the camera up based on the direction and strafing plan
  void StrafeMoveUp(VCNFloat speed);

  /// Strafe the camera perpendicularly to the strafing direction and the Y axis
  void StrafeMoveFront(VCNFloat speed);
  
  /// Move the camera backward or forward
  void Move(VCNFloat speed);

  /// Elevate the camera up or down. (using up vector)
  void Elevate(VCNFloat speed);

  /// Rotate the camera using screen coordinate.
  void Rotate(int deltaX, int deltaY);

  /// Rotate the camera using screen coordinate with more precision.
  void Rotate(VCNFloat yaw, VCNFloat pitch);

  /// Adjust the camera height and set UVN vectors
  void AdjustCameraHeight(VCNFloat height);

  /// Update the UVN system of the camera.
  void Update();

private:

// Implementation

  /// Rotate the view, applying rotation transformations.
  void RotateView(VCNFloat angle, VCNFloat X, VCNFloat Y, VCNFloat Z);

// Data members

  VCNFloat        mRotateSensitivity;     ///< Set the rotation sensitivity
  Matrix4     mViewMatrix;            ///< Computed view matrix (need to be updated)
  Vector3     mPosition;              ///< Position of the camera in world space
  Vector3     mStrafe;                ///< Strafing vector
  Vector3     mFocus;                 ///< Focus target position
  Vector3     mUp;                    ///< Up vector (head of camera)
  Vector3     mDir;                   ///< Cached dir.

// Constants

  static const VCNFloat BLOCK_ANGLE;          ///< Angle at which the camera is blocked
  static const VCNFloat DEFAULT_SENSITIVITY;  ///< Default rotation sensitivity
};

#endif // __CAMERA_H__
