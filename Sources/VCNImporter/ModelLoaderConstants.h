///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Constants only used while loading models
///

#ifndef VICUNA_MODEL_LOADER_CONSTANTS
#define VICUNA_MODEL_LOADER_CONSTANTS

const VCNFloat  kModelDefaultVersion      = 1.0f;

// File extensions
const VCNTChar* kXMLModelExtension        = VCNTXT(".MDL.xml");
const VCNTChar* kXModelExtension          = VCNTXT(".x");

// Nodes
const VCNTChar* kModelResourceNode        = VCNTXT("VCNModel");
const VCNTChar* kModelNode                = VCNTXT("Node");
const VCNTChar* kAttrModelNodeName        = VCNTXT("Name");
const VCNTChar* kAttrModelNodeType        = VCNTXT("Type");
const VCNTChar* kAttrModelNodePosX        = VCNTXT("posx");
const VCNTChar* kAttrModelNodePosY        = VCNTXT("posy");
const VCNTChar* kAttrModelNodePosZ        = VCNTXT("posz");
const VCNTChar* kAttrModelNodeRotX        = VCNTXT("qrotx");
const VCNTChar* kAttrModelNodeRotY        = VCNTXT("qroty");
const VCNTChar* kAttrModelNodeRotZ        = VCNTXT("qrotz");
const VCNTChar* kAttrModelNodeRotW        = VCNTXT("qrotw");
const VCNTChar* kAttrModelNodeScaleX      = VCNTXT("sclx");
const VCNTChar* kAttrModelNodeScaleY      = VCNTXT("scly");
const VCNTChar* kAttrModelNodeScaleZ      = VCNTXT("sclz");
const VCNTChar* kAttrModelNodeResource    = VCNTXT("Mesh");

// Render nodes have materials
const VCNTChar* kAttrModelNodeMaterial    = VCNTXT("Material");


#endif
