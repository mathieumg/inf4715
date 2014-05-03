///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// Vicuna's Material Core
/// Core module for material management.
///

#ifndef VICUNA_MATERIAL_CORE
#define VICUNA_MATERIAL_CORE

#include "VCNCore/Core.h"
#include "VCNResources/Material.h"

class VCNMaterial;

class VCNMaterialCore : public VCNCore<VCNMaterialCore>
{
  VCN_CLASS;

public:

  VCNMaterialCore();
  virtual ~VCNMaterialCore();

  /// Return the current material
  VCNResID GetCurrentMaterialID() const;
  
  /// Returns the current material pointer
  VCNMaterial* GetCurrentMaterial();

  /// API specific method to select a material
  void SelectMaterial(VCNResID matID);

  /// Initialize the module
  virtual VCNBool Initialize() override;

  /// Release the module
  virtual VCNBool Uninitialize() override;

  /// Load a texture to the resources manager.
  virtual void LoadTexture(const VCNString& filename);

  /// We'll host the function to create textures...
  virtual VCNResID CreateTexture(const VCNString& filename) = 0;
  
  /// Creates a cubic texture
  virtual VCNResID CreateCubeTexture(const VCNString& filename) = 0;

  /// Creates a custom texture
  virtual VCNResID CreateTexture(VCNUInt width, VCNUInt height, VCNUInt32 mipLevel, VCNTextureFlags flags) = 0;

protected:
    
  VCNMaterial mDefaultMaterial;
  VCNResID    mCurrentMaterial;
};

//-------------------------------------------------------------
inline VCNResID VCNMaterialCore::GetCurrentMaterialID() const
{
  return mCurrentMaterial;
}

//-------------------------------------------------------------
inline void VCNMaterialCore::SelectMaterial( VCNResID matID )
{
  mCurrentMaterial = matID;
}

#endif