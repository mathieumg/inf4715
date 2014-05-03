///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// Defines the physic interface
/// 

#pragma once

#include "PhysicActor.h"
#include "PhysicController.h"

#include "VCNCore/Core.h"
#include "VCNUtils/Vector.h"
#include "VCNUtils/Matrix.h"

// Forward declarations
class VCNNode;
class VCNSphere;

class VCNPhysicCore : public VCNCore<VCNPhysicCore>
{
public:

// Scene attributes

  /// Sets the world gravity vector.
  virtual void SetGravity(const Vector3& gravity) = 0;

// Creation

  /// Create an actor based on the given node.
  virtual VCNPhysicActor* const CreateActor(const VCNNode* node) = 0;
  virtual VCNPhysicActor* const CreateActor(const VCNNode* node, const bool& hasGravity) = 0;

  /// Create an CreateCharacterController.
  virtual VCNPhysicController* const CreateCharacterController(const VCNNode* node) = 0;

  /// Creates a ground plane at (0,1,0,0)
  virtual VCNPhysicActor* const CreateGroundPlane(const Vector4& plane = Vector4(0, 1, 0, 0)) = 0;

  /// Creates a dynamic sphere.
  virtual VCNPhysicActor* const CreateSphere(const VCNSphere& sphereDesc) = 0;

  /// Creates a static actor from a given mesh.
  virtual VCNPhysicActor* const CreateStaticMesh(const VCNResID meshID, const Matrix4& transform = Matrix4()) = 0;

  virtual void RemoveActor(VCNPhysicActor* actor) = 0;

// Debugging

  virtual void UpdateDebugCamera(const Vector3& pos, const Vector3& target, const Vector3& up = Vector3(0, 1, 0)) = 0; 

  
  // Boom!
  virtual ~VCNPhysicCore() = 0;

protected:

  VCNPhysicCore();
};
