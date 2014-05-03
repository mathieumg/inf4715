///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Camera implementation
///

#include "Precompiled.h"
#include "Camera.h"

// Project includes
#include "Config.h"

// Engine includes
#include "VCNUtils/Utilities.h"

/// Constants definitions
const VCNFloat Camera::BLOCK_ANGLE          = _R(0.99);
const VCNFloat Camera::DEFAULT_SENSITIVITY  = _R(0.002);

//////////////////////////////////////////////////////////////////////////
///  Construct the camera object specifying all coordinates individuality.
///
///  @param [in]      x [=0.000000]   x coordinate for the position
///  @param [in]      y [=0.000000]   y coordinate for the position
///  @param [in]      z [=0.000000]   z coordinate for the position
///  @param [in]      xf [=0.000000]  x coordinate for the target position
///  @param [in]      yf [=0.000000]  y coordinate for the target position
///  @param [in]      zf [=-1.000000] z coordinate for the target position
///  @param [in]      xu [=0.000000]  x coordinate for the up vector orientation
///  @param [in]      yu [=1.000000]  y coordinate for the up vector orientation
///  @param [in]      zu [=0.000000]  z coordinate for the up vector orientation
///
///  @see Camera::setup
///
//////////////////////////////////////////////////////////////////////////
Camera::Camera(
               VCNFloat x , VCNFloat y , VCNFloat z,
               VCNFloat xf, VCNFloat yf, VCNFloat zf,
               VCNFloat xu, VCNFloat yu, VCNFloat zu)
               : mRotateSensitivity(DEFAULT_SENSITIVITY)
{
  setup(x, y, z, xf, yf, zf, xu, yu, zu);
}



//////////////////////////////////////////////////////////////////////////
///  Setup the camera properties
///
///  @param [in]      position   position of the camera
///  @param [in]      focus      target position of the camera
///  @param [in]      up        up vector
///
///  This function doesn't return a value
//////////////////////////////////////////////////////////////////////////
void Camera::setup(
                   const Vector3 &position,
                   const Vector3 &focus,
                   const Vector3 &up)
{
  mPosition  = position;
  mFocus     = focus;
  mUp        = up;

  // Prevent the camera to be jammed looking at self.
  if ( VCN::IsZero( (mFocus - mPosition).Length() ) )
  {
    mFocus = mPosition + Vector3(0, 1, 0);
  }

  Update();
}



//////////////////////////////////////////////////////////////////////////
///  Setup the camera properties
///
///  @param [in]      x   x coordinate for the position
///  @param [in]      y   y coordinate for the position
///  @param [in]      z   z coordinate for the position
///  @param [in]      xf  x coordinate for the target position
///  @param [in]      yf  y coordinate for the target position
///  @param [in]      zf  z coordinate for the target position
///  @param [in]      xu  x coordinate for the up vector orientation
///  @param [in]      yu  y coordinate for the up vector orientation
///  @param [in]      zu  z coordinate for the up vector orientation
///
///  This function doesn't return a value
//////////////////////////////////////////////////////////////////////////
void Camera::setup(
                   VCNFloat x , VCNFloat y , VCNFloat z,
                   VCNFloat xf, VCNFloat yf, VCNFloat zf,
                   VCNFloat xu, VCNFloat yu, VCNFloat zu)
{
  setup (Vector3(x, y, z), Vector3(xf, yf, zf), Vector3(xu, yu, zu));
}



//////////////////////////////////////////////////////////////////////////
///  Set the camera sensitivity. Used when rotating the camera.
///
///  @param [in]      sensitivity value between [0,1]
///
///  This function doesn't return a value
//////////////////////////////////////////////////////////////////////////
void Camera::SetRotationSensitivity(VCNFloat sensitivity)
{
  mRotateSensitivity = VCN::Clamp(sensitivity, _R(0.0001), _R(1.0));
}



//////////////////////////////////////////////////////////////////////////
///  Update the camera UVN system. Making sure every vector is validated.
///  This view matrix is updated here.
///
///  This function doesn't return a value
//////////////////////////////////////////////////////////////////////////
void Camera::Update()
{
  mDir = (mFocus - mPosition).Normalized();
  mStrafe = mUp.CrossProduct(mDir).Normalized();
  mViewMatrix.SetLookAt(mPosition, mFocus, mUp);
}



//////////////////////////////////////////////////////////////////////////
///  Move the camera forward or backward.
///
///  @param [in]      speed   moving speed. a negative value indicates that
///                      the camera moves backward otherwise it moves forward.
///
///  This function doesn't return a value
//////////////////////////////////////////////////////////////////////////
void Camera::Move(VCNFloat speed)
{
  const Vector3 dir = mDir * speed;
  mPosition += dir;
  mFocus    += dir;
}



//////////////////////////////////////////////////////////////////////////
///  Strafe the camera left or right based on @a speed.
///
///  @param [in]      speed   Moving speed. A negative value indicates that
///                      the camera moves left otherwise it strafes right.
///
///  This function doesn't return a value
//////////////////////////////////////////////////////////////////////////
void Camera::StrafeMove(VCNFloat speed)
{
  const Vector3 dir = mStrafe * speed;
  mPosition += dir;
  mFocus += dir;
}



//////////////////////////////////////////////////////////////////////////
///
///  Strafe up perpendicular to the dir x strafe plan
///
///  @param [in]      speed   moving speed
///
///  This function doesn't return a value
///
//////////////////////////////////////////////////////////////////////////
void Camera::StrafeMoveUp(VCNFloat speed)
{
  const Vector3 dir = mDir.CrossProduct(mStrafe) * speed;
  mPosition += dir;
  mFocus    += dir;
}

//////////////////////////////////////////////////////////////////////////
///
///  Strafe up perpendicular to the strafe plan x Y axis
///
///  @param [in]      speed   moving speed
///
///  This function doesn't return a value
///
//////////////////////////////////////////////////////////////////////////
void Camera::StrafeMoveFront(VCNFloat speed)
{
    const Vector3 Y(0.f, 1.f, 0.f);

    const Vector3 dir = mStrafe.CrossProduct(Y) * speed;
    mPosition += dir;
    mFocus    += dir;
}

//////////////////////////////////////////////////////////////////////////
///  Elevate the camera down or up.
///
///  @param [in]      speed   Moving speed. A negative value indicates that
///                      the camera goes down otherwise it goes up.
///
///  This function doesn't return a value
//////////////////////////////////////////////////////////////////////////
void Camera::Elevate(VCNFloat speed)
{
  mPosition += mUp * speed;
  mFocus    += mUp * speed;
}



//////////////////////////////////////////////////////////////////////////
///  Rotate the camera view.
///
///  @param [in]      angle  angle of rotation in radians.
///  @param [in]      x     rotation factor on the x axis.
///  @param [in]      y     rotation factor on the y axis.
///  @param [in]      z     rotation factor on the z axis.
///
///  This function doesn't return a value
//////////////////////////////////////////////////////////////////////////
void Camera::RotateView(VCNFloat angle, VCNFloat x, VCNFloat y, VCNFloat z)
{
  Vector3 newView,
    vView = mDir;

  VCNFloat cosTheta = cos(angle),
    sinTheta = sin(angle);

  // Find the new x position for the new rotated point
  newView.x  = (cosTheta + (1 - cosTheta) * x * x)      * vView.x;
  newView.x += ((1 - cosTheta) * x * y - z * sinTheta)  * vView.y;
  newView.x += ((1 - cosTheta) * x * z + y * sinTheta)  * vView.z;

  // Find the new y position for the new rotated point
  newView.y  = ((1 - cosTheta) * x * y + z * sinTheta)  * vView.x;
  newView.y += (cosTheta + (1 - cosTheta) * y * y)      * vView.y;
  newView.y += ((1 - cosTheta) * y * z - x * sinTheta)  * vView.z;

  // Find the new z position for the new rotated point
  newView.z  = ((1 - cosTheta) * x * z - y * sinTheta)  * vView.x;
  newView.z += ((1 - cosTheta) * y * z + x * sinTheta)  * vView.y;
  newView.z += (cosTheta + (1 - cosTheta) * z * z)      * vView.z;

  // Now we just add the newly rotated vector to our position to set
  // our new rotated view of our camera.
  mFocus  = mPosition;
  mFocus += newView;
  mDir = (mFocus - mPosition).Normalized();
}



//////////////////////////////////////////////////////////////////////////
///  Rotate the camera based on screen movement.
///
///  @param [in]      deltaX   movement delta on X
///  @param [in]      deltaY   movement delta on Y
///
///  This function doesn't return a value
//////////////////////////////////////////////////////////////////////////
void Camera::Rotate(int deltaX, int deltaY)
{
  if ( deltaX == 0 && deltaY == 0 )
    return;

  float angleY = deltaX * mRotateSensitivity,
    angleZ = deltaY * mRotateSensitivity;

  Rotate(angleY, angleZ);
}



//////////////////////////////////////////////////////////////////////////
///  Rotate the camera.
///
///  @param [in]      yaw   movement delta on X
///  @param [in]      pitch  movement delta on Y
///
///  @remark this version of rotation rotate using floats.
///
///  This function doesn't return a value
//////////////////////////////////////////////////////////////////////////
void Camera::Rotate(VCNFloat yaw, VCNFloat pitch)
{
  Vector3 vAxis = mFocus - mPosition;
  vAxis = vAxis.CrossProduct(mUp).Normalized();

  RotateView(pitch, vAxis.x, vAxis.y, vAxis.z);

  // Prevent head and feet angles
  if ( VCN::Abs(mDir.DotProduct(mUp)) > BLOCK_ANGLE )
  {
    RotateView(-pitch, vAxis.x, vAxis.y, vAxis.z);
  }

  RotateView(yaw, mUp.x, mUp.y, mUp.z);
}



//////////////////////////////////////////////////////////////////////////
/// Adjust camera height. 
///
/// @param  height          - The new height. 
//////////////////////////////////////////////////////////////////////////
void Camera::AdjustCameraHeight(VCNFloat height)
{
  const Vector3& camPos = GetViewerPosition();
  const Vector3& camTarget = GetFocusPosition();

  const float oldViewerHeight = camPos.y;
  SetViewerPosition(Vector3(camPos.x, height, camPos.z));

  const float adjustHeight = height - oldViewerHeight;
  SetFocusPosition(Vector3(camTarget.x, camTarget.y + adjustHeight, camTarget.z));
}
