///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// The Vicuna Effect Param Set. It holds params to configure
/// an effect for rendering.
///

#ifndef VICUNA_EFFECT_PARAM_SET
#define VICUNA_EFFECT_PARAM_SET

#include "Resource.h"
#include "VCNUtils/RenderTypes.h"
#include "VCNUtils/StringHash.h"
#include "VCNUtils/Types.h"

//-------------------------------------------------------------
// The effect param set class
//-------------------------------------------------------------
class VCNEffectParamSet
{
  VCN_CLASS;

public:

  VCNEffectParamSet();
  virtual ~VCNEffectParamSet();

  // Reset
  void Reset();

  // Accessors
  void AddString( const VCNStringHash& name, const VCNString& value );
  VCNString GetString( const VCNStringHash& name ) const;

  void AddFloat( const VCNStringHash& name, VCNFloat value );
  VCNFloat GetFloat( const VCNStringHash& name ) const;

  void AddResource( const VCNStringHash& name, VCNResID value );
  VCNResID GetResource( const VCNStringHash& name ) const;

  void AddColorParam( const VCNStringHash& name, const VCNColor& value );
  VCNColor GetColor( const VCNStringHash& name ) const;

  bool IsValid() const;
  VCNEffectID GetEffectID() const;
  void SetEffectID( VCNEffectID effectID );
  void SetEffectID( const VCNString& effectName );

protected:

  // Target shader for these params
  VCNEffectID mEffectID;

  // Maps to hold the param names with their values
  std::unordered_map<VCNStringHash,VCNString> mStringParams;
  std::unordered_map<VCNStringHash,VCNFloat>  mFloatParams;
  std::unordered_map<VCNStringHash,VCNResID>  mResourceParams;
  std::unordered_map<VCNStringHash,VCNColor>  mColorParams;
};

//-------------------------------------------------------------
inline void VCNEffectParamSet::AddString( const VCNStringHash& name, const VCNString& value )
{
  mStringParams[name] = value;
}

//-------------------------------------------------------------
inline void VCNEffectParamSet::AddFloat( const VCNStringHash& name, VCNFloat value )
{
  mFloatParams[name] = value;
}

//-------------------------------------------------------------
inline void VCNEffectParamSet::AddResource( const VCNStringHash& name, VCNResID value )
{
  mResourceParams[name] = value;
}

//-------------------------------------------------------------
inline void VCNEffectParamSet::AddColorParam( const VCNStringHash& name, const VCNColor& value )
{
  mColorParams[name] = value;
}

//-------------------------------------------------------------
inline void VCNEffectParamSet::SetEffectID( VCNEffectID effectID )
{
  mEffectID = effectID;
}

//-------------------------------------------------------------
inline VCNEffectID VCNEffectParamSet::GetEffectID() const
{
  return mEffectID;
}

//-------------------------------------------------------------
inline bool VCNEffectParamSet::IsValid() const
{
  return (mEffectID != kNumEffects);
}


#endif
