///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#include "Precompiled.h"
#include "DirectionalLight.h"

#include "VCNNodes/NodeCore.h"
#include "VCNRenderer/XformCore.h"


//-------------------------------------------------------------
/// Constructor
//-------------------------------------------------------------
VCNDirectionalLight::VCNDirectionalLight( VCNNodeID nodeID ) 
  : VCNLight(nodeID)
  , mDirection()
{
}


//-------------------------------------------------------------
/// Destructor
//-------------------------------------------------------------
VCNDirectionalLight::~VCNDirectionalLight()
{
}


//-------------------------------------------------------------
/// Do we light this sphere
//-------------------------------------------------------------
VCNBool VCNDirectionalLight::Intersects( const VCNSphere& sphere ) const
{
  // We light everything!
  return true;
}


////////////////////////////////////////////////////////////////////////
Matrix4 VCNDirectionalLight::GetViewMatrix() const
{
  VCN_ASSERT_FAIL( "Must be overridden" );
  return Matrix4();
}


///////////////////////////////////////////////////////////////////////
Matrix4 VCNDirectionalLight::GetProjectionMatrix() const 
{
  VCN_ASSERT_FAIL( "Must be overridden" );
  return Matrix4();
}


///////////////////////////////////////////////////////////////////////
VCNDirectionalLight* VCNDirectionalLight::Copy() const
{
  VCNDirectionalLight* nodeCopy = VCNNodeCore::GetInstance()->CreateNode<VCNDirectionalLight>();
  CopyTo( nodeCopy );
  return nodeCopy;
}


///////////////////////////////////////////////////////////////////////
void VCNDirectionalLight::CopyTo(VCNDirectionalLight* node) const
{
  VCNLight::CopyTo( node );

  node->mDirection = this->mDirection;
}

///////////////////////////////////////////////////////////////////////
VCNLightType VCNDirectionalLight::GetLightType() const
{
  return LT_DIRECTIONAL;
}
