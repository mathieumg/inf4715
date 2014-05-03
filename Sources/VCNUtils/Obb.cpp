///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#include "Precompiled.h"

#include "Obb.h"
#include "Plane.h"
#include "Ray.h"
#include "Vector.h"
#include "Matrix.h"

void VCNObb::DeTransform(const VCNObb &obb, const Matrix4 &m) 
{
  // rotate center and axis to matrix coord.-space
  this->vcCenter = m.TransformCoord(obb.vcCenter);
  this->vcA0     = m.TransformNormal(obb.vcA0);
  this->vcA1     = m.TransformNormal(obb.vcA1);
  this->vcA2     = m.TransformNormal(obb.vcA2);
  
  // copy axis length
  fA0 = obb.fA0;
  fA1 = obb.fA1;
  fA2 = obb.fA2;
}

/*----------------------------------------------------------------*/

// helper function
void VCNObb::ObbProj(const VCNObb &Obb, const Vector3 &vcV,
                     float *pfMin, float *pfMax) 
{
  float fDP = vcV.DotProduct(Obb.vcCenter);
  float fR = Obb.fA0 * fabsf(vcV.DotProduct(Obb.vcA0)) +
             Obb.fA0 * fabsf(vcV.DotProduct(Obb.vcA1)) +
             Obb.fA1 * fabsf(vcV.DotProduct(Obb.vcA2));
  *pfMin = fDP - fR;
  *pfMax = fDP + fR;
}

/*----------------------------------------------------------------*/


// helperfunction
void VCNObb::TriProj(const Vector3 &v0, const Vector3 &v1, 
                     const Vector3 &v2, const Vector3 &vcV,
                     float *pfMin, float *pfMax) 
{
  *pfMin = vcV.DotProduct(v0);
  *pfMax = *pfMin;

  float fDP = vcV.DotProduct(v1);
  if (fDP < *pfMin) *pfMin = fDP;
  else if (fDP > *pfMax) *pfMax = fDP;

  fDP = vcV.DotProduct(v2);
  if (fDP < *pfMin) *pfMin = fDP;
  else if (fDP > *pfMax) *pfMax = fDP;
}

/*----------------------------------------------------------------*/


// intersects trianlge
bool VCNObb::Intersects(const Vector3 &v0, 
                        const Vector3 &v1, 
                        const Vector3 &v2) 
{
  float     fMin0, fMax0, fMin1, fMax1;
  float     fD_C;
  Vector3 vcV, vcTriEdge[3], vcA[3];

  // just for looping
  vcA[0] = this->vcA0;
  vcA[1] = this->vcA1;
  vcA[2] = this->vcA2;

  // direction of tri-normals
  vcTriEdge[0] = v1 - v0;
  vcTriEdge[1] = v2 - v0;

  vcV = CrossProduct(vcTriEdge[0], vcTriEdge[1]);

  fMin0 = vcV.DotProduct(v0);
  fMax0 = fMin0;

  this->ObbProj((*this), vcV, &fMin1, &fMax1);
  if ( fMax1 < fMin0 || fMax0 < fMin1 )
    return true;

  // direction of obb planes
  // =======================
  // axis 1:
  vcV = this->vcA0;
  this->TriProj(v0, v1, v2, vcV, &fMin0, &fMax0);
  fD_C = vcV.DotProduct(this->vcCenter);
  fMin1 = fD_C - this->fA0;
  fMax1 = fD_C + this->fA0;
  if ( fMax1 < fMin0 || fMax0 < fMin1 )
    return true;

  // axis 2:
  vcV = this->vcA1;
  this->TriProj(v0, v1, v2, vcV, &fMin0, &fMax0);
  fD_C = vcV.DotProduct(this->vcCenter);
  fMin1 = fD_C - this->fA1;
  fMax1 = fD_C + this->fA1;
  if ( fMax1 < fMin0 || fMax0 < fMin1 )
    return true;

  // axis 3:
  vcV = this->vcA2;
  this->TriProj(v0, v1, v2, vcV, &fMin0, &fMax0);
  fD_C = vcV.DotProduct(this->vcCenter);
  fMin1 = fD_C - this->fA2;
  fMax1 = fD_C + this->fA2;
  if ( fMax1 < fMin0 || fMax0 < fMin1 )
    return true;


  // direction of tri-obb edge-crossproducts
  vcTriEdge[2] = vcTriEdge[1] - vcTriEdge[0];
  for (int j=0; j<3; j++) {
    for (int k=0; k<3; k++) {
      vcV = CrossProduct(vcTriEdge[j], vcA[k]);

      this->TriProj(v0, v1, v2, vcV, &fMin0, &fMax0);
      this->ObbProj((*this), vcV, &fMin1, &fMax1);
          
      if ( (fMax1 < fMin0) || (fMax0 < fMin1) )
      return true;
      }
    }

  return true;
}

/*----------------------------------------------------------------*/

// intersects ray, slaps method
bool VCNObb::Intersects(const VCNRay &Ray, float *t) 
{
  float e, f, t1, t2, temp;
  float tmin = -99999.9f, 
      tmax = +99999.9f;

  Vector3 vcP = this->vcCenter - Ray.m_vcOrig;

  // 1st slap
  e = this->vcA0.DotProduct(vcP);
  f = this->vcA0.DotProduct(Ray.m_vcDir);
  if (fabsf(f) > 0.00001f) {

    t1 = (e + this->fA0) / f;
    t2 = (e - this->fA0) / f;

    if (t1 > t2) { temp=t1; t1=t2; t2=temp; }
    if (t1 > tmin) tmin = t1;
    if (t2 < tmax) tmax = t2;
    if (tmin > tmax) return false;
    if (tmax < 0.0f) return false;
    }
  else if ( ((-e - this->fA0) > 0.0f) || ((-e + this->fA0) < 0.0f) )
    return false;

  // 2nd slap
  e = this->vcA1.DotProduct(vcP);
  f = this->vcA1.DotProduct(Ray.m_vcDir);
  if (fabsf(f) > 0.00001f) {

    t1 = (e + this->fA1) / f;
    t2 = (e - this->fA1) / f;

    if (t1 > t2) { temp=t1; t1=t2; t2=temp; }
    if (t1 > tmin) tmin = t1;
    if (t2 < tmax) tmax = t2;
    if (tmin > tmax) return false;
    if (tmax < 0.0f) return false;
    }
  else if ( ((-e - this->fA1) > 0.0f) || ((-e + this->fA1) < 0.0f) )
    return false;

  // 3rd slap
  e = this->vcA2.DotProduct(vcP);
  f = this->vcA2.DotProduct(Ray.m_vcDir);
  if (fabsf(f) > 0.00001f) {

    t1 = (e + this->fA2) / f;
    t2 = (e - this->fA2) / f;

    if (t1 > t2) { temp=t1; t1=t2; t2=temp; }
    if (t1 > tmin) tmin = t1;
    if (t2 < tmax) tmax = t2;
    if (tmin > tmax) return false;
    if (tmax < 0.0f) return false;
    }
  else if ( ((-e - this->fA2) > 0.0f) || ((-e + this->fA2) < 0.0f) )
    return false;

  if (tmin > 0.0f) {
    if (t) *t = tmin;
    return true;
    }

  if (t) *t = tmax;

  return true;
}

/*----------------------------------------------------------------*/

// intersects ray at certain length (line segment), slaps method
bool VCNObb::Intersects(const VCNRay &Ray, float fL, float *t) 
{
  float e, f, t1, t2, temp;
  float tmin = -99999.9f, 
      tmax = +99999.9f;

  Vector3 vcP = this->vcCenter - Ray.m_vcOrig;

  // 1st slap
  e = this->vcA0.DotProduct(vcP);
  f = this->vcA0.DotProduct(Ray.m_vcDir);
  if (fabsf(f) > 0.00001f) {

    t1 = (e + this->fA0) / f;
    t2 = (e - this->fA0) / f;

    if (t1 > t2) { temp=t1; t1=t2; t2=temp; }
    if (t1 > tmin) tmin = t1;
    if (t2 < tmax) tmax = t2;
    if (tmin > tmax) return false;
    if (tmax < 0.0f) return false;
    }
  else if ( ((-e - this->fA0) > 0.0f) || ((-e + this->fA0) < 0.0f) )
    return false;

  // 2nd slap
  e = this->vcA1.DotProduct(vcP);
  f = this->vcA1.DotProduct(Ray.m_vcDir);
  if (fabsf(f) > 0.00001f) {

    t1 = (e + this->fA1) / f;
    t2 = (e - this->fA1) / f;

    if (t1 > t2) { temp=t1; t1=t2; t2=temp; }
    if (t1 > tmin) tmin = t1;
    if (t2 < tmax) tmax = t2;
    if (tmin > tmax) return false;
    if (tmax < 0.0f) return false;
    }
  else if ( ((-e - this->fA1) > 0.0f) || ((-e + this->fA1) < 0.0f) )
    return false;

  // 3rd slap
  e = this->vcA2.DotProduct(vcP);
  f = this->vcA2.DotProduct(Ray.m_vcDir);
  if (fabsf(f) > 0.00001f) {

    t1 = (e + this->fA2) / f;
    t2 = (e - this->fA2) / f;

    if (t1 > t2) { temp=t1; t1=t2; t2=temp; }
    if (t1 > tmin) tmin = t1;
    if (t2 < tmax) tmax = t2;
    if (tmin > tmax) return false;
    if (tmax < 0.0f) return false;
    }
  else if ( ((-e - this->fA2) > 0.0f) || ((-e + this->fA2) < 0.0f) )
    return false;

  if ( (tmin > 0.0f) && (tmin <= fL) ) {
    if (t) *t = tmin;
    return true;
    }

  // intersection on line but not on segment
  if (tmax > fL) return false;

  if (t) *t = tmax;

  return true;
}

/*----------------------------------------------------------------*/

// intersects another obb
bool VCNObb::Intersects(const VCNObb &Obb) 
{
  float T[3];

  // difference vector between both obb
  Vector3 vcD = Obb.vcCenter - this->vcCenter;

  float matM[3][3];   // B's axis in relation to A
  float ra,           // radius A
      rb,           // radius B
      t;            // absolute values from T[]

  // Obb A's axis as separation axis?
  // ================================
  // first axis vcA0
  matM[0][0] = this->vcA0.DotProduct(Obb.vcA0);
  matM[0][1] = this->vcA0.DotProduct(Obb.vcA1);
  matM[0][2] = this->vcA0.DotProduct(Obb.vcA2);
  ra   = this->fA0;
  rb   = Obb.fA0 * fabsf(matM[0][0]) + 
      Obb.fA1 * fabsf(matM[0][1]) + 
      Obb.fA2 * fabsf(matM[0][2]);

  T[0] = vcD.DotProduct(this->vcA0);
  t    = fabsf(T[0]);
  if(t > (ra + rb) ) 
    return false;

  // second axis vcA1
  matM[1][0] = this->vcA1.DotProduct(Obb.vcA0);
  matM[1][1] = this->vcA1.DotProduct(Obb.vcA1);
  matM[1][2] = this->vcA1.DotProduct(Obb.vcA2);
  ra   = this->fA1;
  rb   = Obb.fA0 * fabsf(matM[1][0]) + 
      Obb.fA1 * fabsf(matM[1][1]) + 
      Obb.fA2 * fabsf(matM[1][2]);
  T[1] = vcD * this->vcA1;
  t    = fabsf(T[1]);
  if(t > (ra + rb) ) 
    return false;

  // third axis vcA2
  matM[2][0] = this->vcA2 * Obb.vcA0;
  matM[2][1] = this->vcA2 * Obb.vcA1;
  matM[2][2] = this->vcA2 * Obb.vcA2;
  ra   = this->fA2;
  rb   = Obb.fA0 * fabsf(matM[2][0]) + 
      Obb.fA1 * fabsf(matM[2][1]) + 
      Obb.fA2 * fabsf(matM[2][2]);
  T[2] = vcD * this->vcA2;
  t    = fabsf(T[2]);
  if(t > (ra + rb) ) 
    return false;

  // Obb B's axis as separation axis?
  // ================================
  // first axis vcA0
  ra = this->fA0 * fabsf(matM[0][0]) + 
    this->fA1 * fabsf(matM[1][0]) + 
    this->fA2 * fabsf(matM[2][0]);
  rb = Obb.fA0;
  t = fabsf( T[0]*matM[0][0] + T[1]*matM[1][0] + T[2]*matM[2][0] );
  if(t > (ra + rb) )
    return false;

  // second axis vcA1
  ra = this->fA0 * fabsf(matM[0][1]) + 
    this->fA1 * fabsf(matM[1][1]) + 
    this->fA2 * fabsf(matM[2][1]);
  rb = Obb.fA1;
  t = fabsf( T[0]*matM[0][1] + T[1]*matM[1][1] + T[2]*matM[2][1] );
  if(t > (ra + rb) )
    return false;

  // third axis vcA2
  ra = this->fA0 * fabsf(matM[0][2]) + 
    this->fA1 * fabsf(matM[1][2]) + 
    this->fA2 * fabsf(matM[2][2]);
  rb = Obb.fA2;
  t = fabsf( T[0]*matM[0][2] + T[1]*matM[1][2] + T[2]*matM[2][2] );
  if(t > (ra + rb) )
    return false;

  // other candidates: cross products of axis:
  // =========================================
  // axis A0xB0
  ra = this->fA1*fabsf(matM[2][0]) + this->fA2*fabsf(matM[1][0]);
  rb = Obb.fA1*fabsf(matM[0][2]) + Obb.fA2*fabsf(matM[0][1]);
  t = fabsf( T[2]*matM[1][0] - T[1]*matM[2][0] );
  if( t > ra + rb )
    return false;

  // axis A0xB1
  ra = this->fA1*fabsf(matM[2][1]) + this->fA2*fabsf(matM[1][1]);
  rb = Obb.fA0*fabsf(matM[0][2]) + Obb.fA2*fabsf(matM[0][0]);
  t = fabsf( T[2]*matM[1][1] - T[1]*matM[2][1] );
  if( t > ra + rb )
    return false;

  // axis A0xB2
  ra = this->fA1*fabsf(matM[2][2]) + this->fA2*fabsf(matM[1][2]);
  rb = Obb.fA0*fabsf(matM[0][1]) + Obb.fA1*fabsf(matM[0][0]);
  t = fabsf( T[2]*matM[1][2] - T[1]*matM[2][2] );
  if( t > ra + rb )
    return false;

  // axis A1xB0
  ra = this->fA0*fabsf(matM[2][0]) + this->fA2*fabsf(matM[0][0]);
  rb = Obb.fA1*fabsf(matM[1][2]) + Obb.fA2*fabsf(matM[1][1]);
  t = fabsf( T[0]*matM[2][0] - T[2]*matM[0][0] );
  if( t > ra + rb )
    return false;

  // axis A1xB1
  ra = this->fA0*fabsf(matM[2][1]) + this->fA2*fabsf(matM[0][1]);
  rb = Obb.fA0*fabsf(matM[1][2]) + Obb.fA2*fabsf(matM[1][0]);
  t = fabsf( T[0]*matM[2][1] - T[2]*matM[0][1] );
  if( t > ra + rb )
    return false;

  // axis A1xB2
  ra = this->fA0*fabsf(matM[2][2]) + this->fA2*fabsf(matM[0][2]);
  rb = Obb.fA0*fabsf(matM[1][1]) + Obb.fA1*fabsf(matM[1][0]);
  t = fabsf( T[0]*matM[2][2] - T[2]*matM[0][2] );
  if( t > ra + rb )
    return false;

  // axis A2xB0
  ra = this->fA0*fabsf(matM[1][0]) + this->fA1*fabsf(matM[0][0]);
  rb = Obb.fA1*fabsf(matM[2][2]) + Obb.fA2*fabsf(matM[2][1]);
  t = fabsf( T[1]*matM[0][0] - T[0]*matM[1][0] );
  if( t > ra + rb )
    return false;

  // axis A2xB1
  ra = this->fA0*fabsf(matM[1][1]) + this->fA1*fabsf(matM[0][1]);
  rb = Obb.fA0 *fabsf(matM[2][2]) + Obb.fA2*fabsf(matM[2][0]);
  t = fabsf( T[1]*matM[0][1] - T[0]*matM[1][1] );
  if( t > ra + rb )
    return false;

  // axis A2xB2
  ra = this->fA0*fabsf(matM[1][2]) + this->fA1*fabsf(matM[0][2]);
  rb = Obb.fA0*fabsf(matM[2][1]) + Obb.fA1*fabsf(matM[2][0]);
  t = fabsf( T[1]*matM[0][2] - T[0]*matM[1][2] );
  if( t > ra + rb )
    return false;

  // no separation axis found => intersection
  return true;
}

/*----------------------------------------------------------------*/

/**
 * Culls OBB to n sided frustrum. Normals pointing outwards.
 * -> IN:  VCNPlane   - array of planes building frustrum
 *         int        - number of planes in array
 *    OUT: VCNVISIBLE - obb totally inside frustrum
 *         VCNCLIPPED - obb clipped by frustrum
 *         VCNCULLED  - obb totally outside frustrum
 */
int VCNObb::Cull(const VCNPlane *pPlanes, int nNumPlanes) 
{
  Vector3 vN;
  int       nResult = VCNVISIBLE;
  float     fRadius, fTest;

  // for all planes
  for (int i=0; i<nNumPlanes; i++) {
    // frustrum normals pointing outwards, we need inwards
    vN = pPlanes[i].m_vcN * -1.0f;

    // calculate projected box radius
    fRadius = fabsf(fA0 * (vN * vcA0)) 
        + fabsf(fA1 * (vN * vcA1))
        + fabsf(fA2 * (vN * vcA2));

    // testvalue: (N*C - d) (#)
    fTest = vN * this->vcCenter - pPlanes[i].m_fD;

    // obb totally outside of at least one plane: (#) < -r
    if (fTest < -fRadius)
      return VCNCULLED;
    // or maybe intersecting this plane?
    else if (!(fTest > fRadius))
      nResult = VCNCLIPPED;
    } // for

  // if not culled then clipped or inside
  return nResult;
}

/*----------------------------------------------------------------*/

