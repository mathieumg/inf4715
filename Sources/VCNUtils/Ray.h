///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#ifndef VICUNA_RAY
#define VICUNA_RAY

#pragma once

#include "Vector.h"
#include "Matrix.h"

class VCNPlane;
class VCNAabb;
class VCNObb;
class VCNSphere;

// Our basic ray class
class VCNRay
{
public:
  Vector3 m_vcOrig;    // ray origin
  Vector3 m_vcDir;    // ray direction

  //---------------------------------------

  VCNRay(void) { /* nothing to do */ ; }

  VCNRay(const Vector3& orig, const Vector3& dir) { m_vcOrig = orig; m_vcDir = dir; }

  void Set(const Vector3& vcOrig, const Vector3& vcDir);
  void DeTransform(const Matrix4 &_m); // move to matrixspace

  VCNBool Intersects(const Vector3 &vc0, const Vector3 &vc1,
    const Vector3 &vc2, VCNBool bCull, VCNFloat *t);

  VCNBool Intersects(const Vector3 &vc0, const Vector3 &vc1,
    const Vector3 &vc2, VCNBool bCull, VCNFloat fL, VCNFloat *t);

  VCNBool Intersects(const VCNPlane &plane, VCNBool bCull,
    VCNFloat *t, Vector3 *vcHit);       

  VCNBool Intersects(const VCNPlane &plane, VCNBool bCull,
    VCNFloat fL, VCNFloat *t, Vector3 *vcHit);

  VCNBool Intersects(const VCNAabb &aabb, VCNFloat *t);

  VCNBool Intersects(const VCNAabb &aabb, VCNFloat fL, VCNFloat *t);

  VCNBool Intersects(const VCNObb &obb, VCNFloat *t);

  VCNBool Intersects(const VCNObb &obb, VCNFloat fL, VCNFloat *t) ;

  VCNBool Intersects(const VCNSphere& sphere, VCNFloat& t) const;
};


#endif