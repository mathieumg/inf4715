///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief PhysX actor implementation
///

#include "Precompiled.h"
#include "PhysxActor.h"
#include "VCNUtils/Quaternion.h"
#include "PhysxUtils.h"

VCN_TYPE( VCNPhysxActor, VCNPhysicActor );

VCNPhysxActor::VCNPhysxActor(PxActor* pxActor)
  : mPxActor(pxActor)
{
}



VCNPhysxActor::~VCNPhysxActor()
{
}



///////////////////////////////////////////////////////////////////////
Matrix4 VCNPhysxActor::GetTransform() const
{
  Matrix4 result;

  if ( mPxActor->isRigidActor() )
  {
    PxRigidActor* rigidActor = static_cast<PxRigidActor*>( mPxActor );
    const PxTransform& globalPose = rigidActor->getGlobalPose();

    VCNQuat qRot( globalPose.q.x, globalPose.q.y, globalPose.q.z, globalPose.q.w );
    qRot.GetMatrix( result );
    result.SetTranspose();
    result.SetTranslation( globalPose.p.x, globalPose.p.y, globalPose.p.z );
  }
  return result;
}

///////////////////////////////////////////////////////////////////////
void VCNPhysxActor::AddForce(const Vector3& force)
{
  VCN_ASSERT( mPxActor->isRigidDynamic() );
  
  PxRigidDynamic* pxActor = static_cast<PxRigidDynamic*>( mPxActor );
  pxActor->addForce( (const PxVec3&)force );
}

void VCNPhysxActor::SetKinematicTarget( const Matrix4& target )
{
	VCN_ASSERT( mPxActor->isRigidDynamic() );
  
    PxRigidDynamic* pxActor = static_cast<PxRigidDynamic*>( mPxActor );
	pxActor->setKinematicTarget(VCNPhysxUtils::ToTransform(target));
}

void VCNPhysxActor::SetAngularVelocity( const Vector3& velocity )
{
	VCN_ASSERT( mPxActor->isRigidDynamic() );

	PxRigidDynamic* pxActor = static_cast<PxRigidDynamic*>( mPxActor );
	pxActor->setAngularVelocity(PxVec3(velocity.x, velocity.y, velocity.z));
}

void VCNPhysxActor::SetAngularDamping( const float& damping )
{
	VCN_ASSERT( mPxActor->isRigidDynamic() );

	PxRigidDynamic* pxActor = static_cast<PxRigidDynamic*>( mPxActor );
	pxActor->setAngularDamping(damping);
}

void VCNPhysxActor::SetLinearVelocity( const Vector3& velocity )
{
    VCN_ASSERT( mPxActor->isRigidDynamic() );

    PxRigidDynamic* pxActor = static_cast<PxRigidDynamic*>( mPxActor );
    pxActor->setLinearVelocity(PxVec3(velocity.x, velocity.y, velocity.z));
}

void VCNPhysxActor::SetLinearDamping( const float& damping )
{
    VCN_ASSERT( mPxActor->isRigidDynamic());

    PxRigidDynamic* pxActor = static_cast<PxRigidDynamic*>( mPxActor );
    pxActor->setLinearDamping(damping);
}

void* VCNPhysxActor::GetWrappedActor()
{
	return mPxActor;
}
