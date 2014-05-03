///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#include "Precompiled.h"

#include "Obb.h"
#include "Plane.h"
#include "Polygon.h"
#include "Vector.h"
#include "Matrix.h"
#include "Ray.h"

void VCNPlane::Set(const Vector3 &vcN, const Vector3 &vcP) 
{
  m_fD      = - ( vcN * vcP);
  m_vcN     = vcN;
  m_vcPoint = vcP;
}

/*----------------------------------------------------------------*/

void VCNPlane::Set(const Vector3 &vcN, const Vector3 &vcP, float fD) 
{
  m_vcN     = vcN;
  m_fD      = fD;
  m_vcPoint = vcP;
}
/*----------------------------------------------------------------*/

void VCNPlane::Set(const Vector3 &v0, const Vector3 &v1, const Vector3 &v2) 
{
  Vector3 vcEdge1 = v1 - v0;
  Vector3 vcEdge2 = v2 - v0;

  m_vcN = CrossProduct(vcEdge1, vcEdge2);
  m_vcN.Normalize();
  m_fD = - (m_vcN * v0);
  m_vcPoint = v0;
}

/*----------------------------------------------------------------*/

// Calculate distance to point. Plane normal must be normalized.
const float VCNPlane::Distance(const Vector3 &vcPoint) const
{
  return ( fabsf((m_vcN*vcPoint) - m_fD) );
}

/*----------------------------------------------------------------*/


// Classify point to plane.
int VCNPlane::Classify(const Vector3 &vcPoint) const
{
  float f = (vcPoint * m_vcN) + m_fD;

  if (f >  0.00001) return VCNFRONT;
  if (f < -0.00001) return VCNBACK;
  return VCNPLANAR;
}

/*----------------------------------------------------------------*/


// Classify polygon with respect to this plane
int VCNPlane::Classify(const VCNPolygon &Poly) const
{
  int NumFront=0, NumBack=0, NumPlanar=0;
  int nClass;

  // cast away const
  VCNPolygon *pPoly = ((VCNPolygon*)&Poly);

  int NumPoints = pPoly->GetNumPoints();

  // loop through all points
  for (int i=0; i < NumPoints; i++) {
    nClass = Classify( pPoly->m_pPoints[i] );
      
    if (nClass == VCNFRONT)     NumFront++;
    else if (nClass == VCNBACK) NumBack++;
    else {
      NumFront++;
      NumBack++;
      NumPlanar++;
      }
    } // for

  // all points are planar
  if (NumPlanar == NumPoints)
    return VCNPLANAR;
  // all points are in front of plane
  else if (NumFront == NumPoints)
    return VCNFRONT;
  // all points are on backside of plane
  else if (NumBack == NumPoints)
    return VCNBACK;
  // poly is intersecting the plane
  else
    return VCNCLIPPED;
}

/*----------------------------------------------------------------*/



// clips a ray into two segments if it intersects the plane
bool VCNPlane::Clip(const VCNRay *_pRay, float fL, VCNRay *pF, VCNRay *pB)
{
  Vector3 vcHit(0.0f,0.0f,0.0f);

  VCNRay *pRay = (VCNRay*)_pRay;

  // ray intersects plane at all?
  if ( !pRay->Intersects( *this, false, fL, NULL, &vcHit) ) 
    return false;

  int n = Classify( _pRay->m_vcOrig );

  // ray comes fron planes backside
  if ( n == VCNBACK ) {
    if (pB) pB->Set(pRay->m_vcOrig, pRay->m_vcDir);
    if (pF) pF->Set(vcHit, pRay->m_vcDir);
    }
  // ray comes from planes front side
  else if ( n == VCNFRONT ) {
    if (pF) pF->Set(pRay->m_vcOrig, pRay->m_vcDir);
    if (pB) pB->Set(vcHit, pRay->m_vcDir);
    }

  return true;
}

/*----------------------------------------------------------------*/


// Intersection of two planes. If third parameter is given the line
// of intersection will be calculated. (www.magic-software.com)
const bool VCNPlane::Intersects(const VCNPlane &plane, VCNRay *pIntersection) const
{
  Vector3 vcCross;
  float     fSqrLength;

  // if crossproduct of normals 0 than planes parallel
  vcCross = CrossProduct(this->m_vcN, plane.m_vcN);
  fSqrLength = vcCross.SquareLength();

  if (fSqrLength < 1e-08f) 
    return false;

  // find line of intersection
  if (pIntersection) {
    float fN00 = this->m_vcN.SquareLength();
    float fN01 = this->m_vcN * plane.m_vcN;
    float fN11 = plane.m_vcN.SquareLength();
    float fDet = fN00*fN11 - fN01*fN01;

    if (fabsf(fDet) < 1e-08f) 
      return false;

    float fInvDet = 1.0f/fDet;
    float fC0 = (fN11*this->m_fD - fN01*plane.m_fD) * fInvDet;
    float fC1 = (fN00*plane.m_fD - fN01*this->m_fD) * fInvDet;

    (*pIntersection).m_vcDir  = vcCross;
    (*pIntersection).m_vcOrig = this->m_vcN*fC0 + plane.m_vcN*fC1;
    }

  return true;
}

/*----------------------------------------------------------------*/


// Intersection of a plane with a triangle. If all vertices of the
// triangle are on the same side of the plane, no intersection occured. 
const bool VCNPlane::Intersects(const Vector3 &vc0, const Vector3 &vc1, const Vector3 &vc2) const 
{
  int n = this->Classify(vc0);

  if ( (n == this->Classify(vc1)) && 
    (n == this->Classify(vc2)) )
    return false;
  return true;
}

/*----------------------------------------------------------------*/


// Intersection with AABB. Search for AABB diagonal that is most
// aligned to plane normal. Test its two vertices against plane.
// (Möller/Haines, "VCNFloat-Time Rendering")
const bool VCNPlane::Intersects(const VCNAabb &aabb) const
{
  Vector3 Vmin, Vmax;

  // x component
  if (m_vcN.x >= 0.0f) 
  {
    Vmin.x = aabb.vcMin.x;
    Vmax.x = aabb.vcMax.x;
  }
  else
  {
    Vmin.x = aabb.vcMax.x;
    Vmax.x = aabb.vcMin.x;
  }

  // y component
  if (m_vcN.y >= 0.0f) 
  {
    Vmin.y = aabb.vcMin.y;
    Vmax.y = aabb.vcMax.y;
  }
  else 
  {
    Vmin.y = aabb.vcMax.y;
    Vmax.y = aabb.vcMin.y;
  }

  // z component
  if (m_vcN.z >= 0.0f) 
  {
    Vmin.z = aabb.vcMin.z;
    Vmax.z = aabb.vcMax.z;
  }
  else 
  {
    Vmin.z = aabb.vcMax.z;
    Vmax.z = aabb.vcMin.z;
  }

  if ( ((m_vcN * Vmin) + m_fD) > 0.0f)
    return false;

  if ( ((m_vcN * Vmax) + m_fD) >= 0.0f)
    return true;

  return false;
}

/*----------------------------------------------------------------*/


// Intersection with OBB. Same as obb culling to frustum planes.
const bool VCNPlane::Intersects(const VCNObb &obb) const
{
  float fRadius = fabsf( obb.fA0 * (m_vcN * obb.vcA0) ) 
          + fabsf( obb.fA1 * (m_vcN * obb.vcA1) ) 
          + fabsf( obb.fA2 * (m_vcN * obb.vcA2) );

  float fDistance = this->Distance(obb.vcCenter);
  return (fDistance <= fRadius);
}

/*----------------------------------------------------------------*/
