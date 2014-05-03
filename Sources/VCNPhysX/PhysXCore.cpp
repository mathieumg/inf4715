///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Dispatch calls to the PhysX implementation VCNPhysXCoreImpl
///

#include "Precompiled.h"
#include "PhysxCore.h"

#include "PhysxActor.h"
#include "PhysxCoreImpl.h"

namespace
{
}

//////////////////////////////////////////////////////////////////////////
VCNPhysxCore::VCNPhysxCore()
  : mImpl( new VCNPhysxCoreImpl() )
{
}

//////////////////////////////////////////////////////////////////////////
VCNPhysxCore::~VCNPhysxCore()
{
}

//////////////////////////////////////////////////////////////////////////
VCNBool VCNPhysxCore::Initialize()
{
  if ( !VCNPhysicCore::Initialize() )
    return false;

  return mImpl->Initialize();
}

//////////////////////////////////////////////////////////////////////////
VCNBool VCNPhysxCore::Uninitialize()
{
  // Clears actors
  for (ActorList::iterator it = mActors.begin(), end = mActors.end(); it != end; ++it)
  {
    delete *it;
  }

  for (ControllerList::iterator it = mControllers.begin(), end = mControllers.end(); it != end; ++it)
  {
	  delete *it;
  }

  // Shutdown the implementation
  if ( !mImpl->Uninitialize() )
    return false;

  return VCNPhysicCore::Uninitialize();
}

//////////////////////////////////////////////////////////////////////////
VCNBool VCNPhysxCore::Process(const float elapsedTime)
{
  return mImpl->Process( elapsedTime );
}


///////////////////////////////////////////////////////////////////////
VCNPhysxActor* const VCNPhysxCore::AddActor(PxActor* pxActor)
{
  VCNPhysxActor* actor = new VCNPhysxActor(pxActor);

  mActors.push_back( actor );

  return actor;
}

VCNPhysxController* const VCNPhysxCore::AddController(physx::PxController* pxController)
{
	VCNPhysxController* controller = new VCNPhysxController(pxController);

	mControllers.push_back(controller);

	return controller;

}

///////////////////////////////////////////////////////////////////////
VCNPhysicActor* const VCNPhysxCore::CreateGroundPlane(const Vector4& plane)
{
  PxActor* actor = mImpl->CreateGroundPlane( plane );
  return AddActor( actor );
}

///////////////////////////////////////////////////////////////////////
VCNPhysicActor* const VCNPhysxCore::CreateStaticMesh(const VCNResID meshID, const Matrix4& transform)
{
  PxActor* actor = mImpl->CreateStaticMesh( meshID, transform );
  return AddActor( actor );
}

///////////////////////////////////////////////////////////////////////
VCNPhysicActor* const VCNPhysxCore::CreateSphere(const VCNSphere& sphereDesc)
{
  PxActor* actor = mImpl->CreateSphere( sphereDesc );
  return AddActor( actor );
}



///////////////////////////////////////////////////////////////////////

VCNPhysicActor* const VCNPhysxCore::CreateActor(const VCNNode* node)
{
	return CreateActor(node, true);
}


VCNPhysicActor* const VCNPhysxCore::CreateActor(const VCNNode* node, const bool& hasGravity)
{
  PxActor* actor = mImpl->CreateActor( node );
  actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !hasGravity);
  return AddActor( actor );
}

VCNPhysicController* const VCNPhysxCore::CreateCharacterController(const VCNNode* node)
{
	PxController* controller = mImpl->CreateCharacterController(node);
	return AddController(controller);
}



///////////////////////////////////////////////////////////////////////
void VCNPhysxCore::UpdateDebugCamera(const Vector3& pos, const Vector3& target, const Vector3& up)
{
  PxVisualDebugger* debugger = mImpl->mPhysics->getVisualDebugger();
  if ( debugger )
  {
    PxVec3 pxPos( pos.x, pos.y, -pos.z );
    PxVec3 pxTarget( target.x, target.y, -target.z );
    PxVec3 pxUp( up.x, up.y, -up.z );

    debugger->updateCamera( "Game", pxPos, pxUp, pxTarget );
  }
}



///////////////////////////////////////////////////////////////////////
void VCNPhysxCore::SetGravity(const Vector3& gravity)
{
  VCN_ASSERT( mImpl->mScene );
  mImpl->mScene->setGravity( (const PxVec3&)gravity );
}

void VCNPhysxCore::RemoveActor( VCNPhysicActor* actor )
{
	PxActor* a = (PxActor*)actor->GetWrappedActor();
	mImpl->RemoveActor(a);
}
