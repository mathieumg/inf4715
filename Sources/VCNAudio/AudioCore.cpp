///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Abstract audio module implementation.
///

#include "Precompiled.h"
#include "AudioCore.h"

// Project includes
#include "VCNAudio/SoundEmitter.h"

// Engine includes
#include "VCNResources/ResourceCore.h"
#include "VCNUtils/Constants.h"
#include "VCNUtils/Macros.h"

VCN_TYPE( VCNAudioCore, VCNCore<VCNAudioCore> ) ;

///////////////////////////////////////////////////////////////////////
///
/// Constructs the audio core.
///
VCNAudioCore::VCNAudioCore()
  : mSoundPath( VCNTXT("Sounds/") )
{
}

///////////////////////////////////////////////////////////////////////
///
/// Destructs the audio core.
///
VCNAudioCore::~VCNAudioCore(void) 
{
}

///////////////////////////////////////////////////////////////////////
///
/// Plays a sound.
/// 
/// @param sndID [IN] sound id to be played
/// @param loop  [IN] sets the loop attribute before playing.
///
void VCNAudioCore::Play(VCNResID sndID, VCNBool loop)
{
  VCNSoundEmitter* emitter = VCNResourceCore::GetInstance()->GetResource<VCNSoundEmitter>( sndID );
  if ( emitter )
  {
    emitter->SetLoop( loop );
    emitter->Play();
  }
  else
  {
    INFO( VCNTXT("Sound [%d] can't be played, cause the resource wasn't found."), sndID );
  }
}

void VCNAudioCore::SetListenerPosition(Vector3& position, Vector3& lookAt)
{
}

void VCNAudioCore::RegisterGameObj(VCNUInt objectId, const VCNString& helperName)
{
}

void VCNAudioCore::SetPosition(VCNUInt objectId, Vector3& position, Vector3& orientation)
{
}

void VCNAudioCore::SetAttenuationScalingFactor(VCNUInt objectId, VCNFloat scale)
{
}

void VCNAudioCore::PostEvent(const VCNString& eventName, VCNUInt gameObjectID)
{
}

void VCNAudioCore::SetState(const VCNString& stateName, const VCNString& stateValue)
{
}

void VCNAudioCore::SetSwitch(const VCNString& switchName, const VCNString& switchValue, VCNUInt gameObjectID)
{
}

VCNUInt VCNAudioCore::GetUniqueGameObjectID()
{
	static VCNUInt idCount = 0;
	return idCount++;
}
