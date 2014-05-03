///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Defines an abstract physic actor
///

#ifndef VCNPHYSICACTOR_H
#define VCNPHYSICACTOR_H

#pragma once

#include "VCNCore/Atom.h"
#include "VCNUtils/Matrix.h"

class VCNPhysicActor : public VCNAtom
{

  VCN_CLASS;

public:

  // Destructor
  virtual ~VCNPhysicActor();

  /// Returns the actor transformation
  virtual Matrix4 GetTransform() const = 0;

  /// Adds a impulsion to the actor
  virtual void AddForce(const Vector3& force) = 0;

protected:

  // Default constructor
  VCNPhysicActor();
  
};

#endif // VCNPHYSICACTOR_H
