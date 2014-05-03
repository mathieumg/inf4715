///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#ifndef VICUNA_OBB
#define VICUNA_OBB

#include "Types.h"
#include "Vector.h"
#include "Matrix.h"

class VCNPlane;
class VCNRay;

//-------------------------------------------------------------
/// Vicuna's basic oriented box class
//-------------------------------------------------------------
class VCNObb
{
public:
  VCNFloat  fA0,   fA1,  fA2; // half axis length
  Vector3   vcA0, vcA1, vcA2; // box axis
  Vector3   vcCenter;         // centerpoint

  //---------------------------------------

  //VCNObb() {}

  void DeTransform( const VCNObb &obb, const Matrix4 &m );

  VCNBool Intersects( const VCNRay &Ray, VCNFloat *t );
  VCNBool Intersects( const VCNRay &Ray, VCNFloat fL, VCNFloat *t );
  VCNBool Intersects( const VCNObb &Obb );
  VCNBool Intersects( const Vector3 &v0, const Vector3 &v1, const Vector3 &v2 );

  VCNInt  Cull(const VCNPlane *pPlanes, VCNInt nNumPlanes);      

private:
  void ObbProj( const VCNObb &Obb, const Vector3 &vcV, VCNFloat *pfMin, VCNFloat *pfMax );
  void TriProj( const Vector3 &v0, const Vector3 &v1, const Vector3 &v2, const Vector3 &vcV, VCNFloat *pfMin, VCNFloat *pfMax );
};

#endif