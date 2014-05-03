///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
/// 
/// @brief Node base class.
///

#ifndef VCNNODE_H
#define VCNNODE_H

#pragma once

#include "Component.h"
#include "EventManager.h"

#include "VCNCore/Atom.h"
#include "VCNUtils/Matrix.h"
#include "VCNUtils/Quaternion.h"

///
/// Node base class, managed by the node manger VCNNodeCore.
///
class VCNNode : public VCNAtom
{
	VCN_CLASS;

protected:

	// Only the core can create and destroy nodes
	friend class VCNNodeCore;
	friend class VCNNodePool;
	VCNNode( VCNNodeID nodeID );
	virtual ~VCNNode();

public:

	typedef std::pair<VCNIComponent::Type, VCNIComponent::Ptr> ComponentPair;
	typedef std::unordered_map<ComponentPair::first_type, ComponentPair::second_type> ComponentMap;

	/// ID accessor
	const VCNNodeID GetNodeID() const;

	/// Gets/Sets tag name (used to categorize)
	const VCNString& GetTag() const;
	void SetTag( const VCNString& tag );

	/// Parent accessors
	void SetParent( VCNNodeID parentID );
	VCNNodeID GetParent() const;

	VCNNode* GetAncestor();
	const VCNNode* GetAncestor() const { return const_cast<VCNNode*>(this)->GetAncestor(); }

	/// Set/Get selectable state
	void SetSelectable(VCNBool val) { mSelectable = val; }
	VCNBool IsSelectable() const { return mSelectable && IsActive(); }

	/// Set/Get is selected state
	void SetSelected(VCNBool val) { mIsSelected = val; }
	VCNBool IsSelected() const { return mIsSelected && IsSelectable(); }

	/// Set/Get saveable state
	void SetSaveable(VCNBool val) { mSaveable = val; }
	VCNBool IsSaveable() const { return mSaveable; }

	void SetActive(VCNBool val) { mActive = val; }
	VCNBool IsActive() const { return mActive; }

	void SetUseRelativeTransform(VCNBool b) { mRelativeTransform = b; }
	VCNBool UseRelativeTransform() const { mRelativeTransform; }

	/// Set the node to default values
	virtual void Reset();

	/// Tick that thing
	virtual void Process(const float elapsedTime);
	
	/// Render that thing!
	virtual void Render() const;

	/// Copy this node and return the copy
	virtual VCNNode* Copy() const;

	// Attach/Detach child nodes
	void     AttachChild( VCNNodeID childID, const VCNBool insertAtHead = false);
	void     RemoveChild( VCNNodeID childID );
	VCNUInt  GetChildCount() const;
	void     GetChildrenArray( std::vector<VCNNodeID>& children ) const;
	VCNNode* GetChildNode(unsigned int index) const;

	// Set the transformations at this node
	void SetRotation(const VCNQuat& rotation);
	void SetTranslation(const Vector3& translation);
	void SetTranslation(VCNFloat x, VCNFloat y, VCNFloat z);
	void SetScale(const Vector3& scale);
	void SetScale(VCNFloat sx, VCNFloat sy, VCNFloat sz);
	void SetScale(VCNFloat uniformScale);
	void ResetTransforms();

	// Get the local transforms
	const Vector3& GetLocalTranslation() const { return mLocalTranslation; }
	const VCNQuat& GetLocalRotation() const { return mLocalRotation; }
	const Vector3& GetLocalScale() const { return mLocalScale; }
	
	VCNBool HasTransformChanged() const { return mComputeTransforms; }

	// Get the world transforms
	Matrix4  GetWorldTransformation() const;
	Vector3  GetWorldTranslation() const;
	Matrix4  GetWorldRotation() const;
	Vector3  GetWorldScale() const;
	VCNFloat GetWorldUniformScale() const;

public: // Components

	/// Adds a new component to the entity model
	VCNBool HasComponent(VCNIComponent::Type type) const;
	VCNIComponent::Ptr GetComponent(VCNIComponent::Type type) const;
	VCNIComponent::Ptr GetComponent(VCNIComponent::Type type);
	const ComponentMap& GetComponents() { return m_Components; }
	template<typename T> T* GetComponent();
	template<typename T> VCNBool HasComponent() const;
	template<typename T> const T* GetComponent() const;
	template<typename T> void RemoveComponent();
	void AddComponent(VCNIComponent* component);
	void AddComponent(const VCNIComponent::Ptr& component);
	void RemoveAllComponents();
	void RemoveComponent(VCNIComponent::Type type);

	void InitializeAllComponentsRecursive();

public: // Events

	VCNEventManager& GetEvents() { return *m_Events.get(); } //is never null

	//Fires a specific event to all listeners
	template<typename EventType> void Fire(const EventType& evt) const { m_Events->Fire(evt); }

	//Fires a specific event to all listeners and continues down
	//the parent child tree to all other EventManagers
	template<typename EventType> void FireDown(const EventType& evt) const;

protected:

	// Copy fields to specified node.
	void CopyTo(VCNNode* node) const;

	/// Updates the world transformation based on parent transform.
	/// Return true if the derived class also needs to update transforms.
	virtual const bool UpdateWorldTransform();

	/// Computes the node world matrix and returns it.
	virtual Matrix4 ComputeWorldTransform() const;

// Data members

	// Make things easier on ourselves
	typedef std::vector<VCNNodeID> ChildrenList;

	ComponentMap m_Components;
	std::vector<VCNIComponent::Type> m_ToRemove;
	std::vector<VCNIUpdatable*> m_UpdatableComponents;
	std::vector<const VCNIDrawable*> m_DrawableComponents;

	VCNEventManager::Handle m_Events;

	// Identification of this node
	VCNNodeID mNodeID;

	// The parent node
	VCNNodeID mParentID;

	// All the children of this node
	ChildrenList mChildren;

	// Helper string
	VCNString mTag;

	// The local transformations of this node
	VCNQuat mLocalRotation;
	Vector3 mLocalTranslation;
	Vector3 mLocalScale;

	// The resulting world transformation of this node
	Matrix4 mWorld;
	VCNBool mComputeTransforms;

	// Node states
	VCNBool mSelectable        : 1;
	VCNBool mSaveable          : 1;
	VCNBool mIsSelected        : 1;
	VCNBool mActive            : 1;
	VCNBool mRelativeTransform : 1;
	VCNBool m_LockedForUpdate  : 1;
};


///////////////////////////////////////////////////////////////////////
inline const VCNNodeID VCNNode::GetNodeID() const
{
	return mNodeID;
}

///////////////////////////////////////////////////////////////////////
inline void  VCNNode::SetTag( const VCNString& tag )
{
	mTag = tag;
}

///////////////////////////////////////////////////////////////////////
inline const VCNString& VCNNode::GetTag() const
{
	return mTag;
}

///////////////////////////////////////////////////////////////////////
inline void VCNNode::SetParent( VCNNodeID parentID )
{
	mParentID = parentID;
	mComputeTransforms = true;
}

///////////////////////////////////////////////////////////////////////
inline VCNNodeID VCNNode::GetParent() const
{
	return mParentID;
}

///////////////////////////////////////////////////////////////////////
inline VCNUInt VCNNode::GetChildCount() const
{
	return (VCNUInt)mChildren.size();
}

///////////////////////////////////////////////////////////////////////
inline void VCNNode::SetRotation(const VCNQuat& rotation)
{
	mLocalRotation = rotation;
	mComputeTransforms = true;
}

///////////////////////////////////////////////////////////////////////
inline void VCNNode::SetTranslation(const Vector3& translation)
{
	if ( !mLocalTranslation.Equals(translation) )
	{
		mLocalTranslation = translation;
		mComputeTransforms = true;
	}
}



///////////////////////////////////////////////////////////////////////
inline void VCNNode::SetTranslation(VCNFloat x, VCNFloat y, VCNFloat z)
{
	SetTranslation( Vector3(x, y, z) );
}



///////////////////////////////////////////////////////////////////////
inline void VCNNode::SetScale(const Vector3& scale)
{
	mLocalScale = scale;
	mComputeTransforms = true;
}



///////////////////////////////////////////////////////////////////////
inline void VCNNode::SetScale(VCNFloat sx, VCNFloat sy, VCNFloat sz)
{
	mLocalScale.Set(sx, sy, sz);
	mComputeTransforms = true;
}


///////////////////////////////////////////////////////////////////////
inline void VCNNode::SetScale(VCNFloat uniformScale)
{
	SetScale(uniformScale, uniformScale, uniformScale);
}


///////////////////////////////////////////////////////////////////////
inline void VCNNode::ResetTransforms()
{
	mWorld.SetIdentity();
	mLocalRotation.SetIdentity();
	mLocalTranslation = Vector3();
	mLocalScale.Set( 1.0f, 1.0f, 1.0f );
	mComputeTransforms = true;
}

///////////////////////////////////////////////////////////////////////
inline Matrix4 VCNNode::GetWorldTransformation() const
{
	if ( mComputeTransforms )
	{
		return ComputeWorldTransform();
	}

	return mWorld;
}

template<typename EventType>
inline void VCNNode::FireDown(const EventType& evt) const
{
	m_Events->Fire(evt);

	size_t count = GetChildCount();
	for(size_t i = 0; i < count; ++i)
	{
		VCNNode* child = GetChildNode(i);
		child->FireDown(evt);
	}
}

template<typename T>
inline void VCNNode::RemoveComponent()
{
	this->RemoveComponent(T::TYPE);
}

template<typename T>
inline const T* VCNNode::GetComponent() const
{
	//make sure no one does any weird mumbo jumbo with the inheritance tree
	//the static cast could fail if people try and get D3DRenderComponent and OpenGLRenderComponent
	//both inheriting from the same VCNRenderComponent... but who would do that?!
	VCN_ASSERT( this->GetComponent(T::TYPE).get() == nullptr || dynamic_cast<T*>(this->GetComponent(T::TYPE).get()) != nullptr );
	return static_cast<T*>(this->GetComponent(T::TYPE).get());
}

template<typename T>
inline T* VCNNode::GetComponent()
{
	//make sure no one does any weird mumbo jumbo with the inheritance tree
	//the static cast could fail if people try and get D3DRenderComponent and OpenGLRenderComponent
	//both inheriting from the same VCNRenderComponent... but who would do that?!
	VCN_ASSERT( this->GetComponent(T::TYPE).get() == nullptr || dynamic_cast<T*>(this->GetComponent(T::TYPE).get()) != nullptr );
	return static_cast<T*>(this->GetComponent(T::TYPE).get());
}

template<typename T>
inline VCNBool VCNNode::HasComponent() const
{
	return this->HasComponent(T::TYPE);
}

#endif // VCNNODE_H
