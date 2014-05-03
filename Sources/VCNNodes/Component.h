///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#pragma once

#include <VCNUtils/Types.h>
#include "VCNUtils/Assert.h"

#include <string>
#include <memory>
#include <type_traits>

#include <boost/property_tree/ptree.hpp>

class VCNNode;

struct VCNIUpdatable
{
	virtual ~VCNIUpdatable();
	virtual void Update(float dt) = 0;
};

struct VCNIDrawable
{
	virtual ~VCNIDrawable();
	virtual void Draw() const = 0;
};

class VCNIComponent
{
public:
	typedef std::size_t BehaviorFlags;

	enum Behavior
	{
		BHV_NONE = 0,
		BHV_UPDATE,
		BHV_DRAW
	};

	struct OwnerData
	{
		VCNNode* Old;
		VCNNode* New;
	};

	typedef boost::property_tree::ptree Parameters;
	typedef const std::pair<const std::string, Parameters >& ParameterItr;
	typedef std::shared_ptr<VCNIComponent> Ptr;
	typedef unsigned int Type;

	virtual ~VCNIComponent();

	void SetOwner(VCNNode* value);

	virtual VCNNode* GetOwner() const
	{
		return m_Owner;
	}

	//function to let concrete classes know their owner is set
	virtual void OnOwnerSet(const OwnerData& data);

	/*
	Abstract interface that needs implementing
	*/
	virtual void Initialise(const Parameters& params) = 0;
	virtual Type GetType() const = 0;
	virtual const std::string& GetName() const = 0;
	virtual BehaviorFlags GetBehaviorFlags() const = 0;

	virtual void OnPersonaApplied();

	template<typename T>
	static BehaviorFlags GetFlagsForType();

	bool IsUpdatable() const;
	bool IsDrawable() const;

	VCNIUpdatable* ToUpdatable();
	const VCNIDrawable* ToDrawable() const;

	/*
	Copy method only necessary for node trees as resources
	for example Mesh hierarchies.
	*/
	virtual Ptr Copy() const;
	

	// This method was added to Vicuna to make the addition of the Level editor support in 3ds Max easier. 
	// You will then be able to read the xml and assume that all the values are strings. This method will make the conversion by itself
	// It should never be called during execution but only when loading a level!!!!!!
	// Usage:
	//		VCNString val = "3";
	//		BaseComponent* c = ...;
	//		c->SetAttribute("AttributeName", val);
	//		// Here the attribute may be an int but the component will be responsible to make the conversion
	// 
	// The idea is that the component classes will know the correct type of the attribute associated with every attribute name
	virtual bool SetAttribute(const VCNString& attributeName, const VCNString& attributeValue)
	{
		VCN_ASSERT_FAIL( "You should not build custom attributes on BaseComponent or unimplemented Component." );
		return false;
	}

private:
	//used to make sure no one inherits directly from us _except_ ComponentBase
	template<typename T>
	friend class VCNComponentBase;

	VCNIComponent();

	VCNNode* m_Owner;
};


template<typename T>
VCNIComponent::BehaviorFlags VCNIComponent::GetFlagsForType()
{
	VCNIComponent::BehaviorFlags flags = BHV_NONE;

	if ( std::is_base_of<VCNIUpdatable, T>::value )
	{
		flags |= BHV_UPDATE;
	}

	if ( std::is_base_of<VCNIDrawable, T>::value )
	{
		flags |= BHV_DRAW;
	}

	return flags;
}

