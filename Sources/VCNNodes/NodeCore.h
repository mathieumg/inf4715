///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Core module for managing nodes.
///

#ifndef VCNNODECORE_H
#define VCNNODECORE_H

#pragma once

#include "VCNCore/Core.h"
#include "VCNNodes/NodeAllocator.h"
#include "VCNUtils/Macros.h"

class VCNNode;

///
/// Node manager
///
class VCNNodeCore : public VCNCore<VCNNodeCore>
{
  VCN_CLASS;

public:

  /// Default constructor
  VCNNodeCore();

  /// Destructor
  virtual ~VCNNodeCore();

  /// Initialize the module
  virtual VCNBool Initialize() override;

  /// Uninitialize the module
  virtual VCNBool Uninitialize() override;

  /// Allocates a unique node ID.
  const VCNNodeID AllocateNodeID();

  /// Creates a new node of a given type
  template <class T> 
  T* CreateNode();

  /// Creates a new node passing one argument to the class constructor
  template <class T, typename A1> 
  T* CreateNode(A1 a1);

  /// Creates a new node passing two arguments to the class constructor
  template <class T, typename A1, typename A2> 
  T* CreateNode(A1 a1, A2 a2);
  
  /// Creates a new node passing three arguments to the class constructor
  template <class T, typename A1, typename A2, typename A3> 
  T* CreateNode(A1 a1, A2 a2, A3 a3);

  /// Creates a new node passing four arguments to the class constructor
  template <class T, typename A1, typename A2, typename A3, typename A4> 
  T* CreateNode(A1 a1, A2 a2, A3 a3, A4 a4);

  /// Creates a new node passing five arguments to the class constructor
  template <class T, typename A1, typename A2, typename A3, typename A4, typename A5> 
  T* CreateNode(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5);

  /// Creates a new node passing six arguments to the class constructor
  template <class T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6> 
  T* CreateNode(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6);

  /// Destroy a node
  void DestroyNode( VCNNodeID nodeID );

  /// Remove a node from the map
  void RemoveNode( VCNNodeID nodeID );
  
  /// Retrieve a node
  VCNNode* GetNode( VCNNodeID nodeID ) const;

  /// Access the root node of the scene
  VCNNode* GetRootNode() const;

  /// Creates a duplicate of a node tree
  VCNNode* CopyTree( VCNNodeID nodeID );

  /// Inserts a node in an existing tree
  void InsertNodeBefore( VCNNodeID insertedNodeID, VCNNodeID positionNodeID, VCNBool inheritXForm = false );
  void InsertNodeAfter( VCNNodeID insertedNodeID, VCNNodeID positionNodeID, VCNBool inheritXForm = false );

  /// Finds all the nodes of a given type in the specified tree (or from the scene root)
  void FindNodeType( type_id nodeType, std::vector<VCNNode*>& nodes, VCNNode* rootNode = NULL );

  /// Add a node for quick search by its name
  void PushName(const VCNString& name, const VCNNodeID nodeID);

  /// Returns a node by its name
  const VCNNodeID GetNodeByName(const VCNString& name);

protected:

  typedef std::vector<VCNNode*>           NodeList;
  typedef std::map<VCNString, VCNNodeID>  NodeNameMap;
  typedef NodeNameMap::iterator           NodeNameIterator;

  // The root node of the scene
  VCNNode*              mRootNode;

  // The vector containing all the nodes in the scene
  NodeList              mNodes;
  VCNNodePool           mNodePool;
  
  // Node name map
  NodeNameMap           mNodeByName;

};

///////////////////////////////////////////////////////////////////////
inline VCNNode* VCNNodeCore::GetRootNode() const
{
  return mRootNode;
}

///////////////////////////////////////////////////////////////////////
inline const VCNNodeID VCNNodeCore::AllocateNodeID()
{
  // Generate new node ID
  const VCNNodeID newNodeID = (VCNNodeID)mNodes.size();
  // Reserve slot
  mNodes.push_back( NULL );
  // Return the new ID
  return newNodeID;
}

///////////////////////////////////////////////////////////////////////
inline VCNNode* VCNNodeCore::GetNode( VCNNodeID nodeID ) const
{
  // Fetch the node if the ID is valid
  // We don't do this in final cause we suppose the software had properly been tested.
  if( nodeID < mNodes.size() )
    return mNodes[nodeID];

  return NULL;
}

///////////////////////////////////////////////////////////////////////
template <class T>
inline T* VCNNodeCore::CreateNode()
{
  // Generate new node ID
  const VCNNodeID newNodeID = AllocateNodeID();
  // Allocate the node and give it an ID
  T* newNode = new(mNodePool.Allocate<T>()) T(newNodeID);
  // Push it to the node vector
  return static_cast<T*>(mNodes[ newNodeID ] = newNode);
}

///////////////////////////////////////////////////////////////////////
template <class T, typename A1>
inline T* VCNNodeCore::CreateNode(A1 a1)
{
  const VCNNodeID newNodeID = AllocateNodeID();
  T* newNode = new(mNodePool.Allocate<T>()) T( newNodeID, a1 );
  return static_cast<T*>( mNodes[ newNodeID ] = newNode );
}

///////////////////////////////////////////////////////////////////////
template <class T, typename A1, typename A2>
inline T* VCNNodeCore::CreateNode(A1 a1, A2 a2)
{
  const VCNNodeID newNodeID = AllocateNodeID();
  T* newNode = new(mNodePool.Allocate<T>()) T( newNodeID, a1, a2 );
  return static_cast<T*>( mNodes[ newNodeID ] = newNode );
}

///////////////////////////////////////////////////////////////////////
template <class T, typename A1, typename A2, typename A3>
inline T* VCNNodeCore::CreateNode(A1 a1, A2 a2, A3 a3)
{
  const VCNNodeID newNodeID = AllocateNodeID();
  T* newNode = new(mNodePool.Allocate<T>()) T( newNodeID, a1, a2, a3 );
  return static_cast<T*>( mNodes[ newNodeID ] = newNode );
}

///////////////////////////////////////////////////////////////////////
template <class T, typename A1, typename A2, typename A3, typename A4>
inline T* VCNNodeCore::CreateNode(A1 a1, A2 a2, A3 a3, A4 a4)
{
  const VCNNodeID newNodeID = AllocateNodeID();
  T* newNode = new(mNodePool.Allocate<T>()) T( newNodeID, a1, a2, a3, a4 );
  return static_cast<T*>( mNodes[ newNodeID ] = newNode );
}

///////////////////////////////////////////////////////////////////////
template <class T, typename A1, typename A2, typename A3, typename A4, typename A5>
inline T* VCNNodeCore::CreateNode(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5)
{
  const VCNNodeID newNodeID = AllocateNodeID();
  T* newNode = new(mNodePool.Allocate<T>()) T( newNodeID, a1, a2, a3, a4, a5 );
  return static_cast<T*>( mNodes[ newNodeID ] = newNode );
}

///////////////////////////////////////////////////////////////////////
template <class T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
inline T* VCNNodeCore::CreateNode(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6)
{
  const VCNNodeID newNodeID = AllocateNodeID();
  T* newNode = new(mNodePool.Allocate<T>()) T( newNodeID, a1, a2, a3, a4, a5, a6 );
  return static_cast<T*>( mNodes[ newNodeID ] = newNode );
}

#endif // VCNNODECORE_H
