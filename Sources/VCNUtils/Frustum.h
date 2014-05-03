///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Frustum Culling - Used to optimize viewing volume.
///

#ifndef FRUSTUM_H
#define FRUSTUM_H

#pragma once

#include "Vector.h"
#include "Matrix.h"
 
class Frustum
{

public:

   /// Update the viewing volume based on OpenGL state.
   void Update(const Matrix4& viewProj);

   /// Get the distance from a specific plane from a point.
   float GetDistanceToPlane(int plane, const Vector3 &center) const;

   /// Check if a point is include in the frustum.
   bool PointInFrustum(float x, float y, float z) const;
   bool PointInFrustum(const Vector3 & center) const;

   /// Check if a sphere is include in the frustum.
   bool SphereInFrustum(float x, float y, float z, float radius) const;
   bool SphereInFrustum(const Vector3 &center, float radius) const;

private:
   
   Vector4   mFrustumPlane[6];   ///< Frustum planes
};

#endif 
