///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Abstract sound emitter interface
////

#ifndef VCNSOUNDEMITTER_H
#define VCNSOUNDEMITTER_H

#pragma once

#include "VCNResources/Resource.h"

/// Defines how the sound gets loaded.
enum PlaybackType
{
  Playback_Sound,
  Playback_Stream
};

///
/// The Vicuna Base Sound Emitter Resource.
///
class VCNSoundEmitter : public VCNResource
{
  VCN_CLASS;

public:
  
  /// Default destructor.
  virtual ~VCNSoundEmitter();

// Attributes

  /// Returns the volume of the emitter
  inline const VCNFloat GetVolume() const;

  /// Sets the volume of the emitter
  inline void SetVolume(const VCNFloat vol);

  // Gets the sound frequency
  inline const VCNFloat GetFrequency() const;
  
  /// Sets the emitter frequency.
  inline void SetFrequency(const VCNFloat freq);

  /// Returns the emitter pan
  inline const VCNFloat GetPan() const;

  /// Sets the emitter pan
  inline void SetPan(const VCNFloat pan);

  /// Checks if the emitter loops.
  inline const VCNBool IsLoop() const;

  /// Sets the if emitter should loop.
  inline void SetLoop(const VCNBool loop);

// Interface

  /// Loads a sound from a file
  virtual const VCNBool LoadSound(const VCNString& filename, PlaybackType pb) = 0;

  /// Check if the sound is already playing
  virtual const VCNBool IsPlaying() const = 0;

  /// Checks if the sound is paused.
  virtual const VCNBool IsPaused() const = 0;

  /// Plays the sound
  virtual const VCNBool Play() = 0;

  /// Sets the sound to pause mode.
  virtual const VCNBool Pause() = 0;

  /// Stops the sound.
  virtual const VCNBool Stop() = 0;

  /// Update the sounds values
  /// @TODO: Removes and use event setting change notifications
  virtual void Update() = 0;

protected:

  /// Default constructor.
  VCNSoundEmitter();

  /// Called when the volume changed
  virtual void OnVolumeChanged() {}

  /// Called when the frequency changed
  virtual void OnFrequencyChanged() {}

  /// Called when the sound panning changed.
  virtual void OnPanChanged() {}

  /// Called when the loop setting changes.
  virtual void OnLoopChanged() {}

// Shared members

  /// Volume of the sound
  VCNFloat mVolume;

  // Every sound has a frequency
  VCNFloat mFrequency;

  /// Pan between left/right
  VCNFloat mPan;

  // Loop the sound
  VCNBool mLoop;
};

///
/// INLINES
///

///////////////////////////////////////////////////////////////////////
const VCNFloat VCNSoundEmitter::GetVolume() const
{
  return mVolume;
}

///////////////////////////////////////////////////////////////////////
void VCNSoundEmitter::SetVolume(const VCNFloat vol)
{
  mVolume = vol;
  OnVolumeChanged();
}

///////////////////////////////////////////////////////////////////////
const VCNFloat VCNSoundEmitter::GetFrequency() const
{
  return mFrequency;
}

///////////////////////////////////////////////////////////////////////
void VCNSoundEmitter::SetFrequency(const VCNFloat freq)
{
  mFrequency = freq;
  OnFrequencyChanged();
}

///////////////////////////////////////////////////////////////////////
const VCNFloat VCNSoundEmitter::GetPan() const
{
  return mPan;
}

///////////////////////////////////////////////////////////////////////
void VCNSoundEmitter::SetPan(const VCNFloat pan)
{
  mPan = pan;
  OnPanChanged();
}

///////////////////////////////////////////////////////////////////////
const VCNBool VCNSoundEmitter::IsLoop() const
{
  return mLoop;
}

///////////////////////////////////////////////////////////////////////
void VCNSoundEmitter::SetLoop(const VCNBool loop)
{
  mLoop = loop;
  OnLoopChanged();
}

#endif
