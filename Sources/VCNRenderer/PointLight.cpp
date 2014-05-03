///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#include "Precompiled.h"
#include "PointLight.h"

#include "XformCore.h"

#include "VCNNodes/NodeCore.h"

//-------------------------------------------------------------
/// Constructor
//-------------------------------------------------------------
VCNPointLight::VCNPointLight( VCNNodeID nodeID ) 
: VCNLight(nodeID)
, mRange(100.0f)
, mMaxRange(120.0f)
, mConstantAttenuation(1.0f)
, mLinearAttenuation(0.1f)
, mQuadAttenuation(0.01f)
{
}

//-------------------------------------------------------------
/// Destructor
//-------------------------------------------------------------
VCNPointLight::~VCNPointLight()
{
}

//-------------------------------------------------------------
/// Do we light this sphere
//-------------------------------------------------------------
VCNBool VCNPointLight::Intersects( const VCNSphere& sphere ) const
{
  return mSphere.Intersects( sphere );
}

///////////////////////////////////////////////////////////////////////
const bool VCNPointLight::UpdateWorldTransform()
{
  if ( VCNLight::UpdateWorldTransform() )
  {
    mSphere.Set(mMaxRange, GetWorldTranslation());

    return true;
  }

  return false;
}


///////////////////////////////////////////////////////////////////////
VCNPointLight* VCNPointLight::Copy() const
{
  VCNPointLight* nodeCopy = VCNNodeCore::GetInstance()->CreateNode<VCNPointLight>();
  CopyTo( nodeCopy );
  return nodeCopy;
}


///////////////////////////////////////////////////////////////////////
void VCNPointLight::CopyTo(VCNPointLight* node) const
{
  VCNLight::CopyTo( node );

  node->mRange = this->mRange;
  node->mMaxRange = this->mMaxRange;
  node->mConstantAttenuation = this->mConstantAttenuation;
  node->mLinearAttenuation = this->mLinearAttenuation;
  node->mQuadAttenuation = this->mQuadAttenuation;
  node->mSphere = this->mSphere;
}

///////////////////////////////////////////////////////////////////////
VCNLightType VCNPointLight::GetLightType() const
{
  return LT_OMNI;
}
