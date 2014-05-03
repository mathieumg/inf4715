///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// The Vicuna Directional Light class.
///

#ifndef VICUNA_DIRECTIONAL_LIGHT
#define VICUNA_DIRECTIONAL_LIGHT

#pragma once

#include "VCNNodes/Light.h"

//-------------------------------------------------------------
// The directional light class
//-------------------------------------------------------------
class VCNDirectionalLight : public VCNLight
{
public:

  VCNDirectionalLight( VCNNodeID nodeID );
  virtual ~VCNDirectionalLight();

  /// Sets the light direction
  void SetDirection( const Vector3& dir );

  /// Gets the light direction
  const Vector3 GetDirection() const;

  // To be overloaded by specific light classes
  virtual VCNLightType GetLightType() const override;

  // Tells us if a sphere is affect by this light
  virtual VCNBool Intersects(const VCNSphere& sphere) const override;

  // Returns the view matrix of the directional light
  virtual Matrix4 GetViewMatrix() const override;

  /// Returns the light project matrix, by default it is the same as the camera view
  virtual Matrix4 GetProjectionMatrix() const override;

protected:

  // Copy fields to specified node.
  void CopyTo(VCNDirectionalLight* node) const;

  // Copy this node and return the copy
  virtual VCNDirectionalLight* Copy() const;

private:

// Data members

  // Direction of our directional light
  Vector3  mDirection;

};

//-------------------------------------------------------------

inline void VCNDirectionalLight::SetDirection( const Vector3& dir )
{
  mDirection = dir;
}

inline const Vector3 VCNDirectionalLight::GetDirection() const
{
  return mDirection;
}


#endif
