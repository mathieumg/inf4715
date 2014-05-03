///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
///

#include "Precompiled.h"
#include "PhysxUtils.h"

#include "VCNUtils/Sphere.h"
#include "VCNUtils/Quaternion.h"
#include "VCNUtils/Vector.h"

///////////////////////////////////////////////////////////////////////
PxTransform VCNPhysxUtils::ToTransform(const Matrix4& mat)
{
  const Vector3& p = mat.GetTranslation();
  VCNQuat q; q.SetFromMatrix( mat.GetRotation().GetTranspose() );
      
  return PxTransform( (const PxVec3&)p, (const PxQuat&)q );
}


///////////////////////////////////////////////////////////////////////
PxTransform VCNPhysxUtils::ToTransform(const VCNSphere& sphere)
{
  PxTransform pxTransform( V2V<PxVec3>(sphere.GetCenter()) );
  return pxTransform;
}
