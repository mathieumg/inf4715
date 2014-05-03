///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// The Vicuna Light resource base class.
///

#ifndef VICUNA_LIGHT
#define VICUNA_LIGHT

#pragma once

// TODO: Move to VCNRenderer

#include "VCNNodes/Node.h"
#include "VCNUtils/RenderTypes.h"

class VCNSphere;

///
/// The light abstract class
///
class VCNLight : public VCNNode
{
  VCN_CLASS;

public:

  // Destructor
  virtual ~VCNLight();

  // Accessors
  void SetDiffuse( const VCNColor& diffuse );
  VCNColor GetDiffuse() const;
  void SetSpecular( const VCNColor& specular );
  VCNColor GetSpecular() const;

  // Accessors for the light's state
  VCNBool IsOn() const;
  VCNBool IsOffLockActivated() const;
  void TurnOn();
  void TurnOff(VCNBool definitely = false);
  void RemoveOffLock();

  // To be overloaded by specific light classes
  virtual VCNLightType GetLightType() const = 0;

  // Tells us if a sphere is affect by this light
  virtual VCNBool Intersects(const VCNSphere& sphere) const = 0;

  // Computes the view matrix of the light
  virtual Matrix4 GetViewMatrix() const;

  /// Returns the light project matrix, by default it is the same as the camera view
  virtual Matrix4 GetProjectionMatrix() const;

protected:

  /// Default destructor
  VCNLight(VCNNodeID nodeID);

  // Copy fields to specified node.
  void CopyTo(VCNLight* node) const;

// Data members

  // The On/Off switch!
  VCNBool   mOn;              // Is the light active
  VCNBool   mOffLock;         // Tells if we can reopen the light
                              // during the game. If true, we can't.

  // Parameters any light should have
  VCNColor  mDiffuse;         // RGBA diffuse light value
  VCNColor  mSpecular;        // RGBA specular light value
};

//-------------------------------------------------------------
inline void VCNLight::SetDiffuse( const VCNColor& diffuse )
{
  mDiffuse = diffuse;
}

//-------------------------------------------------------------
inline VCNColor VCNLight::GetDiffuse() const
{
  return mDiffuse;
}

//-------------------------------------------------------------
inline void VCNLight::SetSpecular( const VCNColor& specular )
{
  mSpecular = specular;
}

//-------------------------------------------------------------
inline VCNColor VCNLight::GetSpecular() const
{
  return mSpecular;
}

//-------------------------------------------------------------
inline VCNBool VCNLight::IsOn() const
{
  return mOn;
}

//-------------------------------------------------------------
inline VCNBool VCNLight::IsOffLockActivated() const
{
  return mOffLock;
}

//-------------------------------------------------------------
inline void VCNLight::TurnOn()
{
  if (!mOffLock)
  {
    mOn = true;
  }
}

//-------------------------------------------------------------
inline void VCNLight::TurnOff(VCNBool offLock)
{
  mOn = false;
  mOffLock = offLock;
}

//-------------------------------------------------------------
inline void VCNLight::RemoveOffLock()
{
  mOffLock = false;
}

#endif
