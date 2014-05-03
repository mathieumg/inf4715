///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#include "Precompiled.h"
#include "AnimCore.h"

// Project includes
#include "AnimationController.h"
#include "Anim.h"
#include "AnimJoint.h"

// Engine includes
#include "VCNNodes/NodeCore.h"
#include "VCNResources/ResourceCore.h"

VCN_TYPE( VCNAnimCore, VCNCore<VCNAnimCore> ) ;

//-------------------------------------------------------------
/// Constructor
//-------------------------------------------------------------
VCNAnimCore::VCNAnimCore()
{
}

//-------------------------------------------------------------
/// Destructor
//-------------------------------------------------------------
VCNAnimCore::~VCNAnimCore()
{
}

//-------------------------------------------------------------
/// Initialize
//-------------------------------------------------------------
VCNBool VCNAnimCore::Initialize()
{
	return BaseCore::Initialize();
}

///////////////////////////////////////////////////////////////////////
VCNBool VCNAnimCore::Uninitialize()
{
	return BaseCore::Uninitialize();
}


//-------------------------------------------------------------
/// Gets the first free anim ID
//-------------------------------------------------------------
VCNAnimID VCNAnimCore::GetFreeAnimID()
{
	static VCNAnimID sNextID = 0;
	return sNextID++;
}

//-------------------------------------------------------------
/// Start to play an animation on a node
//-------------------------------------------------------------
VCNAnimID VCNAnimCore::StartAnimation(VCNNode* node, VCNResID animResID, VCNFlags flags)
{
	// Determine what the ID of this new anim will be
	VCNAnimID newAnimID = GetFreeAnimID();

	// Get the animation resource
	VCNAnim* anim = VCNResourceCore::GetInstance()->GetResource<VCNAnim>( animResID );

	// Go through the anim's joints, and create controllers on the nodes
	for( VCNUInt i = 0; i < anim->GetJointCount(); i++ )
	{
		// Get the joint
		VCNAnimJoint* joint = anim->GetJoint(i);

		// Find the target node
		VCNNode* target = joint->FindTargetNode(node);
		if( target )
		{
			// If the animation is being started as local, create a node between the target and children 
			if ( flags & kLocal )
			{
				VCN_ASSERT_MSG( target->GetChildCount() > 0, "To use local, target node must have children" );

				VCNNode* animatedNode = VCNNodeCore::GetInstance()->CreateNode<VCNNode>();
				animatedNode->SetTag( StringBuilder() << VCNTXT("Anim_") << target->GetTag() );
				animatedNode->SetUseRelativeTransform( false );

				for (int i = target->GetChildCount()-1; i >= 0; --i)
				{
					VCNNode* childNode = target->GetChildNode(i);
					animatedNode->AttachChild(childNode->GetNodeID());
				}

				target->AttachChild( animatedNode->GetNodeID() );
				target = animatedNode;
			}

			// Create a new animation controller to animate the node
			VCNAnimationController* animController = new VCNAnimationController();

			// Tell the controller what he'll be doing
			animController->SetTarget( target->GetNodeID() );
			animController->SetAnimID( newAnimID );
			animController->SetJointID( joint->GetResourceID() );
			animController->SetLooping( (flags & kLoop) == kLoop );

			// Tell the controller to start updating himself with our data
			animController->StartAnimation();

			// Keep track of the controllers for this anim
			mAnimations[newAnimID].push_back( animController );
		}
	}

	return newAnimID;
}


//-------------------------------------------------------------
// Stop to play an animation
//-------------------------------------------------------------
void VCNAnimCore::StopAnimation(VCNAnimID animID)
{
	if (animID == kInvalidAnimID)
		return;

	ControllerList animControllers = mAnimations[animID];
	for (ControllerList::iterator it = animControllers.begin(), end = animControllers.end(); it != end; ++it)
	{
		(*it)->StopAnimation();
	}
}


//-------------------------------------------------------------
// Stop to play an animation
//-------------------------------------------------------------
VCNBool VCNAnimCore::IsAnimationFinished( const VCNAnimID animID )
{
	ControllerList animControllers = mAnimations[animID];
	for (ControllerList::iterator it = animControllers.begin(), end = animControllers.end(); it != end; ++it)
	{
		VCNAnimationController* animController = *it;
		if ( animController->IsActive() )
			return false;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////
void VCNAnimCore::NotifyFinish(VCNAnimID animID)
{
	if( IsAnimationFinished( animID ) )
	{
		StopAnimation( animID );
	}
}


///////////////////////////////////////////////////////////////////////
VCNBool VCNAnimCore::Process(const float elapsedTime)
{
	// Play animations
	AnimationList::iterator animIt = mAnimations.begin();
	while ( animIt != mAnimations.end() )
	{
		for (ControllerList::iterator ctrlIt = animIt->second.begin(), ctrlEnd = animIt->second.end(); ctrlIt != ctrlEnd; ++ctrlIt)
		{
			VCNAnimationController* animController = *ctrlIt;
			animController->Process( elapsedTime );
		}

		// Clean up animation which are finished
		if ( IsAnimationFinished(animIt->first) )
		{
			ControllerList& animControllers = animIt->second;

			for (ControllerList::iterator it = animControllers.begin(), end = animControllers.end(); it != end; ++it)
			{
				VCNAnimationController* animController = *it;
				delete animController;
			}

			animIt = mAnimations.erase( animIt );
		}
		else
		{
			++animIt;
		}
	}

	return true;
}
