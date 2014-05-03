///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#include "Precompiled.h"

#include "Material.h"

VCN_TYPE( VCNMaterial, VCNResource ) ;


//-------------------------------------------------------------
/// Constructor
//-------------------------------------------------------------
VCNMaterial::VCNMaterial() : VCNResource()
{
  Clear();
}

//-------------------------------------------------------------
/// Destructor
//-------------------------------------------------------------
VCNMaterial::~VCNMaterial()
{
}

//-------------------------------------------------------------
/// Clear the material
//-------------------------------------------------------------
void VCNMaterial::Clear()
{
  // Reset the colors
  memset( &mAmbientColor, 0, sizeof(VCNColor) );
  memset( &mDiffuseColor, 0, sizeof(VCNColor) );
  memset( &mSpecularColor, 0, sizeof(VCNColor) );
  mSpecularPower = 0.0f;

  // Reset the effect params
  mEffectParamSet.Reset();
  mEffectParamSet.AddFloat(VCNTXT("Selected"), 0.0f);
}

