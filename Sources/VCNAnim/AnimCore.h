///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// Vicuna's Animation Core
/// Core module for animation management.
///

#ifndef VICUNA_ANIMATION_CORE
#define VICUNA_ANIMATION_CORE

#pragma once

#include "VCNCore/Core.h"

class VCNNode;
class VCNAnimationController;

// Starting animation options
enum
{
	kNoAnimFlag   = 0,
	kLoop         = 1 << 1,
	kLocal        = 1 << 2,
	kGlobal       = 1 << 3,
};

class VCNAnimCore : public VCNCore<VCNAnimCore>
{
	VCN_CLASS;

public:
	VCNAnimCore(void);
	virtual ~VCNAnimCore(void);

	// Core module overloads
	virtual VCNBool Initialize() override;
	virtual VCNBool Uninitialize() override;
	virtual VCNBool Process(const float elapsedTime) override;

	// Apply an animation
	VCNAnimID StartAnimation(VCNNode* node, VCNResID animResID, VCNFlags flags = kNoAnimFlag);
	void StopAnimation(VCNAnimID animID);

	// Call to notify that an animation is finish. We ensure cleanup.
	void NotifyFinish( VCNAnimID animID );

	// Returns the first free slot
	VCNAnimID GetFreeAnimID();

	// Checks if an animation is done
	VCNBool IsAnimationFinished(const VCNAnimID animID);

protected:

	typedef std::vector<VCNAnimationController*>          ControllerList;
	typedef std::unordered_map<VCNAnimID, ControllerList> AnimationList;

	/// Animation controllers
	AnimationList mAnimations;
};

#endif
