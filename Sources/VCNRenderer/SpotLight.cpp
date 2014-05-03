///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#include "Precompiled.h"
#include "SpotLight.h"

#include "VCNNodes/NodeCore.h"
#include "VCNUtils/Sphere.h"

const Vector3 VCNSpotLight::SPOT_DIRECTION = Vector3( 1.0f, 0.0f, 0.0f );

//-------------------------------------------------------------
/// Constructor
//-------------------------------------------------------------
VCNSpotLight::VCNSpotLight( VCNNodeID nodeID ) : VCNLight(nodeID)
, mFalloff(20.0f)
, mMaxRange(1000.0f)
, mPhi(30.0f)
, mTheta(45.0f)
, mConstantAttenuation(0.0001f)
, mLinearAttenuation(0.0001f)
, mQuadAttenuation(0.0001f)
{
}

//-------------------------------------------------------------
/// Destructor
//-------------------------------------------------------------
VCNSpotLight::~VCNSpotLight()
{
}

//-------------------------------------------------------------
/// Do we light this sphere
//-------------------------------------------------------------
VCNBool VCNSpotLight::Intersects( const VCNSphere& sphere ) const
{
  const Vector3 pos = this->mLocalTranslation;
  VCNSphere ourLight( mMaxRange, pos );
  const VCNBool inSphere = sphere.Intersects( ourLight );

  VCNBool inCone = false;
  if (inSphere)
  {  
    VCNFloat coneSin = sin(GetTheta());
    VCNFloat coneCos = cos(GetTheta());
    VCNFloat sphereRadius = sphere.GetRadius();
    const Vector3& spherePos = sphere.GetCenter();
    const Vector3& conePoint = pos;
    const Vector3& coneAxis = GetDirection();
    Vector3 U = conePoint - (sphereRadius/coneSin)*coneAxis;
    Vector3 D = spherePos - U;
    if ( coneAxis.DotProduct(D) >= D.Length()*coneCos )
    {
      // center is inside K’’
      D = spherePos - conePoint;
      if ( -coneAxis.DotProduct(D) >= D.Length()*coneSin )
      {
        // center is inside K’’ and inside K’
        return D.Length() <= sphereRadius;
      }
      else
      {
        // center is inside K’’ and outside K’
        inCone = true;
      }
    }
    else
    {
      // center is outside K’’
      inCone = false;
    }
  }

  return inSphere && inCone;
}

//////////////////////////////////////////////////////////////////////////
Matrix4 VCNSpotLight::GetViewMatrix() const
{
  return mView;
}

///////////////////////////////////////////////////////////////////////
const bool VCNSpotLight::UpdateWorldTransform()
{
  if ( VCNNode::UpdateWorldTransform() )
  {
    //const Vector3 pos = GetWorldTransformation().GetTranslation();
    //mView.SetLookAt(pos, pos + GetDirection(), Vector3(0, 1, 0));
  
    return true;
  }

  return false;
}


///////////////////////////////////////////////////////////////////////
VCNSpotLight* VCNSpotLight::Copy() const
{
  VCNSpotLight* nodeCopy = VCNNodeCore::GetInstance()->CreateNode<VCNSpotLight>();
  CopyTo( nodeCopy );
  return nodeCopy;
}


///////////////////////////////////////////////////////////////////////
void VCNSpotLight::CopyTo(VCNSpotLight* node) const
{
  VCNLight::CopyTo( node );

  node->mDirection = this->mDirection;
  node->mFalloff = this->mFalloff;
  node->mMaxRange = this->mMaxRange;
  node->mPhi = this->mPhi;
  node->mTheta = this->mTheta;
  node->mConstantAttenuation = this->mLinearAttenuation;
  node->mLinearAttenuation = this->mLinearAttenuation;
  node->mQuadAttenuation = this->mQuadAttenuation;
  node->mView = this->mView;
}

///////////////////////////////////////////////////////////////////////
VCNLightType VCNSpotLight::GetLightType() const
{
  return LT_SPOT;
}

void VCNSpotLight::Process( const float elapsedTime )
{
	Matrix4 worldRotationMatrix = GetWorldRotation();
	Vector3 down(0.0f, -1.0f, 0.0f);
	Vector3 resultDirection = down * worldRotationMatrix;
	SetDirection(resultDirection.Normalized());
}
