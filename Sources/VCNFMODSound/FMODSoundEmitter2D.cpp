///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief FMOD 2D sound implementation.
///

#include "Precompiled.h"
#include "FMODSoundEmitter2D.h"

// Project includes
#include "VCNFMODSound/FMODAudioCore.h"

// Engine includes
#include "VCNUtils/Utilities.h"

///////////////////////////////////////////////////////////////////////
///
/// Constructs the sound emitter.
/// 
/// @param FMODSystem  [IN] FMOD system pointer for quick access.
///
VCNFMOD2DSoundEmitter::VCNFMOD2DSoundEmitter(FMOD::System* FMODSystem) 
  : VCN2DSoundEmitter()
  , mFMODSystem(FMODSystem)
  , mSound(NULL)
  , mChannel(NULL)
{
}

///////////////////////////////////////////////////////////////////////
///
/// Release sounds resources.
///
VCNFMOD2DSoundEmitter::~VCNFMOD2DSoundEmitter()
{
  mSound->release();
}

///////////////////////////////////////////////////////////////////////
///
/// Loads the sound into the FMOD system.
/// 
/// @param filename  [IN] filename on the sound on disk.
/// @param playback  [IN] playback mode
///
/// @return true if the file was successfully loaded, other false is returned.
///
const VCNBool VCNFMOD2DSoundEmitter::LoadSound(const VCNString& filename, PlaybackType playback)
{
  // Add the sound path in front
  VCNString fullName = VCNAudioCore::GetInstance()->GetSoundPath();
  fullName = fullName + filename;

  const bool fileExists = VCN::FileExists(fullName);
  VCN_ASSERT_MSG( fileExists, VCNTXT("Sound file [%s] doesn't exists."), filename.c_str() );
  if ( fileExists )
  {
    // Load the sound file as a 2D sound
    FMOD_RESULT result = FMOD_OK;
    if (playback == Playback_Stream)
    {
      result = mFMODSystem->createStream( VCN_TCHAR_TO_ASCII(fullName), FMOD_2D, 0, &mSound );    
    }
    else
    {
      result = mFMODSystem->createSound( VCN_TCHAR_TO_ASCII(fullName), FMOD_2D, 0, &mSound );
    }
    
    // Verify if the load was successful
    VCNFMODAudioCore::FMODASSERT(result);

    if ( result == FMOD_OK )
    {
      mSound->getDefaults(&mFrequency, &mVolume, &mPan, NULL); 
    }

    return result == FMOD_OK;
  }

  return false;
}

///////////////////////////////////////////////////////////////////////
///
/// Plays the sound.
///
/// @return true if the sound can be played, otherwise false is returned.
///
const VCNBool VCNFMOD2DSoundEmitter::Play()
{
  if ( !mSound )
    return false;

  // Check if we should resume first.
  if ( mChannel && IsPlaying() && IsPaused() )
    mChannel->setPaused( false );

  // Set the looping
  mSound->setMode( mLoop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF );
  mSound->setDefaults(mFrequency, mVolume, mPan, 128);

  // Play the sound
  FMOD_RESULT result = mFMODSystem->playSound(FMOD_CHANNEL_REUSE, mSound, false, &mChannel);
  VCNFMODAudioCore::FMODASSERT(result);

  return result == FMOD_OK;
}

///////////////////////////////////////////////////////////////////////
///
/// Stops the sounds.
/// @return success value.
///
const VCNBool VCNFMOD2DSoundEmitter::Stop()
{
  if ( mSound == NULL || mChannel == NULL )
    return false;

  FMOD_RESULT result = mChannel->stop();
  return result == FMOD_OK;
}

///////////////////////////////////////////////////////////////////////
///
/// Updates sound values if they have changed.
///
void VCNFMOD2DSoundEmitter::Update()
{
  if ( mSound == NULL )
    return;

  // Set the looping
  mSound->setMode( mLoop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF );
  mSound->setDefaults(mFrequency, mVolume, mPan, 128);
}

///////////////////////////////////////////////////////////////////////
///
/// Checks if the sound if currently playing.
///
const VCNBool VCNFMOD2DSoundEmitter::IsPlaying() const
{
  if ( mChannel == NULL )
    return false;

  bool isplaying = false;
  mChannel->isPlaying(&isplaying);
  return isplaying;
}

///////////////////////////////////////////////////////////////////////
///
/// Updates the channel volume if the volume setting changes.
///
void VCNFMOD2DSoundEmitter::OnVolumeChanged()
{
  if (!mChannel)
    return;

  mChannel->setVolume(mVolume);
}

///////////////////////////////////////////////////////////////////////
const VCNBool VCNFMOD2DSoundEmitter::IsPaused() const 
{
  if ( !mChannel )
    return false;

  bool paused = false;
  mChannel->getPaused(&paused);

  return paused;
}

///////////////////////////////////////////////////////////////////////
const VCNBool VCNFMOD2DSoundEmitter::Pause()
{
  if ( !mChannel )
    return false;

  mChannel->setPaused( true );

  return true;
}
