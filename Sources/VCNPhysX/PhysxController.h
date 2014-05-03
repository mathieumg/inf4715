///
/// @brief PhysX interface of a character controller
///

#ifndef VCNPHYSXCONTROLLER_H
#define VCNPHYSXCONTROLLER_H

#include "VCNPhysic/PhysicController.h"
#include "characterkinematic/PxExtended.h"
#include "characterkinematic/PxController.h"
#include "Jump.h"

namespace physx
{
	class PxController;
	class PxUserControllerHitReport;
	class PxControllerBehaviorCallback;
	class PxControllerManager;
	class PxPhysics;
	class PxScene;
}

using namespace physx;

namespace SampleRenderer
{
	class Renderer;
}

class RenderBaseActor;
class PhysXSample;

struct ControlledActorDesc
{
	ControlledActorDesc();

	PxControllerShapeType::Enum	mType;
	PxExtendedVec3				mPosition;
	float						mSlopeLimit;
	float						mContactOffset;
	float						mStepOffset;
	float						mInvisibleWallHeight;
	float						mMaxJumpHeight;
	float						mRadius;
	float						mHeight;
	float						mCrouchHeight;
	float						mProxyDensity;
	float						mProxyScale;
};

class VCNPhysxController : public VCNPhysicController
{
	VCN_CLASS;

public:

	/// Default constructor
	explicit VCNPhysxController(PxController* pxController);

	/// Destructor
	virtual ~VCNPhysxController();

	PxController*				init(const ControlledActorDesc& desc, PxControllerManager* manager, PxPhysics* physics, PxScene* scene, PxUserControllerHitReport* report = NULL, PxControllerBehaviorCallback* behaviorCallback = NULL);
	PxExtendedVec3				getFootPosition()	const;
	void						reset();
	void						teleport(const PxVec3& pos);
	void						sync();
	//void						tryStandup();
	void						resizeController(PxReal height);
	void						resizeStanding()			{ resizeController(mStandingSize); }
	void						resizeCrouching()			{ resizeController(mCrouchingSize); }
	void						jump(float force)			{ mJump.startJump(force); }

	PX_FORCE_INLINE	RenderBaseActor*			getRenderActorStanding()	{ return mRenderActorStanding; }
	PX_FORCE_INLINE	RenderBaseActor*			getRenderActorCrouching()	{ return mRenderActorCrouching; }
	PX_FORCE_INLINE	PxController*				getController()				{ return mPxController; }
    PX_FORCE_INLINE Jump&                       getJump()                   { return mJump; }
    PX_FORCE_INLINE PxControllerManager*        getManager()                { return mManager; }

	/// Returns the actor transform (translation + rotation)
	virtual Matrix4 GetTransform() const;

private:
    PxController*  mPxController;
    Jump mJump;
	PxControllerManager* mManager;

protected:
	PxControllerShapeType::Enum	mType;
	

	PxExtendedVec3				mInitialPosition;
	PxVec3						mDelta;
	bool						mTransferMomentum;

	//PxController*				mController;
	RenderBaseActor*			mRenderActorStanding;
	RenderBaseActor*			mRenderActorCrouching;
	PxReal						mStandingSize;
	PxReal						mCrouchingSize;
	PxReal						mControllerRadius;
	bool						mDoStandup;
	bool						mIsCrouching;
//friend class SampleCCTCameraController;

};

//void defaultCCTInteraction(const PxControllerShapeHit& hit);

#endif // VCNPHYSXCONTROLLER_H
