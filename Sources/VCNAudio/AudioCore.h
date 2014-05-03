///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Abstract Audio Core interface
///

#ifndef VCNAUDIOCORE_H
#define VCNAUDIOCORE_H

#pragma once

// Project includes
#include "VCNAudio/SoundEmitter.h"

// Engine includes
#include "VCNCore/Core.h"
#include "VCNUtils/Types.h"
#include "VCNUtils/Vector.h"

///
/// Abstract audio core.
///
class VCNAudioCore : public VCNCore<VCNAudioCore>
{
  VCN_CLASS;

public:

// Predefined types

  /// Default destructor.
  virtual ~VCNAudioCore(void);

// Attributes

  /// Gets the sound path.
  inline const VCNString& GetSoundPath() const;

  /// Changes the path to load sounds from.
  inline void SetSoundPath(const VCNString& path);

// Interface

  /// Sets world 3D settings
  virtual void Set3DSettings(VCNFloat distanceFactor) = 0;

  /// Sets viewer settings.
  virtual void SetViewerSettings(const Vector3& playerPosition, const Vector3& velocity, 
                                 const Vector3& forwardDirection, const Vector3& upDirection) = 0;

  /// Creates a 2D sound.
  virtual const VCNResID Create2DSoundEmitter(const VCNString& sndID, const VCNString& filename, 
    const VCNBool loop = false, const VCNFloat volume = 1.0f, PlaybackType playback = Playback_Stream) = 0;  
  
  /// Creates a 3D sound.
  virtual const VCNResID Create3DSoundEmitter(const VCNString& sndID, const VCNString& filename, 
    const Vector3& position, const VCNFloat minDistance, const VCNFloat maxDistance = 10000.0f,
    const VCNBool loop = false, const VCNFloat volume = 1.0f, PlaybackType playback = Playback_Stream) = 0;

  /// Play a sound
  virtual void Play(VCNResID sndID, VCNBool loop);

  /// Set position of listener in game world
  virtual void SetListenerPosition(Vector3& position, Vector3& lookAt);

  /// Register a game object with the sound engine
  virtual void RegisterGameObj(VCNUInt objectId, const VCNString& helperName);

  /// Set the position of a game object in the game world
  virtual void SetPosition(VCNUInt objectId, Vector3& position, Vector3& orientation);

  /// Set attenuation based on distance of game object
  virtual void SetAttenuationScalingFactor(VCNUInt objectId, VCNFloat scale);
  
  /// Trigger a sound event
  virtual void PostEvent(const VCNString& eventName, VCNUInt gameObjectID);

  /// Change a state
  virtual void SetState(const VCNString& stateName, const VCNString& stateValue);

  /// Change a switch
  virtual void SetSwitch(const VCNString& switchName, const VCNString& switchValue, VCNUInt gameObjectID);

  /// Stop all audio at once
  virtual void StopAll() = 0;
  
  /// Cleans up the audio core.
  virtual void DestroyAll() = 0;


  static VCNUInt GetUniqueGameObjectID();
protected:

  /// Default constructor.
  VCNAudioCore();

// Shared members

  // Vector of all the sounds
  std::vector<VCNResID> mSounds;

  // Path to look for sounds
  VCNString mSoundPath;
};

///
/// INLINES
///

///////////////////////////////////////////////////////////////////////
inline const VCNString& VCNAudioCore::GetSoundPath() const
{
  return mSoundPath;
}

///////////////////////////////////////////////////////////////////////
inline void VCNAudioCore::SetSoundPath(const VCNString& path)
{
  mSoundPath = path;
}

#endif
