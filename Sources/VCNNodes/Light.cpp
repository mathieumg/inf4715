///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#include "Precompiled.h"
#include "Light.h"

#include "VCNRenderer/LightingCore.h"
#include "VCNRenderer/XformCore.h"

VCN_TYPE( VCNLight, VCNNode ) ;


///////////////////////////////////////////////////////////////////////
VCNLight::VCNLight( VCNNodeID nodeID ) 
: VCNNode(nodeID)
, mOn( true )
, mOffLock(false)
{
  mDiffuse.R = mDiffuse.G = mDiffuse.B = mDiffuse.A = 1.0f;
  mSpecular.R = mSpecular.G = mSpecular.B = mSpecular.A = 1.0f;
}


///////////////////////////////////////////////////////////////////////
VCNLight::~VCNLight()
{
  // Remove it from the lighting manager if it was registered
  VCNLightingCore* lightingCore = VCNLightingCore::GetInstance();
  if ( lightingCore )
  {
    lightingCore->RemoveLight( GetNodeID() );
  }
}


///////////////////////////////////////////////////////////////////////
void VCNLight::CopyTo(VCNLight* node) const
{
  VCNNode::CopyTo( node );

  node->mOn = this->mOn;
  node->mOffLock = this->mOffLock;
  node->mDiffuse = this->mDiffuse;
  node->mSpecular = this->mSpecular;

  VCNLightingCore::GetInstance()->RemoveLight(GetNodeID());
  VCNLightingCore::GetInstance()->AddLight(node->GetNodeID());
}

///////////////////////////////////////////////////////////////////////
Matrix4 VCNLight::GetProjectionMatrix() const
{
  return VCNXformCore::GetInstance()->GetProjectionMatrix();
}

///////////////////////////////////////////////////////////////////////
Matrix4 VCNLight::GetViewMatrix() const
{
  return Matrix4();
}
