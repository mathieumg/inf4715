///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Vicuna FMOD Audio Core
///

#ifndef VICUNA_FMOD_AUDIO
#define VICUNA_FMOD_AUDIO

#pragma once

#include "VCNAudio/AudioCore.h"
#include "VCNUtils/StringUtils.h"

// Forward declarations
namespace FMOD {
  class System;
}

class VCNFMODAudioCore : public VCNAudioCore
{
  VCN_CLASS;

public:

  /// Default constructor.
  VCNFMODAudioCore();

  /// Default constructor.
  virtual ~VCNFMODAudioCore();

// VCNCore Interface

  /// Called at startup
  virtual VCNBool Initialize() override;

  /// Called when the core gets destroyed.
  virtual VCNBool Uninitialize() override;

  /// Called once every frame
  virtual VCNBool Process(const float elapsedTime) override;

// VCNAudioCore Interface

  /// Sets world 3D settings
  virtual void Set3DSettings(VCNFloat distanceFactor) override;

  /// Sets 3D/World settings.
  virtual void SetViewerSettings(
    const Vector3& viewerPosition, 
    const Vector3& velocity, 
    const Vector3& forwardDirection, 
    const Vector3& upDirection) override;

  /// Creates a 2D sound.
  virtual const VCNResID Create2DSoundEmitter(
    const VCNString& sndID, 
    const VCNString& filename, 
    const VCNBool loop, 
    const VCNFloat volume, 
    PlaybackType playback) override;  
  
  /// Create a 3D sound
  virtual const VCNResID Create3DSoundEmitter(
    const VCNString& sndID, 
    const VCNString& filename, 
    const Vector3& position, 
    const VCNFloat minDistance, 
    const VCNFloat maxDistance, 
    const VCNBool loop, 
    const VCNFloat volume, 
    PlaybackType playback) override;

  /// Stop all audio at once
  virtual void StopAll() override;

  /// Clean up!
  virtual void DestroyAll() override;

  // Assert function for FMOD
  static void FMODASSERT(int result);

private:

  /// FMOD Sound Pointer
  FMOD::System* mFMODSystem;

  /// Indicates that the sound core failed to initialize. 
  /// We don't want to crash or assert if it does, just ignore any sound request.
  bool mFailedInitOutput;

  /// Viewer settings
  Vector3 mViewerPosition;
  Vector3 mViewerVelocity;
  Vector3 mViewerForwardDirection;
  Vector3 mViewerUpDirection;
};

///
/// INLINES
///

///////////////////////////////////////////////////////////////////////

#endif