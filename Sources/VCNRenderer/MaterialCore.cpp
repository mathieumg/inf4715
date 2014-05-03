///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#include "Precompiled.h"
#include "MaterialCore.h"

#include "VCNResources/ResourceCore.h"


VCN_TYPE( VCNMaterialCore, VCNCore<VCNMaterialCore> ) ;

//-------------------------------------------------------------
/// Constructor
//-------------------------------------------------------------
VCNMaterialCore::VCNMaterialCore():
mCurrentMaterial( kInvalidResID )
{
}

//-------------------------------------------------------------
/// Destructor
//-------------------------------------------------------------
VCNMaterialCore::~VCNMaterialCore()
{
}

//-------------------------------------------------------------
/// Initialize
//-------------------------------------------------------------
VCNBool VCNMaterialCore::Initialize()
{
  if ( !BaseCore::Initialize() )
    return false;

  // Create a default material
  VCNColor pinkColor;
  pinkColor.R = 1.0f; pinkColor.B = 1.0f;
  mDefaultMaterial.SetAmbientColor( pinkColor );
  mDefaultMaterial.SetDiffuseColor( pinkColor );

  return true;
}


///////////////////////////////////////////////////////////////////////
VCNBool VCNMaterialCore::Uninitialize()
{
  return BaseCore::Uninitialize();
}

//-------------------------------------------------------------
// Returns the current material
//-------------------------------------------------------------
VCNMaterial* VCNMaterialCore::GetCurrentMaterial()
{
  if( mCurrentMaterial == kInvalidResID )
    return NULL;

  // Go fetch the material
  VCNMaterial* material = VCNResourceCore::GetInstance()->GetResource<VCNMaterial>( mCurrentMaterial );
  return material;
}

///////////////////////////////////////////////////////////////////////
void VCNMaterialCore::LoadTexture(const VCNString& filename)
{
  CreateTexture( filename );
}
