///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// The Vicuna Spotlight class.
///

#ifndef VICUNA_SPOT_LIGHT
#define VICUNA_SPOT_LIGHT

#include "VCNNodes/Light.h"


//-------------------------------------------------------------
// The spotlight class
//-------------------------------------------------------------
class VCNSpotLight : public VCNLight
{
  static const Vector3 SPOT_DIRECTION;

public:

  VCNSpotLight( VCNNodeID nodeID );
  virtual ~VCNSpotLight();

  // To be overloaded by specific light classes
  virtual VCNLightType GetLightType() const override;

  // Tells us if a sphere is affect by this light
  virtual VCNBool Intersects(const VCNSphere& sphere) const override;

  // Returns the view matrix of the spot light
  virtual Matrix4 GetViewMatrix() const override;

  // Accessors
  inline void SetDirection( const Vector3& dir );
  inline Vector3 GetDirection() const;

  inline void SetFalloff( const VCNFloat range );
  inline VCNFloat GetFalloff() const;

  inline void SetMaxRange( const VCNFloat range );
  inline VCNFloat GetMaxRange() const;

  inline void SetPhi( const VCNFloat angle );
  inline VCNFloat GetPhi() const;

  inline void SetTheta( const VCNFloat angle );
  inline VCNFloat GetTheta() const;

  inline void SetConstantAttenuation( const VCNFloat att );
  inline const VCNFloat GetConstantAttenuation() const;

  inline void SetLinearAttenuation( const VCNFloat att );
  inline const VCNFloat GetLinearAttenuation() const;

  inline void SetQuadAttenuation( const VCNFloat att );
  inline const VCNFloat GetQuadAttenuation() const;

protected:

  // Copy field to specified node.
  void CopyTo(VCNSpotLight* node) const;

  // Copy this node and return the copy
  virtual VCNSpotLight* Copy() const override;

  /// Updates transforms
  virtual const bool UpdateWorldTransform() override;

  virtual void Process( const float elapsedTime );

// Data members

  // Spotlight direction
  Vector3   mDirection;   

  // Distance within which we light fully
  VCNFloat  mFalloff;      

  // Distance after which we do not light at all
  VCNFloat  mMaxRange; 

  // Angle within which we light fully
  VCNFloat  mPhi;      

  // Angle after which we do not light at all
  VCNFloat  mTheta;    

  VCNFloat  mConstantAttenuation;
  VCNFloat  mLinearAttenuation;
  VCNFloat  mQuadAttenuation;
  Matrix4   mView;
};

//-------------------------------------------------------------
inline void VCNSpotLight::SetDirection( const Vector3& dir )
{
  mDirection = Vector3(dir.x, dir.y, dir.z);
}

//-------------------------------------------------------------
inline Vector3 VCNSpotLight::GetDirection() const
{
  return mDirection;
}

//-------------------------------------------------------------
inline void VCNSpotLight::SetFalloff( const VCNFloat range )
{
  mFalloff = range;
}

//-------------------------------------------------------------
inline VCNFloat VCNSpotLight::GetFalloff() const
{
  return mFalloff;
}

//-------------------------------------------------------------
inline void VCNSpotLight::SetMaxRange( const VCNFloat range )
{
  mMaxRange = range;
}

//-------------------------------------------------------------
inline VCNFloat VCNSpotLight::GetMaxRange() const
{
  return mMaxRange;
}

//-------------------------------------------------------------
inline void VCNSpotLight::SetPhi( const VCNFloat angle )
{
  mPhi = angle;
}

//-------------------------------------------------------------
inline VCNFloat VCNSpotLight::GetPhi() const
{
  return mPhi;
}

//-------------------------------------------------------------
inline void VCNSpotLight::SetTheta( const VCNFloat angle )
{
  mTheta = angle;
}

//-------------------------------------------------------------
inline VCNFloat VCNSpotLight::GetTheta() const
{
  return mTheta;
}

//-------------------------------------------------------------
inline void VCNSpotLight::SetConstantAttenuation( const VCNFloat att )
{
  mConstantAttenuation = att;
}

//-------------------------------------------------------------
inline const VCNFloat VCNSpotLight::GetConstantAttenuation() const
{
  return mConstantAttenuation;
}

//-------------------------------------------------------------
inline void VCNSpotLight::SetLinearAttenuation( const VCNFloat att )
{
  mLinearAttenuation = att;
}

//-------------------------------------------------------------
inline const VCNFloat VCNSpotLight::GetLinearAttenuation() const
{
  return mLinearAttenuation;
}

//-------------------------------------------------------------
inline void VCNSpotLight::SetQuadAttenuation( const VCNFloat att )
{
  mQuadAttenuation = att;
}

//-------------------------------------------------------------
inline const VCNFloat VCNSpotLight::GetQuadAttenuation() const
{
  return mQuadAttenuation;
}

#endif
