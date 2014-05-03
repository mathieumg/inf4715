///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// The Vicuna Omni Directional Light class.
///
/// @TODO: Rename to VCNPointLight
///

#ifndef VICUNA_OMNI_LIGHT
#define VICUNA_OMNI_LIGHT

#include "VCNNodes/Light.h"
#include "VCNUtils/Sphere.h"


//-------------------------------------------------------------
// The omni light class
//-------------------------------------------------------------
class VCNPointLight : public VCNLight
{
public:

  VCNPointLight( VCNNodeID nodeID );
  virtual ~VCNPointLight();

  // Accessors
  void SetRange( const VCNFloat range );
  VCNFloat GetRange() const;

  void SetMaxRange( const VCNFloat range );
  VCNFloat GetMaxRange() const;

  void SetConstantAttenuation( const VCNFloat att );
  const VCNFloat GetConstantAttenuation() const;

  void SetLinearAttenuation( const VCNFloat att );
  const VCNFloat GetLinearAttenuation() const;

  void SetQuadAttenuation( const VCNFloat att );
  const VCNFloat GetQuadAttenuation() const;

  // To be overloaded by specific light classes
  virtual VCNLightType GetLightType() const override;

  // Tells us if a sphere is affect by this light
  virtual VCNBool Intersects( const VCNSphere& sphere ) const override;

  /// Updates light sphere
  virtual const bool UpdateWorldTransform() override;

protected:

  // Copy fields to specified node.
  void CopyTo(VCNPointLight* node) const;

  // Copy this node and return the copy
  virtual VCNPointLight* Copy() const override;

// Data members

  /// Distance within which we light fully
  VCNFloat  mRange;               

  /// Distance after which we do not light at all
  VCNFloat  mMaxRange;            

  VCNFloat  mConstantAttenuation;
  VCNFloat  mLinearAttenuation;
  VCNFloat  mQuadAttenuation;
  VCNSphere mSphere;
};

//-------------------------------------------------------------
inline void VCNPointLight::SetRange( const VCNFloat range )
{
  mRange = range;
}

//-------------------------------------------------------------
inline VCNFloat VCNPointLight::GetRange() const
{
  return mRange;
}

//-------------------------------------------------------------
inline void VCNPointLight::SetMaxRange( const VCNFloat range )
{
  mMaxRange = range;
	mComputeTransforms = true;
}

//-------------------------------------------------------------
inline VCNFloat VCNPointLight::GetMaxRange() const
{
  return mMaxRange;
}

//-------------------------------------------------------------
inline void VCNPointLight::SetConstantAttenuation( const VCNFloat att )
{
  mConstantAttenuation = att;
}

//-------------------------------------------------------------
inline const VCNFloat VCNPointLight::GetConstantAttenuation() const
{
  return mConstantAttenuation;
}

//-------------------------------------------------------------
inline void VCNPointLight::SetLinearAttenuation( const VCNFloat att )
{
  mLinearAttenuation = att;
}

//-------------------------------------------------------------
inline const VCNFloat VCNPointLight::GetLinearAttenuation() const
{
  return mLinearAttenuation;
}

//-------------------------------------------------------------
inline void VCNPointLight::SetQuadAttenuation( const VCNFloat att )
{
  mQuadAttenuation = att;
}

//-------------------------------------------------------------
inline const VCNFloat VCNPointLight::GetQuadAttenuation() const
{
  return mQuadAttenuation;
}


#endif
