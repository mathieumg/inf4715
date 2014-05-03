///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#include "Precompiled.h"

// Project includes
#include "AnimJoint.h"
#include "AnimationController.h"

// Engine includes
#include "VCNNodes/NodeCore.h"
#include "VCNResources/ResourceCore.h"

VCN_TYPE( VCNAnimJoint, VCNResource ) ;


//-------------------------------------------------------------
/// Constructor
//-------------------------------------------------------------
VCNAnimJoint::VCNAnimJoint() 
	: VCNResource()
	, mDuration( 0.0f )
	, mTargetName( VCNTXT("") )
{
}

//-------------------------------------------------------------
/// Destructor
//-------------------------------------------------------------
VCNAnimJoint::~VCNAnimJoint()
{
}

//-------------------------------------------------------------
/// Finds the target node when we feed it a parent
//-------------------------------------------------------------
VCNNode* VCNAnimJoint::FindTargetNode(VCNNode* rootNode) const
{
	if ( GetTargetName().empty() )
		return rootNode;

	// Find the animation nodes in the tree
	std::vector<VCNNode*> searchResults;
	VCNNodeCore::GetInstance()->FindNodeType( VCNNode::Type()->GetTypeID(), searchResults, rootNode );

	// Now find the one we're supposed to animate
	for( VCNUInt i=0; i<searchResults.size(); ++i )
	{
		VCNNode* currentNode = searchResults[i];
		if( currentNode->GetTag() == GetTargetName() )
		{
			// We found him!
			return currentNode;
		}
	}

	return NULL;
}


//-------------------------------------------------------------
/// Add a position frame to our animation
//-------------------------------------------------------------
void VCNAnimJoint::AddPosFrame( VCNFloat time, const Vector3& posFrame )
{
	// Adjust the duration of our animation if needed
	if( time > mDuration )
		mDuration = time;

	// Make sure it doesn't already exist
	VCN_ASSERT( mPosFrames.find(time) == mPosFrames.end() );

	// Add the frame
	mPosFrames[time] = posFrame;
}

//-------------------------------------------------------------
/// Add a rotation frame to our animation
//-------------------------------------------------------------
void VCNAnimJoint::AddRotFrame( VCNFloat time, const VCNQuat& rotFrame )
{
	// Adjust the duration of our animation if needed
	if( time > mDuration )
		mDuration = time;

	// Make sure it doesn't already exist
	VCN_ASSERT( mRotFrames.find(time) == mRotFrames.end() );

	// Add the frame
	mRotFrames[time] = rotFrame;
}

//-------------------------------------------------------------
/// Add a scale frame to our animation
//-------------------------------------------------------------
void VCNAnimJoint::AddScaleFrame( VCNFloat time, const Vector3& scaleFrame )
{
	// Adjust the duration of our animation if needed
	if( time > mDuration )
		mDuration = time;

	// Make sure it doesn't already exist
	VCN_ASSERT( mScaleFrames.find(time) == mScaleFrames.end() );

	// Add the frame
	mScaleFrames[time] = scaleFrame;
}

//-------------------------------------------------------------
/// Return the bounding position frames for a certain time
//-------------------------------------------------------------
VCNFloat VCNAnimJoint::GetBoundingPositions( const VCNFloat time, Vector3& lowerBound, Vector3& upperBound )
{
	float percentageIn = -1.0f;

	// If the exact time exists (it could happen!), then return it for both
	if( mPosFrames.find(time) != mPosFrames.end() )
	{
		lowerBound = mPosFrames[time];
		upperBound = mPosFrames[time];
		percentageIn = 0.0f;
	}
	else if( mPosFrames.size() == 1 )
	{
		lowerBound = (*(mPosFrames.begin())).second;
		upperBound = (*(mPosFrames.begin())).second;
		percentageIn = 0.0f;
	}
	else if( mPosFrames.size() > 1 )
	{
		// Get the lower bound (we got screwed here, the map's lowerbound function is...)
		PosFrameMap::const_iterator itLower = mPosFrames.begin();
		PosFrameMap::const_iterator currIt = mPosFrames.begin();
		while( currIt != mPosFrames.end() )
		{
			if( (*currIt).first < time && (*currIt).first > (*itLower).first )
			{
				itLower = currIt;
			}
			++currIt;
		}
		lowerBound = (*itLower).second;
		
		// Get the upper bound
		PosFrameMap::const_iterator itUpper = mPosFrames.lower_bound(time);
		upperBound = (*itUpper).second;

		// Calculate how far in we are
		float timespan = (*itUpper).first - (*itLower).first;
		percentageIn = (time - (*itLower).first) / timespan;
	}

	return percentageIn;
}

//-------------------------------------------------------------
/// Return the bounding rotation frames for a certain time
//-------------------------------------------------------------
VCNFloat VCNAnimJoint::GetBoundingRotations( const VCNFloat time, VCNQuat& lowerBound, VCNQuat& upperBound )
{
	float percentageIn = -1.0f;

	// If the exact time exists (it could happen!), then return it for both
	if( mRotFrames.find(time) != mRotFrames.end() )
	{
		lowerBound = mRotFrames[time];
		upperBound = mRotFrames[time];
		percentageIn = 0.0f;
	}
	else if( mRotFrames.size() == 1 )
	{
		lowerBound = (*(mRotFrames.begin())).second;
		upperBound = (*(mRotFrames.begin())).second;
		percentageIn = 0.0f;
	}
	else if( mRotFrames.size() > 1 )
	{
		// Get the lower bound
		RotFrameMap::const_iterator itLower = mRotFrames.begin();
		RotFrameMap::const_iterator currIt = mRotFrames.begin();
		while( currIt != mRotFrames.end() )
		{
			if( (*currIt).first < time && (*currIt).first > (*itLower).first )
			{
				itLower = currIt;
			}
			++currIt;
		}
		lowerBound = (*itLower).second;

		// Get the upper bound
		RotFrameMap::const_iterator itUpper = mRotFrames.lower_bound(time);
		upperBound = (*itUpper).second;

		// Calculate how far in we are
		float timespan = (*itUpper).first - (*itLower).first;
		percentageIn = (time - (*itLower).first) / timespan;
	}

	return percentageIn;
}

//-------------------------------------------------------------
/// Return the bounding scale frames for a certain time
//-------------------------------------------------------------
VCNFloat VCNAnimJoint::GetBoundingScales( const VCNFloat time, Vector3& lowerBound, Vector3& upperBound )
{
	float percentageIn = -1.0f;

	// If the exact time exists (it could happen!), then return it for both
	if( mScaleFrames.find(time) != mScaleFrames.end() )
	{
		lowerBound = mScaleFrames[time];
		upperBound = mScaleFrames[time];
		percentageIn = 0.0f;
	}
	else if( mScaleFrames.size() == 1 )
	{
		lowerBound = (*(mScaleFrames.begin())).second;
		upperBound = (*(mScaleFrames.begin())).second;
		percentageIn = 0.0f;
	}
	else if( mScaleFrames.size() > 1 )
	{
		// Get the lower bound (we got screwed here, the map's lowerbound function is...)
		PosFrameMap::const_iterator itLower = mScaleFrames.begin();
		PosFrameMap::const_iterator currIt = mScaleFrames.begin();
		while( currIt != mScaleFrames.end() )
		{
			if( (*currIt).first < time && (*currIt).first > (*itLower).first )
			{
				itLower = currIt;
			}
			++currIt;
		}
		lowerBound = (*itLower).second;

		// Get the upper bound
		PosFrameMap::const_iterator itUpper = mScaleFrames.lower_bound(time);
		upperBound = (*itUpper).second;

		// Calculate how far in we are
		float timespan = (*itUpper).first - (*itLower).first;
		percentageIn = (time - (*itLower).first) / timespan;
	}

	return percentageIn;
}

//-------------------------------------------------------------
/// Return the position in the form of a vector at a certain 
/// time.
//-------------------------------------------------------------
VCNBool VCNAnimJoint::GetPositionAtTime( const VCNFloat time, Vector3& resultVector )
{
	Vector3 lowerBound, upperBound;
	const VCNFloat ratio = GetBoundingPositions( time, lowerBound, upperBound );

	if( ratio >= 0.0f )
	{
		resultVector = lowerBound + ((upperBound-lowerBound)*ratio);
		return true;
	}

	return false;
}

//-------------------------------------------------------------
/// Return the rotation in the form of a matrix at a certain 
/// time.
//-------------------------------------------------------------
VCNBool VCNAnimJoint::GetRotationAtTime( const VCNFloat time, Matrix4& resultMatrix )
{
	VCNQuat lowerBound, upperBound;
	const VCNFloat ratio = GetBoundingRotations( time, lowerBound, upperBound );

	if( ratio == 0.0f )
	{
		lowerBound.GetMatrix( resultMatrix );
		return true;
	}
	else if( ratio > 0.0f )
	{
		// Normalize both quats
		lowerBound.Normalize();
		upperBound.Normalize();

		// Slerp
		VCNQuat slerpQuat = VCNQuat::Slerp( lowerBound, upperBound, ratio );

		// Convert to matrix format
		slerpQuat.GetMatrix( resultMatrix );

		return true;
	}

	return false;
}

//-------------------------------------------------------------
/// Return the scale in the form of a vector at a certain 
/// time.
//-------------------------------------------------------------
VCNBool VCNAnimJoint::GetScaleAtTime( const VCNFloat time, Vector3& resultVector )
{
	Vector3 lowerBound, upperBound;
	const VCNFloat ratio = GetBoundingScales( time, lowerBound, upperBound );

	if( ratio >= 0.0f )
	{
		resultVector = lowerBound + ((upperBound-lowerBound)*ratio);
		return true;
	}

	return false;
}
