///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// The Vicuna Render Node class.
/// TODO: Move to VCNRenderer
///

#ifndef VCNRENDERNODE_H
#define VCNRENDERNODE_H

#pragma once

#include "VCNNodes/Node.h"
#include "VCNUtils/Sphere.h"
#include "VCNUtils/RenderTypes.h"
#include "VCNUtils/Aabb.h"

// Forward declarations
class VCNRay;
struct VCNColor;

//-------------------------------------------------------------
// The render node class
//-------------------------------------------------------------
class VCNRenderNode : public VCNNode
{
  VCN_CLASS;

public:

  VCNRenderNode( VCNNodeID nodeID );
  virtual ~VCNRenderNode();

  // Checks if the node is in the view frustum
  bool IsInFrustum() const;

  // Accessors to the mesh
  void SetMeshID( VCNResID resID );
  VCNResID GetMeshID() const;

  // Accessors to the material
  void SetMaterialID( VCNResID resID );
  const VCNResID GetMaterialID() const;

  /// Returns the bounding sphere of the node.
  const VCNSphere& GetBoundingSphere() const { return mBoundingSphere; }
  const VCNAabb& GetBoundingBox() const { return mBoundingBox; }

  // Called every frame (before the rendering pass)
  virtual void Process(const float elapsedTime) override;

  // Called every frame to render
  virtual void Render() const override;

  /// Check if the node's mesh intersects with the ray
  virtual VCNBool Intersects(const VCNRay& ray, VCNFloat& dist) const;

  inline void EnableRender(VCNBool enable) { mRender = enable; }

protected:

  // Copy field to specified node.
  void CopyTo(VCNRenderNode* node) const;

  // Copy this node and return the copy
  virtual VCNRenderNode* Copy() const override;

  // Update transformations
  virtual const bool UpdateWorldTransform() override;

// Data members

  VCNResID  mMeshID;
  VCNResID  mMaterialID;
  VCNSphere mBoundingSphere;
  VCNAabb	mBoundingBox;
  bool		mRender;
};

//
// INLINES
//

///////////////////////////////////////////////////////////////////////
inline void VCNRenderNode::SetMeshID( VCNResID resID )
{
  mMeshID = resID;
}


///////////////////////////////////////////////////////////////////////
inline VCNResID VCNRenderNode::GetMeshID() const
{
  return mMeshID;
}


///////////////////////////////////////////////////////////////////////
inline void VCNRenderNode::SetMaterialID( VCNResID resID )
{
  mMaterialID = resID;
}


///////////////////////////////////////////////////////////////////////
inline const VCNResID VCNRenderNode::GetMaterialID() const
{
  return mMaterialID;
}


///////////////////////////////////////////////////////////////////////

#endif // VCNRENDERNODE_H
