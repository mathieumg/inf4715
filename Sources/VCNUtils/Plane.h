///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#ifndef VICUNA_PLANE
#define VICUNA_PLANE

#include "Types.h"
#include "Vector.h"

class VCNAabb;
class VCNObb;
class VCNPolygon;
class VCNRay;

#define VCNFRONT    0
#define VCNBACK     1
#define VCNPLANAR   2
#define VCNCLIPPED  3
#define VCNCULLED   4
#define VCNVISIBLE  5

//-------------------------------------------------------------
/// Vicuna's basic plane class
//-------------------------------------------------------------
class VCNPlane
{
public:
  Vector3   m_vcN,      // plane normal vector
            m_vcPoint;  // point on plane
  VCNFloat  m_fD;       // distance to origin

  //---------------------------------------

  VCNPlane()
  : m_fD(0) 
  { /* nothing to do */ ; }

  void  Set( const Vector3 &vcN, const Vector3 &vcP );
  void  Set( const Vector3 &vcN, const Vector3 &vcP, VCNFloat fD );
  void  Set( const Vector3 &v0,  const Vector3 &v1, const Vector3 &v2 );
  const VCNFloat Distance( const Vector3 &vcPoVCNInt ) const;
  VCNInt Classify( const Vector3 &vcPoVCNInt ) const;
  VCNInt Classify( const VCNPolygon &Polygon ) const;

  bool Clip(const VCNRay*, VCNFloat, VCNRay*, VCNRay*);

  const VCNBool Intersects(const Vector3 &vc0, const Vector3 &vc1, const Vector3 &vc2) const;
  const VCNBool Intersects(const VCNPlane &plane, VCNRay *pIntersection) const;
  const VCNBool Intersects(const VCNAabb &aabb) const;
  const VCNBool Intersects(const VCNObb &obb) const;

};

#endif