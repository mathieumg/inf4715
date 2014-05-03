///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#include "Precompiled.h"
#include "AnimLoader.h"

// Project includes
#include "AnimLoaderConstants.h"

// Engine includes
#include "VCNAnim/Anim.h"
#include "VCNAnim/AnimJoint.h"
#include "VCNResources/ResourceCore.h"

//-------------------------------------------------------------
VCNAnim* VCNAnimLoader::LoadAnim( const VCNString& filename )
{
	// Make sure we at least have a char and an extension
	VCN_ASSERT( filename.length() > 7 && "Incorrect filename!" );

	VCNAnim* retValue = NULL;

	// Is it an XML format or a binary format?
	if( filename.rfind(kXMLAnimExtension) != VCNString::npos )
	{
		retValue = LoadAnimXML( filename );
	}
	else
	{
		VCN_ASSERT_FAIL( "Unsupported animation extension!" );
	}

	// return the mesh pointer!
	return retValue;
}

//-------------------------------------------------------------
VCNAnim* VCNAnimLoader::LoadAnimXML( const VCNString& filename )
{
	// Create the new mesh to be filled!
	VCNAnim* newAnim = NULL;

	// Open the XML document
	XMLElementPtr pRootElem = LoadDocumentRoot( filename );
	VCN_ASSERT_MSG( pRootElem != NULL, "Could not load XML Animation!" );

	// Go through all the MeshElement nodes
	XMLNodeListPtr elementNodes;
	pRootElem->selectNodes( (VCNTChar*)kAttrAnimNode, &elementNodes );
	VCNLong animNodeCount;
	elementNodes->get_length( &animNodeCount );
	for( VCNLong i=0; i<animNodeCount; i++ )
	{
		// Create the new mesh to be filled!
		newAnim = new VCNAnim();

		// Get the element's node
		XMLNodePtr elementNode;
		elementNodes->get_item( i, &elementNode );

		// Load the base properties
		LoadResourceBaseProperties( elementNode, newAnim );

		// Go through all the joints that compose this anims
		XMLNodeListPtr jointNodes;
		elementNode->selectNodes( (VCNTChar*)kNodeAnimJoint, &jointNodes );
		VCNLong jointNodeCount;
		jointNodes->get_length( &jointNodeCount );
		for( VCNLong i=0; i<jointNodeCount; i++ )
		{
			// Get the mesh's node
			XMLNodePtr jointNode;
			jointNodes->get_item( i, &jointNode );

			// Load the joint
			VCNAnimJoint* animJoint = LoadJointXML( jointNode );

			// The name of the joint will be the name of the anim + the target joint
			animJoint->SetName( newAnim->GetName() + VCNTXT("_") + animJoint->GetTargetName() );

			// Add it to the resource manager
			VCNResID jointID = VCNResourceCore::GetInstance()->AddResource( animJoint->GetName(), animJoint );

			// Add it to the animation
			newAnim->AddJoint( jointID );
		}

		// Make sure we aren't loading it twice
		VCN_ASSERT( !VCNResourceCore::GetInstance()->GetResource<VCNAnim>( newAnim->GetName() ) );

		// Add it
		VCNResourceCore::GetInstance()->AddResource( newAnim->GetName(), newAnim );
	}

	// Free up the doc memory
	ReleaseDocument();

	return newAnim;
}

//-------------------------------------------------------------
VCNAnimJoint* VCNAnimLoader::LoadJointXML( XMLNodePtr jointNode )
{
	VCNAnimJoint* animJoint = new VCNAnimJoint();

	// Read the name of the target for the joint and store it
	VCNString tmpString;
	if( GetAttributeString(jointNode, kAttrAnimJointTarget, tmpString) )
	{
		animJoint->SetTargetName( tmpString );
	}

	// Temp variables for lecture
	VCNFloat fTime;
	Vector3 tmpVector;
	VCNQuat tmpQuat;
	VCNUInt count;
	VCNBool readSuccess;

	// Get the number of rotation frames
	GetAttributeUInt(jointNode, kAttrAnimJointPosCount, count);

	// Lets load up all the positions!
	XMLNodeListPtr posNodes;
	jointNode->selectNodes( (VCNTChar*)kNodeAnimJointPosition, &posNodes );
	VCNLong posNodeCount;
	posNodes->get_length( &posNodeCount );
	VCN_ASSERT( (long)count == posNodeCount );
	for( VCNLong i=0; i<posNodeCount; i++ )
	{
		// Get the mesh's node
		XMLNodePtr posNode;
		posNodes->get_item( i, &posNode );

		// Get the frame time for this position
		GetAttributeFloat(posNode, kAttrAnimJointPosFrameTime, fTime);

		// Get the x position
		GetAttributeFloat(posNode, kAttrAnimJointFramePosX, tmpVector.x);

		// Get the y position
		GetAttributeFloat(posNode, kAttrAnimJointFramePosY, tmpVector.y);

		// Get the z position
		GetAttributeFloat(posNode, kAttrAnimJointFramePosZ, tmpVector.z);

		// Add it to the joint
		animJoint->AddPosFrame( fTime, tmpVector );
	}

	// Get the number of rotation frames
	GetAttributeUInt(jointNode, kAttrAnimJointRotCount, count);

	// Lets load up all the rotations!
	XMLNodeListPtr rotNodes;
	jointNode->selectNodes( (VCNTChar*)kNodeAnimJointRotation, &rotNodes );
	VCNLong rotNodeCount;
	rotNodes->get_length( &rotNodeCount );
	VCN_ASSERT( (long)count == rotNodeCount );
	for( VCNLong i=0; i<rotNodeCount; i++ )
	{
		// Get the mesh's node
		XMLNodePtr rotNode;
		rotNodes->get_item( i, &rotNode );

		// Get the frame time for this position
		readSuccess = GetAttributeFloat(rotNode, kAttrAnimJointPosFrameTime, fTime);
		VCN_ASSERT(readSuccess);

		// Get the x of the quat
		readSuccess = GetAttributeFloat(rotNode, kAttrAnimJointFrameRotX, tmpQuat.x);
		VCN_ASSERT(readSuccess);

		// Get the y of the quat
		readSuccess = GetAttributeFloat(rotNode, kAttrAnimJointFrameRotY, tmpQuat.y);
		VCN_ASSERT(readSuccess);

		// Get the z of the quat
		readSuccess = GetAttributeFloat(rotNode, kAttrAnimJointFrameRotZ, tmpQuat.z);
		VCN_ASSERT(readSuccess);

		// Get the w of the quat
		readSuccess = GetAttributeFloat(rotNode, kAttrAnimJointFrameRotW, tmpQuat.w);
		VCN_ASSERT(readSuccess);

		// Add it to the joint
		animJoint->AddRotFrame( fTime, tmpQuat );
	}

	// Get the number of scale frames
	GetAttributeUInt(jointNode, kAttrAnimJointScaleCount, count);


	// Lets load up all the scales!
	XMLNodeListPtr scaleNodes;
	jointNode->selectNodes( (VCNTChar*)kNodeAnimJointScale, &scaleNodes );
	VCNLong scaleNodeCount;
	scaleNodes->get_length( &scaleNodeCount );
	VCN_ASSERT( (long)count == scaleNodeCount );
	for( VCNLong i=0; i<scaleNodeCount; i++ )
	{
		// Get the mesh's node
		XMLNodePtr scaleNode;
		scaleNodes->get_item( i, &scaleNode );

		// Get the frame time for this position
		readSuccess = GetAttributeFloat(scaleNode, kAttrAnimJointScaleFrameTime, fTime);
		VCN_ASSERT(readSuccess);

		// Get the x of the quat
		readSuccess = GetAttributeFloat(scaleNode, kAttrAnimJointFrameScaleX, tmpVector.x);
		VCN_ASSERT(readSuccess);

		// Get the y of the quat
		readSuccess = GetAttributeFloat(scaleNode, kAttrAnimJointFrameScaleY, tmpVector.y);
		VCN_ASSERT(readSuccess);

		// Get the z of the quat
		readSuccess = GetAttributeFloat(scaleNode, kAttrAnimJointFrameScaleZ, tmpVector.z);
		VCN_ASSERT(readSuccess);

		// Add it to the joint
		animJoint->AddScaleFrame( fTime, tmpVector );
	}

	// Return the joint
	return animJoint;
}
