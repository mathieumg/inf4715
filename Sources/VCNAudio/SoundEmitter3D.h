///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Abstract 3D sound emitter interface
///

#ifndef VICUNA_3D_SOUND_EMITTER
#define VICUNA_3D_SOUND_EMITTER

#pragma once

#include "VCNAudio/SoundEmitter.h"
#include "VCNUtils/Vector.h"

///
/// The Vicuna Base 3D Sound Emitter Resource.
///
class VCN3DSoundEmitter : public VCNSoundEmitter
{
public:
  
  /// Default destructor
  virtual ~VCN3DSoundEmitter();

  /// Returns the emitter position.
  inline const Vector3& GetPosition() const;

  /// Sets the position of the emitter.
  inline void SetPosition(const Vector3& position);

  /// Returns the sound velocity.
  inline const Vector3& GetVelocity() const;

  /// Sets the sound velocity.
  inline void SetVelocity(const Vector3& velocity);

  /// Returns the min distance.
  inline const VCNFloat GetMinDistance() const;

  /// Returns the max distance,
  inline const VCNFloat GetMaxDistance() const;

  /// Sets the min and max distance in 3D of the sound.
  inline void SetMinMaxDistance(VCNFloat min, VCNFloat max);

protected:

  /// Default constructor
  VCN3DSoundEmitter();

// Events

  /// Called when the position changed.
  virtual void OnPositionChanged() {}

  /// Called when the velocity setting changes.
  virtual void OnVelocityChanged() {}

  /// Called when  the min and max distances change.
  virtual void OnMinMaxDistanceChanged() {}

// Shared members

  /// Position of the emitter
  Vector3 mPosition;

  /// Sound velocity.
  Vector3 mVelocity;

  /// min and max distance in 3D
  VCNFloat mMinDistance, mMaxDistance;
};

///
/// INLINES
///

///////////////////////////////////////////////////////////////////////
inline const Vector3& VCN3DSoundEmitter::GetPosition() const
{
  return mPosition;
}

///////////////////////////////////////////////////////////////////////
inline void VCN3DSoundEmitter::SetPosition(const Vector3& position)
{
  mPosition = position;
  OnPositionChanged();
}

///////////////////////////////////////////////////////////////////////
inline const Vector3& VCN3DSoundEmitter::GetVelocity() const
{
  return mVelocity;
}

///////////////////////////////////////////////////////////////////////
inline void VCN3DSoundEmitter::SetVelocity(const Vector3& velocity)
{
  mVelocity = velocity;
  OnVelocityChanged();
}

///////////////////////////////////////////////////////////////////////
inline const VCNFloat VCN3DSoundEmitter::GetMinDistance() const
{
  return mMinDistance;
}

///////////////////////////////////////////////////////////////////////
const VCNFloat VCN3DSoundEmitter::GetMaxDistance() const
{
  return mMaxDistance;
}

///////////////////////////////////////////////////////////////////////
void VCN3DSoundEmitter::SetMinMaxDistance(VCNFloat min, VCNFloat max)
{
  mMinDistance = min;
  mMaxDistance = max;
  OnMinMaxDistanceChanged();
}

#endif
