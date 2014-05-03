///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Frustum Culling implementation
///

#include "Precompiled.h"
#include "Frustum.h"

#include "Matrix.h"
#include "Chrono.h"

#define RIGHT  0  // The RIGHT side of the frustum
#define LEFT   1  // The LEFT    side of the frustum
#define BOTTOM 2  // The BOTTOM side of the frustum
#define TOP    3  // The TOP side of the frustum
#define BACK   4  // The BACK   side of the frustum
#define FRONT  5  // The FRONT side of the frustum

//////////////////////////////////////////////////////////////////////////
///
///  Update the viewing volume based on OpenGL state.
///
///  @return nothing
///
//////////////////////////////////////////////////////////////////////////
void Frustum::Update(const Matrix4& clip)
{    
  Vector4* p = 0;

  // Now that we have our model view and projection matrix, if we combine these 
  // 2 matrices, it will give us our clipping planes.  To combine 2 matrices, 
  // we multiply them.

  p = &mFrustumPlane[RIGHT];
  p->x=clip[3]-clip[0];
  p->y=clip[7]-clip[4];
  p->z=clip[11]-clip[8];
  p->w=clip[15]-clip[12];

  p=&mFrustumPlane[LEFT];
  p->x=clip[3]+clip[0];
  p->y=clip[7]+clip[4];
  p->z=clip[11]+clip[8];
  p->w=clip[15]+clip[12];

  p=&mFrustumPlane[BOTTOM];
  p->x=clip[3]+clip[1];
  p->y=clip[7]+clip[5];
  p->z=clip[11]+clip[9];
  p->w=clip[15]+clip[13];

  p=&mFrustumPlane[TOP];
  p->x=clip[3]-clip[1];
  p->y=clip[7]-clip[5];
  p->z=clip[11]-clip[9];
  p->w=clip[15]-clip[13];

  p=&mFrustumPlane[BACK];
  p->x=clip[3]-clip[2];
  p->y=clip[7]-clip[6];
  p->z=clip[11]-clip[10];
  p->w=clip[15]-clip[14];

  p=&mFrustumPlane[FRONT];
  p->x=clip[3]+clip[2];
  p->y=clip[7]+clip[6];
  p->z=clip[11]+clip[10];
  p->w=clip[15]+clip[14];

  // Normalize all plane normals
  float magnitude = 0.0;
  for(int i=0;i<6;i++) { 
    magnitude = sqrt(
      mFrustumPlane[i].x * mFrustumPlane[i].x + 
      mFrustumPlane[i].y * mFrustumPlane[i].y + 
      mFrustumPlane[i].z * mFrustumPlane[i].z );
    if(magnitude)
      mFrustumPlane[i] /= magnitude;
  }
}



//////////////////////////////////////////////////////////////////////////
///
///  Check if a point is include in the frustum.
///
///  @param [in]       center  point to check
///
///  @return true if the point is include in frustum.
///
//////////////////////////////////////////////////////////////////////////
bool Frustum::PointInFrustum(const Vector3 & center) const
{
  return PointInFrustum(center[0], center[1], center[2]);
}



//////////////////////////////////////////////////////////////////////////
///
///  Check if a point is include in the frustum.
///
///  @param [in]       x  x coordinate of point to check
///  @param [in]       y  y coordinate of point to check
///  @param [in]       z  z coordinate of point to check
///
///  @return true if the point is include in frustum.
///
//////////////////////////////////////////////////////////////////////////
bool Frustum::PointInFrustum( float x, float y, float z ) const
{
  // Go through all the sides of the frustum
  for(int i = 0; i < 6; i++ )
  {
    // Calculate the plane equation and check if the point is behind a side of the frustum
    if(mFrustumPlane[i].x * x + 
       mFrustumPlane[i].y * y + 
       mFrustumPlane[i].z * z + 
       mFrustumPlane[i].w <= 0)
    {
      // The point was behind a side, so it ISN'T in the frustum
      return false;
    }
  }

  // The point was inside of the frustum (In front of ALL the sides of the frustum)
  return true;
}



//////////////////////////////////////////////////////////////////////////
///
///  This retrieves our distance from a frustum plane
///
///  @param [in]       plane  reference plane
///  @param [in]       center  point to check
///
///  @return distance between plane and point
///
//////////////////////////////////////////////////////////////////////////
float Frustum::GetDistanceToPlane(int i, const Vector3 &center) const
{
  return mFrustumPlane[i].x * center.x +
         mFrustumPlane[i].y * center.y +
         mFrustumPlane[i].z * center.z +
         mFrustumPlane[i].w;
}



//////////////////////////////////////////////////////////////////////////
///
///  Check if a sphere is include in the frustum.
///
///  @param [in]       center  center of sphere
///  @param [in]       radius  radius of sphere
///
///  @return true if the sphere is in the frustum
///
//////////////////////////////////////////////////////////////////////////
bool Frustum::SphereInFrustum(const Vector3 &center, float radius) const
{
  return SphereInFrustum(center.x, center.y, center.z, radius);
}



//////////////////////////////////////////////////////////////////////////
///
///  Check if a sphere is include in the frustum.
///
///  @param [in]       x  x coordinate of center of the sphere to check
///  @param [in]       y  y coordinate of center of the sphere to check
///  @param [in]       z  z coordinate of center of the sphere to check
///  @param [in]       radius  radius of sphere
///
///  @return true if the sphere is in the frustum
///
//////////////////////////////////////////////////////////////////////////
bool Frustum::SphereInFrustum(float x, float y, float z, float radius) const
{
  // Go through all the sides of the frustum
  for(int i = 0; i < 6; ++i )   
  {
    // If the center of the sphere is farther away from the plane than the radius
    if( mFrustumPlane[i].x * x + 
        mFrustumPlane[i].y * y + 
        mFrustumPlane[i].z * z + 
        mFrustumPlane[i].w <= -radius )
    {
      // The distance was greater than the radius so the sphere is outside of the frustum
      return false;
    }
  }

  // The sphere was inside of the frustum!
  return true;
}

