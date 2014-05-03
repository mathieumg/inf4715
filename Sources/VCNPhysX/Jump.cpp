#include "Precompiled.h"
#include "Jump.h"

using namespace physx;

static PxF32 gJumpGravity = -50.0f;

Jump::Jump() :
mV0(0.0f),
mJumpTime(0.0f),
mJump(false)
{
}

void Jump::startJump(PxF32 v0)
{
	if (mJump)	return;
	mJumpTime = 0.0f;
	mV0 = v0;
	mJump = true;
}

void Jump::stopJump()
{
	if (!mJump)	return;
	mJump = false;
	//mJumpTime = 0.0f;
	//mV0	= 0.0f;
}

PxF32 Jump::getHeight(PxF32 elapsedTime)
{
	if (!mJump)	return 0.0f;
	mJumpTime += elapsedTime;
	const PxF32 h = gJumpGravity*mJumpTime*mJumpTime + mV0*mJumpTime;
	return h*elapsedTime;
}

