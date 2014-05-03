///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Abstract 3d sound emitter implementation.
////

#include "Precompiled.h"
#include "SoundEmitter3D.h"

///////////////////////////////////////////////////////////////////////
///
/// Constructs the 3D sound emitter.
///
VCN3DSoundEmitter::VCN3DSoundEmitter() 
  : VCNSoundEmitter()
  , mMinDistance(1.0f)
  , mMaxDistance(10000.0f)
{
}

///////////////////////////////////////////////////////////////////////
///
/// Destructs the 3D sound emitter.
///
VCN3DSoundEmitter::~VCN3DSoundEmitter()
{
}
