#ifndef GAMESCENECONSTANTS_H
#define GAMESCENECONSTANTS_H

///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Constants only used while loading/editing the game scene 
///        using the file GameScene.xml
///

// Instances Node types
static const VCNTChar* kXMLInstancesNodeName  = VCNTXT("Instances");

// Instances Node attributes
static const VCNTChar* kXMLInstancesName      = VCNTXT("name");
static const VCNTChar* kXMLInstancesOf        = VCNTXT("of");
static const VCNTChar* kXMLInstancesPosX      = VCNTXT("posx");
static const VCNTChar* kXMLInstancesPosY      = VCNTXT("posy");
static const VCNTChar* kXMLInstancesPosZ      = VCNTXT("posz");
static const VCNTChar* kXMLInstancesRotPitch  = VCNTXT("pitch");
static const VCNTChar* kXMLInstancesRotYaw    = VCNTXT("yaw");
static const VCNTChar* kXMLInstancesRotRoll   = VCNTXT("roll");
static const VCNTChar* kXMLInstancesSclX     = VCNTXT("sclx");
static const VCNTChar* kXMLInstancesSclY     = VCNTXT("scly");
static const VCNTChar* kXMLInstancesSclZ    = VCNTXT("sclz");

// Objects Node types
static const VCNTChar* kXMLObjects        = VCNTXT("Objects");

// Objects Node attributes
static const VCNTChar* kXMLObjectsName        = VCNTXT("name");
static const VCNTChar* kXMLObjectsModel     = VCNTXT("model");
static const VCNTChar* kXMLObjectsMesh       = VCNTXT("mesh");
static const VCNTChar* kXMLObjectsMat      = VCNTXT("material");

// File names
static const VCNString kEmptyHouseModelFileName  = VCNTXT("1stFloor.MDL");
static const VCNString kFurnituresConfigFileName = VCNTXT("FurnituresDefinitions.XML");
static const VCNString kModelsInstancesFileName  = VCNTXT("SceneInstances.xml");

static const VCNString kModelFileExtension       = VCNTXT(".MDL.xml");

#endif // GAMESCENECONSTANTS_H
