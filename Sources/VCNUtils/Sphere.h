///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#ifndef VICUNA_SPHERE
#define VICUNA_SPHERE

#include "Types.h"
#include "Vector.h"

//-------------------------------------------------------------
/// Vicuna's basic spherical bounding volume class
//-------------------------------------------------------------
class VCNSphere
{
public:
  VCNSphere();
  VCNSphere( VCNFloat radius );
  VCNSphere( VCNFloat radius, const Vector3& center );

  void Set( VCNFloat radius, const Vector3& center );

  const VCNFloat GetRadius() const { return mRadius; }

  const Vector3& GetCenter() const { return mCenter; }

  VCNBool Intersects( const VCNSphere &sphere ) const;

private:

  // The center and radius of the sphere
  Vector3   mCenter;
  VCNFloat  mRadius;
};

#endif