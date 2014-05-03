///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief FMOD 3D sound implementation
///

#include "Precompiled.h"
#include "FMODSoundEmitter3D.h"

// Project includes
#include "FMODAudioCore.h"

///////////////////////////////////////////////////////////////////////
///
/// Constructs the 3D sound.
/// 
/// @param FMODSystem  [IN] FMOD system pointer
/// @param position    [IN] startup 3D position
///
VCNFMOD3DSoundEmitter::VCNFMOD3DSoundEmitter(FMOD::System* FMODSystem, const Vector3& position) 
  : VCN3DSoundEmitter()
  , mFMODSystem(FMODSystem)
  , mSound(NULL)
  , mChannel(NULL)
{
  mPosition = position;
}

//-------------------------------------------------------------
/// Destructor
//-------------------------------------------------------------
VCNFMOD3DSoundEmitter::~VCNFMOD3DSoundEmitter()
{
  mSound->release();
}


///////////////////////////////////////////////////////////////////////
///
/// Loads a sound into the FMOD system.
/// 
/// @param filename  [IN] filename on disk to be loaded
/// @param playback  [IN] playback mode
///
/// @return true if the sound was loaded successfully.
///
const VCNBool VCNFMOD3DSoundEmitter::LoadSound(const VCNString& filename, PlaybackType playback)
{
  // Add the sound path in front
  VCNString fullName = VCNAudioCore::GetInstance()->GetSoundPath();
  fullName = fullName + filename;

  // Convert from wstring to string
  std::string soundFile(fullName.begin(), fullName.end());

  // Load the sound file as a 3D sound
  FMOD_RESULT result = FMOD_OK;
  if (playback == Playback_Stream)
  {
    result = mFMODSystem->createStream(soundFile.c_str(), FMOD_3D, 0, &mSound);    
  }
  else
  {
    result = mFMODSystem->createSound(soundFile.c_str(), FMOD_3D, 0, &mSound);    
  }

  // Verify if the load was successful
  VCNFMODAudioCore::FMODASSERT(result);

  return true;
}

///////////////////////////////////////////////////////////////////////
///
/// 
/// 
///
/// @return none
///
const VCNBool VCNFMOD3DSoundEmitter::Play()
{
  if ( !mSound )
    return false;

  // Check if we should resume first.
  if ( mChannel && IsPlaying() && IsPaused() )
    mChannel->setPaused( false );

  // Set the looping
  if(mLoop)
    mSound->setMode(FMOD_LOOP_NORMAL);
  else
    mSound->setMode(FMOD_LOOP_OFF);  

  // Play the sound
  FMOD_RESULT result = mFMODSystem->playSound(FMOD_CHANNEL_FREE, mSound, false, &mChannel);
  VCNFMODAudioCore::FMODASSERT(result);

  // PAUSE!!!

  // Set the position
  mChannel->set3DAttributes((const FMOD_VECTOR*)&mPosition, 0);

  // Set the volume
  mChannel->setVolume(mVolume/100.0f);

  // Set the frequency
  if(mFrequency != -1)
    mChannel->setFrequency(mFrequency/1.0f);

  return true;
}

///////////////////////////////////////////////////////////////////////
///
/// 
/// 
///
/// @return none
///
const VCNBool VCNFMOD3DSoundEmitter::Stop()
{
  if (mChannel)
  {
    mChannel->stop();
    return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////////
///
/// 
/// 
///
/// @return none
///
void VCNFMOD3DSoundEmitter::Update()
{
  if ( mSound == NULL )
    return;

  // Set the looping
  mSound->setMode( mLoop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF );
  mSound->setDefaults(mFrequency, mVolume, mPan, 128);
}

///////////////////////////////////////////////////////////////////////
///
/// 
/// 
///
/// @return none
///
const VCNBool VCNFMOD3DSoundEmitter::IsPlaying() const
{
  if (!mChannel)
    return false;

  bool isplaying = false;
  mChannel->isPlaying(&isplaying);
  return isplaying;
}

///////////////////////////////////////////////////////////////////////
///
/// 
/// 
///
/// @return none
///
void VCNFMOD3DSoundEmitter::OnPositionChanged()
{
  if ( mChannel )
  {
    FMOD_VECTOR soundPosition;
    soundPosition.x = mPosition.x;
    soundPosition.y = mPosition.y;
    soundPosition.z = mPosition.z;

    mChannel->set3DAttributes(&soundPosition, 0);
  }
}


///////////////////////////////////////////////////////////////////////
///
/// 
/// 
///
/// @return none
///
void VCNFMOD3DSoundEmitter::OnMinMaxDistanceChanged()
{
  if ( mSound )
  {
    mSound->set3DMinMaxDistance(mMinDistance, mMaxDistance);
  }
}

///////////////////////////////////////////////////////////////////////
const VCNBool VCNFMOD3DSoundEmitter::IsPaused() const 
{
  if ( !mChannel )
    return false;

  bool paused = false;
  mChannel->getPaused(&paused);
  return paused;
}

///////////////////////////////////////////////////////////////////////
const VCNBool VCNFMOD3DSoundEmitter::Pause()
{
  if ( !mChannel )
    return false;

  mChannel->setPaused( true );
  return true;
}
