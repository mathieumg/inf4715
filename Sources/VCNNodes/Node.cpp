///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Node implementation
///

#include "Precompiled.h"
#include "Node.h"

// Engine includes
#include "VCNUtils/Constants.h"
#include "VCNNodes/NodeCore.h"
#include "VCNUtils/RenderTypes.h"
#include "VCNRenderer/RenderCore.h"

VCN_TYPE( VCNNode, VCNAtom );

///////////////////////////////////////////////////////////////////////
///
/// Constructs the base node
/// 
/// @param nodeID [IN] Unique node ID given by the node manager.
///
VCNNode::VCNNode( VCNNodeID nodeID )
	: mNodeID(nodeID)
	, mParentID(kInvalidResID)
	, mLocalTranslation(0.0f, 0.0f, 0.0f)
	, mLocalScale(1.0f, 1.0f, 1.0f)
	, mComputeTransforms(true)
	, mSelectable(false)
	, mSaveable(false)
	, mIsSelected(false)
	, mActive(true)
	, mRelativeTransform(true)
	, m_Events( VCNEventManager::Create() )
	, m_LockedForUpdate(false)
{
}


///////////////////////////////////////////////////////////////////////
///
/// Destroy a node and remove self from its parent if any.
///
VCNNode::~VCNNode()
{
	ChildrenList children = mChildren;

	// Detach from parent if any.
	if (mParentID != kInvalidResID)
	{
		VCNNode* parent = VCNNodeCore::GetInstance()->GetNode( mParentID );
		if (parent)
		{
			parent->RemoveChild( mNodeID );
		}
	}

	ChildrenList::const_iterator currChild = children.begin(), end = children.end();
	while( currChild != end )
	{
		VCNNodeCore::GetInstance()->DestroyNode( *currChild );
		++currChild;
	}
}



///////////////////////////////////////////////////////////////////////
///
/// Set the node to default values
///
void VCNNode::Reset()
{
	mParentID = kInvalidResID;
	mChildren.clear();
	ResetTransforms();
	RemoveAllComponents();
}


///////////////////////////////////////////////////////////////////////
///
/// Do any grunt work that doesn't involve rendering
/// 
/// @param elapsedTime [IN] time elapsed since last frame
///
void VCNNode::Process(const float elapsedTime)
{
	if (!mActive)
		return;

	// Fire all the delayed function calls
	GetEvents().FireInvokedFunctions(this);

	const bool parentWasUpdated = UpdateWorldTransform();

	m_LockedForUpdate = true;

	std::for_each( m_UpdatableComponents.begin(), m_UpdatableComponents.end(), 
		std::bind2nd(std::mem_fun1( &VCNIUpdatable::Update), elapsedTime));

	m_LockedForUpdate = false;

	for(std::size_t i = 0; i < m_ToRemove.size(); ++i)
		RemoveComponent(m_ToRemove[i]);

	m_ToRemove.clear();

	// And now process all the child nodes
	VCNNodeCore* nodeCore = VCNNodeCore::GetInstance();
	ChildrenList::iterator currChild = mChildren.begin(), end = mChildren.end();
	while( currChild != end )
	{
		VCNNode* childNode = nodeCore->GetNode( (*currChild) );
		if( childNode )
		{
			childNode->mComputeTransforms |= parentWasUpdated;
			childNode->Process( elapsedTime );
		}
		++currChild;
	}
}


///////////////////////////////////////////////////////////////////////
///
/// Updates the world transformation matrix. The matrix gets cached for 
/// faster usage afterward. Any changes to local transforms need to set
/// @mComputeTransforms to true for the computation to occur at next frame.
///
/// @return true if the world transformation was updated.
///
const bool VCNNode::UpdateWorldTransform()
{
	bool wasUpated = false;

	VCNNodeCore* nodeCore = VCNNodeCore::GetInstance();
	VCNNode* parent = nodeCore->GetNode(mParentID);

	mComputeTransforms |= (parent && parent->HasTransformChanged());
	if ( mComputeTransforms )
	{
		mWorld = ComputeWorldTransform();
		wasUpated = true;
		mComputeTransforms = false;
	}

	return wasUpated;
}


///////////////////////////////////////////////////////////////////////
///
/// Locally computes the node world transformation and returns it.
/// M = TQS (matResult = matTrans * matRotation * matScale)
///
Matrix4 VCNNode::ComputeWorldTransform() const
{
	if ( mRelativeTransform )
	{
		// Using this mode, translation and rotation IS modified relative to parent transform.

		// Get SRT matrices
		Matrix4 translate, rotate, scale;
		translate.BuildTranslation( mLocalTranslation );
		mLocalRotation.GetMatrix( rotate );
		scale.BuildScale( mLocalScale );
		Matrix4 worldTransform = scale * rotate * translate;

		// And now apply the world transformation of the parent
		// Root is always identity, no need to consider it.
		const VCNNodeCore* nodeCore = VCNNodeCore::GetInstance();
		const VCNNode* parent = nodeCore->GetNode(mParentID);
		if( parent && parent != nodeCore->GetRootNode() )
		{
			worldTransform *= parent->GetWorldTransformation();
		}

		return worldTransform;
	}
	else
	{
		// Using this mode, translation and rotation IS NOT modified by parent transform.
		// e.g. useful for animated nodes

		Vector3 worldTranslation = mLocalTranslation;
		Vector3 worldScale = mLocalScale;
		VCNQuat worldRotation = mLocalRotation;

		const VCNNodeCore* nodeCore = VCNNodeCore::GetInstance();
		const VCNNode* parent = nodeCore->GetNode(mParentID);
		while ( parent )
		{
			worldTranslation += parent->GetLocalTranslation();
			worldRotation *= parent->GetLocalRotation();
			worldScale = worldScale.MulComponents(parent->GetLocalScale());

			parent = nodeCore->GetNode( parent->GetParent() );
		}

		Matrix4 translate, rotate, scale;
		translate.BuildTranslation( worldTranslation );
		worldRotation.GetMatrix( rotate );
		scale.BuildScale( worldScale );

		return scale * rotate * translate;
	}
}


///////////////////////////////////////////////////////////////////////
///
/// Passes rendering to children since a base node itself has nothing 
/// to render.
///
void VCNNode::Render() const
{
	if (!mActive)
		return;

	// Draw all renderable components
	std::for_each( m_DrawableComponents.begin(), m_DrawableComponents.end(), 
		std::mem_fun(&VCNIDrawable::Draw));

	if ( mChildren.empty() )
		return;

    if(IsSelected())
    {
        VCNRenderCore* renderer = VCNRenderCore::GetInstance();
        VCNRenderState oldState = renderer->GetShadeMode();
        renderer->SetShadeMode(VCNRenderState::RS_SHADE_WIREFRAME);

        VCNNode* nonconst = const_cast<VCNNode*>(this);
        nonconst->SetScale(1.2f);
        // Draw all renderable components
        std::for_each( m_DrawableComponents.begin(), m_DrawableComponents.end(), 
            std::mem_fun(&VCNIDrawable::Draw));
        nonconst->SetScale(1.0f);
        renderer->SetShadeMode(oldState);
    }

	VCNNodeCore* nodeCore = VCNNodeCore::GetInstance();

	// And now render all the child nodes
	for (ChildrenList::const_iterator currChild = mChildren.begin(), end = mChildren.end(); currChild != end; ++currChild)
	{
		// Get the child and render him
		VCNNode* childNode = nodeCore->GetNode( *currChild );
		if( childNode )
		{
			childNode->Render();
		}
	}
}


///////////////////////////////////////////////////////////////////////
///
/// Attach a child to self. 
///
/// @remark Old parent will be automatically discarded.
/// 
/// @param childID      [IN] node ID to be attached.
/// @param insertAtHead [IN] indicates if the child is to be added at 
///                          the end or beginning of the children array.
///
///
void VCNNode::AttachChild( VCNNodeID childID, const VCNBool insertAtHead /*= false*/ )
{
	VCNNode* childNode = VCNNodeCore::GetInstance()->GetNode( childID );

	// If the node exists
	if( childNode )
	{
		// Detach from old parent if any
		if ( childNode->GetParent() != kInvalidNodeID )
		{
			VCNNode* parentNode = VCNNodeCore::GetInstance()->GetNode( childNode->GetParent() );
			parentNode->RemoveChild( childID );
		}

		if (!insertAtHead)
		{
			mChildren.push_back( childID );
		}
		else
		{
			mChildren.insert( mChildren.begin(), childID );
		}
		childNode->SetParent( mNodeID );
	}
}


///////////////////////////////////////////////////////////////////////
///
/// Removes a given child node. 
///
/// @remark The node is given to new parenting.
/// 
/// @param childID  [IN] node ID to be removed.
///
void VCNNode::RemoveChild( VCNNodeID childID )
{
	if (!mChildren.empty())
		mChildren.erase( std::find(mChildren.begin(), mChildren.end(), childID) );

	// If the node exists, remove its parent
	VCNNode* childNode = VCNNodeCore::GetInstance()->GetNode( childID );
	if( childNode )
		childNode->SetParent( kInvalidResID );
}


///////////////////////////////////////////////////////////////////////
///
/// Template method to create a new copy of a node.
///
/// @return a copy of the node
///
VCNNode* VCNNode::Copy() const
{
	VCNNode* nodeCopy = VCNNodeCore::GetInstance()->CreateNode<VCNNode>();
	CopyTo( nodeCopy );
	return nodeCopy;
}

///////////////////////////////////////////////////////////////////////
void VCNNode::CopyTo(VCNNode* node) const
{
	VCN_ASSERT( node );

	node->mLocalRotation = mLocalRotation;
	node->mLocalTranslation = mLocalTranslation;
	node->mLocalScale = mLocalScale;
	node->mTag = mTag;

	std::for_each( std::begin(m_Components), std::end(m_Components), [&node](VCNNode::ComponentPair pair)
	{
		VCNIComponent::Ptr copy = pair.second->Copy();
		if (!copy)
			return;

		node->AddComponent( copy );
	});

}


///////////////////////////////////////////////////////////////////////
void VCNNode::GetChildrenArray( std::vector<VCNNodeID>& children ) const
{
	children.clear();
	ChildrenList::const_iterator currChild = mChildren.begin();
	while( currChild != mChildren.end() )
	{
		children.push_back( (*currChild) );
		++currChild;
	}
}

///////////////////////////////////////////////////////////////////////
VCNNode* VCNNode::GetChildNode( unsigned int index ) const
{
	if (index >= mChildren.size()) 
		return 0;

	ChildrenList::const_iterator itr = mChildren.begin();
	for (unsigned int i = 0; i < index; i++)
	{
		++itr;
	}
	VCNNode* node = VCNNodeCore::GetInstance()->GetNode(*itr);

	return node;
}

///////////////////////////////////////////////////////////////////////
VCNNode* VCNNode::GetAncestor() 
{
	VCNNode* parent = 0;
	if (GetParent() != kInvalidResID)
	{
		parent = VCNNodeCore::GetInstance()->GetNode(GetParent());

		if (parent != VCNNodeCore::GetInstance()->GetRootNode())
			return parent->GetAncestor();
	}

	return this;
}

///////////////////////////////////////////////////////////////////////
Vector3 VCNNode::GetWorldTranslation() const
{
	return GetWorldTransformation().GetTranslation();
}

///////////////////////////////////////////////////////////////////////
Vector3 VCNNode::GetWorldScale() const
{
	Vector3 scale = mLocalScale;
	if (mParentID != kInvalidResID && mParentID != kRootNodeID)
	{
		const VCNNode* parent = VCNNodeCore::GetInstance()->GetNode( mParentID );
		const Vector3 pscale = parent->GetWorldScale();
		scale.x *= pscale.x;
		scale.y *= pscale.y;
		scale.z *= pscale.z;
	}

	return scale;
}

///////////////////////////////////////////////////////////////////////
VCNFloat VCNNode::GetWorldUniformScale() const
{
	const Vector3 scale = GetWorldScale();
	return (scale.x + scale.y + scale.z) / 3.0f;
}

///////////////////////////////////////////////////////////////////////
Matrix4 VCNNode::GetWorldRotation() const
{
	Matrix4 rot;
	mLocalRotation.GetMatrix( rot );
	if (mParentID != kInvalidResID)
	{
		const VCNNode* parent = VCNNodeCore::GetInstance()->GetNode( mParentID );
		rot = parent->GetWorldRotation() * rot;
	}

	return rot;
}

void VCNNode::AddComponent(const VCNIComponent::Ptr& component)
{
	VCN_ASSERT( !m_LockedForUpdate && "Component Addition not supported during update. Use EventManager::InvokeLater instead");
	// Only one component per type is allowed per node
	VCN_ASSERT( m_Components.find(component->GetType()) == std::end(m_Components) );

	m_Components[component->GetType()] = component;
	component->SetOwner( this );

	if (component->IsDrawable())
	{
		m_DrawableComponents.push_back( component->ToDrawable() );
	}

	if (component->IsUpdatable())
	{
		m_UpdatableComponents.push_back( component->ToUpdatable() );
	}
}

void VCNNode::AddComponent(VCNIComponent* component)
{
	VCN_ASSERT(component != nullptr);
	AddComponent( VCNIComponent::Ptr(component) );
}

VCNIComponent::Ptr VCNNode::GetComponent(VCNIComponent::Type type) const
{
	auto found = m_Components.find(type);
	if (found == std::end(m_Components))
		return VCNIComponent::Ptr(nullptr);

	return found->second;
}

VCNIComponent::Ptr VCNNode::GetComponent(VCNIComponent::Type type)
{
	return const_cast<const VCNNode*>(this)->GetComponent(type);
}

VCNBool VCNNode::HasComponent(VCNIComponent::Type type) const
{
	return m_Components.find(type) != std::end(m_Components);
}

void VCNNode::RemoveComponent(VCNIComponent::Type type)
{
	if (m_LockedForUpdate)
	{
		m_ToRemove.push_back(type);
		return;
	}

	ComponentMap::iterator mainitr = m_Components.find(type);
	VCNIComponent::Ptr component = mainitr->second;

	if (component->IsDrawable())
	{
		const VCNIDrawable* drawable = component->ToDrawable();
		auto itr = std::find( m_DrawableComponents.begin(), m_DrawableComponents.end(), drawable);
		VCN_ASSERT( itr != m_DrawableComponents.end() );

		m_DrawableComponents.erase( itr );
	}

	if (component->IsUpdatable())
	{
		const VCNIUpdatable* updatable = component->ToUpdatable();
		auto itr = std::find( m_UpdatableComponents.begin(), m_UpdatableComponents.end(), updatable);
		VCN_ASSERT( itr != m_UpdatableComponents.end() );

		m_UpdatableComponents.erase( itr );
	}

	m_Components.erase( mainitr );
}

void VCNNode::RemoveAllComponents()
{
	VCN_ASSERT( !m_LockedForUpdate && "Mass Component Removal not supported during update. Use EventManager::InvokeLater instead");

	m_Components.clear();
	m_UpdatableComponents.clear();
	m_DrawableComponents.clear();
}


void VCNNode::InitializeAllComponentsRecursive()
{
	std::for_each( std::begin(m_Components), std::end(m_Components), [](VCNNode::ComponentPair pair)
	{
		boost::property_tree::ptree emptyParams;	
		pair.second->Initialise(emptyParams);
	});

	VCNNodeCore* nodeCore = VCNNodeCore::GetInstance();
	for (ChildrenList::const_iterator currChild = mChildren.begin(), end = mChildren.end(); currChild != end; ++currChild)
	{
		// Get the child and render him
		VCNNode* childNode = nodeCore->GetNode( *currChild );
		if( childNode )
		{
			childNode->InitializeAllComponentsRecursive();
		}
	}
}
