///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// Constants only used while loading scene configuration
///

#ifndef VICUNA_SCENE_CONFIG_LOADER_CONSTANTS
#define VICUNA_SCENE_CONFIG_LOADER_CONSTANTS

// Node types
const VCNTChar* kXMLConfigGamepay       = VCNTXT("Gameplay");
const VCNTChar* kXMLConfigStress        = VCNTXT("Stress");
const VCNTChar* kXMLConfigPhysics       = VCNTXT("Physics");
const VCNTChar* kXMLConfigSound         = VCNTXT("Sound");

// Gameplay attributes
const VCNTChar* kXMLConfigHealth        = VCNTXT("health");

// Physics attributes
const VCNTChar* kXMLConfigGravity       = VCNTXT("gravity");

// Sounds attributes
const VCNTChar* kXMLConfigVolume        = VCNTXT("volume");
const VCNTChar* kXMLConfigSounds        = VCNTXT("sounds");
const VCNTChar* kXMLConfigMusic         = VCNTXT("music");
const VCNTChar* kXMLConfigUnit          = VCNTXT("world_unit");

#endif