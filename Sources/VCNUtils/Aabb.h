///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#ifndef VICUNA_AABB
#define VICUNA_AABB

#include "Types.h"
#include "Vector.h"

class VCNObb;
class VCNPlane;
class VCNRay;

//-------------------------------------------------------------
/// Vicuna's basic axis aligned bounding box class
//-------------------------------------------------------------
class VCNAabb
{
public:
  Vector3 vcMin, vcMax; // box extreme points

  //---------------------------------------

  VCNAabb() {}
  VCNAabb( const Vector3& vcMin, const Vector3& vcMax );

  void Construct( const VCNObb *pObb ); // build from obb
  VCNInt Cull( const VCNPlane *pPlanes, VCNInt nNumPlanes );  

  // Get the planes with normals pointing outwards
  void GetPlanes( VCNPlane *pPlanes );

  const VCNBool Contains( const VCNRay &Ray, VCNFloat fL ) const;
  const VCNBool Intersects( const VCNRay &Ray, VCNFloat *t = 0) const;
  const VCNBool Intersects( const VCNRay &Ray, VCNFloat fL, VCNFloat *t = 0) const;
  const VCNBool Intersects( const VCNAabb &aabb ) const;
  const VCNBool Intersects( const Vector3 &vc0 ) const;
};

#endif