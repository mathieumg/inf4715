///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// The Vicuna Animation Node class.
///

#ifndef VICUNA_ANIMATED_NODE
#define VICUNA_ANIMATED_NODE

#pragma once

#include "VCNNodes/Node.h"
#include "VCNUtils/Constants.h"

//-------------------------------------------------------------
// The animation node class
//-------------------------------------------------------------
class VCNAnimationController : public VCNAtom
{
	VCN_CLASS;

public:

	VCNAnimationController();
	virtual ~VCNAnimationController();

	// Joint accessors
	void SetJointID( VCNResID id );
	const VCNResID GetJointID() const;

	// Anim accessors
	void SetAnimID( VCNAnimID id );
	const VCNAnimID GetAnimID() const;

	// Target accessors
	void SetTarget(VCNNodeID nodeTargetID);
	VCNNodeID GetTarget() const;

	// Control looping
	void SetLooping(bool loop);
	bool IsLooping() const;

	// Accessor
	const VCNBool IsActive() const;

	// Stop anything we were doing
	void StartAnimation();
	void StopAnimation();

	// Called every frame (before the rendering pass)
	void Process(float elapsedTime);
	
protected:

	VCNAnimID mAnimID;
	VCNResID  mJointID;
	VCNFloat  mCurrentTime;
	VCNBool   mActive;
	VCNNodeID mNodeTargetID;
	VCNBool   mLooping;
};

//-------------------------------------------------------------
inline void VCNAnimationController::StartAnimation()
{
	VCN_ASSERT( mAnimID != kInvalidAnimID && mJointID != kInvalidResID && mNodeTargetID != kInvalidNodeID );

	mCurrentTime = 0.0f;
	mActive = true;
}

//-------------------------------------------------------------
inline void VCNAnimationController::StopAnimation()
{
	mCurrentTime = 0.0f;
	mActive = false;
}

//-------------------------------------------------------------
inline void VCNAnimationController::SetJointID( VCNResID id )
{
	mJointID = id;
}

//-------------------------------------------------------------
inline const VCNResID VCNAnimationController::GetJointID() const
{
	return mJointID;
}

//-------------------------------------------------------------
inline void VCNAnimationController::SetAnimID( VCNAnimID id )
{
	mAnimID = id;
}

//-------------------------------------------------------------
inline const VCNAnimID VCNAnimationController::GetAnimID() const
{
	return mAnimID;
}

//-------------------------------------------------------------
inline const VCNBool VCNAnimationController::IsActive() const
{
	return mActive;
}

//-------------------------------------------------------------
inline void VCNAnimationController::SetTarget(VCNNodeID nodeTargetID)
{
	mNodeTargetID = nodeTargetID;
}

//-------------------------------------------------------------
inline VCNNodeID VCNAnimationController::GetTarget() const
{
	return mNodeTargetID;
}

///////////////////////////////////////////////////////////////////////
inline void VCNAnimationController::SetLooping(bool loop)
{
	mLooping = loop;
}

///////////////////////////////////////////////////////////////////////
inline bool VCNAnimationController::IsLooping() const
{
	return mLooping;
}

#endif
