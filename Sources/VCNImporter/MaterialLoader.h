///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// This class contains all the facilities to read materials
/// from files (XML or BIN) and to write materials to files
/// (BIN).
///

#ifndef VICUNA_MATERIAL_LOADER
#define VICUNA_MATERIAL_LOADER

#pragma once

#include "VCNImporter/XMLLoader.h"
#include "VCNUtils/RenderTypes.h"

// Forward declarations
class VCNMaterial;
class VCNEffectParamSet;

class VCNMaterialLoader : public VCNXMLLoader
{

public:

  typedef std::vector<VCNMaterial*> MaterialList;

  static void LoadMaterial( const VCNString& filename );

protected:

  // Functions relating to XML loading...
  static void  LoadMultipleMaterialXML( const VCNString& filename , MaterialList& outMaterialsList);

  // Load a single material from a given node.
  static VCNMaterial*  LoadMaterialXML( XMLNodePtr node );

  // Loads up a color
  static VCNColor LoadMaterialColorXML( XMLNodePtr node );

  // Loads up a texture
  static VCNResID LoadMaterialTexture( const VCNString& filename);
  static VCNResID LoadMaterialCubeTexture( const VCNString& filename);

  // Loads up the effect params
  static void LoadMaterialEffectNode( XMLNodePtr node, VCNEffectParamSet& effectParams );

  // Help to read the params of an effect node
  static void LoadTextureParams( XMLNodePtr node, VCNEffectParamSet& effectParams );
  static void LoadCubeTextureParams( XMLNodePtr node, VCNEffectParamSet& effectParams );
  static void LoadStringParams( XMLNodePtr node, VCNEffectParamSet& effectParams );
  static void LoadFloatParams( XMLNodePtr node, VCNEffectParamSet& effectParams );
  static void LoadColorParams( XMLNodePtr node, VCNEffectParamSet& effectParams );

protected:
  // This utility class should not be instanced.
  VCNMaterialLoader() {}
  virtual ~VCNMaterialLoader() {}

};

#endif
