///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Particle module interface
///

#ifndef VCNPARTICLECORE_H
#define VCNPARTICLECORE_H

#pragma once

#include "VCNCore/Core.h"
#include "VCNUtils/RenderTypes.h"
#include "VCNUtils/SmartTypes.h"
#include "VCNUtils/Vector.h"

// Forward declaration
class VCNCache;
class VCNParticleEffect;

enum BlendMode{ ALPHA, ADDITIVE, SUBTRACTIVE };

//! Structure to hold all of the actual information about the emitter
struct ParticleEmitterSettings
{
  VCNResID    texID;        //!< Texture associated with this emitter
  Vector3     accel;        //!< Acceleration on the particles
  VCNColor    startColor;   //!< Start color to lerp from
  VCNColor    endColor;     //!< End color to lerp to
  BlendMode   blend;        //!< What type of blending the particles use
  float       emitterLife;  //!< Actual life of the emitter (-1 is infinite)
  float       particleLife; //!< Maximum life of the particles
  float       vel;          //!< Maximum velocity of particles
  float       size;         //!< Maximum size of particles
  float       variance;     //!< How varied the vel, dir, siz, etc. are (A VARIANCE OF <em>1.0</em> IS NONE)
  int         rate;         //!< particles per second
  float       rotationRate; //!< rate the particle will rotate in radians per second
};

class VCNParticleCore : public VCNCore<VCNParticleCore>
{
  VCN_CLASS;

public:

// Interface
  
  /// Default constructor
  VCNParticleCore();

  /// Destructor
  virtual ~VCNParticleCore();

  /// Returns the number of active particles
  int GetParticleCount() const;

  /// Returns particle settings
  ParticleEmitterSettings& GetSettings(const VCNString& name);

  /// Add a new emitter 
  void AddEmitter(VCNParticleEmitterPointer em);

  /// Remove an emitter
  void RemoveEmitter(VCNParticleEmitterPointer emitter);
  
  /// Renders the particles as billboarded quads 
  void Render();

protected:

// IVCNCore overrides

  /// Initialize the particle module
  virtual VCNBool Initialize() override;

  /// Cleans up the module resources
  virtual VCNBool Uninitialize() override;

  /// Updates particles emitters
  virtual VCNBool Process(const float elapsedTime) override;

private:

// Predefined types

  typedef std::map<VCNString, ParticleEmitterSettings> SettingMap;

// Implementation

  /// Fills the index buffer
  void FillIndexBuffer();

  /// Renders particle in batch
  void FlushBuffer(int& numParticlesToRender);

  /// Loads particle settings from file
  SettingMap::value_type LoadSettingsFromFile(const VCNString& emitterFile);
  
// Data members

  VCNString                       mDirectory;
  SettingMap                      mEmitterSettings;
  VCNParticleEmitterList          mEmitters;

  VCNCache*                       mVB;
  VCNCache*                       mIB;
  VCNParticleEffect*              mEffect;
  
  /// Particle rendering state
  int                             mBase;
  const int                       mFlush;
  const int                       mDiscard;
};

#endif // VCNPARTICLECORE_H
