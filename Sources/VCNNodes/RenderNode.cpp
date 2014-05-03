///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 

#include "Precompiled.h"
#include "RenderNode.h"

#include "VCNNodes/NodeCore.h"
#include "VCNRenderer/EffectCore.h"
#include "VCNRenderer/GPUProfiling.h"
#include "VCNRenderer/MaterialCore.h"
#include "VCNRenderer/RenderCore.h"
#include "VCNRenderer/XformCore.h"
#include "VCNResources/Mesh.h"
#include "VCNResources/ResourceCore.h"
#include "VCNUtils/Constants.h"
#include "VCNUtils/Frustum.h"
#include "VCNUtils/Ray.h"

// TODO: Refactor
#include "VCND3D9/D3D9Animator.h"

// HACK: Create VCNInput and update include
#include "GameApp/InputManager.h"
#include "VCNUtils/RenderTypes.h"

VCN_TYPE( VCNRenderNode, VCNNode ) ;

///////////////////////////////////////////////////////////////////////
/// Constructor
///////////////////////////////////////////////////////////////////////
VCNRenderNode::VCNRenderNode( VCNNodeID nodeID ) 
	: VCNNode( nodeID )
	, mMeshID( kInvalidResID )
	, mMaterialID( kInvalidResID )
	, mRender(true)
{
	// Check so render nodes don't become too big.
	static_assert( sizeof(VCNRenderNode) <= VCNNodePool::kBlockSize, "Render node is becoming a bit big");
}

///////////////////////////////////////////////////////////////////////
/// Destructor
///////////////////////////////////////////////////////////////////////
VCNRenderNode::~VCNRenderNode()
{
}

///////////////////////////////////////////////////////////////////////
/// Called every frame to process data
///////////////////////////////////////////////////////////////////////
void VCNRenderNode::Process(const float elapsedTime)
{
	if (!mActive)
		return;

	// Let the base class do its stuff
	VCNNode::Process( elapsedTime );
}

///////////////////////////////////////////////////////////////////////
const bool VCNRenderNode::UpdateWorldTransform()
{
	if ( VCNNode::UpdateWorldTransform() )
	{
		// Update mesh bounding sphere
		const VCNResID meshID = GetMeshID();
		if ( meshID != kInvalidResID )
		{
			VCNMesh* mesh = VCNResourceCore::GetInstance()->GetResource<VCNMesh>(meshID);
			const VCNSphere boundingSphere = mesh->GetBoundingSphere();

			// Transform the sphere relative to the render node
			const Vector3 ascSacle = GetWorldScale();
			const VCNFloat radius = boundingSphere.GetRadius() * std::max(ascSacle.x, std::max(ascSacle.y, ascSacle.z));

			mBoundingSphere.Set( radius, boundingSphere.GetCenter() * mWorld );
			mBoundingBox.vcMin = (mesh->GetBoundingBox().vcMin.MulComponents(ascSacle)) + GetWorldTranslation();
			mBoundingBox.vcMax = (mesh->GetBoundingBox().vcMax.MulComponents(ascSacle)) + GetWorldTranslation();
		}

		return true;
	}

	return false;
}


///////////////////////////////////////////////////////////////////////
/// Called every frame to render
///////////////////////////////////////////////////////////////////////
void VCNRenderNode::Render() const
{
	if (!mActive || !mRender)
		return;

	// Let the base class do its stuff
	VCNNode::Render();

	// Check if we are in the view frustum
	if ( IsInFrustum() )
	{
		// Trace the object being rendered
		//GPU_PROFILE_OBJECT_BLOCK( GetTag().c_str() );

		// Tell the transform manager what the new current transform is
		VCNXformCore::GetInstance()->SetWorldTransform( mWorld );
		
        VCNMaterialCore* materialCore = VCNMaterialCore::GetInstance();

		// Set the current material
		materialCore->SelectMaterial( mMaterialID );

		VCNMesh* mesh = VCNResourceCore::GetInstance()->GetResource<VCNMesh>( mMeshID );
		if ( const VCND3DAnimator* anim = GetComponent<VCND3DAnimator>() )
		{
			mesh->SetBoneTransforms( anim->GetTransformPtr() );
		}
        VCNNode* parent = VCNNodeCore::GetInstance()->GetNode(GetParent());

        VCNMaterial* material = materialCore->GetCurrentMaterial();

        VCNEffectParamSet& params = material->GetEffectParamSet();
        params.AddFloat(VCNTXT("Selected"), (IsSelected() || parent->IsSelected()) ? 1.0f : 0.0f);
        
        // Render the mesh
		VCNEffectCore::GetInstance()->RenderMesh( mMeshID, mBoundingSphere );
		
		mesh->SetBoneTransforms( nullptr );

#if !defined( FINAL )
		// Render the frame of the node
		if ( InputManager::GetInstance().IsKeyPressed( KEY_F ) )
		{
			const Matrix4 nodeFrame = GetWorldTransformation();
			const Vector3 center = nodeFrame.GetTranslation();
			VCNRenderCore::GetInstance()->DrawLine( center, center + nodeFrame.GetXAxis().Normalized(), VCNColor(1.0f, 0.0f, 0.0f) );
			VCNRenderCore::GetInstance()->DrawLine( center, center + nodeFrame.GetYAxis().Normalized(), VCNColor(0.0f, 1.0f, 0.0f) );
			VCNRenderCore::GetInstance()->DrawLine( center, center + nodeFrame.GetZAxis().Normalized(), VCNColor(0.0f, 0.0f, 1.0f) );
		}
#endif
	}
}

///////////////////////////////////////////////////////////////////////
VCNRenderNode* VCNRenderNode::Copy() const
{
	VCNRenderNode* nodeCopy = VCNNodeCore::GetInstance()->CreateNode<VCNRenderNode>();
	CopyTo( nodeCopy );
	return nodeCopy;
}

///////////////////////////////////////////////////////////////////////
void VCNRenderNode::CopyTo(VCNRenderNode* node) const
{
	VCNNode::CopyTo( node );

	node->mMeshID = mMeshID;
	node->mMaterialID = mMaterialID;
	node->mBoundingSphere = mBoundingSphere;
	node->mBoundingBox = mBoundingBox;
}

///////////////////////////////////////////////////////////////////////
VCNBool IsSelectableInHierarchie(const VCNNode* node)
{
	bool selectable = false;

	VCNNode* parent = 0;
	if (node->GetParent() != kInvalidResID)
	{
		parent = VCNNodeCore::GetInstance()->GetNode(node->GetParent());
	}

	selectable |= node->IsSelectable() || (parent && IsSelectableInHierarchie(parent));

	return selectable;
}

//////////////////////////////////////////////////////////////////////////
VCNBool VCNRenderNode::Intersects( const VCNRay& ray, VCNFloat& dist ) const
{
	if (!mActive)
		return false;

	if (IsSelectableInHierarchie(this))
	{
		VCNBool intersect = ray.Intersects(mBoundingSphere, dist);

		return intersect && dist >= 0.0f;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////
bool VCNRenderNode::IsInFrustum() const
{
	const Frustum& viewFrustum = VCNXformCore::GetInstance()->GetViewFrustum();
	return viewFrustum.SphereInFrustum( mBoundingSphere.GetCenter(), mBoundingSphere.GetRadius() );
}
