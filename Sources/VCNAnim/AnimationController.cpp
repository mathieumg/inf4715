///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#include "Precompiled.h"
#include "AnimationController.h"

// Project includes
#include "AnimCore.h"
#include "AnimJoint.h"

// Engine includes
#include "VCNNodes/NodeCore.h"
#include "VCNResources/ResourceCore.h"

VCN_TYPE( VCNAnimationController, VCNAtom ) ;

//-------------------------------------------------------------
/// Constructor
//-------------------------------------------------------------
VCNAnimationController::VCNAnimationController()
: mAnimID( kInvalidAnimID )
, mJointID( kInvalidResID )
, mCurrentTime( 0.0f )
, mActive( false )
, mNodeTargetID( kInvalidNodeID )
, mLooping( false )
{
}

//-------------------------------------------------------------
/// Destructor
//-------------------------------------------------------------
VCNAnimationController::~VCNAnimationController()
{
}

//-------------------------------------------------------------
/// Called every frame to process data
//-------------------------------------------------------------
void VCNAnimationController::Process(const float elapsedTime)
{
	// If we're not active, then exit.
	if( !mActive )
		return; 

	// Lets get a joint (think clean)
	VCNAnimJoint* joint = VCNResourceCore::GetInstance()->GetResource<VCNAnimJoint>( mJointID );

	// If the joint is not available, then skip this frame
	if( joint )
	{
		// Get the new time and clamp it
		mCurrentTime += elapsedTime;
		if( mCurrentTime >= joint->GetDuration() )
		{
			if ( mLooping )
			{
				mCurrentTime = 0.0f;
			}
			else
			{
				// Clamp the animation time
				mCurrentTime = joint->GetDuration();

				// Turn ourselves off
				mActive = false;

				// Animation is finished, signal it to someone for removal.
				VCNAnimCore::GetInstance()->NotifyFinish( GetAnimID() );
			}
		}
		else if ( VCNNode* targetNode = VCNNodeCore::GetInstance()->GetNode(mNodeTargetID) )
		{
			Vector3 position;
			Matrix4 rotation;
			Vector3 scale( 1.0f, 1.0f, 1.0f );

			// Settle position
			if( joint->GetPositionAtTime( mCurrentTime, position ) )
			{
				targetNode->SetTranslation( position );
			}

			// Settle rotation
			if( joint->GetRotationAtTime( mCurrentTime, rotation ) )
			{
				targetNode->SetRotation( rotation );
			}

			// Settle position
			if( joint->GetScaleAtTime( mCurrentTime, scale ) )
			{
				targetNode->SetScale( scale );
			}
		}
	}

}
