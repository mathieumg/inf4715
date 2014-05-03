#ifndef JUMP_H
#define JUMP_H

#include "common/PxPhysXCommon.h"

using namespace physx;

class Jump
{
public:
	Jump();

	PxF32		mV0;
	PxF32		mJumpTime;
	bool		mJump;

	void		startJump(PxF32 v0);
	void		stopJump();
	PxF32		getHeight(PxF32 elapsedTime);
};

#endif
