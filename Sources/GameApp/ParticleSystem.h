///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Game particle system interface
///

#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#pragma once

#include "VCNNodes/Node.h"

///
/// The particle system is a composite node to store all emitter nodes
///
class ParticleSystem : public VCNNode
{
public:

  /// Default constructor
  explicit ParticleSystem( VCNNodeID nodeID );

  /// Destructor
  virtual ~ParticleSystem();

  /// Creates a fire emitter
  void CreateFireEmitter(const Vector3& position);

protected:

  // Called every frame (before the rendering pass)
  virtual void Process(const float elapsedTime) override;

  // Called every frame to render
  virtual void Render() const override;
};

#endif // PARTICLESYSTEM_H
