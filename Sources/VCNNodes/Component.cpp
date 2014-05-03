///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#include "Precompiled.h"
#include "Component.h"

#include <VCNUtils/Assert.h>
#include <VCNUtils/Utilities.h>

VCNIUpdatable::~VCNIUpdatable()
{}

VCNIDrawable::~VCNIDrawable()
{}

VCNIComponent::VCNIComponent()
	: m_Owner(nullptr)
{}


VCNIComponent::~VCNIComponent()
{}

VCNIComponent::Ptr VCNIComponent::Copy() const
{
	/*
	If this assert pops, make sure it makes sens to copy this component, if so, override the copy function.
	It is important to know why said component is being copyied, (eg. it is found in a resource object), if
	it doesn't make sens, this could be the sign of a more important bug.
	*/
	VCN_ASSERT_MSG(false, "Component of type %s is being copyied but doesn't support it", GetName().c_str());
	return Ptr(nullptr);
}

void VCNIComponent::SetOwner(VCNNode* value)
{
	OwnerData data;
	data.New = value;
	data.Old = m_Owner;

	m_Owner = value;

	OnOwnerSet( data );
}

void VCNIComponent::OnPersonaApplied()
{}

void VCNIComponent::OnOwnerSet(const OwnerData& data)
{
	//do nothing by default
}

bool VCNIComponent::IsUpdatable() const
{
	return VCN::HasFlag( GetBehaviorFlags(), BHV_UPDATE );
}

bool VCNIComponent::IsDrawable() const
{
	return VCN::HasFlag( GetBehaviorFlags(), BHV_DRAW );
}

VCNIUpdatable* VCNIComponent::ToUpdatable()
{
	VCN_ASSERT(IsUpdatable());
	return dynamic_cast<VCNIUpdatable*>(this);
}

const VCNIDrawable* VCNIComponent::ToDrawable() const
{
	VCN_ASSERT(IsDrawable());
	return dynamic_cast<const VCNIDrawable*>(this);;
}