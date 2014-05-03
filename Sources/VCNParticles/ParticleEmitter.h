///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Base particle emitter class interface
///

#ifndef VCNPARTICLEEMITTER_H
#define VCNPARTICLEEMITTER_H

#pragma once

#include "Particle.h"
#include "ParticleCore.h" // REMOVE ME

#include "VCNUtils/Vector.h"
#include "VCNUtils/Types.h"

//! The parameters specified each time you create an emitter (if one is unused for a specific emitter it can be ignored)
struct ParticleEmitterParams
{
  Vector3   pos;
  Vector3   dir;
  float     radius; //!< can be ignored for default
  VCNString name;
};

///
/// Handles the management of particles: all of their creation, updating, etc.
///
class VCNParticleEmitter : public std::enable_shared_from_this<VCNParticleEmitter>
{
public:
  explicit VCNParticleEmitter(ParticleEmitterParams& params);
  virtual ~VCNParticleEmitter();

  /// The update function that is called each frame
  /// Returns false when the emitter is dead!
  const bool Update(float dt); 

  /// returns the list of particles for rendering
  std::vector<VCNParticle>& GetParticles() { return mParticles; } 

  /// Controls the position that particles are initiliazed at (or center of circle etc)
  void SetPosition(const Vector3& vPos)  { mPosistion = vPos; } 

  /// Sets the direction of the emitter (if used)
  void SetDirection(const Vector3& vDir) { mDirection = vDir; } 
  void SetAlive(bool alive) { mAlive = alive; }
  bool GetAlive() const { return mAlive; }

  const Vector3& GetPosition() const { return mPosistion; }

  /// Used in rendering to get the appropriate blending and colors
  const ParticleEmitterSettings& GetSettings() const { return mSettings; } 

  /// A temporary debug function
  const int GetParticleCount() const { return mActive; } 

protected:

  /// Must call this per particle in updateParticles
  void UpdateParticle(VCNParticle& p, float dt); 

// Contract

  /// Determines pos and dir of created particles - must override this function for other effects
  virtual void InitParticle(VCNParticle& p, float dt) = 0;    

  /// Loop over all particles and perform updating (of pos/vel) - must override this function for other effects
  virtual void UpdateParticles(float dt) = 0;   
  
// Data members

  std::vector<VCNParticle>        mParticles;
  const ParticleEmitterSettings&  mSettings;

  /// Position of the emitter
  Vector3                         mPosistion;         

  /// Direction the emitters emits in - (0,0,0) is all directions
  Vector3                         mDirection;         
  float                           mLife;
  float                           mLastUpdate;
  bool                            mAlive;

  int                             mNumToAdd;

  /// Number of active particles
  int                             mActive;    
};

/// 
/// The default implementation of an emitter.
/// Initializes all particles from a single point. Direction is random based around direction specified.
/// You will end up emitting in a cone if a direction is specified or in all directions if the dir is (0,0,0).
///
class VCNParticleDefaultEmitter : public VCNParticleEmitter
{
public:

  VCNParticleDefaultEmitter(ParticleEmitterParams& params);

protected:

  /// Determines pos and dir of created particles
  virtual void InitParticle(VCNParticle& p, float dt) override;    

  /// Updates the particle state
  virtual void UpdateParticles(float dt) override;
};

#endif // VCNPARTICLEEMITTER_H
