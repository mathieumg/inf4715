///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Vicuna FMOD 3D Sound Emitter
///

#ifndef VICUNA_FMOD_3D_SOUND_EMITTER
#define VICUNA_FMOD_3D_SOUND_EMITTER

#pragma once

#include "VCNAudio/SoundEmitter3D.h"

class VCNFMOD3DSoundEmitter : public VCN3DSoundEmitter
{
public:

  /// Default constructor
  VCNFMOD3DSoundEmitter(FMOD::System* FMODSystem, const Vector3& soundPosition);

  /// Default destructor.
  virtual ~VCNFMOD3DSoundEmitter();

  /// Load a sound from a file
  virtual const VCNBool LoadSound(const VCNString& filename, PlaybackType playback) override;

  /// Check if the sound is already playing
  virtual const VCNBool IsPlaying() const;

  /// Checks if the sound is paused.
  virtual const VCNBool IsPaused() const override;

  /// Plays the sound
  virtual const VCNBool Play() override;

  /// Pause the sound.
  virtual const VCNBool Pause() override;
  
  /// Stops the sound
  virtual const VCNBool Stop() override;

  /// Update the sounds values
  virtual void Update();

private:

// Events

  /// Called when the position changes.
  virtual void OnPositionChanged();

  /// Called when the min and max distances change.
  virtual void OnMinMaxDistanceChanged();

// Data members

  /// FMOD system access
  FMOD::System* mFMODSystem;

  /// FMOD sound
  FMOD::Sound* mSound;

  /// FMOD channel
  FMOD::Channel* mChannel;
};

#endif
