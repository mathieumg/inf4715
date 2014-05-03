///
/// @brief PhysX controller implementation
///

#include "Precompiled.h"
#include "PhysxController.h"
#include "VCNUtils/Quaternion.h"

#include "characterkinematic/PxBoxController.h"
#include "characterkinematic/PxCapsuleController.h"
#include "characterkinematic/PxControllerManager.h"
//#include "RenderBoxActor.h"
//#include "RenderCapsuleActor.h"
//#include "PhysXSample.h"

using namespace physx;
using namespace SampleRenderer;

ControlledActorDesc::ControlledActorDesc() :
mType(PxControllerShapeType::eFORCE_DWORD),
mPosition(PxExtendedVec3(0, 0, 0)),
mSlopeLimit(0.0f),
mContactOffset(0.0f),
mStepOffset(0.0f),
mInvisibleWallHeight(0.0f),
mMaxJumpHeight(0.0f),
mRadius(0.0f),
mHeight(0.0f),
mCrouchHeight(0.0f),
mProxyDensity(10.0f),
//	mProxyScale			(0.8f)
mProxyScale(0.9f)
{
}

VCN_TYPE(VCNPhysxController, VCNPhysicController);

VCNPhysxController::VCNPhysxController(PxController* pxController) :
mPxController(pxController),
mType(PxControllerShapeType::eFORCE_DWORD),
mRenderActorStanding(NULL),
mRenderActorCrouching(NULL),
mStandingSize(0.0f),
mCrouchingSize(0.0f),
mControllerRadius(0.0f),
mDoStandup(false),
mIsCrouching(false),
mInitialPosition(0,0,0),
mDelta(0),
mTransferMomentum(false)
{
}



VCNPhysxController::~VCNPhysxController()
{
	//mPxController->release();
	//mPxController = 0;
	//mRenderActorCrouching;
	//delete mRenderActorStanding;
	//mManager->release();
	//mManager = 0;
	//mJump
}

void VCNPhysxController::reset()
{
	mPxController->setPosition(mInitialPosition);
}

void VCNPhysxController::teleport(const PxVec3& pos)
{
	mPxController->setPosition(PxExtendedVec3(pos.x, pos.y, pos.z));
	mTransferMomentum = false;
	mDelta = PxVec3(0);
}

PxExtendedVec3 VCNPhysxController::getFootPosition() const
{
	return mPxController->getFootPosition();
}

void VCNPhysxController::sync()
{
	/*if (mDoStandup)
		tryStandup();*/

	//if (mRenderActorStanding)
	//	mRenderActorStanding->setRendering(!mIsCrouching);
	//if (mRenderActorCrouching)
	//	mRenderActorCrouching->setRendering(mIsCrouching);

	const PxExtendedVec3& newPos = mPxController->getPosition();

	const PxTransform tr(toVec3(newPos));

	//	printf("%f %f %f\n", tr.p.x, tr.p.y, tr.p.z);

	//if (mRenderActorStanding)
	//	mRenderActorStanding->setTransform(tr);
	//if (mRenderActorCrouching)
	//	mRenderActorCrouching->setTransform(tr);
}


PxController* VCNPhysxController::init(const ControlledActorDesc& desc, PxControllerManager* manager, PxPhysics* physics, PxScene* scene, PxUserControllerHitReport* report, PxControllerBehaviorCallback* behaviorCallback)
{
	PxControllerShapeType::Enum	type = desc.mType;
	const PxExtendedVec3& position = desc.mPosition;
	float slopeLimit = desc.mSlopeLimit;
	float contactOffset = desc.mContactOffset;
	float stepOffset = desc.mStepOffset;
	float invisibleWallHeight = desc.mInvisibleWallHeight;
	float maxJumpHeight = desc.mMaxJumpHeight;
	float radius = desc.mRadius;
	float height = desc.mHeight;
	float crouchHeight = desc.mCrouchHeight;

	PxControllerDesc* cDesc;
	PxBoxControllerDesc boxDesc;
	PxCapsuleControllerDesc capsuleDesc;

	if (type == PxControllerShapeType::eBOX)
	{
		height *= 0.5f;
		height += radius;
		crouchHeight *= 0.5f;
		crouchHeight += radius;
		boxDesc.halfHeight = height;
		boxDesc.halfSideExtent = radius;
		boxDesc.halfForwardExtent = radius;
		cDesc = &boxDesc;
	}
	else
	{
		PX_ASSERT(type == PxControllerShapeType::eCAPSULE);
		capsuleDesc.height = height;
		capsuleDesc.radius = radius;
		cDesc = &capsuleDesc;
	}

	cDesc->density = desc.mProxyDensity;
	cDesc->scaleCoeff = desc.mProxyScale;
	// Create a basic default material for now
	cDesc->material = physics->createMaterial(0.5f, 0.5f, 0.1f);
	cDesc->position = position;
	cDesc->slopeLimit = slopeLimit;
	cDesc->contactOffset = contactOffset;
	cDesc->stepOffset = stepOffset;
	cDesc->invisibleWallHeight = invisibleWallHeight;
	cDesc->maxJumpHeight = maxJumpHeight;
	cDesc->callback = report;
	cDesc->behaviorCallback = behaviorCallback;
	//	cDesc->nonWalkableMode		= PxCCTNonWalkableMode::eFORCE_SLIDING;
	//	cDesc->volumeGrowth			= 2.0f;

	mType = type;
	mInitialPosition = position;
	mStandingSize = height;
	mCrouchingSize = crouchHeight;
	mControllerRadius = radius;

	PxController* ctrl = static_cast<PxBoxController*>(manager->createController(*physics, scene, *cDesc));
	PX_ASSERT(ctrl);

	// remove controller shape from scene query for standup overlap test
	//PxRigidDynamic* actor = ctrl->getActor();
	//if (actor)
	//{
	//	if (actor->getNbShapes())
	//	{
	//		PxShape* ctrlShape;
	//		actor->getShapes(&ctrlShape, 1);
	//		ctrlShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);

	//		Renderer* renderer = mOwner.getRenderer();

	//		if (type == PxControllerShapeType::eBOX)
	//		{
	//			const PxVec3 standingExtents(radius, height, radius);
	//			const PxVec3 crouchingExtents(radius, crouchHeight, radius);

	//			mRenderActorStanding = SAMPLE_NEW(RenderBoxActor)(*renderer, standingExtents);
	//			mRenderActorCrouching = SAMPLE_NEW(RenderBoxActor)(*renderer, crouchingExtents);
	//		}
	//		else if (type == PxControllerShapeType::eCAPSULE)
	//		{
	//			mRenderActorStanding = SAMPLE_NEW(RenderCapsuleActor)(*renderer, radius, height*0.5f);
	//			mRenderActorCrouching = SAMPLE_NEW(RenderCapsuleActor)(*renderer, radius, crouchHeight*0.5f);
	//		}
	//	}
	//}

	mPxController = ctrl;
	return ctrl;
}

void VCNPhysxController::resizeController(PxReal height)
{
	mIsCrouching = true;
	mPxController->resize(height);
}

PX_INLINE void addForceAtPosInternal(PxRigidBody& body, const PxVec3& force, const PxVec3& pos, PxForceMode::Enum mode, bool wakeup)
{
	/*	if(mode == PxForceMode::eACCELERATION || mode == PxForceMode::eVELOCITY_CHANGE)
	{
	Ps::getFoundation().error(PxErrorCode::eINVALID_PARAMETER, __FILE__, __LINE__,
	"PxRigidBodyExt::addForce methods do not support eACCELERATION or eVELOCITY_CHANGE modes");
	return;
	}*/

	const PxTransform globalPose = body.getGlobalPose();
	const PxVec3 centerOfMass = globalPose.transform(body.getCMassLocalPose().p);

	const PxVec3 torque = (pos - centerOfMass).cross(force);
	body.addForce(force, mode, wakeup);
	body.addTorque(torque, mode, wakeup);
}

static void addForceAtLocalPos(PxRigidBody& body, const PxVec3& force, const PxVec3& pos, PxForceMode::Enum mode, bool wakeup = true)
{
	//transform pos to world space
	const PxVec3 globalForcePos = body.getGlobalPose().transform(pos);

	addForceAtPosInternal(body, force, globalForcePos, mode, wakeup);
}

//void defaultCCTInteraction(const PxControllerShapeHit& hit)
//{
//	PxRigidDynamic* actor = hit.shape->getActor().is<PxRigidDynamic>();
//	if (actor)
//	{
//		if (actor->getRigidDynamicFlags() & PxRigidDynamicFlag::eKINEMATIC)
//			return;
//
//		// We only allow horizontal pushes. Vertical pushes when we stand on dynamic objects creates
//		// useless stress on the solver. It would be possible to enable/disable vertical pushes on
//		// particular objects, if the gameplay requires it.
//		const PxVec3 upVector = hit.controller->getUpDirection();
//		const PxF32 dp = hit.dir.dot(upVector);
//		//		printf("%f\n", fabsf(dp));
//		if (fabsf(dp)<1e-3f)
//			//		if(hit.dir.y==0.0f)
//		{
//			const PxTransform globalPose = actor->getGlobalPose();
//			const PxVec3 localPos = globalPose.transformInv(toVec3(hit.worldPos));
//			::addForceAtLocalPos(*actor, hit.dir*hit.length*1000.0f, localPos, PxForceMode::eACCELERATION);
//		}
//	}
//}

///////////////////////////////////////////////////////////////////////
Matrix4 VCNPhysxController::GetTransform() const
{
	Matrix4 result;

	if (mPxController->getActor()->isRigidActor())
	{
		PxRigidActor* rigidActor = static_cast<PxRigidActor*>(mPxController->getActor());
		const PxTransform& globalPose = rigidActor->getGlobalPose();

		VCNQuat qRot(globalPose.q.x, globalPose.q.y, globalPose.q.z, globalPose.q.w);
		qRot.GetMatrix(result);
		result.SetTranspose();
		result.SetTranslation(globalPose.p.x, globalPose.p.y, globalPose.p.z);
	}

	return result;
}

