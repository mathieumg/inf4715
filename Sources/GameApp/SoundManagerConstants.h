///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
///  @brief Constants only used while loading/editing the game scene using 
///         the file GameScene.xml
///

#ifndef SOUNDS_LOADER_CONSTANTS
#define SOUNDS_LOADER_CONSTANTS

// File name
static const VCNString kSoundsFileName                  = VCNTXT("Sounds/Sounds.xml");
static const VCNTChar* kSoundsRootNodeName              = VCNTXT("Sounds");

// Node properties
static const VCNTChar* kSoundIDPropertyName             = VCNTXT("ID");
static const VCNTChar* kSoundFilenamePropertyName       = VCNTXT("Filename");
static const VCNTChar* kSoundVolumePropertyName         = VCNTXT("Volume");
static const VCNTChar* kSoundLoopPropertyName           = VCNTXT("Loop");
static const VCNTChar* kSoundDimensionPropertyName      = VCNTXT("Dimension");
static const VCNTChar* kSoundTypePropertyName           = VCNTXT("Type");

// Defines some various sound types
static const VCNString kAmbientSoundPropertyName        = VCNTXT("Ambient");
static const VCNString kMenu2DSoundPropertyName         = VCNTXT("Menu");

// Used to define a sound made by the player... i.e: breathing, speaking, etc
static const VCNString kPlayerSoundPropertyName         = VCNTXT("Player");

// Used to define a sound that can be triggered
static const VCNString kTriggerSoundPropertyName        = VCNTXT("Triggered");

// Used to define a sound that is played for collision
static const VCNString kContactSoundPropertyName        = VCNTXT("Contact");
static const VCNString kContactObjectTagName            = VCNTXT("Object");
static const VCNTChar* kContactObjectPropertyName       = VCNTXT("name");

// Used to define the position of a 3D sound emitter
static const VCNTChar* k3DSoundPosXPropertyName         = VCNTXT("PositionX");
static const VCNTChar* k3DSoundPosYPropertyName         = VCNTXT("PositionY");
static const VCNTChar* k3DSoundPosZPropertyName         = VCNTXT("PositionZ");
static const VCNTChar* k3DSoundMinDistancePropertyName  = VCNTXT("MinDistance");
static const VCNTChar* k3DSoundMaxDistancePropertyName  = VCNTXT("MaxDistance");

// Defines game sound IDs
static const VCNTChar* kWind1SoundID                    = VCNTXT("Wind1");
static const VCNTChar* kCreditsSongID                   = VCNTXT("CreditsTrack");

#endif // SOUNDS_LOADER_CONSTANTS