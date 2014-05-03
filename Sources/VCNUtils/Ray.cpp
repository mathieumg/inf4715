///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#include "Precompiled.h"

#include <limits>

#include "Matrix.h"
#include "Obb.h"
#include "Plane.h"
#include "Polygon.h"
#include "Ray.h"
#include "Vector.h"
#include "Sphere.h"

// set attributes
void VCNRay::Set(const Vector3& vcOrig, const Vector3& vcDir) 
{
  m_vcOrig = vcOrig;
  m_vcDir  = vcDir;
}


// transform ray into matrix space
void VCNRay::DeTransform(const Matrix4 &_m) 
{
  Matrix4 mInv;
  Matrix4 m=_m;
  // invert translation
  m_vcOrig.x -= m.m41;   
  m_vcOrig.y -= m.m42;
  m_vcOrig.z -= m.m43;

  // delete it from matrix
  m.m41 = m.m42 = m.m43 = 0.0f;

  // invert matrix and apply to ray
  mInv = m.GetInverse();
  m_vcOrig = m_vcOrig * mInv;
  m_vcDir  = m_vcDir  * mInv;
}
/*----------------------------------------------------------------*/


// test for intersection with triangle
bool VCNRay::Intersects(const Vector3 &vc0, const Vector3 &vc1, 
                        const Vector3 &vc2, bool bCull, float *t) 
{
  Vector3 pvec, tvec, qvec;

  Vector3 edge1 = vc1 - vc0;
  Vector3 edge2 = vc2 - vc0;

  pvec = CrossProduct(m_vcDir, edge2);

  // if close to 0 ray is parallel
  float det = edge1 * pvec;
  if ( (bCull) && (det < 0.0001f) )
    return false;
  else if ( (det < 0.0001f) && (det > -0.0001f) )
    return false;

  // distance to plane, < 0 means beyond plane
  tvec = m_vcOrig - vc0;
  float u = tvec * pvec;
  if (u < 0.0f || u > det)
    return false;

  qvec = CrossProduct(tvec, edge1);
  float v = m_vcDir * qvec;
  if (v < 0.0f || u+v > det)
    return false;

  if (t) 
  {
    *t = edge2 * qvec;
    float fInvDet = 1.0f / det;
    *t *= fInvDet;
  }

  return true;
}

/*----------------------------------------------------------------*/


// test for intersection with triangle at certain length (line segment),
// same as above but test distance to intersection vs segment length.
bool VCNRay::Intersects(const Vector3 &vc0, const Vector3 &vc1, 
                        const Vector3 &vc2, bool bCull, float fL, float *t) 
{
   Vector3 pvec, tvec, qvec;

  Vector3 edge1 = vc1 - vc0;
  Vector3 edge2 = vc2 - vc0;

  pvec = CrossProduct(m_vcDir, edge2);

  // if close to 0 ray is parallel
  float det = edge1 * pvec;
  if ( (bCull) && (det < 0.0001f) )
    return false;
  else if ( (det < 0.0001f) && (det > -0.0001f) )
    return false;

  // distance to plane, < 0 means beyond plane
  tvec = m_vcOrig - vc0;
  float u = tvec * pvec;
  if (u < 0.0f || u > det)
    return false;

  qvec = CrossProduct(tvec, edge1);
  float v = m_vcDir * qvec;
  if (v < 0.0f || u+v > det)
    return false;

  if (t) 
  {
    *t = edge2 * qvec;
    float fInvDet = 1.0f / det;
    *t *= fInvDet;
    // collision but not on segment?
    if (*t > fL) return false; 
  }
  else 
  {
    // collision but not on segment?
    float f = (edge2*qvec) * (1.0f / det);
    if (f > fL) return false;
  }

  return true;
}

/*----------------------------------------------------------------*/


// test for intersection with aabb, original code by Andrew Woo, 
// from "Geometric Tools...", Morgan Kaufmann Publ., 2002
bool VCNRay::Intersects(const VCNAabb &aabb, float *t) 
{
  float t0, t1, tmp;
  float tNear = -999999.9f;
  float tFar  =  999999.9f;
  float epsilon = 0.00001f;
  Vector3 MaxT;

  // first pair of planes
  if (fabsf(m_vcDir.x) < epsilon) 
  {
    if ( (m_vcOrig.x < aabb.vcMin.x) ||
      (m_vcOrig.x > aabb.vcMax.x) )
    return false;
  }
  t0 = (aabb.vcMin.x - m_vcOrig.x) / m_vcDir.x;
  t1 = (aabb.vcMax.x - m_vcOrig.x) / m_vcDir.x;
  if (t0 > t1) { tmp=t0; t0=t1; t1=tmp; }
  if (t0 > tNear) tNear = t0;
  if (t1 < tFar)  tFar = t1;
  if (tNear > tFar) return false;
  if (tFar < 0) return false;

  // second pair of planes
  if (fabsf(m_vcDir.y) < epsilon)
  {
    if ( (m_vcOrig.y < aabb.vcMin.y) ||
      (m_vcOrig.y > aabb.vcMax.y) )
      return false;
  }
  t0 = (aabb.vcMin.y - m_vcOrig.y) / m_vcDir.y;
  t1 = (aabb.vcMax.y - m_vcOrig.y) / m_vcDir.y;
  if (t0 > t1) { tmp=t0; t0=t1; t1=tmp; }
  if (t0 > tNear) tNear = t0;
  if (t1 < tFar)  tFar = t1;
  if (tNear > tFar) return false;
  if (tFar < 0) return false;

  // third pair of planes
  if (fabsf(m_vcDir.z) < epsilon)
  {
    if ( (m_vcOrig.z < aabb.vcMin.z) ||
      (m_vcOrig.z > aabb.vcMax.z) )
      return false;
  }
  t0 = (aabb.vcMin.z - m_vcOrig.z) / m_vcDir.z;
  t1 = (aabb.vcMax.z - m_vcOrig.z) / m_vcDir.z;
  if (t0 > t1) { tmp=t0; t0=t1; t1=tmp; }
  if (t0 > tNear) tNear = t0;
  if (t1 < tFar)  tFar = t1;
  if (tNear > tFar) return false;
  if (tFar < 0) return false;


  if (tNear > 0) 
  { 
    if (t) 
      *t = tNear; 
  }
  else { if (t) *t = tFar; }
  return true;
} 

/*----------------------------------------------------------------*/


// test for intersection with aabb, original code by Andrew Woo, 
// from "Geometric Tools...", Morgan Kaufmann Publ., 2002
bool VCNRay::Intersects(const VCNAabb &aabb, float fL, float *t) 
{
  float t0, t1, tmp, tFinal;
  float tNear = -999999.9f;
  float tFar  =  999999.9f;
  float epsilon = 0.00001f;
  Vector3 MaxT;

  // first pair of planes
  if (fabsf(m_vcDir.x) < epsilon) 
  {
    if ( (m_vcOrig.x < aabb.vcMin.x) ||
      (m_vcOrig.x > aabb.vcMax.x) )
      return false;
  }
  t0 = (aabb.vcMin.x - m_vcOrig.x) / m_vcDir.x;
  t1 = (aabb.vcMax.x - m_vcOrig.x) / m_vcDir.x;
  if (t0 > t1) { tmp=t0; t0=t1; t1=tmp; }
  if (t0 > tNear) tNear = t0;
  if (t1 < tFar)  tFar = t1;
  if (tNear > tFar) return false;
  if (tFar < 0) return false;

  // second pair of planes
  if (fabsf(m_vcDir.y) < epsilon) 
  {
    if ( (m_vcOrig.y < aabb.vcMin.y) ||
      (m_vcOrig.y > aabb.vcMax.y) )
      return false;
  }
  t0 = (aabb.vcMin.y - m_vcOrig.y) / m_vcDir.y;
  t1 = (aabb.vcMax.y - m_vcOrig.y) / m_vcDir.y;
  if (t0 > t1) { tmp=t0; t0=t1; t1=tmp; }
  if (t0 > tNear) tNear = t0;
  if (t1 < tFar)  tFar = t1;
  if (tNear > tFar) return false;
  if (tFar < 0) return false;

  // third pair of planes
  if (fabsf(m_vcDir.z) < epsilon) 
  {
    if ( (m_vcOrig.z < aabb.vcMin.z) ||
      (m_vcOrig.z > aabb.vcMax.z) )
      return false;
  }
  t0 = (aabb.vcMin.z - m_vcOrig.z) / m_vcDir.z;
  t1 = (aabb.vcMax.z - m_vcOrig.z) / m_vcDir.z;
  if (t0 > t1) { tmp=t0; t0=t1; t1=tmp; }
  if (t0 > tNear) tNear = t0;
  if (t1 < tFar)  tFar = t1;
  if (tNear > tFar) return false;
  if (tFar < 0) return false;


  if (tNear > 0) 
    tFinal = tNear;
  else 
    tFinal = tFar;

  if (tFinal > fL) 
    return false;
  if (t) 
    *t = tFinal;
  return true;
} // Intersects(Aabb) at length
/*----------------------------------------------------------------*/


// test for intersection with obb, slaps method
bool VCNRay::Intersects(const VCNObb &obb, float *t)
{
  float e, f, t1, t2, temp;
  float tmin = -99999.9f, 
      tmax = +99999.9f;

  Vector3 vcP = obb.vcCenter - m_vcOrig;

  // 1st slap
  e = obb.vcA0 * vcP;
  f = obb.vcA0 * m_vcDir;
  if (fabsf(f) > 0.00001f)
  {
    t1 = (e + obb.fA0) / f;
    t2 = (e - obb.fA0) / f;

    if (t1 > t2) { temp=t1; t1=t2; t2=temp; }
    if (t1 > tmin) tmin = t1;
    if (t2 < tmax) tmax = t2;
    if (tmin > tmax) return false;
    if (tmax < 0.0f) return false;
  }
  else if ( ((-e - obb.fA0) > 0.0f) || ((-e + obb.fA0) < 0.0f) )
    return false;

  // 2nd slap
  e = obb.vcA1 * vcP;
  f = obb.vcA1 * m_vcDir;
  if (fabsf(f) > 0.00001f) 
  {
    t1 = (e + obb.fA1) / f;
    t2 = (e - obb.fA1) / f;

    if (t1 > t2) { temp=t1; t1=t2; t2=temp; }
    if (t1 > tmin) tmin = t1;
    if (t2 < tmax) tmax = t2;
    if (tmin > tmax) return false;
    if (tmax < 0.0f) return false;
  }
  else if ( ((-e - obb.fA1) > 0.0f) || ((-e + obb.fA1) < 0.0f) )
    return false;

  // 3rd slap
  e = obb.vcA2 * vcP;
  f = obb.vcA2 * m_vcDir;
  if (fabsf(f) > 0.00001f) 
  {

    t1 = (e + obb.fA2) / f;
    t2 = (e - obb.fA2) / f;

    if (t1 > t2) { temp=t1; t1=t2; t2=temp; }
    if (t1 > tmin) tmin = t1;
    if (t2 < tmax) tmax = t2;
    if (tmin > tmax) return false;
    if (tmax < 0.0f) return false;
  }
  else if ( ((-e - obb.fA2) > 0.0f) || ((-e + obb.fA2) < 0.0f) )
    return false;

  if (tmin > 0.0f) 
  {
    if (t) *t = tmin;
    return true;
  }

  if (t)
    *t = tmax;

  return true;
}
/*----------------------------------------------------------------*/


// test for intersection with obb at certain length (line segment),
// slaps method but compare result if true to length prior return.
bool VCNRay::Intersects(const VCNObb &obb, float fL, float *t) 
{
  float e, f, t1, t2, temp;
  float tmin = -99999.9f, 
      tmax = +99999.9f;

  Vector3 vcP = obb.vcCenter - m_vcOrig;

  // 1st slap
  e = obb.vcA0 * vcP;
  f = obb.vcA0 * m_vcDir;
  if (fabsf(f) > 0.00001f) 
  {

    t1 = (e + obb.fA0) / f;
    t2 = (e - obb.fA0) / f;

    if (t1 > t2) { temp=t1; t1=t2; t2=temp; }
    if (t1 > tmin) tmin = t1;
    if (t2 < tmax) tmax = t2;
    if (tmin > tmax) return false;
    if (tmax < 0.0f) return false;
  }
  else if ( ((-e - obb.fA0) > 0.0f) || ((-e + obb.fA0) < 0.0f) )
    return false;

  // 2nd slap
  e = obb.vcA1 * vcP;
  f = obb.vcA1 * m_vcDir;
  if (fabsf(f) > 0.00001f) 
  {
    t1 = (e + obb.fA1) / f;
    t2 = (e - obb.fA1) / f;

    if (t1 > t2) { temp=t1; t1=t2; t2=temp; }
    if (t1 > tmin) tmin = t1;
    if (t2 < tmax) tmax = t2;
    if (tmin > tmax) return false;
    if (tmax < 0.0f) return false;
  }
  else if ( ((-e - obb.fA1) > 0.0f) || ((-e + obb.fA1) < 0.0f) )
    return false;

  // 3rd slap
  e = obb.vcA2 * vcP;
  f = obb.vcA2 * m_vcDir;
  if (fabsf(f) > 0.00001f)
  {
    t1 = (e + obb.fA2) / f;
    t2 = (e - obb.fA2) / f;

    if (t1 > t2) { temp=t1; t1=t2; t2=temp; }
    if (t1 > tmin) tmin = t1;
    if (t2 < tmax) tmax = t2;
    if (tmin > tmax) return false;
    if (tmax < 0.0f) return false;
  }
  else if ( ((-e - obb.fA2) > 0.0f) || ((-e + obb.fA2) < 0.0f) )
    return false;

  if ( (tmin > 0.0f) && (tmin <= fL) ) 
  {
    if (t) 
      *t = tmin;
    return true;
  }

  // intersection on line but not on segment
  if (tmax > fL) 
    return false;

  if (t) 
    *t = tmax;

  return true;
}

/*----------------------------------------------------------------*/


// Intersection with Plane from origin till infinity. 
bool VCNRay::Intersects(const VCNPlane &plane, bool bCull, 
                        float *t, Vector3 *vcHit) 
{
  float Vd = plane.m_vcN * m_vcDir;

  // ray parallel to plane
  if (fabsf(Vd) < 0.00001f)
    return false;

  // normal pointing away from ray dir
  // => intersection backface if any
  if (bCull && (Vd > 0.0f))
    return false;

  float Vo = -( (plane.m_vcN * m_vcOrig) + plane.m_fD);

  float _t = Vo / Vd;

  // intersection behind ray origin
  if (_t < 0.0f)
    return false;

  if (vcHit) {
    (*vcHit) = m_vcOrig + (m_vcDir * _t);
    }

  if (t)
    (*t) = _t;

  return true;
}

/*----------------------------------------------------------------*/


// Intersection with Plane at distance fL. 
bool VCNRay::Intersects(const VCNPlane &plane, bool bCull, float fL,
                        float *t, Vector3 *vcHit) 
{
  float Vd = plane.m_vcN * m_vcDir;

  // ray parallel to plane
  if (fabsf(Vd) < 0.00001f)
    return false;

  // normal pointing away from ray dir
  // => intersection backface if any
  if (bCull && (Vd > 0.0f))
    return false;

  float Vo = -( (plane.m_vcN * m_vcOrig) + plane.m_fD);

  float _t = Vo / Vd;

  // intersection behind ray origin or beyond valid range
  if ( (_t < 0.0f) || (_t > fL) )
    return false;

  if (vcHit) {
    (*vcHit) = m_vcOrig + (m_vcDir * _t);
    }

  if (t)
    (*t) = _t;

  return true;
}

//////////////////////////////////////////////////////////////////////////
VCNBool VCNRay::Intersects( const VCNSphere& sphere, VCNFloat& t ) const
{
  Vector3 w = sphere.GetCenter() - m_vcOrig;
  VCNFloat c = w.Length();
  VCNFloat v = w.DotProduct(m_vcDir);
  VCNFloat d = (sphere.GetRadius() * sphere.GetRadius()) - (c*c - v*v);

  t = -1.0f;

  // If there was no intersection, return -1
  if (d < 0.0f)
    return false;

  // Return the distance to the [first] intersecting point
  t = v - sqrt(d);
  return true;
}

/*----------------------------------------------------------------*/


