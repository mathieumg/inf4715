///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// Constants only used while loading materials
///

#ifndef VCNMATERIALLOADERCONSTANTS_H
#define VCNMATERIALLOADERCONSTANTS_H

#pragma once

// File extensions
const VCNTChar* kXMLMaterialExtension               = VCNTXT("MAT.xml");

// Base colors
const VCNTChar* kMultipleMaterialsNode              = VCNTXT("Materials");
const VCNTChar* kMaterialNode                       = VCNTXT("Material");
const VCNTChar* kNodeMaterialAmbientColor           = VCNTXT("AmbientColor");
const VCNTChar* kNodeMaterialDiffuseColor           = VCNTXT("DiffuseColor");
const VCNTChar* kNodeMaterialSpecularColor          = VCNTXT("SpecularColor");

// Colors elements
const VCNTChar* kAttrMaterialColorR                 = VCNTXT("r");
const VCNTChar* kAttrMaterialColorG                 = VCNTXT("g");
const VCNTChar* kAttrMaterialColorB                 = VCNTXT("b");
const VCNTChar* kAttrMaterialColorA                 = VCNTXT("a");
const VCNTChar* kAttrMaterialPower                  = VCNTXT("power");

// Effect node
const VCNTChar* kNodeMaterialEffect                 = VCNTXT("Effect");
const VCNTChar* kAttrMaterialEffectName             = VCNTXT("name");

// The types of effect param nodes
const VCNTChar* kNodeMaterialEffectFloatParam       = VCNTXT("FloatParam");
const VCNTChar* kNodeMaterialEffectStringParam      = VCNTXT("StringParam");
const VCNTChar* kNodeMaterialEffectTextureParam     = VCNTXT("TextureParam");
const VCNTChar* kNodeMaterialEffectTextureCubeParam = VCNTXT("TextureCubeParam");
const VCNTChar* kNodeMaterialEffectColorParam       = VCNTXT("ColorParam");

const VCNTChar* kAttrMaterialEffectParamName        = VCNTXT("name");
const VCNTChar* kAttrMaterialEffectParamValue       = VCNTXT("value");

#endif // VCNMATERIALLOADERCONSTANTS_H
