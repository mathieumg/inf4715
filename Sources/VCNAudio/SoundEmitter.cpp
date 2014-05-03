///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Abstract sound emitter implementation.
///

#include "Precompiled.h"
#include "SoundEmitter.h"

VCN_TYPE( VCNSoundEmitter, VCNResource ) ;

///////////////////////////////////////////////////////////////////////
///
/// Constructs the sound emitter.
///
VCNSoundEmitter::VCNSoundEmitter() 
  : VCNResource()
  , mVolume(1.0f)
  , mFrequency(44000.0f)
  , mPan(0.0f)
  , mLoop(true)
{
}

///////////////////////////////////////////////////////////////////////
///
/// Destruct the sound emitter.
///
VCNSoundEmitter::~VCNSoundEmitter()
{
}
