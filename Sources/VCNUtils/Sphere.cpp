///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#include "Precompiled.h"
#include "Sphere.h"

//-------------------------------------------------------------
/// Constructor
//-------------------------------------------------------------
VCNSphere::VCNSphere()
:mRadius( 0.0f )
,mCenter( 0.0f, 0.0f, 0.0f )
{
}


//-------------------------------------------------------------
/// Constructor
//-------------------------------------------------------------
VCNSphere::VCNSphere( VCNFloat radius )
  : mRadius(radius)
  , mCenter( 0.0f, 0.0f, 0.0f )
{
}


//-------------------------------------------------------------
/// Constructor
//-------------------------------------------------------------
VCNSphere::VCNSphere( VCNFloat radius, const Vector3& center )
  : mRadius(radius)
  , mCenter(center)
{
}

 
//-------------------------------------------------------------
/// Constructor
//-------------------------------------------------------------
VCNBool VCNSphere::Intersects( const VCNSphere &sphere ) const
{
  VCNFloat dist2 = (mCenter - sphere.mCenter).SquareLength();
  VCNFloat sum = (mRadius + sphere.mRadius);
  return (dist2 < sum*sum);
}

///////////////////////////////////////////////////////////////////////
void VCNSphere::Set(VCNFloat radius, const Vector3& center)
{
  mRadius = radius;
  mCenter = center;
}
