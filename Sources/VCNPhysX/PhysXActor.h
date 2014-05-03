///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief PhysX interface of an actor
///

#ifndef VCNPHYSXACTOR_H
#define VCNPHYSXACTOR_H

#pragma once

#include "VCNPhysic/PhysicActor.h"

class VCNPhysxActor : public VCNPhysicActor
{
  VCN_CLASS;

public:

  /// Default constructor
  explicit VCNPhysxActor(PxActor* pxActor);

  /// Destructor
  virtual ~VCNPhysxActor();

  /// Returns the actor transform (translation + rotation)
  virtual Matrix4 GetTransform() const;

  /// Adds an impulsion to the actor
  virtual void AddForce(const Vector3& force) override;

  virtual void SetKinematicTarget( const Matrix4& target );

  virtual void SetAngularVelocity( const Vector3& velocity );

  virtual void SetAngularDamping( const float& damping );

  virtual void SetLinearVelocity( const Vector3& velocity );

  virtual void SetLinearDamping( const float& damping );

  virtual void* GetWrappedActor();
private:

  PxActor*  mPxActor;

};

#endif // VCNPHYSXACTOR_H
