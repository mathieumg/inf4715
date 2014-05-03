///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#ifndef VICUNA_POLYGON
#define VICUNA_POLYGON

#include "Types.h"

#include "Aabb.h"
#include "Plane.h"

//-------------------------------------------------------------
/// Vicuna's basic polygon class
//-------------------------------------------------------------
class VCNPolygon
{
  friend class VCNPlane;        // access for easier classifying

private:
  VCNPlane       m_Plane;    // plane which poly lies in

  VCNInt            m_NumP;     // number of points
  VCNInt            m_NumI;     // number of indices
  VCNAabb        m_Aabb;     // bounding box
  VCNUInt   m_Flag;     // whatever you want it to be

  void CalcBoundingBox(void);

  //---------------------------------------

public:
  VCNPolygon(void);
  ~VCNPolygon(void);


  Vector3    *m_pPoints;  // list of points
  VCNUInt *m_pIndis;   // index list

  void          Set(const Vector3 *pPoints, VCNInt nNumP, const VCNUInt *pIndis, VCNInt nNumI);

  void          Clip(const VCNPlane &Plane, VCNPolygon *pFront, VCNPolygon *pBack);
  void          Clip(const VCNAabb &aabb);
  VCNInt        Cull(const VCNAabb &aabb);

  void          CopyOf( const VCNPolygon &Poly );

  void          SwapFaces(void);

  VCNBool          Intersects(const VCNRay &Ray, VCNBool, VCNFloat *t);
  VCNBool          Intersects(const VCNRay &Ray, VCNBool, VCNFloat fL, VCNFloat *t);

  const VCNInt GetNumPoints() const{ return m_NumP;    }
  const VCNInt GetNumIndis() const { return m_NumI;    }
  Vector3*    GetPoints(void)         { return m_pPoints; }
  VCNUInt*    GetIndices(void)        { return m_pIndis;  }
  VCNPlane    GetPlane(void) const { return m_Plane;   }
  VCNAabb      GetAabb(void) const { return m_Aabb;    }
  VCNUInt      GetFlag(void) const { return m_Flag;    }
  void      SetFlag(VCNUInt n)    { m_Flag = n;       }
};


#endif