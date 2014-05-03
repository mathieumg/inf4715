///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Vicuna 2D Sound Emitter
///

#ifndef VCNFMOD2DSOUNDEMITTER_H
#define VCNFMOD2DSOUNDEMITTER_H

#pragma once

#include "VCNAudio/SoundEmitter2D.h"

///
/// FMOD 2D sound
///
class VCNFMOD2DSoundEmitter : public VCN2DSoundEmitter
{
public:

  /// Constructs emitter.
  VCNFMOD2DSoundEmitter(FMOD::System* const FMODSystem);

  /// Default destructor.
  virtual ~VCNFMOD2DSoundEmitter();

// Interface 

  /// Returns the sound channel
  inline FMOD::Channel* GetChannel() const;

// VCNSoundEmitter Interface

  /// Loads a sound from a file
  virtual const VCNBool LoadSound(const VCNString& filename, PlaybackType pb) override;

  /// Check if the sound is already playing
  virtual const VCNBool IsPlaying() const override;

  /// Checks if the sound is paused.
  virtual const VCNBool IsPaused() const override;

  /// Plays sound
  virtual const VCNBool Play() override;

  /// Pause the sound.
  virtual const VCNBool Pause() override;

  /// Stops sound
  virtual const VCNBool Stop() override;

  /// Updates the sounds values
  virtual void Update() override;
  
private:

// Events

  /// Called when the volume changes.
  virtual void OnVolumeChanged();

// Data members

  /// FMOD system pointer
  FMOD::System* mFMODSystem;

  // FMOD sound
  FMOD::Sound* mSound;

  // FMOD channel
  FMOD::Channel* mChannel;
};

///
/// INLINES
///

///////////////////////////////////////////////////////////////////////
inline FMOD::Channel* VCNFMOD2DSoundEmitter::GetChannel() const 
{
  return mChannel;
}

#endif // VCNFMOD2DSOUNDEMITTER_H
