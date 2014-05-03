///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Node module implementation
///

#include "Precompiled.h"
#include "NodeCore.h"

#include "VCNNodes/Node.h"
#include "VCNUtils/Constants.h"
#include "PhysicsComponent.h"

VCN_TYPE( VCNNodeCore, VCNCore<VCNNodeCore> ) ;

///////////////////////////////////////////////////////////////////////
/// Constructor
///////////////////////////////////////////////////////////////////////
VCNNodeCore::VCNNodeCore() 
 : VCNCore<VCNNodeCore>()
 , mRootNode(NULL)
{
}

///////////////////////////////////////////////////////////////////////
/// Destructor
///////////////////////////////////////////////////////////////////////
VCNNodeCore::~VCNNodeCore()
{
  // Proper delete of nodes should be done through Uninitialize();
  VCN_ASSERT( mNodes.empty() );
}


///////////////////////////////////////////////////////////////////////
///
/// Initialize the node module. We create the initial root that will 
/// own all other sub-nodes.
///
/// @return true if successful
///
VCNBool VCNNodeCore::Initialize()
{
  if ( !BaseCore::Initialize() )
    return false;

  // Make sure we don't have unwanted invitees
  VCN_ASSERT( mNodes.empty() );

  // Create the root node
  mRootNode = CreateNode<VCNNode>();

  // We're initialized
  return mRootNode != 0;
}


///////////////////////////////////////////////////////////////////////
///
/// Releases all node resources.
///
/// @return normally true is returned.
///
VCNBool VCNNodeCore::Uninitialize()
{
  for ( VCNUInt i = 0; i < mNodes.size(); ++i ) 
  {
    // Delete through NodeAllocator
    mNodePool.Deallocate( mNodes[i] );
  }
  mNodes.clear();

  return BaseCore::Uninitialize();
}

///////////////////////////////////////////////////////////////////////
/// Destroy a node
///////////////////////////////////////////////////////////////////////
void VCNNodeCore::DestroyNode( VCNNodeID nodeID )
{
  VCN_ASSERT( nodeID < mNodes.size() );
  if( nodeID < mNodes.size() )
  {
	  VCNNode* n = mNodes[nodeID];
    // Delete through NodeAllocator
    mNodePool.Deallocate( n );
    mNodes[nodeID] = NULL;
  }
}

///////////////////////////////////////////////////////////////////////
void VCNNodeCore::RemoveNode( VCNNodeID nodeID )
{
  if( nodeID < mNodes.size() )
  {
	  VCNNode* n = mNodes[nodeID];
	  if (n)
	  {
		  VCNPhysicsComponent* pc = n->GetComponent<VCNPhysicsComponent>();
		  if (pc)
		  {
			  pc->DestroyPhysicsActor();
		  }
	  }
    mNodes[nodeID] = NULL;
  }
}

///////////////////////////////////////////////////////////////////////
VCNNode* VCNNodeCore::CopyTree( VCNNodeID nodeID )
{
  // Get the node at the root of the tree
  VCNNode* treeRoot = GetNode( nodeID );

  // Create a copy of the root
  VCNNode* copyRoot = treeRoot->Copy();
    
  // Copy all of the children
  VCNNode::ChildrenList::iterator currChild = treeRoot->mChildren.begin();
  while( currChild != treeRoot->mChildren.end() )
  {
	  VCNNode* currChildNode = GetNode(*currChild);
    // Get the child and copy him
    VCNNode* childCopy = CopyTree( (*currChild) );

	if (currChildNode->HasProperty(VCNTXT("BaseName")))
	{
		VCNString* currName = currChildNode->GetProperty(VCNTXT("BaseName")).Cast<VCNString*>();
		PushName(*currName, childCopy->GetNodeID());
	}


    // Attach him to the root
    copyRoot->AttachChild( childCopy->GetNodeID() );

    // Present him to his parents
    childCopy->SetParent( copyRoot->GetNodeID() );

    // Go to the next element in the list
    ++currChild;
  }

  return copyRoot;
}

///////////////////////////////////////////////////////////////////////
void VCNNodeCore::InsertNodeBefore( VCNNodeID insertedNodeID, VCNNodeID positionNodeID, VCNBool inheritXForm )
{
  // Get the node at the root of the insertion
  VCNNode* positionNode = GetNode( positionNodeID );

  // Get the node to be inserted
  VCNNode* insertedNode = GetNode( insertedNodeID );

  // Go on the parent of the insertion point and remove that one child and add the new one
  VCNNodeID parentID = positionNode->GetParent();
  VCNNode* parentNode = NULL;
  if( parentID != kInvalidResID )
  {
    parentNode = GetNode(parentID);
    if( parentNode )
    {
      parentNode->AttachChild(insertedNodeID);
      parentNode->RemoveChild(positionNodeID);
    }
  }

  // The insertion position becomes our child and his parent becomes our parent
  insertedNode->AttachChild( positionNodeID );

  // Do we want our new node to pick up the transforms
  if( inheritXForm )
  {
    insertedNode->mLocalRotation = positionNode->mLocalRotation;
    insertedNode->mLocalTranslation = positionNode->mLocalTranslation;
    insertedNode->mLocalScale = positionNode->mLocalScale;
        positionNode->ResetTransforms();
  }
}

///////////////////////////////////////////////////////////////////////
void VCNNodeCore::InsertNodeAfter( VCNNodeID insertedNodeID, VCNNodeID positionNodeID, VCNBool inheritXForm )
{
  // Get the node at the root of the insertion
  VCNNode* insertionRoot = GetNode( positionNodeID );

  // Get the node to be inserted
  VCNNode* insertedNode = GetNode( insertedNodeID );

  // The inserted node will take on all the children
  VCNNode::ChildrenList::iterator currChild = insertionRoot->mChildren.begin();
  while( currChild != insertionRoot->mChildren.end() )
  {
    // Get the node to be inserted
    VCNNode* childNode = GetNode( (*currChild) );

    // Attach him to the inserted node
    insertedNode->AttachChild( childNode->GetNodeID() );

    // Set the child's new parent
    childNode->SetParent( insertedNode->GetNodeID() );

    // Go to the next element in the list
    ++currChild;
  }

  // Clear the parent's children now that they've been transfered
    insertionRoot->mChildren.clear();

  // Add the inserted node as the only child
    insertionRoot->AttachChild( insertedNodeID );

  // Set the parent of the inserted node
  insertedNode->SetParent( positionNodeID );

  // Do we want our new node to pick up the transforms
  if( inheritXForm )
  {
    insertedNode->mLocalRotation = insertionRoot->mLocalRotation;
    insertedNode->mLocalTranslation = insertionRoot->mLocalTranslation;
    insertedNode->mLocalScale = insertionRoot->mLocalScale;
    insertionRoot->ResetTransforms();
  }
}

///////////////////////////////////////////////////////////////////////
void VCNNodeCore::FindNodeType(type_id nodeType, std::vector<VCNNode*>& nodes, VCNNode* rootNode)
{
  // Use the scene root if none is specified.
  if( !rootNode )
    rootNode = GetRootNode();

  // If the current root is of the right type, then add him
  if( rootNode->GetRealType()->IsA(nodeType) )
    nodes.push_back( rootNode );

  // Go through all the children
  VCNNode::ChildrenList::iterator currChild = rootNode->mChildren.begin();
  while( currChild != rootNode->mChildren.end() )
  {
    // Get the node to be inserted
    VCNNode* childNode = GetNode( (*currChild) );

    // Check him out
    FindNodeType( nodeType, nodes, childNode );

    // Go to the next element in the list
    ++currChild;
  }
}

//////////////////////////////////////////////////////////////////////////
const VCNNodeID VCNNodeCore::GetNodeByName(const VCNString& name)
{
  NodeNameIterator fItr = mNodeByName.find( name );
  if ( fItr != mNodeByName.end() )
    return fItr->second;
  return kInvalidNodeID;
}

//////////////////////////////////////////////////////////////////////////
void VCNNodeCore::PushName(const VCNString& name, const VCNNodeID nodeID)
{
  mNodeByName[name] = nodeID;
}
