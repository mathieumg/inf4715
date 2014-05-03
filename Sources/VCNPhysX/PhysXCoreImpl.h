///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief PhysX concrete implementation
///

#ifndef VCNPHYSXCOREIMPL_H
#define VCNPHYSXCOREIMPL_H

#pragma once

#include "PhysxUserAllocator.h"
#include "PhysxErrorCallback.h"

#include "VCNUtils/Types.h"
#include "VCNUtils/Vector.h"
#include "VCNUtils/Matrix.h"
#include "characterkinematic/PxController.h"
#include "characterkinematic/PxControllerBehavior.h"
#include "characterkinematic/PxControllerObstacles.h"
#include "PhysxController.h"

// Forward declaractions
class VCNNode;
class VCNSphere;

class VCNPhysxCoreImpl
{
public:

  friend class VCNPhysxCore;
  VCNPhysxController*		mPhysxController;

  ~VCNPhysxCoreImpl();

protected:

  VCNPhysxCoreImpl();

  /// Initialize the physic engine
  VCNBool Initialize();

  /// Release the physic engine
  VCNBool Uninitialize();

  /// Process the physic loop
  VCNBool Process(const float elapsedTime);

  /// Creates an actor based on the given node.
  PxActor* const CreateActor(const VCNNode* node);

  /// Creates an CharacterController.
  PxController* const CreateCharacterController(const VCNNode* node);

  /// Creates a basic ground plane
  PxActor* const CreateGroundPlane(const Vector4& plane);

  /// Creates a dynamic sphere.
  PxActor* const CreateSphere(const VCNSphere& sphereDesc);

  /// Creates a triangle mesh from a VCN mesh
  PxActor* const CreateStaticMesh(const VCNResID meshID, const Matrix4& transform);

  void RemoveActor(PxActor* actor);

private:


// Typedefs

  typedef std::shared_ptr<PxGeometry> ShapePointer;
  typedef std::vector<ShapePointer>   ShapeList;

  /// Creates recursively shapes for the given actor
  void CreateActorShapes(const VCNNode* node, const bool isDynamic, ShapeList& shapes);

// Data members

  VCNPhysxErrorReporter     mDefaultErrorCallback;
  VCNPhysXDefaultAllocator  mDefaultAllocatorCallback;
  PxSimulationFilterShader  mDefaultFilterShader;


  PxFoundation*             mFoundation;
  PxPhysics*                mPhysics;
  PxCooking*                mCooking;
  PxScene*                  mScene;
  PxControllerManager*		mControllerManager;
  PxDefaultCpuDispatcher*   mCpuDispatcher;
  CudaContextManager*       mCudaContextManager;

  PxMaterial*               mDefaultMaterial;

  VCNFloat                  mAccumulator;
  const VCNFloat            mStepSize;
  
};

#endif // VCNPHYSXCOREIMPL_H
