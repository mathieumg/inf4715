///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// The Vicuna Material resource base class.
///

#ifndef VICUNA_MATERIAL
#define VICUNA_MATERIAL

#include "Resource.h"

#include "EffectParamSet.h"

//-------------------------------------------------------------
// The material class
//-------------------------------------------------------------
class VCNMaterial : public VCNResource
{
  VCN_CLASS;

public:

  VCNMaterial();
  virtual ~VCNMaterial();

  void Clear();

  // Accessors for all the colors of the material
  const VCNColor& GetAmbientColor() const;
  void SetAmbientColor( const VCNColor& color );

  const VCNColor& GetDiffuseColor() const;
  void SetDiffuseColor( const VCNColor& color );

  const VCNColor& GetSpecularColor() const;
  void SetSpecularColor( const VCNColor& color );

  float GetSpecularPower() const;
  void SetSpecularPower( float power );

  // Allow the exterior to manipulate the effect params
  VCNEffectParamSet& GetEffectParamSet();
  const VCNEffectParamSet& GetEffectParamSet() const;

protected:

  // Basic colors
  VCNColor  mAmbientColor;
  VCNColor  mDiffuseColor;
  VCNColor  mSpecularColor;
  VCNFloat  mSpecularPower;

  // Effect params
  VCNEffectParamSet mEffectParamSet;
};

//-------------------------------------------------------------
inline const VCNColor& VCNMaterial::GetAmbientColor() const
{
  return mAmbientColor;
}

//-------------------------------------------------------------
inline void VCNMaterial::SetAmbientColor( const VCNColor& color )
{
  mAmbientColor = color;
}

//-------------------------------------------------------------
inline const VCNColor& VCNMaterial::GetDiffuseColor() const
{
  return mDiffuseColor;
}

//-------------------------------------------------------------
inline void VCNMaterial::SetDiffuseColor( const VCNColor& color )
{
  mDiffuseColor = color;
}

//-------------------------------------------------------------
inline const VCNColor& VCNMaterial::GetSpecularColor() const
{
  return mSpecularColor;
}

//-------------------------------------------------------------
inline void VCNMaterial::SetSpecularColor( const VCNColor& color )
{
  mSpecularColor = color;
}

//-------------------------------------------------------------
inline float VCNMaterial::GetSpecularPower() const
{
  return mSpecularPower;
}

//-------------------------------------------------------------
inline void VCNMaterial::SetSpecularPower( float power )
{
  mSpecularPower = power;
}

//-------------------------------------------------------------
inline VCNEffectParamSet& VCNMaterial::GetEffectParamSet()
{
  return mEffectParamSet;
}

//-------------------------------------------------------------
inline const VCNEffectParamSet& VCNMaterial::GetEffectParamSet() const
{
  return mEffectParamSet;
}
#endif
