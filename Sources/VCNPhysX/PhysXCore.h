///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Declares the PhysX engine manager interface.
///

#ifndef VCNPHYSXCORE_H 
#define VCNPHYSXCORE_H

#include "VCNPhysic/PhysicCore.h"

// VCN forward declarations
class VCNPhysxActor;
class VCNPhysxController;
class VCNPhysxCoreImpl;

// PhysX forward declarations
namespace physx {
  class PxActor;
  class PxController;
}

class VCNPhysxCore : public VCNPhysicCore
{
public:

  VCNPhysxCore();
  virtual ~VCNPhysxCore();

protected:

  /// Initialize the physic engine
  virtual VCNBool Initialize() override;

  /// Release the physic engine
  virtual VCNBool Uninitialize() override;

  /// Process the physic loop
  virtual VCNBool Process(const float elapsedTime) override;

private:

  typedef std::vector<VCNPhysxActor*> ActorList;
  typedef std::vector<VCNPhysxController*> ControllerList;

  /// Adds a new actor to the physic actor database
  VCNPhysxActor* const AddActor(physx::PxActor* pxActor);
  /// Adds a new controller to the physic controller database
  VCNPhysxController* const AddController(physx::PxController* pxController);

// VCNPhysicCore interface

  virtual void SetGravity(const Vector3& gravity) override;
  virtual VCNPhysicActor* const CreateActor(const VCNNode* node) override;
  virtual VCNPhysicActor* const CreateActor(const VCNNode* node, const bool& hasGravity) override;
  virtual VCNPhysicController* const CreateCharacterController(const VCNNode* node) override;
  virtual VCNPhysicActor* const CreateGroundPlane(const Vector4& plane) override;
  virtual VCNPhysicActor* const CreateSphere(const VCNSphere& sphereDesc) override;
  virtual VCNPhysicActor* const CreateStaticMesh(const VCNResID meshID, const Matrix4& transform) override;
  virtual void UpdateDebugCamera(const Vector3& pos, const Vector3& target, const Vector3& up) override; 

  virtual void RemoveActor( VCNPhysicActor* actor );

// Data members

  std::shared_ptr<VCNPhysxCoreImpl> mImpl;
  
  ActorList                         mActors;
  ControllerList                    mControllers;
};

#endif // VCNPHYSXCORE_H
