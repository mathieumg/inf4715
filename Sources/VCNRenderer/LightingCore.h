///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// Vicuna's Lighting Core
/// Core module for light management.
///

#ifndef VICUNA_LIGHTING_CORE
#define VICUNA_LIGHTING_CORE

#include "VCNCore/Core.h"
#include "VCNUtils/RenderTypes.h"

class VCNLight;
class VCNSphere;

class VCNLightingCore : public VCNCore<VCNLightingCore>
{
  VCN_CLASS;

public:
  VCNLightingCore(void);
  virtual ~VCNLightingCore(void);

  // Core module overloads
  virtual VCNBool Initialize() override;
  virtual VCNBool Uninitialize() override;

  // Create a light of a certain type
  VCNLight* CreateLight( const VCNLightType type, const VCNString& name );
  void AddLight(const VCNNodeID resID);
  void RemoveLight(const VCNNodeID resID);

  // Returns an array of lights intersecting with a sphere
  void IntersectLights( const VCNSphere& sphere, std::vector<VCNLight*>& lights );

  // Set ambient light level
  inline const VCNBool HasAmbientLight() const;
  inline const VCNColor& GetAmbientLight() const;
  
  void SetAmbientLight( VCNFloat red, VCNFloat green, VCNFloat blue );

  // Returns a indexed light
  VCNLight* GetLight(const VCNUInt idx) const;

protected:
   
  std::vector<VCNNodeID> mLights;
  VCNColor mAmbientLightColor;
};

//-------------------------------------------------------------
const VCNBool VCNLightingCore::HasAmbientLight() const
{
  return ( mAmbientLightColor.R>0.0f ||
    mAmbientLightColor.G>0.0f ||
    mAmbientLightColor.B>0.0f );
}

const VCNColor& VCNLightingCore::GetAmbientLight() const
{
  return mAmbientLightColor;
}

#endif