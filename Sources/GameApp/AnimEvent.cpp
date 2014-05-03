///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///
/// @brief Animation Event Implementation
///

#include "Precompiled.h"
#if 0
#include "AnimEvent.h"

// Project includes
#include "SoundManager.h"

// Engine includes
#include "VCNAnim/AnimCore.h"
#include "VCNAnim/AnimationController.h"
#include "VCNNodes/Node.h"
#include "VCNNodes/NodeCore.h"

////////////////////////////////////////////////////////////////////////
///
/// Constructor for the AnimEvent class.
///
/// @param[im] nodeId : the node id to be associated with this event
///            id : the resource id to be associated with this event
///            sound : the name of the sound from the "SoundsManagerConstants"
///            open : (true) the node is opened
///                   (false) the node is closed
///
/// @return AnimEvent
///
////////////////////////////////////////////////////////////////////////
AnimEvent::AnimEvent(const VCNNodeID& nodeId, const VCNResID& id, const VCNTChar* sound ,bool open, bool repeatable)
	: mNodeId(nodeId)
	, mId(id)
	, mOpen(open)
{
	mRepeatable = repeatable;
	mActivated = false;

	mSound = sound;
}

////////////////////////////////////////////////////////////////////////
///
/// Destructor for the AnimEvent class.
///
/// @return void
///
////////////////////////////////////////////////////////////////////////
AnimEvent::~AnimEvent()
{
}

////////////////////////////////////////////////////////////////////////
///
/// This event's execution.
///
/// @return void
///
////////////////////////////////////////////////////////////////////////
void AnimEvent::CallActions()
{
	VCN_ASSERT_FAIL( "FIXME: Get right animation controller and start the animation" );
#if 0
	if(mRepeatable || !mActivated)
	{
		VCNNode* node = VCNNodeCore::GetInstance()->GetNode(mNodeId);
		if (node)
		{
			VCNAnimationController* animNode = static_cast<VCNAnimationController*>(node->GetChildNode(0));
			VCNAnimCore* animCore = VCNAnimCore::GetInstance();
			animCore->StartAnimation(node, mId);
			animNode->SetOpen(mOpen);
			if (mSound != VCNTXT(""))
			{
				SoundManager::GetInstance().PlaySound(mSound);
			}
			mActivated = true;
		}
	}
#endif
}
#endif