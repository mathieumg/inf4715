///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#include "Precompiled.h"

#include "EffectParamSet.h"
#include "VCNUtils/Assert.h"

VCN_BASETYPE( VCNEffectParamSet ) ;

//-------------------------------------------------------------
const VCNTChar* kEffectNames[] =
{
  VCNTXT("Basic"),
  VCNTXT("Lit"),
  VCNTXT("Textured"),
  VCNTXT("LitTextured"),
  VCNTXT("Terrain"),
  VCNTXT("ShadowMap"),
  VCNTXT("SSAO")
};

//-------------------------------------------------------------
/// Constructor
//-------------------------------------------------------------
VCNEffectParamSet::VCNEffectParamSet():
mEffectID( kNumEffects )
{
}

//-------------------------------------------------------------
/// Destructor
//-------------------------------------------------------------
VCNEffectParamSet::~VCNEffectParamSet()
{
}

//-------------------------------------------------------------
/// Reset
//-------------------------------------------------------------
void VCNEffectParamSet::Reset()
{
  // Reset the ID
  mEffectID = kNumEffects;

  // Empty the maps
  mStringParams.clear();
  mFloatParams.clear();
  mResourceParams.clear();
  mColorParams.clear();
}


//-------------------------------------------------------------
/// Sets the effect ID by the name
//-------------------------------------------------------------
void VCNEffectParamSet::SetEffectID( const VCNString& effectName )
{
  for( VCNUInt i=0; i<kNumEffects; i++ )
  {
    if( kEffectNames[i] == effectName )
    {
      mEffectID = static_cast<VCNEffectID>( i );
      return;
    }
  }

  VCN_ASSERT_FAIL( VCNTXT("Effect [%s] wasn't found"), effectName.c_str() );
}

//-------------------------------------------------------------
/// Returns a string param
//-------------------------------------------------------------
VCNString VCNEffectParamSet::GetString( const VCNStringHash& name ) const
{
  std::unordered_map<VCNStringHash,VCNString>::const_iterator fItr = mStringParams.find(name);
  if( fItr == mStringParams.end() )
  {
    VCN_ASSERT_FAIL( "Missing a string in this effect param!" );
    return VCNTXT("");
  }

  return fItr->second;
}

//-------------------------------------------------------------
/// Returns a float param
//-------------------------------------------------------------
VCNFloat VCNEffectParamSet::GetFloat( const VCNStringHash& name ) const
{
  std::unordered_map<VCNStringHash,VCNFloat>::const_iterator fItr = mFloatParams.find(name);
  if( fItr == mFloatParams.end() )
  {
    VCN_ASSERT_FAIL( "Missing a float in this effect param!" );
    return 0.0f;
  }

  return fItr->second;
}

//-------------------------------------------------------------
/// Returns a resource param
//-------------------------------------------------------------
VCNResID VCNEffectParamSet::GetResource(const VCNStringHash& name ) const
{
  std::unordered_map<VCNStringHash,VCNResID>::const_iterator fItr = mResourceParams.find(name);
  if( fItr == mResourceParams.end() )
  {
    VCN_ASSERT_FAIL( "Missing a resource in this effect param!" );
    return kInvalidResID;
  }

  return fItr->second;
}

//-------------------------------------------------------------
/// Returns a color param
//-------------------------------------------------------------
VCNColor VCNEffectParamSet::GetColor( const VCNStringHash& name ) const
{
  std::unordered_map<VCNStringHash,VCNColor>::const_iterator fItr = mColorParams.find(name);
  if( fItr == mColorParams.end() )
  {
    VCN_ASSERT( false && "Missing a color in this effect param!" );
    return VCNColor();
  }

  return fItr->second;
}

